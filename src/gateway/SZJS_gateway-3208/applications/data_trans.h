#ifndef __DATA_TRANS_H__
#define __DATA_TRANS_H__

#include "rtthread.h"
#include "board.h"
#include "COM_if.h"

#define THREAD_COM_IF_STACK_SIZE     4096
#define THREAD_COM_IO_STACK_SIZE     2048

#define COM_TYPE_1_U_SYS_STATUS				1
#define COM_TYPE_2_U_DEV_STATUS	            2
#define COM_TYPE_3_C_SYS_INIT               3
#define COM_TYPE_4_U_SYS_INFO               4
#define COM_TYPE_5_R_SYS_INFO               5
#define COM_TYPE_6_U_COM_DATA               6
#define COM_TYPE_7_C_UARTS_INIT             7



typedef enum
{
    verify_none = 0,
    verify_odd,
    verify_even
} e_COM_uart_verify;


typedef struct
{
    uint8_t             uart_index;
    uint32_t            baud;
    uint8_t             databit;
    e_COM_uart_verify   verify;
    uint8_t             stopbit;
} PACK_STRUCT_STRUCT t_COM_uart_cfg;



extern t_COM_pkt COM_pkt;

extern rt_uint8_t prio_COM_IO;
extern rt_thread_t thread_COM_IO;

extern rt_uint8_t prio_COM_if;
extern rt_thread_t thread_COM_if;

extern struct rt_device * device_COM_if;

extern void rt_thread_entry_COM_if(void* parameter);
extern void rt_thread_entry_COM_IO(void* parameter);


extern int COM_send_COM_data(uint8_t *data, uint32_t len, uint32_t timeout);

extern int COM_IO_check_rec(uint8_t *buf, uint32_t *data_len);

extern int COM_R_sysinfo_affirmed(void *param);
extern int COM_R_sysinfo_denyed(void *param);
extern int COM_R_sysinfo_timeout(void *param);

extern int COM_require_sys_info(void);

#endif // __DATA_TRANS_H__
