#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

/* =============================================================================
 * SECTION 1: Memory-Mapped I/O Register Definitions
 * ============================================================================= */

// Simulated hardware register addresses (typical ARM Cortex-M addresses)
#define GPIO_BASE_ADDR      0x40020000
#define UART_BASE_ADDR      0x40011000  
#define TIMER_BASE_ADDR     0x40010000
#define ADC_BASE_ADDR       0x40012000

// GPIO Register Structure - MUST use volatile for hardware registers
typedef struct {
    volatile uint32_t INPUT;        // 0x00: Input data register
    volatile uint32_t OUTPUT;       // 0x04: Output data register  
    volatile uint32_t DIRECTION;    // 0x08: Pin direction (0=input, 1=output)
    volatile uint32_t PULLUP;       // 0x0C: Pull-up enable
    volatile uint32_t INTERRUPT;    // 0x10: Interrupt status/clear
    volatile uint32_t RESERVED[3];  // 0x14-0x1C: Reserved
} GPIO_TypeDef;

// UART Register Structure
typedef struct {
    volatile uint32_t DATA;         // 0x00: Data register
    volatile uint32_t STATUS;       // 0x04: Status register
    volatile uint32_t CONTROL;      // 0x08: Control register
    volatile uint32_t BAUDRATE;     // 0x0C: Baud rate divisor
} UART_TypeDef;

// Timer Register Structure
typedef struct {
    volatile uint32_t COUNTER;      // 0x00: Current counter value
    volatile uint32_t RELOAD;       // 0x04: Auto-reload value
    volatile uint32_t CONTROL;      // 0x08: Control register
    volatile uint32_t STATUS;       // 0x0C: Status/interrupt flags
} TIMER_TypeDef;

// Hardware register mapping (in real embedded system)
#define GPIO    ((GPIO_TypeDef*)GPIO_BASE_ADDR)
#define UART    ((UART_TypeDef*)UART_BASE_ADDR)
#define TIMER   ((TIMER_TypeDef*)TIMER_BASE_ADDR)

// Status register bit definitions
#define UART_RX_READY   (1 << 0)
#define UART_TX_EMPTY   (1 << 1)
#define UART_ERROR      (1 << 2)

#define TIMER_OVERFLOW  (1 << 0)
#define TIMER_ENABLE    (1 << 0)

/* =============================================================================
 * SECTION 2: Global Variables for Interrupt Scenarios
 * ============================================================================= */

// Variables modified by interrupt service routines MUST be volatile
volatile bool uart_data_ready = false;
volatile uint8_t uart_received_data = 0;
volatile uint32_t timer_overflow_count = 0;
volatile bool system_shutdown = false;

// Variables for demonstration - showing difference with/without volatile
static uint32_t *non_volatile_register = (uint32_t*)0x40020000;
static volatile uint32_t *volatile_register = (volatile uint32_t*)0x40020000;

// Shared flag between threads/interrupts
volatile bool processing_complete = false;

/* =============================================================================
 * SECTION 3: Compiler Optimization Demonstration Functions
 * ============================================================================= */

/**
 * @brief Demonstrates compiler optimization WITHOUT volatile
 * WARNING: This function may create infinite loop with optimization enabled!
 */
void dangerous_polling_without_volatile(void) {
    printf("\n=== DANGEROUS: Polling without volatile ===\n");
    
    uint32_t *status_reg = (uint32_t*)0x40011004;  // UART status register
    uint32_t timeout = 1000000;
    
    printf("Waiting for UART data (without volatile)...\n");
    
    // COMPILER OPTIMIZATION PROBLEM:
    // With -O2, compiler may read *status_reg only once and cache the value
    // If hardware changes the register, this becomes infinite loop!
    while ((*status_reg & UART_RX_READY) == 0 && timeout > 0) {
        timeout--;
        // Compiler might optimize this to:
        // uint32_t cached_value = *status_reg;
        // while ((cached_value & UART_RX_READY) == 0 && timeout > 0)
    }
    
    if (timeout == 0) {
        printf("TIMEOUT: Hardware register never detected as changed!\n");
    } else {
        printf("Data received after %d iterations\n", 1000000 - timeout);
    }
}

/**
 * @brief Demonstrates correct polling WITH volatile
 */
void safe_polling_with_volatile(void) {
    printf("\n=== SAFE: Polling with volatile ===\n");
    
    volatile uint32_t *status_reg = (volatile uint32_t*)0x40011004;
    uint32_t timeout = 1000000;
    
    printf("Waiting for UART data (with volatile)...\n");
    
    // CORRECT BEHAVIOR:
    // volatile forces compiler to read from memory every time
    // Hardware changes are always detected
    while ((*status_reg & UART_RX_READY) == 0 && timeout > 0) {
        timeout--;
        // Each access guaranteed to read from actual memory location
    }
    
    if (timeout == 0) {
        printf("Timeout occurred - no data received\n");
    } else {
        printf("Data received after %d iterations\n", 1000000 - timeout);
    }
}

/**
 * @brief Shows assembly differences between volatile and non-volatile
 */
void demonstrate_assembly_differences(void) {
    printf("\n=== Assembly Code Differences ===\n");
    
    uint32_t regular_var = 42;
    volatile uint32_t volatile_var = 42;
    
    printf("Regular variable operations:\n");
    // Compiler may optimize multiple accesses
    regular_var++;
    regular_var++;  
    regular_var++;
    printf("Regular result: %u\n", regular_var);
    
    printf("Volatile variable operations:\n");  
    // Each operation forces memory access
    volatile_var++;
    volatile_var++;
    volatile_var++;
    printf("Volatile result: %u\n", volatile_var);
    
    // With optimization, regular_var operations might be combined to:
    // regular_var += 3;
    // But volatile_var operations remain separate
}

/* =============================================================================
 * SECTION 4: Memory-Mapped I/O Examples
 * ============================================================================= */

/**
 * @brief GPIO manipulation using memory-mapped registers
 */
void gpio_operations_example(void) {
    printf("\n=== GPIO Memory-Mapped I/O Example ===\n");
    
    // Configure pin 5 as output
    GPIO->DIRECTION |= (1 << 5);
    printf("Configured GPIO pin 5 as output\n");
    
    // Set pin 5 high
    GPIO->OUTPUT |= (1 << 5);
    printf("Set GPIO pin 5 HIGH\n");
    
    // Read input pin 3
    if (GPIO->INPUT & (1 << 3)) {
        printf("GPIO pin 3 is HIGH\n");
    } else {
        printf("GPIO pin 3 is LOW\n");
    }
    
    // Toggle pin 5
    GPIO->OUTPUT ^= (1 << 5);
    printf("Toggled GPIO pin 5\n");
    
    // Clear pin 5
    GPIO->OUTPUT &= ~(1 << 5);
    printf("Set GPIO pin 5 LOW\n");
}

/**
 * @brief UART communication using volatile registers
 */
void uart_communication_example(void) {
    printf("\n=== UART Communication Example ===\n");
    
    // Wait for transmit buffer to be empty
    while (!(UART->STATUS & UART_TX_EMPTY)) {
        // volatile ensures we read status register each time
        // Hardware updates this bit when buffer becomes empty
    }
    
    // Send a byte
    UART->DATA = 'A';
    printf("Sent byte 'A' via UART\n");
    
    // Wait for receive data
    uint32_t timeout = 100000;
    while (!(UART->STATUS & UART_RX_READY) && timeout > 0) {
        timeout--;
        // Without volatile, this might become infinite loop
    }
    
    if (UART->STATUS & UART_RX_READY) {
        uint8_t received = UART->DATA & 0xFF;
        printf("Received byte: 0x%02X ('%c')\n", received, 
               (received >= 32 && received <= 126) ? received : '?');
    } else {
        printf("UART receive timeout\n");
    }
}

/**
 * @brief Timer operations demonstrating volatile register access
 */
void timer_operations_example(void) {
    printf("\n=== Timer Operations Example ===\n");
    
    // Configure timer
    TIMER->RELOAD = 1000;           // Set reload value
    TIMER->CONTROL |= TIMER_ENABLE; // Enable timer
    printf("Timer configured and started\n");
    
    // Wait for timer overflow
    uint32_t initial_count = timer_overflow_count;
    printf("Waiting for timer overflow...\n");
    
    while (timer_overflow_count == initial_count) {
        // volatile ensures we read the actual variable value
        // which is modified by interrupt service routine
        
        // Check timer status register
        if (TIMER->STATUS & TIMER_OVERFLOW) {
            printf("Timer overflow detected in status register\n");
            TIMER->STATUS |= TIMER_OVERFLOW; // Clear flag
            break;
        }
    }
    
    printf("Timer overflow occurred! Count: %u\n", timer_overflow_count);
}

/* =============================================================================
 * SECTION 5: Interrupt Service Routine Simulation
 * ============================================================================= */

/**
 * @brief Simulated UART interrupt service routine
 * In real embedded system, this would be called by hardware interrupt
 */
void UART_IRQHandler(void) {
    // Check if receive interrupt
    if (UART->STATUS & UART_RX_READY) {
        // Read data from hardware register
        uart_received_data = UART->DATA & 0xFF;
        
        // Set flag for main program
        uart_data_ready = true;  // MUST be volatile!
        
        // Clear interrupt flag
        UART->STATUS |= UART_RX_READY;
        
        printf("[ISR] UART data received: 0x%02X\n", uart_received_data);
    }
    
    // Check for errors
    if (UART->STATUS & UART_ERROR) {
        printf("[ISR] UART error detected!\n");
        UART->STATUS |= UART_ERROR; // Clear error flag
    }
}

/**
 * @brief Simulated Timer interrupt service routine
 */
void TIMER_IRQHandler(void) {
    if (TIMER->STATUS & TIMER_OVERFLOW) {
        // Increment overflow counter
        timer_overflow_count++;  // MUST be volatile!
        
        // Clear interrupt flag
        TIMER->STATUS |= TIMER_OVERFLOW;
        
        printf("[ISR] Timer overflow #%u\n", timer_overflow_count);
        
        // Shutdown system after 5 overflows
        if (timer_overflow_count >= 5) {
            system_shutdown = true;  // MUST be volatile!
        }
    }
}

/* =============================================================================
 * SECTION 6: Signal Handler for Demonstration
 * ============================================================================= */

/**
 * @brief Signal handler to simulate hardware interrupts
 */
void signal_handler(int sig) {
    static int call_count = 0;
    call_count++;
    
    if (sig == SIGALRM) {
        if (call_count % 2 == 0) {
            UART_IRQHandler();
        } else {
            TIMER_IRQHandler();
        }
    }
}

/* =============================================================================
 * SECTION 7: Volatile vs Non-Volatile Comparison
 * ============================================================================= */

/**
 * @brief Demonstrates performance impact of volatile
 */
void performance_comparison(void) {
    printf("\n=== Performance Impact Comparison ===\n");
    
    const int iterations = 1000000;
    clock_t start, end;
    
    // Test with regular variable
    uint32_t regular_counter = 0;
    start = clock();
    for (int i = 0; i < iterations; i++) {
        regular_counter++;
        regular_counter--;
        regular_counter += 2;
    }
    end = clock();
    double regular_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Test with volatile variable  
    volatile uint32_t volatile_counter = 0;
    start = clock();
    for (int i = 0; i < iterations; i++) {
        volatile_counter++;
        volatile_counter--;
        volatile_counter += 2;
    }
    end = clock();
    double volatile_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Regular variable time: %.6f seconds\n", regular_time);
    printf("Volatile variable time: %.6f seconds\n", volatile_time);
    printf("Performance overhead: %.2fx slower\n", volatile_time / regular_time);
    printf("Regular final value: %u\n", regular_counter);
    printf("Volatile final value: %u\n", volatile_counter);
}

/**
 * @brief Shows when NOT to use volatile
 */
void when_not_to_use_volatile(void) {
    printf("\n=== When NOT to Use Volatile ===\n");
    
    // DON'T use volatile for regular local variables
    // volatile int unnecessary_volatile = 42;  // WRONG!
    int normal_local = 42;  // CORRECT
    
    // DON'T use volatile for function parameters (usually)
    // void func(volatile int param) { }  // Usually WRONG
    
    // DON'T use volatile for thread synchronization
    // Use proper synchronization primitives instead
    volatile bool bad_thread_flag = false;  // INSUFFICIENT for threading
    
    printf("Local variable (correct): %d\n", normal_local);
    printf("Volatile flag (insufficient for threading): %s\n", 
           bad_thread_flag ? "true" : "false");
    
    printf("Use atomic operations or mutexes for thread safety!\n");
}

/* =============================================================================
 * SECTION 8: Best Practices and Common Mistakes
 * ============================================================================= */

/**
 * @brief Demonstrates volatile best practices
 */
void volatile_best_practices(void) {
    printf("\n=== Volatile Best Practices ===\n");
    
    // 1. Always use volatile for memory-mapped registers
    volatile uint32_t *correct_reg = (volatile uint32_t*)0x40020000;
    
    // 2. Use const volatile for read-only hardware registers
    const volatile uint32_t *readonly_reg = (const volatile uint32_t*)0x40020004;
    
    // 3. Volatile pointers vs pointer to volatile
    volatile uint32_t *ptr_to_volatile;     // Pointer to volatile data
    uint32_t * volatile volatile_ptr;       // Volatile pointer to data  
    volatile uint32_t * volatile both;      // Both pointer and data volatile
    
    // 4. Proper ISR variable declaration
    static volatile bool isr_flag = false;
    static volatile uint32_t isr_counter = 0;
    
    printf("Hardware register access: 0x%08X\n", *correct_reg);
    printf("Read-only register: 0x%08X\n", *readonly_reg);
    printf("ISR flag: %s\n", isr_flag ? "true" : "false");
    printf("ISR counter: %u\n", isr_counter);
}

/**
 * @brief Shows common volatile mistakes
 */
void common_volatile_mistakes(void) {
    printf("\n=== Common Volatile Mistakes ===\n");
    
    // MISTAKE 1: Forgetting volatile on hardware registers
    uint32_t *wrong_reg = (uint32_t*)0x40020000;  // WRONG!
    printf("Wrong register access (may be optimized away)\n");
    
    // MISTAKE 2: Using volatile unnecessarily
    volatile int unnecessary = 42;  // WRONG for normal variables
    printf("Unnecessary volatile overhead: %d\n", unnecessary);
    
    // MISTAKE 3: Assuming volatile provides atomicity
    volatile uint64_t not_atomic = 0;  // WRONG assumption!
    not_atomic++;  // This might not be atomic on 32-bit systems
    printf("Non-atomic 64-bit operation: %llu\n", not_atomic);
    
    // MISTAKE 4: Volatile for thread synchronization
    volatile bool bad_sync = false;  // INSUFFICIENT!
    printf("Bad synchronization flag: %s\n", bad_sync ? "true" : "false");
    
    printf("Remember: volatile != thread-safe!\n");
}

/* =============================================================================
 * SECTION 9: Main Demonstration Function
 * ============================================================================= */

int main(void) {
    printf("=======================================================\n");
    printf("     VOLATILE KEYWORD COMPREHENSIVE DEMONSTRATION\n");
    printf("=======================================================\n");
    
    // Set up signal handler to simulate interrupts
    signal(SIGALRM, signal_handler);
    alarm(1);  // Trigger signal in 1 second
    
    // Initialize simulated hardware registers with dummy values
    // In real system, these would be actual hardware addresses
    static uint32_t simulated_gpio[8] = {0};
    static uint32_t simulated_uart[4] = {UART_TX_EMPTY, 0, 0, 9600};
    static uint32_t simulated_timer[4] = {0, 1000, TIMER_ENABLE, 0};
    
    // Point our register pointers to simulated memory
    // (In real embedded system, these would be actual hardware addresses)
    
    printf("Compiler: %s\n", __VERSION__);
    printf("Compilation date: %s %s\n", __DATE__, __TIME__);
    
    // Run demonstrations
    demonstrate_assembly_differences();
    performance_comparison();
    gpio_operations_example();
    uart_communication_example();
    timer_operations_example();
    volatile_best_practices();
    common_volatile_mistakes();
    when_not_to_use_volatile();
    
    // Demonstrate interrupt handling
    printf("\n=== Interrupt Handling Demonstration ===\n");
    printf("Waiting for interrupts (simulated via signals)...\n");
    
    int wait_count = 0;
    while (!system_shutdown && wait_count < 10) {
        if (uart_data_ready) {
            printf("Main: Processing UART data: 0x%02X\n", uart_received_data);
            uart_data_ready = false;  // Clear flag
        }
        
        if (processing_complete) {
            printf("Main: Processing completed\n");
            processing_complete = false;
        }
        
        printf("Main loop iteration %d (timer overflows: %u)\n", 
               wait_count, timer_overflow_count);
        
        sleep(1);  // Simulate work
        wait_count++;
        alarm(1);  // Trigger next interrupt
    }
    
    printf("\n=== Demonstration Complete ===\n");
    printf("Key Takeaways:\n");
    printf("1. Use volatile for hardware registers\n");
    printf("2. Use volatile for ISR-modified variables\n");
    printf("3. Don't use volatile for normal variables\n");
    printf("4. Volatile != thread-safe\n");
    printf("5. Volatile prevents compiler optimizations\n");
    printf("6. Always test with optimization enabled!\n");
    
    return 0;
}