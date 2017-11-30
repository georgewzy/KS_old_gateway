#include "controller.h"

#include "sensor_sample.h"
#include "fire_alarm.h"

rt_uint8_t prio_ctrl_in = 10;
rt_thread_t thread_ctrl_in;

rt_uint8_t prio_ctrl_out = 10;
rt_thread_t thread_ctrl_out;



s_output_ctrl output_ctrl_out[] = 
{
    {1, PIN_OUTPUT_01, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    {2, PIN_OUTPUT_02, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
    {3, PIN_OUTPUT_03, 0, 0, output_action_disconn, output_action_disconn, 0, 0},
};

s_IO_input input_ctrl_in[] = 
{
    { 1, PIN_INPUT_01, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0,  0, 0},
    { 2, PIN_INPUT_02, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0,  0, 0},
    { 3, PIN_INPUT_03, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0,  0, 0},
    { 4, PIN_INPUT_04, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0,  0, 0},
};



void rt_thread_entry_ctrl_in(void* parameter)
{
    int res = 0;
    

    rt_mutex_init(&mt_IO_input, "mt_IO_input", RT_IPC_FLAG_FIFO);

    mq_IO_input = rt_mq_create("mq_IO_input",
                 sizeof(s_IO_input_report),
                 64,
                 RT_IPC_FLAG_FIFO
                );
    
    while(1)
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }
    
    
    p_IO_input_cb = rt_malloc(sizeof(s_IO_input_cb));
    if (p_IO_input_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("IO input malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    IO_input_init(p_IO_input_cb,
                 input_ctrl_in,
                 sizeof(input_ctrl_in)/sizeof(input_ctrl_in[0]),
                 NULL,
                 IO_input_pin_read);
    
    while(1)
    {
        if (sys_config.sys_mode == sys_mode_testing)
        {
            
        }
        else if ((sys_config.sys_mode != sys_mode_testing) && (sys_config.sys_mode != sys_mode_tested))
        {
            IO_input_server(p_IO_input_cb);
        }
        rt_thread_delay(1);
    }
    
}

void rt_thread_entry_ctrl_out(void* parameter)
{
    int res = 0;
    

    rt_mutex_init(&mt_output, "mt_output", RT_IPC_FLAG_FIFO);
    
    mq_output_ctrl = rt_mq_create("mq_output_ctrl",
                 sizeof(s_output_data),
                 64,
                 RT_IPC_FLAG_FIFO
                );
    while(1)
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }
    

    p_output_ctrl_cb = rt_malloc(sizeof(s_output_ctrl_cb));
    if (p_output_ctrl_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Output ctrl malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    
    output_ctrl_init(p_output_ctrl_cb,
                     output_ctrl_out,
                     sizeof(output_ctrl_out)/sizeof(output_ctrl_out[0]),
                     output_write_port,
                     output_write_pin);
    
    while(1)
    {
        if (sys_config.sys_mode == sys_mode_testing)
        {
            
        }
        else if ((sys_config.sys_mode != sys_mode_testing) && (sys_config.sys_mode != sys_mode_tested))
        {
            output_ctrl_server(p_output_ctrl_cb);
        }
        rt_thread_delay(1);
    }
    
}


