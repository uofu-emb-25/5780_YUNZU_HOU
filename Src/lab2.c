#include <stm32f0xx_it.h>
#include <assert.h>
#include <hal_gpio.h>
#include <main.h>

volatile int iCount = 0;  // 交给主循环处理的长任务

/**
 * @brief  EXTI0_1 中断处理函数
 */
void EXTI0_1_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {  // 确保是 EXTI0 触发
        EXTI->PR |= (1 << 0);   // **立即清除中断标志**
        
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
        iCount = 1500000;  
    }
}

/**
 * @brief  Lab2 主函数
 */
int lab2_main(void) {
    // 系统时钟配置
    SystemClock_Config();

    // 使能 GPIO 和 SYSCFG 时钟
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    My_HAL_RCC_GPIOA_CLK_ENABLE();
    My_HAL_RCC_SYSCFG_CLK_ENABLE();

    // 初始化 PC6, PC7, PC8, PC9 为输出（LED）
    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};
    My_HAL_GPIO_Init(GPIOC, &initStr);

    // 初始化 PA0 作为输入（用户按钮）
    GPIO_InitTypeDef initStr2 = {
        GPIO_PIN_0,
        GPIO_MODE_INPUT,
        GPIO_SPEED_FREQ_LOW,
        GPIO_PULLDOWN
    };
    My_HAL_GPIO_Init(GPIOA, &initStr2);

    // 确保 EXTI0 配置正确
    assert((EXTI->RTSR & 0x1) == 0);
    EXTI_rising_edge_trigger();
    assert((EXTI->RTSR & 0x1) == 1);

    // 配置 NVIC
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    //Test for LAB2.7
    NVIC_SetPriority(EXTI0_1_IRQn, 1);   // **降低 EXTI0_1 优先级**
    NVIC_SetPriority(SysTick_IRQn, 2);   // **确保 SysTick 有最高优先级**
    //NVIC_SetPriority(EXTI0_1_IRQn, 2);   // **降低 EXTI0_1 优先级**

    // 连接 PA0 到 EXTI0
    SYSCFG_setup();
    assert((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0) == 0);

    // **初始化 LED 状态**
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_9, GPIO_PIN_SET);

    uint32_t last_toggle = HAL_GetTick(); // **记录上次 LED 变换时间**

    while(1) {
        // **非阻塞 LED 闪烁**
        if (HAL_GetTick() - last_toggle >= 400) {
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            last_toggle = HAL_GetTick();
        }

        // **处理长任务**
        if (iCount > 0) {
            iCount--;
            if (iCount == 0) {
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
            }
        }
    }
}
