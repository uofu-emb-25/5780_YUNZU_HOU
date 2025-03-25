#include "stm32f0xx_hal.h"
#include "main.h"
#include <stdint.h>

// I3G4250D Definitions
#define GYRO_ADDR    (0xD2)   // 7-bit address 0x69 shifted left (0x69 << 1)
#define WHO_AM_I_REG 0x0F
#define WHO_AM_I_VAL 0xD3

#define GREEN_LED GPIO_PIN_9
#define RED_LED   GPIO_PIN_6

int lab5_main(void) {
    HAL_Init();
    SystemClock_Config();

    // -------------------- GPIO Clock Enable --------------------
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

    // -------------------- Configure PC6, PC9 for LED output --------------------
    GPIO_InitTypeDef led_config = {
        .Pin = RED_LED | GREEN_LED,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Pull = GPIO_NOPULL
    };
    HAL_GPIO_Init(GPIOC, &led_config);

    // -------------------- Configure PB14, PC0 as Output High --------------------
    GPIOB->MODER |= GPIO_MODER_MODER14_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_14;
    GPIOB->BSRR = GPIO_PIN_14;

    GPIOC->MODER |= GPIO_MODER_MODER0_0;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_0;
    GPIOC->BSRR = GPIO_PIN_0;

    // -------------------- Configure PB11 (SDA) and PB13 (SCL) --------------------
    GPIOB->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER13);
    GPIOB->MODER |= (GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1); // AF mode
    GPIOB->OTYPER |= (1 << 11) | (1 << 13); // Open-drain
    GPIOB->AFR[1] |= (1 << ((11 - 8) * 4)); // PB11 -> AF1 (SDA)
    GPIOB->AFR[1] |= (5 << ((13 - 8) * 4)); // PB13 -> AF5 (SCL)

    // -------------------- Enable I2C2 Clock and Configure Timing --------------------
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    I2C2->TIMINGR = 0x00201D2B;  // 100kHz @ 8MHz input clock
    I2C2->CR1 |= I2C_CR1_PE;

    HAL_Delay(10);  // Small delay to allow sensor boot-up

    // -------------------- Step 1: Write WHO_AM_I Register Address --------------------
    I2C2->CR2 = (GYRO_ADDR & ~I2C_CR2_RD_WRN)     // Write mode
              | (1 << 16)                         // NBYTES = 1
              | I2C_CR2_START;                    // Generate START

    while (!(I2C2->ISR & I2C_ISR_TXIS));          // Wait for TX ready
    I2C2->TXDR = WHO_AM_I_REG;                    // Send register address
    while (!(I2C2->ISR & I2C_ISR_TC));            // Wait for transfer complete

    // -------------------- Step 2: Read 1 Byte from Sensor --------------------
    I2C2->CR2 = GYRO_ADDR
              | I2C_CR2_RD_WRN                    // Read mode
              | (1 << 16)                         // NBYTES = 1
              | I2C_CR2_START;                    // Generate RESTART

    while (!(I2C2->ISR & I2C_ISR_RXNE));          // Wait for RX ready
    uint8_t whoami = I2C2->RXDR;                  // Read data
    while (!(I2C2->ISR & I2C_ISR_TC));            // Wait for transfer complete
    I2C2->CR2 |= I2C_CR2_STOP;                    // Generate STOP

    // -------------------- Step 3: Check WHO_AM_I --------------------
    if (whoami == WHO_AM_I_VAL) {
        HAL_GPIO_WritePin(GPIOC, GREEN_LED, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOC, RED_LED, GPIO_PIN_SET);
    }

    while (1);  // Keep LED state
}
