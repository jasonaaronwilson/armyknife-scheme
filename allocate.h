#ifndef _ALLOCATE_H_
#define _ALLOCATE_H_

#include <stdint.h>

extern uint8_t* checked_malloc(char* file, int line, uint64_t amount);
extern void checked_free(char* file, int line, void* pointer);

#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))
#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

#endif /* _ALLOCATE_H_ */
