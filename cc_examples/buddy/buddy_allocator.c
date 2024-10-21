#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

static int get_level(size_t size);
static void split_block(BuddyAllocator *allocator, int level);
static void merge_block(BuddyAllocator *allocator, Block *block, int level);

BuddyAllocator *init_buddy_allocator(size_t pool_size) {
    if (pool_size < MIN_BLOCK_SIZE) {
        fprintf(stderr, "Error: Pool size must be at least %d bytes\n", MIN_BLOCK_SIZE);
        return NULL;
    }

    BuddyAllocator *allocator = (BuddyAllocator *)malloc(sizeof(BuddyAllocator));
    if (!allocator) {
        perror("Failed to allocate memory for BuddyAllocator");
        return NULL;
    }

    allocator->memory_pool = malloc(pool_size);
    if (!allocator->memory_pool) {
        perror("Failed to allocate memory pool");
        free(allocator);
        return NULL;
    }

    allocator->pool_size = pool_size;
    memset(allocator->free_lists, 0, sizeof(allocator->free_lists));

    Block *initial_block = (Block *)allocator->memory_pool;
    initial_block->size = pool_size;
    initial_block->free = 1;
    initial_block->next = NULL;

    int level = get_level(pool_size);
    allocator->free_lists[level] = initial_block;

    return allocator;
}

void *buddy_alloc(BuddyAllocator *allocator, size_t size) {
    if (size <= 0) {
        fprintf(stderr, "Error: Allocation size must be greater than 0\n");
        return NULL;
    }

    size += sizeof(Block);
    int level = get_level(size);

    for (int i = level; i < MAX_LEVELS; i++) {
        if (allocator->free_lists[i]) {
            while (i > level) {
                split_block(allocator, i);
                i--;
            }

            Block *block = allocator->free_lists[level];
            allocator->free_lists[level] = block->next;
            block->free = 0;
            return (void *)(block + 1);
        }
    }

    fprintf(stderr, "Error: Not enough memory to allocate %zu bytes\n", size);
    return NULL;
}

void buddy_free(BuddyAllocator *allocator, void *ptr) {
    if (!ptr) {
        fprintf(stderr, "Error: Cannot free a NULL pointer\n");
        return;
    }

    Block *block = (Block *)ptr - 1;
    int level = get_level(block->size);

    block->free = 1;
    block->next = allocator->free_lists[level];
    allocator->free_lists[level] = block;

    merge_block(allocator, block, level);
}

void destroy_buddy_allocator(BuddyAllocator *allocator) {
    if (allocator) {
        free(allocator->memory_pool);
        free(allocator);
    }
}

void print_free_list(BuddyAllocator *allocator) {
    for (int i = 0; i < MAX_LEVELS; i++) {
        printf("Level %d: ", i);
        Block *block = allocator->free_lists[i];
        while (block) {
            printf("[%d] -> ", block->size);
            block = block->next;
        }
        printf("NULL\n");
    }
}

void print_memory_pool(BuddyAllocator *allocator) {
    Block *block = (Block *)allocator->memory_pool;
    while ((char *)block < (char *)allocator->memory_pool + allocator->pool_size) {
        printf("Block at %p: size=%d, free=%d\n", (void *)block, block->size, block->free);
        block = (Block *)((char *)block + block->size);
    }
}

void print_memory_pool_graph(BuddyAllocator *allocator) {
    Block *block = (Block *)allocator->memory_pool;
    while ((char *)block < (char *)allocator->memory_pool + allocator->pool_size) {
        char status = block->free ? '-' : 'X';
        for (int i = 0; i < block->size; i++) {
            printf("%c", status);
        }
        block = (Block *)((char *)block + block->size);
    }
    printf("\n");
}

static int get_level(size_t size) {
    int level = 0;
    size_t block_size = MIN_BLOCK_SIZE;

    while (block_size < size && level < MAX_LEVELS) {
        block_size <<= 1;
        level++;
    }

    return level;
}

void split_block(BuddyAllocator *allocator, int level) {
    if (level <= 0 || level >= MAX_LEVELS || !allocator->free_lists[level]) {
        return;
    }

    Block *block = allocator->free_lists[level];
    allocator->free_lists[level] = block->next;

    size_t half_size = block->size / 2;
    Block *buddy = (Block *)((char *)block + half_size);

    block->size = half_size;
    block->free = 1;
    block->next = buddy;

    buddy->size = half_size;
    buddy->free = 1;
    buddy->next = allocator->free_lists[level - 1];

    allocator->free_lists[level - 1] = block;
}

void merge_block(BuddyAllocator *allocator, Block *block, int level) {
    if (level >= MAX_LEVELS - 1) {
        return;
    }

    size_t block_size = block->size;
    uintptr_t block_addr = (uintptr_t)block;
    uintptr_t buddy_addr = block_addr ^ block_size;
    Block *buddy = (Block *)buddy_addr;

    if (buddy->free && (size_t)buddy->size == block_size) {
        Block **prev = &allocator->free_lists[level];
        while (*prev && *prev != buddy) {
            prev = &(*prev)->next;
        }

        if (*prev) {
            *prev = buddy->next;
        }

        if (block_addr > buddy_addr) {
            Block *temp = block;
            block = buddy;
            buddy = temp;
        }

        block->size *= 2;
        merge_block(allocator, block, level + 1);
    } else {
        block->next = allocator->free_lists[level];
        allocator->free_lists[level] = block;
    }
}
