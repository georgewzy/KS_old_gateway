#ifndef __SYS_MISC_H__
#define __SYS_MISC_H__

#include "sys_config.h"


#define BACKUP_IAP_FLAG     RTC_BKP_DR3
#define BACKUP_IAP_VALUE    0x5AA5


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


extern struct VirtEeprom eeprom_buf;
extern const struct VirtEeprom eeprom_init_data;

//extern struct t_sys_cfg sys_cfg_buf;
extern const struct t_sys_cfg sys_cfg_init_data;


//extern u32_t ntohl(u32_t n);
//extern u16_t htons(u16_t n);
//extern u16_t ntohs(u16_t n);
//extern u32_t htonl(u32_t n);

//extern void Print_caller_adr(uint8_t* file, uint32_t line);
//extern const char *lwip_strerr(err_t err);

//extern uint8_t parity_check_uchar(uint8_t x);

//#define print_caller_adr()		Print_caller_adr((uint8_t *)__FILE__, __LINE__)


#endif // End of sys_misc.h
