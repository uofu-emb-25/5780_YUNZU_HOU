#include "stm32f0xx.h"

// 定义 USART3 TX (PB10), RX (PB11) 引脚
#define USART3_TX_PIN 10
#define USART3_RX_PIN 11
#define USART3_AF_NUM 4 // USART3的 AF 号

// 函数声明
void USART3_Init(void);
void USART3_SendChar(char c);
void USART3_SendString(const char* str);
char USART3_ReceiveChar(void);

// 实验主函数
int lab4_main(void) {
    USART3_Init(); // 初始化 USART3

    USART3_SendString("USART3 Initialized!\r\n");

    while (1) {
        char received = USART3_ReceiveChar(); // 接收字符
        USART3_SendChar(received); // 回显收到的字符
    }

    return 0;
}

// 初始化 USART3
void USART3_Init(void) {
    // 1. 使能 GPIOB 时钟（TX: PB10, RX: PB11）
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // 2. 使能 USART3 时钟
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // 3. 配置 PB10 (TX) 和 PB11 (RX) 为复用模式
    GPIOB->MODER &= ~((3 << (USART3_TX_PIN * 2)) | (3 << (USART3_RX_PIN * 2))); // 清零
    GPIOB->MODER |= (2 << (USART3_TX_PIN * 2)) | (2 << (USART3_RX_PIN * 2)); // 设置为复用模式

    // 4. 配置 AF（备用功能编号）
    GPIOB->AFR[1] |= (USART3_AF_NUM << ((USART3_TX_PIN - 8) * 4)); // TX: AF4
    GPIOB->AFR[1] |= (USART3_AF_NUM << ((USART3_RX_PIN - 8) * 4)); // RX: AF4

    // 5. 配置 USART3 波特率
    USART3->BRR = SystemCoreClock / 115200;

    // 6. 使能 USART3 的发送和接收
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // 7. 使能 USART3
    USART3->CR1 |= USART_CR1_UE;
}

// 发送单个字符
void USART3_SendChar(char c) {
    while (!(USART3->ISR & USART_ISR_TXE)); // 等待 TXE 置位（发送寄存器为空）
    USART3->TDR = c; // 发送字符
}

// 发送字符串
void USART3_SendString(const char* str) {
    while (*str) {
        USART3_SendChar(*str++);
    }
}

// 接收单个字符
char USART3_ReceiveChar(void) {
    while (!(USART3->ISR & USART_ISR_RXNE)); // 等待 RXNE 置位（接收寄存器非空）
    return USART3->RDR; // 读取数据
}
