#include "pro_ctrl.h"



#include "ADC.h"
#include "MCP23X17.h"
#include "fire_alarm.h"

#define SENSOR_TESTING_TIMES        5
#define SENSOR_TESTING_VALUE        (4096*25/30)
#define SENSOR_TESTING_VALUE_MIN    (SENSOR_TESTING_VALUE - SENSOR_TESTING_VALUE*10/100)
#define SENSOR_TESTING_VALUE_MAX    (SENSOR_TESTING_VALUE + SENSOR_TESTING_VALUE*10/100)

rt_uint8_t prio_PRO_CTRL = 10;
rt_thread_t thread_PRO_CTRL;


//uint32_t mb_PRO_ADC_buf[4] = {0};


static s_output_data output_data_temp = {0};
static s_IO_input_report IO_input_report_temp = {0};


s_sensor_handler PRO_sensor_handler_CH[] = 
{
    {1, sensor_out_type_4_20mA, 0, 0, 0},
    {2, sensor_out_type_4_20mA, 0, 0, 0},
    {3, sensor_out_type_4_20mA, 0, 0, 0},
    {4, sensor_out_type_4_20mA, 0, 0, 0},
};

s_IO_input PRO_input_ctrl_in[] = 
{
    { 1, 0, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 2, 1, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 3, 2, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 4, 3, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 5, 4, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 6, 5, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 7, 6, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
    { 8, 7, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
};

s_output_ctrl PRO_output_ctrl[] = 
{
    { 1, 7, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    { 2, 6, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    { 3, 5, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    { 4, 4, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    { 5, 3, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    { 6, 2, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
//    { 7, 1, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
//    { 8, 0, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    
};



int PRO_IO_input_read_port(void *data)
{
    s_IO_input_cb *cb = (s_IO_input_cb *)data;
    
    cb->port = MCP_23X17_input();
}

int PRO_IO_input_read_pin(void *data, int channel)
{
    s_IO_input_cb *cb = (s_IO_input_cb *)data;
    
    return (cb->port & (1 << cb->state[channel].pin));
}

int PRO_output_write_port(void *data, uint32_t value)
{
    s_output_ctrl_cb *cb = (s_output_ctrl_cb *)data;
    int i = 0;
    
    for (i=0;i<cb->output_num;i++)
    {
        if (value & (1<<i))
        {
            MCP_23X17_output_bit(cb->output[i].pin, 1);
        }
        else
        {
            MCP_23X17_output_bit(cb->output[i].pin, 0);
        }
    }
    
    return 0;
}

int PRO_output_write_pin(void *data, int channel, uint8_t value)
{
    s_output_ctrl_cb *cb = (s_output_ctrl_cb *)data;
    
    return MCP_23X17_output_bit(cb->output[channel].pin, value);
}



int PRO_IO_input_init(s_IO_input_cb *cb)
{
    int res = 0;
    int i = 0;
    int num = 0;
    
    num = sizeof(PRO_input_ctrl_in)/sizeof(PRO_input_ctrl_in[0]);
    if (num > INPUT_CHANNEL_MAX)
    {
        SYS_log( SYS_DEBUG_ERROR, ("INPUT channel number is too much, %d\n", num));
        while(1) rt_thread_delay(10);
    }
    
    rt_memset(cb, 0x00, sizeof(s_IO_input_cb));
    
    cb->status = IO_input_status_idle;
    cb->IO_num = 0;
    cb->port = 0;
    cb->TS_flag = 0;
    cb->timestamp = 0;
    cb->handler = NULL;
    cb->handler_2 = NULL;
    cb->read_port = PRO_IO_input_read_port;
    cb->read_pin = PRO_IO_input_read_pin;
    
    for (i=0;i<num;i++)
    {
        IO_input_pin_cfg(cb, &PRO_input_ctrl_in[i]);
    }
    
    
    rt_time(&cb->timestamp);
    
    for (i=0;i<cb->IO_num;i++)
    {
        // Load the configuration.
        cb->state[i].state_trig = g_INPUT_trig_cfg[i];
        cb->state[i].period = g_INPUT_report_period[i];
        // Inital the timer for period report.
        if (cb->state[i].state_trig == IO_input_trig_period)
        {
            cb->state[i].timer = cb->timestamp + cb->state[i].period;
        }
    }

    return 0;
}


void rt_thread_entry_PRO_CTRL(void* parameter)
{
    int res = 0;
    int i = 0;
    
    mb_sensor_ADC = rt_mb_create("mb_ADC",
                 SENSOR_CHANNEL_MAX,
                 RT_IPC_FLAG_FIFO
                );                 

    mq_sensor_send = rt_mq_create("mq_sensor_send",
                 sizeof(s_sensor_rough_data),
                 256,
                 RT_IPC_FLAG_FIFO
                );
                 
    mq_output_ctrl = rt_mq_create("mq_output_ctrl",
                 sizeof(s_output_data),
                 256,
                 RT_IPC_FLAG_FIFO
                );   
                 
    mq_IO_input = rt_mq_create("mq_IO_input",
                 sizeof(s_IO_input_report),
                 64,
                 RT_IPC_FLAG_FIFO
                );
                 
    rt_mutex_init(&mt_sensor, "mt_sensor", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_output, "mt_output", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_IO_input, "mt_IO_input", RT_IPC_FLAG_FIFO);
                
    while(1)
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }
    
    json_cfg_load_PRO_CTRL();
    for (i=0;i<sizeof(PRO_sensor_handler_CH)/sizeof(PRO_sensor_handler_CH[0]);i++)
    {
        PRO_sensor_handler_CH[i].period = g_sensor_period[i];
    }
    
    for (i=0;i<sizeof(PRO_input_ctrl_in)/sizeof(PRO_input_ctrl_in[0]);i++)
    {
        PRO_input_ctrl_in[i].state_trig = g_INPUT_trig_cfg[i];
        PRO_input_ctrl_in[i].period = g_INPUT_report_period[i];
    }

    p_sensor_cb = rt_malloc(sizeof(s_sensor_cb));
    if (p_sensor_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("sensor_cb malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }

    p_output_ctrl_cb = rt_malloc(sizeof(s_output_ctrl_cb));
    if (p_output_ctrl_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("PRO Output ctrl malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    p_IO_input_cb = rt_malloc(sizeof(s_IO_input_cb));
    if (p_IO_input_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("PRO IO input malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    MCP23X17_hw_init("i2c1");
    
    sensor_sample_init(p_sensor_cb,
                       PRO_sensor_handler_CH,
                       sizeof(PRO_sensor_handler_CH)/sizeof(PRO_sensor_handler_CH[0]),
                       ADC_PRO_init);
    
    
    output_ctrl_init(p_output_ctrl_cb,
                     PRO_output_ctrl,
                     sizeof(PRO_output_ctrl)/sizeof(PRO_output_ctrl[0]),
                     PRO_output_write_port,
                     PRO_output_write_pin);

    IO_input_init(p_IO_input_cb,
                 PRO_input_ctrl_in,
                 sizeof(PRO_input_ctrl_in)/sizeof(PRO_input_ctrl_in[0]),
                 PRO_IO_input_read_port,
                 PRO_IO_input_read_pin);
    
    while(1)
    {
        if (sys_config.sys_mode == sys_mode_testing)
        {
            sensor_sample_handler_testing(p_sensor_cb);
            sensor_sample_server_testing(p_sensor_cb);
            output_ctrl_server_testing(p_output_ctrl_cb);
        }
        else if ((sys_config.sys_mode != sys_mode_testing) && (sys_config.sys_mode != sys_mode_tested))
        {
            sensor_sample_handler(p_sensor_cb);
            sensor_sample_server(p_sensor_cb);
            output_ctrl_server(p_output_ctrl_cb);
            IO_input_server(p_IO_input_cb);
        }
        rt_thread_delay(1);
    }
    
}



