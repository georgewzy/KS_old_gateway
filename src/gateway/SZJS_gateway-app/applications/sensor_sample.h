#ifndef __SENSOR_SAMPLE_H__
#define __SENSOR_SAMPLE_H__

#include "IOT_analog_in.h"
#include "IOT_digit_out.h"



#define THREAD_SENSOR_SAMPLE_STACK_SIZE     2048

extern rt_uint8_t prio_sensor_sample;
extern rt_thread_t thread_sensor_sample;


extern void rt_thread_entry_sensor_sample(void* parameter);



#endif // __SENSOR_SAMPLE_H__
