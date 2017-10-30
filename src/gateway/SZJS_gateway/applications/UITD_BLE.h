#ifndef __UITD_BLE_H__
#define __UITD_BLE_H__

#include "board.h"
#include "UITD.h"

////extern t_server_handler    *p_UITD_BLE_SZJS;


#define BLE_CMD_RESET   "AT+RESET"       


#define THREAD_UITD_BLE_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_BLE;
extern rt_thread_t thread_UITD_BLE;

#define THREAD_UITD_BLE_RX_STACK_SIZE     2048
extern rt_uint8_t prio_UITD_BLE_rx;
extern rt_thread_t thread_UITD_BLE_rx;

#define THREAD_UITD_BLE_REC_STACK_SIZE     2048
extern rt_uint8_t prio_UITD_BLE_rec;
extern rt_thread_t thread_UITD_BLE_rec;



typedef int (* pFun_BLE_connect_fail)(void *data);

typedef int (* pFun_BLE_connected)(void *data);
typedef int (* pFun_BLE_disconnected)(void *data, int socket);
typedef int (* pFun_BLE_received)(void *data, uint32_t len);
typedef int (* pFun_BLE_rejected)(void *data);
typedef int (* pFun_BLE_closed)(void *data);
typedef int (* pFun_BLE_accepted)(void *data);
typedef int (* pFun_BLE_report_IP)(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port);












extern struct rt_device * device_module_BLE;

extern int module_BLE_init(uint8_t *uart);


extern void rt_thread_entry_UITD_BLE(void* parameter);
extern void rt_thread_entry_UITD_BLE_rx(void* parameter);
extern void rt_thread_entry_UITD_BLE_rec(void* parameter);

#endif // __UITD_BLE_H__
