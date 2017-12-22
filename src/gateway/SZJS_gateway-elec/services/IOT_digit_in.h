#ifndef __IOT_DIGIT_IN_H__
#define __IOT_DIGIT_IN_H__


#include "board.h"
#include "sys_def.h"
#include "UITD.h"


typedef int (*pFun_IO_input_read_port) (void *data);
typedef int (*pFun_IO_input_read_pin) (void *data, int channel);


typedef struct
{
    uint8_t             ID; // > 0
    uint8_t             pin;
    e_IO_input_state    state;
    e_IO_input_state    state_last;
    e_IO_input_trig     state_trig;
    uint32_t            period; // unit: S
    uint32_t            timer;  // unit: S
    uint16_t            counter;
    
} s_IO_input;

typedef struct
{
    e_IO_input_status   status;
    void                *handler;
    void                *handler_2;
    
    s_IO_input          state[INPUT_CHANNEL_MAX];
    uint8_t             IO_num;
    uint32_t            port;
    
    uint8_t             TS_flag;
    uint32_t            timestamp;
    
    pFun_IO_input_read_port     read_port;
    pFun_IO_input_read_pin      read_pin;

} s_IO_input_cb;



extern uint8_t g_INPUT_trig_cfg[INPUT_CHANNEL_MAX];
extern uint32_t g_INPUT_report_period[INPUT_CHANNEL_MAX];

extern struct rt_messagequeue *mq_IO_input;

extern struct rt_mutex  mt_IO_input;

extern int IO_input_pin_cfg(s_IO_input_cb *cb, s_IO_input *cfg);
extern int IO_input_pin_read(void *data, int channel);



extern volatile s_IO_input_cb *p_IO_input_cb;

extern int IO_input_init(s_IO_input_cb *cb, 
                 s_IO_input *cfg,
                 int cfg_num,
                 pFun_IO_input_read_port read_port,
                 pFun_IO_input_read_pin read_pin);


extern int IO_input_handler(s_IO_input_cb *cb);
extern int IO_input_server(s_IO_input_cb *cb);







#endif // __IOT_DIGIT_IN_H__
 