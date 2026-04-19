
#include "coalesce.hpp"

void Coalesce(MemoryHeader* first, MemoryHeader* second) {
  first->size += sizeof(MemoryHeader) + second->size;
  first->next = second->next;
}
