#ifndef __SNTP_H__
#define __SNTP_H__

#include "board.h"


extern int socket_sntp;

extern struct rt_semaphore sem_sntp_done;

//extern uint8_t sntp_start_flag;
extern rt_thread_t thread_sntp;


//extern void thread_sntp_reinit(void);
extern void sntp_init(void);


#endif // __SNTP_H__
