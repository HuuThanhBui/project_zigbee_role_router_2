#include "app/framework/include/af.h"
#include "Source/Driver/adc.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_iadc.h"
#include "em_gpio.h"

#define CLK_SRC_ADC_FREQ        1000000 //40000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ            10000 // CLK_ADC - 10MHz max in normal mode

// When changing GPIO port/pins below, make sure to change xBUSALLOC macro's
// accordingly - here for PC05 and GND
#define IADC_INPUT_BUS          ABUSALLOC
#define IADC_INPUT_0_BUS        ABUSALLOC
#define IADC_INPUT_0_BUSALLOC   GPIO_CDBUSALLOC_CDEVEN0_ADC0
#define IADC_INPUT_1_BUS        ABUSALLOC
//#define IADC_INPUT_1_BUSALLOC   GPIO_CDBUSALLOC_CDODD0_ADC0
#define IADC_INPUT_1_BUSALLOC   GPIO_ABUSALLOC_AODD0_ADC0

#define TIMER_CYCLES              10000  // 10000 => 100   samples/second
                                         // 1000  => 1000  samples/second
                                         // 200   => 5000  samples/second
                                         // 100   => 10000 samples/second
                                         // 40    => 25000 samples/second

void initIADC(void)		//Resolusion is 12 bit
{
	IADC_Init_t init = IADC_INIT_DEFAULT;
	IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
	IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
	IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;
	init.warmup = iadcWarmupNormal;
	init.timerCycles = TIMER_CYCLES;
	initSingle.fifoDmaWakeup = true;
	initSingle.start = true;
	IADC_reset(IADC0);
//	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_HFRCOEM23);
	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_EM01GRPACLK);
	init.warmup = iadcWarmupKeepWarm;
	init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
	initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
	initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
																	 CLK_ADC_FREQ,
																	 0,
																	 iadcCfgModeNormal,
																	 init.srcClkPrescale);
	// Configure Input sources for single ended conversion
//	initSingleInput.posInput = iadcPosInputPortCPin4;
//	initSingleInput.posInput = iadcPosInputPortCPin5;
	initSingleInput.posInput = iadcPosInputPortAPin3;
	initSingleInput.negInput = iadcNegInputGnd;
	IADC_init(IADC0, &init, &initAllConfigs);
	IADC_initSingle(IADC0, &initSingle, &initSingleInput);
	// Allocate the analog bus for ADC0 inputs
//	GPIO->IADC_INPUT_BUS |= IADC_INPUT_0_BUSALLOC;
	GPIO->IADC_INPUT_BUS |= IADC_INPUT_1_BUSALLOC;

	IADC_command(IADC0, iadcCmdStartSingle);
}

uint32_t IADC_read(void)
{
	uint32_t result = 0;
	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != _IADC_STATUS_SINGLEFIFODV_MASK);
	result = IADC_pullSingleFifoResult(IADC0).data;
	IADC_command(IADC0, iadcCmdStartSingle);
	return result;
}





















































//// Set CLK_ADC to 10MHz
//#define CLK_SRC_ADC_FREQ          20000000 // CLK_SRC_ADC
//#define CLK_ADC_FREQ              10000000 // CLK_ADC - 10MHz max in normal mode
//
///*
// * Specify the IADC input using the IADC_PosInput_t typedef.  This
// * must be paired with a corresponding macro definition that allocates
// * the corresponding ABUS to the IADC.  These are...
// *
// * GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0
// * GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0
// * GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN0_ADC0
// * GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD0_ADC0
// * GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0
// * GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0
// *
// * ...for port A, port B, and port C/D pins, even and odd, respectively.
// */
//#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortCPin5;
//#define IADC_INPUT_1_PORT_PIN     iadcNegInputGnd;
//
//#define IADC_INPUT_0_BUS          CDBUSALLOC
//#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0
//#define IADC_INPUT_1_BUS          CDBUSALLOC
//#define IADC_INPUT_1_BUSALLOC     GPIO_CDBUSALLOC_CDODD0_ADC0
//
//void initIADC(void)
//{
//	CHIP_Init();
//	// Declare init structs
//	IADC_Init_t init = IADC_INIT_DEFAULT;
//	IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
//	IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
//	IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;
//
//	CMU_ClockEnable(cmuClock_IADC0, true);
//	CMU_ClockEnable(cmuClock_GPIO, true);
//
//	IADC_reset(IADC0);
//
//	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);  // FSRCO - 20MHz
//
//	init.warmup = iadcWarmupKeepWarm;
//
//	init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
//
//	// Configuration 0 is used by both scan and single conversions by default
//	// Use unbuffered AVDD (supply voltage in mV) as reference
//	initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
//	initAllConfigs.configs[0].vRef = 3300;
//
//	initAllConfigs.configs[0].twosComplement = iadcCfgTwosCompBipolar; // Force IADC to use bipolar inputs for conversion
//	// Divides CLK_SRC_ADC to set the CLK_ADC frequency
//	initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
//											 CLK_ADC_FREQ,
//											 0,
//											 iadcCfgModeNormal,
//											 init.srcClkPrescale);
//
//	// Assign pins to positive and negative inputs in differential mode
//	initSingleInput.posInput   = iadcPosInputPortCPin5;
//	initSingleInput.negInput   = iadcNegInputGnd;
//
//	// Initialize the IADC
//	IADC_init(IADC0, &init, &initAllConfigs);
//
//	// Initialize the Single conversion inputs
//	IADC_initSingle(IADC0, &initSingle, &initSingleInput);
//
//	// Allocate the analog bus for ADC0 inputs
//	GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
//	GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;
//
//	IADC_command(IADC0, iadcCmdStartSingle);
//}
//
//double IADC_read(void)
//{
//	uint32_t sample = 0;
//	double singleResult = 0.0; // Volts
//	// Wait for conversion to be complete
//	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
//				| _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively
//
//	// Get ADC result
//	sample = IADC_pullSingleFifoResult(IADC0).data;
//
//	// Calculate input voltage:
//	//  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
//	//  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
//	singleResult = (sample * 6.6) / 0xFFF;
//	return singleResult;
//}














