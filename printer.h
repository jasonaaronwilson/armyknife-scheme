#ifndef _PRINTER_H_
#define _PRINTER_H_

#include <stdint.h>

#include "byte-array.h"
#include "tagged-reference.h"

__attribute__((warn_unused_result)) byte_array_t*
    print_tagged_reference_to_byte_arary(byte_array_t* destination,
                                         tagged_reference_t reference);

#endif /* _PRINTER_H_ */
