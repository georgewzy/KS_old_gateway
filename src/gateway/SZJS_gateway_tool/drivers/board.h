/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stm32f2xx.h>
#include "rtthread.h"


#define SYS_ISP_ID_EN           1

// Song: Debug mode.
#define DEBUG_SYS_TEST_EN       0

#define IAP_TRIG_KEY_EN         0


/* TOOL support testing */
// <o> TOOL support testing  <1=>Support  <0=>Not suppport
// 	<i>Default: 0
#define TOOL_SUPPORT_TESTING    0


/* TOOL_USE_EXT_FW */
// <o> TOOL use extern SD card fw_boot.bin  <1=>Use  <0=>Not use
// 	<i>Default: 0
#define TOOL_USE_EXT_FW     1

/* board configuration */
// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
// 	<i>Default: 1
#define STM32_USE_SDIO			0


// <o> System IAP mode enable : <0=> Normal mode <1=>IAP mode 
// 	<i>Default: 1
#define SYS_IAP_MODE_EN     0   

#if SYS_IAP_MODE_EN
#define VECTTAB_START   0x20000
#else
#define VECTTAB_START   0x0
#endif  // SYS_IAP_MODE_EN

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#ifdef __ICCARM__
// Use *.icf ram symbal, to avoid hardcode.
extern char __ICFEDIT_region_RAM_end__;
#define STM32_SRAM_END          &__ICFEDIT_region_RAM_end__
#else
#define STM32_SRAM_SIZE         128
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)
#endif

// <o> Console on USART: <0=> no console <1=>USART 1 <2=>USART 2 <3=> USART 3
// 	<i>Default: 1
#define STM32_CONSOLE_USART		3

void rt_hw_board_init(void);

#if STM32_CONSOLE_USART == 0
#define CONSOLE_DEVICE "no"
#elif STM32_CONSOLE_USART == 1
#define CONSOLE_DEVICE "uart1"
#elif STM32_CONSOLE_USART == 2
#define CONSOLE_DEVICE "uart2"
#elif STM32_CONSOLE_USART == 3
#define CONSOLE_DEVICE "uart3"
#elif STM32_CONSOLE_USART == 4
#define CONSOLE_DEVICE "uart4"
#elif STM32_CONSOLE_USART == 5
#define CONSOLE_DEVICE "uart5"
#endif

#define FINSH_DEVICE_NAME   CONSOLE_DEVICE


#define SYS_TESTING_MODE_ADDR   0x20000000
#define SYS_TESTING_MODE_CODE   0xA55A55AA


#define FW_FILE_PATH    "/fw/firmware.bin"
#define FW_UPGRADE_FLAG "/fw/upgrade.flg"
#define FW_FILE_DIR     "/fw"
#define DOWN_FILE_DIR   "/download"


#define STM32_OTP_ADDRESS       0x1FFF7800
#define STM32_OTP_LOCK_ADDRESS  0x1FFF7A00

#define SYS_ID_OTP_ADDR         0x88  // The ID position in the OTP area.
#define SYS_ID_OTP_LOCK_ADDR    ((SYS_ID_OTP_ADDR/32)/4)
#define SYS_ID_ADDRESS          (STM32_OTP_ADDRESS + SYS_ID_OTP_ADDR)
#define SYS_ID_LOCK_ADDRESS     (STM32_OTP_LOCK_ADDRESS + SYS_ID_OTP_LOCK_ADDR*4)

//#define SYS_ID_ADDRESS  0x807FE00

#define SYS_PASSWORD        "SZJSWLWWG"


//#if STM32_USE_SDIO
///**
//  * @brief  SD FLASH SDIO Interface
//  */
////#define SD_DETECT_PIN                    GPIO_Pin_0                 /* PB.0 */
////#define SD_DETECT_GPIO_PORT              GPIOB                       /* GPIOB */
////#define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOB
//   
//#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
///** 
//  * @brief  SDIO Intialization Frequency (400KHz max)
//  */
//#define SDIO_INIT_CLK_DIV                ((uint8_t)0x76)
///** 
//  * @brief  SDIO Data Transfer Frequency (25MHz max) 
//  */
//#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x0) 

//#define SD_SDIO_DMA                   DMA2
//#define SD_SDIO_DMA_CLK               RCC_AHB1Periph_DMA2
// 
//#define SD_SDIO_DMA_STREAM3	          3
////#define SD_SDIO_DMA_STREAM6           6

//#ifdef SD_SDIO_DMA_STREAM3
// #define SD_SDIO_DMA_STREAM            DMA2_Stream3
// #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
// #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
// #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
// #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
// #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
// #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF3 
//#elif defined SD_SDIO_DMA_STREAM6
// #define SD_SDIO_DMA_STREAM            DMA2_Stream6
// #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
// #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
// #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
// #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
// #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
// #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF6 
//#endif /* SD_SDIO_DMA_STREAM3 */


//void SD_LowLevel_DeInit(void);
//void SD_LowLevel_Init(void); 
//void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
//void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);

//#endif

#define COM_TEST_BAUDRATE   19200


#define AD_SAMPLE_RATE      400

// Pin define .
#define PIN_GSM_RING        0
#define PIN_GSM_PWR_SHUT    1
#define PIN_GSM_ON_OFF      2
#define PIN_GSM_RST         3
#define PIN_GSM_STATE_LED   4
#define PIN_IO_IN           5
#define PIN_IO_OUT          6
#define PIN_GPIO_1          7
#define PIN_LED_PWR         8
#define PIN_LED_STATUS      9
#define PIN_EXT_RST         10
#define PIN_TIM1_IN         11
#define PIN_TIM3_OUT        12
#define PIN_ISP_RESET       13
#define PIN_ISP_BOOT        14
#define PIN_BAT_DET         15
#define PIN_BAT_CTRL        16


#define GSM_PIN_PWR_PUSH     PIN_LOW
#define GSM_PIN_PWR_POP      PIN_HIGH  

#define PWR_CTRL_EN         PIN_LOW
#define PWR_CTRL_DIS        PIN_HIGH

#define GSM_PWR_SHUT_EN     PIN_HIGH
#define GSM_PWR_SHUT_DIS    PIN_LOW

#define GSM_RST_EN          PIN_LOW
#define GSM_RST_DIS         PIN_HIGH

#define UITD_UART_WIFI      "uart1"
#define UITD_UART_GSM       "uart2"

// old version AP_TOOL configuration
////#define UITD_UART_COM_BUS   "uart3"

#define UITD_UART_COM_BUS   "uart4"



#define DEV_TYPE_SAMPLE_CTRL        1
#define DEV_TYPE_SIMPLE_UITD        2


typedef enum
{
    dev_type_unknown = 0,
    dev_type_sample_ctrl = DEV_TYPE_SAMPLE_CTRL,
    dev_type_simple_UITD = DEV_TYPE_SIMPLE_UITD,
} e_dev_type;


extern e_dev_type   sys_dev_type;
extern uint8_t sys_board_type_checked;

extern uint32_t sys_cfg;

extern uint8_t sys_board_ID[2];

extern uint8_t sys_main_version;
extern uint8_t sys_sub_version;


extern int32_t svr_use_name;
extern uint8_t svr_if_name[];
extern uint8_t svr_if_ip[15+1];
extern uint16_t svr_if_port;

extern uint8_t sys_fw_update_reboot;
extern uint8_t sys_inited_flag;

extern volatile uint32_t sys_tick_counter;
extern volatile uint32_t systick_temp_1;
extern volatile uint32_t systick_temp_2;
extern volatile uint32_t systick_temp_3;
extern volatile uint32_t systick_temp_4;



extern void rt_hw_board_init(void);
extern rt_uint32_t rt_hw_tick_get_millisecond(void);
extern rt_uint32_t rt_hw_tick_get_microsecond(void);
extern e_dev_type sys_get_board_type(int ID_1, int ID_2);



#endif

