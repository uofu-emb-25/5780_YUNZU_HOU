#include <stm32f0xx_it.h>
#include <assert.h>
#include <hal_gpio.h>
#include <pwm_config.h>
#include <main.h>

// Define global TIM handle
TIM_HandleTypeDef htim2;

void Configure_PC6_PC7_AF(void) {
    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC6 and PC7 as Alternate Function (AF1 for TIM3)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // Alternate Function Push-Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;   // Select AF1 (TIM3 Channels)
    My_HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int lab3_main(void) {
    // Initialize HAL and system clock
    HAL_Init();
    SystemClock_Config();

    // Configure PC6 and PC7 for TIM3 PWM alternate function
    Configure_PC6_PC7_AF();

    // Initialize TIM3 for PWM
    TIM3_PWM_Init();

    // Start TIM3
    TIM3->CR1 |= TIM_CR1_CEN;

    // Infinite loop (PWM brightness control is automatic)
    while (1) {
        // No main loop operations needed
    }
}