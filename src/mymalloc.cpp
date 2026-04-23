#include <errno.h>
#include <stdio.h>

#include "my-memory-allocator.hpp"

bool first_call{true};
MemoryHeader* free_list{nullptr};
size_t allocated_count{0};
const size_t kLargeChunkSize{1024 * 1024};

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

  // alignment
  arg_size = (arg_size + 7) & (~7);
  const size_t kRequiredBlockSize{sizeof(MemoryHeader) + arg_size};
  // search from the begining of free_list
  MemoryHeader* current_header = free_list;
  while (current_header) {
    if (current_header->is_free && current_header->size > arg_size) {
      void* payload = static_cast<void*>(current_header + 1);
      if (current_header->size > kRequiredBlockSize) {
        // split current_header
        MemoryHeader* new_header{reinterpret_cast<MemoryHeader*>(
            static_cast<char*>(payload) + arg_size)};
        SetMemoryHeader(new_header, current_header->size - kRequiredBlockSize,
                        true, current_header->next);
        // update current_header
        SetMemoryHeader(current_header, arg_size, false, new_header);
      } else {
        // use witout split
        SetMemoryHeader(current_header, arg_size, false, current_header->next);
      }
      ++allocated_count;
      return payload;
    }
    current_header = current_header->next;
  }

  return nullptr;
}
