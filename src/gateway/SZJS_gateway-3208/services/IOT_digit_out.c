#include "IOT_digit_out.h" 
 
 
 
uint32_t output_testing_counter = 0;

 
struct rt_messagequeue *mq_output_ctrl;
struct rt_mutex mt_output;

s_output_ctrl_cb *p_output_ctrl_cb = NULL;


static s_output_data output_data_temp = {0};

 
 
 
int output_ctrl_cfg(s_output_ctrl_cb *cb, s_output_ctrl *output)
{
    rt_memcpy(&cb->output[cb->output_num], output, sizeof(s_output_ctrl)); 
    cb->output_num ++;
    
    return 0;
}


int output_write_port(void *data, uint32_t value)
{
    s_output_ctrl_cb *cb = (s_output_ctrl_cb *)data;
    int i = 0;
    
    for (i=0;i<cb->output_num;i++)
    {
        if (value & (1<<i))
        {
            rt_pin_write(cb->output[i].pin, 1);
        }
        else
        {
            rt_pin_write(cb->output[i].pin, 0);
        }
    }
    
    return 0;
}

int output_write_pin(void *data, int channel, uint8_t value)
{
    s_output_ctrl_cb *cb = (s_output_ctrl_cb *)data;
    
    rt_pin_write(cb->output[channel].pin, value);
    
    return 0;
}


int output_ctrl_init(s_output_ctrl_cb *cb, 
                     s_output_ctrl *cfg, 
                     int cfg_num, 
                     pFun_IO_output_write_port Fun_write_port,
                     pFun_IO_output_write_pin Fun_write_pin
                     )
{
    int num = 0;
    int i = 0;
    
    rt_mutex_take(&mt_output, 100);
    rt_memset(cb, 0x00, sizeof(s_output_ctrl_cb));

    if (cfg_num > OUTPUT_CHANNEL_MAX)
    {
        SYS_log( SYS_DEBUG_ERROR, ("OUTPUT channel number is too much, %d\n", num));
        while(1) rt_thread_delay(10);
    }

    
    cb->status = e_output_sta_idle;
    cb->output_num = 0;
    cb->TS_flag = 0;
    cb->port = 0;
    cb->write_port = Fun_write_port;
    cb->write_pin = Fun_write_pin;
    
    for (i=0;i<cfg_num;i++)
    {
        output_ctrl_cfg(cb, &cfg[i]);
    }

    rt_mutex_release(&mt_output);
    return 0;
}

int output_ctrl_handler(s_output_ctrl_cb *cb)
{
    int i = 0;
    int res = 0;
    
    res = rt_mq_recv(mq_output_ctrl, &output_data_temp, sizeof(output_data_temp), 0);
    if (res == RT_EOK)
    {
        rt_mutex_take(&mt_output,10);
        
        if (output_data_temp.ID == 0)
        {
            for (i=0;i<cb->output_num;i++)
            {
                {
                    cb->output[i].action = output_data_temp.action;
                    cb->output[i].delay = output_data_temp.delay + 1;
                    cb->output[i].valid = 1;
                }
            }
        }
        else
        {
            for (i=0;i<cb->output_num;i++)
            {
                if (cb->output[i].ID == output_data_temp.ID)
                {
                    cb->output[i].action = output_data_temp.action;
                    cb->output[i].delay = output_data_temp.delay + 1;
                    cb->output[i].valid = 1;
                    break;
                }
            }
        }
        
        rt_mutex_release(&mt_output);
    }
    
    rt_mutex_take(&mt_output,10);
    for (i=0;i<cb->output_num;i++)
    {
        if (cb->output[i].valid)
        {
            switch(cb->output[i].action)
            {
                case output_action_disconn:
                    cb->write_pin(cb, i, 0);
                    cb->output[i].action_last = cb->output[i].action;
                    cb->output[i].valid = 0;
                    break;
                case output_action_connect:
                    cb->write_pin(cb, i, 1);
                    cb->output[i].action_last = cb->output[i].action;
                    cb->output[i].valid = 0;
                    break;
                case output_action_toggle:
                    if (cb->output[i].action_last == output_action_disconn)
                    {
                        cb->write_pin(cb, i, 1);
                        cb->output[i].action_last = output_action_connect;
                    }
                    else if (cb->output[i].action_last == output_action_connect)
                    {
                        cb->write_pin(cb, i, 0);
                        cb->output[i].action_last = output_action_disconn;
                    }
                    cb->output[i].valid = 0;
                    break;
                case output_action_disconn_delay:
                    if (cb->output[i].delay)
                    {
                        if (cb->TS_flag)
                        {
                            cb->TS_flag = 0;
                            
                            if (cb->output[i].counter >= 10) // 10 * 100ms = 1s
                            {
                                cb->output[i].counter = 0;
                                
                                cb->write_pin(cb, i, 0);
                                cb->output[i].delay--;
                                if (cb->output[i].delay == 0)
                                {
                                    if (cb->output[i].action_last == output_action_disconn)
                                    {
                                        cb->write_pin(cb, i, 0);
                                    }
                                    else if (cb->output[i].action_last == output_action_connect)
                                    {
                                        cb->write_pin(cb, i, 1);
                                    }
                                    else
                                    {
                                        cb->write_pin(cb, i, 1);
                                    }
                                    
                                    cb->output[i].valid = 0;
                                }
                            }
                            cb->output[i].counter ++;
                        }
                    }
                    break;
                case output_action_connect_delay:
                    if (cb->output[i].delay)
                    {
                        if (cb->TS_flag)
                        {
                            cb->TS_flag = 0;

                            if (cb->output[i].counter >= 10) // 10 * 100ms = 1s
                            {
                                cb->output[i].counter = 0;
                            
                                cb->write_pin(cb, i, 1);
                                cb->output[i].delay--;
                                if (cb->output[i].delay == 0)
                                {
                                    if (cb->output[i].action_last == output_action_disconn)
                                    {
                                        cb->write_pin(cb, i, 0);
                                    }
                                    else if (cb->output[i].action_last == output_action_connect)
                                    {
                                        cb->write_pin(cb, i, 1);
                                    }
                                    else
                                    {
                                        cb->write_pin(cb, i, 0);
                                    }
                                    
                                    cb->output[i].valid = 0;
                                }
                            }
                            cb->output[i].counter ++;
                        }
                    }
                    break;
                case output_action_just_delay:
                    if (cb->output[i].delay)
                    {
                        if (cb->TS_flag)
                        {
                            cb->TS_flag = 0;
                            
                            if (cb->output[i].counter >= 10) // 10 * 100ms = 1s
                            {
                                cb->output[i].counter = 0;
                                
                                cb->output[i].delay--;
                                if (cb->output[i].delay == 0)
                                {
                                    cb->output[i].valid = 0;
                                }
                            }
                            cb->output[i].counter ++;
                        }
                    }
                    break;
                default :
                    break;
            }
        }
    }
    rt_mutex_release(&mt_output);
    
    return 0;
}

int output_ctrl_server_testing(s_output_ctrl_cb *cb)
{
    int i = 0;
    
    switch (cb->status)
    {
        case e_output_sta_idle:
            cb->status = e_output_sta_init;
            break;
        case e_output_sta_init:
            
            cb->status = e_output_inited;
            break;
        case e_output_inited:
            
            cb->status = e_output_sta_working;
            break;
        case e_output_sta_working:
            output_testing_counter++;
            
            if (output_testing_counter == (RT_TICK_PER_SECOND/2))
            {
                cb->write_port(cb, 0xFF);
            }
            else if (output_testing_counter >= (RT_TICK_PER_SECOND))
            {
                cb->write_port(cb, 0xFF);
                output_testing_counter = 0;
            }
            
            break;
        case e_output_sta_stop:
            
            break;
        case e_output_sta_error:
            
            break;
        default:
            break;
    }
    return 0;
}


int output_ctrl_server(s_output_ctrl_cb *cb)
{
    int i = 0;
    
    switch (cb->status)
    {
        case e_output_sta_idle:

            break;
        case e_output_sta_init:
            
            cb->status = e_output_inited;
            break;
        case e_output_inited:
            
            cb->status = e_output_sta_working;
            break;
        case e_output_sta_working:
            
            output_ctrl_handler(cb);
            break;
        case e_output_sta_stop:
            
            break;
        case e_output_sta_error:
            
            break;
        default:
            break;
    }
    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
int controller_output(uint8_t ch, uint8_t state)
{
    s_output_data data = {0};
    
    if (p_output_ctrl_cb == NULL) return -1;
    
    data.ID = ch;
    if (state == 0)
    {
        data.action = output_action_disconn;
    }
    else if (state == 1)
    {
        data.action = output_action_connect;
    }
    
    rt_mq_send(mq_output_ctrl, &data, sizeof(s_output_data));
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(controller_output, OUTPUT, Control relay state  --ch --state.);

int controller_output_delay(uint8_t ch, uint8_t state, uint32_t delay)
{
    s_output_data data = {0};
    
    if (p_output_ctrl_cb == NULL) return -1;
    
    data.ID = ch;
    if (state == 0)
    {
        data.action = output_action_disconn_delay;
    }
    else if (state == 1)
    {
        data.action = output_action_connect_delay;
    }
    data.delay = delay;
    
    rt_mq_send(mq_output_ctrl, &data, sizeof(s_output_data));
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(controller_output_delay, OUTPUT_DELAY, Keep relay a state for a while  --ch --state --delay.);


int controller_toggle(uint8_t ch)
{
    s_output_data data = {0};
    
    if (p_output_ctrl_cb == NULL) return -1;
    
    data.ID = ch;
    data.action = output_action_toggle;
    
    rt_mq_send(mq_output_ctrl, &data, sizeof(s_output_data));
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(controller_toggle, TOGGLE, Toggle relay state  --ch .);

#endif // RT_USING_FINSH

 