/**
 * @file   mm.c
 * @Author 02335 team
 * @date   September, 2024
 * @brief  Memory management skeleton.
 *
 */

#include <stdint.h>

#include "mm.h"



/* Proposed data structure elements */

typedef struct header {
  struct header * next;     // Bit 0 is used to indicate free block
  uint64_t user_block[0];   // Standard trick: Empty array to make sure start of user block is aligned
} BlockHeader;

/* Macros to handle the free flag at bit 0 of the next pointer of header pointed at by p */
#define FREE_FLAG_MASK  (1)             // Mask to isolate the free flag in the LSB
#define POINTER_MASK    (~FREE_FLAG_MASK) // Mask to clear the LSB (pointer portion)

// Macro to get the next pointer, masking out the free flag
#define GET_NEXT(p)     ((void *)((uintptr_t)(p->next) & POINTER_MASK))

// Macro to set the next pointer, preserving the free flag
#define SET_NEXT(p, n)  p->next = (void *)(((uintptr_t)(p->next) & FREE_FLAG_MASK) | ((uintptr_t)(n) & POINTER_MASK))

#define GET_FREE(p)    (uint8_t) ( (uintptr_t) (p->next) & 0x1 )   /* OK -- do not change */
#define SET_FREE(p, f)  p->next = (void *)(((uintptr_t)(p->next) & POINTER_MASK) | ((f) & FREE_FLAG_MASK))
#define SIZE(p)  ((size_t)((uintptr_t)(GET_NEXT(p)) - (uintptr_t)(p) - sizeof(BlockHeader)))

#define MIN_SIZE     (8)   // A block should have at least 8 bytes available for the user


BlockHeader *first = NULL;
BlockHeader *current = NULL;
BlockHeader *last = NULL;

/**
 * @name    simple_init
 * @brief   Initialize the block structure within the available memory
 *
 */
void simple_init() {
    uintptr_t aligned_memory_start = (memory_start + 7) & ~0x7;  // Align to 8-byte boundary
    uintptr_t aligned_memory_end = memory_end & ~0x7;             // Align to 8-byte boundary

    if (aligned_memory_start + 2 * sizeof(BlockHeader) + MIN_SIZE <= aligned_memory_end) {
        first = (BlockHeader *)aligned_memory_start;
        last = (BlockHeader *)(aligned_memory_end - sizeof(BlockHeader));  // Set global last
        SET_NEXT(first, last);
        SET_FREE(first, 1);

        SET_NEXT(last, first);  // Create circular link
        SET_FREE(last, 0);

        current = first;
    }
}

void* simple_malloc(size_t size) {
    if (first == NULL) {
        simple_init();
        if (first == NULL) return NULL;
    }

    size_t aligned_size = (size + 7) & ~0x7;  // Align to 8-byte boundary

    BlockHeader *search_start = current;  // Start from the current block

    do {
        if (GET_FREE(current)) {
            if (SIZE(current) >= aligned_size + sizeof(BlockHeader)) {
                size_t remaining_size = SIZE(current) - aligned_size - sizeof(BlockHeader);

                if (remaining_size >= MIN_SIZE + sizeof(BlockHeader)) {
                    // Split the block if there's enough space left for a new block
                    BlockHeader *new_block = (BlockHeader *)((uintptr_t)current + sizeof(BlockHeader) + aligned_size);
                    SET_NEXT(new_block, GET_NEXT(current));
                    SET_FREE(new_block, 1);
                    SET_NEXT(current, new_block);
                }

                // Mark current block as not free
                SET_FREE(current, 0);

                // Return the pointer to the user block
                void *user_block = (void *)(current->user_block);

                // Move to the next block for future allocations
                current = GET_NEXT(current);  // Continue from the next block for future allocations

                return user_block;
            }
        }

        current = GET_NEXT(current);  // Move to the next block
    } while (current != search_start);  // Wrap around if necessary

    return NULL;  // No suitable block found
}


void simple_free(void *ptr) {
    if (!ptr) return;

    BlockHeader *block = (BlockHeader *)((uintptr_t)ptr - sizeof(BlockHeader));  // Find the block for the given pointer

    if (GET_FREE(block)) {
        // Block is already free, return to avoid double free
        return;
    }

    SET_FREE(block, 1);  // Mark the block as free

    // Coalesce with next block if it's free
    BlockHeader *next_block = GET_NEXT(block);
    if (GET_FREE(next_block)) {
        // Merge with the next block
        SET_NEXT(block, GET_NEXT(next_block));
    }

    // Coalesce with previous block if it's free
    BlockHeader *prev_block = first;
    while (GET_NEXT(prev_block) != block && prev_block != last) {
        prev_block = GET_NEXT(prev_block);
    }

    if (GET_FREE(prev_block)) {
        SET_NEXT(prev_block, GET_NEXT(block));  // Merge the previous block with the current one
    }
}

#include "mm_aux.c"
