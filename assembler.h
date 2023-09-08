#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <stdint.h>

#include "paged-memory.h"
#include "symbol-table.h"

typedef struct {
  uint64_t address_start;
  uint64_t address_end;
  int symbols_unknown;
  int symbols_modified;
  symbol_table *symbols;
} assembly_result;

extern assembly_result assemble(paged_memory *memory, uint64_t addresss,
                                symbol_table *symbols, char *statement);

#endif /* _ASSEMBLER_H_ */
