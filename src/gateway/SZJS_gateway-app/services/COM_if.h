#ifndef __COM_H__
#define __COM_H__

#include <rtthread.h>
#include "board.h"
#include "sys_def.h"

#include "stdint.h"

#define COM_IF_VERSION      "1.01"


#define UITD_COM_IO_BUF_SIZE        4096


#define COM_VERSION_MAIN        0
#define COM_VERSION_USER        2



#define COM_SW_VER_MAIN			1
#define COM_SW_VER_USER			1

#define COM_PKT_HEAD_CHAR		'@'
#define COM_PKT_TAIL_CHAR		'#'
#define COM_PKT_HEAD			"@@"
#define COM_PKT_TAIL			"##"

#define COM_PKT_HEAD_LEN        (sizeof(COM_PKT_HEAD) - 1)
#define COM_PKT_TAIL_LEN        (sizeof(COM_PKT_TAIL) - 1)

#define COM_CTRL_LEN			sizeof(t_COM_ctrl_unit)
#define COM_DATA_LEN_MIN		0
#define COM_DATA_LEN_MAX		1024
#define COM_CHECKSUM_LEN		1
#define COM_PACKET_LEN_MIN		(COM_CTRL_LEN + COM_DATA_LEN_MIN + COM_CHECKSUM_LEN)
#define COM_PACKET_LEN_MAX		(COM_CTRL_LEN + COM_DATA_LEN_MAX + COM_CHECKSUM_LEN)
#define COM_ROUGH_LEN_MIN		(COM_PACKET_LEN_MIN + sizeof(COM_PKT_HEAD) + sizeof(COM_PKT_TAIL))
#define COM_ROUGH_LEN_MAX		(COM_PACKET_LEN_MAX + sizeof(COM_PKT_HEAD) + sizeof(COM_PKT_TAIL))
#define COM_ROUGH_BUF_MAX		(COM_ROUGH_LEN_MAX * 4)
#define COM_PACKET_NO_DATA_LEN  COM_PACKET_LEN_MIN
#define COM_PACKET_DATA_HEAD_LEN    2

#define COM_CMD_RESERVE		0
#define COM_CMD_CTRL		1
#define COM_CMD_SEND_DATA	2
#define COM_CMD_AFFIRM		3
#define COM_CMD_REQUIRE		4
#define COM_CMD_ACK			5
#define COM_CMD_DENY		6

#define COM_TYPE_RESERVE    0

#define COM_ACK_TIMEOUT_DEFAULT         100  // need to make sure.
#define COM_SEND_DATA_MAILBOX_TIMEOUT   10

typedef int	(* fun_send_retry)(void *data, int node, int time);
typedef int	(* fun_send_fail)(void *data, int node);
typedef int	(* fun_send_success)(void *data, int node);
//typedef int (* fun_send_data)(int socket, char *data, int len);
typedef int (* fun_send_affirmed)(void *pkt);
typedef int (* fun_send_denyed)(void *pkt);
typedef int (* fun_ack_timeout)(void *pkt);
typedef int (* fun_require_acked)(void *pkt);



//typedef int (* fun_data_deal)(void *pkt);
typedef int (* fun_deal_require)(uint8_t cmd);
typedef int (* fun_deal_ctrl)(uint8_t cmd);
typedef int (* fun_deal_send_data)(uint8_t cmd);



typedef enum
{
	COM_E_OK = 0,
	COM_E_LEN_LESS,
	COM_E_LEN_OVER,
	COM_E_ARG_ERR,
	COM_E_LEN_ERR,
	COM_E_CHECKSUM,
	COM_E_NULL,
	COM_E_BUF_WR,
	COM_E_OTHERS
	
} e_COM_err;

typedef struct
{
	uint8_t	ver_user    :4;
	uint8_t	ver_main    :4;
	
} PACK_STRUCT_STRUCT	t_COM_ctrl_version;

typedef struct
{
	uint8_t	sec;
	uint8_t	min;
	uint8_t	hour;
	uint8_t	day;
	uint8_t	month;
	uint8_t	year;
	
} PACK_STRUCT_STRUCT	t_COM_ctrl_timestamp;

typedef struct
{
	uint8_t addr[6];
	
} PACK_STRUCT_STRUCT	t_COM_ctrl_srcaddr;

typedef struct
{
	uint8_t addr[6];
	
} PACK_STRUCT_STRUCT	t_COM_ctrl_desaddr;


typedef struct
{
	//uint8_t						SN;
	t_COM_ctrl_version		ver;
	//t_COM_ctrl_timestamp	    TS;
	//t_COM_ctrl_srcaddr		src;
	//t_COM_ctrl_desaddr		des;
	uint8_t							cmd;
	uint16_t						data_len;
	
} PACK_STRUCT_STRUCT t_COM_ctrl_unit;

typedef enum
{
	parse_idle = 0,
	parse_head,
	parse_data,
	parse_tail,
} e_COM_parse_status;



typedef struct
{
	uint8_t					valid;
	e_COM_parse_status	    status;
	uint8_t 				buf[COM_ROUGH_BUF_MAX];
	uint32_t 				len;
	
} t_COM_parse;


typedef struct
{
	uint8_t 	type;
	uint8_t		num;
	uint32_t	data_len;
	void 			*data;
} t_COM_data_unit;

typedef enum
{
    COM_idle = 0,
    COM_sending,
    COM_sended,
//    COM_wait_ack,
    COM_ack_timeout,
    COM_rec_ack,
    COM_send_fault,
    
} e_COM_status;

typedef struct
{
    e_COM_status            status;
    uint32_t                timer;
    
	t_COM_ctrl_unit	        ctrl;
	uint8_t					buf[COM_DATA_LEN_MAX];
	uint8_t 				checksum;
	
	uint8_t					data_type;
	uint8_t					data_num;
	
	t_COM_data_unit	        data_unit;
    
    uint8_t                 if_none_data;
//	int 					socket;
	uint8_t					ack_cmd;
//	fun_send_retry 			send_retry;
//	fun_send_fail 			send_fail;
//	fun_send_success 		send_success;
	fun_send_affirmed		send_affirmed;
	fun_send_denyed			send_denyed;
    fun_deal_require        deal_require;
    fun_deal_ctrl           deal_ctrl;
    fun_deal_send_data      deal_send_data;
    fun_require_acked       require_acked;
    
    uint32_t                timeout;
    fun_ack_timeout         ack_timeout;
    
//    fun_data_deal           data_deal;
	rt_device_t             dev;
//	fun_ack_deal				ack_deal;
	
} t_COM_pkt;

typedef struct
{
	//	int 						socket;
	t_COM_ctrl_unit	        ctrl;
	uint8_t					*buf;
	//	uint8_t					len;
	uint8_t 				checksum;
	uint8_t         if_malloc;
	uint8_t                 if_none_data;
	uint8_t                 data_type;
	uint8_t                 data_num;
	uint16_t                data_len;

	t_COM_pkt               *pkt;

}t_COM_send;


extern rt_mailbox_t mail_COM_send;




extern struct rt_semaphore sem_COM_if_rx;

extern int COM_if_init(t_COM_pkt *pkt, rt_device_t dev, fun_deal_require require, fun_deal_ctrl ctrl, fun_deal_send_data send_data);
extern int COM_if_CMD_send_denyed_cb(fun_send_denyed denyed);
extern int COM_if_CMD_send_affrim_cb(fun_send_affirmed acked);
extern int COM_if_ack_timeout_cb(fun_ack_timeout ack_timeout);
extern int COM_if_send_handler(void);
extern int COM_if_rec_handler(void);
extern int COM_send_CMD_data(t_COM_send *pkt , uint32_t timeout);
extern int COM_send_CMD_affirm(void);
extern int COM_send_CMD_deny(void);
extern int COM_check_send_idle(void);


#endif // __COM_H__
