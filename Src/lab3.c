#include <stm32f0xx_it.h>
#include <assert.h>
#include <hal_gpio.h>
#include <main.h>

// Define global TIM handle
TIM_HandleTypeDef htim2;

int lab3_main(void) {
    // Initialize HAL and system clock
    HAL_Init();
    SystemClock_Config();

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Define GPIO initialization structure
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;   // PC8, PC9
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // Push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;              // No pull-up or pull-down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;     // Low speed mode

    // Initialize PC8 and PC9 as LED output
    My_HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Turn on PC8 initially
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

    // **Initialize TIM2 (4Hz interrupt)**
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // Enable TIM2 clock
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 999;    // 8MHz / (999+1) = 8kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1999;      // 8kHz / (1999+1) = 4Hz (0.25s interrupt)
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);

    // Enable TIM2 update interrupt
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->SR &= ~TIM_SR_UIF;  // Clear any pending interrupt flags

    // Set NVIC priority and enable TIM2 interrupt
    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Start TIM2
    TIM2->CR1 |= TIM_CR1_CEN;

    // **Infinite loop (LED toggling is handled in the interrupt routine)**
    while (1) {
        // No operations needed in the main loop
    }
}

// **TIM2 Interrupt Handler**
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // Check update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF;  // Clear interrupt flag

        // Toggle LED states
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    }
}
