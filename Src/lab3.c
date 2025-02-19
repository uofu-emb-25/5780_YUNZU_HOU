#include <stm32f0xx_it.h>
#include <assert.h>
#include <hal_gpio.h>
#include <pwm_config.h>
#include <main.h>

// Define global TIM handle
TIM_HandleTypeDef htim2;

int lab3_main(void) {
    // Initialize HAL and system clock
    HAL_Init();
    SystemClock_Config();

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Initialize PC6 and PC7 as PWM output pins
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;   // PC6, PC7
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;         // Alternate function mode (for PWM)
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;      // TIM3_CH1 & TIM3_CH2 alternate mapping
    My_HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Initialize TIM3 to enable PWM
    TIM3_PWM_Init();

    // Start TIM3
    TIM3->CR1 |= TIM_CR1_CEN;

    // Infinite loop (PWM brightness adjustment is handled automatically)
    while (1) {
        // No operations needed in the main loop
    }
}
