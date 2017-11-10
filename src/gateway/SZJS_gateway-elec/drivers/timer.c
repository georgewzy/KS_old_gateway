

#include "stm32f2xx.h"
#include "stm32f2xx_tim.h"
//#include "platform_config.h"
#include "stm32f2xx_gpio.h"
#include "timer.h"
#include "sys_status.h"


TIM_TimeBaseInitTypeDef  TIMx_TimeBaseStructure;
TIM_OCInitTypeDef  			TIMx_OCInitStructure;




/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)	// 2KHZ //
{
//////	static uint8_t Temp[6];
	uint8_t i;
    uint8_t temp_uint8 = 0;
	uint32_t Temp_uint32;

    /* enter interrupt */
    rt_interrupt_enter();
		
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)	    
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	

        //keys_handler();
        
        if (sys_ctrl.timer >= 10*2) // 10ms
        {
            sys_ctrl.timer = 0;
            //sys_ctrl_handler(&sys_ctrl);
        }
        else
        {
            sys_ctrl.timer ++;
        }    
	}
    
    /* leave interrupt */
    rt_interrupt_leave();
}

void tim2_init(void)
{
    
  RCC_ClocksTypeDef RCC_Clocks;
  
  RCC_GetClocksFreq(&RCC_Clocks);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* Time Base configuration */
  /*-------------------------------------------------------------------
  TIM2CLK=72MHz  预分频系数Prescaler=72 经过分频 定时器时钟为1MHz ,重载寄存器值为500 ，定时器周期为0.5ms。
   
  -------------------------------------------------------------------*/
  TIMx_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency*2/1000000) - 1;  // 1us						//预分频器TIM2_PSC=72	 
  TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数器向上计数模式 TIM2_CR1[4]=0
  TIMx_TimeBaseStructure.TIM_Period =500 - 1;				        //自动重装载寄存器TIM2_APR      		     
  TIMx_TimeBaseStructure.TIM_ClockDivision = 0x0;					//时钟分频因子 TIM2_CR1[9:8]=00

  TIM_TimeBaseInit(TIM2,&TIMx_TimeBaseStructure);					//写TIM2各寄存器参数
  
  /* TIM1 counter enable */
   TIM_Cmd(TIM2,ENABLE);											//启动定时器2 TIM2_CR1[0]=1;  
   TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 							//  允许更新中断
}


void tim1_init(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  
  RCC_GetClocksFreq(&RCC_Clocks);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  /* Time Base configuration */
  /*-------------------------------------------------------------------
  TIM1CLK=72MHz  预分频系数Prescaler=72*5 经过分频 定时器时钟为200KHz ,重载寄存器值为0xFFFF 。
   
  -------------------------------------------------------------------*/
  TIMx_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK2_Frequency*2/1000000)*5 - 1;	// 5us.	//预分频器	 
  TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数器向上计数模式
  TIMx_TimeBaseStructure.TIM_Period = 0xFFFF;				        //自动重装载寄存器      		     
  TIMx_TimeBaseStructure.TIM_ClockDivision = 0x0;					//时钟分频因子 
  TIM_TimeBaseInit(TIM1,&TIMx_TimeBaseStructure);					//写TIM1各寄存器参数
  

  /* TIM1 counter enable */
   TIM_Cmd(TIM1,ENABLE);											//启动定时器2 TIM2_CR1[0]=1;  
   TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); 							//  允许更新中断
}

void tim3_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM3_OCInitStructure;
    
  RCC_ClocksTypeDef RCC_Clocks;
  
  RCC_GetClocksFreq(&RCC_Clocks);
    
  
////  RCC_APB2PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE); 	   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;						//PB0复用为TIM3的通道3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;						//PB1复用为TIM3的通道4
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM4 , ENABLE);			 //TIM3局部复用功能开启	 在TIM3的局部复用开启时，PB5会被复用为TIM3_CH2

  TIM3_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency*2/1000000)-1;	// 1us			//预分频器TIM3_PSC=72	 
  TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数器向上计数模式 TIM3_CR1[4]=0
  TIM3_TimeBaseStructure.TIM_Period =100-1;				        //自动重装载寄存器TIM3_APR      		     
  TIM3_TimeBaseStructure.TIM_ClockDivision = 0x0;					//时钟分频因子 TIM3_CR1[9:8]=00

  TIM_TimeBaseInit(TIM3,&TIM3_TimeBaseStructure);					//写TIM3各寄存器参数
  
  TIM3_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1 ;				//TIM3_CCMR1[14:12]=011  翻转 当TIM3_CCR2=TIM3_CNT时，翻转OC2REF的电平 
  TIM3_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输入/捕获2输出允许  OC2信号输出到对应的输出引脚PB5
  TIM3_OCInitStructure.TIM_Pulse = 1;						    //若CC1通道配置为输出：CCR2是装入当前捕获/比较2 TIM3_CCR2寄存器的值（预装载值）。
  																	//当前捕获/比较寄存器包含了与计数器TIM3_CNT比较的值，并且在OC端口上输出信号
  TIM3_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//输出极性  低电平有效 TIM3_CCER[5]=1;

  TIM_OC4Init(TIM3, &TIM3_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
				    
  TIM_OC3Init(TIM3, &TIM3_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);			    

	TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIM1 counter enable */
   ////TIM_Cmd(TIM3,ENABLE);											//启动定时器3 TIM3_CR1[0]=1;  
   //TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE); 							//TIM3_DIER[2]=1  允许捕获/比较2中断
}




/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
		
    /* enter interrupt */
    rt_interrupt_enter();
    
    
 if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)	    
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);			//软件清除中断标志
		

  }
  
 if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)	    
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);			//软件清除中断标志
		
    systick_temp_4 = sys_tick_counter - systick_temp_3;
    systick_temp_3 = sys_tick_counter;
  }
      
    /* leave interrupt */
    rt_interrupt_leave();
}


void tim4_init(void){
//  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_ClocksTypeDef RCC_Clocks;
  
  RCC_GetClocksFreq(&RCC_Clocks);
    //  RCC_APB2PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE); 	   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  

  /* Time Base configuration */
  /*-------------------------------------------------------------------
  TIM4CLK=72MHz  预分频系数Prescaler=72 经过分频 定时器时钟周期为1uS ,重载寄存器值为1000 ，定时器周期为1ms。
   
  -------------------------------------------------------------------*/
  TIM_DeInit(TIM4);

	TIMx_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency*2/1000000)-1;	// 1uS 
  TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数器向上计数模式 TIM2_CR1[4]=0
  //TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIMx_TimeBaseStructure.TIM_Period =(uint16_t)(1000000/(AD_SAMPLE_RATE*2)) - 1;//自动重装载寄存器TIM3_APR      		     
  TIMx_TimeBaseStructure.TIM_ClockDivision = 0x0;					//时钟分频因子 TIM3_CR1[9:8]=00

  TIM_TimeBaseInit(TIM4,&TIMx_TimeBaseStructure);					//写TIM3各寄存器参数
 
  /* Output Compare Toggle Mode configuration: Channel1	  */
  TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIMx_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIMx_OCInitStructure.TIM_Pulse = 1;							
  TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIMx_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIMx_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIMx_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;		
  TIM_OC4Init(TIM4, &TIMx_OCInitStructure);		  

}

