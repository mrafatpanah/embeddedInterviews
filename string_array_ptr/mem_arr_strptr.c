#include <stdio.h>

int main() {
    char* str1 = "string";
    char str2[] = "string";

    printf("base     str1: %s (Address: %p)\n", str1, (void*)str1);
    printf("base     str2: %s (Address: %p)\n", str2, (void*)str2);

    // Attempting modification
    str1[0] = 'S';  // Uncommenting this will likely cause a segmentation fault
    
    
    str2[0] = 'S';  //  Modifies the array successfully
    printf("modified str2: %s\n", str2);

    return 0;
}
