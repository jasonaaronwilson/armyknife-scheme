#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "pair.h"
#include "tagged-reference.h"

// This is not the most efficient calling convention. It might be
// smarter to pass the first 6 to 8 args as explicit arguments (as
// this will cover the vast majority of primitives without resorting
// to linked lists).

// primitive_t is the new type name being defined.
typedef tagged_reference_t (*primitive_t)(pair_t* args);

extern tagged_reference_t primtive_function_plus(pair_t* args);

static inline primitive_t untag_primitive(tagged_reference_t reference) {
  require_tag(reference, TAG_PRIMITIVE);
  return (primitive_t) reference.data;
}

#endif /* _PRIMITIVE_H_ */