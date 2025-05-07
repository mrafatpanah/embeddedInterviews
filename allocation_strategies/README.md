# Embedded Interview Series
## Memory Allocation Strategies in C

As we continue exploring critical embedded systems concepts, today's post addresses another frequent interview topic: memory allocation strategies and their implications in resource-constrained environments.

### The Question:
What are the different memory allocation techniques in C, and what are their trade-offs in embedded systems?

### The Answer:
C provides three primary methods for memory allocation, each with distinct characteristics that impact performance, reliability, and resource usage.

1. **Stack allocation (automatic)**
```c
void function() {
    int array[100];  // Fixed size, automatically managed
}
```

2. **Heap allocation (dynamic)**
```c
void function() {
    int* array = (int*)malloc(100 * sizeof(int));  // Variable size, manual management
    // ...
    free(array);  // Must be explicitly freed
}
```

3. **Static allocation**
```c
void function() {
    static int array[100];  // Exists for program duration
}
```

### Memory Regions Comparison:
![Comparison](Memory%20Regions%20Comparison.jpg)

### Critical Embedded Considerations:

1. **Stack Overflow Risk:**
```c
void recursive_function(int depth) {
    char large_buffer[1000];  // 1KB on stack each call
    if (depth > 0)
        recursive_function(depth - 1);  // Danger!
}
```

2. **Heap Fragmentation:**
```c
// Over time, this pattern creates memory "holes"
void fragment_memory() {
    char* blocks[10];
    // Allocate blocks of varying sizes
    for(int i = 0; i < 10; i++)
        blocks[i] = malloc(rand() % 100 + 50);
        
    // Free every other block
    for(int i = 0; i < 10; i+=2) {
        free(blocks[i]);
        blocks[i] = NULL;
    }
    // Remaining gaps may be too small for new allocations
}
```

### Common Embedded Strategies:

- Static Pre-allocation: Allocate everything at startup
- Memory Pools: Pre-allocate fixed-size blocks for specific purposes
- Stack-only Design: Avoid heap entirely in critical systems
- Bounded Heap: Use heap with fixed upper limit and monitoring

### Why This Matters:
In embedded systems, memory is often severely constrained. A deep understanding of allocation strategies helps developers:
- Prevent unpredictable runtime failures
- Ensure deterministic timing behavior
- Maximize efficient use of limited resources
- Avoid system crashes in long-running applications

Proper memory management is particularly critical in safety-critical systems where reliability and predictability are non-negotiable requirements.
