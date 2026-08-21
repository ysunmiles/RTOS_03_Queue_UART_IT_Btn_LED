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
    osDelay(1000);
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
    UartStr[Size] = '\0';

    OLED_ShowString(4, 1, "                ");
    OLED_ShowString(4, 1, UartStr);

    memset(UartStr, 0, sizeof(UartStr));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)UartStr, sizeof(UartStr));
  }
}