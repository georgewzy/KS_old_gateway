#ifndef __STM32_IAP_H__
#define __STM32_IAP_H__

#include "sys_misc.h"
#include "board.h"
//#include "platform_config.h"

extern uint8_t prio_sys_IAP;


extern rt_thread_t thread_sys_IAP;
extern void rt_thread_entry_sys_IAP(void* parameter);







#endif // __STM32_IAP_H__

