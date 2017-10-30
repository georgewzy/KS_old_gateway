#ifndef __SENSOR_SAMPLE_H__
#define __SENSOR_SAMPLE_H__

#include "board.h"
#include "sys_def.h"
#include "ADC.h"

#include "UITD.h"

#define SENSOR_HANDLER_MAX      16
#define SENSOR_CHANNAL_NUM      2

#define OUTPUT_CHANNAL_MAX      8
#define OUTPUT_CHANNAL_NUM      1

#define SENSOR_SAMPLE_PERIOD_DEFAULT        10  // unit : S


#define THREAD_SENSOR_SAMPLE_STACK_SIZE     1024



typedef int (*pFun_sensor_send_data) (void *cb, uint8_t ID, uint16_t  data, e_sensor_out_type type);
typedef int (*pFun_sensor_send_data_success) (void *data);
typedef int (*pFun_sensor_send_data_fail) (void *data);


typedef enum
{
    e_sensor_sta_idle = 0,
    e_sensor_sta_init,
    e_sensor_sta_inited,
    e_sensor_sta_sampling,
    e_sensor_sta_stop,
    e_sensor_sta_error
} e_sensor_status;


typedef enum
{
    e_sensor_type_res = 0,
    e_sensor_type_press,
    e_sensor_type_deep,
    e_sensor_type_therm,
} e_sensor_type;

typedef struct
{
    uint16_t        data[ADC_CHANNALS];
    e_sensor_type   type;
} s_sensor_data_rough;

typedef struct
{
    float           data[ADC_CHANNALS];
    e_sensor_type   type;
} s_sensor_data_dealed;



typedef struct
{
    uint8_t             ID;
    e_sensor_out_type   out_type;
    uint16_t            data_rough;
    uint32_t            timer;
    uint32_t            period;

} s_sensor_handler;


typedef enum
{
    e_output_sta_idle = 0,
    e_output_sta_init,
    e_output_inited,
    e_output_sta_working,
    e_output_sta_stop,
    e_output_sta_error
} e_OUTPUT_status;

typedef struct
{
    uint8_t     ID;
    uint8_t     pin;
    uint8_t     valid;
    uint8_t     action;
    uint32_t    delay;
} s_output_ctrl;

typedef struct
{
    e_OUTPUT_status status;
    void *          handler;
    
    uint8_t         TS_flag;
    //uint8_t         valid;
    uint8_t         output_num;
    s_output_ctrl   output[OUTPUT_CHANNAL_MAX];
    //uint32_t        timestamp;
} s_output_ctrl_cb;


typedef struct
{
    e_sensor_status         status;
    void                    *handler;
    
    uint8_t                 sensor_num;
    s_sensor_handler        sensor[SENSOR_HANDLER_MAX];                
    //s_sensor_handler        sensor_ch2;    

    //s_output_ctrl_cb        output;
    
    
    s_sensor_data_rough     data_rough;
    s_sensor_data_dealed    data_dealed;
    
    pFun_sensor_send_data           send_data;
    pFun_sensor_send_data_success   send_data_success;
    pFun_sensor_send_data_fail      send_data_fail;
    
} s_sensor_cb;



extern s_sensor_cb *p_sensor_cb;
extern s_output_ctrl_cb *p_output_ctrl_cb;

extern rt_uint8_t prio_sensor_sample;
extern rt_thread_t thread_sensor_sample;

extern struct rt_messagequeue mq_sensor_send;
extern struct rt_messagequeue mq_output_ctrl;
extern struct rt_mutex mt_sensor;
extern struct rt_mutex mt_output;

extern struct rt_mailbox mb_sensor_ADC;

extern void rt_thread_entry_sensor_sample(void* parameter);







#endif // __SENSOR_SAMPLE_H__
