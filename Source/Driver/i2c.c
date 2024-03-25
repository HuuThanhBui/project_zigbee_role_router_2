#include "Source/Driver/i2c.h"
#include "app/framework/include/af.h"
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"

#define I2C_TXBUFFER_SIZE	10
#define I2C_RXBUFFER_SIZE   10

//static volatile bool i2c_startTx;

void initCMU(void)
{
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
}

void initI2C(void)
{
	// Use default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	// Using PB1 (SDA) and PB0 (SCL)
	GPIO_PinModeSet(gpioPortB, 0, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortB, 1, gpioModeWiredAndPullUpFilter, 1);

	// Route I2C pins to GPIO
	GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
						| (gpioPortB << _GPIO_I2C_SDAROUTE_PORT_SHIFT
						| (1 << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
						| (gpioPortB << _GPIO_I2C_SCLROUTE_PORT_SHIFT
						| (0 << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;


	// Initialize the I2C
	I2C_Init(I2C0, &i2cInit);


	// Enable automatic STOP on NACK
	I2C0->CTRL = I2C_CTRL_AUTOSN;

//	I2C_Enable(I2C0, true);
}

void I2C_Read(uint16_t SensorAddress, uint8_t targetAddress, uint8_t *rxBuff, uint8_t numBytes)
{
	// Transfer structure
	I2C_TransferSeq_TypeDef i2cTransfer;
	I2C_TransferReturn_TypeDef result;

	uint8_t TxBuffer[1]; //Just a buffer to
	TxBuffer[0] = targetAddress;

	// Initialize I2C transfer
	i2cTransfer.addr          = SensorAddress;
	i2cTransfer.flags         = I2C_FLAG_WRITE_READ; // must write target address before reading
	i2cTransfer.buf[0].data   = TxBuffer;
	i2cTransfer.buf[0].len    = 1;
	i2cTransfer.buf[1].data   = rxBuff;
	i2cTransfer.buf[1].len    = numBytes;

	result = I2C_TransferInit(I2C0, &i2cTransfer);

	// Send data
	while (result == i2cTransferInProgress) {
		result = I2C_Transfer(I2C0);
	}
}

void I2C_Write(uint8_t address, uint8_t reg, uint8_t *value)
{
  // Transfer structure
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef result;

  uint8_t TxBuffer[2];
  TxBuffer[0] = reg;
  TxBuffer[1] = *value;

  // Initializing I2C transfer
  i2cTransfer.addr          = address;
  i2cTransfer.flags         = I2C_FLAG_WRITE;
  i2cTransfer.buf[0].data   = TxBuffer;
  i2cTransfer.buf[0].len    = 2;

  result = I2C_TransferInit(I2C0, &i2cTransfer);

  // Sending data
  while (result == i2cTransferInProgress)
  {
    result = I2C_Transfer(I2C0);
  }

}


