#ifndef __PRO_UITD_H__
#define __PRO_UITD_H__


#include "IOT_analog_in.h"
#include "IOT_digit_in.h"
#include "IOT_digit_out.h"

#include "sensor_sample.h"
#include "fire_alarm.h"

#define THREAD_PRO_UITD_STACK_SIZE      2048

extern rt_uint8_t prio_PRO_UITD;
extern rt_thread_t thread_PRO_UITD;



extern void rt_thread_entry_PRO_UITD(void* parameter);




#endif // __PRO_UITD_H__
