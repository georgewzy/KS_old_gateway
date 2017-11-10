#include "pro_UITD.h"

#include "ADC.h"
#include "MCP23X17.h"
#include "fire_alarm.h"

rt_uint8_t prio_PRO_UITD = 10;
rt_thread_t thread_PRO_UITD;

static s_IO_input_report IO_input_report_temp = {0};


s_sensor_handler PRO_UITD_sensor_handler_CH[] = 
{
    {1, sensor_out_type_4_20mA, 0, 0, 0},
    {2, sensor_out_type_4_20mA, 0, 0, 0},
    {3, sensor_out_type_4_20mA, 0, 0, 0},
    {4, sensor_out_type_4_20mA, 0, 0, 0},
};

s_IO_input PRO_UITD_input_ctrl_in[] = 
{
    { 1, PIN_INPUT_01, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
};






void rt_thread_entry_PRO_UITD(void* parameter)
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
                 
    mq_IO_input = rt_mq_create("mq_IO_input",
                 sizeof(s_IO_input_report),
                 64,
                 RT_IPC_FLAG_FIFO
                );
                 
    rt_mutex_init(&mt_sensor, "mt_sensor", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_IO_input, "mt_IO_input", RT_IPC_FLAG_FIFO);
                
    while(1)
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }
    
    json_cfg_load_PRO_UITD();
    
    for (i=0;i<sizeof(PRO_UITD_sensor_handler_CH)/sizeof(PRO_UITD_sensor_handler_CH[0]);i++)
    {
        PRO_UITD_sensor_handler_CH[i].period = g_sensor_period[i];
    }
    
    for (i=0;i<sizeof(PRO_UITD_input_ctrl_in)/sizeof(PRO_UITD_input_ctrl_in[0]);i++)
    {
        PRO_UITD_input_ctrl_in[i].state_trig = g_INPUT_trig_cfg[i];
        PRO_UITD_input_ctrl_in[i].period = g_INPUT_report_period[i];
    }

    p_sensor_cb = rt_malloc(sizeof(s_sensor_cb));
    if (p_sensor_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("sensor_cb malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }

    p_IO_input_cb = rt_malloc(sizeof(s_IO_input_cb));
    if (p_IO_input_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("PRO UITD IO input malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    
    sensor_sample_init(p_sensor_cb,
                       PRO_UITD_sensor_handler_CH,
                       sizeof(PRO_UITD_sensor_handler_CH)/sizeof(PRO_UITD_sensor_handler_CH[0]),
                       ADC_PRO_init);
    
    

    IO_input_init(p_IO_input_cb,
                 PRO_UITD_input_ctrl_in,
                 sizeof(PRO_UITD_input_ctrl_in)/sizeof(PRO_UITD_input_ctrl_in[0]),
                 NULL,
                 IO_input_pin_read);
    
    while(1)
    {
        if (sys_config.sys_mode == sys_mode_testing)
        {
            sensor_sample_handler_testing(p_sensor_cb);
            sensor_sample_server_testing(p_sensor_cb);
        }
        else if ((sys_config.sys_mode != sys_mode_testing) && (sys_config.sys_mode != sys_mode_tested))
        {
            sensor_sample_handler(p_sensor_cb);
            sensor_sample_server(p_sensor_cb);
            IO_input_server(p_IO_input_cb);
        }
        rt_thread_delay(1);
    }
    
}







