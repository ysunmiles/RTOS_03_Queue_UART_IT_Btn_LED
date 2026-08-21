#include "LEDType.h"
#include "cmsis_os2.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
#include <stdlib.h>

void StartLEDTask(void *argument) {
  LEDCmdType cmd;
  for (;;) {
    osMessageQueueGet(LEDCmdQueueHandle, &cmd, 0, osWaitForever);
    switch (cmd.num) {
      case LED_1:
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, cmd.state ? GPIO_PIN_SET:GPIO_PIN_RESET);
      break;
      case LED_2:
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, cmd.state ? GPIO_PIN_SET:GPIO_PIN_RESET);
      break;
      case LED_3:
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, cmd.state ? GPIO_PIN_SET:GPIO_PIN_RESET);
      break;
    }
    OLED_ShowNum(2, 15, cmd.num, 1);
    OLED_ShowNum(3, 15, cmd.state, 1);
  }
}