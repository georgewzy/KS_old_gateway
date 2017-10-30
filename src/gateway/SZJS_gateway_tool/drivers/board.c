/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f2xx.h"
#include "board.h"
#include "usart.h"
#include "gpio.h"

#include "sys_def.h"
#include "sys_config.h"
#include "fire_alarm.h"



#if defined(FINSH_USING_SYMTAB) && !defined(FINSH_USING_MSH_ONLY)
#include <finsh.h>
uint32_t sys_cfg = 0;
FINSH_VAR_EXPORT(sys_cfg, finsh_type_int, System configed flag.)
#endif


int32_t svr_use_name = 1;
uint8_t svr_if_name[SVR_IF_NAME_LEN] = SVR_IF_NAME;
uint8_t svr_if_ip[15+1] = SVR_IF_IP;
uint16_t svr_if_port = SVR_IF_PORT;

uint8_t sys_fw_update_reboot = 0;

uint8_t sys_inited_flag = 0;

//uint8_t sys_board_ID[2] = {0, 0};

uint8_t sys_main_version = 0;
uint8_t sys_sub_version = 0;

e_dev_type   sys_dev_type = dev_type_unknown;
uint8_t sys_board_type_checked = 0;

volatile uint32_t sys_tick_counter = 0;
volatile uint32_t systick_temp_1 = 0;
volatile uint32_t systick_temp_2 = 0;
volatile uint32_t systick_temp_3 = 0;
volatile uint32_t systick_temp_4 = 0;

const uint8_t dev_ID_table[][3] = 
{
    {DEV_TYPE_SAMPLE_CTRL, 3, 3},
    {DEV_TYPE_SIMPLE_UITD, 3, 4},
};

/**
 * @addtogroup STM32
 */

/*@{*/



//#if STM32_USE_SDIO


///**
//  * @brief  DeInitializes the SDIO interface.
//  * @param  None
//  * @retval None
//  */
//void SD_LowLevel_DeInit(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  
//  /*!< Disable SDIO Clock */
//  SDIO_ClockCmd(DISABLE);
//  
//  /*!< Set Power State to OFF */
//  SDIO_SetPowerState(SDIO_PowerState_OFF);

//  /*!< DeInitializes the SDIO peripheral */
//  SDIO_DeInit();
//  
//  /* Disable the SDIO APB2 Clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);

//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_MCO);

//  /* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* Configure PD.02 CMD line */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

//  /* Configure PC.12 pin: CLK pin */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//}

///**
//  * @brief  Initializes the SD Card and put it into StandBy State (Ready for 
//  *         data transfer).
//  * @param  None
//  * @retval None
//  */
//void SD_LowLevel_Init(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;

//  /* GPIOC and GPIOD Periph clock enable */
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE);

//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);

//  /* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* Configure PD.02 CMD line */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

//  /* Configure PC.12 pin: CLK pin */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//  
////  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
////  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
////  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

//  /* Enable the SDIO APB2 Clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

//  /* Enable the DMA2 Clock */
//  RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);
//}

///**
//  * @brief  Configures the DMA2 Channel4 for SDIO Tx request.
//  * @param  BufferSRC: pointer to the source buffer
//  * @param  BufferSize: buffer size
//  * @retval None
//  */
//void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
//{
//  DMA_InitTypeDef SDDMA_InitStructure;

//  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

//  /* DMA2 Stream3  or Stream6 disable */
//  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

//  /* DMA2 Stream3  or Stream6 Config */
//  DMA_DeInit(SD_SDIO_DMA_STREAM);

//  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
//  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
//  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
//  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//  SDDMA_InitStructure.DMA_BufferSize = 0;
//  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
//  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
//  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
//  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);

//  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

//  /* DMA2 Stream3  or Stream6 enable */
//  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
//    
//}

///**
//  * @brief  Configures the DMA2 Channel4 for SDIO Rx request.
//  * @param  BufferDST: pointer to the destination buffer
//  * @param  BufferSize: buffer size
//  * @retval None
//  */
//void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
//{
//  DMA_InitTypeDef SDDMA_InitStructure;

//  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

//  /* DMA2 Stream3  or Stream6 disable */
//  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

//  /* DMA2 Stream3 or Stream6 Config */
//  DMA_DeInit(SD_SDIO_DMA_STREAM);

//  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
//  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
//  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferDST;
//  SDDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//  SDDMA_InitStructure.DMA_BufferSize = 0;
//  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
//  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
//  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
//  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);

//  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

//  /* DMA2 Stream3 or Stream6 enable */
//  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
//}

///**
//  * @brief  Returns the DMA End Of Transfer Status.
//  * @param  None
//  * @retval DMA SDIO Stream Status.
//  */
//uint32_t SD_DMAEndOfTransferStatus(void)
//{
//  return (uint32_t)DMA_GetFlagStatus(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_TCIF);
//}
//#endif


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECTTAB_START);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

//	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;
	cnts = ((rcc_clocks.SYSCLK_Frequency)/1000000) * 1000;  // 1ms systick

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    static uint32_t cnt = 0;
    
    /* enter interrupt */
    rt_interrupt_enter();

    //rt_tick_increase();
    
    cnt ++;
    if (cnt >= (1000000/RT_TICK_PER_SECOND/1000))  // 1000000us / tick_per_sec(100) / 1000us = 10.
    {
        rt_tick_increase();
        cnt = 0;
    }

//    if (p_com_bus_cb->status == com_bus_status_alive)
//    {
//        p_com_bus_cb->alive_timer ++;
//    }
//    
    sys_tick_counter ++;


    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This function will configure the MCO1 as the HSE output pin.
 *
 */
static void mco_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* MCO configure */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
}


/**
 * This fucntion returns milliseconds since system passed
 */
rt_uint32_t rt_hw_tick_get_millisecond(void)
{
    rt_tick_t tick;
    rt_uint32_t value;

#define TICK_MS (1000/RT_TICK_PER_SECOND)

    tick = rt_tick_get();
    value = tick * TICK_MS + (SysTick->LOAD - SysTick->VAL) * TICK_MS / SysTick->LOAD;

    return value;
}

/**
 * This fucntion returns microseconds since system passed
 */
rt_uint32_t rt_hw_tick_get_microsecond(void)
{
    rt_tick_t tick;
    rt_uint32_t value;

#define TICK_US	(1000000/RT_TICK_PER_SECOND)

    tick = rt_tick_get();
    value = tick * TICK_US + (SysTick->LOAD - SysTick->VAL) * TICK_US / SysTick->LOAD;

    return value;
}


e_dev_type sys_get_board_type(int ID_1, int ID_2)
{
    int i = 0;
    int dev_type_num = sizeof(dev_ID_table)/sizeof(dev_ID_table[0]);
    
    if ((ID_1 < 0) || (ID_2 < 0))
    {
        return dev_type_unknown;
    }
    
    for (i=0;i<dev_type_num;i++)
    {
        if ((dev_ID_table[i][1] == ID_1) && (dev_ID_table[i][2] == ID_2))
        {
            return (e_dev_type)dev_ID_table[i][0];
        }
    }
    
    return dev_type_unknown;
}


/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Configuration();

    stm32_hw_usart_init();
    stm32_hw_pin_init();
    
    //mco_config();
    
#ifdef RT_USING_CONSOLE
    rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/
