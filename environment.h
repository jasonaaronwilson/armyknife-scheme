#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "boolean.h"
#include "pair.h"

typedef struct environment_S {
  struct environment_S* parent;
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

#endif /* _ENVIRONMENT_H_ */
