#include <stdio.h>
#include "mm_alloc.h"
int main() {
    // Allocate two small blocks
    void *small_block1 = mm_malloc(10);
    void *small_block2 = mm_malloc(10);

    // Free the two small blocks
    mm_free(small_block1);
    mm_free(small_block2);

    // Allocate a bigger block that should fit in the space of the two smaller blocks
    void *big_block = mm_malloc(20);

    // Check if the bigger block uses the space previously used by the small blocks
    if (big_block == small_block1) {
        printf("Coalescing works as expected.\n");
    } else {
        printf("Coalescing failed.\n");
    }

    // Free the memory
    mm_free(big_block);

    return 0;
}
