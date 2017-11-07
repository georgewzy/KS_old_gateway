#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "IOT_digit_in.h"
#include "IOT_digit_out.h"


#define THREAD_CTRL_IN_STACK_SIZE     512
#define THREAD_CTRL_OUT_STACK_SIZE     512


extern rt_uint8_t prio_ctrl_in;
extern rt_thread_t thread_ctrl_in;

extern rt_uint8_t prio_ctrl_out;
extern rt_thread_t thread_ctrl_out;


extern void rt_thread_entry_ctrl_in(void* parameter);
extern void rt_thread_entry_ctrl_out(void* parameter);


#endif // __CONTROLLER_H__
