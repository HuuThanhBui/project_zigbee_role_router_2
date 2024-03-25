#include "dht11.h"
#include "app/framework/include/af.h"
#include "platform/emdrv/ustimer/inc/ustimer.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "em_gpio.h"

#define PIN_DHT11 	4
#define PORT_DHT11 	gpioPortA

static void delay_ms(uint32_t time)
{
	uint32_t time_deboune = halCommonGetInt32uMillisecondTick();
	while(1)
	{
		if(halCommonGetInt32uMillisecondTick() - time_deboune >= time)
		{
			return;
		}
	}
}

void DHT11_Init(void)
{
	GPIO_PinModeSet(PORT_DHT11,PIN_DHT11,gpioModePushPull,0);
	GPIO_PinOutSet(PORT_DHT11,PIN_DHT11);
}

void Start_DHT11(void)
{
	GPIO_PinModeSet(PORT_DHT11,PIN_DHT11,gpioModePushPull,0);
	GPIO_PinOutClear(PORT_DHT11,PIN_DHT11);
	delay_ms(20);
	GPIO_PinOutSet(PORT_DHT11,PIN_DHT11);
	USTIMER_Delay(30);
}

uint8_t check_dht11(void)
{
	uint32_t time = 0;
	GPIO_PinModeSet(PORT_DHT11,PIN_DHT11,gpioModeInput,0);
	while(GPIO_PinInGet(PORT_DHT11,PIN_DHT11) && time < 100)
	{
		time++;
		USTIMER_Delay(1);
	}
	if(time >= 100)
	{
		return 1;
	}
	else{
		time = 0;
	}
	
	while(!GPIO_PinInGet(PORT_DHT11,PIN_DHT11) && time < 100)
	{
		time++;
		USTIMER_Delay(1);
	}
	if(time >= 100)
	{
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t read_1_bit_data(void)
{
	uint32_t time = 0;
	while(GPIO_PinInGet(PORT_DHT11,PIN_DHT11) && time < 100)
	{
		time++;
		USTIMER_Delay(1);
	}
	while(!GPIO_PinInGet(PORT_DHT11,PIN_DHT11) && time < 100)
	{
		time++;
		USTIMER_Delay(1);
	}
	USTIMER_Delay(40);
	if(GPIO_PinInGet(PORT_DHT11,PIN_DHT11))
	{
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t read_1_byte_data(void)
{
	uint8_t i = 0;
	uint8_t data = 0;
	
	for(i = 0; i < 8; i++)
	{
		data <<= 1;
		data |= read_1_bit_data();
	}
	return data;
}

uint8_t read_5_byte_data(uint8_t *temp, uint8_t *humi)
{
	uint8_t i = 0;
	uint8_t buf[5];
	
	
	Start_DHT11();
	
	if(check_dht11() == 0)
	{
		for(i = 0; i < 5; i++)
		{
			buf[i] = read_1_byte_data();
		}
		if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			*temp = buf[2];
			*humi = buf[0];
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

