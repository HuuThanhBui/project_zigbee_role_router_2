#ifndef _SEND_H_
#define _SEND_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct have_been_sent address_have_been_sent_t;		//Cau truc dua lieu cua mot phan tu trong mang luu dia chi phuc vu cho viec dap send loop
struct have_been_sent{
	uint32_t address;
	uint8_t status;
};
void reset_arr(void);
void SEND_ReportInfoToHc(void);
void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID, EmberAfAttributeId attributeId, uint8_t globalCommand, uint16_t *value, uint8_t length, uint8_t dataType);
void SEND_SendCommandUnicast(uint8_t source, uint8_t destination, uint16_t address);
void SEND_OnOffStateReport(uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t value);
void SEND_PIRStateReport(uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t value);
void SEND_LDRValue(uint8_t endpoint_source, uint8_t endpoint_destination, uint16_t value);
void SEND_Data(uint16_t address, uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t *value);

#endif
