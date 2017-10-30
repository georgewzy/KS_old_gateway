#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__

#include <stdint.h>
#include <stdio.h>

//#include "sys_def.h"
//#include "stm32f4xx_rtc.h"

#define BIN_PATH    "/bin"

#define SYS_CFG_DIR         "/config"
#define SYS_CFG_FILE_PATH   "/config/sys.cfg"


#define ELMFS_DATAFLASH_DIR     "/"
#define SD_DIR                  "/SD"
#define ROMFS_DIR               "/rom"

#define SYS_INIT_FILE_PATH      "/init.rc"
#define SYS_MODULE_SCRIPT_PATH  "/modules.rc"

#define SYS_ISP_BOOT_FW_FILE    "/fw/fw_boot.bin"   // Factory inital firmware. Include bootloader and firmware.
#define SYS_ISP_FIRMWARE_FILE   "/fw/firmware.bin"

#define SYS_ISP_SD_BOOT_FW_FILE   "/SD/fw_boot.bin"


#define SYS_ISP_FW_START_ADDR   0x8020000

#define SYS_RTC_SYNC_ADDR   RTC_BKP_DR1

#define UITD_USART_SUB_BUF_SIZE     256
#define UITD_USART_COM_BUS_SIZE     256

/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/
#define SVR_CFG_PC          0
#define SVR_CFG_KINGSUM     1  

#define SVR_CFG_CHOOSE  SVR_CFG_PC
////#define SVR_CFG_CHOOSE  SVR_CFG_KINGSUM
/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/



#if SVR_CFG_CHOOSE == SVR_CFG_PC

#define SVR_IF_USE_NAME         0
#define SVR_IF_NAME				"quantahealth.oicp.net"   
#define SVR_IF_IP               "114.216.166.89"   
#define SVR_IF_PORT             23366    

#elif SVR_CFG_CHOOSE == SVR_CFG_KINGSUM

#define SVR_IF_USE_NAME         1
#define SVR_IF_NAME				"quantahealth.oicp.net"  
#define SVR_IF_IP               "114.216.166.89"   
#define SVR_IF_PORT             23366    

#endif 







/* serial_configure structure for GSM module */
#define UITD_USART_CFG_GSM           \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}


/* serial_configure structure for WIFI module */
#define UITD_USART_CFG_WIFI           \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}

/* serial_configure structure for extern device communication */
#define UITD_USART_CFG_COM_BUS             \
{                                          \
    BAUD_RATE_115200, /* 19200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    UITD_USART_COM_BUS_SIZE, /* Buffer size */  \
    0                                      \
}

/* serial_configure structure for console */
#define UITD_USART_CFG_CONSOLE             \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}


#endif  // __SYS_CONFIG_H__

