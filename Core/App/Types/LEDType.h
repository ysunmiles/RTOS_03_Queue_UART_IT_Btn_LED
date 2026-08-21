#ifndef __LEDTYPE_H
#define __LEDTYPE_H

typedef enum {
    LED_1 = 1,
    LED_2 = 2,
    LED_3 = 3,
} LED_Num;

typedef enum {
    LED_State_Off = 0,
    LED_State_On = 1,
}LED_State;

typedef struct {
    LED_Num num;
    LED_State state;
}LEDCmdType;

#endif