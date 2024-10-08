#include <stdio.h>
#include <stdint.h>

#include "mm.h"

/** 
 * Test program that makes some simple allocations and enables
 * you to inspect the result.
 *
 * Demonstrates that the memory manager does not use a first-fit strategy.
 */

int main(int argc, char ** argv) {
    /* Ensure that macros are working */
    int ret = simple_macro_test();
    if (ret > 0) {
        printf("Macro test returned %d\n", ret);
        return 1;
    }

    void * a = simple_malloc(0x200);  // Allocate 512 bytes
    void * b = simple_malloc(0x100);  // Allocate 256 bytes
    void * c = simple_malloc(0x200);  // Allocate another 512 bytes

    simple_free(a);  // Free the 512-byte block `a`

    void * d = simple_malloc(0x100);  // Allocate another 256 bytes

    // Now, we have two 256-byte allocations (b and d).
    // If the allocator is not using first-fit, `d` might not use the space freed by `a`.
    // If `d` does not occupy the same address as `a`, then it did not use the first available block.

    if (d == a) {
        printf("Test failed: The allocator used a first-fit strategy.\n");
    } else {
        printf("Test passed: The allocator did not use a first-fit strategy.\n");
    }

    simple_block_dump(); 

    return 0;
}
