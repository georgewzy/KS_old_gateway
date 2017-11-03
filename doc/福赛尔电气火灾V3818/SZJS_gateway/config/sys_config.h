#ifndef __SYS_CONFIG_H____
#define __SYS_CONFIG_H____

#include <stdint.h>
#include <stdio.h>

//#include "fire_alarm.h"
//#include "sys_def.h"
//#include "stm32f4xx_rtc.h"


#define SYS_DEBUG

#define SYS_SN_LEN  13
#define SYS_SN_MAX  9999999999999

#define WIFI_SSID_DEFAULT   "SZJS_BF4B"
#define WIFI_PW_DEFAULT     "szjs1234"

#define WIFI_SSID_TEST      "szjstest"
#define WIFI_PW_TEST        "szjstest"


#define BIN_PATH    "/bin"

#define SYS_CFG_DIR         "/config"
#define SYS_CFG_FILE_PATH   "/config/sys.cfg"
#define ELEC_FIRE_CFG_FILE_PATH	"/config/elec.cfg"	//wzy
#define AP01_CFG_FILE_PATH  "/config/AP01.cfg"
#define AP02_CFG_FILE_PATH  "/config/AP02.cfg"
#define ETH_CFG_FILE_PATH   "/config/eth.cfg"
#define WIFI_CFG_FILE_PATH  "/config/WIFI.cfg"
#define MODE_CFG_FILE_PATH  "/config/mode.cfg"
#define TRANS_CFG_FILE_PATH "/config/transparent.cfg"
#define PRO_CTRL_CFG_FILE_PATH      "/config/PRO_CTRL.cfg"
#define PRO_UITD_CFG_FILE_PATH      "/config/PRO_UITD.cfg"
#define FA_CFG_FILE_PATH            "/config/FA.cfg"


#define ELMFS_DATAFLASH_DIR     "/"
#define SD_DIR                  "/SD"
#define ROMFS_DIR               "/rom"
#define SCRIPT_DIR              "/script"
#define MODULE_DIR              "/module"
#define MODULE_LIB_DIR          "/module/lib"  // must as same as the MACRO MODULE_ROOT_DIR which in "dlopen.c"

#define SYS_INIT_FILE_PATH      "/init.rc"
#define SYS_MODULE_SCRIPT_PATH  "/modules.rc"

#define SYS_TESTING_FILE_PATH   "/test_log.txt"
#define SYS_LOG_INFO_NAME       "logs.txt"

#define SYS_RTC_SYNC_ADDR           RTC_BKP_DR1
#define SYS_RTC_TEST_MODE           RTC_BKP_DR2
#define SYS_RTC_FW_UPGRADE_ADDR     RTC_BKP_DR3


#define UITD_USART_SUB_BUF_SIZE     256
#define UITD_USART_COM_BUS_SIZE     256

/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/
#define SVR_CFG_PC          0
#define SVR_CFG_KINGSUM     1  

////#define SVR_CFG_CHOOSE  SVR_CFG_PC
#define SVR_CFG_CHOOSE  SVR_CFG_KINGSUM
/*///////////////////////////////////////////////////////////////////////////////////////////////////////*/

#define MODE_CFG_MULT_WAY       0
#define MODE_CFG_SINGLE_WAY     1

#define MODE_DEFAULT        MODE_CFG_MULT_WAY
#define MODE_DEFAULT_GPRS   2
#define MODE_DEFAULT_ETH    1
#define MODE_DEFAULT_WIFI   0



#if SVR_CFG_CHOOSE == SVR_CFG_PC

#define SVR_IF_USE_NAME         1
#define SVR_IF_NAME				"1q5a419933.51mypc.cn"   
#define SVR_IF_IP               "114.216.166.89"   
#define SVR_IF_PORT             23367    

#define SVR_2_IF_USE_NAME       1
#define SVR_2_IF_NAME			"1q5a419933.51mypc.cn"   
#define SVR_2_IF_IP             "114.216.166.89"   
#define SVR_2_IF_PORT           23366    

#elif SVR_CFG_CHOOSE == SVR_CFG_KINGSUM

#define SVR_IF_USE_NAME         1
#define SVR_IF_NAME				"platform.kingsum.biz"  
////#define SVR_IF_NAME				"quantahealth.oicp.net"  
#define SVR_IF_IP               "180.169.14.34"   
#define SVR_IF_PORT             4009    

#define SVR_2_IF_USE_NAME       1
#define SVR_2_IF_NAME			"platform.kingsum.biz"  
////#define SVR_2_IF_NAME				"quantahealth.oicp.net"  
#define SVR_2_IF_IP             "180.169.14.34"   
#define SVR_2_IF_PORT           4009    

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
    BAUD_RATE_57600, /* 9600 bits/s */  \
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


/* serial_configure structure for console */
#define VK32_USART_CFG_CONSOLE             \
{                                          \
    BAUD_RATE_9600, /* 9600 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}



#endif  // __SYS_CONFIG_H__

