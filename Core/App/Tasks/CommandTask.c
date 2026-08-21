#include "LEDType.h"
#include "cmsis_os2.h"
#include "main.h"
#include "stm32f103xb.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

char UartStr[16];

void StartCommandTask(void *argument) {
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)UartStr, sizeof(UartStr));
  for (;;) {
    char str[16];
    osMessageQueueGet(UARTStrQueueHandle, str, 0, osWaitForever);

    OLED_ShowString(4, 1, "                ");
    OLED_ShowString(4, 1, str);

    // "LED1ON", "LED2OFF"
    LEDCmdType cmd = {0xFF,0xFF};
    switch (str[3]) {
      case '1':
      cmd.num = LED_1;
      break;
      case '2':
      cmd.num = LED_2;
      break;
      case '3':
      cmd.num = LED_3;
      break;
    }
    if (str[4]=='O' && str[5]=='N'){
      cmd.state = LED_State_On;
    }else if (str[4]=='O' && str[5]=='F' && str[6]=='F') {
      cmd.state = LED_State_Off;
    }
    osMessageQueuePut(LEDCmdQueueHandle, &cmd, 0, osWaitForever);
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
    char temp[16];
    memset(temp, 0, sizeof(temp));
    memcpy(temp, UartStr, len);

    osMessageQueuePut(UARTStrQueueHandle, UartStr, 0, 0);

    memset(UartStr, 0, sizeof(UartStr));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)UartStr, sizeof(UartStr));
  }
}