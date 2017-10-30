#ifndef __IOT_DIGIT_OUT_H__
#define __IOT_DIGIT_OUT_H__


#include "board.h"
#include "sys_def.h"

#include "UITD.h"


typedef int (*pFun_IO_output_write_port) (void *data, uint32_t value);
typedef int (*pFun_IO_output_write_pin) (void *data, int channel, uint8_t value);



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
    uint8_t             ID;
    uint8_t             pin;
    uint8_t             valid;
    uint8_t             waiting;
    e_output_action     action;
    e_output_action     action_last;
    uint16_t            counter;
    uint32_t            delay;
} s_output_ctrl;

typedef struct
{
    e_OUTPUT_status status;
    void *          handler;
    void *          handler_2;
    
    uint8_t         TS_flag;
    uint8_t         output_num;
    s_output_ctrl   output[OUTPUT_CHANNEL_MAX];
    
    uint32_t        port;
    
    pFun_IO_output_write_port   write_port;
    pFun_IO_output_write_pin    write_pin;
} s_output_ctrl_cb;



extern int output_write_port(void *data, uint32_t value);
extern int output_write_pin(void *data, int channel, uint8_t value);


extern struct rt_messagequeue *mq_output_ctrl;
extern struct rt_mutex mt_output;

extern s_output_ctrl_cb *p_output_ctrl_cb;



extern int output_ctrl_init(s_output_ctrl_cb *cb, 
                     s_output_ctrl *cfg, 
                     int cfg_num, 
                     pFun_IO_output_write_port Fun_write_port,
                     pFun_IO_output_write_pin Fun_write_pin
                     );
                     
extern int output_ctrl_handler(s_output_ctrl_cb *cb);
extern int output_ctrl_server_testing(s_output_ctrl_cb *cb);
extern int output_ctrl_server(s_output_ctrl_cb *cb);


#endif // __IOT_DIGIT_OUT_H__
