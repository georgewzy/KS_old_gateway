#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f2xx.h"	  
//#include "stm32f2xx_conf.h"
#include "stm32f2xx_adc.h"
#include "stm32f2xx_dma.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_gpio.h"
//#include "proj_arch.h"
#include "board.h"

#define ADC_CHANNALS	2

#define ADC_SAMPLES	 	AD_SAMPLE_RATE


extern DMA_InitTypeDef DMA_ADC_InitStructure;
extern volatile uint16_t ADC_ConvertedValue[ADC_SAMPLES][ADC_CHANNALS];
extern volatile uint16_t ADC_ConvertedValue_2[ADC_SAMPLES][ADC_CHANNALS];


extern void ADC_init(void);


#endif // End of ADC.h
