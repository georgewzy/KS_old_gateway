#ifndef __UITD_WIFI_H__
#define __UITD_WIFI_H__

#include <rtthread.h>
#include "UITD.h"


#define THREAD_UITD_WIFI_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_WIFI;
extern rt_thread_t thread_UITD_WIFI;

#define THREAD_UITD_2_WIFI_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_2_WIFI;
extern rt_thread_t thread_UITD_2_WIFI;


#define THREAD_UITD_WIFI_REC_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_WIFI_rec;
extern rt_thread_t thread_UITD_WIFI_rec;


extern struct rt_semaphore sem_UITD_WIFI_rx;
extern struct rt_semaphore sem_UITD_WIFI_tx;

//extern int UITD_sensor_handler_init(t_server_handler *handler);
//extern int UITD_sensor_handler_2_init(t_server_handler *handler);

extern void rt_thread_entry_UITD_WIFI(void* parameter);
extern void rt_thread_entry_UITD_2_WIFI(void* parameter);
extern void rt_thread_entry_UITD_WIFI_rec(void* parameter);

extern int UITD_WIFI_server_info(void);

#endif // __UITD_WIFI_H__