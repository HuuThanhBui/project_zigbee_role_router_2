#include "stdint.h"
#include "stdbool.h"
#include "main.h"
#include "app/framework/include/af.h"
#include "app/framework/plugin/reporting/reporting.h"
#include "app/framework/util/attribute-storage.h"
#include "zigbee-framework/zigbee-device-common.h"
#include "app/framework/plugin/find-and-bind-target/find-and-bind-target.h"
#include "app/framework/plugin/find-and-bind-initiator/find-and-bind-initiator.h"
#include "Source/App/Network/Network.h"
#include "Source/App/Receive/Receive.h"
#include "Source/App/Send/Send.h"
#include "Source/Mid/Led/led.h"
#include "platform/emdrv/ustimer/inc/ustimer.h"
#include "Source/Mid/dht11/dht11.h"
//#include "Source/Mid/PIR/pir.h"
#include "Ringbuffer.h"

enum state_of_system{		//Hang so trang thai cua he thong
	POWER_ON_STATE,		//Khi moi khoi dong
	REPORT_STATE,		//Khi gui gia tri len ZC
	IDLE_STATE,			//Khi he thong hoat dong binh thuong
	REBOOT_STATE		//Khi he thong thuc hien khoi dong lai
};

typedef struct {
	uint8_t cmdid;
	uint8_t type;
}frm_common_t;

typedef struct data_struct_zone_2 data_struct_zone_2_t;
struct data_struct_zone_2{
	uint8_t data_led[8];
	uint16_t real_temp_dht11;
	uint16_t real_humi_dht11;
	uint8_t byte_1_pir;
	uint8_t byte_2_pir;
	uint8_t byte_1_temp_dht11;
	uint8_t byte_2_temp_dht11;
	uint8_t byte_1_humi_dht11;
	uint8_t byte_2_humi_dht11;
};

data_struct_zone_2_t data_zone_2;

EmberEventControl mainStateEventControl;
EmberEventControl timerCheckNetworkControl;
EmberEventControl timerResetSystemControl;
EmberEventControl TimerSendDataToCoordinatorControl;
EmberEventControl TimerReadDHT11Control;
EmberEventControl TimerProcessDataControl;
EmberEventControl TimerCheckNetworkStartupControl;
var_proc_t var_proc;

static RingBuff ringbuff;
uint8_t arr_mem_data[SIZE_OF_QUEUE] = {0};
static uint8_t byRxBuffer2[SIZE_OF_QUEUE] = {0};
static uint8_t byRxBufState = RX_STATE_START_BYTE;
static uint8_t byIndexRxBuf = 0;
static uint8_t byCheckXorRxBuf = 0;

void timerCheckNetworkHandler(void);
void mainStateEventHandler(void);							//Ham xu ly event network
void networkEventHandle(uint8_t networkResult);				//Ham callback xu ly network
void USER_PIREventHandle(uint8_t pirAction);				//Ham xu ly PIR
void function_Recive_Data_Over_Zigbee(EmberAfIncomingMessage* incomingMessage);

void delay_ms(uint32_t time)
{
	uint32_t count = halCommonGetInt32uMillisecondTick();
	while(halCommonGetInt32uMillisecondTick() - count <= time)
	{
		;
	}
}

void reset_data_struct_zone_2(void)
{
	data_zone_2.byte_1_humi_dht11 = 0;
	data_zone_2.byte_1_pir = 0;
	data_zone_2.byte_1_temp_dht11 = 0;
	data_zone_2.byte_2_humi_dht11 = 0;
	data_zone_2.byte_2_pir = 0;
	data_zone_2.byte_2_temp_dht11 = 0;
	data_zone_2.real_humi_dht11 = 0;
	data_zone_2.real_temp_dht11 = 0;
	data_zone_2.data_led[0] = 0;
	data_zone_2.data_led[1] = 0;
	data_zone_2.data_led[2] = 0;
	data_zone_2.data_led[3] = 0;
	data_zone_2.data_led[4] = 0;
	data_zone_2.data_led[5] = 0;
	data_zone_2.data_led[6] = 0;
	data_zone_2.data_led[7] = 0;
}

void calcu_data_to_send(type_device_t device, action_t action, uint8_t command_id, uint8_t option, uint8_t sequen,uint8_t data_to_send[], uint8_t leng_of_data, uint8_t data[])
{
	uint8_t result_xor = 0;
	if(device == LED)
	{
		if(action == SET)
		{
			result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
			for(uint32_t i = 0; i < leng_of_data; i++)
			{
				result_xor = result_xor ^ data_to_send[i];
			}
			data[0] = FRAME_SOF; data[1] = LENG_BUFF_LED; data[2] = option; data[3] = command_id; data[4] = 0x01;
			data[5] = data_to_send[0]; data[6] = sequen; data[7] = result_xor;
		}
		else if(action == GET)
		{
			result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x02 ^ sequen);
			for(uint32_t i = 0; i < leng_of_data; i++)
			{
				result_xor = result_xor ^ data_to_send[i];
			}
			data[0] = FRAME_SOF; data[1] = LENG_BUFF_LED; data[2] = option; data[3] = command_id; data[4] = 0x02;
			data[5] = data_to_send[0]; data[6] = sequen; data[7] = result_xor;
		}
	}
	else if(device == LM75)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == BH1750)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == TEMP_DHT11)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == HUMI_DHT11)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == LM35)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == PIR)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == MQ7)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
}

uint8_t table_convert_to_command_id(uint8_t number_led, uint8_t number_zone)
{
	name_led_t name_led = LED_NUM_1;
	name_zone_t name_zone = ZONE_NUM_1;
	if(number_led == 1){name_led = LED_NUM_1;}
	else if(number_led == 2){name_led = LED_NUM_2;}
	else if(number_led == 3){name_led = LED_NUM_3;}
	else if(number_led == 4){name_led = LED_NUM_4;}
	else if(number_led == 5){name_led = LED_NUM_5;}
	else if(number_led == 6){name_led = LED_NUM_6;}
	else if(number_led == 7){name_led = LED_NUM_7;}
	else if(number_led == 8){name_led = LED_NUM_8;}

	if(number_zone == 1){name_zone = ZONE_NUM_1;}
	else if(number_zone == 2){name_zone = ZONE_NUM_2;}
	else if(number_zone == 3){name_zone = ZONE_NUM_3;}

	switch(name_zone)
	{
		case ZONE_NUM_1:
		{
			if(name_led == LED_NUM_1) {return 0x02;}
			else if(name_led == LED_NUM_2) {return 0x03;}
			else if(name_led == LED_NUM_3) {return 0x04;}
			else if(name_led == LED_NUM_4) {return 0x05;}
			else if(name_led == LED_NUM_5) {return 0x06;}
			else if(name_led == LED_NUM_6) {return 0x07;}
			else if(name_led == LED_NUM_7) {return 0x08;}
			else if(name_led == LED_NUM_8) {return 0x09;}
			break;
		}
		case ZONE_NUM_2:
		{
			if(name_led == LED_NUM_1) {return 0x0D;}
			else if(name_led == LED_NUM_2) {return 0x0E;}
			else if(name_led == LED_NUM_3) {return 0x0F;}
			else if(name_led == LED_NUM_4) {return 0x10;}
			else if(name_led == LED_NUM_5) {return 0x11;}
			else if(name_led == LED_NUM_6) {return 0x12;}
			else if(name_led == LED_NUM_7) {return 0x13;}
			else if(name_led == LED_NUM_8) {return 0x14;}
			break;
		}
		case ZONE_NUM_3:
		{
			if(name_led == LED_NUM_1) {return 0x17;}
			else if(name_led == LED_NUM_2) {return 0x18;}
			else if(name_led == LED_NUM_3) {return 0x19;}
			else if(name_led == LED_NUM_4) {return 0x1A;}
			else if(name_led == LED_NUM_5) {return 0x1B;}
			else if(name_led == LED_NUM_6) {return 0x1C;}
			else if(name_led == LED_NUM_7) {return 0x1D;}
			else if(name_led == LED_NUM_8) {return 0x1E;}
			break;
		}
	}
	return 0xFF;
}

void convert_dec_to_hex(uint16_t value, unsigned int *data_1, unsigned int *data_2)
{
	char arr_temp[10] = "", arr_temp_1[3] = "",arr_temp_2[3] = "";
	sprintf(arr_temp,"%x",value);
	arr_temp_1[0] = arr_temp[0];
	arr_temp_1[1] = arr_temp[1];
	arr_temp_1[2] = '\0';
	arr_temp_2[0] = arr_temp[2];
	arr_temp_2[1] = arr_temp[3];
	arr_temp_2[2] = '\0';
	sscanf(arr_temp_1,"%x",data_1);
	sscanf(arr_temp_2,"%x",data_2);
}

void clear_network(void)
{
	uint8_t contents[ZDO_MESSAGE_OVERHEAD+1];
	contents[0] = 0x00;
	(void)emberSendZigDevRequest(0x0000,LEAVE_RESPONSE,EMBER_AF_DEFAULT_APS_OPTIONS,contents,sizeof(contents));
	emberClearBindingTable();
	emberAfCorePrintln(" Joined network, prepare leave network ");
	emberSendZigbeeLeave(emberGetNodeId(),EMBER_ZIGBEE_LEAVE_AND_REJOIN);
	emberLeaveNetwork();
}

void TimerCheckNetworkStartupHandler(void)
{
	static uint8_t count = 0;
	EmberNetworkStatus nwkStatusCurrent;
	emberEventControlSetInactive(TimerCheckNetworkStartupControl);
	nwkStatusCurrent = emberAfNetworkState();
	emberAfCorePrintln("\n\nState_Of_Network: %d\n\n",(uint8_t)nwkStatusCurrent);

	if(nwkStatusCurrent != EMBER_NO_NETWORK || nwkStatusCurrent != EMBER_JOINED_NETWORK_NO_PARENT)
	{
		clear_network();
		emberEventControlSetDelayMS(timerCheckNetworkControl,5000);
		return;
	}
	else
	{
		count++;
		if(count >= 7)
		{
			emberEventControlSetInactive(TimerCheckNetworkStartupControl);
			var_proc.systemState = POWER_ON_STATE;		//Gan trang thai cua he thong la vua khoi dong len
			emberEventControlSetDelayMS(timerCheckNetworkControl,3000);
		}
	}
	emberEventControlSetDelayMS(TimerCheckNetworkStartupControl,3000);
}

/*
 * func: emberAfMainInitCallback
 * brief: Ham main se duoc stack zigbee goi dau tien khi he thong khoi dong len
 * param: NONE
 * retval: NONE
 */

void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("emberAfMainInitCallback");
	reset_data_struct_zone_2();
	halInternalStartSystemTimer();
	USTIMER_Init();
	DHT11_Init();
//	PIR_Init(USER_PIREventHandle);
	init_function_callback_recive_data_over_zigbee(function_Recive_Data_Over_Zigbee);
	ring_buff_init(&ringbuff, arr_mem_data, SIZE_OF_QUEUE);
	/************************** Cai dat phan cung *********************/
	ledinit(porta,3);			//Khoi tao chan pin lam GPIO dieu khien LED
	ledinit(portd,0);			//Khoi tao chan pin lam GPIO dieu khien LED
	ledinit(portd,1);			//Khoi tao chan pin lam GPIO dieu khien LED
	ledinit(portd,2);			//Khoi tao chan pin lam GPIO dieu khien LED
	ledinit(portc,1);			//Khoi tao chan pin lam GPIO dieu khien LED
	ledinit(portc,2);			//Khoi tao chan pin lam GPIO dieu khien LED

	ledoff(porta,3);
	ledoff(portd,0);
	ledoff(portd,1);
	ledoff(portd,2);
	ledon(portc,1);
	ledon(portc,2);

	/******************************************************************/

	/******************** Cai dat mang khoi tao trang thai *****************/
	NETWORK_Init(networkEventHandle);		//Khoi tao ham xu lys trang thai cua mang zigbee

	emberEventControlSetActive(mainStateEventControl);
	emberEventControlSetInactive(timerCheckNetworkControl);
	emberEventControlSetInactive(timerResetSystemControl);
	emberEventControlSetInactive(TimerReadDHT11Control);
	emberEventControlSetInactive(TimerProcessDataControl);
	emberEventControlSetInactive(TimerCheckNetworkStartupControl);

	emberEventControlSetDelayMS(TimerReadDHT11Control,3000);
	emberEventControlSetInactive(TimerSendDataToCoordinatorControl);
	emberEventControlSetDelayMS(TimerSendDataToCoordinatorControl,5000);
	emberEventControlSetDelayMS(TimerCheckNetworkStartupControl,3000);
	emberEventControlSetActive(TimerProcessDataControl);
	/**********************************************************************/
}

/*
 * func: networkEventHandle
 * brief: Ham callback se duoc su dung de xu ly cac trang thai cua mang zigbee ma tham so networkResult duoc ham emberAfStackStatusCallback truyen vao
 * param:
 * 		networkResult: Trang thai cua mang zigbee tham so nay duoc truyen boi ham emberAfStackStatusCallback
 * retval: NONE
 */

void networkEventHandle(uint8_t networkResult)
{
	emberAfCorePrintln("Main_NetworkCallbackHandler");
	switch(networkResult)
	{
		case NETWORK_HAS_PARENT:
		{
			emberAfCorePrintln("\nZone 2 Da Join mang !!!\n");
			SEND_ReportInfoToHc();				//Gui thong tin cua thiet bi toi ZC de phuc vu tao Icon tren App
			var_proc.networkReady = true;
			var_proc.systemState = REPORT_STATE;						//Gan trang thai he thong REPORT_STATE de gui thong tin cua he thong len ZC
			emberEventControlSetDelayMS(mainStateEventControl,1000);	//Sau 1s goi lai ham mainStateEventHandler de xu ly trang thai he thong
			break;
		}
		case NETWORK_JOIN_FAIL:
		{
			var_proc.systemState = IDLE_STATE;						//Gan trang thai he thong IDLE_STATE de he thong hoat dong binh thuong
			emberEventControlSetDelayMS(mainStateEventControl,1000);	//Sau 1s goi lai ham mainStateEventHandler de xu ly trang thai he thong
			break;
		}
		case NETWORK_JOIN_SUCCESS:
		{
			emberAfCorePrintln("\nZone 2 Da Join mang !!!\n");
			var_proc.networkReady = true;
			var_proc.systemState = REPORT_STATE;						//Gan trang thai he thong REPORT_STATE de gui thong tin cua he thong len ZC
			SEND_ReportInfoToHc();				//Gui thong tin cua thiet bi toi ZC de phuc vu tao Icon tren App
			emberEventControlSetDelayMS(mainStateEventControl,1000);	//Sau 1s goi lai ham mainStateEventHandler de xu ly trang thai he thong
			break;
		}
		case NETWORK_LOST_PARENT:
		{
			var_proc.systemState = IDLE_STATE;						//Gan trang thai he thong IDLE_STATE de he thong hoat dong binh thuong
			emberEventControlSetDelayMS(mainStateEventControl,1000);	//Sau 1s goi lai ham mainStateEventHandler de xu ly trang thai he thong
			break;
		}
		case NETWORK_OUT_NETWORK:
		{
			if(var_proc.networkReady)
			{
				var_proc.systemState = REBOOT_STATE;							//Bat trang thai reboot he thong len
				emberEventControlSetDelayMS(mainStateEventControl,3000);		//Kich hoat reboot he thong
			}
			break;
		}
		case REPORT_STATE:
		{
			var_proc.systemState = IDLE_STATE;
			SEND_ReportInfoToHc();				//Gui thong tin cua thiet bi toi ZC de phuc vu tao Icon tren App
			break;
		}
		default:
			break;
	}
}

/*
 * func: mainStateEventHandler
 * brief: Ham se duoc su dung de xu ly cac trang thai cua he thong nhu: IDLE, REPORT, POWER_ON,...
 * param: NONE
 * retval: NONE
 */

void mainStateEventHandler(void)					//Ham callback de xu ly cac trang thai cua he thong
{
	EmberNetworkStatus nwkStatusCurrent;
	emberEventControlSetInactive(mainStateEventControl);
	switch(var_proc.systemState)
	{
		case POWER_ON_STATE:			//Trang thai he thong khi khoi dong
		{
			nwkStatusCurrent = emberAfNetworkState();		//API emberAfNetworkState tra ve trang thai mang cua he thong
			if(nwkStatusCurrent == EMBER_NO_NETWORK)
			{
				NETWORK_FindAndJoin();						//Tim mang va join vao mang da tim duoc
			}
			var_proc.systemState = IDLE_STATE;
			break;
		}
		case REPORT_STATE:				//Trang thai he thong khi can report cho ZC
		{
			var_proc.systemState = IDLE_STATE;
			SEND_ReportInfoToHc();				//Gui thong tin cua thiet bi toi ZC de phuc vu tao Icon tren App
			break;
		}
		case IDLE_STATE:				//Trang thai he thong khi nghi
		{
			break;
		}
		case REBOOT_STATE:				//Trang thai he thong khi reset
		{
			var_proc.systemState = IDLE_STATE;
			uint8_t contents[ZDO_MESSAGE_OVERHEAD+1];
			contents[0] = 0x00;
			(void)emberSendZigDevRequest(0x0000,LEAVE_RESPONSE,EMBER_AF_DEFAULT_APS_OPTIONS,contents,sizeof(contents));
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			emberClearBindingTable();
			if(networkStatus == EMBER_JOINED_NETWORK || networkStatus == NETWORK_JOIN_SUCCESS)
			{
				emberAfCorePrintln(" Joined network, prepare leave network ");
				emberSendZigbeeLeave(emberGetNodeId(),EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN);
				emberLeaveNetwork();
			}
			emberEventControlSetDelayMS(timerResetSystemControl, 5000);
			break;
		}
		default:
			break;
	}
}

/*
 * func: timerCheckNetworkHandler
 * brief: Ham xu ly event timerCheckNetworkControl, khi he thong khong join bat ky mang zigbee nao, ham se tu dong join mang zigbee da ton tai
 * param: NONE
 * retval: NONE
 */

void timerCheckNetworkHandler(void)
{
	EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();		//API emberAfNetworkState tra ve trang thai mang cua he thong
	emberEventControlSetInactive(timerCheckNetworkControl);
	if(nwkStatusCurrent == EMBER_NO_NETWORK)
	{
		NETWORK_FindAndJoin();
		emberEventControlSetDelayMS(timerCheckNetworkControl,5000);//Tim mang va join vao mang da tim duoc
		return;
	}
	emberEventControlSetInactive(TimerCheckNetworkStartupControl);
	emberEventControlSetInactive(timerCheckNetworkControl);
}

/*
 * func: timerResetSystemHandler
 * brief: Ham xu ly event reset va roi mang
 * param: NONE
 * retval: NONE
 */

void timerResetSystemHandler(void)
{
	halReboot();
	emberEventControlSetInactive(timerResetSystemControl);
}

void USER_PIREventHandle(uint8_t pirAction)
{
//	static boolean sendFlag = true;
//	switch(pirAction)
//	{
//		case PIR_MOTION:
//		{
//			if(sendFlag)
//			{
//				emberAfCorePrintln("PIR_MOTION_TEST_LOG................");
//				sendFlag = false;
//				data_zone_2.byte_1_pir = 1;
//			}
//			ledon(porta,3);
//			break;
//		}
//		case PIR_UNMOTION:
//		{
//			emberAfCorePrintln("PIR_UNMOTION_TEST_LOG................");
//			sendFlag = true;
//			data_zone_2.byte_1_pir = 0;
//			ledoff(porta,3);
//			break;
//		}
//		default:
//			break;
//	}
}

void TimerSendDataToCoordinatorHandler(void)
{
	EmberNetworkStatus nwkStatusCurrent;
	uint8_t arr_data[3] = {0x00, 0x00, 0x00}, data_send[10] = {0};
	emberEventControlSetInactive(TimerSendDataToCoordinatorControl);

	arr_data[0] = data_zone_2.byte_1_humi_dht11; arr_data[1] = data_zone_2.byte_2_humi_dht11;
//	emberAfCorePrintln("Humi byte of DHT11 before send: %d  ---  Humity of DHT11: %d",data_zone_2.byte_1_humi_dht11,data_zone_2.byte_2_humi_dht11);
	calcu_data_to_send(HUMI_DHT11, SET, CMD_ID_HUMI_DHT11, ZONE_NUM_2, 0,(uint8_t *)arr_data, 2, data_send);
	SEND_Data(0x0000, 9, 11, data_send);

	arr_data[0] = data_zone_2.byte_1_temp_dht11; arr_data[1] = data_zone_2.byte_2_temp_dht11;
	calcu_data_to_send(TEMP_DHT11, SET, CMD_ID_TEMP_DHT11, ZONE_NUM_2, 0,(uint8_t *)arr_data, 2, data_send);
	SEND_Data(0x0000, 8, 10, data_send);

	arr_data[0] = data_zone_2.byte_1_pir;
	calcu_data_to_send(PIR, SET, CMD_ID_PIR, ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 10, 12, data_send);

	arr_data[0] = data_zone_2.data_led[0];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_1, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 1, 1, data_send);

	arr_data[0] = data_zone_2.data_led[1];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_2, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 2, 2, data_send);

	arr_data[0] = data_zone_2.data_led[2];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_3, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 3, 3, data_send);

	arr_data[0] = data_zone_2.data_led[3];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_4, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 4, 4, data_send);

	arr_data[0] = data_zone_2.data_led[4];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_5, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 5, 5, data_send);

	arr_data[0] = data_zone_2.data_led[5];
	calcu_data_to_send(LED, SET, table_convert_to_command_id(LED_NUM_6, ZONE_NUM_2), ZONE_NUM_2, 0,(uint8_t *)arr_data, 1, data_send);
	SEND_Data(0x0000, 6, 6, data_send);

	nwkStatusCurrent = emberAfNetworkState();
	emberAfCorePrintln("\n\nState_Of_Network: %d\n\n",(uint8_t)nwkStatusCurrent);

	emberEventControlSetDelayMS(TimerSendDataToCoordinatorControl,5000);
}

uint8_t PollRxBuff(void)
{
	uint8_t byRxData;
	uint8_t byUartState = (uint8_t)UART_STATE_IDLE;
	byRxBufState = RX_STATE_START_BYTE;
	while((ring_buff_available(&ringbuff) != 0) && (byUartState == UART_STATE_IDLE))
	{
		ring_buff_pop(&ringbuff, &byRxData);
		switch(byRxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if(byRxData == FRAME_SOF)
				{
					byIndexRxBuf = 0;
					byCheckXorRxBuf = CXOR_INIT_VAL;
					byRxBufState = RX_STATE_DATA_BYTES;
				}
				else if(byRxData == FRAME_ACK)
				{
					byUartState = UART_STATE_ACK_RECEIVED;
				}
				else if(byRxData == FRAME_NACK)
				{
					byUartState = UART_STATE_NACK_RECEIVED;
				}
				else
				{
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			case RX_STATE_DATA_BYTES:
			{
				if(byIndexRxBuf < 254)
				{
					byRxBuffer2[byIndexRxBuf] = byRxData;
					if(byIndexRxBuf > 0)
					{
						byCheckXorRxBuf ^= byRxData;
					}
					if(++byIndexRxBuf == *byRxBuffer2)
					{
						byRxBufState = RX_STATE_CXOR_BYTE;
					}
				}
				else
				{
					byRxBufState = RX_STATE_START_BYTE;
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			case RX_STATE_CXOR_BYTE:
			{
				if(byRxData == byCheckXorRxBuf)
				{
					byUartState = UART_STATE_DATA_RECEIVED;
				}
				else
				{
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			default:
				byRxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return byUartState;
}

void UartCommandProcess(void *arg)
{
	uint16_t real_data = 0;
	frm_common_t *pCmd = (frm_common_t *)arg;
	if(byRxBuffer2[1] != 2)
	{
		return;
	}
	switch(pCmd->cmdid)
	{
		case CMD_ID_TEMP_DHT11:
		{
//			printf("\nLM75 Need handler !\n");
//			emberAfCorePrintln("Raw Data LM75: %d  --  %d",byRxBuffer2[4], byRxBuffer2[5]);
			data_zone_2.byte_1_temp_dht11 = byRxBuffer2[4];
			data_zone_2.byte_2_temp_dht11 = byRxBuffer2[5];
			if(byRxBuffer2[5] != 0)
			{
				real_data = byRxBuffer2[4] * 10 + byRxBuffer2[5] * 1;
			}
			else
			{
				real_data = byRxBuffer2[4];
			}
			emberAfCorePrintln("Data temp dht11: %d",real_data);
			data_zone_2.real_temp_dht11 = real_data;
			break;
		}
		case CMD_ID_HUMI_DHT11:
		{
//			printf("\nBH1750 Need handler !\n");
//			emberAfCorePrintln("Raw Data BH1750: %d  --  %d",byRxBuffer2[4], byRxBuffer2[5]);
			data_zone_2.byte_1_humi_dht11 = byRxBuffer2[4];
			data_zone_2.byte_2_humi_dht11 = byRxBuffer2[5];
			if(byRxBuffer2[5] != 0)
			{
				real_data = byRxBuffer2[4] * 10 + byRxBuffer2[5] * 1;
			}
			else
			{
				real_data = byRxBuffer2[4];
			}
			emberAfCorePrintln("Data huni dht11: %d",real_data);
			data_zone_2.real_humi_dht11 = real_data;
			break;
		}
		case CMD_ID_LED_1_ZONE_2:
		{
//			printf("\nLED_1_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_1_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[0] = byRxBuffer2[4];
			if(data_zone_2.data_led[0] == 0)
			{
				ledoff(porta,3);
			}
			else if(data_zone_2.data_led[0] == 1)
			{
				ledon(porta,3);
			}
			break;
		}
		case CMD_ID_LED_2_ZONE_2:
		{
//			printf("\nLED_2_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_2_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[1] = byRxBuffer2[4];
			if(data_zone_2.data_led[1] == 0)
			{
				ledoff(portd,0);
			}
			else if(data_zone_2.data_led[1] == 1)
			{
				ledon(portd,0);
			}
			break;
		}
		case CMD_ID_LED_3_ZONE_2:
		{
//			printf("\nLED_3_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_3_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[2] = byRxBuffer2[4];
			if(data_zone_2.data_led[2] == 0)
			{
				ledoff(portd,1);
			}
			else if(data_zone_2.data_led[2] == 1)
			{
				ledon(portd,1);
			}
			break;
		}
		case CMD_ID_LED_4_ZONE_2:
		{
//			printf("\nLED_4_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_4_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[3] = byRxBuffer2[4];
			if(data_zone_2.data_led[3] == 0)
			{
				ledoff(portd,2);
			}
			else if(data_zone_2.data_led[3] == 1)
			{
				ledon(portd,2);
			}
			break;
		}
		case CMD_ID_LED_5_ZONE_2:
		{
//			printf("\nLED_5_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_5_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[4] = byRxBuffer2[4];
			if(data_zone_2.data_led[4] == 0)
			{
				ledon(portc,1);
			}
			else if(data_zone_2.data_led[4] == 1)
			{
				ledoff(portc,1);
			}
			break;
		}
		case CMD_ID_LED_6_ZONE_2:
		{
//			printf("\nLED_6_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_6_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[5] = byRxBuffer2[4];
			if(data_zone_2.data_led[5] == 0)
			{
				ledon(portc,2);
			}
			else if(data_zone_2.data_led[5] == 1)
			{
				ledoff(portc,2);
			}
			break;
		}
		case CMD_ID_LED_7_ZONE_2:
		{
//			printf("\nLED_7_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_7_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[6] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_8_ZONE_2:
		{
//			printf("\nLED_8_ZONE_1 Need handler !\n");
			emberAfCorePrintln("Raw Data LED_8_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_2.data_led[7] = byRxBuffer2[4];
			break;
		}
		default:
//			printf("\nData Error !!!\n");
			emberAfCorePrintln("Data Error !!!");
			break;
	}
}

void processDataReceiver(void)
{
	uint8_t stateRx;
	stateRx = PollRxBuff();
	if(stateRx != UART_STATE_IDLE)
	{
		switch(stateRx)
		{
			case UART_STATE_ACK_RECEIVED:
			{
				break;
			}
			case UART_STATE_NACK_RECEIVED:
			{
				break;
			}
			case UART_STATE_DATA_RECEIVED:
			{
				UartCommandProcess(&byRxBuffer2[2]);
				break;
			}
			case UART_STATE_ERROR:
			case UART_STATE_RX_TIMEOUT:
			{
				break;
			}
			default:
				break;
		}
	}
}

void TimerProcessDataHandler(void)
{
	emberEventControlSetInactive(TimerProcessDataControl);
	processDataReceiver();
	emberEventControlSetActive(TimerProcessDataControl);
}

void TimerReadDHT11Handler(void)
{
	uint8_t temper = 0, humi = 0;
	emberEventControlSetInactive(TimerReadDHT11Control);
	read_5_byte_data(&temper,&humi);
	emberAfCorePrintln("Temper of DHT11: %d  ---  Humity of DHT11: %d",temper,humi);
	data_zone_2.real_temp_dht11 = temper;
	convert_dec_to_hex(temper,(unsigned int *)&data_zone_2.byte_1_temp_dht11, (unsigned int *)&data_zone_2.byte_2_temp_dht11);
	data_zone_2.real_humi_dht11 = humi;
	convert_dec_to_hex(humi, (unsigned int *)&data_zone_2.byte_1_humi_dht11, (unsigned int *)&data_zone_2.byte_2_humi_dht11);
	emberEventControlSetDelayMS(TimerReadDHT11Control,3000);
}

void function_Recive_Data_Over_Zigbee(EmberAfIncomingMessage* incomingMessage)
{
//	emberAfCorePrintln("\n------------ Data: %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x form address: 0x%2x   ------\n\n",incomingMessage->message[0],
//		incomingMessage->message[1],incomingMessage->message[2],incomingMessage->message[3],incomingMessage->message[4],
//		incomingMessage->message[5],incomingMessage->message[6],incomingMessage->message[7],incomingMessage->message[8],
//		incomingMessage->message[9],incomingMessage->message[10],incomingMessage->message[11],incomingMessage->message[12],
//		incomingMessage->message[13],incomingMessage->message[14],incomingMessage->message[15],incomingMessage->message[16],
//		incomingMessage->message[17],incomingMessage->message[18],incomingMessage->message[19],incomingMessage->message[20],
//		incomingMessage->message[21],incomingMessage->message[22],incomingMessage->message[23],incomingMessage->message[24],
//		incomingMessage->message[25],incomingMessage->message[26],incomingMessage->message[27],incomingMessage->message[28],
//		incomingMessage->message[29],incomingMessage->message[30],incomingMessage->message[31],incomingMessage->message[32],
//		incomingMessage->message[33],incomingMessage->message[34],incomingMessage->message[35],incomingMessage->message[36],
//		incomingMessage->message[37],incomingMessage->message[38],incomingMessage->message[39],incomingMessage->message[40],
//		incomingMessage->source);		//Data se duoc thay tu byte thu 7
//	emberAfCorePrintln("\nData_Length: %u\n",incomingMessage->msgLen);
	for (uint32_t i = 0; i < incomingMessage->msgLen; i++)
	{
		ring_buff_push(&ringbuff, incomingMessage->message[i]);
	}
}

void emberAfHalButtonIsrCallback(uint8_t button, uint8_t status)
{

}
