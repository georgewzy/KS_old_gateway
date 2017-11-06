#include "sensor_sample.h"



rt_uint8_t prio_sensor_sample = 10;
rt_thread_t thread_sensor_sample;



s_output_ctrl IO_output_ctrl[] = 
{
    {1, PIN_IO_OUT, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
};




s_sensor_handler sensor_handler_CH[] = 
{
    {1, sensor_out_type_4_20mA, 0, 0, 0},
    {2, sensor_out_type_4_20mA, 0, 0, 0},
};








void rt_thread_entry_sensor_sample(void* parameter)
{
    int res = 0;
    int i = 0;

		//创建邮箱信号量
	
		//ADC采样邮箱信号量
    mb_sensor_ADC = rt_mb_create("mb_ADC",			
                 SENSOR_CHANNEL_MAX,
                 RT_IPC_FLAG_FIFO
                );                
                
    //数据发送邮箱信号量	
    mq_sensor_send = rt_mq_create("mq_sensor_send",
                 sizeof(s_sensor_rough_data),
                 256,
                 RT_IPC_FLAG_FIFO
                );
                
		//控制邮箱信号
    mq_output_ctrl = rt_mq_create("mq_output_ctrl",
                 sizeof(s_output_data),
                 64,
                 RT_IPC_FLAG_FIFO
                );

                
    //互斥锁
    rt_mutex_init(&mt_sensor, "mt_sensor", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_output, "mt_output", RT_IPC_FLAG_FIFO);
                
    while(1)   //等待系统初始化完成
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }
    
    json_cfg_load_AP01();			//读取AP01配置文件 中 配置参数
    
    for (i=0;i<sizeof(sensor_handler_CH)/sizeof(sensor_handler_CH[0]);i++)
    {
        sensor_handler_CH[i].period = g_sensor_period[i];
    }

    p_sensor_cb = rt_malloc(sizeof(s_sensor_cb));
    if (p_sensor_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Sensor sample malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }

    p_output_ctrl_cb = rt_malloc(sizeof(s_output_ctrl_cb));
    if (p_output_ctrl_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Output ctrl malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    sensor_sample_init(p_sensor_cb,
                       sensor_handler_CH,
                       sizeof(sensor_handler_CH)/sizeof(sensor_handler_CH[0]),
                       ADC_init);
    
    output_ctrl_init(p_output_ctrl_cb,
                     IO_output_ctrl,
                     sizeof(IO_output_ctrl)/sizeof(IO_output_ctrl[0]),
                     output_write_port,
                     output_write_pin);
    
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
        }
        rt_thread_delay(1);
    }
    
}

















