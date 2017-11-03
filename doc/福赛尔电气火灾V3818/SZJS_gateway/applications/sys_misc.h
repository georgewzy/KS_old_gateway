#ifndef __SYS_MISC_H__
#define __SYS_MISC_H__

#include "sys_config.h"
//#include "sys_def.h"

#define BACKUP_IAP_FLAG     RTC_BKP_DR3
#define BACKUP_IAP_VALUE    0x5AA5






//#define SYS_DEBUG_ERROR          0x01
//#define SYS_DEBUG_WARNING        0x02
//#define SYS_DEBUG_INFO           0x04
//#define SYS_DEBUG_DEBUG          0x08
//#define SYS_DEBUG_LEVEL          (SYS_DEBUG_INFO | SYS_DEBUG_WARNING | SYS_DEBUG_ERROR | SYS_DEBUG_DEBUG)
//////#define SYS_DEBUG_LEVEL          (SYS_DEBUG_INFO | SYS_DEBUG_WARNING | SYS_DEBUG_ERROR )


extern uint8_t sys_dbg_level;
/* #define SYS_DEBUG */
#ifdef SYS_DEBUG
#ifdef RT_USING_LOGTRACE
#define SYS_log(level, x)        do { if (level & sys_config.debug_level)      \
    { rt_kprintf x;   \
        if (sys_config.log_file_en && sys_config.sys_inited_flag) {log_trace x; log_trace_flush();}   \
    }}while (0)
//#define SYS_log(level, x)        do { if (level & SYS_DEBUG_LEVEL)      \
//    {rt_kprintf("SYS %s, %d:", __FUNCTION__, __LINE__); rt_kprintf x;   \
//    rt_kprintf ("\n");}}while (0)
    
#define SYS_log_HEX(level, x)        do { if (level & sys_config.debug_level)      \
    { SYS_print_hex_dump x;   \
    }}while (0)
//#define SYS_log_HEX(level, x)        do { if (level & SYS_DEBUG_LEVEL)      \
//    {rt_kprintf("SYS %s, %d:", __FUNCTION__, __LINE__); SYS_print_hex_dump x;   \
//    rt_kprintf ("\n");}}while (0)    
#else
#define SYS_log(level, x)        do { if (level & sys_config.debug_level)      \
    { rt_kprintf x;   \
    }}while (0)
//#define SYS_log(level, x)        do { if (level & SYS_DEBUG_LEVEL)      \
//    {rt_kprintf("SYS %s, %d:", __FUNCTION__, __LINE__); rt_kprintf x;   \
//    rt_kprintf ("\n");}}while (0)
    
#define SYS_log_HEX(level, x)        do { if (level & sys_config.debug_level)      \
    { SYS_print_hex_dump x;   \
    }}while (0)
//#define SYS_log_HEX(level, x)        do { if (level & SYS_DEBUG_LEVEL)      \
//    {rt_kprintf("SYS %s, %d:", __FUNCTION__, __LINE__); SYS_print_hex_dump x;   \
//    rt_kprintf ("\n");}}while (0)    
    
#endif // RT_USING_LOGTRACE
#else
#define SYS_log(level, x)
#define SYS_log_HEX(level, x)
#endif  // SYS_DEBUG


    
typedef struct
{
    uint32_t     SEN01_report_period;
    uint32_t     SEN02_report_period;
} s_AP01_cfg;


typedef struct
{
    uint32_t     SEN01_report_period;
    uint32_t     SEN02_report_period;
    uint32_t     SEN03_report_period;
    uint32_t     SEN04_report_period;
    uint8_t      SW01_trig_cfg;
    uint32_t     SW01_report_period;

} s_PRO_UITD_cfg;


typedef struct
{
    uint32_t     SEN01_report_period;
    uint32_t     SEN02_report_period;
    uint32_t     SEN03_report_period;
    uint32_t     SEN04_report_period;
    uint8_t      SW01_trig_cfg;
    uint32_t     SW01_report_period;
    uint8_t      SW02_trig_cfg;
    uint32_t     SW02_report_period;
    uint8_t      SW03_trig_cfg;
    uint32_t     SW03_report_period;
    uint8_t      SW04_trig_cfg;
    uint32_t     SW04_report_period;
    uint8_t      SW05_trig_cfg;
    uint32_t     SW05_report_period;
    uint8_t      SW06_trig_cfg;
    uint32_t     SW06_report_period;
    uint8_t      SW07_trig_cfg;
    uint32_t     SW07_report_period;
    uint8_t      SW08_trig_cfg;
    uint32_t     SW08_report_period;
} s_PRO_CTRL_cfg;


//typedef struct
//{
//    uint32_t    FA_type;
//    uint32_t    FA_baud;
//    uint32_t    FA_listen;
//    uint8_t     SW01_trig_cfg;
//    uint32_t    SW01_report_period;
//} s_AP02_cfg;    


typedef struct
{
    uint8_t     FA_en_uart0;
    uint8_t     FA_name_uart0[16];
    uint32_t    FA_type_uart0;
    uint32_t    FA_baud_uart0;
    uint32_t    FA_listen_uart0;
    
    uint8_t     FA_en_uart1;
    uint8_t     FA_name_uart1[16];
    uint32_t    FA_type_uart1;
    uint32_t    FA_baud_uart1;
    uint32_t    FA_listen_uart1;
    
    uint8_t     FA_en_uart2;
    uint8_t     FA_name_uart2[16];
    uint32_t    FA_type_uart2;
    uint32_t    FA_baud_uart2;
    uint32_t    FA_listen_uart2;
    
    uint8_t     FA_en_uart3;
    uint8_t     FA_name_uart3[16];
    uint32_t    FA_type_uart3;
    uint32_t    FA_baud_uart3;
    uint32_t    FA_listen_uart3;
    
    uint8_t     FA_en_uart4;
    uint8_t     FA_name_uart4[16];
    uint32_t    FA_type_uart4;
    uint32_t    FA_baud_uart4;
    uint32_t    FA_listen_uart4;
    
    uint8_t     SW01_trig_cfg;
    uint32_t    SW01_report_period;
    
} s_FA_cfg;   

//wzy
typedef struct
{
    uint8_t     sys_addr;
    uint32_t    addr_area;
    uint32_t    addr_line; 
    
} s_ELEC_cfg; 
//wzy
extern s_ELEC_cfg *p_ELEC_cfg;

    
typedef struct
{
    uint8_t     if_DHCP;
    uint8_t     IP[16];
    uint8_t     GW[16];
    uint8_t     MASK[16];
    uint8_t     DNS[16];
} s_eth_cfg;

typedef struct
{
    uint8_t     SSID[32];
    uint8_t     PW[32];
} s_WIFI_cfg;


typedef struct
{
    uint8_t GPRS_com;
    uint8_t GPRS_SZJS;
    uint8_t GPRS_XFZD;
    uint8_t ETH_com;
    uint8_t ETH_SZJS;
    uint8_t ETH_XFZD;
    uint8_t WIFI_com;
    uint8_t WIFI_SZJS;
    uint8_t WIFI_XFZD;

} s_mode_thread;

typedef struct
{
    uint8_t         mode;  // 0:One connection per physical link, default mode, 1: Two connections per physical link.
    uint8_t         GPRS;
    uint8_t         ETHERNET;
    uint8_t         WIFI;
    s_mode_thread   thread;
} s_mode_cfg;

typedef struct
{
    uint32_t    baudrate;
    uint8_t     bits;
    uint8_t     verify;
    uint8_t     stopbit;
} s_transparent_com_cfg;

typedef struct
{
    uint32_t                trans_period;  // unit ms.
    uint32_t                buffer_period; // unit ms.
    
    uint8_t                 uart[8];
    s_transparent_com_cfg   cfg;
    
} s_transparent_cfg;

extern uint8_t sys_version_flag;
extern uint8_t sys_version_buf[3];
extern uint8_t sensor_fw_updated;

extern const char sys_init_file_content[];


//#include "arch/cc.h"
////#include "sys_def.h"
//#include "lwip/err.h"
//typedef unsigned char  u8_t;
//typedef unsigned short u16_t;
//typedef unsigned int   u32_t;
//typedef signed char    s8_t;
//typedef signed short   s16_t;
//typedef signed int     s32_t;

extern uint8_t console_output_enable;

extern struct VirtEeprom eeprom_buf;
extern const struct VirtEeprom eeprom_init_data;
extern const s_WIFI_cfg WIFI_cfg_init_data;
//extern const s_mode_cfg mode_cfg_init_data;
//extern struct t_sys_cfg sys_cfg_buf;

extern const s_FA_cfg FA_cfg_init_data;
extern const s_PRO_UITD_cfg PRO_UITD_cfg_init_data;
extern const s_PRO_CTRL_cfg PRO_CTRL_cfg_init_data;
extern const s_AP01_cfg AP01_cfg_init_data;
//extern const s_AP02_cfg AP02_cfg_init_data;
extern const struct t_sys_cfg sys_cfg_init_data;
extern const s_eth_cfg eth_cfg_init_data;
extern const s_transparent_cfg transparent_cfg_init_data;

extern const s_mode_cfg mode_cfg_init_data_GPRS;
extern const s_mode_cfg mode_cfg_init_data_ETH;
extern const s_mode_cfg mode_cfg_init_data_GPRS_ETH;
extern const s_mode_cfg mode_cfg_init_data_IOT_PRO;
extern const s_mode_cfg mode_cfg_init_data_EC_AP;
extern const s_mode_cfg mode_cfg_init_data_EC_TERM;

extern void SYS_print_hex_dump(uint8_t *str, uint8_t *buf, uint32_t len);

//extern u32_t ntohl(u32_t n);
//extern u16_t htons(u16_t n);
//extern u16_t ntohs(u16_t n);
//extern u32_t htonl(u32_t n);

//extern void Print_caller_adr(uint8_t* file, uint32_t line);
//extern const char *lwip_strerr(err_t err);

//extern uint8_t parity_check_uchar(uint8_t x);

//#define print_caller_adr()		Print_caller_adr((uint8_t *)__FILE__, __LINE__)


#endif // End of sys_misc.h
