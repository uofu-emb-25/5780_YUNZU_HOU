#include "pwm_config.h"

// 定义 TIM3 句柄
TIM_HandleTypeDef htim3;

void TIM3_PWM_Init(void) {
    // 在 RCC 中使能 TIM3 时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 设置预分频器和 ARR 以生成 800Hz PWM
    // 时钟频率 = 8MHz，选择预分频器 = 9 → 定时器时钟 = 800kHz
    // ARR = (800kHz / 800Hz) - 1 = 999
    TIM3->PSC = 9;        // 预分频器将 8MHz 降低到 800kHz
    TIM3->ARR = 999;      // 设置 ARR 以匹配 800Hz PWM

    // 在 CCMR1 中设置通道 1 和 2 为 PWM 模式
    TIM3->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC2M);   // 清除 OC1M 和 OC2M 位
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  // 设置通道 1 为 PWM 模式 2 (110)
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);  // 设置通道 2 为 PWM 模式 1 (110)

    // 使能输出比较预装载
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

    // 在 CCER 中使能通道 1 和 2 的输出
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

    // 设置初始占空比为 ARR 的 20%
    TIM3->CCR1 = (50 * TIM3->ARR) / 100;  // 20% 占空比
    TIM3->CCR2 = (50 * TIM3->ARR) / 100;  // 20% 占空比

    // 使能 TIM3 (但暂时不启动)
    TIM3->CR1 |= TIM_CR1_ARPE;  // 使能自动重装载预装载
}
