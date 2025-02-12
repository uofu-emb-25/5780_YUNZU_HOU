#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    for (uint8_t pin = 0; pin < 16; pin++)
    {
        if (GPIO_Init->Pin & (1 << pin))  // 仅对选中的引脚进行配置
        {
            GPIOx->MODER &= ~(0b11 << (2 * pin));  // 清除模式位
            GPIOx->MODER |= (GPIO_Init->Mode << (2 * pin));  // 设置模式

            if (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD)
                GPIOx->OTYPER |= (1 << pin); 

            GPIOx->OSPEEDR &= ~(0b11 << (2 * pin));
            GPIOx->OSPEEDR |= (GPIO_Init->Speed << (2 * pin));

            GPIOx->PUPDR &= ~(0b11 << (2 * pin));
            GPIOx->PUPDR |= (GPIO_Init->Pull << (2 * pin));
        }
    }
}



void My_HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
    for (uint8_t pin = 0; pin < 16; pin++)
    {
        if (GPIO_Pin & (1 << pin))  // 仅清除目标引脚
        {
            GPIOx->MODER &= ~(0b11 << (2 * pin));
            GPIOx->OTYPER &= ~(1 << pin);
            GPIOx->OSPEEDR &= ~(0b11 << (2 * pin));
            GPIOx->PUPDR &= ~(0b11 << (2 * pin));
        }
    }
}




GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return (GPIOx->IDR & GPIO_Pin) ? 1 : 0;
}




void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    GPIOx->BSRR = (PinState == GPIO_PIN_SET) ? GPIO_Pin : (GPIO_Pin << 16);
}




void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;
}

void EXTI_rising_edge_trigger(void){
    EXTI->IMR |= 0x0001;
    EXTI->RTSR |= 0x0001; 
}

void SYSCFG_setup(void){
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
}