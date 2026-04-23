#ifndef MY_MEMORY_ALLOCATOR_HEADER
#define MY_MEMORY_ALLOCATOR_HEADER

#include <sys/mman.h>

#include "allocator-internal.hpp"
#include "memory-header.hpp"

extern bool first_call;
extern MemoryHeader* free_list;
extern size_t allocated_count;
extern const size_t kLargeChunkSize;

void* mymalloc(size_t size);
void myfree(void* payload);

#endif  // MY_MEMORY_ALLOCATOR_HEADER
