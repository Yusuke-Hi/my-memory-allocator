#include "coalesce.hpp"
#include "my-memory-allocator.hpp"

void myfree(void* payload) {
  auto* target = GetMemoryHeader(payload);
  target->is_free = true;

  // Coalescing
  if (free_list == target) {
    if (target->next && target->next->is_free) {
      Coalesce(target, target->next);
    }
    return;
  }

  auto* current{free_list};
  while (current && current->next) {
    if (current->next == target) {
      // coalesce from from the rear, if can.
      if (target->next && target->next->is_free) {
        Coalesce(target, target->next);
      }
      if (current->is_free) {
        Coalesce(current, target);
      }
      break;
    }
    current = current->next;
  }
}
