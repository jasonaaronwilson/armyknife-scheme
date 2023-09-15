/**
 * @file closure.c
 */

// ======================================================================
// This is block is extraced to closure.h
// ======================================================================

#ifndef _CLOSURE_H_
#define _CLOSURE_H_

#include "environment.h"
#include "tagged-reference.h"

typedef struct {
  tagged_reference_t code;
  environment_t* env;
  char* debug_name;
  uint64_t n_arg_names;
  char* arg_names[0];
} closure_t;

extern closure_t* allocate_closure(uint64_t n_arg_names);

// extern char* closure_get_debug_name(closure_t* closure);

static inline closure_t* untag_closure_t(tagged_reference_t closure) {
  require_tag(closure, TAG_CLOSURE_T);
  return (closure_t*) closure.data;
}

#endif /* _CLOSURE_H_ */

// ======================================================================

#include "closure.h"
#include "allocate.h"

/**
 * Allocate the space for a closure accepting at most "N"
 * arguments. The caller must fill in the values of all fields in the
 * closure (see evaluator.c).
 */
closure_t* allocate_closure(uint64_t n_arg_names) {
  return (closure_t*) malloc_bytes(sizeof(closure_t)
                                   + n_arg_names * sizeof(char*));
}
