#ifndef __IOT_ANALOG_IN_H__
#define __IOT_ANALOG_IN_H__



#include "board.h"
#include "sys_def.h"
#include "ADC.h"

#include "UITD.h"


#define SENSOR_TESTING_TIMES        5
#define SENSOR_TESTING_VALUE        (4096*25/30)
#define SENSOR_TESTING_VALUE_MIN    (SENSOR_TESTING_VALUE - SENSOR_TESTING_VALUE*10/100)
#define SENSOR_TESTING_VALUE_MAX    (SENSOR_TESTING_VALUE + SENSOR_TESTING_VALUE*10/100)

typedef int (*pFun_sensor_init) (void *data);
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
    uint16_t        *data;
    e_sensor_type   type;
} s_sensor_data_rough;

typedef struct
{
    float           *data;
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

typedef struct
{
    e_sensor_status         status;
    void                    *handler;
    void                    *handler_2;
    
    uint8_t                 sensor_num;
    s_sensor_handler        sensor[SENSOR_CHANNEL_MAX];                
    
    s_sensor_data_rough     data_rough;
    s_sensor_data_dealed    data_dealed;
    
    pFun_sensor_init                init;
    pFun_sensor_send_data           send_data;
    pFun_sensor_send_data_success   send_data_success;
    pFun_sensor_send_data_fail      send_data_fail;
    
} s_sensor_cb;


extern uint32_t sensor_testing_counter;
extern uint16_t sensor_testing_value[SENSOR_TESTING_TIMES][ADC_CHANNALS];
extern uint32_t sensor_testing_value_sum[ADC_CHANNALS];
extern uint32_t output_testing_counter;

extern uint32_t mb_ADC_buf[SENSOR_CHANNEL_MAX];

extern s_sensor_cb *p_sensor_cb;

extern struct rt_messagequeue *mq_sensor_send;

extern int sensor_sample_send_data_mq(void *p_cb, uint8_t ID, uint16_t data, e_sensor_out_type type);

extern struct rt_mailbox *mb_sensor_ADC;


extern struct rt_mutex mt_sensor;





extern int sensor_sample_init(s_sensor_cb *cb,
                               s_sensor_handler *cfg,
                               int cfg_num,
                               pFun_sensor_init init);

extern int sensor_sample_handler_testing(s_sensor_cb *cb);
extern int sensor_sample_handler(s_sensor_cb *cb);
extern int sensor_sample_server_testing(s_sensor_cb *cb);          
extern int sensor_sample_server(s_sensor_cb *cb);                               


#endif // __IOT_ANALOG_IN_H__
 