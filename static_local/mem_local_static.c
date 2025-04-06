#include <stdio.h>

void countCalls() {
    static int callCount = 0;
    callCount++;
    printf("Function called %d times\n", callCount);
}

int main() {
    countCalls();
    countCalls();
    countCalls();
    return 0;
}