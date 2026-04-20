#ifndef MY_MEMORY_ALLOCATOR_HEADER
#define MY_MEMORY_ALLOCATOR_HEADER

#include "allocator-internal.hpp"
#include "memory-header.hpp"

extern MemoryHeader* free_list;

void* mymalloc(size_t size);
void myfree(void* payload);

#endif  // MY_MEMORY_ALLOCATOR_HEADER
