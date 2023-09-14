/**
 * @file evaluator.c
 *
 * The file contains an interpreter for a very tiny subset of scheme
 * which is used to implement the debugger repl as well as expressions
 * in the assembler. See reader.c for a parser that reads character
 * oriented text and converts it to the format suitable for expr (aka
 * linked lists built out of pairs plus various "atoms").
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocate.h"
#include "closure.h"
#include "evaluator.h"
#include "fatal-error.h"
#include "optional.h"
#include "pair.h"
#include "primitive.h"
#include "scheme-symbol.h"
#include "string-util.h"

#define TAIL_CALL return

// See the symbol-hash command line tool in this directory if you need
// to generate more. Example make symbol-hash && ./symbol-hash 'if'
// 'set!' 'quote' 'lambda'.

#define HASHCODE_DEFINE UINT64_C(1242841461589453162)
#define HASHCODE_IF UINT64_C(12687957717205024595)
#define HASHCODE_LAMBDA UINT64_C(11364329973434366565)
#define HASHCODE_QUOTE UINT64_C(10597478766694597373)
#define HASHCODE_SET_BANG UINT64_C(8292903574644452355)

// These must have the same signature as eval() to have a chance of
// doing tail recursion.

tagged_reference_t eval_if_expression(environment_t* env,
                                      tagged_reference_t expr,
                                      boolean_t in_tail_position);
tagged_reference_t eval_assignment(environment_t* env, tagged_reference_t expr,
                                   boolean_t in_tail_position);
tagged_reference_t eval_application(environment_t* env, tagged_reference_t expr,
                                    boolean_t in_tail_position);
tagged_reference_t eval_lambda(environment_t* env, tagged_reference_t expr,
                               boolean_t in_tail_position);

/**
 * This is the entry point to the evaluator. Dvaluate the given
 * expression and return a tagged_reference_t to the result of
 * interpreting it.
 */
tagged_reference_t eval(environment_t* env, tagged_reference_t expr,
                        boolean_t in_tail_position) {

  fprintf(stderr, "in_tail_position = %d\n", in_tail_position);

  // Handle self-evaluating values and variable lookups
  switch (expr.tag) {
  case TAG_NULL:
  case TAG_STRING:
  case TAG_UINT64_T:
  case TAG_ERROR_T:
    if (in_tail_position && !env->is_captured) {
      free_bytes(env);
    }
    return expr;

  case TAG_SCHEME_SYMBOL:
    if (1) {
      optional_t result = environment_get(env, (char*) expr.data);
      if (!optional_is_present(result)) {
        fatal_error(ERROR_VARIABLE_NOT_FOUND);
      }
      if (in_tail_position && !env->is_captured) {
        free_bytes(env);
      }
      return optional_value(result);
    }
  }

  pair_t* lst = untag_pair(expr);

  if (pair_list_length(lst) == 0) {
    if (in_tail_position && !env->is_captured) {
      free_bytes(env);
    }
    return (tagged_reference_t){ERROR_CANT_EVAL_EMPTY_EXPRESSION, TAG_ERROR_T};
  }

  tagged_reference_t first = pair_list_get(lst, 0);
  if (first.tag == TAG_SCHEME_SYMBOL) {
    char* symbol_name = untag_reader_symbol(first);
    // We probably don't need all 64 bits and with so few special
    // forms, just doing a chain of sring_equal() calls may be faster
    // or nearly as fast though there is probably a break even point
    // where this is much faster...
    uint64_t hashcode = string_hash(symbol_name);
    switch (hashcode) {
    case HASHCODE_IF:
      if (!string_equal(symbol_name, "if")) {
        break;
      }
      TAIL_CALL eval_if_expression(env, expr, in_tail_position);
      break;

    case HASHCODE_SET_BANG:
      if (!string_equal(symbol_name, "set!")) {
        break;
      }
      TAIL_CALL eval_assignment(env, expr, in_tail_position);

    case HASHCODE_QUOTE:
      if (in_tail_position && !env->is_captured) {
        free_bytes(env);
      }
      return pair_list_get(lst, 1);

    case HASHCODE_LAMBDA:
      TAIL_CALL eval_lambda(env, expr, in_tail_position);

    case HASHCODE_DEFINE:
      if (1) {
        tagged_reference_t name = pair_list_get(lst, 1);
        tagged_reference_t value = eval(env, pair_list_get(lst, 2), false);
        environment_define(env, untag_reader_symbol(name), value);
        if (in_tail_position && !env->is_captured) {
          free_bytes(env);
        }
        return NIL;
      }
    }
  }

  TAIL_CALL eval_application(env, expr, in_tail_position);
}

tagged_reference_t eval_if_expression(environment_t* env,
                                      tagged_reference_t expr,
                                      boolean_t in_tail_position) {
  pair_t* lst = untag_pair(expr);
  tagged_reference_t test_expr = pair_list_get(lst, 1);
  tagged_reference_t consequent_expr = pair_list_get(lst, 2);
  tagged_reference_t alternative_expr = NIL;
  if (pair_list_length(lst) >= 3) {
    alternative_expr = pair_list_get(lst, 3);
  }
  tagged_reference_t evaluated_expr = eval(env, test_expr, false);
  if (is_false(evaluated_expr)) {
    TAIL_CALL eval(env, alternative_expr, in_tail_position);
  } else {
    TAIL_CALL eval(env, consequent_expr, in_tail_position);
  }
}

// I guess technically this doesn't need have the same signature?
tagged_reference_t eval_assignment(environment_t* env, tagged_reference_t expr,
                                   boolean_t in_tail_position) {
  // TODO(jawilson): do the assignment!
  if (in_tail_position && !env->is_captured) {
    free_bytes(env);
  }
  return NIL;
}

/**
 * Evaluate and application, i.e., a function call.
 */
tagged_reference_t eval_application(environment_t* env, tagged_reference_t expr,
                                    boolean_t in_tail_position) {
  primitive_arguments_t arguments = {.n_args = 0};

  // The above should be sufficient but just clear the entire
  // structure while we are still in early development.
  memset(&arguments, 0, sizeof(arguments));

  // perform an "application" (aka, function call to a primitive or
  // closure).

  pair_t* lst = untag_pair(expr);

  tagged_reference_t fn = eval(env, pair_list_get(lst, 0), false);

  for (int i = 1; (i < pair_list_length(lst)); i++) {
    if (i >= MAX_PRIMITIVE_ARGS) {
      fatal_error(ERROR_MAX_PRIMITIVE_ARGS);
    }
    tagged_reference_t arg_expr = pair_list_get(lst, i);
    arguments.args[arguments.n_args++] = eval(env, arg_expr, false);
  }

  if (in_tail_position && !env->is_captured) {
    free_bytes(env);
    env = 0;
  }

  if (fn.tag == TAG_PRIMITIVE) {
    primitive_t primitive = untag_primitive(fn);
    return primitive(arguments);
  }

  closure_t* closure = untag_closure_t(fn);
  env = make_environment(closure->env);
  // make sure number of args are compatible.
  for (int i = 0; (i < closure->n_arg_names); i++) {
    environment_define(env, closure->arg_names[i], arguments.args[i]);
  }

  pair_t* sequence = untag_pair(closure->code);
  while (sequence->tail.tag != TAG_NULL) {
    eval(env, sequence->head, false);
    sequence = untag_pair(sequence->tail);
  }

  TAIL_CALL eval(env, sequence->head, in_tail_position);
}

/**
 * Make a closure
 */
tagged_reference_t eval_lambda(environment_t* env, tagged_reference_t expr,
                               boolean_t in_tail_position) {
  pair_t* argument_list = untag_pair(pair_list_get(untag_pair(expr), 1));
  uint64_t n_args = pair_list_length(argument_list);
  closure_t* closure = allocate_closure(n_args);
  closure->code = cdr(cdr(expr));
  closure->env = env;
  closure->debug_name = NULL;
  closure->n_arg_names = n_args;
  for (int i = 0; i < n_args; i++) {
    closure->arg_names[i]
        = untag_scheme_symbol(pair_list_get(argument_list, i));
  }

  // Once we close over an environment we need a garbage collector to
  // reclaim it (and don't need to free it here even if we are
  // in_tail_position).
  environment_capture(env);

  return tagged_reference(TAG_CLOSURE_T, closure);
}
