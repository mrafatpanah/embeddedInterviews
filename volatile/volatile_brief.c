#include <stdio.h>
#include <stdint.h>

// Simulated hardware register
uint32_t hardware_register = 0;

void simulate_hardware_interrupt(void) {
    hardware_register = 0xDEADBEEF;  // Hardware changes register
}

int main(void) {
    printf("=== VOLATILE KEYWORD DEMONSTRATION ===\n\n");
    
    // WITHOUT VOLATILE - BROKEN with -O2 optimization
    printf("1. WITHOUT volatile (BROKEN with -O2):\n");
    uint32_t *reg1 = &hardware_register;
    hardware_register = 0;
    
    int count1 = 0;
    while (*reg1 == 0 && count1 < 3) {
        printf("   Waiting... register = 0x%08X\n", *reg1);
        if (count1 == 1) simulate_hardware_interrupt();
        count1++;
    }
    printf("   Result: %s (register = 0x%08X)\n", 
           (*reg1 == 0) ? "FAILED - optimization cached value!" : "Success", *reg1);
    
    printf("\n");
    
    // WITH VOLATILE - WORKS CORRECTLY
    printf("2. WITH volatile (CORRECT):\n");
    volatile uint32_t *reg2 = (volatile uint32_t*)&hardware_register;
    hardware_register = 0;
    
    int count2 = 0;
    while (*reg2 == 0 && count2 < 3) {
        printf("   Waiting... register = 0x%08X\n", *reg2);
        if (count2 == 1) simulate_hardware_interrupt();
        count2++;
    }
    printf("   Result: %s (register = 0x%08X)\n",
           (*reg2 != 0) ? "SUCCESS - detected hardware change!" : "Failed", *reg2);
    
    printf("\n=== CONCLUSION ===\n");
    printf("WITHOUT volatile: Compiler optimizes away memory reads\n");
    printf("WITH volatile:    Forces actual memory access each time\n");
    
    return 0;
}