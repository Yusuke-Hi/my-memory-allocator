#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>

#include "my-memory-allocator.hpp"

static bool first_call{true};
MemoryHeader* free_list{nullptr};
static const size_t kLargeChunkSize{1024 * 1024};

void* mymalloc(size_t arg_size) {
  std::lock_guard<std::mutex> lock{mtx};
  if (first_call) {
    free_list = static_cast<MemoryHeader*>(
        mmap(NULL, kLargeChunkSize, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (free_list == MAP_FAILED) {
      perror("mmap");
      return nullptr;
    }

    // initialize free_list
    SetMemoryHeader(free_list, kLargeChunkSize - sizeof(MemoryHeader), true,
                    nullptr);

    first_call = false;
  }

  // search from the begining of free_list
  MemoryHeader* current_header = free_list;
  while (current_header) {
    if (current_header->is_free && current_header->size >= arg_size) {
      // split current block
      void* payload = static_cast<void*>(current_header + 1);
      MemoryHeader* new_header{reinterpret_cast<MemoryHeader*>(
          static_cast<char*>(payload) + arg_size)};

      SetMemoryHeader(new_header, current_header->size - arg_size, true,
                      current_header->next);
      // update current_header
      SetMemoryHeader(current_header, arg_size, false, new_header);

      return payload;
    }
    current_header = current_header->next;
  }

  return nullptr;
}
