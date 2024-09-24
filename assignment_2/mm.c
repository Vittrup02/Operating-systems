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
  int size;
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

/**
 * @name    simple_init
 * @brief   Initialize the block structure within the available memory
 *
 */
void simple_init() {
  uintptr_t aligned_memory_start = memory_start;  /* TODO: Alignment */
  if (aligned_memory_start%MIN_SIZE != 0) {
    aligned_memory_start = memory_start + (MIN_SIZE-memory_start%MIN_SIZE);
  }
  uintptr_t aligned_memory_end   = memory_end;    /* TODO: Alignment */
  if (aligned_memory_end%MIN_SIZE != 0) {
    aligned_memory_end = memory_start + (MIN_SIZE-memory_end%MIN_SIZE);
  }
  BlockHeader * last;

  /* Already initalized ? */
  if (first == NULL) {
    /* Check that we have room for at least one free block and an end header */
    if (aligned_memory_start + 2*sizeof(BlockHeader) + MIN_SIZE <= aligned_memory_end) {
      first = (BlockHeader *) aligned_memory_start;
      first->size = (aligned_memory_end-aligned_memory_start) -2 * sizeof(BlockHeader);
      first->next = (BlockHeader *)(aligned_memory_start + sizeof(BlockHeader) + first->size);
      /* TODO: Place first and last blocks and set links and free flags properly */
    }
    current = first;     
  } 
}


/**
 * @name    simple_malloc
 * @brief   Allocate at least size contiguous bytes of memory and return a pointer to the first byte.
 *
 * This function should behave similar to a normal malloc implementation. 
 *
 * @param size_t size Number of bytes to allocate.
 * @retval Pointer to the start of the allocated memory or NULL if not possible.
 *
 */
void* simple_malloc(size_t size) {
  
  if (first == NULL) {
    simple_init();
    if (first == NULL) return NULL;
  }


  size_t aligned_size = size;  /* TODO: Alignment */
  if (size%8!= 0) {
    aligned_size = size + (8-size%8);
  }
  /* Search for a free block */
  BlockHeader * search_start = current;
  do {
 
    if (GET_FREE(current)) {

      /* Possibly coalesce consecutive free blocks here */

      /* Check if free block is large enough */
      if (SIZE(current) >= aligned_size) {
        /* Will the remainder be large enough for a new block? */
        if (SIZE(current) - aligned_size < sizeof(BlockHeader) + MIN_SIZE) {
          /* TODO: Use block as is, marking it non-free*/
        } else {
          /* TODO: Carve aligned_size from block and allocate new free block for the rest */
        }
        
        return (void *) NULL; /* TODO: Return address of current's user_block and advance current */
      }
    }

    current = GET_NEXT(current);
  } while (current != search_start);

 /* None found */
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
  BlockHeader * block = NULL; /* TODO: Find block corresponding to ptr */
  if (GET_FREE(block)) {
    /* Block is not in use -- probably an error */
    return;
  }

  /* TODO: Free block */

  /* Possibly coalesce consecutive free blocks here */
}


