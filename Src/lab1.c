#include <stm32f0xx_hal.h>

int lab1_main(void) {
    HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); // Configure the system clock

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 

    // Configure PC6, PC7 as output mode
    GPIOC->MODER &= ~(3 << (6 * 2)); // Clear mode bits for PC6
    GPIOC->MODER &= ~(3 << (7 * 2)); // Clear mode bits for PC7
    GPIOC->MODER |= (1 << (6 * 2));  // Set PC6 as output
    GPIOC->MODER |= (1 << (7 * 2));  // Set PC7 as output

    // Set initial LED states: PC6 ON, PC7 OFF
    GPIOC->BSRR = (1 << 6);  // Set PC6 (turn on red LED)
    GPIOC->BSRR = (1 << (7 + 16));  // Reset PC7 (turn off blue LED)

    // Infinite loop for blinking LEDs
    while (1) {
        HAL_Delay(200);  // Delay 200ms
        GPIOC->ODR ^= (1 << 6) | (1 << 7); // Toggle PC6 and PC7
    }
}
