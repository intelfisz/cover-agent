#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    size_t pool_size = 1024;
    BuddyAllocator *allocator = init_buddy_allocator(pool_size);

    if (!allocator) {
        fprintf(stderr, "Failed to initialize buddy allocator\n");
        return EXIT_FAILURE;
    }

    void *ptr1 = buddy_alloc(allocator, 100);
    void *ptr2 = buddy_alloc(allocator, 200);
    void *ptr3 = buddy_alloc(allocator, 50);

    printf("Memory pool after allocations:\n");
    print_memory_pool(allocator);

    buddy_free(allocator, ptr1);
    buddy_free(allocator, ptr2);
    buddy_free(allocator, ptr3);

    printf("Memory pool after deallocations:\n");
    print_memory_pool(allocator);

    destroy_buddy_allocator(allocator);
    return 0;
}
