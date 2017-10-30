#include "sensor_sample.h"

#include "ADC.h"

//#define SENSOR_SAMPLE_TESTING   

rt_uint8_t prio_sensor_sample = 10;
rt_thread_t thread_sensor_sample;


struct rt_mailbox mb_sensor_ADC;
uint32_t mb_ADC_buf[4] = {0};

struct rt_messagequeue mq_sensor_send;
s_sensor_rough_data mq_sensor_data_buf[10] = {0};

struct rt_messagequeue mq_output_ctrl;
s_output_data mq_output_ctrl_buf[10] = {0};

struct rt_mutex mt_sensor;
struct rt_mutex mt_output;

s_output_ctrl_cb *p_output_ctrl_cb = NULL;

s_output_ctrl output_ctrl_CH1 = 
{
    1,
    PIN_IO_OUT,
    0,
    0
};


static s_output_data output_data_temp = {0};

s_sensor_cb *p_sensor_cb = NULL;

s_sensor_handler sensor_handler_CH1 = 
{
    1,
    sensor_out_type_4_20mA,
    0,
    0,
    5
};

s_sensor_handler sensor_handler_CH2 = 
{
    2,
    sensor_out_type_5V,
    0,
    0,
    5
};


int sensor_ADC_DSP(uint16_t *data, uint32_t len, uint16_t *res)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t data_sum[ADC_CHANNALS] = {0};
    
    for (j=0;j<ADC_CHANNALS;j++)
    {
        for (i=0;i<len;i++)
        {
            data_sum[j] += data[i*ADC_CHANNALS + j];
        }
        *(res+j) = data_sum[j]/len;
    }
    
    return 0;
}


int sensor_sample_send_data_mq(void *p_cb, uint8_t ID, uint16_t data, e_sensor_out_type type)
{
    s_sensor_rough_data rough_data = {0};
    
    rough_data.ID = ID;
    rough_data.out_type = type;
    rough_data.data = data;
    
    return rt_mq_send(&mq_sensor_send, &rough_data, sizeof(s_sensor_rough_data));
}

int sensor_sample_cfg(s_sensor_cb *cb, s_sensor_handler *sensor)
{
    rt_memcpy(&cb->sensor[cb->sensor_num], sensor, sizeof(s_sensor_handler)); 
    cb->sensor_num ++;

    return 0;
}

int output_ctrl_cfg(s_output_ctrl_cb *cb, s_output_ctrl *output)
{
    rt_memcpy(&cb->output[cb->output_num], output, sizeof(s_output_ctrl)); 
    cb->output_num ++;
    
    return 0;
}

int output_ctrl_init(s_output_ctrl_cb *cb)
{
    rt_mutex_take(&mt_output, 100);
    rt_memset(cb, 0x00, sizeof(s_output_ctrl_cb));
    
    cb->status = e_output_sta_idle;
    cb->output_num = 0;
    cb->TS_flag = 0;
    //cb->timestamp = 0;
    
    output_ctrl_cfg(cb, &output_ctrl_CH1);
    
    rt_mutex_release(&mt_output);
    return 0;
}

int output_ctrl_handler(s_output_ctrl_cb *cb)
{
    int i = 0;
    int res = 0;
    
    res = rt_mq_recv(&mq_output_ctrl, &output_data_temp, sizeof(output_data_temp), 0);
    if (res == RT_EOK)
    {
        rt_mutex_take(&mt_output,10);
        for (i=0;i<cb->output_num;i++)
        {
            if (cb->output[i].ID == output_data_temp.ID)
            {
                cb->output[i].action = output_data_temp.action;
                cb->output[i].delay = output_data_temp.delay;
                cb->output[i].valid = 1;
            }
        }
        
        rt_mutex_release(&mt_output);
    }
    
    rt_mutex_take(&mt_output,10);
    for (i=0;i<cb->output_num;i++)
    {
        if (cb->output[i].valid)
        {
            if (cb->output[i].delay)
            {
                if (cb->TS_flag)
                {
                    cb->TS_flag = 0;
                    
                    rt_pin_write(cb->output[i].pin, cb->output[i].action);
                    cb->output[i].delay--;
                    if (cb->output[i].delay == 0)
                    {
                        if (cb->output[i].action == 0)
                        {
                            rt_pin_write(cb->output[i].pin, 1);
                        }
                        else 
                        {
                            rt_pin_write(cb->output[i].pin, 0);
                        }
                        
                        cb->output[i].valid = 0;
                    }
                }
            }
            else
            {
                rt_pin_write(cb->output[i].pin, cb->output[i].action);
                cb->output[i].valid = 0;
            }
        }
    }
    rt_mutex_release(&mt_output);
    
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

int sensor_sample_init(s_sensor_cb *cb)
{
    rt_mutex_take(&mt_sensor, 100);
    rt_memset(cb, 0x00, sizeof(s_sensor_cb));
    
    cb->status = e_sensor_sta_idle;
    cb->handler = NULL;
    cb->sensor_num = 0;
    
    //cb->send_data = &sensor_sample_send_data_wait;
    cb->send_data = &sensor_sample_send_data_mq;


    cb->send_data_success = NULL;
    cb->send_data_fail = NULL;
    
    sensor_sample_cfg(cb, &sensor_handler_CH1);
    sensor_sample_cfg(cb, &sensor_handler_CH2);
    
//    cb->sensor_ch1.out_type = sensor_out_type_4_20mA;
//    cb->sensor_ch1.data_rough = 0x0000;
//    cb->sensor_ch1.timer = 2;
//    cb->sensor_ch1.period = SENSOR_SAMPLE_PERIOD_DEFAULT;
//    cb->sensor_ch2.out_type = sensor_out_type_5V;
//    cb->sensor_ch2.data_rough = 0x0000;
//    cb->sensor_ch2.timer = 0;
//    cb->sensor_ch2.period = SENSOR_SAMPLE_PERIOD_DEFAULT;
    rt_mutex_release(&mt_sensor);
    
    return 0;
}


int sensor_sample_handler(s_sensor_cb *cb)
{
    int i = 0;
    int res = 0;
    uint16_t *p = NULL;
    
    res = rt_mb_recv(&mb_sensor_ADC, (uint32_t *)&p, 0);
    if (res == RT_EOK)
    {
        sensor_ADC_DSP(p, ADC_SAMPLES, (uint16_t *)&cb->data_rough.data);
        
        rt_mutex_take(&mt_sensor, 100);
        for (i=0;i<cb->sensor_num;i++)
        {
            if (cb->sensor[i].period)
            {
                cb->sensor[i].timer ++;
                if (cb->sensor[i].timer >= cb->sensor[i].period)
                {
                    cb->sensor[i].timer = 0;
                    if (cb->sensor[i].ID <= SENSOR_CHANNAL_NUM)
                    {
                        cb->sensor[i].data_rough = cb->data_rough.data[cb->sensor[i].ID-1];
                    }
                    cb->send_data(cb, cb->sensor[i].ID, cb->sensor[i].data_rough, cb->sensor[i].out_type);
                }
            }
            else
            {
                if (cb->sensor[i].ID <= SENSOR_CHANNAL_NUM)
                {
                    cb->sensor[i].data_rough = cb->data_rough.data[cb->sensor[i].ID-1];
                }
            }
        }
        rt_mutex_release(&mt_sensor);
        
    }
    
//    res = rt_mq_recv(&mq_output_ctrl, &output_data_temp, sizeof(output_data_temp), 0);
//    if (res == RT_EOK)
//    {
//        cb->output.data.ID = output_data_temp.ID;
//        cb->output.data.action = output_data_temp.action;
//        cb->output.data.delay = output_data_temp.delay;
//        
//        cb->output.valid = 1;
//    }
    
}

int sensor_sample_server(s_sensor_cb *cb)
{
    
    switch (cb->status)
    {
        case e_sensor_sta_idle:
            #ifdef SENSOR_SAMPLE_TESTING
            cb->status = e_sensor_sta_init;
            #endif // SENSOR_SAMPLE_TESTING
            break;
        case e_sensor_sta_init:
            //sensor_sample_init();
            ADC_init();
            tim4_init();
            cb->status = e_sensor_sta_inited;
            break;
        case e_sensor_sta_inited:
            
            DMA_Cmd(DMA2_Stream0, ENABLE);
            ADC_DMACmd(ADC1, ENABLE);
            ADC_Cmd(ADC1, ENABLE);
                
            TIM_Cmd(TIM4,DISABLE);
            TIM_SetCounter(TIM4,0);
            TIM_Cmd(TIM4,ENABLE);
            
            cb->status = e_sensor_sta_sampling;
            break;
        case e_sensor_sta_sampling:
            
            break;
        case e_sensor_sta_stop:
            
            break;
        case e_sensor_sta_error:
            
            break;
        default:
            break;
    }
    
//    if (cb->output.valid)
//    {
//        cb->output.valid = 0;
//        
//        // Song: TBD
//        if (cb->output.data.action == 1)
//        {
//            rt_pin_write(PIN_IO_OUT, PIN_HIGH);
//        }
//        else if (cb->output.data.action == 0)
//        {
//            rt_pin_write(PIN_IO_OUT, PIN_LOW);
//        }
//    }

}



void rt_thread_entry_sensor_sample(void* parameter)
{
    int res = 0;
    
    rt_mb_init( &mb_sensor_ADC,
                "mb_ADC",
                &mb_ADC_buf,
                sizeof(mb_ADC_buf)/4,
                RT_IPC_FLAG_FIFO
                );

    rt_mq_init( &mq_sensor_send,
                "mq_sensor_send",
                &mq_sensor_data_buf,
                sizeof(mq_sensor_data_buf)/sizeof(mq_sensor_data_buf[0]),
                sizeof(mq_sensor_data_buf),
                RT_IPC_FLAG_FIFO
                );

    rt_mq_init( &mq_output_ctrl,
                "mq_output_ctrl",
                &mq_output_ctrl_buf,
                sizeof(mq_output_ctrl_buf)/sizeof(mq_output_ctrl_buf[0]),
                sizeof(mq_output_ctrl_buf),
                RT_IPC_FLAG_FIFO
                );

    rt_mutex_init(&mt_sensor, "mt_sensor", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_output, "mt_output", RT_IPC_FLAG_FIFO);
                
    while(1)
    {
        if (sys_inited_flag) break;
        rt_thread_delay(5);
    }

    p_sensor_cb = rt_malloc(sizeof(s_sensor_cb));
    if (p_sensor_cb == NULL)
    {
        rt_kprintf("Sensor sample malloc failed ! \n");
        while(1) rt_thread_delay(10);
    }

    p_output_ctrl_cb = rt_malloc(sizeof(s_output_ctrl_cb));
    if (p_output_ctrl_cb == NULL)
    {
        rt_kprintf("Output ctrl malloc failed ! \n");
        while(1) rt_thread_delay(10);
    }
    
    sensor_sample_init(p_sensor_cb);
    output_ctrl_init(p_output_ctrl_cb);
    
    while(1)
    {
        sensor_sample_handler(p_sensor_cb);
        sensor_sample_server(p_sensor_cb);
        output_ctrl_server(p_output_ctrl_cb);
        rt_thread_delay(1);
    }
    
}

















