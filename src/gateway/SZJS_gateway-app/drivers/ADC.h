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


// Song: maximum channal number.
#define ADC_CHANNALS	SENSOR_CHANNEL_MAX

#define ADC_SAMPLES	 	AD_SAMPLE_RATE


extern DMA_InitTypeDef DMA_ADC_InitStructure;

// Song: TODO: optimize the RAM used.
//extern volatile uint16_t ADC_ConvertedValue[ADC_SAMPLES][ADC_CHANNALS];
//extern volatile uint16_t ADC_ConvertedValue_2[ADC_SAMPLES][ADC_CHANNALS];

extern volatile uint16_t *ADC_ConvertedValue;
extern volatile uint16_t *ADC_ConvertedValue_2;


extern void ADC_init(void *data);
extern void ADC_PRO_init(void *data);


#endif // End of ADC.h
