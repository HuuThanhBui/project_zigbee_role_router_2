#include "lm35.h"
#include "stdint.h"
#include "Source/Driver/adc.h"

void lm35_init(void)
{
	initIADC();
}

uint32_t lm35_read(void)
{
	uint32_t total_temp_lm35_value = 0, final_result = 0;
	uint8_t count = 0;
	while(count <= 20)
	{
		count++;
		total_temp_lm35_value += (uint16_t)((IADC_read() * 2.7 * 100)/16384);
		if(count == 20)
		{
			final_result = (uint32_t)(total_temp_lm35_value / 20);
			break;
		}
	}
	return final_result;
}

