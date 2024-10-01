/**
 * @file   mm.c
 * @Author 02335 team
 * @date   September, 2024
 * @brief  Memory management skeleton.
 * 
 */

#include <stdint.h>

#include "mm_aux.c"
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
#define SET_FREE(p, f)  p->next = (void *)(((uintptr_t)(p->next) & ~FREE_FLAG_MASK) | ((f) & FREE_FLAG_MASK))
#define SIZE(p)  ((size_t)((uintptr_t)(GET_NEXT(p)) - (uintptr_t)(p)))

#define MIN_SIZE     (8)   // A block should have at least 8 bytes available for the user


static BlockHeader * first = NULL;
static BlockHeader * current = NULL;
static BlockHeader * last = NULL;

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

    BlockHeader *search_start = current;

    do {
        if (GET_FREE(current)) {
            if (SIZE(current) >= aligned_size) {
                size_t remaining_size = SIZE(current) - aligned_size - sizeof(BlockHeader);

                if (remaining_size >= MIN_SIZE) {
                    // Split the block
                    BlockHeader *new_block = (BlockHeader *)((uintptr_t)current + sizeof(BlockHeader) + aligned_size);
                    SET_NEXT(new_block, GET_NEXT(current));
                    SET_FREE(new_block, 1);

                    SET_NEXT(current, new_block);
                }

                // Mark current block as not free
                SET_FREE(current, 0);
                current = GET_NEXT(current);
                
                return (void *)(current->user_block);
            }
        }

        current = GET_NEXT(current);
    } while (current != search_start);

    return NULL;
}


/**
 * @name    simple_free
 * @brief   Frees previously allocated memory and makes it available for subsequent calls to simple_malloc
 *
 * This function should behave similar to a normal free implementation. 
 *
 * @param void *ptr Pointer to the memory to free.
 *
 */
void simple_free(void * ptr) {
    BlockHeader * block = (BlockHeader *)((uintptr_t)ptr - sizeof(BlockHeader));  // Find the block for the given pointer

    if (GET_FREE(block)) {
        // Block is already free, probably an error
        return;
    }

    SET_FREE(block, 1);  // Mark the block as free

    // Possibly coalesce consecutive free blocks here
    BlockHeader *next_block = GET_NEXT(block);
    if (GET_FREE(next_block)) {
        // Coalesce the current block with the next one if it's free
        SET_NEXT(block, GET_NEXT(next_block));
    }
}



