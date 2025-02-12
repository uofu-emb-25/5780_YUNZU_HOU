#include "stm32f0xx_hal.h"
#include "main.h"

int lab2_main(void)
{
    // 1. 初始化 HAL
    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // 2. 配置 GPIO（PC6, PC7, PC8, PC9 作为 LED）
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 3. 预测 SYSCFG 预期值（断言）
    assert((SYSCFG->EXTICR[0] & 0xF) == 0x0); // PA0 预期连接到 EXTI0

    // 4. 配置 SYSCFG 连接 PA0 到 EXTI0
    Configure_SYSCFG();

    // 5. 验证 SYSCFG 是否正确配置
    assert((SYSCFG->EXTICR[0] & 0xF) == 0x0); // 断言 PA0 仍然映射到 EXTI0

    // 6. 配置 EXTI0（PA0 按键中断）
    Configure_EXTI0();

    // 7. 进入主循环
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);  // 500ms 切换 PC6（红色 LED）
        HAL_Delay(500);
    }
}
