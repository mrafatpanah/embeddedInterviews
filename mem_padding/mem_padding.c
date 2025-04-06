#include <stdio.h>

typedef struct Example_a {
    char a;
    int b;
    char c;
    int d;
} example_a;

typedef struct Example_b {
    int b;
    int d;      
    char a;     
    char c;     
} example_b;


typedef struct Example_c {
    char c;
    double d;
    int s;
} example_c;

typedef struct Example_d {
    double d;
    int s;
    char c;
} example_d;

int main()
{
    printf("sizeof(example_a) = %lu\n", sizeof(example_a));
    printf("sizeof(example_b) = %lu\n", sizeof(example_b));
    
    printf("sizeof(example_c) = %lu\n", sizeof(example_c));
    printf("sizeof(example_d) = %lu\n", sizeof(example_d));

    return 0;
}