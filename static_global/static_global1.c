#include <stdio.h>

// Static global variable - only visible within this file
static int moduleCounter = 0;

// Static function - only callable within this file
static void static_increment(void) {
    moduleCounter++;
    printf("Counter incremented to: %d\n", moduleCounter);
}

// Public function that uses our private implementation
void public_increment(void) {
    static_increment();
}

// Another public function using the same private state
void get_status(void) {
    printf("Current module status: %d operations performed\n", moduleCounter);
}