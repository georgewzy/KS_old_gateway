
#include "ADC.h"
#include "sensor_sample.h"

//#define ADC_DR_ADDRESS  ((uint32_t)0x4001244C)  // STM32F10x

#define ADC1_DR_ADDRESS  ((uint32_t)0x4001204C)  // STM32F2xx
#define ADC3_DR_ADDRESS  ((uint32_t)0x4001224C)  // STM32F2xx

#define ADC_DR_ADDRESS      ADC1_DR_ADDRESS  

#define ADC_LEVEL_VALUE_MAX     4096  // 3.0V
#define ADC_LEVEL_NUM_MAX       6     // 7 level: 0, 0.5, 1, 1.5, 2, 2.5, 3 
#define ADC_LEVEL_VALUE_BASE    (ADC_LEVEL_VALUE_MAX/ADC_LEVEL_NUM_MAX)
#define ADC_LEVEL_VALUE_OFFSET  (ADC_LEVEL_VALUE_MAX/ADC_LEVEL_NUM_MAX*2/5)  // 0.2V offset

//uint16_t Dev_IO_Level[7][2] = {0, 
//                    ADC_LEVEL_MAX/6*1 + ADC_LEVEL_MAX/6/4,
//                    ADC_LEVEL_MAX/6*2 + ADC_LEVEL_MAX/6/4,
//                    ADC_LEVEL_MAX/6*3 + ADC_LEVEL_MAX/6/4,
//                    ADC_LEVEL_MAX/6*4 + ADC_LEVEL_MAX/6/4,
//                    ADC_LEVEL_MAX/6*5 + ADC_LEVEL_MAX/6/4,
//                    ADC_LEVEL_MAX/6*6 + ADC_LEVEL_MAX/6/4,
//                    };




static ADC_InitTypeDef ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

DMA_InitTypeDef DMA_ADC_InitStructure;
volatile uint16_t ADC_ConvertedValue[ADC_SAMPLES][ADC_CHANNALS];
volatile uint16_t ADC_ConvertedValue_2[ADC_SAMPLES][ADC_CHANNALS];

                    
                    

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1_Channel1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Stream0_IRQHandler(void)
{
    
    /* enter interrupt */
    rt_interrupt_enter();
    
	 if(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) != RESET)
	 {
			
         
         
        TIM_Cmd(TIM4,DISABLE);


//        ADC_Cmd(ADC1, DISABLE);
//        ADC_DMACmd(ADC1, DISABLE);

         
        DMA_Cmd(DMA2_Stream0, DISABLE);
		if(DMA_ADC_InitStructure.DMA_Memory0BaseAddr == (u32)ADC_ConvertedValue)
		{
			DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
            DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue_2;
            rt_mb_send(&mb_sensor_ADC, (uint32_t)&ADC_ConvertedValue[0][0]);
		}
		else
		{
			DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;
            DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue;
            rt_mb_send(&mb_sensor_ADC, (uint32_t)&ADC_ConvertedValue_2[0][0]);
		}
		////DMA_Init(DMA2_Stream0, &DMA_ADC_InitStructure);
	
        DMA_Cmd(DMA2_Stream0, ENABLE);
//        ADC_DMACmd(ADC1, ENABLE);
//        ADC_Cmd(ADC1, ENABLE);

		////TIM_SetCounter(TIM4,0);
		TIM_Cmd(TIM4,ENABLE);

//         TIM_Cmd(TIM4,DISABLE);
//         
//            ADC_Cmd(ADC1, DISABLE);
//            ADC_DMACmd(ADC1, DISABLE);
//            DMA_Cmd(DMA2_Stream0, DISABLE);
        
        DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
         
	 }
    /* leave interrupt */
    rt_interrupt_leave();
}
   
                    
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the DMA2_Stream0 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIM4 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel =TIM4_IRQn ;			  //
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);      
}


                    
void ADC_init(void)
{

    GPIO_InitTypeDef      GPIO_InitStructure;

    
    NVIC_Configuration();
    
//    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* Enable ADC1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_AHB1Periph_GPIOA , ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    
    /* Configure ADC1 Channel10/15 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    

    /* DMA channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA2_Stream0);
    DMA_ADC_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_ADC_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_ADDRESS;
    DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
    DMA_ADC_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_ADC_InitStructure.DMA_BufferSize = sizeof(ADC_ConvertedValue)/sizeof(ADC_ConvertedValue[0][0]);
    DMA_ADC_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_ADC_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_ADC_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_ADC_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //DMA_ADC_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_ADC_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_ADC_InitStructure.DMA_Priority = DMA_Priority_High;
    //DMA_ADC_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_ADC_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_ADC_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_ADC_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_ADC_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
    DMA_Init(DMA2_Stream0, &DMA_ADC_InitStructure);
  
    DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);

    /* Enable DMA channel1 */
    //DMA_Cmd(DMA2_Stream0, ENABLE);
    
    /* Enable ADC1 DMA */															  
    //ADC_DMACmd(ADC1, ENABLE);		//允许ADC1进行DMA传送
 
    
  /* ADC1 configuration ------------------------------------------------------*/
  // Song: to be tested.
  
    /* Common configuration (applicable for the three ADCs) *********************/
    /* Single ADC mode */
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    /* ADCCLK = PCLK2/8 */
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    /* Available only for multi ADC mode */
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;   ///////////
    /* Delay between 2 sampling phases */
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_15Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);  
    
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	    				
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;						    //扫描模式使能
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//连续转换模式使能
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;	//启动转换的外部事件--TIM4 CC4
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//转换后的数据右对齐
  ADC_InitStructure.ADC_NbrOfConversion = ADC_CHANNALS;								//转换的通道数为2
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_84Cycles);	 // ADC1 IN0 signal 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_84Cycles);	 // ADC1 IN1 signal 

    ////ADC_DiscModeChannelCountConfig(ADC1, 1);
    ////ADC_DiscModeCmd(ADC1, ENABLE);

//  /* ADC1 regular channel0 configuration */ 
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);	  //通道0采样时间
//  /* ADC1 regular channel1 configuration */ 
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);	  //通道1采样时间


    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  
 
  /* Enable ADC1 */
  //ADC_Cmd(ADC1, ENABLE);														  //使能ADC1

//   /* Enable ADC1 reset calibaration register */   
//   ADC_ResetCalibration(ADC1);													  //允许ADC1复位校准寄存器 
//   /* Check the end of ADC1 reset calibration register */
//   while(ADC_GetResetCalibrationStatus(ADC1));									  //检测校准寄存器是否复位完成 

//   /* Start ADC1 calibaration */
//   ADC_StartCalibration(ADC1);													  //启动ADC1 校准
//   /* Check the end of ADC1 calibration */
//   while(ADC_GetCalibrationStatus(ADC1));									      //检测校准是否完成 
  
	
	//ADC_ExternalTrigConvCmd(ADC1, ENABLE);   
    //////ADC1->CR2 |= ADC_CR2_EXTEN;

  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);										  //软件触发启动ADC1转换

}



int ADC_get_level(uint16_t level)
{
    int i = 0;
    
    if (level < ADC_LEVEL_VALUE_OFFSET)
    {
        return 0;
    }
    
    for (i=1;i<(ADC_LEVEL_NUM_MAX+1);i++)
    {
        if ((level > (i*ADC_LEVEL_VALUE_BASE - ADC_LEVEL_VALUE_OFFSET)) && 
            (level < (i*ADC_LEVEL_VALUE_BASE + ADC_LEVEL_VALUE_OFFSET)))
        {
            return i;
        }
    }
    
    return -1;
}


int ADC_board_check(e_dev_type *dev_type)
{
    uint32_t    i = 0;
    int ID_1 = 0;
    int ID_2 = 0;
   ADC_InitTypeDef       ADC_InitStructure;
   ADC_CommonInitTypeDef ADC_CommonInitStructure;
   DMA_InitTypeDef       DMA_InitStructure;
   GPIO_InitTypeDef      GPIO_InitStructure;
    
    uint16_t ADC3ConvertedValue[2] = {0};

   /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

   /* DMA2 Stream0 channel2 configuration **************************************/
   DMA_InitStructure.DMA_Channel            = DMA_Channel_2;  
   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
   DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)ADC3ConvertedValue;
   DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
   DMA_InitStructure.DMA_BufferSize         = 2 ;
   DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable; //DMA_MemoryInc_Disable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
   DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
   DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
   DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
   DMA_Init(DMA2_Stream0, &DMA_InitStructure);
   DMA_Cmd(DMA2_Stream0, ENABLE);

   /* Configure ADC3 Channel5 Ff7 pin as analog input ******************************/
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   /* ADC Common Init **********************************************************/
   ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div8;
   ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);

   /* ADC3 Init ****************************************************************/
   ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode         = ENABLE;//DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
   ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion      = 2;
   ADC_Init(ADC3, &ADC_InitStructure);

   /* ADC3 regular channel5 configuration *************************************/
   ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);
   ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 2, ADC_SampleTime_112Cycles);

  /* Enable DMA request after last transfer (Single-ADC mode) */
   //ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
   /* Enable ADC3 DMA */
   ADC_DMACmd(ADC3, ENABLE);
   /* Enable ADC3 */
   ADC_Cmd(ADC3, ENABLE);

   ADC_SoftwareStartConv(ADC3);
   
   while(1)
   {
        if (ADC3ConvertedValue[1] != 0)
        {
            i = 10000;
            while(i--);
            break;
        }
        
        i = 10000;
        while(i--);
        
   }

   ADC_Cmd(ADC3, DISABLE);
   
   // Song: TODO
   ID_1 = ADC_get_level(ADC3ConvertedValue[0]);  //AP_LED_POWER pin voltage.
   ID_2 = ADC_get_level(ADC3ConvertedValue[1]);  //AP_LED_STATUS pin voltage.
   
   ADC_DeInit();
   
   *dev_type = sys_get_board_type(ID_1, ID_2);
   
   rt_kprintf(" Board ID : %d - %d \n Dev_type : %d\n", ID_1, ID_2, *dev_type);
   
   return 0;
}
