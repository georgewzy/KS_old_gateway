#include "fire_alarm.h"
#include "sys_config.h"

#include <dlfcn.h>

uint8_t com_bus_testing_string[] = "123456789\r";
uint8_t com_bus_testing_string_2[] = "012345678\r";
uint32_t com_bus_testing_cnt = 0;
uint8_t com_bus_testing_input_1 = 0;

uint16_t g_FA_type = FAC_TYPE_UNKNOWN;
uint32_t g_FA_baud = 0;
// If just listen the 232/485 bus, otherwise communicate with firealarm controler, send alive data.
uint32_t g_FA_listen = 0; 

uint8_t g_INPUT_trig_cfg[INPUT_CHANNEL_MAX] = {0};
uint32_t g_INPUT_report_period[INPUT_CHANNEL_MAX] = {0};


rt_uint8_t prio_fire_alarm = 10;
rt_thread_t thread_fire_alarm;

rt_uint8_t prio_fire_alarm_rx = 4;
rt_thread_t thread_fire_alarm_rx;

s_com_bus_cfg com_testing_cfg = 
{
    COM_TEST_BAUDRATE,
    8,
    0,
    1
};

struct rt_device * device_com_bus;

static uint8_t fire_alarm_inited = 0;

volatile s_com_bus_cb *p_com_bus_cb = NULL;

uint8_t FA_uart_num = 1;

s_com_bus_cb *p_FA_com_bus_cb[FA_COM_BUS_MAX] = {0};


s_FA_uart_cfg FA_uart_cfg[FA_COM_BUS_MAX] = {0};

s_FA_uart_cfg FA_uart_cfg_default = 
{
    "uart3",
    FAC_type_JB_QB_GST_5000,
    2400,
    1
};
        

struct rt_semaphore sem_FA_rx;

struct rt_event evt_FA_rx;

struct rt_messagequeue *mq_FA_manul_fire;
//uint8_t mq_FA_manul_fire_buf[128] = {0};
struct rt_messagequeue *mq_FA_2_manul_fire;
//uint8_t mq_FA_2_manul_fire_buf[128] = {0};

struct rt_messagequeue *mq_FA_fire;
//uint8_t mq_FA_fire_buf[128] = {0};
struct rt_messagequeue *mq_FA_2_fire;
//uint8_t mq_FA_2_fire_buf[128] = {0};

struct rt_messagequeue *mq_FA_power_off;
//uint8_t mq_FA_power_off_buf[64] = {0};
struct rt_messagequeue *mq_FA_2_power_off;
//uint8_t mq_FA_2_power_off_buf[64] = {0};

struct rt_messagequeue *mq_FA_fault;
//uint8_t mq_FA_fault_buf[128] = {0};
struct rt_messagequeue *mq_FA_2_fault;
//uint8_t mq_FA_2_fault_buf[128] = {0};


struct rt_messagequeue *mq_FA_reset;
struct rt_messagequeue *mq_FA_2_reset;




s_IO_input fire_alarm_IO_input[] = 
{
    { 1, PIN_IO_IN, IO_input_state_low, IO_input_state_low, IO_input_trig_none, 0, 0, 0},
};


int com_bus_config(rt_device_t dev, s_com_bus_cfg *cfg);

// fire device information table
const s_FAC_config FAC_config_table[] = {
    
    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_100
        FAC_type_JB_QB_GST_100,
        "JB-QB-GST-100",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            1,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_200
        FAC_type_JB_QB_GST_200,
        "JB-QB-GST-200",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            1,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_500
        FAC_type_JB_QB_GST_500,
        "JB-QB-GST-500",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            1,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_5000
        FAC_type_JB_QB_GST_5000,
        "JB-QB-GST-5000",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            1,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_9000
        FAC_type_JB_QB_GST_9000,
        "JB-QB-GST-9000",
        {2400, 8, PARITY_NONE, 1},
        3000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            1,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },
    
    {/////////////////////////////////////////////// FAC_type_JB_3101G
        FAC_type_JB_3101G,
        "JB-3101G",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },
    
    {/////////////////////////////////////////////// FAC_type_JB_3101
        FAC_type_JB_3101,
        "JB-3101",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_3101B
        FAC_type_JB_3101B,
        "JB-3101B",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
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
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },
    {/////////////////////////////////////////////// FAC_type_JB_QB_5Ei
        FAC_type_JB_QB_5Ei,
        "JB-QB-5Ei",
        {9600, 8, PARITY_NONE, 1},
        1000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {1, {0xF5,0xF5,0xF5,0xF5,0xF5}, 5, "", 0, 100}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x23}, 1}, // head
            {0, {0}, 0}, // tail
            1, 23,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x23}, // buf
                0, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    14, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    16, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    18, // data_index
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
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },
    
    {/////////////////////////////////////////////// FAC_type_JB_QX_5Li
        FAC_type_JB_QX_5Li,
        "JB-QX-5Li",
        {9600, 8, PARITY_NONE, 1},
        1000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {1, {0xF5,0xF5,0xF5,0xF5,0xF5}, 5, "", 0, 100}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x23}, 1}, // head
            {0, {0}, 0}, // tail
            1, 23,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x23}, // buf
                0, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    14, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    16, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    18, // data_index
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
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_1501_A
        FAC_type_JB_1501_A,
        "JB-1501(A)",
        {9600, 8, PARITY_NONE, 1},
        200, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFE,0x64,0xFF}, 3, {0xFE,0x64,0xFF}, 3, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFE}, 1}, // head
            {0, {0}, 0}, // tail
            1, 7,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x02}, // buf
                1, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    14, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
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
                1,  // if_alarm
                {0x01}, // buf
                1, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    2, // data_index
                    FAC_data_HEX, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
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

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },

    {/////////////////////////////////////////////// FAC_type_JB_QB_21S_VFC3010A
        FAC_type_JB_QB_21S_VFC3010A,
        "JB-QB-21S-VFC3010A",
        {2400, 8, PARITY_NONE, 1},
        1000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x09 ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFF,0xFF,0xFF}, 3}, // head
            {0, {0}, 0}, // tail
            1, 30,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x0A}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x0B}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },    

    {/////////////////////////////////////////////// FAC_type_JB_TG_JBF_11S
        FAC_type_JB_TG_JBF_11S,
        "JB-TG-JBF-11S",
        {2400, 8, PARITY_NONE, 1},
        1000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x09 ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFF,0xFF,0xFF}, 3}, // head
            {0, {0}, 0}, // tail
            1, 30,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x0A}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x0B}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },    

    {/////////////////////////////////////////////// FAC_type_JB_TG_JBF_11S_2013
        FAC_type_JB_TG_JBF_11S_2013,
        "JB-TG-JBF-11S(2013)",
        {2400, 8, PARITY_NONE, 1},
        1000, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x0A ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFF,0xFF,0xFF}, 3}, // head
            {0, {0}, 0}, // tail
            1, 30,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x80}, // buf
                3, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x0B}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    }, 

    {/////////////////////////////////////////////// FAC_type_BT8002
        FAC_type_BT8002,
        "BT8002",
        {9600, 8, PARITY_NONE, 1},
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x68}, 1}, // head
            {0, {0}, 0}, // tail
            1, 25,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x68}, // buf
                0, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    13, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    14, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x0B}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },     
    
    {/////////////////////////////////////////////// FAC_type_FC_720R
        FAC_type_FC_720R,
        "FC-720R",
        {9600, 8, PARITY_NONE, 1},
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {'*'}, 1}, // head
            {0, {'#'}, 1}, // tail
            1, 23,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x00}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    6, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {4,{3,2,1,0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x01}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    6, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {4,{3,2,1,0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },      
 
    {/////////////////////////////////////////////// FAC_type_CHNTAD_8000
        FAC_type_CHNTAD_8000,
        "CHNTAD8000",
        {2400, 8, PARITY_NONE, 1},
        0, // alive_period ?????????????????????????
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {1, {0xFF, 0xFF, 0xFF, 0x09, 0x00, 0x00, 0x01, 0x6E, 0x01, 0x0A, 0x00, 0x83}, 12, {0xFF, 0xFF, 0xFF, 0x13, 0x01, 0x01, 0x6E, 0x00, 0x01, 0x00, 0x00, 0x84}, 12, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0xFF,0xFF,0xFF}, 3}, // head
            {0, {'#'}, 1}, // tail
            1, 30,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x0B}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    10, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    11, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    12, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x01}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    6, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {4,{3,2,1,0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },      
   
    {/////////////////////////////////////////////// FAC_type_JB_Q100GZ2L_LA040
        FAC_type_JB_Q100GZ2L_LA040,
        "JB-Q100GZ2L-LA040",
        {22800, 8, PARITY_NONE, 1},
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {'@'}, 1}, // head
            {0, {'#'}, 1}, // tail
            1, 32,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {'1'}, // buf
                1, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    10, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    14, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{0,1}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    16, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {3,{0,1,2}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x01}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    6, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {4,{3,2,1,0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    }, 

    {/////////////////////////////////////////////// FAC_type_TX3000
        FAC_type_TX3000,
        "TX3000",
        {22800, 8, PARITY_NONE, 1},
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {'@'}, 1}, // head
            {0, {'#'}, 1}, // tail
            1, 32,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {'1'}, // buf
                1, // index
                1, // len
                { // sys_addr
                    0, // if_data
                    10, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    14, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{0,1}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    16, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {3,{0,1,2}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                0,  // if_alarm
                {0x01}, // buf
                1, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{1,0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    6, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {4,{3,2,1,0}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            0,  // if_reset
            {0x22}, // buf
            1, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    }, 
    {/////////////////////////////////////////////// FAC_type_SD2100_2200
        FAC_type_SD2100_2200,
        "SD2100_2200",
        {1200, 8, PARITY_NONE, 1}, // or 9600
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x82}, 1}, // head
            {1, {0x83}, 1}, // tail
            0, 8,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x50}, // buf
                6, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{0,1}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    7, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {1,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    8, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {3,{0,1,2}} // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x52}, // buf
                6, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    1, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {2,{0,1}} // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {1,{0}} // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_ASCII, // data_type
                    {0,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {3,{0,1,2}} // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            1,  // if_reset
            {0x6F}, // buf
            6, // index
            1, // len
            0, // if_sys_addr
            0, // sys_addr
            0, // sys_addr_index
        }        
    },     
    {/////////////////////////////////////////////// FAC_type_V6851
        FAC_type_V6851,
        "V6851",
        {9600, 8, PARITY_NONE, 1}, // or 9600
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x66, 0xF0}, 2}, // head
            {1, {0xFD}, 1}, // tail
            0, 8,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x01}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x03}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            1,  // if_reset
            {0x20}, // buf
            3, // index
            1, // len
            1, // if_sys_addr
            0, // sys_addr
            2, // sys_addr_index
        }        
    },      
    
};






rt_err_t com_bus_rx_ind(rt_device_t dev, rt_size_t size)
{
    int i = 0;
    //rt_sem_release(&sem_FA_rx); 
    
    for (i=0;i<FA_uart_num;i++)
    {
        if (strcmp(dev->parent.name, FA_uart_cfg[i].FA_name) == 0)
        {
            rt_event_send(&evt_FA_rx, (1<<i));    
        }
    }
}



rt_err_t com_bus_tx_complete(rt_device_t dev, void *buffer)
{


}




int com_bus_server_testing(s_com_bus_cb *cb)
{
    
    switch (cb->status)
    {
        case com_bus_status_idle:
            cb->status = com_bus_status_init;
            break;
        case com_bus_status_init:
            com_bus_config(cb->dev, &com_testing_cfg);
            com_bus_testing_cnt = 0;
            cb->status = com_bus_status_test_waiting;
            break;
        case com_bus_status_test_waiting: // Song: note, must waiting 5S !
            com_bus_testing_cnt ++;
            if (com_bus_testing_cnt > 5*RT_TICK_PER_SECOND)
            {
                com_bus_testing_cnt = 0;
                cb->status = com_bus_status_testing;
            }
            break;
        case com_bus_status_testing:
            com_bus_testing_cnt ++;
            if (com_bus_testing_cnt >= (RT_TICK_PER_SECOND/2))
            {
                rt_device_write(cb->dev, 0, com_bus_testing_string, sizeof(com_bus_testing_string)-1);
                
                com_bus_testing_cnt = 0;
            }
            
            if ((com_bus_testing_input_1 == 0) && (rt_pin_read(PIN_IO_IN) == 1))
            {
                com_bus_testing_input_1 = 1;
            }
            else if (com_bus_testing_input_1)
            {
                if (rt_pin_read(PIN_IO_IN) == 0)
                {
                    sys_config.test_flag.input = 1;
                }
            }
            
            if (sys_config.test_flag.RS232 && sys_config.test_flag.RS485 && sys_config.test_flag.input)
            {
                cb->status = com_bus_status_tested;
            }
            
            break;
        case com_bus_status_tested:
            while(1) rt_thread_delay(5);
        
            break;
        default:
            break;
    }
    
    return 0;
}

int com_bus_server(s_com_bus_cb * cb)
{
    switch (cb->status)
    {
        case com_bus_status_idle:
            
            break;
        case com_bus_status_init:
            com_bus_config(cb->dev, &cb->FAC_config.cfg);
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

int com_bus_config(rt_device_t dev, s_com_bus_cfg *cfg)
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
    
    rt_device_control(dev, RT_DEVICE_CTRL_CONFIG, &serial_cfg);
    
    return 0;
}


int com_bus_testing_parser(s_com_bus_cb * cb)
{

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

        if (cb->rec_len >= sizeof(cb->rec_buf))
        {
            SYS_log(SYS_DEBUG_ERROR, ("Combus rec_buf overflow ! \n"));
            
            cb->rec_len = 0;
        }

        if (data_temp == '\r')
        {
            if (rt_memcmp(cb->rec_buf, com_bus_testing_string, cb->rec_len) == 0)
            {
                sys_config.test_flag.RS232 = 1;
            }
            else if (rt_memcmp(cb->rec_buf, com_bus_testing_string_2, cb->rec_len) == 0)
            {
                sys_config.test_flag.RS485 = 1;
            }
            
            
            cb->rec_len = 0;
        }


    }
    return 0;
}

int FAC_usual_rx_parser(s_com_bus_cb * cb)
{
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

        if (cb->rec_len >= sizeof(cb->rec_buf))
        {
            SYS_log(SYS_DEBUG_ERROR, ("Combus rec_buf overflow ! \n"));
            
            cb->rec_len = 0;
        }
        
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

				break;
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
            
				break;
			case FAC_parse_tail:
				
				break;
            case FAC_parse_end:
                break;
			default:
				
				break;
		}
        
    
    }
    
    
    
    return -1;
}


int FAC_alarm_data_element_parse(uint8_t *buf, s_FAC_data *data, uint32_t *result)
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
            *result = res_data;
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
            *result = res_data;
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
            *result = res_data;
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

void fire_alarm_struct_init(s_fire_dev_status_info *dev_info)
{
    dev_info->reserved = -1;
    dev_info->port = -1;
    dev_info->controller = -1;
    dev_info->loop = -1;
    dev_info->device_ID = -1;
    dev_info->device_port = -1;
    rt_memset(dev_info->res, 0x00, sizeof(dev_info->res));
}
RTM_EXPORT(fire_alarm_struct_init);


int FAC_alarm_data_parse(uint8_t *buf, s_FAC_alarm_struct *alarm, s_com_bus_R_alarm *alarm_data)
{
    int res = 0;
    //s_fire_dev_status_info *status = (s_fire_dev_status_info *)alarm_data->remark;
    
    alarm_data->valid = 0;
    alarm_data->port = 0; 
    alarm_data->sys_addr = 0;
    alarm_data->addr_main = 0;
    alarm_data->addr_sub = 0;
    
    // set all the seg not used to initial the struct.
    fire_alarm_struct_init(&alarm_data->dev_info);
    
    res = FAC_alarm_data_element_parse(buf, &alarm->sys_addr, &alarm_data->sys_addr);
    if (res < 0) return -1;
    alarm_data->dev_info.controller = alarm_data->sys_addr;
    res = FAC_alarm_data_element_parse(buf, &alarm->addr_main, &alarm_data->addr_main);
    if (res < 0) return -1;
    alarm_data->dev_info.loop = alarm_data->addr_main;
    res = FAC_alarm_data_element_parse(buf, &alarm->addr_sub, &alarm_data->addr_sub);
    if (res < 0) return -1;
    alarm_data->dev_info.device_ID = alarm_data->addr_sub;
    
    alarm_data->valid = 1;
    return 0;
}
RTM_EXPORT(FAC_alarm_data_parse);

int FA_mq_fire(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_fire, buffer, size);
}
RTM_EXPORT(FA_mq_fire);

int FA_mq_fire_2(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_2_fire, buffer, size);
}
RTM_EXPORT(FA_mq_fire_2);

int FA_mq_fault(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_fault, buffer, size);
}
RTM_EXPORT(FA_mq_fault);

int FA_mq_fault_2(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_2_fault, buffer, size);
}
RTM_EXPORT(FA_mq_fault_2);

int FA_mq_reset(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_reset, buffer, size);
}
RTM_EXPORT(FA_mq_reset);

int FA_mq_reset_2(void *buffer, rt_size_t size)
{
    return rt_mq_send(mq_FA_2_reset, buffer, size);
}
RTM_EXPORT(FA_mq_reset_2);

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
                cb->alarm_fire.port = cb->port;
                SYS_log(SYS_DEBUG_INFO, ("Alarm_fire parse success: %d, %d, %d\n", 
                                cb->alarm_fire.sys_addr, 
                                cb->alarm_fire.addr_main, 
                                cb->alarm_fire.addr_sub));

                fire_alarm_struct_init(&cb->alarm_fire.dev_info);
                cb->alarm_fire.dev_info.port = cb->port;
                cb->alarm_fire.dev_info.controller = cb->alarm_fire.sys_addr;
                cb->alarm_fire.dev_info.loop = cb->alarm_fire.addr_main;
                cb->alarm_fire.dev_info.device_ID = cb->alarm_fire.addr_sub;


                rt_mq_send(mq_FA_fire, &cb->alarm_fire, sizeof(s_com_bus_R_alarm));
                rt_mq_send(mq_FA_2_fire, &cb->alarm_fire, sizeof(s_com_bus_R_alarm));
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
                cb->alarm_fault.port = cb->port;
                SYS_log(SYS_DEBUG_INFO, ("Alarm_fault parse success: %d, %d, %d\n", 
                                cb->alarm_fault.sys_addr, 
                                cb->alarm_fault.addr_main, 
                                cb->alarm_fault.addr_sub));
                
                fire_alarm_struct_init(&cb->alarm_fault.dev_info);
                cb->alarm_fault.dev_info.port = cb->port;
                cb->alarm_fault.dev_info.controller = cb->alarm_fault.sys_addr;
                cb->alarm_fault.dev_info.loop = cb->alarm_fault.addr_main;
                cb->alarm_fault.dev_info.device_ID = cb->alarm_fault.addr_sub;
                
                rt_mq_send(mq_FA_fault, &cb->alarm_fault, sizeof(s_com_bus_R_alarm));
                rt_mq_send(mq_FA_2_fault, &cb->alarm_fault, sizeof(s_com_bus_R_alarm));
                cb->alarm_fault.valid = 0;
            }
        }
        else if ((cb->FAC_config.reset.if_reset) && 
             (rt_memcmp(&cb->parse.buf[cb->FAC_config.reset.index], 
                        cb->FAC_config.reset.buf, 
                        cb->FAC_config.reset.len) == 0))
        {
            SYS_log(SYS_DEBUG_INFO, ("Alarm_reset !!!\n"));
            if (cb->FAC_config.reset.if_sys_addr)
            {
                cb->alarm_reset.port = cb->port;
                cb->alarm_reset.sys_addr = cb->parse.buf[cb->FAC_config.reset.sys_addr_index];
            }
            else
            {
                cb->alarm_reset.port = cb->port;
                cb->alarm_reset.sys_addr = 0;
            }
            
            rt_mq_send(mq_FA_reset, &cb->alarm_reset, sizeof(s_com_bus_R_reset));
            rt_mq_send(mq_FA_2_reset, &cb->alarm_reset, sizeof(s_com_bus_R_reset));
            cb->alarm_reset.valid = 0;
        }
        else
        {
        
        
        }
    }
    
    
    return 0;
}

int FAC_usual_server(s_com_bus_cb *cb)
{
    if ( (cb->FA_listen == 0) && cb->FAC_config.send_alive_1.if_send_alive)
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


int com_bus_init(s_com_bus_cb * cb, rt_device_t dev, s_FA_uart_cfg *cfg, uint8_t port)
{
    uint32_t index = 0;
    int res = 0;
    uint8_t file[64] = {0};
    
    void *FAC_lib_handler = NULL;
    
    rt_memset(cb, 0x00, sizeof(s_com_bus_cb));
    
    cb->status = com_bus_status_idle;
    cb->dev = dev;
    cb->table_index = 0;
    cb->table_num = sizeof(FAC_config_table)/sizeof(FAC_config_table[0]);
    cb->FAC_type = cfg->FA_type;
    
    //cb->port = FAC_PORT_DEFAULT;
    cb->port = port;
    
//    res = FAC_config_find(cb, type);
//    if (res < 0)
//    {
//        rt_kprintf("Can not find fire alarm controller type !\n");
//        while(1) rt_thread_delay(10);
//    }
    
        
    if ((cb->FAC_type < FAC_type_unusual_type) && (cb->FAC_type > FAC_type_unknown))
    {
        res = FAC_config_find(cb, cb->FAC_type);
        if (res < 0)
        {
            SYS_log(SYS_DEBUG_ERROR, ("Can not find fire alarm controller type !\n"));
            while(1) rt_thread_delay(10);
        }
        
        FAC_config_load(cb);
        
        if (cfg->FA_baud)
        {
            cb->FAC_config.cfg.baudrate = cfg->FA_baud;
            SYS_log(SYS_DEBUG_INFO, ("Reconfigure the baudrate : %d\n ", cfg->FA_baud));
        }
        cb->FA_listen = cfg->FA_listen;
        
        cb->alive_timer = 0;
        cb->alive_period = cb->FAC_config.alive_period;
        
        cb->rx_parser = &FAC_usual_rx_parser;
        cb->rx_handler = &FAC_usual_rx_handler;
        cb->rx_server = &FAC_usual_server;
    
    }
    else if ((cb->FAC_type > FAC_type_unusual_type) && (cb->FAC_type < FAC_type_usual_cfg))
    {
        // Song: just for test, default use FAC_16
        #if 0
        rt_memcpy(&cb->FAC_config, &FAC_config_table[12], sizeof(s_FAC_config));
        cb->FA_listen = g_FA_listen;
        #endif 

        
        // FAC module file must be this type: FAC_nnnn.so
        sprintf(file, "%s/FAC_%d.so", MODULE_LIB_DIR, cb->FAC_type);
        SYS_log(SYS_DEBUG_INFO, ("Find module %s\n", file));
        
        #if 0  // Just for shanghai NTN project upgrade .
        
        rm("/module/lib/FAC_10007.so");
        copy("/config/FAC_10007.so","/module/lib/FAC_10007.so");
        rt_thread_delay(300); //
        
        #endif 
        
        //rt_module_open(file);
        
        FAC_lib_handler = dlopen(file, RTLD_NOW);
        if(FAC_lib_handler != RT_NULL)
        {
            cb->rx_init  = (pFun_FAC_rx_init)dlsym(FAC_lib_handler, "FAC_unusual_rx_init");
            cb->rx_parser  = (pFun_FAC_rx_parser)dlsym(FAC_lib_handler, "FAC_unusual_rx_parser");
            cb->rx_handler = (pFun_FAC_rx_handler)dlsym(FAC_lib_handler, "FAC_unusual_rx_handler");
            cb->rx_server  = (pFun_FAC_server)dlsym(FAC_lib_handler, "FAC_unusual_server");

            if (cb->rx_init == NULL)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Load FAC function %s failed !\n", "FAC_unusual_rx_init"));
                while(1) rt_thread_delay(10);
            }
            if (cb->rx_parser == NULL)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Load FAC function %s failed !\n", "FAC_unusual_rx_parser"));
                while(1) rt_thread_delay(10);
            }
            if (cb->rx_handler == NULL)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Load FAC function %s failed !\n", "FAC_unusual_rx_handler"));
                while(1) rt_thread_delay(10);
            }
            if (cb->rx_server == NULL)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Load FAC function %s failed !\n", "FAC_unusual_server"));
                while(1) rt_thread_delay(10);
            }
            SYS_log(SYS_DEBUG_INFO, ("Load module %s success !\n", file));
            
            // Initialize the FAC configuration
            cb->rx_init(cb);
            
            cb->alive_timer = 0;
            cb->alive_period = cb->FAC_config.alive_period;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Load module %s failed !  STOP thread !!!\n", file));
            while(1) rt_thread_delay(10);
        }
        
//        if (type == FAC_type_unusual_01)
//        {
//            
//            cb->rx_parser = &FAC_unusual_01_rx_parser;
//            cb->rx_handler = &FAC_unusual_01_rx_handler;
//            cb->rx_server = &FAC_unusual_01_server;
//        } 
//        else
//        {
//            rt_kprintf("Unknown fire alarm controller type !\n");
//            while (1){rt_thread_delay(10);}
//        }
    }
    else if (cb->FAC_type > FAC_type_usual_cfg)
    {
//        res = FAC_config_find(cb, type);
//        if (res < 0)
//        {
//            SYS_log(SYS_DEBUG_ERROR, ("Can not find fire alarm controller type !\n"));
//            while(1) rt_thread_delay(10);
//        }
//        
//        FAC_config_load(cb);
        // FAC module file must be this type: FAC_nnnn.so
        sprintf(file, "%s/FAC_%d.so", MODULE_LIB_DIR, cb->FAC_type);
        SYS_log(SYS_DEBUG_INFO, ("Find module %s\n", file));

        FAC_lib_handler = dlopen(file, RTLD_NOW);
        if(FAC_lib_handler != RT_NULL)
        {
            cb->rx_init  = (pFun_FAC_rx_init)dlsym(FAC_lib_handler, "FAC_usual_cfg_init");
            if (cb->rx_init == NULL)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Load FAC function %s failed !\n", "FAC_usual_cfg_init"));
                while(1) rt_thread_delay(10);
            }
            
            // Initialize the FAC configuration
            cb->rx_init(cb);
            
        }        
        
        
        if (cfg->FA_baud)
        {
            cb->FAC_config.cfg.baudrate = cfg->FA_baud;
            SYS_log(SYS_DEBUG_INFO, ("Reconfigure the baudrate : %d\n ", cfg->FA_baud));
        }
        cb->FA_listen = cfg->FA_listen;
        
        cb->alive_timer = 0;
        cb->alive_period = cb->FAC_config.alive_period;
        
        cb->rx_parser = &FAC_usual_rx_parser;
        cb->rx_handler = &FAC_usual_rx_handler;
        cb->rx_server = &FAC_usual_server;
    }
    
    return 0;
}



void rt_thread_entry_fire_alarm(void* parameter)
{
    int res = 0;
    uint8_t data_temp = 0x00;
    int pin_state = 0;
    int pin_state_last = 0;
    int counter = 0;
    int pwr_off_flag = 0;
    int i = 0;
    int FA_num = 0;

    
    rt_sem_init(&sem_FA_rx, "FA_rx", 0, RT_IPC_FLAG_FIFO);			//初始化静态信号量
    
    rt_event_init(&evt_FA_rx, "FA_event", RT_IPC_FLAG_FIFO);		//初始化事件
  
																																//创建消息队列
    mq_FA_power_off = rt_mq_create("mq_FA_power_off",						//电源关闭报警
                 sizeof(s_com_bus_R_power_off),
                 64,
                 RT_IPC_FLAG_FIFO
                );
    
    mq_FA_2_power_off = rt_mq_create("mq_FA_2_power_off",				
                 sizeof(s_com_bus_R_power_off),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_fire = rt_mq_create("mq_FA_fire",											//火警
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_2_fire = rt_mq_create("mq_FA_2_fire",									
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_manul_fire = rt_mq_create("mq_FA_manul_fire",					//手动报警
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );
                 
    mq_FA_2_manul_fire = rt_mq_create("mq_FA_2_manul_fire",
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );
                
    mq_FA_fault = rt_mq_create("mq_FA_fault",										//故障报警
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_2_fault = rt_mq_create("mq_FA_2_fault",
                 sizeof(s_com_bus_R_alarm),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_reset = rt_mq_create("mq_FA_reset",										//复位
                 sizeof(s_com_bus_R_reset),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    mq_FA_2_reset = rt_mq_create("mq_FA_2_reset",
                 sizeof(s_com_bus_R_reset),
                 64,
                 RT_IPC_FLAG_FIFO
                );
                 
    mq_IO_input = rt_mq_create("mq_IO_input",										//IO输入
                 sizeof(s_IO_input_report),
                 64,
                 RT_IPC_FLAG_FIFO
                );

    rt_mutex_init(&mt_IO_input, "mt_IO_input", RT_IPC_FLAG_FIFO);
                
//    if (sys_config.dev_type == dev_type_IOT_PRO_UITD)
//    {
//        json_cfg_load_FA(&FA_uart_cfg, &FA_uart_num);
//    }
//    else
//    {
//        json_cfg_load_AP02(&FA_uart_cfg);  
//    }
    json_cfg_load_FA(&FA_uart_cfg, &FA_uart_num);

        

    for (i=0;i<sizeof(fire_alarm_IO_input)/sizeof(fire_alarm_IO_input[0]);i++)
    {
        fire_alarm_IO_input[i].state_trig = g_INPUT_trig_cfg[i];
        fire_alarm_IO_input[i].period = g_INPUT_report_period[i];
    }
    
    
    for (i=0;i<FA_uart_num;i++)
    {
        device_com_bus = rt_device_find(FA_uart_cfg[i].FA_name);
        if(device_com_bus == RT_NULL)
        {
            SYS_log(SYS_DEBUG_ERROR, ("Serial device %s not found!\r\n", FA_uart_cfg[i].FA_name));
            continue;
        }
        else
        {
            rt_device_open(device_com_bus, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
            rt_device_set_rx_indicate(device_com_bus, com_bus_rx_ind);
            //rt_device_set_tx_complete(device_com_bus, com_bus_tx_complete);
            FA_num ++;
        }

        
        p_com_bus_cb = rt_malloc(sizeof(s_com_bus_cb));
        if (p_com_bus_cb == NULL)
        {
            SYS_log(SYS_DEBUG_ERROR, ("Com bus malloc failed ! \n"));
            while(1) rt_thread_delay(10);
        }
        else 
        {
            p_FA_com_bus_cb[FA_num-1] = p_com_bus_cb;
        }
        
        if (FA_uart_cfg[FA_num-1].FA_type == FAC_TYPE_UNKNOWN)
        {
            com_bus_init(p_com_bus_cb, device_com_bus, &FA_uart_cfg_default, FA_num-1);
        }
        else
        {
            com_bus_init(p_com_bus_cb, device_com_bus, &FA_uart_cfg[FA_num-1], FA_num-1);
        }    
    }
    FA_uart_num = FA_num;

    ////com_bus_init(p_com_bus_cb, device_com_bus, FAC_type_JB_3101G);
    

//    p_IO_input_cb = rt_malloc(sizeof(s_IO_input_cb));
//    if (p_IO_input_cb == NULL)
//    {
//        rt_kprintf("IO input malloc failed ! \n");
//        while(1) rt_thread_delay(10);
//    }
    
//    IO_input_init(p_IO_input_cb,
//                 fire_alarm_IO_input,
//                 sizeof(fire_alarm_IO_input)/sizeof(fire_alarm_IO_input[0]),
//                 NULL,
//                 IO_input_pin_read);
    
    
    fire_alarm_inited = 1;
    
    while (1)
    {
        for (i=0;i<FA_uart_num;i++)
        {
            p_com_bus_cb = p_FA_com_bus_cb[i];
            
            if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
            {
                com_bus_server_testing(p_com_bus_cb);
            }
            else
            {
                com_bus_server(p_com_bus_cb);
            }
        }
        
        if ((pwr_off_flag == 0) && (rt_pin_read(PIN_IO_IN) == 0))
        {
            counter++;
            if (counter > 20)
            {
                pwr_off_flag = 1;
                        
                p_com_bus_cb->alarm_power_off.valid = 1;
                rt_mq_send(mq_FA_power_off, &p_com_bus_cb->alarm_power_off, sizeof(s_com_bus_R_power_off));
                rt_mq_send(mq_FA_2_power_off, &p_com_bus_cb->alarm_power_off, sizeof(s_com_bus_R_power_off));
            }
        }
        
        
//        IO_input_server(p_IO_input_cb);
        rt_thread_delay(1);
    }
    
    
}

void rt_thread_entry_fire_alarm_rx(void* parameter)
{
    int res = 0;
    int i = 0;
    rt_uint32_t evt = 0;

    while(fire_alarm_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        
        //if (rt_sem_take(&sem_FA_rx, RT_WAITING_FOREVER) == RT_EOK)
        // Waiting all channels data receive.
        if (rt_event_recv(&evt_FA_rx, 
                        ((1<<FA_uart_num) - 1), 
                        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 
                        RT_WAITING_FOREVER, 
                        &evt) == RT_EOK)
        {
            for (i=0;i<FA_uart_num;i++)
            {
                p_com_bus_cb = p_FA_com_bus_cb[i];
                
                if (evt && (1<<i))
                {
                    
                    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
                    {
                        com_bus_testing_parser(p_com_bus_cb);
                        
                    }
                    else
                    {
                        if (p_com_bus_cb->rx_parser(p_com_bus_cb) == 0)			 			//数据截取
                        {
                            res = p_com_bus_cb->rx_handler(p_com_bus_cb);					//解析
                            if (res)
                            {
                                
                            }
                        }
                    }
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

int fire_alarm_controller_list(void)
{
    uint32_t FAC_num = 0;
    int i = 0;
    
    FAC_num = sizeof(FAC_config_table)/sizeof(FAC_config_table[0]);
    
    rt_kprintf("Firealarm controller support list :\n");
    
    for (i=0;i<FAC_num;i++)
    {
        rt_kprintf("%d\t%d\t%s\n",
                    FAC_config_table[i].type,
                    FAC_config_table[i].cfg.baudrate,
                    FAC_config_table[i].name
                    );
    }
    
    //rt_kprintf("-------------------------------------------\n");
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(fire_alarm_controller_list, FAC_list, List the supported firealarm controllers.);
#endif // 

