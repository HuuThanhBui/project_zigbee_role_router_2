#include "stdint.h"
#include "stdbool.h"
#include "app/framework/include/af.h"
#include "Source/Mid/Led/led.h"
#include "gpiointerrupt.h"
#include "pir.h"

#define PIR_PORT	gpioPortA
#define PIR_PIN		0

EmberEventControl pirDetechEventControl;

pirActionHandle_t pirHardHandle;
uint8_t pirState;

/*
 * func: isMotionSignal
 * brief: Ham doc gia tri cua cam bien chuye dong PIR
 * param: NONE
 * retval:
 * 		value: Gia tri cua chan duoc su dung de doc gia tri PIR
 */

uint8_t isMotionSignal(void)
{
	uint8_t value = 0;
	value = GPIO_PinInGet(PIR_PORT,PIR_PIN);

	return value;
}

/*
 * func: PIR_Enable
 * brief: Ham de enable hoac disable chan pin cua cam bien PIR lam ngat ngoai
 * param:
 * 		en: Co de enable hoac disable chan pin duoc su dung lam ngat ngoai
 * retval: NONE
 */

void PIR_Enable(boolean en)
{
	if(en)
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,true,false,true);
	}
	else
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,false,false,false);
	}
}

/*
 * func: pirDetechEventHandle
 * brief: Ham xu ly su kien pirDetechEventControl, ham nay se xu ly cac su kien cua PIR: trang thai co vat can, trang thai khong con vat can, ...
 * param: NONE
 * retval: NONE
 */

void pirDetechEventHandle(void)
{
	emberEventControlSetInactive(pirDetechEventControl);
	switch(pirState)
	{
		case PIR_STATE_DEBOUNCE:		//Khi su kien ngat vua moi xay ra
		{
			if(isMotionSignal())
			{
				emberAfCorePrintln("PIR_STATE_DEBOUNCE_TRUE.........................");
				emberAfCorePrintln("PIR_DETECH_MOTION");
				if(pirHardHandle != NULL)
				{
					pirHardHandle(PIR_MOTION);		//Goi den ham xu ly PIR do user code (gui cho ZC, nhay led)
				}
				emberEventControlSetDelayMS(pirDetechEventControl,5000);
			}
			else
			{
				emberAfCorePrintln("PIR_STATE_DEBOUNCE_FAILED.........................");
				pirState = PIR_STATE_WAIT_5S;		//Gan trang thai cho su kien tiep theo
				ledoff(porta,3);
				emberEventControlSetDelayMS(pirDetechEventControl,200);
				PIR_Enable(true);
			}
			break;
		}
		case PIR_STATE_WAIT_5S:		//Trang thai cho 30s neu khong con chuyen dong
		{
			emberAfCorePrintln("TEST_LOG_PIR_STATE_WAIT_5S..........................");
			pirState = PIR_STATE_WAIT_30S;		//Gan trang thai cho su kien tiep theo
			PIR_Enable(true);
			emberEventControlSetDelayMS(pirDetechEventControl,30000);
			break;
		}
		case PIR_STATE_WAIT_30S:
		{
			if(pirHardHandle != NULL)
			{
				emberAfCorePrintln("TEST_LOG_PIR_STATE_WAIT_30S.........................");
				emberAfCorePrintln("PIR_DETECH_UNMOTION");
				pirHardHandle(PIR_UNMOTION);			//Goi den ham xu ly PIR do user code (gui cho ZC, nhay led)
			}
			break;
		}
		default:
			break;
	}
}

/*
 * func: PIR_INTSignalHandle
 * brief: Ham callback de xu ly su kien ngat
 * param:
 * 		pin: Chan pin dang say ra su kien ngat
 * retval: NONE
 */

static void PIR_INTSignalHandle(uint8_t pin)
{
	emberAfCorePrintln(".......................ALRAM_STATE_IS_ENABLE......................");
	if(pin != PIR_PIN)
	{
		emberAfCorePrintln("PIR_PIN_TEST_LOG..................");
		return;
	}
	if(isMotionSignal())
	{
		emberAfCorePrintln("PIR_STATE_DEBOUNCE_TEST_LOG..................");
		pirState = PIR_STATE_DEBOUNCE;
		PIR_Enable(false);
		emberEventControlSetInactive(pirDetechEventControl);
		emberEventControlSetDelayMS(pirDetechEventControl,200);		//Cu 200ms kiem tra trang thai PIR 1 lan
	}
}

/*
 * func: PIR_Init
 * brief: Ham setup chan cua cam bien PIR lam chan ngat
 * param:
 * 		userPirHandler: Function parameter se duoc su dung de lay su kien cua cam bien PIR
 * retval: NONE
 */

void PIR_Init(pirActionHandle_t userPirHandler)
{
	GPIOINT_Init();
	GPIO_PinModeSet(PIR_PORT,PIR_PIN,gpioModeInput,1);				//Cai dat chan ngat
	GPIOINT_CallbackRegister(PIR_PIN,PIR_INTSignalHandle);			//Dang ky ham ngat, line ngat
	pirHardHandle = userPirHandler;									//Gan ham ngat
	PIR_Enable(true);
	emberAfCorePrintln("Exit PIR Init");
}








