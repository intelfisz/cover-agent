#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_BLOCK_SIZE 32
#define MAX_LEVELS 10

/**
 * @brief Structure representing a memory block in the buddy allocator.
 */
typedef struct Block {
    struct Block *next; /**< Pointer to the next block in the free list */
    int size;           /**< Size of the block */
    int free;           /**< Flag indicating if the block is free (1) or allocated (0) */
} Block;

/**
 * @brief Structure representing the buddy allocator.
 */
typedef struct BuddyAllocator {
    Block *free_lists[MAX_LEVELS]; /**< Array of free lists for each level */
    void *memory_pool;             /**< Pointer to the memory pool */
    size_t pool_size;              /**< Size of the memory pool */
} BuddyAllocator;

/**
 * @brief Initializes the buddy allocator with a given pool size.
 *
 * @param pool_size The size of the memory pool to be allocated.
 * @return Pointer to the initialized BuddyAllocator, or NULL on failure.
 */
BuddyAllocator *init_buddy_allocator(size_t pool_size);

/**
 * @brief Allocates memory from the buddy allocator.
 *
 * @param allocator Pointer to the BuddyAllocator.
 * @param size The size of the memory to be allocated.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void *buddy_alloc(BuddyAllocator *allocator, size_t size);

/**
 * @brief Frees memory back to the buddy allocator.
 *
 * @param allocator Pointer to the BuddyAllocator.
 * @param ptr Pointer to the memory to be freed.
 */
void buddy_free(BuddyAllocator *allocator, void *ptr);

/**
 * @brief Destroys the buddy allocator and frees the memory pool.
 *
 * @param allocator Pointer to the BuddyAllocator to be destroyed.
 */
void destroy_buddy_allocator(BuddyAllocator *allocator);

/**
 * @brief Prints the free list for each level of the buddy allocator.
 *
 * @param allocator Pointer to the BuddyAllocator.
 */
void print_free_list(BuddyAllocator *allocator);

/**
 * @brief Prints the memory pool indicating allocated and free regions.
 *
 * @param allocator Pointer to the BuddyAllocator.
 */
void print_memory_pool(BuddyAllocator *allocator);

#ifdef __cplusplus
}
#endif

#endif // BUDDY_ALLOCATOR_H
