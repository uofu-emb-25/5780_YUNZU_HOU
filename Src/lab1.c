#include <stm32f0xx_hal.h>
#include <assert.h>  // Include assert.h for assertions

int lab1_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config();  // Configure the system clock

    // Enable GPIOC clock in RCC
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Assertion: Check if GPIOC clock is enabled in RCC
    assert((RCC->AHBENR & RCC_AHBENR_GPIOCEN) != 0);

    // Configure GPIOC 8 & 9
    GPIO_InitTypeDef initStr = {
        GPIO_PIN_8 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL
    };
    HAL_GPIO_Init(GPIOC, &initStr);

    // Assertion: Check if GPIOC->MODER is correctly set to output mode
    assert((GPIOC->MODER & (0b01 << (8 * 2))) != 0);  // PC8 should be output
    assert((GPIOC->MODER & (0b01 << (9 * 2))) != 0);  // PC9 should be output

    // Assertion: Check if GPIOC->PUPDR is set to no pull-up/pull-down
    assert((GPIOC->PUPDR & (0b00 << (8 * 2))) != 0);
    assert((GPIOC->PUPDR & (0b00 << (9 * 2))) != 0);

    // Set PC8 to high
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    // Assertion: Check if GPIOC->ODR is correctly set for PC8 to high
    assert((GPIOC->ODR & GPIO_PIN_8) != 0);  // PC8 should be high

    while (1) {
        HAL_Delay(200);  // Delay for 200ms
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);

        // Assertion: Ensure GPIO state toggles correctly
        assert((GPIOC->ODR & (GPIO_PIN_8 | GPIO_PIN_9)) == 0 || 
               (GPIOC->ODR & (GPIO_PIN_8 | GPIO_PIN_9)) == (GPIO_PIN_8 | GPIO_PIN_9));
    }
}
