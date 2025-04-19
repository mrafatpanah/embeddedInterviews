#include <stdio.h>

// Default alignment
struct DefaultStruct {
    char a;
    int b;
    char c;
    int d;
};

// Pack with alignment of 1 byte
#pragma pack(push, 1)
struct PackedStruct {
    char a;
    int b;
    char c;
    int d;
};
#pragma pack(pop)

// Pack with alignment of 2 bytes
#pragma pack(push, 2)
struct Pack2Struct {
    char a;
    int b;
    char c;
    int d;
};
#pragma pack(pop)

// Pack with alignment of 4 bytes
#pragma pack(push, 4)
struct Pack4Struct {
    char a;
    int b;
    char c;
    int d;
};
#pragma pack(pop)

int main() {
    printf("Size of DefaultStruct: %zu bytes\n", sizeof(struct DefaultStruct));
    printf("Size of PackedStruct (pack 1): %zu bytes\n", sizeof(struct PackedStruct));
    printf("Size of Pack2Struct (pack 2): %zu bytes\n", sizeof(struct Pack2Struct));
    printf("Size of Pack4Struct (pack 4): %zu bytes\n", sizeof(struct Pack4Struct));
    
    // Print memory layout visualization
    printf("\nMemory Layout Visualization:\n");
    printf("DefaultStruct: |a|pad|pad|pad|b|b|b|b|c|pad|pad|pad|d|d|d|d|\n");
    printf("PackedStruct:  |a|b|b|b|b|c|d|d|d|d|\n");
    printf("Pack2Struct:   |a|pad|b|b|b|b|c|pad|d|d|d|d|\n");
    printf("Pack4Struct:   |a|pad|pad|pad|b|b|b|b|c|pad|pad|pad|d|d|d|d|\n");
    
    return 0;
}