#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function without restrict keyword
void vector_add_standard(int* result, const int* a, const int* b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        result[i] = a[i] + b[i];
    }
}

// Function with restrict keyword
void vector_add_restrict(int* restrict result, const int* restrict a, 
                        const int* restrict b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        result[i] = a[i] + b[i];
    }
}

// Function with potential aliasing problem
void vector_scale_inplace(int* data, int scale, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] = data[i] * scale;
    }
}

// Function with restrict keyword to prevent aliasing
void vector_scale_restrict(int* restrict output, const int* restrict input, 
                          int scale, size_t length) {
    for (size_t i = 0; i < length; i++) {
        output[i] = input[i] * scale;
    }
}

// Aliasing example - the compiler can't optimize this well
void dangerous_aliasing_example(int* a, int* b, size_t length) {
    printf("Dangerous aliasing example:\n");

    for (size_t i = 0; i < length; i++) {
        a[i] = i + 1;
    }
    
    printf("Initial values: ");
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("...\n");
    
    // If a and b might be the same array, compiler must be conservative
    for (size_t i = 0; i < length; i++) {
        b[i] = a[i] * 2;
    }
    
    printf("After operation: ");
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("...\n\n");
}

// Safe version with restrict keyword
void safe_no_aliasing_example(int* restrict a, int* restrict b, size_t length) {
    printf("Safe no-aliasing example with restrict:\n");
    
    for (size_t i = 0; i < length; i++) {
        a[i] = i + 1;
    }
    
    printf("Initial values: ");
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("...\n");
    
    // With restrict, compiler knows a and b don't overlap
    for (size_t i = 0; i < length; i++) {
        b[i] = a[i] * 2;
    }
    
    printf("After operation: ");
    for (size_t i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("...\n");
}

int main(void) {
    const size_t SIZE = 1000000;
    int *array1 = malloc(SIZE * sizeof(int));
    int *array2 = malloc(SIZE * sizeof(int));
    int *result = malloc(SIZE * sizeof(int));
    
    // Initialize data
    for (size_t i = 0; i < SIZE; i++) {
        array1[i] = i % 100;
        array2[i] = i % 50;
        result[i] = 0;
    }
    
    // Compare performance (simplified)
    clock_t start, end;
    double time_standard, time_restrict;
    
    // Time standard version
    start = clock();
    for (int run = 0; run < 100; run++) {
        vector_add_standard(result, array1, array2, SIZE);
    }
    end = clock();
    time_standard = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Reset result array
    memset(result, 0, SIZE * sizeof(int));
    
    // Time restrict version
    start = clock();
    for (int run = 0; run < 100; run++) {
        vector_add_restrict(result, array1, array2, SIZE);
    }
    end = clock();
    time_restrict = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Restrict Keyword Performance Example\n");
    printf("===================================\n\n");
    printf("Standard vector_add time: %.6f seconds\n", time_standard);
    printf("Restrict vector_add time: %.6f seconds\n", time_restrict);
    printf("Performance improvement: %.2f%%\n\n", 
           (time_standard - time_restrict) / time_standard * 100);
    
    // Demonstrate aliasing issues with a small array
    int small_a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // This is dangerous - aliased pointers!
    dangerous_aliasing_example(small_a, small_a, 10);
    
    // This is safe with restrict, but must not be called with aliased pointers
    int small_b[10];
    safe_no_aliasing_example(small_a, small_b, 10);
    
    // Clean up
    free(array1);
    free(array2);
    free(result);
    
    return 0;
}
