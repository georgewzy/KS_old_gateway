#ifndef __FIRE_ALARM_H__
#define __FIRE_ALARM_H__


#include "board.h"
#include "sys_def.h"
#include <rtdevice.h>
#include "UITD.h"



#define THREAD_FIRE_ALARM_STACK_SIZE        1024
#define THREAD_FIRE_ALARM_RX_STACK_SIZE     512

#define IO_INPUT_PIN_MAX    64

#define FAC_PACKET_LEN_MAX  256
#define FAC_PACKET_LEN_MIN  3


#define COM_BUS_REC_MAX     (FAC_PACKET_LEN_MAX*2)
#define COM_BUS_SEND_MAX    256

#define FAC_TYPE_UNKNOWN            0
#define FAC_TYPE_JB_QB_GST_100      1
#define FAC_TYPE_JB_QB_GST_200      2
#define FAC_TYPE_JB_QB_GST_500      3
#define FAC_TYPE_JB_QB_GST_4000     4
#define FAC_TYPE_JB_QB_GST_5000     5
#define FAC_TYPE_JB_QB_GST_9000     6
#define FAC_TYPE_JB_3101G           7
#define FAC_TYPE_JB_3101            8
#define FAC_TYPE_JB_3101B           9
#define FAC_TYPE_JB_3201A           10
#define FAC_TYPE_JB_3208G_B         11



#define FAC_TYPE_UNUSUAL            10000
#define FAC_TYPE_UNUSUAL_01         (1 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_UNUSUAL_02         (2 + FAC_TYPE_UNUSUAL)

typedef enum
{
    FAC_type_unknown = 0,
    FAC_type_JB_QB_GST_100 = FAC_TYPE_JB_QB_GST_100,
    FAC_type_JB_QB_GST_200 = FAC_TYPE_JB_QB_GST_200,
    FAC_type_JB_QB_GST_500 = FAC_TYPE_JB_QB_GST_500,
    FAC_type_JB_QB_GST_4000 = FAC_TYPE_JB_QB_GST_4000,
    FAC_type_JB_QB_GST_5000 = FAC_TYPE_JB_QB_GST_5000,
    FAC_type_JB_QB_GST_9000 = FAC_TYPE_JB_QB_GST_9000,
    FAC_type_JB_3101G = FAC_TYPE_JB_3101G,
    FAC_type_JB_3101 = FAC_TYPE_JB_3101,
    FAC_type_JB_3101B = FAC_TYPE_JB_3101B,
    FAC_type_JB_3201A = FAC_TYPE_JB_3201A,
    FAC_type_JB_3208G_B = FAC_TYPE_JB_3208G_B,
   
    
    FAC_type_unusual_type = FAC_TYPE_UNUSUAL,
    FAC_type_unusual_01 = FAC_TYPE_UNUSUAL_01,
    FAC_type_unusual_02 = FAC_TYPE_UNUSUAL_02,
    
    FAC_type_end = 65535

} e_FAC_type;


typedef struct
{
    uint32_t    baudrate;
    uint8_t     bits;
    uint8_t     verify;
    uint8_t     stopbit;
} s_com_bus_cfg;

//typedef struct
//{
//    uint8_t         name[32];
//    s_com_bus_cfg   cfg;
//    

//} s_fire_dev_info;

typedef enum
{
    com_bus_status_idle = 0,
    com_bus_status_init,
    com_bus_status_inited,
    com_bus_status_alive,
    com_bus_status_stop,
    com_bus_status_error

} e_com_bus_status;

typedef struct
{
    uint8_t                 valid;
    uint8_t                 sys_addr;
    uint8_t                 addr_main;
    uint8_t                 addr_sub;
} s_com_bus_R_alarm;

//typedef struct
//{
//    uint8_t                 valid;
//    uint8_t                 sys_addr;
//    uint8_t                 addr_main;
//    uint8_t                 addr_sub;
//} s_com_bus_R_fault;



typedef int (* pFun_FAC_rx_parser)(void * cb);
typedef int (* pFun_FAC_rx_handler)(void * cb);
typedef int (* pFun_FAC_server)(void * cb);



typedef struct
{
    uint8_t         if_pre_send;
    uint8_t         pre_send_buf[64];
    uint32_t        pre_send_len;
    uint32_t        pre_send_delay;
} s_FAC_pre_send;

typedef struct
{
    uint8_t         if_rec_alive;
    uint8_t         rec_alive_buf[64];
    uint32_t        rec_alive_len;
    uint8_t         rec_alive_ack_buf[64];
    uint32_t        rec_alive_ack_len;
    uint32_t        rec_alive_delay;
} s_FAC_rec_alive;

typedef struct
{
    uint8_t         if_send_alive;
    uint8_t         send_alive_buf[64];
    uint32_t        send_alive_len;
    uint8_t         send_alive_ack_buf[64];
    uint32_t        send_alive_ack_len;
    uint32_t        send_alive_delay;
} s_FAC_send_alive;


typedef struct
{
    uint8_t         if_reset;
    uint8_t         reset_buf[64];
    uint32_t        reset_len;
    uint8_t         reset_ack_buf[64];
    uint32_t        reset_ack_len;
    uint32_t        reset_delay;
} s_FAC_reset;


typedef struct
{
    uint8_t         if_head;
    uint8_t         head_buf[32];
    uint8_t         head_len;
} s_FAC_alarm_head;

typedef struct
{
    uint8_t         if_tail;
    uint8_t         tail_buf[32];
    uint8_t         tail_len;
} s_FAC_alarm_tail;

typedef enum
{
    FAC_data_HEX = 0,
    FAC_data_BCD = 1,
    FAC_data_ASCII = 2,
} e_FAC_data_type;


typedef struct
{
    uint8_t     num;
    uint8_t     index[3];
} s_FAC_data_HEX;

typedef struct
{
    uint8_t     num;
    uint8_t     index[6];
} s_FAC_data_BCD;

typedef struct
{
    uint8_t     num;
    uint8_t     index[6];
} s_FAC_data_ASCII;

typedef struct
{
    uint8_t             if_data;
    uint16_t            data_index;
    //uint16_t            data_len;
    
    e_FAC_data_type     data_type; 

    s_FAC_data_HEX      data_HEX;
    s_FAC_data_BCD      data_BCD;
    s_FAC_data_ASCII    data_ASCII;
    
} s_FAC_data;


typedef struct
{
    uint8_t         if_alarm;
    uint8_t         buf[8];
    uint16_t        index;
    uint16_t        len;
    
    s_FAC_data      sys_addr;
    s_FAC_data      addr_main;
    s_FAC_data      addr_sub;

} s_FAC_alarm_struct;

//typedef struct
//{
//    uint8_t         if_fault;
//    uint8_t         fault_buf[256];
//    uint16_t        fault_index;
//    uint16_t        fault_len;
//   
//    s_FAC_data      sys_addr;
//    s_FAC_data      addr_main;
//    s_FAC_data      addr_sub;

//} s_FAC_alarm_fault;



typedef struct
{
    s_FAC_alarm_head        head;
    
    s_FAC_alarm_tail        tail;

    uint8_t                 if_alarm_fix_len;
    uint16_t                alarm_fix_len;
    
    s_FAC_alarm_struct      alarm;
    s_FAC_alarm_struct      fault;

} s_FAC_ALARM;


typedef struct
{
    e_FAC_type          type;
    uint8_t             name[32];
    s_com_bus_cfg       cfg;
    uint32_t            alive_period;

    s_FAC_pre_send      pre_send_1;
    s_FAC_pre_send      pre_send_2;
    
    s_FAC_rec_alive     rec_alive_1;
    s_FAC_rec_alive     rec_alive_2;
    
    s_FAC_send_alive    send_alive_1;
    s_FAC_send_alive    send_alive_2;

    s_FAC_reset         reset_1;
    s_FAC_reset         reset_2;
    s_FAC_reset         reset_3;
    
    s_FAC_ALARM         alarm;
    
//    pFun_FAC_rx_parser  rx_parser;
//    pFun_FAC_rx_handler rx_handler;
//    pFun_FAC_server     rx_server;

} s_FAC_config;


typedef enum
{
	FAC_parse_idle = 0,
	FAC_parse_head,
	FAC_parse_data,
	FAC_parse_tail,
    FAC_parse_end,
} e_FAC_parse_status;

typedef struct
{
	uint8_t					valid;
	e_FAC_parse_status	    status;
	uint8_t 				buf[FAC_PACKET_LEN_MAX];
	uint32_t 				len;
	
} t_FAC_parse;

typedef struct
{
    e_com_bus_status    status;
    rt_device_t         dev;
    void                *handler;
    //s_com_bus_cfg       com_cfg;
    uint32_t            table_index;
    uint32_t            table_num;
    
    uint32_t            alive_timer;
    uint32_t            alive_period;
    
    e_FAC_type          FAC_type;
    s_FAC_config        FAC_config;
    
    t_FAC_parse         parse;
    
    uint8_t             rec_buf[COM_BUS_REC_MAX];
    uint32_t            rec_len;
    uint32_t            rec_index;
//    uint8_t             send_buf[COM_BUS_SEND_MAX];
//    uint32_t            send_len;
//    uint32_t            send_index;
    
    void *              FAC_handler;
    pFun_FAC_rx_parser  rx_parser;
    pFun_FAC_rx_handler rx_handler;
    pFun_FAC_server     rx_server;
    
    s_com_bus_R_alarm   alarm_fire;
    s_com_bus_R_alarm   alarm_fault;

} s_com_bus_cb;

typedef enum
{
    IO_input_status_idle = 0,
    IO_input_status_init,
    IO_input_status_inited,
    IO_input_status_working,
    IO_input_status_stop,
    IO_input_status_error
} e_IO_input_status;

typedef enum
{
    IO_input_state_low = 0,
    IO_input_state_high = 1,
} e_IO_input_state;


typedef enum
{
    IO_input_trig_none = 0,
    IO_input_trig_rise = 1,
    IO_input_trig_fall = 2,
    IO_input_trig_edge = 3,
    IO_input_trig_period = 4,
} e_IO_input_trig;


typedef struct
{
    uint8_t             ID;
    e_IO_input_state    state;
    
} s_IO_input_report;

typedef struct
{
    uint8_t             ID;
    e_IO_input_trig     trig;
    uint32_t            period;
} s_IO_input_cfg_report;

typedef struct
{
    uint8_t             ID; // > 0
    uint8_t             pin;
    e_IO_input_state    state;
    e_IO_input_state    state_last;
    e_IO_input_trig     state_trig;
    uint32_t            period; // unit: S
    uint32_t            timer;  // unit: S
    
} s_IO_input;

typedef struct
{
    e_IO_input_status   status;
    void                *handler;
    
    s_IO_input          state[IO_INPUT_PIN_MAX];
    uint8_t             IO_num;
    
    uint8_t             TS_flag;
    uint32_t            timestamp;

} s_IO_input_cb;

extern volatile s_IO_input_cb *p_IO_input_cb;


extern volatile s_com_bus_cb *p_com_bus_cb;

extern struct rt_messagequeue mq_FA_fire;
extern struct rt_messagequeue mq_FA_fault;
extern struct rt_messagequeue mq_IO_input;
extern struct rt_mutex  mt_IO_input;

extern struct rt_semaphore sem_FA_rx;

extern rt_uint8_t prio_fire_alarm;
extern rt_thread_t thread_fire_alarm;
extern rt_uint8_t prio_fire_alarm_rx;
extern rt_thread_t thread_fire_alarm_rx;


extern void rt_thread_entry_fire_alarm(void* parameter);
extern void rt_thread_entry_fire_alarm_rx(void* parameter);



#endif // __FIRE_ALARM_H__
