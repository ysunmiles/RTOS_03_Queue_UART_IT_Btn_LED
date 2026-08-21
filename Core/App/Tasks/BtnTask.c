#include "main.h"
#include "LEDType.h"
#include "cmsis_os2.h"
#include "OLED.h"
#include <stdlib.h>

void StartBtnTask(void *argument) {
  LED_State State = LED_State_Off;
  for (;;) {
    if (HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET) {
      if (HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET) {
        LEDCmdType* pLEDCmd = malloc(sizeof(LEDCmdType));
        pLEDCmd->num = LED_1;
        State = !State;
        pLEDCmd->state = State;
        osMessageQueuePut(LEDCmdQueueHandle, &pLEDCmd, 0, osWaitForever);
        OLED_ShowString(1, 1, "Btn Pressed");
        osDelay(500);
        OLED_ShowString(1, 1, "           ");
      }
      while (HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET) {
        osDelay(10);
      }
    }else{
      osDelay(10);
    }
  }
}