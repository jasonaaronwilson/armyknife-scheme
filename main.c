/**
 * This is the program entry point of the Comet VM (comet-vm)
 * executable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocate.h"
#include "evaluator.h"
#include "global-environment.h"
#include "printer.h"
#include "reader.h"

byte_array_t* read_expression_lines(char* prompt);

/**
 * This is a simple main routine for the interpreter.
 */
int main(int argc, char** argv) {

  fprintf(stderr,
          ";;; armyknife-scheme - a demonstration scheme interpreter in C\n");
  fprintf(stderr, ";;;   C-c will exit\n");

  environment_t* env = make_global_environment();

  // read(), eval(), print() loop.
  while (1) {
    byte_array_t* input_array = read_expression_lines("]=> ");
    char* input = byte_array_c_substring(input_array, 0,
                                         byte_array_length(input_array));

    tagged_reference_t expr = read_expression(input, 0).result;
    byte_array_t* output = make_byte_array(128);
    output = print_tagged_reference_to_byte_arary(output, expr);
    output = byte_array_append_byte(output, '\0');

    tagged_reference_t result = eval(env, expr, true);

    byte_array_t* output2 = make_byte_array(128);
    output2 = print_tagged_reference_to_byte_arary(output2, result);
    output2 = byte_array_append_byte(output2, '\0');

    fprintf(stdout, "\n;Value: %s\n\n", &output2->elements[0]);
  }

  exit(0);
}

/**
 * Read one or more lines from stdin.
 */
byte_array_t* read_expression_lines(char* prompt) {
  byte_array_t* result = make_byte_array(128);
  fputs(prompt, stderr);

  char line[1024];
  fgets(line, sizeof(line), stdin);

  // TODO(jawilson): read more lines if necessary to finish an
  // expression.

  result = byte_array_append_bytes(result, (uint8_t*) line, strlen(line));
  return result;
}
