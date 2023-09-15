/**
 * @file tagged-pair.c
 *
 * tagged pairs are used to build up lists or trees of elements. These
 * are primarily utilized by the tokenizer, reader and the repl
 * interpreter where the flexibility of dynamic typing outweighs any
 * performance concerns.
 */

// ======================================================================
// This is block is extraced to allocate.h
// ======================================================================

#ifndef _PAIR_H_
#define _PAIR_H_

#include "optional.h"
#include "tagged-reference.h"

typedef struct {
  tagged_reference_t head;
  tagged_reference_t tail;
} pair_t;

extern pair_t* make_pair(tagged_reference_t head, tagged_reference_t tail);

extern uint64_t pair_list_length(pair_t* head);
extern tagged_reference_t pair_list_get(pair_t* head, uint64_t index);
extern void pair_list_set(pair_t* head, uint64_t index,
                          tagged_reference_t element);
extern pair_t* pair_list_append(pair_t* lst1, pair_t* lst2);
extern optional_t pair_assoc_list_lookup(pair_t* lst, char* name);

static inline pair_t* untag_pair(tagged_reference_t reference) {
  require_tag(reference, TAG_PAIR_T);
  return (pair_t*) reference.data;
}

// Pairs are used so much in code implementing the scheme interpreter
// that we provide some convenient utilities to avoid seeing lots of
// untags and such.

extern tagged_reference_t cons(tagged_reference_t head,
                               tagged_reference_t tail);
extern tagged_reference_t car(tagged_reference_t pair);
extern tagged_reference_t cdr(tagged_reference_t pair);

#endif /* _TAGGED_PAIR_H_ */

// ======================================================================

#include <stdint.h>

#include "allocate.h"
#include "fatal-error.h"
#include "optional.h"
#include "pair.h"
#include "string-util.h"

pair_t* make_pair(tagged_reference_t head, tagged_reference_t tail) {
  pair_t* result = malloc_struct(pair_t);
  result->head = head;
  result->tail = tail;
  return result;
}

/**
 * The same as make_pair except the result is a tagged_reference_t
 * which is often more convenient.
 */
tagged_reference_t cons(tagged_reference_t head, tagged_reference_t tail) {
  return tagged_reference(TAG_PAIR_T, make_pair(head, tail));
}

tagged_reference_t car(tagged_reference_t pair) {
  return untag_pair(pair)->head;
}

tagged_reference_t cdr(tagged_reference_t pair) {
  return untag_pair(pair)->tail;
}

uint64_t pair_list_length(pair_t* lst) {
  uint64_t length = 0;
  while (lst) {
    lst = (pair_t*) (lst->tail.data);
    length++;
  }
  return length;
}

tagged_reference_t pair_list_get(pair_t* lst, uint64_t index) {
  uint64_t length = 0;
  while (lst) {
    if (length == index) {
      return lst->head;
    }
    lst = (pair_t*) (lst->tail.data);
    length++;
  }
  fatal_error(ERROR_ILLEGAL_LIST_INDEX);
}

void pair_list_set(pair_t* head, uint64_t index, tagged_reference_t element) {
  uint64_t length = 0;
  while (head) {
    if (length == index) {
      head->head = element;
      return;
    }
    length++;
  }
  fatal_error(ERROR_ILLEGAL_LIST_INDEX);
}

pair_t* pair_list_append(pair_t* lst_1, pair_t* lst_2) {
  if (lst_1 && lst_2) {
    pair_t* head = lst_1;
    while (head->tail.data) {
      head = (pair_t*) head->tail.data;
    }
    head->tail = (tagged_reference_t){(uint64_t) lst_2, TAG_PAIR_T};
    return lst_1;
  } else if (lst_1) {
    return lst_1;
  } else {
    return lst_2;
  }
}

optional_t pair_assoc_list_lookup(pair_t* lst, char* name) {
  while (lst) {
    tagged_reference_t element = lst->head;
    pair_t* binding = untag_pair(element);

    if (string_equal(name, untag_string_or_reader_symbol(binding->head))) {
      return optional_of(binding->tail);
    }
    lst = ((pair_t*) lst->tail.data);
  }

  return optional_empty();
}

// TODO(jawilson): tagged_pair_alist_find, tagged_pair_alist_insert,
// tagged_pair_alist_remove
