/**
 * @file environment.c
 */

// ======================================================================
// This is block is extraced to environment.h
// ======================================================================

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "boolean.h"
#include "byte-array.h"
#include "pair.h"
#include "printer.h"

typedef struct environment_S {
  // This is a standard way to handle lexically scoped variables.
  struct environment_S* parent;

  // This allows environments to be freed "early" during expression
  // evaluation without waiting for the garbage collector.
  boolean_t is_captured;

  // The global environment uses more buckets than a child environment
  int n_buckets;

  tagged_reference_t buckets[0]; // NIL or TAG_PAIR_T (association list)
} environment_t;

extern environment_t* make_environment(environment_t* parent);
extern optional_t environment_get(environment_t* env, char* var_name);
extern void environment_set(environment_t* env, char* var_name,
                            tagged_reference_t value);
extern void environment_define(environment_t* env, char* var_name,
                               tagged_reference_t value);
extern byte_array_t* print_environment(environment_t* env);

static inline void environment_capture(environment_t* env) {
  env->is_captured = true;
}

#endif /* _ENVIRONMENT_H_ */

// ======================================================================

#include <stdio.h>
#include <stdlib.h>

#include "allocate.h"
#include "boolean.h"
#include "closure.h"
#include "environment.h"
#include "optional.h"
#include "pair.h"
#include "string-util.h"
#include "tagged-reference.h"

// #define GLOBAL_ENVIRONMENT_BUCKETS 73
#define GLOBAL_ENVIRONMENT_BUCKETS 1
#define NESTED_ENVIRONMENT_BUCKETS 1

/**
 * Make an empty environment with the given parent. When parent is
 * NULL, the environment is optimized for lookup performance over
 * space.
 */
environment_t* make_environment(environment_t* parent) {
  // The global environment gets more buckets since it will have way
  // more symbols than a child environment. We can also avoid hashing
  // when n_buckets == 1.
  int n_buckets = (parent == NULL) ? GLOBAL_ENVIRONMENT_BUCKETS
                                   : NESTED_ENVIRONMENT_BUCKETS;
  environment_t* result = (environment_t*) malloc_bytes(
      sizeof(environment_t) + n_buckets * sizeof(tagged_reference_t));
  result->parent = parent;
  result->n_buckets = n_buckets;

  return result;
}

pair_t* environment_find_binding(environment_t* env, char* var_name) {
  if (env == NULL) {
    return NULL;
  }

  tagged_reference_t lst = NIL;
  if (env->n_buckets > 1) {
    uint64_t hash_code = string_hash(var_name);
    uint64_t bucket_number = hash_code % env->n_buckets;
    lst = env->buckets[bucket_number];
  } else {
    lst = env->buckets[0];
  }

  if (lst.tag == TAG_NULL) {
    return environment_find_binding(env->parent, var_name);
  }

  pair_t* binding = pair_assoc_list_find_binding(untag_pair(lst), var_name);

  if (binding != NULL) {
    return binding;
  }

  return environment_find_binding(env->parent, var_name);
}

/**
 * Perform a lookup in the environment returning optional_empty() when
 * the var_name isn't found and otherwise the tagged_reference_t which
 * is bound to var_name (which can be NIL, etc.).
 */
optional_t environment_get(environment_t* env, char* var_name) {
  if (!env) {
    return optional_empty();
  }

  pair_t* binding = environment_find_binding(env, var_name);
  if (binding == NULL) {
    return optional_empty();
  }

  return optional_of(binding->tail);
}

void environment_set(environment_t* env, char* var_name,
                     tagged_reference_t value) {

  if (!env) {
    fatal_error(ERROR_NULL_ENVIRONMENT);
  }

  pair_t* binding = environment_find_binding(env, var_name);
  if (binding == NULL) {
    fatal_error(ERROR_VARIABLE_NOT_FOUND);
  }
  binding->tail = value;
}

/**
 * Define a new variable in this environment (possibly shadowing an
 * older binding).
 */
void environment_define(environment_t* env, char* var_name,
                        tagged_reference_t value) {
  if (env == NULL) {
    fatal_error(ERROR_NULL_ENVIRONMENT);
  }

  /*
  // Total hack to put a debug name on closures defined at the
  // top-level. This is completely optional behavior as the debug name
  // is not directly accessible.
  if (env->parent == NULL) {
    if (value.tag == TAG_CLOSURE_T) {
      closure_t* closure = untag_closure_t(value);
      if (closure->debug_name == NULL) {
        closure->debug_name = var_name;
      }
    }
  }
  */

  pair_t* binding = environment_find_binding(env, var_name);
  if (binding != NULL) {
    binding->tail = value;
    byte_array_t* env_as_string = print_environment(env);
  } else {
    uint64_t hash_code = string_hash(var_name);
    uint64_t bucket_number = hash_code % env->n_buckets;
    tagged_reference_t new_binding
        = cons(tagged_reference(TAG_SCHEME_SYMBOL, var_name), value);
    tagged_reference_t new_binding_list
        = cons(new_binding, env->buckets[bucket_number]);
    env->buckets[bucket_number] = new_binding_list;

    // We should immediately find what we just defined...
    pair_t* b = environment_find_binding(env, var_name);
    if (b == NULL) {
      fatal_error(ERROR_UKNOWN);
    }
  }
}

byte_array_t* print_environment(environment_t* env) {
  byte_array_t* output = make_byte_array(1024);

  if (env == NULL) {
    output = byte_array_append_string(output, "NULL");
    output = byte_array_append_byte(output, '\0');
    return output;
  }

  for (int i = 0; (i < env->n_buckets); i++) {
    output = byte_array_append_string(output, "BUCKET - ");
    tagged_reference_t ref = env->buckets[i];
    output = print_tagged_reference_to_byte_arary(output, ref);
    output = byte_array_append_string(output, "\n");
  }
  output = byte_array_append_byte(output, '\0');

  return output;
}
