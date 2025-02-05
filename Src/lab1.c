#include <stm32f0xx_hal.h>

int lab1_main(void) {
    HAL_Init();  // Initialize the HAL library
    SystemClock_Config();  // Configure the system clock

    // 1️⃣ Enable the clocks for GPIOC and GPIOA (PC6, PC7 for LEDs, PA0 for the button)
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;

    // 2️⃣ Configure PC6 (Red LED) and PC7 (Blue LED) as output mode
    GPIOC->MODER &= ~(3 << (6 * 2));  // Clear mode bits for PC6
    GPIOC->MODER &= ~(3 << (7 * 2));  // Clear mode bits for PC7
    GPIOC->MODER |= (1 << (6 * 2));   // Set PC6 as output
    GPIOC->MODER |= (1 << (7 * 2));   // Set PC7 as output

    // 3️⃣ Configure PA0 (button) as input mode
    GPIOA->MODER &= ~(3 << (0 * 2));  // Clear mode bits for PA0 (default input mode)

    // 4️⃣ Enable the pull-up resistor for PA0 (prevents floating state)
    GPIOA->PUPDR &= ~(3 << (0 * 2));  // Clear PUPDR for PA0
    GPIOA->PUPDR |= (1 << (0 * 2));   // Set PA0 as pull-up mode

    // 5️⃣ Set initial LED states (Red LED ON, Blue LED OFF)
    GPIOC->BSRR = (1 << 6);        // Turn ON PC6 (Red LED)
    GPIOC->BSRR = (1 << (7 + 16)); // Turn OFF PC7 (Blue LED)

    // 6️⃣ Main loop
    while (1) {
        if (GPIOA->IDR & (1 << 0)) {  // Read PA0 button state
            // Button not pressed (High level)
            GPIOC->BSRR = (1 << 6);        // Turn ON Red LED (PC6)
            GPIOC->BSRR = (1 << (7 + 16)); // Turn OFF Blue LED (PC7)
        } else {
            // Button pressed (Low level)
            GPIOC->BSRR = (1 << 7);        // Turn ON Blue LED (PC7)
            GPIOC->BSRR = (1 << (6 + 16)); // Turn OFF Red LED (PC6)
        }
    }
}
