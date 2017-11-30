
#include "ADC.h"
#include "sensor_sample.h"
#include "pro_ctrl.h"

//#define ADC_DR_ADDRESS  ((uint32_t)0x4001244C)  // STM32F10x

#define ADC1_DR_ADDRESS  ((uint32_t)0x4001204C)  // STM32F2xx
#define ADC3_DR_ADDRESS  ((uint32_t)0x4001224C)  // STM32F2xx

#define ADC_DR_ADDRESS      ADC1_DR_ADDRESS  

#define ADC_LEVEL_VALUE_MAX     4096  // 3.0V
#define ADC_LEVEL_NUM_MAX       12     // 13 level: 0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3 
#define ADC_LEVEL_VALUE_BASE    (ADC_LEVEL_VALUE_MAX/ADC_LEVEL_NUM_MAX)
#define ADC_LEVEL_VALUE_OFFSET  (ADC_LEVEL_VALUE_MAX/ADC_LEVEL_NUM_MAX*2/5)  // 0.1V offset

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



//volatile uint16_t ADC_ConvertedValue[ADC_SAMPLES][ADC_CHANNALS];
//volatile uint16_t ADC_ConvertedValue_2[ADC_SAMPLES][ADC_CHANNALS];

volatile uint16_t *ADC_ConvertedValue = NULL;             
volatile uint16_t *ADC_ConvertedValue_2 = NULL;           

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
         
        if (p_sensor_cb)
        {
            if(DMA_ADC_InitStructure.DMA_Memory0BaseAddr == (u32)ADC_ConvertedValue)
            {
                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue_2;
                rt_mb_send(mb_sensor_ADC, (uint32_t)ADC_ConvertedValue);
            }
            else
            {
                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;
                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue;
                rt_mb_send(mb_sensor_ADC, (uint32_t)ADC_ConvertedValue_2);
            }
        }
//        else if (p_PRO_sensor_cb)
//        {
//            if(DMA_ADC_InitStructure.DMA_Memory0BaseAddr == (u32)ADC_ConvertedValue)
//            {
//                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
//                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue_2;
//                rt_mb_send(&mb_PRO_sensor_ADC, (uint32_t)&ADC_ConvertedValue[0][0]);
//            }
//            else
//            {
//                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;
//                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue;
//                rt_mb_send(&mb_PRO_sensor_ADC, (uint32_t)&ADC_ConvertedValue_2[0][0]);
//            }
//        }
        else
        {
            if(DMA_ADC_InitStructure.DMA_Memory0BaseAddr == (u32)ADC_ConvertedValue)
            {
                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue_2;
            }
            else
            {
                DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue;
                DMA2_Stream0->M0AR = (u32)ADC_ConvertedValue;
            }
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


                    
void ADC_init(void *data)
{

    GPIO_InitTypeDef      GPIO_InitStructure;
    s_sensor_cb *cb = (s_sensor_cb *)data;

    ADC_ConvertedValue = rt_malloc(sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    if (ADC_ConvertedValue == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc ADC buffer failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(ADC_ConvertedValue, 0x00, sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    }    
    
    ADC_ConvertedValue_2 = rt_malloc(sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    if (ADC_ConvertedValue_2 == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc ADC buffer failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(ADC_ConvertedValue_2, 0x00, sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    }     
    
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
    DMA_ADC_InitStructure.DMA_BufferSize = ADC_SAMPLES * cb->sensor_num;
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
  ADC_InitStructure.ADC_NbrOfConversion = 2;								//转换的通道数为2
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

// 4 channal
void ADC_PRO_init(void *data)
{

    GPIO_InitTypeDef      GPIO_InitStructure;
    s_sensor_cb *cb = (s_sensor_cb *)data;

    ADC_ConvertedValue = rt_malloc(sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    if (ADC_ConvertedValue == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc ADC buffer failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(ADC_ConvertedValue, 0x00, sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    }    
    
    ADC_ConvertedValue_2 = rt_malloc(sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    if (ADC_ConvertedValue_2 == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc ADC buffer failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(ADC_ConvertedValue_2, 0x00, sizeof(uint16_t) * ADC_SAMPLES * cb->sensor_num);
    }      
    
    
    NVIC_Configuration();
    
//    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* Enable ADC1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    
    /* Configure ADC1 Channel10/15 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure ADC1 Channel10/15 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure ADC1 Channel10/15 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    /* DMA channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA2_Stream0);
    DMA_ADC_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_ADC_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_ADDRESS;
    DMA_ADC_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_ConvertedValue_2;
    DMA_ADC_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_ADC_InitStructure.DMA_BufferSize = ADC_SAMPLES * cb->sensor_num;
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
  ADC_InitStructure.ADC_NbrOfConversion = 4;								//转换的通道数为2
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_84Cycles);	 // ADC1 IN8 signal 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_84Cycles);	 // ADC1 IN9 signal 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10,3, ADC_SampleTime_84Cycles);	 // ADC1 IN10 signal 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_84Cycles);	 // ADC1 IN3 signal 

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


int ADC_mother_board_check(e_mother_type *mother_type)
{
    volatile uint32_t    i = 0;
    int ID = 0;
   ADC_InitTypeDef       ADC_InitStructure;
   ADC_CommonInitTypeDef ADC_CommonInitStructure;
   DMA_InitTypeDef       DMA_InitStructure;
   GPIO_InitTypeDef      GPIO_InitStructure;
    
    uint16_t ADC1ConvertedValue[10] = {0};
    uint16_t ADC1_sum = 0;
    uint16_t ADC1_diff_sum = 0;

    // PA6  ADC1_IN6
   /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//   /* DMA2 Stream0 channel2 configuration **************************************/
//   DMA_InitStructure.DMA_Channel            = DMA_Channel_2;  
//   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
//   DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)ADC1ConvertedValue;
//   DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
//   DMA_InitStructure.DMA_BufferSize         = 2 ;
//   DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
//   DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable; //DMA_MemoryInc_Disable;
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//   DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
//   DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
//   DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
//   DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
//   DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
//   DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
//   DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
//   DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//   DMA_Cmd(DMA2_Stream0, ENABLE);

   /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   GPIO_SetBits(GPIOA, GPIO_Pin_6);
   rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS


   /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* ADC Common Init **********************************************************/
   ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div8;
   ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);

   /* ADC1 Init ****************************************************************/
   ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode         = DISABLE;//DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
   ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_Ext_IT11;
   ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion      = 1;
   ADC_Init(ADC1, &ADC_InitStructure);

   /* ADC1 regular channe6 configuration *************************************/
   ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_112Cycles);

  /* Enable DMA request after last transfer (Single-ADC mode) */
   //ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
   /* Enable ADC3 DMA */
   //ADC_DMACmd(ADC1, ENABLE);
   /* Enable ADC3 */

   //Waiting for Vcc and Avcc stable.
   rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS

   ADC_Cmd(ADC1, ENABLE);

    ADC1_sum = 0;
    for (i=0;i<10;i++)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS
        ADC_SoftwareStartConv(ADC1);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        ADC1ConvertedValue[i] = ADC_GetConversionValue(ADC1);
        ADC1_sum += ADC1ConvertedValue[i];
    }
   
    ADC_Cmd(ADC1, DISABLE);
   
    
    ADC1_sum /= 10;
    ADC1_diff_sum = 0;
    for (i=0;i<10;i++)
    {
        ADC1_diff_sum += (ADC1ConvertedValue[i]>ADC1_sum)? (ADC1ConvertedValue[i] - ADC1_sum):(ADC1_sum - ADC1ConvertedValue[i]);
    }
    
    if (ADC1_diff_sum > (ADC_LEVEL_VALUE_OFFSET/2 * 10))
    {
        ADC_DeInit();
        *mother_type = mother_type_GPRS_1_2;
        rt_kprintf(" Mother board ID : %d   type : %d - %s\n", MOTHER_TYPE_UNKNOWN, *mother_type, sys_get_mother_board_string(*mother_type));
        return 0;
    }
    

//   /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
//   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//   GPIO_Init(GPIOA, &GPIO_InitStructure);


    ADC_Cmd(ADC1, ENABLE);

    ADC1_sum = 0;
    for (i=0;i<10;i++)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS
        ADC_SoftwareStartConv(ADC1);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        ADC1ConvertedValue[i] = ADC_GetConversionValue(ADC1);
        ADC1_sum += ADC1ConvertedValue[i];
    }
    
    ADC_Cmd(ADC1, DISABLE);
   
    ADC1_sum /= 10;
    ID = ADC_get_level(ADC1_sum);  //AP_LED_POWER pin voltage.

    ADC_DeInit();
   
    // If ID is 0, we set mother type as type_GPRS_1_2 by default.
    if (ID == 0)
    {
        *mother_type = mother_type_GPRS_1_2;
    }
    else
    {
        *mother_type = sys_get_mother_board_type(ID);
    }
    
    rt_kprintf(" Mother board ID : %d   type : %d - %s\n", ID, *mother_type, sys_get_mother_board_string(*mother_type));
    
    return 0;
}

int ADC_ext_board_module_check(e_dev_type *dev_type, e_dev_type *module_type)
{
    volatile uint32_t    i = 0;
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
   ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_Ext_IT11;
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

//    // delay about 240mS.
//   i = 10000000;
//   while(i--);

    // Initalize and enable the ext_module ID output.
    rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT_OD);   
    rt_pin_write(PIN_ID_DETECT      , PIN_LOW);

    rt_thread_delay(RT_TICK_PER_SECOND/10); // delay 100mS
    ADC_SoftwareStartConv(ADC3);
    rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS

   ADC_DMACmd(ADC3, DISABLE);
   ADC_Cmd(ADC3, DISABLE);
   ADC_DeInit();
   
   // Song: TODO
   ID_1 = ADC_get_level(ADC3ConvertedValue[0]);  //AP_LED_POWER pin voltage.
   ID_2 = ADC_get_level(ADC3ConvertedValue[1]);  //AP_LED_STATUS pin voltage.
   
//   ADC_DeInit();
   
   *dev_type = sys_get_ext_board_type(ID_1, ID_2);
   
   rt_kprintf(" Extend board ID : %d - %d    type : %d -- %s\n", ID_1, ID_2, *dev_type, sys_get_ext_dev_module_string(*dev_type));
   
   // Enable the ext_module ID output.
   rt_pin_write(PIN_ID_DETECT      , PIN_HIGH);
   

    DMA_DeInit(DMA2_Stream0);
   DMA_Init(DMA2_Stream0, &DMA_InitStructure);
   DMA_Cmd(DMA2_Stream0, ENABLE);
   ADC_CommonInit(&ADC_CommonInitStructure);
   ADC_Init(ADC3, &ADC_InitStructure);

   ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);
   ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 2, ADC_SampleTime_112Cycles);


   ADC_DMACmd(ADC3, ENABLE);
   ADC_Cmd(ADC3, ENABLE);
   
    rt_thread_delay(RT_TICK_PER_SECOND/10); // delay 100mS
    ADC_SoftwareStartConv(ADC3);
    rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS
   
   ADC_Cmd(ADC3, DISABLE);
   ADC_DMACmd(ADC3, DISABLE);
   ADC_DeInit();

   ID_1 = ADC_get_level(ADC3ConvertedValue[0]);  //AP_LED_POWER pin voltage.
   ID_2 = ADC_get_level(ADC3ConvertedValue[1]);  //AP_LED_STATUS pin voltage.
   
   *module_type = sys_get_ext_module_type(ID_1, ID_2);
   
   rt_kprintf(" Extend module ID : %d - %d   type : %d --- %s\n", ID_1, ID_2, *module_type, sys_get_ext_dev_module_string(*module_type));


//   if (*dev_type == dev_type_sample_ctrl)
//   {
//        // Disable this pin, reconfig to input, this dev board not support extern module.
//       rt_pin_mode(PIN_ID_DETECT, PIN_MODE_INPUT);
//   }
//   else
   {
        // Disable the ext_module ID output.
        rt_pin_write(PIN_ID_DETECT      , PIN_LOW);
   }
   
   return 0;
}


// retrun: 1 - support power_battery check, 0 - not support.
int ADC_power_battery_init(void)
{
    volatile uint32_t    i = 0;
   ADC_InitTypeDef       ADC_InitStructure;
   ADC_CommonInitTypeDef ADC_CommonInitStructure;
   DMA_InitTypeDef       DMA_InitStructure;
   GPIO_InitTypeDef      GPIO_InitStructure;
    
    uint16_t ADC2ConvertedValue[10] = {0};
    uint16_t ADC2_sum = 0;
    uint16_t ADC2_diff_sum = 0;

    // PA0  ADC1_IN0
   /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    if ((sys_config.mother_type == mother_type_GPRS_1_2) || (sys_config.mother_type == mother_type_GPRS))
    {
       /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
       GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 ;
       GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       GPIO_SetBits(GPIOA, GPIO_Pin_1);
       rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS


       /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
       GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 ;
    //   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       /* ADC Common Init **********************************************************/
       ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
       ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div8;
       ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
       ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
       ADC_CommonInit(&ADC_CommonInitStructure);

       /* ADC1 Init ****************************************************************/
       ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
       ADC_InitStructure.ADC_ScanConvMode         = DISABLE;//DISABLE;
       ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
       ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
       ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_Ext_IT11;
       ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
       ADC_InitStructure.ADC_NbrOfConversion      = 1;
       ADC_Init(ADC2, &ADC_InitStructure);

       /* ADC1 regular channe6 configuration *************************************/
       ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_112Cycles);
    }
    else
    {
       /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
       GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 ;
       GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       GPIO_SetBits(GPIOA, GPIO_Pin_0);
       rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS


       /* Configure ADC1 Channel6 PA6 pin as analog input ******************************/
       GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 ;
    //   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  // push down the PA6 inside, check if PA6 is float.
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       /* ADC Common Init **********************************************************/
       ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
       ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div8;
       ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
       ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
       ADC_CommonInit(&ADC_CommonInitStructure);

       /* ADC1 Init ****************************************************************/
       ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
       ADC_InitStructure.ADC_ScanConvMode         = DISABLE;//DISABLE;
       ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
       ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
       ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_Ext_IT11;
       ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
       ADC_InitStructure.ADC_NbrOfConversion      = 1;
       ADC_Init(ADC2, &ADC_InitStructure);

       /* ADC1 regular channe6 configuration *************************************/
       ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_112Cycles);
    }
    


   //Waiting for Vcc and Avcc stable.
   rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS

   ADC_Cmd(ADC2, ENABLE);

    ADC2_sum = 0;
    for (i=0;i<10;i++)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS
        ADC_SoftwareStartConv(ADC2);
        while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
        ADC2ConvertedValue[i] = ADC_GetConversionValue(ADC1);
        ADC2_sum += ADC2ConvertedValue[i];
    }
   
    ADC_Cmd(ADC2, DISABLE);
   
    
    ADC2_sum /= 10;
    ADC2_diff_sum = 0;
    for (i=0;i<10;i++)
    {
        ADC2_diff_sum += (ADC2ConvertedValue[i]>ADC2_sum)? (ADC2ConvertedValue[i] - ADC2_sum):(ADC2_sum - ADC2ConvertedValue[i]);
    }
    
    if (ADC2_diff_sum > (ADC_LEVEL_VALUE_OFFSET/2 * 10))
    {
        ADC_DeInit();

        sys_config.battery_check = 0;
        
        SYS_log(SYS_DEBUG_INFO, ("Not support battery check !\n"));
        return 0;
    }

    ADC_Cmd(ADC2, ENABLE);

    ADC2_sum = 0;
    for (i=0;i<10;i++)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100); // delay 10mS
        ADC_SoftwareStartConv(ADC2);
        while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
        ADC2ConvertedValue[i] = ADC_GetConversionValue(ADC2);
        ADC2_sum += ADC2ConvertedValue[i];
    }
    
    ADC_Cmd(ADC2, DISABLE);
   
    ADC2_sum /= 10;
    //ID = ADC_get_level(ADC1_sum);  //AP_LED_POWER pin voltage.

    sys_config.battery_check = 1;
    //ADC_DeInit();
   
    return 1;
}




// Return : -1 - error, 0-5000 - voltage(unit 10mV).
int16_t ADC_power_battery_sample(void)
{
    int16_t voltage = -1;
    uint32_t ADC_res = 0;
    
    ADC_Cmd(ADC2, ENABLE);

    
    ADC_SoftwareStartConv(ADC2);
    while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
    ADC_res = (uint32_t)ADC_GetConversionValue(ADC2);

    ADC_Cmd(ADC2, DISABLE);
   
    voltage = (int16_t)((ADC_res * 3000)/4096 * 2);

    return voltage;
}



