#include <stm32f0xx_hal.h>
#include "main.h"
#include <assert.h>

#define GYRO_ADDRESS   (0x69 << 1)
#define WHO_AM_I_REG   0x0F
#define WHO_AM_I_VAL   0xD3

#define RED     GPIO_PIN_6
#define BLUE    GPIO_PIN_7
#define ORANGE  GPIO_PIN_8
#define GREEN   GPIO_PIN_9

#define TILT_THRESHOLD 2000
#define OUT_X_L        (0x28 | 0x80)

void gyro_init(void);
void gyro_configure_measurement_mode(void);
void gyro_write_who_am_i_register(void);
void gyro_read_who_am_i_register(void);
void gyro_read_and_display_tilt(void);

int lab5_main(void) {
    HAL_Init();
    SystemClock_Config();

    // Enable GPIOB and GPIOC clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOBEN;

    // Configure LED pins
    GPIO_InitTypeDef ledInit = {RED | BLUE | ORANGE | GREEN, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &ledInit);

    // Set PB14 and PC0 high for I2C interface configuration
    GPIOB->MODER |= GPIO_MODER_MODER14_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_14;
    GPIOB->BSRR = GPIO_PIN_14;

    GPIOC->MODER |= GPIO_MODER_MODER0_0;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_0;
    GPIOC->BSRR = GPIO_PIN_0;

    // Configure I2C2 SDA (PB11) and SCL (PB13)
    GPIOB->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER13);
    GPIOB->MODER |= (GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1);
    GPIOB->OTYPER |= (1 << 11) | (1 << 13);
    GPIOB->AFR[1] |= (1 << ((11 - 8) * 4)) | (5 << ((13 - 8) * 4));

    // Enable I2C2 and configure timing
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    I2C2->TIMINGR = 0x00201D2B;
    I2C2->CR1 |= I2C_CR1_PE;

    // Verify WHO_AM_I and configure sensor
    gyro_write_who_am_i_register();
    HAL_Delay(1000);
    gyro_configure_measurement_mode();
    HAL_Delay(1000);

    while (1) {
        HAL_GPIO_WritePin(GPIOC, RED | BLUE | GREEN | ORANGE, GPIO_PIN_RESET);
        gyro_read_and_display_tilt();
        HAL_Delay(1000);
    }
}

void gyro_write_who_am_i_register(void) {
    uint32_t timeout;
    I2C2->CR2 = (GYRO_ADDRESS & ~(1 << I2C_CR2_RD_WRN_Pos)) | (1 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_START_Pos);
    timeout = 100000;
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) && --timeout);
    if (timeout == 0 || (I2C2->ISR & I2C_ISR_NACKF)) {
        I2C2->ICR = I2C_ICR_NACKCF;
        I2C2->CR2 |= I2C_CR2_STOP;
        HAL_GPIO_WritePin(GPIOC, RED, GPIO_PIN_SET);
        return;
    }
    I2C2->TXDR = WHO_AM_I_REG;
    timeout = 100000;
    while (!(I2C2->ISR & I2C_ISR_TC) && --timeout);
    if (timeout == 0) {
        I2C2->CR2 |= I2C_CR2_STOP;
        HAL_GPIO_WritePin(GPIOC, RED, GPIO_PIN_SET);
        return;
    }
    gyro_read_who_am_i_register();
}

void gyro_read_who_am_i_register(void) {
    uint32_t timeout;
    I2C2->CR2 = GYRO_ADDRESS | (1 << I2C_CR2_RD_WRN_Pos) | (1 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_START_Pos);
    timeout = 100000;
    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF)) && --timeout);
    if (timeout == 0 || (I2C2->ISR & I2C_ISR_NACKF)) {
        HAL_GPIO_WritePin(GPIOC, RED, GPIO_PIN_SET);
        I2C2->ICR = I2C_ICR_NACKCF;
        return;
    }
    uint8_t received = I2C2->RXDR;
    if (received == WHO_AM_I_VAL) {
        HAL_GPIO_WritePin(GPIOC, GREEN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOC, ORANGE, GPIO_PIN_SET);
    }
    I2C2->CR2 |= I2C_CR2_STOP;
}

void gyro_configure_measurement_mode(void) {
    uint8_t config = 0x0F;
    uint32_t timeout;
    I2C2->CR2 = (GYRO_ADDRESS & ~(1 << I2C_CR2_RD_WRN_Pos)) | (2 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_START_Pos);
    timeout = 100000;
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) && --timeout);
    if (timeout == 0 || (I2C2->ISR & I2C_ISR_NACKF)) {
        I2C2->ICR = I2C_ICR_NACKCF;
        I2C2->CR2 |= I2C_CR2_STOP;
        return;
    }
    I2C2->TXDR = 0x20;
    timeout = 100000;
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) && --timeout);
    if (timeout == 0 || (I2C2->ISR & I2C_ISR_NACKF)) {
        I2C2->ICR = I2C_ICR_NACKCF;
        I2C2->CR2 |= I2C_CR2_STOP;
        return;
    }
    I2C2->TXDR = config;
    timeout = 100000;
    while (!(I2C2->ISR & I2C_ISR_TC) && --timeout);
    I2C2->CR2 |= I2C_CR2_STOP;
}

void gyro_read_and_display_tilt(void) {
    uint8_t buffer[6];
    uint32_t timeout;
    uint8_t idx = 0;

    I2C2->CR2 = (GYRO_ADDRESS & ~(1 << I2C_CR2_RD_WRN_Pos)) | (1 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_START_Pos);
    timeout = 100000;
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) && --timeout);
    if (timeout == 0 || (I2C2->ISR & I2C_ISR_NACKF)) {
        I2C2->ICR = I2C_ICR_NACKCF;
        I2C2->CR2 |= I2C_CR2_STOP;
        return;
    }
    I2C2->TXDR = OUT_X_L;
    timeout = 100000;
    while (!(I2C2->ISR & I2C_ISR_TC) && --timeout);
    I2C2->CR2 |= I2C_CR2_STOP;

    I2C2->CR2 = GYRO_ADDRESS | (1 << I2C_CR2_RD_WRN_Pos) | (6 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_START_Pos);
    timeout = 100000;
    idx = 0;
    while (idx < 6) {
        while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF)) && --timeout);
        if (timeout == 0) break;
        buffer[idx++] = I2C2->RXDR;
    }
    I2C2->CR2 |= I2C_CR2_STOP;

    int16_t x = (int16_t)((buffer[1] << 8) | buffer[0]);
    int16_t y = (int16_t)((buffer[3] << 8) | buffer[2]);
    int16_t z = (int16_t)((buffer[5] << 8) | buffer[4]);

    HAL_GPIO_WritePin(GPIOC, RED | BLUE | GREEN | ORANGE, GPIO_PIN_RESET);
    if (x > TILT_THRESHOLD) HAL_GPIO_WritePin(GPIOC, RED, GPIO_PIN_SET);
    if (y > TILT_THRESHOLD) HAL_GPIO_WritePin(GPIOC, BLUE, GPIO_PIN_SET);
    if (z > TILT_THRESHOLD) HAL_GPIO_WritePin(GPIOC, GREEN, GPIO_PIN_SET);
}
