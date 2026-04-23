#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "my-memory-allocator.hpp"

constexpr size_t kBasicBlockSize{64};

TEST(MyMallocTest, BasicAllocation) {
  void* p = mymalloc(kBasicBlockSize);
  EXPECT_NE(p, nullptr);
  myfree(p);
}

TEST(MyMallocTest, WithoutSplit) {
  void* p1 = mymalloc(16);
  // to block coalescing
  void* p2 = mymalloc(kBasicBlockSize);

  myfree(p1);
  // allocation witout split.
  void* p3 = mymalloc(8);
  EXPECT_EQ(p3, p1);
  EXPECT_EQ(GetMemoryHeader(p3)->next, GetMemoryHeader(p2));

  myfree(p2);
  myfree(p3);
}

TEST(MyMallocTest, CoalesceNext) {
  void* p1 = mymalloc(kBasicBlockSize);
  void* p2 = mymalloc(kBasicBlockSize);
  void* p3 = mymalloc(kBasicBlockSize);

  myfree(p2);
  myfree(p1);

  // check address
  void* p4 = mymalloc(2 * kBasicBlockSize);
  EXPECT_EQ(p1, p4);
  // check size
  auto* mh4 = GetMemoryHeader(p4);
  EXPECT_EQ(mh4->size, 2 * kBasicBlockSize);

  myfree(p3);
  myfree(p4);
}

TEST(MyMallocTest, CoalescePrev) {
  void* p1 = mymalloc(kBasicBlockSize);
  void* p2 = mymalloc(kBasicBlockSize);
  void* p3 = mymalloc(kBasicBlockSize);

  myfree(p1);
  myfree(p2);

  // check address
  void* p4 = mymalloc(2 * kBasicBlockSize);
  EXPECT_EQ(p1, p4);

  // check size
  auto* mh4 = GetMemoryHeader(p4);
  EXPECT_EQ(mh4->size, 2 * kBasicBlockSize);

  myfree(p3);
  myfree(p4);
}

TEST(MyMallocTest, CoalesceBoth) {
  void* p1 = mymalloc(kBasicBlockSize);
  void* p2 = mymalloc(kBasicBlockSize);
  void* p3 = mymalloc(kBasicBlockSize);
  void* p4 = mymalloc(kBasicBlockSize);

  myfree(p1);
  myfree(p3);
  myfree(p2);

  // check address
  void* p5 = mymalloc(3 * kBasicBlockSize);
  EXPECT_EQ(p1, p5);
  // check size
  auto* mh5 = GetMemoryHeader(p5);
  EXPECT_EQ(mh5->size, 3 * kBasicBlockSize);

  myfree(p4);
  myfree(p5);
}

void UseAllocator(size_t size) {
  void* p = mymalloc(size);
  EXPECT_NE(p, nullptr);
  myfree(p);
}

TEST(MyMallocTest, MultiThread) {
  std::promise<void> go;
  std::shared_future<void> ready = go.get_future().share();
  auto task = [&ready]() {
    ready.wait();
    UseAllocator(kBasicBlockSize);
  };

  std::thread t1(task);
  std::thread t2(task);

  go.set_value();

  t1.join();
  t2.join();
}

TEST(MyMallocTest, Alignment) {
  void* p1 = mymalloc(1);
  void* p2 = mymalloc(kBasicBlockSize);

  EXPECT_EQ(reinterpret_cast<uintptr_t>(p1) % 8, 0u);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(p2) % 8, 0u);

  myfree(p1);
  myfree(p2);
}
