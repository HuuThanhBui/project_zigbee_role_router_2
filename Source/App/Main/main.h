#ifndef _MAIN_H_
#define _MAIN_H_

#define CMD_ID_LM75			0x00
#define CMD_ID_BH1750		0x01
#define CMD_ID_LED_1		0x02
#define CMD_ID_LED_2		0x03
#define CMD_ID_LED_3		0x04
#define CMD_ID_LED_4		0x05
#define CMD_ID_LED_5		0x06
#define CMD_ID_LED_6		0x07
#define CMD_ID_LED_7		0x08
#define CMD_ID_LED_8		0x09

typedef enum{
	ZONE_NUM_1 = 1,
	ZONE_NUM_2,
	ZONE_NUM_3
}name_zone_t;

#define SIZE_OF_QUEUE   	300
#define FRAME_SOF 			0xB1
#define FRAME_ACK 			0x06
#define FRAME_NACK 			0x15
#define CXOR_INIT_VAL 		0xFF

#define LENG_BUFF_SENSOR		7
#define LENG_BUFF_LED			6
#define LENG_PAYLOAD_SENSOR		9
#define LENG_PAYLOAD_LED		8

#define CMD_ID_LM75					0x00
#define CMD_ID_BH1750				0x01
#define CMD_ID_LED_1_ZONE_1			0x02
#define CMD_ID_LED_2_ZONE_1			0x03
#define CMD_ID_LED_3_ZONE_1			0x04
#define CMD_ID_LED_4_ZONE_1			0x05
#define CMD_ID_LED_5_ZONE_1			0x06
#define CMD_ID_LED_6_ZONE_1			0x07
#define CMD_ID_LED_7_ZONE_1			0x08
#define CMD_ID_LED_8_ZONE_1			0x09
#define CMD_ID_TEMP_DHT11			0x0A
#define CMD_ID_HUMI_DHT11			0x0B
#define CMD_ID_LM35					0x0C
#define CMD_ID_LED_1_ZONE_2			0x0D
#define CMD_ID_LED_2_ZONE_2			0x0E
#define CMD_ID_LED_3_ZONE_2			0x0F
#define CMD_ID_LED_4_ZONE_2			0x10
#define CMD_ID_LED_5_ZONE_2			0x11
#define CMD_ID_LED_6_ZONE_2			0x12
#define CMD_ID_LED_7_ZONE_2			0x13
#define CMD_ID_LED_8_ZONE_2			0x14
#define CMD_ID_PIR					0x15
#define CMD_ID_MQ8					0x16
#define CMD_ID_LED_1_ZONE_3			0x17
#define CMD_ID_LED_2_ZONE_3			0x18
#define CMD_ID_LED_3_ZONE_3			0x19
#define CMD_ID_LED_4_ZONE_3			0x1A
#define CMD_ID_LED_5_ZONE_3			0x1B
#define CMD_ID_LED_6_ZONE_3			0x1C
#define CMD_ID_LED_7_ZONE_3			0x1D
#define CMD_ID_LED_8_ZONE_3			0x1E

typedef enum{
	UART_STATE_IDLE,
	RX_STATE_START_BYTE,
	RX_STATE_DATA_BYTES,
	UART_STATE_ACK_RECEIVED,
	UART_STATE_NACK_RECEIVED,
	UART_STATE_ERROR,
	RX_STATE_CXOR_BYTE,
	UART_STATE_DATA_RECEIVED,
	UART_STATE_RX_TIMEOUT
}status_revice_data;

typedef enum{
	LM75,
	BH1750,
	TEMP_DHT11,
	HUMI_DHT11,
	LM35,
	PIR,
	MQ7,
	LED
}type_device_t;

typedef enum{
	GET,
	SET
}action_t;

typedef enum{
	LED_NUM_1 = 1,
	LED_NUM_2,
	LED_NUM_3,
	LED_NUM_4,
	LED_NUM_5,
	LED_NUM_6,
	LED_NUM_7,
	LED_NUM_8
}name_led_t;

#endif
