/**
 * @file printer.c
 *
 * This file contains all kinds of routines print stuff. Some is for
 * the scheme interpreter only so this file probaly makes sense to
 * split up.
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "byte-array.h"
#include "pair.h"
#include "printer.h"
#include "string-util.h"
#include "tagged-reference.h"

byte_array_t*
    print_tagged_reference_to_byte_arary(byte_array_t* destination,
                                         tagged_reference_t reference) {
  char* prefix = NULL;
  char* str = NULL;
  char* suffix = NULL;
  char buffer[64];

  switch (reference.tag) {
  case TAG_NULL:
    str = "()";
    break;

  case TAG_PAIR_T:
    // Use dot notation (which we can't even parse ourselves)
    destination = byte_array_append_byte(destination, '(');
    destination = print_tagged_reference_to_byte_arary(
        destination, untag_pair(reference)->head);
    destination = byte_array_append_byte(destination, ' ');
    destination = byte_array_append_byte(destination, '.');
    destination = byte_array_append_byte(destination, ' ');
    destination = print_tagged_reference_to_byte_arary(
        destination, untag_pair(reference)->tail);
    destination = byte_array_append_byte(destination, ')');
    break;

  case TAG_STRING:
    prefix = "\"";
    str = untag_string(reference);
    suffix = "\"";
    break;

  case TAG_SCHEME_SYMBOL:
    str = untag_reader_symbol(reference);
    break;

  case TAG_UINT64_T:
    snprintf(buffer, sizeof(buffer), "%lu", untag_uint64_t(reference));
    str = &buffer[0];
    break;

  case TAG_ERROR_T:
    prefix = "#<error-code-";
    snprintf(buffer, sizeof(buffer), "%lu", reference.data);
    str = &buffer[0];
    suffix = ">";
    break;

  case TAG_BOOLEAN_T:
    if (is_false(reference)) {
      str = "#f";
    }
    if (is_true(reference)) {
      str = "#t";
    } else {
      str = "#<illegal-boolean-value>";
    }
    break;

  case TAG_PRIMITIVE:
    str = "#<primitive-procedure>";
    break;

  case TAG_CLOSURE_T:
    str = "#<closure>";
    break;

  case TAG_CPU_THREAD_STATE_T:
    str = "#<thread-state>";
    break;
  }

  if (prefix) {
    destination = byte_array_append_bytes(destination, (uint8_t*) prefix,
                                          strlen(prefix));
  }
  if (str) {
    destination
        = byte_array_append_bytes(destination, (uint8_t*) str, strlen(str));
  }
  if (suffix) {
    destination = byte_array_append_bytes(destination, (uint8_t*) suffix,
                                          strlen(suffix));
  }

  return destination;
}
