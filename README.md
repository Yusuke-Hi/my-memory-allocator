# my-memory-allocator
A simple memory allocator implementation written in C++. This project was created to learn memory allocation.

## Feature
- **First-fit allocation** with free list traversal
- **Memory coalescing** to reduce fragmentation
- **8-byte alignment** for CPU performance
- **Thread-safe** with mutex
- **Auto re-initialization** when all blocks are freed

## Build
```
mkdir build
cd build
cmake ..
make
```

## How to use
```
size_t size{64};
void* ptr = mymalloc(size);
// use ptr.
myfree(ptr);
```

## Architecture
### Memory Layout
Large chunk (1MB) obtained via mmap, divided into blocks:

[ MemoryHeader | payload ] → [ MemoryHeader | payload ] → nullptr
  - size: payload size
  - is_free: availability flag
  - next: pointer to next block

### mymalloc
Allocate a memory block of the requested size.

### myfree
Free a block and coalesce fragmented memory blocks.

### MemoryHeader
Manage memory block with this header.
