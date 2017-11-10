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
#include "stm32f2xx_iwdg.h"
#include "board.h"
#include "usart.h"
#include "gpio.h"

#include "sys_def.h"
#include "sys_config.h"
#include "fire_alarm.h"
#include "transparent.h"
#include "pro_ctrl.h"



#if defined(FINSH_USING_SYMTAB) && !defined(FINSH_USING_MSH_ONLY)
#include <finsh.h>
uint32_t sys_cfg = 0;
FINSH_VAR_EXPORT(sys_cfg, finsh_type_int, System configed flag.)
#endif

#if PATCH_UITD_ALIVE_EN
volatile uint32_t  patch_alive_cnt = 0;
#endif // PATCH_UITD_ALIVE_EN        


// SYS_TESTING_MODE_CODE : 0xA55A55AA @ 0x20000000
////uint32_t sys_testing_mode __attribute__((at(SYS_TESTING_MODE_ADDR)));

s_sys_cfg   sys_config = 
{
    1,
	0,
    {SVR_IF_USE_NAME, SVR_IF_NAME, SVR_IF_IP, SVR_IF_PORT, 0},
    {SVR_2_IF_USE_NAME, SVR_2_IF_NAME, SVR_2_IF_IP, SVR_2_IF_PORT, 1},
 
    0,
    0,
    0,
    {0},  
    
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    
    SYS_DEBUG_LEVEL, // debug_level
    SYS_LOG_FILE_EN, // log_file_en
    SYS_LOG_FILE_PATH, // log_file_path
    dev_type_unknown,
    mother_type_unknown,
    dev_type_unknown,
    
    0,
    0,
    0,
    0,
    
    0,
    
};
RTM_EXPORT(sys_config); // Song: Added at 2017-3-22 13:36:54

s_eth_cfg eth_cfg = 
{
    1,
    "192.168.1.199",
    "192.168.1.1",
    "255.255.255.0",
    "114.114.114.114",
};

s_mode_cfg mode_cfg = 
{
    MODE_DEFAULT,
    MODE_DEFAULT_GPRS,
    MODE_DEFAULT_ETH,
    MODE_DEFAULT_WIFI,
    {0},
};


s_WIFI_cfg WIFI_cfg = 
{
    WIFI_SSID_DEFAULT,
    WIFI_PW_DEFAULT,
};

uint32_t g_sensor_period[SENSOR_CHANNEL_MAX] = {0};

uint32_t g_sensor_period_1 = 0;
uint32_t g_sensor_period_2 = 0;
uint32_t g_sensor_period_3 = 0;
uint32_t g_sensor_period_4 = 0;

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
e_mother_type   sys_mother_type = mother_type_unknown;


uint8_t board_type_checked = 0;

volatile uint32_t sys_tick_counter = 0;
volatile uint32_t sys_100ms_counter = 0;

volatile uint32_t systick_temp_1 = 0;
volatile uint32_t systick_temp_2 = 0;
volatile uint32_t systick_temp_3 = 0;
volatile uint32_t systick_temp_4 = 0;


const s_mother_ID mother_ID_table[] = 
{
    {MOTHER_TYPE_UNKNOWN,   0 *2, "MOTHER_TYPE_UNKNOWN"},
    {MOTHER_TYPE_GPRS_1_2,  1   , "MOTHER_TYPE_GPRS_1_2"},
    {MOTHER_TYPE_GPRS,      1 *2, "MOTHER_TYPE_GPRS"},
    {MOTHER_TYPE_ETH,       2 *2, "MOTHER_TYPE_ETH"},
    {MOTHER_TYPE_GPRS_ETH,  3 *2, "MOTHER_TYPE_GPRS_ETH"},
    {MOTHER_TYPE_IOT_PRO ,  4 *2, "MOTHER_TYPE_IOT_PRO"},
    {MOTHER_TYPE_PWR_DET,   9,    "MOTHER_TYPE_PWR_DET"},
    {MOTHER_TYPE_EC_AP,     10,   "MOTHER_TYPE_EC_AP"},
    {MOTHER_TYPE_EC_TERM,   11,   "MOTHER_TYPE_EC_TERM"},
};



const s_dev_ID dev_ID_table[] = 
{
    {DEV_TYPE_UNKNOWN,          0, 0,     "DEV_TYPE_UNKNOWN"},
    {DEV_TYPE_SAMPLE_CTRL,      6, 6,     "DEV_TYPE_SAMPLE_CTRL"},
    {DEV_TYPE_SIMPLE_UITD,      6, 8,     "DEV_TYPE_SIMPLE_UITD"},
    {DEV_TYPE_CONTROLLER,       6, 7,     "DEV_TYPE_CONTROLLER"},////
    {DEV_TYPE_WIRE_EXTEND_BAT,  6, 5,     "DEV_TYPE_WIRE_EXTEND_BAT"},////
    {DEV_TYPE_UITD_BAT,         6, 4,     "DEV_TYPE_UITD_BAT"},//
    {DEV_TYPE_SAMPLE_CTRL_BAT,  6, 3,     "DEV_TYPE_SAMPLE_CTRL_BAT"},//
    {DEV_TYPE_CONTROLLER_BAT,   6, 2,     "DEV_TYPE_CONTROLLER_BAT"},//
    {DEV_TYPE_UITD_3UART_BAT,   6, 1,     "DEV_TYPE_UITD_3UART_BAT"},//
    {DEV_TYPE_FULL_FUNC_BAT,    4, 4,     "DEV_TYPE_FULL_FUNC_BAT"},
    {DEV_TYPE_IOT_PRO_UITD,     5, 1,     "DEV_TYPE_IOT_PRO_UITD"},//
    {DEV_TYPE_IOT_PRO_CTRL,     4, 1,     "DEV_TYPE_IOT_PRO_CTRL"},//
    {DEV_TYPE_EC_SENSOR_01,     2, 1,     "DEV_TYPE_EC_SENSOR_01"},//
    {DEV_TYPE_PWR_DET,          2, 2,     "DEV_TYPE_PWR_DET"},//

    // MODULE and MODE ID  must more than  6 - 8
    {DEV_TYPE_MODULE_WIFI,      7, 9, "DEV_TYPE_MODULE_WIFI"},
    {DEV_TYPE_MODULE_BLE,       7, 10, "DEV_TYPE_MODULE_BLE"},////
    {DEV_TYPE_MODULE_WIFI_BLE,  7, 11, "DEV_TYPE_MODULE_WIFI_BLE"},
    
    {DEV_TYPE_MODE_TEST,       11, 9, "DEV_TYPE_MODE_TEST"},
    {DEV_TYPE_MODE_FORMAT,    11, 10, "DEV_TYPE_MODE_FORMAT"},
    {DEV_TYPE_MODE_RESET,      11, 11, "DEV_TYPE_MODE_RESET"},
    {DEV_TYPE_MODE_RECONFIG,   11, 12, "DEV_TYPE_MODE_RECONFIG"},
    {DEV_TYPE_MODE_BOOT,       10,  9, "DEV_TYPE_MODE_BOOT"},
    {DEV_TYPE_MODE_DEBUG,      10, 10, "DEV_TYPE_MODE_DEBUG"},
    
    
    
//    {DEV_TYPE_FUN_FORMAT_FS, 5, 1},
//    {DEV_TYPE_FUN_INIT_CFG, 5, 2},
//    {DEV_TYPE_TESTING_MODE, 6, 0},
//    {DEV_TYPE_TESTING_MODE, 6, 1},
//    {DEV_TYPE_TESTING_MODE, 6, 2},
//    {DEV_TYPE_TESTING_MODE, 6, 3},
//    {DEV_TYPE_TESTING_MODE, 6, 4},
//    {DEV_TYPE_TESTING_MODE, 6, 5},
//    {DEV_TYPE_TESTING_MODE, 6, 6},
};



//const uint8_t module_ID_table[][3] = 
//{
//    {MODULE_TYPE_UNKNOWN, 0, 0},
//    {MODULE_TYPE_BLE, 3, 3},
//    {MODULE_TYPE_WIFI, 3, 4},
//    {MODULE_TYPE_WIFI_BLE, 3, 5},
//    {MODULE_TYPE_MODE_RESET, 5, 3},
//    {MODULE_TYPE_MODE_FORMATE, 5, 4},
//    {MODULE_TYPE_MODE_TEST, 5, 5},
//    
//};

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


 /**
 * 初始化独立看门狗
 * prer:分频数:0~7(只有低 3 位有效!)
 * 分频因子=4*2^prer.但最大值只能是 256!
 * rlr:重装载寄存器值:低 11 位有效.
 * 时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
 */
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* 使能对寄存器IWDG_PR和IWDG_RLR的写操作*/
    IWDG_SetPrescaler(prer);    /*设置IWDG预分频值:设置IWDG预分频值*/
    IWDG_SetReload(rlr);     /*设置IWDG重装载值*/
    IWDG_ReloadCounter();    /*按照IWDG重装载寄存器的值重装载IWDG计数器*/
    IWDG_Enable();        /*使能IWDG*/
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
    static uint32_t iwdg_cnt = 0;
    /* enter interrupt */
    rt_interrupt_enter();

    //rt_tick_increase();
    
    cnt ++;
    if (cnt >= (1000000/RT_TICK_PER_SECOND/1000))  // 1000000us / tick_per_sec(100) / 1000us = 10.
    {
        rt_tick_increase();
        cnt = 0;
    }

#if PATCH_UITD_ALIVE_EN
    patch_alive_cnt ++;
    if (patch_alive_cnt > (1000*60*PATCH_ALIVE_TIMEOUT))  // 1ms * 1000 * 60 * xxx min 
    {
        //SYS_log(SYS_DEBUG_ERROR, ("Patch_UITD_alive : timeout, reboot !!!\n"));
        sys_reboot();
        while(1);
    }
#endif // PATCH_UITD_ALIVE_EN 
    
    #if SYS_WDG_EN
    iwdg_cnt ++;
    if (iwdg_cnt >= (100000/1000)) // 100mS
    {
        IWDG_ReloadCounter();
        iwdg_cnt = 0;
    }
    #endif // SYS_WDG_EN
    
    if (p_com_bus_cb->status == com_bus_status_alive)
    {
        p_com_bus_cb->alive_timer ++;
    }
    
    sys_tick_counter ++;

    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        sys_config.test_flag.counter ++;
    }
    
    if (p_transparent_cb)
    {
        if (p_transparent_cb->counter >= p_transparent_cb->cfg.trans_period)
        {
            p_transparent_cb->counter = 0;
            p_transparent_cb->tick_flag = 1;
        }
        p_transparent_cb->counter ++;
    }
    
    sys_100ms_counter ++;
    if (sys_100ms_counter >= 100)
    {
        sys_100ms_counter = 0;
        
        if (p_IO_input_cb != NULL)
        {
            p_IO_input_cb->TS_flag = 1;
        }
        
        if (p_output_ctrl_cb != NULL)
        {
            p_output_ctrl_cb->TS_flag = 1;
        }        
        
//        if (p_PRO_output_ctrl_cb != NULL)
//        {
//            p_PRO_output_ctrl_cb->TS_flag = 1;
//        }
        
    }
    
    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This function will configure the MCO1 as the HSE output pin.
 *
 */
void mco_config(void)
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


e_mother_type sys_get_mother_board_type(int ID)
{
    int i = 0;
    int mother_type_num = sizeof(mother_ID_table)/sizeof(mother_ID_table[0]);
    
    if (ID < 0)
    {
        return mother_type_unknown;
    }
   
    for (i=0;i<mother_type_num;i++)
    {
        if (mother_ID_table[i].ID == ID)
        {
            return mother_ID_table[i].mother_type;
        }
    }
    
    return mother_type_unknown;
}

// extern board ID is all even number.
e_dev_type sys_get_ext_board_type(int ID_1, int ID_2)
{
    int i = 0;
    int dev_type_num = sizeof(dev_ID_table)/sizeof(dev_ID_table[0]);
    
    if ((ID_1 < 0) || (ID_2 < 0))
    {
        return dev_type_unknown;
    }

    if ((ID_1 <= 6) && (ID_2 <= 8))
    {
        for (i=0;i<dev_type_num;i++)
        {
            if ((dev_ID_table[i].ID_1 == ID_1) && (dev_ID_table[i].ID_2 == ID_2))
            {
                return dev_ID_table[i].dev_type;
            }
        }
    }
    
    
    return dev_type_unknown;
}

// extern board ID is all odd number.
e_dev_type sys_get_ext_module_type(int ID_1, int ID_2)
{
    int i = 0;
    int dev_type_num = sizeof(dev_ID_table)/sizeof(dev_ID_table[0]);
    
    if ((ID_1 <= 0) || (ID_2 <= 0))
    {
        return dev_type_unknown;
    }
    
    if ((ID_1 <= 6) && (ID_2 <= 8))
    {
        return dev_type_unknown;
    }
    else
    {
   
        for (i=0;i<dev_type_num;i++)
        {
            if ((dev_ID_table[i].ID_1 == ID_1) && (dev_ID_table[i].ID_2 == ID_2))
            {
                return dev_ID_table[i].dev_type;
            }
        }
    }   
    
    return dev_type_unknown;
}

uint8_t * sys_get_mother_board_string(e_mother_type mother_type)
{
    int i = 0;
    int mother_type_num = sizeof(mother_ID_table)/sizeof(mother_ID_table[0]);
 
    for (i=0;i<mother_type_num;i++)
    {
        if (mother_ID_table[i].mother_type == mother_type)
        {
            return (uint8_t *)mother_ID_table[i].type_name;
        }
    }
    
    if (i == mother_type_num)
    {
        return (uint8_t *)mother_ID_table[0].type_name;
    }
}


uint8_t * sys_get_ext_dev_module_string(e_dev_type dev_type)
{
    int i = 0;
    int dev_type_num = sizeof(dev_ID_table)/sizeof(dev_ID_table[0]);
 
    for (i=0;i<dev_type_num;i++)
    {
        if (dev_ID_table[i].dev_type == dev_type)
        {
            return (uint8_t *)dev_ID_table[i].type_name;
        }
    }
    
    if (i == dev_type_num)
    {
        return (uint8_t *)dev_ID_table[0].type_name;
    }
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
    
    #if SYS_WDG_EN
    IWDG_Init(4, 256); // 409mS
    #endif // SYS_WDG_EN

    stm32_hw_usart_init();
    stm32_hw_pin_init();
    
    
#ifdef RT_USING_CONSOLE
    rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/
