#include "stm32f0xx_hal.h"
#include <string.h>

// **USART3: TX = PB10, RX = PB11**
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim3;

// **LED 连接引脚**
#define LED_RED_PIN    GPIO_PIN_6  // PC6
#define LED_BLUE_PIN   GPIO_PIN_7  // PC7
#define LED_ORANGE_PIN GPIO_PIN_8  // PC8
#define LED_GREEN_PIN  GPIO_PIN_9  // PC9
#define LED_PORT       GPIOC

// **全局变量**
volatile uint16_t blinking_leds = 0;  // 当前正在闪烁的多个 LED

// **函数声明**
void SystemClock_Config(void);
void GPIO_Init(void);
void UART3_Init(void);
void TIM3_Init(void);
void Transmit_String(const char* str);
void Keystroke_Led_Toggle(void);
void Handle_LED_Command(char color, char action);
void TIM3_IRQHandler(void);

// **实验主函数**
int lab4_main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    UART3_Init();
    TIM3_Init();

    Transmit_String("USART3 Initialized. Type 'r', 'g', 'b', 'o' + '0/1/2' to control LEDs.\n");

    while (1) {
        Keystroke_Led_Toggle();
    }
}

/* -------------------------------------------------------
   GPIO_Init: 初始化 LED (PC6, PC7, PC8, PC9)
------------------------------------------------------- */
void GPIO_Init(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE(); // 使能 GPIOC 时钟

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_RED_PIN | LED_BLUE_PIN | LED_ORANGE_PIN | LED_GREEN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // **默认关闭所有 LED**
    HAL_GPIO_WritePin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_ORANGE_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
}

/* -------------------------------------------------------
   UART3_Init: 初始化 USART3 (PB10, PB11)
------------------------------------------------------- */
void UART3_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();   // 使能 GPIOB 时钟
    __HAL_RCC_USART3_CLK_ENABLE();  // 使能 USART3 时钟

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置 PB10 (TX) 和 PB11 (RX)
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 配置 USART3
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart3);
}

/* -------------------------------------------------------
   TIM3_Init: 初始化 TIM3 作为 LED 闪烁定时器
------------------------------------------------------- */
void TIM3_Init(void) {
    __HAL_RCC_TIM3_CLK_ENABLE();  // **使能 TIM3 时钟**

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 4800 - 1;  // **48MHz / 4800 = 10kHz**
    htim3.Init.Period = 5000 - 1;     // **10kHz / 5000 = 2Hz (每 0.5 秒闪烁一次)**
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&htim3);

    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

/* -------------------------------------------------------
   TIM3_IRQHandler: 定时器中断处理函数 (LED 翻转)
------------------------------------------------------- */
void TIM3_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim3);
    
    if (blinking_leds) {
        HAL_GPIO_TogglePin(LED_PORT, blinking_leds);
    }
}

/* -------------------------------------------------------
   Transmit_String: 通过 USART3 发送字符串
------------------------------------------------------- */
void Transmit_String(const char* str) {
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/* -------------------------------------------------------
   Keystroke_Led_Toggle: 接收用户输入并控制 LED
------------------------------------------------------- */
void Keystroke_Led_Toggle() {
    char received_data[2];

    Transmit_String("Enter color (r, g, b, o) + mode (0=off, 1=on, 2=toggle): ");

    HAL_UART_Receive(&huart3, (uint8_t*)received_data, 2, HAL_MAX_DELAY);

    Transmit_String("\nReceived: ");
    HAL_UART_Transmit(&huart3, (uint8_t*)received_data, 2, HAL_MAX_DELAY);
    Transmit_String("\n");

    Handle_LED_Command(received_data[0], received_data[1]);
}

/* -------------------------------------------------------
   Handle_LED_Command: 解析命令并控制多个 LED
------------------------------------------------------- */
void Handle_LED_Command(char color, char action) {
    uint16_t led_pin = 0;
    
    if (color == 'r') led_pin = LED_RED_PIN;
    else if (color == 'b') led_pin = LED_BLUE_PIN;
    else if (color == 'o') led_pin = LED_ORANGE_PIN;
    else if (color == 'g') led_pin = LED_GREEN_PIN;
    else {
        Transmit_String("Invalid LED color!\n");
        return;
    }

    if (action == '0') {
        HAL_GPIO_WritePin(LED_PORT, led_pin, GPIO_PIN_RESET);
        blinking_leds &= ~led_pin;  // **从闪烁列表中移除该 LED**
        if (blinking_leds == 0) {
            HAL_TIM_Base_Stop_IT(&htim3);  // **如果没有 LED 在闪烁，则停止定时器**
        }
        Transmit_String("LED OFF\n");
    } else if (action == '1') {
        HAL_GPIO_WritePin(LED_PORT, led_pin, GPIO_PIN_SET);
        blinking_leds &= ~led_pin;  // **确保该 LED 不闪烁**
        if (blinking_leds == 0) {
            HAL_TIM_Base_Stop_IT(&htim3);
        }
        Transmit_String("LED ON\n");
    } else if (action == '2') {
        blinking_leds |= led_pin;  // **添加 LED 进入闪烁列表**
        HAL_TIM_Base_Start_IT(&htim3);  // **开启定时器**
        Transmit_String("LED FLASHING\n");
    } else {
        Transmit_String("Invalid action!\n");
    }
}
