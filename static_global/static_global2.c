#include <stdio.h>

// Public function declaration
void public_increment(void);
void get_status(void);
int moduleCounter = 0;

int main(void) {
    printf("Starting program...\n");
    
    // Call public functions from static_global1.c
    public_increment();     // Counter incremented to: 1
    public_increment();     // Counter incremented to: 2
    get_status();           // Current module status: 2 operations performed
    public_increment();     // Counter incremented to: 3
    get_status();           // Current module status: 3 operations performed
    printf("ModuleCounter status in 2nd file with" // moduleCounter = 0
            "the exact same name: %d\n", moduleCounter); 
    
    // The following would cause compilation errors if uncommented:
    // static_increment();  // Error: static function not visible here
    
    printf("Program completed.\n");
    return 0;
}