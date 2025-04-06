# Embedded Interview Series
## String Declarations in C

As an embedded systems developer, I’ve encountered many technical questions that test a deep understanding of low-level concepts. One question that frequently comes up is:

### The Question:
What is the difference between these two declarations?
```
char* str1 = "string";
char str2[] = "string";
```

### The Answer:
While both store the same text, they behave differently in memory.

`char* str1 = "string";`

**Pointer to a String Literal:**
This creates a pointer to a string literal stored in read-only memory (typically in the .rodata section). The pointer itself resides in RAM, but it points to immutable memory. Any attempt to modify its contents (e.g., `str1[0] = 'S'`) will likely result in a segmentation fault.

`char str2[] = "string";`
**Character Array:**
This declares a mutable character array in writable memory (usually the stack), containing a copy of the string. Unlike the first case, modifying the content (e.g., `str2[0] = 'S'`) is perfectly valid.
You can see the attached pictures for better understanding.

### Why This Matters:
In embedded systems with limited resources, understanding memory allocation is crucial. The first approach saves RAM by keeping only a pointer, while the second duplicates the string in RAM but allows modifications.
This distinction becomes even more important when dealing with large strings or when memory constraints are tight.