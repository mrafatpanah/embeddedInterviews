#include <stdio.h>

// Public function declaration
void public_increment(void);
void get_status(void);
int moduleCounter = 0;

int main(void) {
    printf("Starting program...\n");
    
    // Call public functions from static_global1.c
    public_increment();
    public_increment();
    get_status();
    public_increment();
    get_status();
    printf("ModuleCounter status in 2nd file with the exact same name: %d\n", moduleCounter);  // Error: static variable not visible here
    
    // The following would cause compilation errors if uncommented:
    // static_increment();  // Error: static function not visible here
    
    printf("Program completed.\n");
    return 0;
}