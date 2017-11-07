#ifndef __UITD_MAIN_H__
#define __UITD_MAIN_H__

#include <rtthread.h>
#include "UITD.h"


#define THREAD_UITD_MAIN_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_main;
extern rt_thread_t thread_UITD_main;

#define THREAD_UITD_2_MAIN_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_2_main;
extern rt_thread_t thread_UITD_2_main;


#define THREAD_UITD_GPRS_REC_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_GPRS_rec;
extern rt_thread_t thread_UITD_GPRS_rec;


extern struct rt_semaphore sem_UITD_main_rx;
extern struct rt_semaphore sem_UITD_main_tx;

extern struct rt_device * device_sub_muc;

extern int UITD_sensor_handler_init(t_server_handler *handler);
extern int UITD_sensor_handler_2_init(t_server_handler *handler);

extern void rt_thread_entry_UITD_main(void* parameter);
extern void rt_thread_entry_UITD_2_main(void* parameter);
extern void rt_thread_entry_UITD_GPRS_rec(void* parameter);

extern int UITD_server_info(void);

#endif // __UITD_MAIN_H__
