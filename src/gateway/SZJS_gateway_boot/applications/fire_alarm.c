#include "fire_alarm.h"


rt_uint8_t prio_fire_alarm = 10;
rt_thread_t thread_fire_alarm;

rt_uint8_t prio_fire_alarm_rx = 10;
rt_thread_t thread_fire_alarm_rx;


struct rt_device * device_com_bus;

static uint8_t fire_alarm_inited = 0;

volatile s_com_bus_cb *p_com_bus_cb = NULL;
volatile s_IO_input_cb *p_IO_input_cb = NULL;

struct rt_semaphore sem_FA_rx;


struct rt_messagequeue mq_FA_fire;
s_com_bus_R_alarm mq_FA_fire_buf[10] = {0};

struct rt_messagequeue mq_FA_fault;
s_com_bus_R_alarm mq_FA_fault_buf[10] = {0};

struct rt_messagequeue mq_IO_input;
s_IO_input_report mq_IO_input_buf[10] = {0};

static s_IO_input_report IO_input_report_temp = {0};

struct rt_mutex mt_IO_input;

s_IO_input fire_alarm_IO_input = 
{
    1,
    PIN_IO_IN,
    IO_input_state_low,
    IO_input_state_low,
    IO_input_trig_period,
    8,
    0
};


int com_bus_config(s_com_bus_cfg *cfg);

// fire device information table
const s_FAC_config FAC_config_table[] = {
    
    {/////////////////////////////////////////////// TYPE_JB_QB_GST_100
        FAC_type_JB_QB_GST_100,
        "JB-QB-GST-100",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xAA}, 1}, // head
            {0, {0}, 0}, // tail
            1, 24,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x15}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },

    {/////////////////////////////////////////////// TYPE_JB_QB_GST_200
        FAC_type_JB_QB_GST_200,
        "JB-QB-GST-200",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xAA}, 1}, // head
            {0, {0}, 0}, // tail
            1, 24,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x15}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },

    {/////////////////////////////////////////////// TYPE_JB_QB_GST_500
        FAC_type_JB_QB_GST_500,
        "JB-QB-GST-500",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xAA}, 1}, // head
            {0, {0}, 0}, // tail
            1, 24,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x15}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },

    {/////////////////////////////////////////////// TYPE_JB_QB_GST_5000
        FAC_type_JB_QB_GST_5000,
        "JB-QB-GST-5000",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xAA}, 1}, // head
            {0, {0}, 0}, // tail
            1, 24,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x15}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },

    {/////////////////////////////////////////////// TYPE_JB_QB_GST_9000
        FAC_type_JB_QB_GST_9000,
        "JB-QB-GST-9000",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xAA}, 1}, // head
            {0, {0}, 0}, // tail
            1, 24,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x15}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },
    
    {/////////////////////////////////////////////// FAC_TYPE_JB_3101G
        FAC_type_JB_3101G,
        "JB-3101G",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFE}, 1}, // head
            {0, {0}, 0}, // tail
            1, 10,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x01}, // buf
                2, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },
    
    {/////////////////////////////////////////////// FAC_TYPE_JB_3101
        FAC_type_JB_3101,
        "JB-3101",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFE}, 1}, // head
            {0, {0}, 0}, // tail
            1, 10,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x01}, // buf
                2, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },

    {/////////////////////////////////////////////// FAC_TYPE_JB_3101B
        FAC_type_JB_3101B,
        "JB-3101B",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
        {0, "", 0, 0}, // pre_send_1
        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
        {0, "", 0, "", 0, 0}, // reset_2
        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFE}, 1}, // head
            {0, {0}, 0}, // tail
            1, 10,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x01}, // buf
                2, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x21}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {2,{0,1}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {1,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    3, // data_index
                    FAC_data_BCD, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {3,{1,2,3}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        }
    },


};






rt_err_t com_bus_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_FA_rx);    
}

rt_err_t com_bus_tx_complete(rt_device_t dev, void *buffer)
{


}



int IO_input_init(s_IO_input_cb *cb)
{
    int res = 0;
    int i = 0;
    
    rt_memset(cb, 0x00, sizeof(s_IO_input_cb));
    
    cb->status = IO_input_status_idle;
    cb->IO_num = 0;
    cb->TS_flag = 0;
    cb->timestamp = 0;
    cb->handler = NULL;
    
    IO_input_pin_cfg(cb, &fire_alarm_IO_input);
    
    
    
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
    
    if (cb->TS_flag)
    {
        cb->TS_flag = 0;
        
        rt_time(&cb->timestamp);
        
        rt_mutex_take(&mt_IO_input, 100);
        for (i=0;i<cb->IO_num;i++)
        {
            cb->state[i].state_last = cb->state[i].state;
            cb->state[i].state = rt_pin_read(cb->state[i].pin);
            
            IO_input_report_temp.ID = cb->state[i].ID;
            IO_input_report_temp.state = cb->state[i].state;
            
            if (cb->state[i].state_trig == IO_input_trig_period)
            {
                if (cb->timestamp >= cb->state[i].timer)
                {
                    // Song: TODO, report the IO state
                    rt_mq_send(&mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                    cb->state[i].timer = cb->timestamp + cb->state[i].period;
                }
            }
            else if (cb->state[i].state_trig == IO_input_trig_rise)
            {
                if ((cb->state[i].state_last == IO_input_state_low) && 
                    (cb->state[i].state == IO_input_state_high))
                {
                    // Song: TODO, report the rise state.
                    rt_mq_send(&mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                }
            }
            else if (cb->state[i].state_trig == IO_input_trig_fall)
            {
                if ((cb->state[i].state_last == IO_input_state_high) && 
                    (cb->state[i].state == IO_input_state_low))
                {
                    // Song: TODO, report the fall state.
                    rt_mq_send(&mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
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
                    rt_mq_send(&mq_IO_input, &IO_input_report_temp, sizeof(s_IO_input_report));
                }
            }
            else
            {
            
            
            }
        }
        rt_mutex_release(&mt_IO_input);
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



int com_buf_server(s_com_bus_cb * cb)
{
    switch (cb->status)
    {
        case com_bus_status_idle:
            
            break;
        case com_bus_status_init:
            com_bus_config(&cb->FAC_config.cfg);
            //com_bus_config(&fire_dev_info_table[0].cfg);
            cb->status = com_bus_status_inited;
            break;
        case com_bus_status_inited:
            cb->alive_timer = 0;
            cb->status = com_bus_status_alive;
            break;
        case com_bus_status_alive:
            if (cb->rx_server != NULL) cb->rx_server(cb);
            break;
        case com_bus_status_stop:
            cb->status = com_bus_status_idle;
            break;
        case com_bus_status_error:
            cb->status = com_bus_status_idle;
            break;
        default:
            break;
    }

    return 0;
}

int com_bus_config(s_com_bus_cfg *cfg)
{
    struct serial_configure serial_cfg = {0};
    
    serial_cfg.baud_rate = cfg->baudrate;
    serial_cfg.data_bits = cfg->bits;
    serial_cfg.parity = cfg->verify;
    serial_cfg.stop_bits = cfg->stopbit;
    
    serial_cfg.bit_order = BIT_ORDER_LSB;
    serial_cfg.bufsz = UITD_USART_COM_BUS_SIZE;
    serial_cfg.invert = NRZ_NORMAL;
    serial_cfg.reserved = 0;
    
    rt_device_control(device_com_bus, RT_DEVICE_CTRL_CONFIG, &serial_cfg);
    
    return 0;
}




int FAC_unusual_01_rx_parser(s_com_bus_cb * cb)
{
    int res = 0;
    uint8_t data_temp = 0;


    return 0;
}

int FAC_unusual_01_rx_handler(s_com_bus_cb * cb)
{

    
    return 0;
}

int FAC_unusual_01_server(s_com_bus_cb *cb)
{
    

    return 0;
}


int FAC_usual_rx_parser(s_com_bus_cb * cb)
{
    //uint8_t data = 0x00;
    uint8_t data_temp = 0x00;
    int res = 0;
    
    while(1)
    {
        res = rt_device_read(cb->dev, 0, &data_temp, 1);
        if (res < 1)
        {
            return -1;
        }
        cb->rec_buf[cb->rec_len] = data_temp;
        cb->rec_len ++;

		switch (cb->parse.status)
		{
			case FAC_parse_idle:
                
                if (cb->rec_len >= cb->FAC_config.alarm.head.head_len)
                {
                    if (rt_memcmp(&cb->rec_buf[cb->rec_len-cb->FAC_config.alarm.head.head_len], 
                                    cb->FAC_config.alarm.head.head_buf, 
                                    cb->FAC_config.alarm.head.head_len) == 0)
                    {
                        rt_memcpy(cb->parse.buf, cb->FAC_config.alarm.head.head_buf, cb->FAC_config.alarm.head.head_len);
                        cb->parse.len = cb->FAC_config.alarm.head.head_len;
                        cb->parse.status = FAC_parse_data;
                    }
                    else
                    {
                        if (cb->rec_len > COM_BUS_REC_MAX/2)
                        {
                            cb->rec_len = 0;
                        }
                    }
                }
//				if (data_temp == COM_PKT_HEAD_CHAR)
//				{
//					cb->parse.status = FAC_parse_head;
//					cb->parse.len = 0;
//					cb->parse.valid = 0;
//				}
//				else
//				{
//					
//				}
				break;
//			case FAC_parse_head:
//				if (data_temp == COM_PKT_HEAD_CHAR)
//				{
//					cb->parse.status = FAC_parse_data;
//				}
//				else
//				{
//					cb->parse.status = FAC_parse_idle;
//				}
//				
//				break;
			case FAC_parse_data:
                if (cb->FAC_config.alarm.tail.if_tail)
                {
                    cb->parse.buf[cb->parse.len] = data_temp;
                    cb->parse.len ++;
                    
                    if (cb->parse.len >= sizeof(cb->parse.buf))
                    {
                        cb->rec_len = 0;
                        cb->parse.len = 0;
                        cb->parse.status = FAC_parse_idle;
                        break;
                    }
                    
                    if (cb->parse.len >= (cb->FAC_config.alarm.head.head_len + cb->FAC_config.alarm.tail.tail_len))
                    {
                        if (rt_memcmp(&cb->parse.buf[cb->parse.len - cb->FAC_config.alarm.tail.tail_len], 
                                       cb->FAC_config.alarm.tail.tail_buf,
                                       cb->FAC_config.alarm.tail.tail_len) == 0)
                        {
                            
                            cb->rec_len = 0;
                            cb->parse.status = FAC_parse_idle;
                            cb->parse.valid = 1;
                            return 0;
                        }
                        
                    }
                    
                }
                else
                {
                    if (cb->FAC_config.alarm.if_alarm_fix_len)
                    {
                        cb->parse.buf[cb->parse.len] = data_temp;
                        cb->parse.len ++;
                        
                        if (cb->parse.len >= cb->FAC_config.alarm.alarm_fix_len)
                        {
                            cb->rec_len = 0;
                            cb->parse.status = FAC_parse_idle;
                            cb->parse.valid = 1;
                            return 0;
                        }
                    
                    }
                
                }
            
            
//				if (data_temp == COM_PKT_TAIL_CHAR)
//				{
//					cb->parse.status = FAC_parse_tail;
//					
//				}
//				else
//				{
//					cb->parse.buf[cb->parse.len] = data_temp;
//					cb->parse.len ++;
//					if (cb->parse.len > FAC_PACKET_LEN_MAX)
//					{
//						cb->parse.status = FAC_parse_idle;
//					}
//				}
				break;
			case FAC_parse_tail:
//				if (data_temp == COM_PKT_TAIL_CHAR)
//				{
//                    // Checking the packet lenght, if too short, discard.
//                    if (cb->parse.len < FAC_PACKET_LEN_MIN)
//                    {
//                        cb->parse.status = FAC_parse_idle;
//                        break;
//                    }
//                    
//                    // If data body lenght is not equal the actual packet, go on receiving the data.
//                    ctrl =  (t_COM_ctrl_unit *)cb->parse.buf;
//                    if (ctrl->data_len != (cb->parse.len - COM_PACKET_NO_DATA_LEN))
//                    {
//                        cb->parse.buf[cb->parse.len] = COM_PKT_TAIL_CHAR;
//                        cb->parse.len ++;
//                        cb->parse.buf[cb->parse.len] = COM_PKT_TAIL_CHAR;
//                        cb->parse.len ++;
//                        if (cb->parse.len > FAC_PACKET_LEN_MAX)
//                        {
//                            cb->parse.status = FAC_parse_idle;
//                            break;
//                        }
//                        cb->parse.status = FAC_parse_data;
//                        break;
//                    }
//                    
//                    // Packet is parsed correctly, exit.
//					cb->parse.valid = 1;
//					////rt_kprintf("Parse valid ");
//					cb->parse.status = FAC_parse_idle;
//					return 0;
//				}
//				else
//				{
//					cb->parse.buf[cb->parse.len] = COM_PKT_TAIL_CHAR;
//					cb->parse.len ++;
//					cb->parse.buf[cb->parse.len] = data_temp;
//					cb->parse.len ++;
//					if (cb->parse.len > FAC_PACKET_LEN_MAX)
//					{
//						cb->parse.status = FAC_parse_idle;
//                        break;
//					}
//					cb->parse.status = FAC_parse_data;
//				}
				
				break;
            case FAC_parse_end:
                break;
			default:
				
				break;
		}
        
    
    }
    
    
    
    return -1;
}


int FAC_alarm_data_element_parse(uint8_t *buf, s_FAC_data *data, uint8_t *result)
{

    uint32_t res_data = 0;
    uint8_t data_temp = 0;
    int i = 0;

    if (data->if_data)
    {
        res_data = 0;
        
        if (data->data_type == FAC_data_HEX)
        {
            for (i=0;i<data->data_HEX.num;i++)
            {
                res_data *= 0x0100;
                res_data += buf[data->data_index + data->data_HEX.index[i]];
            }
            *result = (uint8_t)res_data;
        }
        else if (data->data_type == FAC_data_BCD)
        {
            for (i=0;i<data->data_BCD.num;i++)
            {
                res_data *= 10;
                if (data->data_BCD.index[i]%2)
                {
                    data_temp = (buf[data->data_index + data->data_BCD.index[i]/2])%0x10;
                }
                else
                {
                    data_temp = (buf[data->data_index + data->data_BCD.index[i]/2])/0x10;
                }
                
                if (data_temp > 9)
                {
                    rt_kprintf("parse BCD data error : %d\n", data_temp);
                    return -1;
                }
                else
                {
                    res_data += data_temp;
                }
            }
            *result = (uint8_t)res_data;
        }
        else if (data->data_type == FAC_data_ASCII)
        {
            for (i=0;i<data->data_ASCII.num;i++)
            {
                res_data *= 10;
                data_temp = buf[data->data_index + data->data_ASCII.index[i]];
                if ((data_temp >= '0') && (data_temp <= '9'))
                {
                    res_data += (data_temp - '0');
                }
                else
                {
                    rt_kprintf("Parse ASCII data error: %d\n", res_data);
                    return -2;
                }
            }
            *result = (uint8_t)res_data;
        }
        else
        {
            rt_kprintf("Alarm_data_parse failed !\n");
            return -3;
        }
    }
    else
    {
        *result = 0;
    }
    
    return 0;
}


int FAC_alarm_data_parse(uint8_t *buf, s_FAC_alarm_struct *alarm, s_com_bus_R_alarm *alarm_data)
{
    int res = 0;
    
    alarm_data->valid = 0;
    alarm_data->sys_addr = 0;
    alarm_data->addr_main = 0;
    alarm_data->addr_sub = 0;
    
    res = FAC_alarm_data_element_parse(buf, &alarm->sys_addr, &alarm_data->sys_addr);
    if (res < 0) return -1;
    res = FAC_alarm_data_element_parse(buf, &alarm->addr_main, &alarm_data->addr_main);
    if (res < 0) return -1;
    res = FAC_alarm_data_element_parse(buf, &alarm->addr_sub, &alarm_data->addr_sub);
    if (res < 0) return -1;
    
    alarm_data->valid = 1;
    return 0;
}

int FAC_usual_rx_handler(s_com_bus_cb * cb)
{
    int res = 0;
    
    if (cb->parse.valid)
    {
        cb->parse.valid = 0;
        
        if ((cb->FAC_config.alarm.alarm.if_alarm) && 
             (rt_memcmp(&cb->parse.buf[cb->FAC_config.alarm.alarm.index], 
                        cb->FAC_config.alarm.alarm.buf, 
                        cb->FAC_config.alarm.alarm.len) == 0))
        {
            res = FAC_alarm_data_parse(cb->parse.buf, &cb->FAC_config.alarm.alarm, &cb->alarm_fire);
            if (res == 0)
            {
                rt_kprintf("Alarm_fire parse success: %d, %d, %d\n", 
                                cb->alarm_fire.sys_addr, 
                                cb->alarm_fire.addr_main, 
                                cb->alarm_fire.addr_sub);
                rt_mq_send(&mq_FA_fire, &cb->alarm_fire, sizeof(s_com_bus_R_alarm));
                cb->alarm_fire.valid = 0;

            }
        }
        else if ((cb->FAC_config.alarm.fault.if_alarm) && 
             (rt_memcmp(&cb->parse.buf[cb->FAC_config.alarm.fault.index], 
                        cb->FAC_config.alarm.fault.buf, 
                        cb->FAC_config.alarm.fault.len) == 0))
        {
            res = FAC_alarm_data_parse(cb->parse.buf, &cb->FAC_config.alarm.fault, &cb->alarm_fault);
            if (res == 0)
            {
                rt_kprintf("Alarm_fault parse success: %d, %d, %d\n", 
                                cb->alarm_fault.sys_addr, 
                                cb->alarm_fault.addr_main, 
                                cb->alarm_fault.addr_sub);
                rt_mq_send(&mq_FA_fault, &cb->alarm_fault, sizeof(s_com_bus_R_alarm));
                cb->alarm_fault.valid = 0;
            }
        }
        else
        {
        
        
        }
        
        
    }
    
//    if (0) // send alarm to UITD thread.
//    {
//        rt_mq_send(&mq_FA_fire, &cb->alarm_fire, sizeof(s_com_bus_R_alarm));
//        rt_mq_send(&mq_FA_fault, &cb->alarm_fault, sizeof(s_com_bus_R_alarm));
//    }
    
    return 0;
}

int FAC_usual_server(s_com_bus_cb *cb)
{
    if (cb->FAC_config.send_alive_1.if_send_alive)
    {
        if (cb->alive_timer >= cb->alive_period)
        {
            cb->alive_timer = 0;
            
            rt_device_write(cb->dev, 0, cb->FAC_config.send_alive_1.send_alive_buf, cb->FAC_config.send_alive_1.send_alive_len);
            if (cb->FAC_config.send_alive_1.send_alive_delay)
            {
                rt_thread_delay(cb->FAC_config.send_alive_1.send_alive_delay/(1000/RT_TICK_PER_SECOND) + 
                            cb->FAC_config.send_alive_1.send_alive_delay%(1000/RT_TICK_PER_SECOND));
            }
            
            if (cb->FAC_config.send_alive_2.if_send_alive)
            {
                rt_device_write(cb->dev, 0, cb->FAC_config.send_alive_2.send_alive_buf, cb->FAC_config.send_alive_2.send_alive_len);
                if (cb->FAC_config.send_alive_2.send_alive_delay)
                {
                    rt_thread_delay(cb->FAC_config.send_alive_2.send_alive_delay/(1000/RT_TICK_PER_SECOND) + 
                                cb->FAC_config.send_alive_2.send_alive_delay%(1000/RT_TICK_PER_SECOND));
                }
            }
        }
        
    }
    
    

    return 0;
}


int FAC_config_find(s_com_bus_cb * cb, e_FAC_type type)
{
    int i = 0;
    
    for (i=0;i<cb->table_num;i++)
    {
        if (FAC_config_table[i].type == type)
        {
            cb->table_index = i;
            return 0;
        }
    }

    return -1;
}

int FAC_config_load(s_com_bus_cb *cb)
{
    
    rt_memcpy(&cb->FAC_config, &FAC_config_table[cb->table_index], sizeof(s_FAC_config));
    
    return 0;
}


int com_bus_init(s_com_bus_cb * cb, rt_device_t dev, e_FAC_type type)
{
    uint32_t index = 0;
    int res = 0;
    rt_memset(cb, 0x00, sizeof(s_com_bus_cb));
    
    cb->status = com_bus_status_idle;
    cb->dev = dev;
    cb->table_index = 0;
    cb->table_num = sizeof(FAC_config_table)/sizeof(FAC_config_table[0]);
    cb->FAC_type = type;
    
    
    res = FAC_config_find(cb, type);
    if (res < 0)
    {
        rt_kprintf("Can not find fire alarm controller type !\n");
        while(1) rt_thread_delay(10);
    }
    
    
    

        
    if ((type < FAC_type_unusual_type) && (type > FAC_type_unknown))
    {
        FAC_config_load(cb);
        
        cb->alive_timer = 0;
        cb->alive_period = cb->FAC_config.alive_period;
        
        cb->rx_parser = &FAC_usual_rx_parser;
        cb->rx_handler = &FAC_usual_rx_handler;
        cb->rx_server = &FAC_usual_server;
    
    }
    else
    {
        if (type == FAC_type_unusual_01)
        {
            
            cb->rx_parser = &FAC_unusual_01_rx_parser;
            cb->rx_handler = &FAC_unusual_01_rx_handler;
            cb->rx_server = &FAC_unusual_01_server;
        } 
//        else if (type == FAC_type_unusual_02)
//        {
//            
//        }
        else
        {
            rt_kprintf("Unknown fire alarm controller type !\n");
            while (1){rt_thread_delay(10);}
        }
    }
    
    
    
    return 0;
}



void rt_thread_entry_fire_alarm(void* parameter)
{
    int res = 0;
    uint8_t data_temp = 0x00;
    int pin_state = 0;
    int pin_state_last = 0;

    rt_sem_init(&sem_FA_rx, "FA_rx", 0, RT_IPC_FLAG_FIFO);
    
    rt_mq_init( &mq_FA_fire,
                "mq_FA_fire",
                &mq_FA_fire_buf,
                sizeof(mq_FA_fire_buf)/sizeof(mq_FA_fire_buf[0]),
                sizeof(mq_FA_fire_buf),
                RT_IPC_FLAG_FIFO
                );

    rt_mq_init( &mq_FA_fault,
                "mq_FA_fault",
                &mq_FA_fault_buf,
                sizeof(mq_FA_fault_buf)/sizeof(mq_FA_fault_buf[0]),
                sizeof(mq_FA_fault_buf),
                RT_IPC_FLAG_FIFO
                );

    rt_mq_init( &mq_IO_input,
                "mq_IO_input",
                &mq_IO_input_buf,
                sizeof(mq_IO_input_buf)/sizeof(mq_IO_input_buf[0]),
                sizeof(mq_IO_input_buf),
                RT_IPC_FLAG_FIFO
                );                

    rt_mutex_init(&mt_IO_input, "mt_IO_input", RT_IPC_FLAG_FIFO);
                
    
    device_com_bus = rt_device_find(UITD_UART_COM_BUS);
    if(device_com_bus == RT_NULL)
    {
        rt_kprintf("Serial device %s not found!\r\n", UITD_UART_COM_BUS);
    }
    else
    {
		rt_device_open(device_com_bus, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(device_com_bus, com_bus_rx_ind);
        rt_device_set_tx_complete(device_com_bus, com_bus_tx_complete);
    }
    
    while(1)
    {
        if (sys_inited_flag) break;
        rt_thread_delay(5);
    }

    p_com_bus_cb = rt_malloc(sizeof(s_com_bus_cb));
    if (p_com_bus_cb == NULL)
    {
        rt_kprintf("Com bus malloc failed ! \n");
        while(1) rt_thread_delay(10);
    }
    
    com_bus_init(p_com_bus_cb, device_com_bus, FAC_type_JB_QB_GST_500);
    ////com_bus_init(p_com_bus_cb, device_com_bus, FAC_type_JB_3101G);

    p_IO_input_cb = rt_malloc(sizeof(s_IO_input_cb));
    if (p_IO_input_cb == NULL)
    {
        rt_kprintf("IO input malloc failed ! \n");
        while(1) rt_thread_delay(10);
    }
    
    IO_input_init(p_IO_input_cb);
    
    fire_alarm_inited = 1;
    
    while (1)
    {
        com_buf_server(p_com_bus_cb);
        IO_input_server(p_IO_input_cb);
        rt_thread_delay(5);
    }
    
    
//    while(1)
//    {
//        while(1)
//        {
//            res = rt_device_read(device_com_bus, 0, &data_temp, 1);
//            if (res == 1)
//            {
//                rt_kprintf("%c", data_temp);
//            }
//            else
//            {
//                break;
//            }
//        }
//        
//        res = rt_pin_read(PIN_IO_IN);
//        if (res != pin_state)
//        {
//            pin_state = res;
//            if (pin_state != pin_state_last)
//            {
//                pin_state_last = pin_state;
//                rt_kprintf("Fire alarm device input : %d\n", pin_state);
//            }
//        }
//        
//        rt_thread_delay(5);
//    }
    
}

void rt_thread_entry_fire_alarm_rx(void* parameter)
{
    int res = 0;

    while(fire_alarm_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        
        if (rt_sem_take(&sem_FA_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (p_com_bus_cb->rx_parser(p_com_bus_cb) == 0)
            {
                res = p_com_bus_cb->rx_handler(p_com_bus_cb);
                if (res)
                {
                    
                }
            }
        }
        else
        {
            
        }
    }
}




#ifdef RT_USING_FINSH
#include <finsh.h>

int fire_alarm_send(uint8_t *str)
{
    return rt_device_write(device_com_bus, 0, str, strlen(str));
}

FINSH_FUNCTION_EXPORT_ALIAS(fire_alarm_send, fire_send, Send data to fire alarm device.);


int fire_alarm_IO_out(uint8_t output)
{
    rt_pin_write(PIN_IO_OUT , output);
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(fire_alarm_IO_out, fire_out, Send data to fire alarm device.);
#endif // 
