#ifndef __FIRE_ALARM_H__
#define __FIRE_ALARM_H__


#include "IOT_analog_in.h"
#include "IOT_digit_in.h"



#define THREAD_FIRE_ALARM_STACK_SIZE        2048
#define THREAD_FIRE_ALARM_RX_STACK_SIZE     512

#define FAC_PORT_DEFAULT    1

#define FA_COM_BUS_MAX      5



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
#define FAC_TYPE_JB_QB_5Ei          12
#define FAC_TYPE_JB_QX_5Li          13
#define FAC_TYPE_JB_1501_A          14
#define FAC_TYPE_JB_QB_21S_VFC3010A 15
#define FAC_TYPE_JB_TG_JBF_11S      16
#define FAC_TYPE_JB_TG_JBF_11S_2013 17
#define FAC_TYPE_BT8002             18
#define FAC_TYPE_FC_720R            19
#define FAC_TYPE_CHNTAD_8000        20
#define FAC_TYPE_JB_Q100GZ2L_LA040  21
#define FAC_TYPE_TX3000             22
#define FAC_TYPE_SD2100_2200        23
#define FAC_TYPE_V6851              24



#define FAC_TYPE_UNUSUAL            10000  // use external parse program to parse communication protocol.
#define FAC_TYPE_UNUSUAL_01         (1 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_UNUSUAL_02         (2 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_UNUSUAL_03         (3 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_UNUSUAL_04         (4 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_UNUSUAL_05         (5 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_JB_QT_LD_128K_HB   (6 + FAC_TYPE_UNUSUAL)
#define FAC_TYPE_FC18R              (7 + FAC_TYPE_UNUSUAL)

#define FAC_TYPE_USUAL_CFG          20000 // use usual config to parse the communication protocol.


#define FAC_TYPE_UNUSUAL_MODULE     10016


#define FA_TYPE_DEFAULT     FAC_TYPE_JB_QB_GST_5000
//#define FA_TYPE_DEFAULT     FAC_TYPE_JB_TG_JBF_11S


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
    FAC_type_JB_QB_5Ei = FAC_TYPE_JB_QB_5Ei,
    FAC_type_JB_QX_5Li = FAC_TYPE_JB_QX_5Li,
    FAC_type_JB_1501_A = FAC_TYPE_JB_1501_A,
    FAC_type_JB_QB_21S_VFC3010A = FAC_TYPE_JB_QB_21S_VFC3010A,
    FAC_type_JB_TG_JBF_11S = FAC_TYPE_JB_TG_JBF_11S,
    FAC_type_JB_TG_JBF_11S_2013 = FAC_TYPE_JB_TG_JBF_11S_2013,
    FAC_type_BT8002 = FAC_TYPE_BT8002,
    FAC_type_FC_720R = FAC_TYPE_FC_720R,
    FAC_type_CHNTAD_8000 = FAC_TYPE_CHNTAD_8000,
    FAC_type_JB_Q100GZ2L_LA040 = FAC_TYPE_JB_Q100GZ2L_LA040,
    FAC_type_TX3000 = FAC_TYPE_TX3000,
    FAC_type_SD2100_2200 = FAC_TYPE_SD2100_2200,
    FAC_type_V6851 = FAC_TYPE_V6851,    
    
    
    FAC_type_unusual_type = FAC_TYPE_UNUSUAL,
    FAC_type_unusual_01 = FAC_TYPE_UNUSUAL_01,
    FAC_type_unusual_02 = FAC_TYPE_UNUSUAL_02,
    FAC_type_JB_3102_A = FAC_TYPE_UNUSUAL_03,
    FAC_type_FC18R = FAC_TYPE_FC18R,


    FAC_type_usual_cfg = FAC_TYPE_USUAL_CFG,


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
    com_bus_status_error,
    
    com_bus_status_test_waiting,
    com_bus_status_testing,
    com_bus_status_tested,

} e_com_bus_status;

typedef struct
{
    uint8_t                 valid;
    uint16_t                port;
    uint32_t                sys_addr;
    uint32_t                addr_main;
    uint32_t                addr_sub;
    s_fire_dev_status_info  dev_info;
} s_com_bus_R_alarm;

typedef struct
{
    uint8_t                 valid;
    uint8_t                 port;
    uint8_t                 sys_addr;   //设备机
} s_com_bus_R_reset;


typedef struct
{
    uint8_t                 valid;
} s_com_bus_R_power_off;



typedef int (* pFun_FAC_rx_init) (void * cb);
typedef int (* pFun_FAC_rx_parser)(void * cb);
typedef int (* pFun_FAC_rx_handler)(void * cb);
typedef int (* pFun_FAC_server)(void * cb);



typedef struct
{
    uint8_t         if_pre_send;
    uint8_t         pre_send_buf[32];
    uint32_t        pre_send_len;
    uint32_t        pre_send_delay;
} s_FAC_pre_send;

typedef struct
{
    uint8_t         if_rec_alive;
    uint8_t         rec_alive_buf[32];
    uint32_t        rec_alive_len;
    uint8_t         rec_alive_ack_buf[32];
    uint32_t        rec_alive_ack_len;
    uint32_t        rec_alive_delay;
} s_FAC_rec_alive;

typedef struct
{
    uint8_t         if_send_alive;
    uint8_t         send_alive_buf[32];
    uint32_t        send_alive_len;
    uint8_t         send_alive_ack_buf[32];
    uint32_t        send_alive_ack_len;
    uint32_t        send_alive_delay;
} s_FAC_send_alive;


typedef struct
{
    uint8_t         if_reset;
    uint8_t         reset_buf[32];
    uint32_t        reset_len;
    uint8_t         reset_ack_buf[32];
    uint32_t        reset_ack_len;
    uint32_t        reset_delay;
} s_FAC_reset;


typedef struct
{
    uint8_t         if_head;						//判断协议是否有 包头
    uint8_t         head_buf[8];				//包头内容
    uint8_t         head_len;						//包头长度
} s_FAC_alarm_head;

typedef struct
{
    uint8_t         if_tail;						//判断协议是否有 包尾
    uint8_t         tail_buf[8];				//包尾内容
    uint8_t         tail_len;						//包尾长度
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
    uint8_t         if_alarm;							//是否包含报警信息
    uint8_t         buf[8];
    uint16_t        index;
    uint16_t        len;
    
    s_FAC_data      sys_addr;
    s_FAC_data      addr_main;
    s_FAC_data      addr_sub;

} s_FAC_alarm_struct;

typedef struct
{
    uint8_t         if_reset;							
    uint8_t         buf[8];
    uint16_t        index;
    uint16_t        len;
    uint8_t         if_sys_addr;
    uint8_t         sys_addr;
    uint8_t         sys_addr_index;
    
} s_FAC_RESET;


typedef struct
{
    s_FAC_alarm_head        head;											//包头
    
    s_FAC_alarm_tail        tail;											//包尾

    uint8_t                 if_alarm_fix_len;				  //没有包尾的情况下 判断是否有包长度
    uint16_t                alarm_fix_len;						//包长度
    
    s_FAC_alarm_struct      alarm;										//火警报警
    s_FAC_alarm_struct      fault;										//异常报警

} s_FAC_ALARM;


typedef struct
{
    e_FAC_type          type;									//控制器型号标识
    uint8_t             name[32];							//控制器型号名称
    s_com_bus_cfg       cfg;									//串口信息参数
    uint32_t            alive_period;														//数据上报服务器 周期

//    s_FAC_pre_send      pre_send_1;
//    s_FAC_pre_send      pre_send_2;
    
    s_FAC_rec_alive     rec_alive_1;														//
//    s_FAC_rec_alive     rec_alive_2;
    
    s_FAC_send_alive    send_alive_1;														//数据上报服务器 结构1
    s_FAC_send_alive    send_alive_2;														//数据上报服务器 结构2

    s_FAC_reset         reset_1;																//
//    s_FAC_reset         reset_2;
//    s_FAC_reset         reset_3;
    
    s_FAC_ALARM         alarm;								//报警数据信息
    s_FAC_RESET         reset;     						//复位数据信息

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
    rt_device_t         dev;									//设备接口															
    void                *handler;
    void                *handler_2;
    //s_com_bus_cfg       com_cfg;
    uint32_t            table_index;
    uint32_t            table_num;
    
    uint32_t            alive_timer;
    uint32_t            alive_period;
    
    e_FAC_type          FAC_type;
    s_FAC_config        FAC_config;								//包含串口配置参数信息
    uint8_t             FA_listen;
    
    t_FAC_parse         parse;									//解析数据包结构
    
    uint8_t             rec_buf[COM_BUS_REC_MAX];				//接收数据缓存
    uint32_t            rec_len;								//接收数据的长度
    uint32_t            rec_index;								//接收数据索引
//    uint8_t             send_buf[COM_BUS_SEND_MAX];
//    uint32_t            send_len;
//    uint32_t            send_index;
    uint8_t             port;
    
		//----so文件回调函数指针
    void *              FAC_handler;
    pFun_FAC_rx_init    rx_init;
    pFun_FAC_rx_parser  rx_parser;
    pFun_FAC_rx_handler rx_handler;
    pFun_FAC_server     rx_server;
		//-----
    
    s_com_bus_R_alarm   alarm_fire;								//火警报警信息
    s_com_bus_R_alarm   alarm_fault;							//异常报警信息
    s_com_bus_R_power_off   alarm_power_off;
    s_com_bus_R_reset   alarm_reset;							//火警复位信息
		
		s_com_bus_R_alarm		alarm_shield;							//屏蔽
		s_com_bus_R_alarm		alarm_feedback;						//反馈
		s_com_bus_R_alarm		alarm_supervise;					//监管
		s_com_bus_R_alarm		alarm_start;							//启动
		s_com_bus_R_reset   alarm_outing;							//巡检

} s_com_bus_cb;


typedef struct
{
    uint8_t     FA_name[16];
    uint16_t    FA_type;
    uint32_t    FA_baud;
    uint8_t     FA_listen;
} s_FA_uart_cfg;




extern uint8_t FA_uart_num;

extern s_com_bus_cb *p_FA_com_bus_cb[FA_COM_BUS_MAX];
extern volatile s_com_bus_cb *p_com_bus_cb;

extern uint16_t g_FA_type;
extern uint32_t g_FA_baud;
extern uint32_t g_FA_listen;
extern struct rt_messagequeue *mq_FA_fire;
extern struct rt_messagequeue *mq_FA_2_fire;
extern struct rt_messagequeue *mq_FA_fault;
extern struct rt_messagequeue *mq_FA_2_fault;
extern struct rt_messagequeue *mq_FA_power_off;
extern struct rt_messagequeue *mq_FA_2_power_off;
extern struct rt_messagequeue *mq_FA_manul_fire;
extern struct rt_messagequeue *mq_FA_2_manul_fire;
extern struct rt_messagequeue *mq_FA_reset;
extern struct rt_messagequeue *mq_FA_2_reset;


extern struct rt_semaphore sem_FA_rx;

extern rt_uint8_t prio_fire_alarm;
extern rt_thread_t thread_fire_alarm;
extern rt_uint8_t prio_fire_alarm_rx;
extern rt_thread_t thread_fire_alarm_rx;


extern void rt_thread_entry_fire_alarm(void* parameter);
extern void rt_thread_entry_fire_alarm_rx(void* parameter);


#endif // __FIRE_ALARM_H__
