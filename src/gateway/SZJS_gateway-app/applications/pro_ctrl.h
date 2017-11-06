#ifndef __PRO_CTRL_H__
#define __PRO_CTRL_H__

#include "IOT_analog_in.h"
#include "IOT_digit_in.h"
#include "IOT_digit_out.h"

#include "sensor_sample.h"
#include "fire_alarm.h"


#define PRO_SENSOR_SAMPLE_PERIOD_DEFAULT        10  // unit : S


#define THREAD_PRO_CTRL_STACK_SIZE     4096

typedef enum
{
    e_PRO_output_sta_idle = 0,
    e_PRO_output_sta_init,
    e_PRO_output_inited,
    e_PRO_output_sta_working,
    e_PRO_output_sta_stop,
    e_PRO_output_sta_error
} e_PRO_OUTPUT_status;



typedef struct
{
    uint8_t             ID;
    uint8_t             pin;
    uint8_t             valid;
    uint8_t             waiting;
    e_output_action     action;
    e_output_action     action_last;
    uint16_t            counter;
    uint32_t            delay;
} s_PRO_output_ctrl;



typedef struct
{
    uint16_t        *data;
    e_sensor_type   type;
} s_PRO_sensor_data_rough;

typedef struct
{
    float           *data;
    e_sensor_type   type;
} s_PRO_sensor_data_dealed;


typedef struct
{
    e_IO_input_status   status;
    void                *handler;
    void                *handler_2;
    
    s_IO_input          state[INPUT_CHANNEL_MAX];
    uint8_t             IO_num;
    
    uint8_t             TS_flag;
    uint32_t            timestamp;

} s_PRO_IO_input_cb;


typedef struct
{
    e_sensor_status         status;
    void                    *handler;
    void                    *handler_2;
    
    uint8_t                 sensor_num;
    s_sensor_handler        sensor[SENSOR_CHANNEL_MAX];                
   
    s_PRO_sensor_data_rough     data_rough;
    s_PRO_sensor_data_dealed    data_dealed;
    
    pFun_sensor_init                init;
    pFun_sensor_send_data           send_data;
    pFun_sensor_send_data_success   send_data_success;
    pFun_sensor_send_data_fail      send_data_fail;
    
} s_PRO_sensor_cb;

extern rt_uint8_t prio_PRO_CTRL;
extern rt_thread_t thread_PRO_CTRL;

extern s_IO_input PRO_input_ctrl_in[];

extern void rt_thread_entry_PRO_CTRL(void* parameter);





#endif // __PRO_CTRL_H__

