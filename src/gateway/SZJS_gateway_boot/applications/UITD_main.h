#ifndef __UITD_MAIN_H__
#define __UITD_MAIN_H__

#include <rtthread.h>



#define THREAD_UITD_MAIN_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_main;
extern rt_thread_t thread_UITD_main;

#define THREAD_UITD_MAIN_REC_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_main_rec;
extern rt_thread_t thread_UITD_main_rec;


extern struct rt_semaphore sem_UITD_main_rx;
extern struct rt_semaphore sem_UITD_main_tx;

extern struct rt_device * device_sub_muc;

extern void rt_thread_entry_UITD_main(void* parameter);
extern void rt_thread_entry_UITD_main_rec(void* parameter);


#endif // __UITD_MAIN_H__
