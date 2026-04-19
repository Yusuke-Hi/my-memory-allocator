#include <gtest/gtest.h>

#include "my-memory-allocator.hpp"

constexpr size_t kBasicBlockSize{64};

TEST(MyMallocTest, BasicAllocation) {
  void* p = mymalloc(kBasicBlockSize);
  EXPECT_NE(p, nullptr);
  myfree(p);
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
