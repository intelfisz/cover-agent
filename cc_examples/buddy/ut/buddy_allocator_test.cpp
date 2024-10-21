#include "buddy_allocator.h"
#include <gtest/gtest.h>

class BuddyAllocatorTest : public ::testing::Test {
protected:
    BuddyAllocator *allocator;

    void SetUp() override {
        allocator = init_buddy_allocator(1024);
        ASSERT_NE(allocator, nullptr);
    }

    void TearDown() override {
        destroy_buddy_allocator(allocator);
    }
};

TEST_F(BuddyAllocatorTest, AllocationAndFree) {
    void *ptr1 = buddy_alloc(allocator, 100);
    ASSERT_NE(ptr1, nullptr);

    void *ptr2 = buddy_alloc(allocator, 200);
    ASSERT_NE(ptr2, nullptr);

    void *ptr3 = buddy_alloc(allocator, 50);
    ASSERT_NE(ptr3, nullptr);

    // Check that the memory pool is correctly allocated
    Block *block1 = (Block *)ptr1 - 1;
    Block *block2 = (Block *)ptr2 - 1;
    Block *block3 = (Block *)ptr3 - 1;

    EXPECT_EQ(block1->free, 0);
    EXPECT_EQ(block2->free, 0);
    EXPECT_EQ(block3->free, 0);

    // Free the allocated memory
    buddy_free(allocator, ptr1);
    buddy_free(allocator, ptr2);
    buddy_free(allocator, ptr3);

    // Check that the memory pool is correctly freed
    EXPECT_EQ(block1->free, 1);
    EXPECT_EQ(block2->free, 1);
    EXPECT_EQ(block3->free, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
