#ifndef _FATAL_ERROR_
#define _FATAL_ERROR_

typedef enum {
  ERROR_UKNOWN,
  ERROR_MEMORY_ALLOCATION,
  ERROR_ARRAY_ACCESS_OUT_OF_BOUNDS,
  ERROR_ILLEGAL_LIST_INDEX,
  ERROR_CANT_EVAL_EMPTY_EXPRESSION,
  ERROR_VARIABLE_NOT_FOUND,
  ERROR_REFERENCE_NOT_EXPECTED_TYPE,
  ERROR_NOT_REACHED,
  ERROR_MAX_PRIMITIVE_ARGS,
  ERROR_WRONG_NUMBER_OF_ARGS,
  ERROR_CLOSURE_HAS_NO_BODY,
} error_code_t;

extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);

#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#endif /* _FATAL_ERROR_ */
