#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example 1: Stack allocation (automatic)
void stack_allocation_example(void) {
    // Stack allocated buffer - limited size but fast
    char buffer[100];
    
    strncpy(buffer, "This is stored on the stack", sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
    
    printf("Stack buffer content: %s\n", buffer);
    printf("Stack buffer address: %p\n", (void*)buffer);
    
    // No deallocation needed - memory automatically freed when function returns
}

// Example 2: Heap allocation (dynamic)
void heap_allocation_example(void) {
    // Heap allocated buffer - size determined at runtime
    char* buffer = (char*)malloc(100);
    
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    
    strncpy(buffer, "This is stored on the heap", 99);
    buffer[99] = '\0';  // Ensure null-termination
    
    printf("Heap buffer content: %s\n", buffer);
    printf("Heap buffer address: %p\n", (void*)buffer);
    
    // Must explicitly free the memory
    free(buffer);
    // buffer = NULL;  // Good practice to prevent use-after-free
}

// Example 3: Static allocation
void static_allocation_example(void) {
    // Static allocation - exists for entire program duration
    static char buffer[100];
    
    strcpy(buffer, "This is stored in static memory");
    
    printf("Static buffer content: %s\n", buffer);
    printf("Static buffer address: %p\n", (void*)buffer);
    
    // Content persists between function calls
}

// Example 4: Memory fragmentation demonstration
void fragmentation_demonstration(void) {
    printf("Memory fragmentation demonstration:\n");
    
    // Allocate several blocks
    char* block1 = malloc(100);
    char* block2 = malloc(100);
    char* block3 = malloc(100);
    
    printf("Initial allocations: %p, %p, %p\n", 
           (void*)block1, (void*)block2, (void*)block3);
    
    // Free the middle block
    free(block2);
    block2 = NULL;
    
    // Try to allocate a larger block
    char* large_block = malloc(250);
    
    printf("After freeing middle block, new large allocation: %p\n", 
           (void*)large_block);
    printf("Note: Even with 100 bytes freed, we may not be able to\n");
    printf("fit a 250-byte allocation in that space\n");
    
    // Clean up
    free(block1);
    free(block3);
    free(large_block);
}

int main(void) {
    printf("Memory Allocation Strategies in Embedded C\n");
    printf("=========================================\n\n");
    
    stack_allocation_example();
    printf("\n");
    
    heap_allocation_example();
    printf("\n");
    
    static_allocation_example();
    printf("\n");
    
    // Call again to show persistence
    static_allocation_example();
    printf("\n");
    
    fragmentation_demonstration();
    
    return 0;
}