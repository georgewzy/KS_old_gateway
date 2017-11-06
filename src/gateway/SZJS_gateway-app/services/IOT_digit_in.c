#include "IOT_digit_in.h"


struct rt_messagequeue *mq_IO_input;

static s_IO_input_report IO_input_report_temp = {0};

volatile s_IO_input_cb *p_IO_input_cb = NULL;


struct rt_mutex mt_IO_input;



int IO_input_pin_read(void *data, int channel)
{
    int res = 0;
    
    s_IO_input_cb *cb = (s_IO_input_cb *)data;
    
    res = rt_pin_read(cb->state[channel].pin);
    if (res == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int IO_input_init(s_IO_input_cb *cb, 
                 s_IO_input *cfg,
                 int cfg_num,
                 pFun_IO_input_read_port read_port,
                 pFun_IO_input_read_pin read_pin)
{
    int res = 0;
    int i = 0;
    int num = 0;
    
    if (cfg_num > INPUT_CHANNEL_MAX)
    {
        SYS_log( SYS_DEBUG_ERROR, ("INPUT channel number is too much, %d\n", num));
        return -1;
    }
    
    rt_memset(cb, 0x00, sizeof(s_IO_input_cb));
    
    cb->status = IO_input_status_idle;
    cb->IO_num = 0;
    cb->port = 0;
    cb->TS_flag = 0;
    cb->timestamp = 0;
    cb->handler = NULL;
    cb->handler_2 = NULL;
    cb->read_port = read_port;
    cb->read_pin = read_pin;

    for (i=0;i<cfg_num;i++)
    {
        IO_input_pin_cfg(cb, &cfg[i]);
    }
    
    
    rt_time(&cb->timestamp);
    
    for (i=0;i<cb->IO_num;i++)
    {
        if (cb->state[i].state_trig == IO_input_trig_period)
        {
            cb->state[i].timer = cb->timestamp + cb->state[i].period;
        }
    }

    return 0;
}



int IO_input_pin_cfg(s_IO_input_cb *cb, s_IO_input *cfg)
{
    
    rt_memcpy(&cb->state[cb->IO_num], cfg, sizeof(s_IO_input)); 
    cb->IO_num ++;
    
    return 0;
}

int IO_input_handler(s_IO_input_cb *cb)
{
    int i = 0;
    int res = 0;
    
    if (cb->TS_flag)
    {
        cb->TS_flag = 0;
        
        if (cb->state[i].counter >= 10) // 10 * 100ms = 1s
        {
            cb->state[i].counter = 0;
            
            rt_time(&cb->timestamp);
            
            rt_mutex_take(&mt_IO_input, 100);
            
            if (cb->read_port != NULL)
            {
                cb->read_port(cb);
            }
            
            for (i=0;i<cb->IO_num;i++)
            {
                cb->state[i].state_last = cb->state[i].state;
                
                res = cb->read_pin(cb, i);
                if (res == 0)
                {
                    cb->state[i].state = 0;
                }
                else
                {
                    cb->state[i].state = 1;
                }
                
                IO_input_report_temp.ID = cb->state[i].ID;
                IO_input_report_temp.state = cb->state[i].state;
                
                if (cb->state[i].state_trig == IO_input_trig_period)
                {
                    if (cb->timestamp >= cb->state[i].timer)
                    {
                        // Song: TODO, report the IO state
                        UITD_get_time(&IO_input_report_temp.timestamp);
                        rt_mq_send(mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                        cb->state[i].timer = cb->timestamp + cb->state[i].period;
                    }
                }
                else if (cb->state[i].state_trig == IO_input_trig_rise)
                {
                    if ((cb->state[i].state_last == IO_input_state_low) && 
                        (cb->state[i].state == IO_input_state_high))
                    {
                        // Song: TODO, report the rise state.
                        UITD_get_time(&IO_input_report_temp.timestamp);
                        rt_mq_send(mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                    }
                }
                else if (cb->state[i].state_trig == IO_input_trig_fall)
                {
                    if ((cb->state[i].state_last == IO_input_state_high) && 
                        (cb->state[i].state == IO_input_state_low))
                    {
                        // Song: TODO, report the fall state.
                        UITD_get_time(&IO_input_report_temp.timestamp);
                        rt_mq_send(mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                    }
                }
                else if (cb->state[i].state_trig == IO_input_trig_edge)
                {
                    if (((cb->state[i].state_last == IO_input_state_high) && 
                        (cb->state[i].state == IO_input_state_low)) ||
                        ((cb->state[i].state_last == IO_input_state_low) && 
                        (cb->state[i].state == IO_input_state_high)))
                    {
                        // Song: TODO, report the rise state.
                        UITD_get_time(&IO_input_report_temp.timestamp);
                        rt_mq_send(mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                    }
                }
                else
                {
                
                
                }
            }
            rt_mutex_release(&mt_IO_input);
        }
        cb->state[i].counter ++;
    }
    return 0;
}


int IO_input_server(s_IO_input_cb *cb)
{
    int i = 0;
    int res = 0;
    
    switch (cb->status)
    {
        case IO_input_status_idle:
            
            break;
        case IO_input_status_init:
            
            rt_time(&cb->timestamp);
            
            rt_mutex_take(&mt_IO_input, 100);
            for (i=0;i<cb->IO_num;i++)
            {
                if (cb->state[i].state_trig == IO_input_trig_period)
                {
                    cb->state[i].timer = cb->timestamp + cb->state[i].period;
                }
            }
            rt_mutex_release(&mt_IO_input);
            cb->status = IO_input_status_inited;
            break;
        case IO_input_status_inited:
            cb->status = IO_input_status_working;
            break;
        case IO_input_status_working:
            
            IO_input_handler(cb);
            break;
        case IO_input_status_stop:
            cb->status = IO_input_status_idle;
            break;
        case IO_input_status_error:
            cb->status = IO_input_status_idle;
            break;
        default:
            break;
    }

    return 0;
}

