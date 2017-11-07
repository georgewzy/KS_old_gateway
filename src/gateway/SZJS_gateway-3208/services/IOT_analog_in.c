#include "IOT_analog_in.h"

uint32_t sensor_testing_counter = 0;
uint16_t sensor_testing_value[SENSOR_TESTING_TIMES][ADC_CHANNALS] = {0};
uint32_t sensor_testing_value_sum[ADC_CHANNALS] = {0};


struct rt_mailbox *mb_sensor_ADC;

struct rt_messagequeue *mq_sensor_send;

struct rt_mutex mt_sensor;

s_sensor_cb *p_sensor_cb = NULL;



int sensor_ADC_DSP(s_sensor_cb *cb, uint16_t *data, uint32_t len, uint16_t *res)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t data_sum[SENSOR_CHANNEL_MAX] = {0};
    
    for (j=0;j<cb->sensor_num;j++)
    {
        for (i=0;i<len;i++)
        {
            data_sum[j] += data[i*cb->sensor_num + j];
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
    UITD_get_time(&rough_data.timestamp);
    
    return rt_mq_send(mq_sensor_send, &rough_data, sizeof(s_sensor_rough_data));
}

int sensor_sample_cfg(s_sensor_cb *cb, s_sensor_handler *sensor)
{
    rt_memcpy(&cb->sensor[cb->sensor_num], sensor, sizeof(s_sensor_handler)); 
    cb->sensor_num ++;

    return 0;
}


int sensor_sample_init(s_sensor_cb *cb,
                       s_sensor_handler *cfg,
                       int cfg_num,
                       pFun_sensor_init init)
{
    int i = 0;
    int num = 0;
   
    rt_mutex_take(&mt_sensor, 100);
    rt_memset(cb, 0x00, sizeof(s_sensor_cb));

    if (cfg_num > SENSOR_CHANNEL_MAX)
    {
        SYS_log( SYS_DEBUG_ERROR, ("SENSOR channel number is too much, %d\n", num));
        while(1) rt_thread_delay(10);
    }
    
    cb->status = e_sensor_sta_idle;
    cb->handler = NULL;
    cb->handler_2 = NULL;
    cb->sensor_num = 0;
    
    cb->init = init;
    cb->send_data = &sensor_sample_send_data_mq;


    cb->send_data_success = NULL;
    cb->send_data_fail = NULL;


    for (i=0;i<cfg_num;i++)
    {
        sensor_sample_cfg(cb, &cfg[i]);
    }
    
    cb->data_rough.data = rt_malloc(sizeof(uint16_t) * cb->sensor_num);
    if (cb->data_rough.data == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc sensor data_rough failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(cb->data_rough.data, 0x00, sizeof(uint16_t) * cb->sensor_num);
    }
    
    cb->data_dealed.data = rt_malloc(sizeof(float) * cb->sensor_num);
    if (cb->data_dealed.data == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc sensor data_dealed failed !\n"));
        while(1) rt_thread_delay(10);
    }
    else
    {
        rt_memset(cb->data_dealed.data, 0x00, sizeof(float) * cb->sensor_num);
    }    
    
    rt_mutex_release(&mt_sensor);
    
    return 0;
}



int sensor_sample_handler_testing(s_sensor_cb *cb)
{
    int i = 0;
    int j = 0;
    int res = 0;
    uint16_t *p = NULL;
    
    res = rt_mb_recv(mb_sensor_ADC, (uint32_t *)&p, 0);
    if (res == RT_EOK)
    {
        sensor_ADC_DSP(cb, p, cb->sensor_num, (uint16_t *)&sensor_testing_value[sensor_testing_counter][0]);
        sensor_testing_counter++;
        
        if (sensor_testing_counter >= SENSOR_TESTING_TIMES)
        {
            sensor_testing_counter = 0;
            
            // Song: stop ADC timer
            TIM_Cmd(TIM4,DISABLE);
            
            for (i=0;i<SENSOR_TESTING_TIMES;i++)
            {
                for (j=0;j<cb->sensor_num;j++)
                {
                    sensor_testing_value_sum[j] += sensor_testing_value[i][j];
                }
            }
            
            for (j=0;j<cb->sensor_num;j++)
            {
                sensor_testing_value_sum[j] /= SENSOR_TESTING_TIMES;
            }
            
            if ((sensor_testing_value_sum[0] > SENSOR_TESTING_VALUE_MIN) && (sensor_testing_value_sum[0] < SENSOR_TESTING_VALUE_MAX))
            {
                sys_config.test_flag.sensor_4_20mA = 1;
            }
            if ((sensor_testing_value_sum[1] > SENSOR_TESTING_VALUE_MIN) && (sensor_testing_value_sum[1] < SENSOR_TESTING_VALUE_MAX))
            {
                sys_config.test_flag.sensor_5V = 1;
            }
            cb->status = e_sensor_sta_stop;
        }
        
        
    }
    
}


int sensor_sample_handler(s_sensor_cb *cb)
{
    int i = 0;
    int res = 0;
    uint16_t *p = NULL;
    
    res = rt_mb_recv(mb_sensor_ADC, (uint32_t *)&p, 0);
    if (res == RT_EOK)
    {
        sensor_ADC_DSP(cb, p, cb->sensor_num, cb->data_rough.data);
        
        rt_mutex_take(&mt_sensor, 100);
        for (i=0;i<cb->sensor_num;i++)
        {

            if (cb->sensor[i].ID <= cb->sensor_num)
            {
                cb->sensor[i].data_rough = cb->data_rough.data[cb->sensor[i].ID-1];
            }
            
            if (cb->sensor[i].period)
            {
                cb->sensor[i].timer ++;
                if (cb->sensor[i].timer >= cb->sensor[i].period)
                {
                    cb->sensor[i].timer = 0;
                    cb->send_data(cb, cb->sensor[i].ID, cb->sensor[i].data_rough, cb->sensor[i].out_type);
                }
            }
            else
            {
                
            }
        }
        rt_mutex_release(&mt_sensor);
        
    }
    
}



int sensor_sample_server_testing(s_sensor_cb *cb)
{
    
    switch (cb->status)
    {
        case e_sensor_sta_idle:
            //#ifdef SENSOR_SAMPLE_TESTING
            cb->status = e_sensor_sta_init;
            //#endif // SENSOR_SAMPLE_TESTING
            break;
        case e_sensor_sta_init:
            cb->init(cb);
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
            ADC_Cmd(ADC1, DISABLE);
            ADC_DMACmd(ADC1, DISABLE);
            DMA_Cmd(DMA2_Stream0, DISABLE);
                
            TIM_Cmd(TIM4,DISABLE);
            
            break;
        case e_sensor_sta_error:
            
            break;
        default:
            break;
    }
    
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
            cb->init(cb);
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

}










