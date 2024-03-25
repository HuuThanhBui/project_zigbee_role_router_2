#ifndef _LIB_PIR_SENSOR_H_
#define _LIB_PIR_SENSOR_H_

#include "stdint.h"
#include "stdbool.h"

enum state_of_pir{
	PIR_UNMOTION,
	PIR_MOTION,
	PIR_STATE_DEBOUNCE,
	PIR_STATE_WAIT_5S,
	PIR_STATE_WAIT_30S
};

typedef void (*pirActionHandle_t)(uint8_t pirAction);
void PIR_Enable(boolean en);
void PIR_Init(pirActionHandle_t userPirHandler);
void pirDetechEventHandle(void);
uint8_t isMotionSignal(void);

#endif
