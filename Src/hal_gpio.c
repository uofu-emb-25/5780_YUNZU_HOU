#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
/*
void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
}
*/

/*
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}
*/

/*
GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return -1;
}
*/

/*
void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
}
*/

/*
void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
}
*/
void Configure_EXTI0(void)
{
    // 1. 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 2. 配置 PA0 为输入模式，启用内部下拉电阻
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // 上升沿触发
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 配置 EXTI（外部中断）
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0); // 设置 EXTI0 优先级
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);         // 使能 EXTI0 中断
}

void Configure_SYSCFG(void)
{
    // 1. 使能 SYSCFG 时钟
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // 2. 将 PA0 映射到 EXTI0
    SYSCFG->EXTICR[0] &= ~(0xF << 0);  // 清除 EXTI0 的配置位
    SYSCFG->EXTICR[0] |= (0x0 << 0);   // 选择 PA0（0b0000）

    // 3. 确保 EXTI0 使能
    EXTI->IMR |= EXTI_IMR_IM0;         // 使能 EXTI0
    EXTI->RTSR |= EXTI_RTSR_RT0;       // 配置为上升沿触发
}
