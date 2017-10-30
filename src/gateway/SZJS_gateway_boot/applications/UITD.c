#define DEBUG_UITD

#ifdef DEBUG_UITD
#define UITD_PRINTF     rt_kprintf
#define UITD_HEX_DUMP   UITD_print_hex_dump
#endif // DEBUG_UITD

#include "UITD.h"
#include <rtdevice.h>
#include "GSM_mg301.h"
//#include <time.h>
#include "rtc.h"
#include <dfs_posix.h>

#include "CRC16.h"
#include "sensor_sample.h"
#include "fire_alarm.h"

////#define UITD_SERVICE_TESTING    

//#include "ringbuffer.h"

//#include "test.h"
//#include "drivers.h"
//#include "app.h"
//#include "lwip\sockets.h"

//s_FW_update  FW_update = {0};

//typedef enum
//{
//	GB_CMD_reserve  	= 0,
//	GB_CMD_ctrl				= 1,
//	GB_CMD_send_data	= 2,
//	GB_CMD_affirm			= 3,
//	GB_CMD_require		= 4,
//	GB_CMD_ack				= 5,
//	GB_CMD_deny				= 6,
//	
//} e_GB_CMD;

//extern int a_new_client_sock_fd ;

uint8_t	g_UITD_addr[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

static s_sensor_rough_data sensor_data_temp = {0};
static s_com_bus_R_alarm FA_fire_temp = {0};
static s_com_bus_R_alarm FA_fault_temp = {0};
static s_IO_input_report IO_input_temp = {0};
static s_IO_input_cfg IO_input_cfg_temp = {0};
static s_IO_input_cfg *p_IO_input_cfg = NULL;
static s_IO_input_cfg_report IO_input_cfg_report_temp = {0};
static s_sensor_cfg_report  sensor_cfg_temp = {0};
static s_sensor_ID_report   sensor_ID_temp[SENSOR_HANDLER_MAX] = {0};
//static s_sensor_data_report sensor_data_report_temp = {0};

//t_GB_ctrl_timestamp g_GB_TS = {0};

//t_server_node  server_node[GB_SERVER_NODE_MAX] = {0};
////t_server_node	 *server_node = NULL;
//t_GB_parse GB_parse = {0};
//t_GB_pkt GB_pkt_rec = {0};
//t_GB_pkt GB_pkt_send = {0};
//uint8_t GB_pkt_buff[GB_PACKET_LEN_MAX] = {0};
//uint8_t GB_rough_buff[GB_ROUGH_BUF_MAX] = {0};

//RingBuffer *ring_buf = NULL;

//static struct rt_ringbuffer    *UITD_ring = NULL;
//static uint8_t  UITD_ring_buf[GB_ROUGH_BUF_MAX] = {0};

////uint8_t g_data_buf[2048] = {0};
//t_server_handler server_handler;

void UITD_print_hex_dump(uint8_t *str, uint8_t *buf, uint32_t len)
{
    int i = 0;
    
    rt_kprintf("%s:", str);
    for (i=0;i<len;i++)
    {
        rt_kprintf(" %02X", buf[i]);
    }
    rt_kprintf("\n");
}

int UITD_cb_ack_deal_somke_alarm(char *data,int len, uint8_t if_affirm)
{
	UITD_PRINTF("CB ack deal -- somke alarm, if_affirm : %d\n", if_affirm);
	return 0;
}

int UITD_cb_send_fail_somke_alarm(void *data, int node)
{
	UITD_PRINTF("CB send fail -- somke alarm, node : %d\n", node);
	return 0;
}

int UITD_net_send_data(int s, char *data, int len)
{
	//return TCP_Send_Data(s, data, len, MSG_DONTWAIT);
}

void UITD_send_pkt_init(t_GB_pkt *pkt)
{
	rt_memset(pkt, 0, sizeof(t_GB_pkt));
}


int UITD_file_open(uint8_t *filename)
{
    int fd = -1;
    /* test and open */
    fd = open(filename, O_RDONLY, 0);
    if (fd >= 0)
    {
        close(fd);
        fd = open(filename, O_WRONLY | O_TRUNC, 0);
    }
    else
    {
        /* file not exists */
        fd = open(filename, O_WRONLY | O_CREAT, 0);
    }
    return fd;
}


int UITD_create_dev_type(e_dev_type sys_dev_type, s_FW_support_dev *dev_type)
{
    rt_memset(dev_type, 0x00, sizeof(s_FW_support_dev));
    
    switch (sys_dev_type)
    {
        case dev_type_sample_ctrl:
            dev_type->dev_sample_ctrl = 1;
            break;
        case dev_type_simple_UITD:
            dev_type->dev_simple_UITD = 1;
            break;
        default:
            break;
    }
    
    return 0;
}



t_server_handler *UITD_init(fun_send_data send_data, 
                            pFun_srv_connect connect, 
                            pFun_srv_connected connected,
                            pFun_srv_disconnect disconnect,
                            pFun_srv_disconnected disconnected,
                            pFun_srv_connect_fail connect_fail,
                            pFun_srv_received received,
                            pFun_srv_rejected rejected,
                            pFun_srv_closed closed,
                            pFun_srv_listen listen,
                            pFun_srv_accepted accepted,
                            pFun_srv_report_IP report_IP
                            )
{
    t_server_handler *handler;
    uint8_t *p = NULL;
    
    handler = rt_malloc(sizeof(t_server_handler));
    if (handler == NULL) return NULL;
    
    rt_memset(handler, 0x00, sizeof(t_server_handler));
    
    rt_ringbuffer_init(&handler->ring_buf, handler->ring_buf_buf, sizeof(handler->ring_buf_buf));
    
    handler->if_connected = 0;
    handler->socket = -1;

    
	handler->send_data = send_data;
    handler->connect = connect;
    handler->connected = connected;
    handler->disconnect = disconnect;
    handler->disconnected = disconnected;
    handler->connect_fail = connect_fail;
    handler->received = received;
    handler->rejected = rejected;
    handler->closed = closed;
    handler->listen = listen;
    handler->accepted = accepted;
    handler->report_IP = report_IP;
    	
	handler->send_SN = 0;
	rt_memcpy(handler->sys_addr, g_UITD_addr, sizeof(handler->sys_addr));
	
    
	handler->ver_main = GB_SW_VER_MAIN;
	handler->ver_user = GB_SW_VER_USER;
    
    handler->FW.file_open = &UITD_file_open;
    handler->FW.file_write = &write;
    handler->FW.file_close = &close;
    
    UITD_create_dev_type(sys_dev_type, &handler->dev_type);
    
    //rt_memcpy(handler->FW.fw_path, FW_FILE_PATH, sizeof(FW_FILE_PATH)+1);
    
	return handler;
}

int UITD_destroy(t_server_handler *handler)
{
    
    rt_free(handler);
    return 0;
}

//int UITD_cfg_IP(t_server_handler *handler, uint8_t if_UDP, uint8_t *IP, uint16_t port)
//{
//    
//    
//    handler->if_UDP = if_UDP;
//    handler->IP[0] = IP[0];
//    handler->IP[1] = IP[1];
//    handler->IP[2] = IP[2];
//    handler->IP[3] = IP[3];
//    handler->port = port;

//}

int UITD_listen(t_server_handler *handler, uint8_t if_UDP, uint16_t listen_port)
{

    handler->if_UDP = if_UDP;
    handler->listen_port = listen_port;
        
    return handler->listen(handler->if_UDP, handler->listen_port, 
                                handler->accepted,
                                handler->disconnected,
                                handler->received,
                                handler->closed,
                                handler->report_IP
                                );
}

int UITD_connect(t_server_handler *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port)
{
    //handler->status = UITD_svc_status_idle;
    UITD_service_init(handler);
    
    
    handler->if_UDP = if_UDP;
    handler->if_use_name = if_use_name;
    
    if (handler->if_use_name == 0)
    {
        sscanf(IP, "%d.%d.%d.%d", 
                    &handler->IP[0],
                    &handler->IP[1],
                    &handler->IP[2],
                    &handler->IP[3]
                    );
        handler->port = port;
        
        return handler->connect(handler->if_UDP, handler->if_use_name, handler->IP, handler->port, 
                                handler->connect_fail,
                                handler->connected,
                                handler->disconnected,
                                handler->received,
                                handler->rejected,
                                handler->closed,
                                handler->report_IP
                                );
    }
    else
    {
        if (strlen(IP) >= SVR_IF_NAME_LEN)
        {
            UITD_PRINTF("Server name is too long ! connect failed \n");
            return -1;
        }
        strcpy(handler->svr_name, IP);
        handler->port = port;
        
        return handler->connect(handler->if_UDP, handler->if_use_name, handler->svr_name, handler->port, 
                                handler->connect_fail,
                                handler->connected,
                                handler->disconnected,
                                handler->received,
                                handler->rejected,
                                handler->closed,
                                handler->report_IP
                                );
    }
    
}

int UITD_disconnect(t_server_handler *handler)
{
    handler->status = UITD_svc_status_stoped;
    rt_free(handler->FW.data);
    
    if (handler->disconnect == NULL)
    {
        handler->disconnect(handler->socket);
    }
    
    GPRS_disconnect(handler->socket);
    
//    while(1)
//    {
//        if (GPRS_srv_get(handler->socket) == NULL) break;
//        rt_thread_delay(1);
//    }
    
}


int UITD_server_clean_node(t_server_handler *handler)
{
	int i = 0;
	
	for (i=0;i<GB_SERVER_NODE_MAX;i++)
	{
		handler->node[i].node_status = GB_NODE_unused;
	}
	
	return 0;
}

int UITD_server_request_node(t_server_handler *handler)
{
	int i = 0;
	
	for (i=0;i<GB_SERVER_NODE_MAX;i++)
	{
		if (handler->node[i].node_status == GB_NODE_unused)
		{
			return i;
		}
	}
	
	return -1;
}

int UITD_server_search_node(t_server_handler *handler, uint16_t SN)
{
	int i = 0;
	
	for (i=0;i<GB_SERVER_NODE_MAX;i++)
	{
		if (handler->node[i].node_status == GB_NODE_sending)
		{
			if ((handler->node[i].ctrl.SN == SN) && (handler->node[i].ack_status == cmd_acked_waiting))
			{
				return i;
			}
		}
	}
	
	return -1;
}

int UITD_server_release_node(t_server_handler *handler, int node_num)
{
	int i = 0;
	
	if (node_num >= GB_SERVER_NODE_MAX)
	{
		return -1;
	}
	
	handler->node[node_num].node_status = GB_NODE_unused;
	
	return 0;
}


int UITD_parse(t_server_handler *handler, t_GB_parse *parse)
{
	uint8_t data_temp = 0x00;
	int res = 0;
    t_GB_ctrl_unit *ctrl = NULL;
	
/*	
	if (RingBuffer_empty(ring_buf))
	{
		return -1;
	}
*/
	////UITD_PRINTF("Ringbuf data number 03 : %d ", RingBuffer_available_data(ring_buf));
	while(1)
	{
		////UITD_PRINTF("Ringbuf data number 04 : %d ", RingBuffer_available_data(ring_buf));
		res = rt_ringbuffer_getchar(&handler->ring_buf, &data_temp);
		if (res == 0)
		{
			////UITD_PRINTF("!");
			return -1;
		}
		////UITD_PRINTF("%02X , %d", data_temp, parse->status);
		switch (parse->status)
		{
			case UITD_parse_idle:
				if (data_temp == GB_PKT_HEAD_CHAR)
				{
					parse->status = UITD_parse_head;
					parse->len = 0;
					parse->valid = 0;
				}
				else
				{
					
				}
				break;
			case UITD_parse_head:
				if (data_temp == GB_PKT_HEAD_CHAR)
				{
					parse->status = UITD_parse_data;
				}
				else
				{
					parse->status = UITD_parse_idle;
				}
				
				break;
			case UITD_parse_data:
				if (data_temp == GB_PKT_TAIL_CHAR)
				{
					parse->status = UITD_parse_tail;
					
				}
				else
				{
					parse->buf[parse->len] = data_temp;
					parse->len ++;
					if (parse->len > GB_PACKET_LEN_MAX)
					{
						parse->status = UITD_parse_idle;
					}
				}
				break;
			case UITD_parse_tail:
				if (data_temp == GB_PKT_TAIL_CHAR)
				{
                    // Checking the packet lenght, if too short, discard.
                    if (parse->len < GB_PACKET_LEN_MIN)
                    {
                        parse->status = UITD_parse_idle;
                        break;
                    }
                    
                    // If data body lenght is not equal the actual packet, go on receiving the data.
                    ctrl =  (t_GB_ctrl_unit *)parse->buf;
                    if (ctrl->data_len != (parse->len - GB_PACKET_NO_DATA_LEN))
                    {
                        parse->buf[parse->len] = GB_PKT_TAIL_CHAR;
                        parse->len ++;
                        parse->buf[parse->len] = GB_PKT_TAIL_CHAR;
                        parse->len ++;
                        if (parse->len > GB_PACKET_LEN_MAX)
                        {
                            parse->status = UITD_parse_idle;
                            break;
                        }
                        parse->status = UITD_parse_data;
                        break;
                    }
                    
                    // Packet is parsed correctly, exit.
					parse->valid = 1;
					////UITD_PRINTF("Parse valid ");
					parse->status = UITD_parse_idle;
					return 0;
				}
				else
				{
					parse->buf[parse->len] = GB_PKT_TAIL_CHAR;
					parse->len ++;
					parse->buf[parse->len] = data_temp;
					parse->len ++;
					if (parse->len > GB_PACKET_LEN_MAX)
					{
						parse->status = UITD_parse_idle;
                        break;
					}
					parse->status = UITD_parse_data;
				}
				
				break;
			default:
				
				break;
		}
		
	}
	return -1;
}



e_GB_err UITD_decode(t_server_handler *handler, t_GB_pkt *pkt, uint8_t *data, uint32_t data_len, int socket)
{
	t_GB_ctrl_unit *ctrl = NULL;
	int k=0;
	uint32_t checksum = 0;
	uint8_t *p_ctrl = NULL;
	
	if (data_len < GB_PACKET_LEN_MIN)
	{
		
		return GB_E_LEN_LESS;
	}
	if (data_len > GB_PACKET_LEN_MAX)
	{
		
		return GB_E_LEN_OVER;
	}
	if (data_len)
	{
		rt_memcpy(handler->GB_pkt_buff, data, data_len);
	}
	////ctrl = &pkt->ctrl;
	
	ctrl = (t_GB_ctrl_unit *)handler->GB_pkt_buff;
	
	if (ctrl->data_len > GB_DATA_LEN_MAX)
	{
	
		return GB_E_ARG_ERR;
	}
	
	if ( (ctrl->data_len + GB_CTRL_LEN + GB_CHECKSUM_LEN) != data_len)
	{
	
		return GB_E_LEN_ERR;
	}
	else
	{
		rt_memcpy(&pkt->ctrl, handler->GB_pkt_buff, GB_CTRL_LEN);
		if (ctrl->data_len >= 2)
		{
			rt_memcpy(&pkt->buf, &handler->GB_pkt_buff[GB_CTRL_LEN], ctrl->data_len);
			pkt->data_type = pkt->buf[0];
			pkt->data_num = pkt->buf[1];
			
		}
		else
		{
			pkt->data_type = GB_TYPE_0_RESERVE;
			pkt->data_num = 0;
		}
		
		pkt->checksum = handler->GB_pkt_buff[GB_CTRL_LEN + pkt->ctrl.data_len];
		checksum = 0;
		p_ctrl = (uint8_t *)&pkt->ctrl;
		for (k=0;k<GB_CTRL_LEN;k++)
		{
			checksum += p_ctrl[k];
		}
		for (k=0;k<pkt->ctrl.data_len;k++)
		{
			checksum += pkt->buf[k];
		}
		checksum %= 256;
		if (checksum != pkt->checksum)
		{
			return GB_E_CHECKSUM;
		}
		
		pkt->socket = socket;
		
	}
	
	
	UITD_PRINTF("SN      : %d \n", ctrl->SN);
	
	UITD_PRINTF("Ver_main: %d \n", ctrl->ver.ver_main);
	UITD_PRINTF("Ver_user: %d \n", ctrl->ver.ver_user);
	
	UITD_PRINTF("TimeStamp: %d-%d-%d %d:%d:%d \n", 
									ctrl->TS.year + 2000, 
									ctrl->TS.month, 
									ctrl->TS.day, 
									ctrl->TS.hour, 
									ctrl->TS.min, 
									ctrl->TS.sec);
	UITD_PRINTF("IP src : %d-%d-%d-%d-%d-%d \n", 
									ctrl->src.addr[5], 
									ctrl->src.addr[4], 
									ctrl->src.addr[3], 
									ctrl->src.addr[2], 
									ctrl->src.addr[1], 
									ctrl->src.addr[0]);

	UITD_PRINTF("IP des : %d-%d-%d-%d-%d-%d \n", 
									ctrl->des.addr[5], 
									ctrl->des.addr[4], 
									ctrl->des.addr[3], 
									ctrl->des.addr[2], 
									ctrl->des.addr[1], 
									ctrl->des.addr[0]);
	
	UITD_PRINTF("CMD : %d \n", ctrl->cmd);
	
	UITD_PRINTF("Checksum : %d , 0x%02X\n", pkt->checksum, pkt->checksum);
	
	UITD_PRINTF("Data_len : %d \n", ctrl->data_len);
	UITD_HEX_DUMP("Data_buf", pkt->buf, pkt->ctrl.data_len);

	return GB_E_OK;
}


int UITD_get_time(t_GB_ctrl_timestamp *TS)
{
    uint32_t now;
    struct tm *ti;
    
    rt_time(&now);
    
    ti = localtime(&now);
    
	TS->year = ti->tm_year + 1900 - 2000;
	TS->month = ti->tm_mon + 1;
	TS->day = ti->tm_mday;
	TS->hour = ti->tm_hour;
	TS->min = ti->tm_min;
	TS->sec = ti->tm_sec;
    
//	TS->year = 16;
//	TS->month = 5;
//	TS->day = 6;
//	TS->hour = 23;
//	TS->min = 1;
//	TS->sec = 30;
    
	return 0;
}

int UITD_fill_ctrl(t_GB_ctrl_unit *ctrl_src, t_GB_ctrl_unit *ctrl_des, uint32_t data_len)
{
	
	ctrl_des->ver.ver_main = GB_SW_VER_MAIN;
	ctrl_des->ver.ver_user = GB_SW_VER_USER;
	
	ctrl_des->SN = ctrl_src->SN;
	rt_memcpy(ctrl_des->src.addr, ctrl_src->src.addr, sizeof(t_GB_ctrl_srcaddr));
	rt_memcpy(ctrl_des->des.addr, ctrl_src->des.addr, sizeof(t_GB_ctrl_desaddr));
	ctrl_des->data_len = data_len;
	// TODO: get time , fill the timestamp field.
	UITD_get_time(&ctrl_des->TS);	
	return 0;
}


int UITD_affirm(t_server_handler *handler, t_GB_pkt *pkt)
{
	pkt->ack_cmd = GB_CMD_AFFIRM;
	
	pkt->send_acked = NULL;
	pkt->send_fail = NULL;
	pkt->send_retry = NULL;
	pkt->send_success = NULL;
	pkt->ack_deal = NULL;
	
	return UITD_send_ack(handler, pkt, NULL);
}

int UITD_deny(t_server_handler *handler, t_GB_pkt *pkt)
{
	pkt->ack_cmd = GB_CMD_DENY;
	
	pkt->send_acked = NULL;
	pkt->send_fail = NULL;
	pkt->send_retry = NULL;
	pkt->send_success = NULL;
	pkt->ack_deal = NULL;
	
	return UITD_send_ack(handler, pkt, NULL);
}

int UITD_ack(t_server_handler *handler, t_GB_pkt *pkt, t_GB_data_unit *data_unit)
{
	pkt->ack_cmd = GB_CMD_ACK;
	
	pkt->send_acked = NULL;
//	pkt->send_fail = NULL;
	pkt->send_retry = NULL;
//	pkt->send_success = NULL;
	pkt->ack_deal = NULL;
	
	return UITD_send_ack(handler, pkt, data_unit);
}

int UITD_send_node_init(t_server_handler *handler)
{
    int i = 0;
    for (i=0;i<GB_SERVER_NODE_MAX;i++)
    {
        handler->node[i].node_status = GB_NODE_unused;
    }
    return 0;
}


int UITD_send_data(t_server_handler *handler,
                    t_GB_pkt *pkt, 
                    t_GB_data_unit *data_unit,
					uint8_t	if_active_send)
{
	int index = 0;
	int i = 0;
	uint32_t checksum = 0;
	uint8_t *data = NULL;
	
	if ((data_unit != 0) && ((data_unit->data_len + 2) > GB_DATA_LEN_MAX))
	{
		return -4;
	}
	
	index = UITD_server_request_node(handler);
	if (index < 0)
	{
		return -1;
	}
	
	if (if_active_send)
	{
		handler->node[index].if_active_send = 1;
		handler->node[index].ctrl.cmd = pkt->ctrl.cmd;
		pkt->ctrl.SN = handler->send_SN;
		handler->send_SN ++;
		pkt->ctrl.ver.ver_main = handler->ver_main;
		pkt->ctrl.ver.ver_user = handler->ver_user;
		rt_memcpy(pkt->ctrl.src.addr, handler->sys_addr, sizeof(pkt->ctrl.src));
		rt_memcpy(pkt->ctrl.des.addr, handler->server_addr, sizeof(pkt->ctrl.des));
	}
	else
	{
		handler->node[index].if_active_send = 0;
		handler->node[index].ctrl.cmd = pkt->ack_cmd;
	}
	
	if (data_unit != NULL)
	{
		UITD_fill_ctrl(&pkt->ctrl, &handler->node[index].ctrl, data_unit->data_len + 2);
	}
	else
	{
		UITD_fill_ctrl(&pkt->ctrl, &handler->node[index].ctrl, 0);
	}
	
	data = (uint8_t *)&handler->node[index].ctrl;
	for (i=0;i<sizeof(t_GB_ctrl_unit);i++)
	{
		checksum += data[i];
	}
	
	UITD_PRINTF("Ctrl checksum: 0x%02X\n", checksum);
	//handler->node[index].checksum = checksum%256;
	
	if (data_unit != NULL)
	{
		handler->node[index].data = malloc(data_unit->data_len + 2);
		if (handler->node[index].data == NULL)
		{
			
			UITD_PRINTF("No space to malloc !\n");
			return -2;
		}
		
		rt_memcpy(&(handler->node[index].data[0]), &data_unit->type, 1);
		rt_memcpy(&(handler->node[index].data[1]), &data_unit->num, 1);
		
		rt_memcpy(&(handler->node[index].data[2]), data_unit->data, data_unit->data_len);
		
		
		UITD_HEX_DUMP("Data unit : ", handler->node[index].data, handler->node[index].ctrl.data_len);
		for (i=0;i<handler->node[index].ctrl.data_len;i++)
		{
			checksum += handler->node[index].data[i];
		}
	}
	else
	{
		handler->node[index].data = NULL;
	}
	
	handler->node[index].checksum = checksum%256;
	
	handler->node[index].socket = handler->socket;
	handler->node[index].retry = 0;
	handler->node[index].time = 0;
	handler->node[index].ack_status = cmd_acked_none;
	handler->node[index].send_retry = pkt->send_retry;
	handler->node[index].send_fail = pkt->send_fail;
	handler->node[index].send_success = pkt->send_success;
	handler->node[index].send_acked = pkt->send_acked;
	handler->node[index].ack_deal = pkt->ack_deal;
	
	handler->node[index].node_status = GB_NODE_sending;
	return index;
}

int UITD_send_ack(t_server_handler *handler,
                    t_GB_pkt *pkt, 
					t_GB_data_unit *data_unit)
{

	return UITD_send_data(handler, pkt, data_unit, 0);
}

int UITD_send_actively(t_server_handler *handler, 
                        t_GB_pkt *pkt, 
                        t_GB_data_unit *data_unit)
{

	return UITD_send_data(handler, pkt, data_unit, 1);
}

int UITD_send_no_ack(t_server_handler *handler, 
                        t_GB_pkt *pkt, 
                        t_GB_data_unit *data_unit)
{

	return UITD_send_data(handler, pkt, data_unit, 0);
}


int UITD_ack_88_28_U_UITD_TIME(t_server_handler *handler, t_GB_pkt *pkt)
{
	t_GB_ctrl_timestamp TS = {0};
	
	UITD_get_time(&TS);
	pkt->data_unit.type = GB_TYPE_28_U_UITD_TIME;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &TS;
	pkt->data_unit.data_len = sizeof(t_GB_ctrl_timestamp);
	
	UITD_HEX_DUMP("TS: ", pkt->data_unit.data, pkt->data_unit.data_len);
	UITD_ack(handler, pkt, &pkt->data_unit);
}


int UITD_send_2_U_FIRE_DEV_STATUS(t_server_handler *handler, t_GB_pkt *pkt, uint8_t	num)
{
	int i = 0;
	t_GB_dev_status_data *p_dev_status = NULL;
	
	if ((num == 0)|| (num >= GB_DEV_STATUS_DATA_MAX))
	{
		UITD_PRINTF("Send fire device status parameter error ! num: %d \n", num);
		return -1;
	}

	p_dev_status = (t_GB_dev_status_data *)pkt->buf;
	UITD_PRINTF(" num: %d , p_dev_status: 0x%08X\n", num, p_dev_status);
	
	for (i=0;i<num;i++)
	{
		UITD_get_time(&p_dev_status[i].timestamp);
	}
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_2_U_FIRE_DEV_STATUS;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = (uint8_t *)&p_dev_status[0];
	pkt->data_unit.data_len = sizeof(t_GB_dev_status_data) * num;
	
	UITD_HEX_DUMP("TS: ", pkt->data_unit.data, pkt->data_unit.data_len);
	UITD_send_actively(handler, pkt, &pkt->data_unit);
	return 0;
}


int UITD_trans_data_failed(void *data, int node)
{
    t_server_handler *handler = (t_server_handler *)data;
    
    handler->status = UITD_svc_status_disconnected;
    
    return 0;
}

int UITD_send_alive_pkt(t_server_handler *handler)
{
	int i = 0;
	t_GB_pkt pkt = {0};
    
	pkt.ctrl.cmd = GB_CMD_ALIVE;
	pkt.data_unit.type = GB_TYPE_128_ALIVE;
	pkt.data_unit.num	= 0;
	pkt.data_unit.data = NULL;
	pkt.data_unit.data_len = 0;
    
    pkt.send_fail = &UITD_trans_data_failed;
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}


int UITD_send_testing_data(t_server_handler *handler, uint32_t  data)
{
	int i = 0;
	t_GB_pkt pkt = {0};
    
	pkt.ctrl.cmd = GB_CMD_ALIVE;
	pkt.data_unit.type = GB_TYPE_128_ALIVE;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &data;
	pkt.data_unit.data_len = 4;
    
	UITD_send_no_ack(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_file_rec_file_data(t_server_handler *handler, uint8_t *data, uint32_t len)
{
    return handler->FW.file_write(handler->FW.fw_fd, data, len);
}

int UITD_file_req_pkt_failed(void *data, int node)
{
    t_server_handler *handler = (t_server_handler *)data;
    
    // Retry to req_pkt, not exit this circle.
    UITD_file_req_pkt(handler, handler->FW.data_req.index);
    //handler->FW.status = FW_failed;
    
    return 0;
}

int UITD_file_req_pkt(t_server_handler *handler, uint32_t index)
{
	int i = 0;
    t_GB_pkt pkt = {0};
    
    handler->FW.data_req.index = index;
    
	pkt.ctrl.cmd = JS_CMD_FILE_SERVICE;
	pkt.data_unit.type = GB_TYPE_130_U_REQ_FW_DATA;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &handler->FW.data_req;
	pkt.data_unit.data_len = sizeof(s_FW_DATA_req);
    
    pkt.send_fail = &UITD_file_req_pkt_failed;
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_file_req_info(t_server_handler *handler)
{
	int i = 0;
    s_FW_info_req fw_info = {0};
    t_GB_pkt pkt = {0};
    
    fw_info.fw_ver_main = sys_main_version;
    fw_info.fw_ver_user = sys_sub_version;
    
   
	pkt.ctrl.cmd = JS_CMD_FILE_SERVICE;
	pkt.data_unit.type = GB_TYPE_132_U_REQ_FW_INFO;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &fw_info;
	pkt.data_unit.data_len = sizeof(s_FW_info_req);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}


int UITD_sensor_upload_rough_data(t_server_handler *handler, uint8_t ID, uint8_t out_type, uint16_t data)
{
	int i = 0;
    s_sensor_rough_data rough_data = {0};
    t_GB_pkt pkt = {0};
    
    rough_data.ID = ID;
    rough_data.out_type = out_type;
    rough_data.data = data;
   
	pkt.ctrl.cmd = JS_CMD_SENSOR_SAMPLE;
	pkt.data_unit.type = JS_TYPE_140_U_SENSOR_DATA;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &rough_data;
	pkt.data_unit.data_len = sizeof(s_sensor_rough_data);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_sensor_ack_rough_data(t_server_handler *handler, uint8_t ID, uint8_t out_type, uint16_t data)
{
	int i = 0;
    s_sensor_rough_data rough_data = {0};
    t_GB_pkt pkt = {0};
    
    rough_data.ID = ID;
    rough_data.out_type = out_type;
    rough_data.data = data;
   
	pkt.ctrl.cmd = JS_CMD_SENSOR_SAMPLE;
	pkt.data_unit.type = JS_TYPE_142_U_SENSOR_DATA_ACK;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &rough_data;
	pkt.data_unit.data_len = sizeof(s_sensor_rough_data);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_sensor_cfg_check_ack(t_server_handler *handler, uint8_t ID, uint32_t period)
{
	int i = 0;
    s_sensor_cfg_report cfg = {0};
    t_GB_pkt pkt = {0};
    
    cfg.ID = ID;
    cfg.period = period;
   
	pkt.ctrl.cmd = JS_CMD_SENSOR_SAMPLE;
	pkt.data_unit.type = JS_TYPE_144_U_SENSOR_CFG_ACK;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &cfg;
	pkt.data_unit.data_len = sizeof(s_sensor_cfg_report);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_sensor_ID_check_ack(t_server_handler *handler, s_sensor_ID_report *ID, uint32_t num)
{
    t_GB_pkt pkt = {0};
   
	pkt.ctrl.cmd = JS_CMD_SENSOR_SAMPLE;
	pkt.data_unit.type = JS_TYPE_147_U_SENSOR_ID_ACK;
	pkt.data_unit.num	= num;
	pkt.data_unit.data = &ID;
	pkt.data_unit.data_len = sizeof(s_sensor_ID_report)*num;
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
	return 0;
}

int UITD_output_ctrl_check_ack(t_server_handler *handler, uint8_t ID, uint8_t action)
{
    s_output_state output_state = {0};
    t_GB_pkt pkt = {0};
   
    output_state.ID = ID;
    output_state.action = action;
    
	pkt.ctrl.cmd = JS_CMD_OUTPUT_CTRL;
	pkt.data_unit.type = JS_TYPE_153_U_OUTPUT_CHECK_ACK;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &output_state;
	pkt.data_unit.data_len = sizeof(s_output_state);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
    return 0;
}

int UITD_IO_state_report(t_server_handler *handler, uint8_t ID, e_IO_input_state state)
{
	int i = 0;
    s_IO_input_report IO_input_report = {0};
    t_GB_pkt pkt = {0};
    
    IO_input_report.ID = ID;
    IO_input_report.state = state;
  
	pkt.ctrl.cmd = JS_CMD_INPUT_REPORT;
	pkt.data_unit.type = JS_TYPE_160_U_INPUT_REPORT;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &IO_input_report;
	pkt.data_unit.data_len = sizeof(s_IO_input_report);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
    return 0;
}

int UITD_IO_state_ack(t_server_handler *handler, uint8_t ID, e_IO_input_state state)
{
	int i = 0;
    s_IO_input_report IO_input_report = {0};
    t_GB_pkt pkt = {0};
    
    IO_input_report.ID = ID;
    IO_input_report.state = state;
  
	pkt.ctrl.cmd = JS_CMD_INPUT_REPORT;
	pkt.data_unit.type = JS_TYPE_162_U_INPUT_STATE_ACK;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &IO_input_report;
	pkt.data_unit.data_len = sizeof(s_IO_input_report);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
    return 0;
}


int UITD_IO_state_cfg_ack(t_server_handler *handler, uint8_t ID, e_IO_input_trig trig, uint16_t period)
{
	int i = 0;
    s_IO_input_cfg_report IO_input_cfg_report = {0};
    t_GB_pkt pkt = {0};
    
    IO_input_cfg_report.ID = ID;
    IO_input_cfg_report.trig = trig;
    IO_input_cfg_report.period = period;
  
	pkt.ctrl.cmd = JS_CMD_INPUT_REPORT;
	pkt.data_unit.type = JS_TYPE_165_U_INPUT_CFG_ACK;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &IO_input_cfg_report;
	pkt.data_unit.data_len = sizeof(s_IO_input_cfg_report);
    
	UITD_send_actively(handler, &pkt, &pkt.data_unit);
    return 0;
}

int UITD_send_dev_status(t_server_handler *handler, e_fire_dev_status dev_status, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, e_GB_data_unit_status status)
{
	int i = 0;
	int if_final = 0;
	static t_GB_dev_status_data *p_dev_status = NULL;
	t_GB_pkt *pkt = &handler->GB_pkt_send;
//	memset(pkt->buf, 0x00, sizeof(pkt->buf));
//	p_dev_status[0] = (t_GB_dev_status_data *)pkt->buf;
	
	switch(status)
	{
		case data_unit_start:
			pkt->data_num = 0;

			memset(pkt->buf, 0x00, sizeof(pkt->buf));
			p_dev_status = (t_GB_dev_status_data *)pkt->buf;
			break;
		case data_unit_normal:
			break;
		case data_unit_final:
			if_final = 1;
			break;
		case data_unit_single:
			if_final = 1;
			pkt->data_num = 0;
		
			memset(pkt->buf, 0x00, sizeof(pkt->buf));
			p_dev_status = (t_GB_dev_status_data *)pkt->buf;
			
			break;
		default:
			break;
	}

	if (pkt->data_num >= GB_DEV_STATUS_DATA_MAX)
	{
		UITD_PRINTF("Data unit is too many !");
		return -1;
	}
	
	
	p_dev_status[pkt->data_num].sys_type = GB_SYS_TYPE_FIRE_ALARMER;
	p_dev_status[pkt->data_num].sys_addr = sys_addr;
	p_dev_status[pkt->data_num].type			= GB_DEV_TYPE_SOMKE_ALARMER;
	rt_memcpy(&p_dev_status[pkt->data_num].addr[0], &addr_sub, 2);
	rt_memcpy(&p_dev_status[pkt->data_num].addr[2], &addr_main, 2);
    
    switch (dev_status)
    {
        case fire_dev_status_smoke_alarm:
            p_dev_status[pkt->data_num].status.if_fire = 1;
            break;
        case fire_dev_status_smoke_fault:
            p_dev_status[pkt->data_num].status.if_fault = 1;
            break;
        case fire_dev_status_smoke_disable:
            p_dev_status[pkt->data_num].status.if_disable = 1;
            break;
        default:
            break;
    }
    
	p_dev_status[pkt->data_num].status.if_fire = 1;
	
	for (i=0;i<(sizeof(p_dev_status[pkt->data_num].remark)-1);i++)
	{
		p_dev_status[pkt->data_num].remark[i] = ' ';
	}

	pkt->data_num ++;
	
//	UITD_PRINTF("sys_addr : %d, data_unit_num : %d , p_dev_status : 0x%08X", 
//				p_dev_status[pkt->data_num].sys_addr,
//				pkt->data_num,
//				p_dev_status);
	
			
	if (if_final)
	{
		// Song: TODO: fetch the current socket.
		//pkt->socket = a_new_client_sock_fd;
		//rt_memcpy(pkt->ctrl.src.addr, g_UITD_addr, sizeof(pkt->ctrl.src));
		
		
		
		UITD_PRINTF("Sending the fire devices status ...\n");
		UITD_send_2_U_FIRE_DEV_STATUS(handler, pkt, pkt->data_num);
	}
	
	return 0;
}

int UITD_send_smoke_fire_alarm(t_server_handler *handler, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, fire_dev_status_smoke_alarm, sys_addr, addr_main, addr_sub, status);
}

int UITD_send_smoke_fire_fault(t_server_handler *handler, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, fire_dev_status_smoke_fault, sys_addr, addr_main, addr_sub, status);
}

int UITD_send_smoke_fire_disable(t_server_handler *handler, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, fire_dev_status_smoke_disable, sys_addr, addr_main, addr_sub, status);
}


int UITD_send_21_U_UITD_STATUS(t_server_handler *handler, t_GB_pkt *pkt, t_GB_UITD_status status)
{
	int i = 0;
	t_GB_UITD_status_data *p_UITD_status = NULL;
	

	p_UITD_status = (t_GB_UITD_status_data *)pkt->buf;
	UITD_PRINTF(" p_UITD_status: 0x%08X\n", p_UITD_status);
	
	p_UITD_status[0].status = status;
	UITD_get_time(&p_UITD_status[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_21_U_UITD_STATUS;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_UITD_status;
	pkt->data_unit.data_len = sizeof(t_GB_UITD_status_data);
	
	UITD_HEX_DUMP("TS: ", pkt->data_unit.data, pkt->data_unit.data_len);
	UITD_send_actively(handler, pkt, &pkt->data_unit);
	return 0;
}

int UITD_send_UITD_main_power_fault(t_server_handler *handler, t_GB_pkt *pkt)
{
	t_GB_UITD_status status = {0};
	
	status.if_main_power = 1;
	UITD_send_21_U_UITD_STATUS(handler, pkt, status);
}

int UITD_send_UITD_backup_power_fault(t_server_handler *handler, t_GB_pkt *pkt)
{
	t_GB_UITD_status status = {0};
	
	status.if_backup_power = 1;
	UITD_send_21_U_UITD_STATUS(handler, pkt, status);
}

int UITD_send_inital_status_ack_deal(char *data,int len, uint8_t if_affirm)
{
    t_server_handler *handler = NULL;
    
    handler = (t_server_handler *)data;
    handler->status = UITD_svc_status_alive;
    
    return 0;
}

int UITD_send_UITD_inital_status(t_server_handler *handler)
{
    t_GB_pkt pkt = {0};
	t_GB_UITD_status status = {0};
    
    pkt.ctrl.SN = 0x0000;
	pkt.ack_deal = &UITD_send_inital_status_ack_deal;
    pkt.send_fail = &UITD_trans_data_failed;
    
	status.if_working = 1;
	UITD_send_21_U_UITD_STATUS(handler, &pkt, status);
}

int UITD_deal(t_server_handler *handler, t_GB_pkt *pkt)
{
    int i = 0;
	int res = 0;
	int node = 0;
    //s_FW_DATA *p_data_rec = NULL;
	s_FW_DATA file_data_rec = {0};
    
	switch (pkt->ctrl.cmd)
	{
		case GB_CMD_CTRL:
			switch (pkt->data_type)
			{
				case GB_TYPE_89_C_UITD_INIT:
					UITD_PRINTF("C_UITD_INIT :%d\n", pkt->data_type);
					// Song: testing
					////UITD_send_UITD_main_power_fault(&GB_pkt_send);
					
					////UITD_send_smoke_fire_alarm(&GB_pkt_send, 2, 2, 2, data_unit_single);
					UITD_send_pkt_init(&handler->GB_pkt_send);
					handler->GB_pkt_send.ack_deal = UITD_cb_ack_deal_somke_alarm;
					handler->GB_pkt_send.send_fail = UITD_cb_send_fail_somke_alarm;
					UITD_send_smoke_fire_alarm(handler, 2, 2, 2, data_unit_start);
					UITD_send_smoke_fire_alarm(handler, 4, 4, 4, data_unit_normal);
					UITD_send_smoke_fire_alarm(handler, 8, 8, 8, data_unit_normal);
					UITD_send_smoke_fire_alarm(handler, 16, 16, 16, data_unit_final);
				
					break;
				case GB_TYPE_90_C_UITD_SYNC:
					// update the system time 
					UITD_PRINTF("C_UITD_SYNC :%d\n", pkt->data_type);
					res = UITD_affirm(handler, pkt);
					UITD_PRINTF("UITD Node : %d\n", res);
				
				break;
				case GB_TYPE_91_C_UITD_CHECK:
					UITD_PRINTF("C_UITD_CHECK :%d\n", pkt->data_type);
					res = UITD_affirm(handler, pkt);
					UITD_PRINTF("UITD Node : %d\n", res);
					
					break;
				default:
					break;
			
			}
			break;
		case GB_CMD_SEND_DATA:
			
			break;
		case GB_CMD_AFFIRM:
			UITD_PRINTF("Received a affirm packet, SN:%d\n", pkt->ctrl.SN);
			node = UITD_server_search_node( handler, pkt->ctrl.SN);
			if (node >= 0)
			{
				handler->node[node].ack_status = cmd_acked_affirm;
				UITD_PRINTF("Searched the match node\n");
			}
			break;
		case GB_CMD_REQUIRE:
			switch (pkt->data_type)
			{
				case GB_TYPE_81_R_UITD_STATUS:
					
					break;
				case GB_TYPE_86_R_UITD_CONFIG:
					
					break;
				case GB_TYPE_88_R_UITD_TIME:
					UITD_ack_88_28_U_UITD_TIME(handler, pkt);
					break;
				default:
					break;
			
			}
			
			break;
		case GB_CMD_ACK:
			
			break;
		case GB_CMD_DENY:
			node = UITD_server_search_node( handler, pkt->ctrl.SN);
			if (node >= 0)
			{
				handler->node[node].ack_status = cmd_acked_deny;
			}
			break;
        case JS_CMD_FILE_SERVICE:
            switch (pkt->data_type)
            {
                case GB_TYPE_129_D_PUSH_FW_INFO:
                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    UITD_PRINTF("Server pushed a new firmware\n");
                    UITD_PRINTF("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user);
                    UITD_PRINTF("Support device: 0X%08x\n", *(uint32_t *)&handler->FW.FW.support_dev);
                    UITD_PRINTF("FW name: %s\n", handler->FW.FW.name);
                    UITD_PRINTF("FW len: %d\n", handler->FW.FW.len);
                    handler->FW.push_FW_flag = 1;
                    handler->FW.status = FW_idle;
                    UITD_affirm(handler, pkt);
                    break;
                case GB_TYPE_136_D_PUSH_FORCE_FW_INFO:
                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    UITD_PRINTF("Server pushed a force firmware\n");
                    UITD_PRINTF("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user);
                    UITD_PRINTF("Support device: 0X%08x\n", *(uint32_t *)&handler->FW.FW.support_dev);
                    UITD_PRINTF("FW name: %s\n", handler->FW.FW.name);
                    UITD_PRINTF("FW len: %d\n", handler->FW.FW.len);
                    handler->FW.push_FW_flag = 1;
                    handler->FW.status = FW_idle;
                    UITD_affirm(handler, pkt);
                case GB_TYPE_131_D_ACK_FW_DATA:
                    node = UITD_server_search_node( handler, pkt->ctrl.SN);
                    if (node >= 0)
                    {
                        handler->node[node].ack_status = cmd_acked_affirm;
                        UITD_PRINTF("Searched the match node\n");
                    }

                
                    switch (handler->FW.status)
                    {
                        case FW_idle:
                            break;
                        case FW_init:
                            break;
                        case FW_fetch:
                            {
                                rt_memcpy(&handler->FW.data_rec, pkt->buf+2, sizeof(s_FW_DATA));
                                if (handler->FW.data_req.index == handler->FW.data_rec.index)
                                {
                                    UITD_PRINTF("Receive file data, SN: %d, Len: %d\n", handler->FW.data_req.index, handler->FW.data_rec.len);
                                    rt_memcpy(handler->FW.data, pkt->buf+2+sizeof(s_FW_DATA), handler->FW.data_rec.len);
                                    // Song: TODO,Check the CRC16 value.
                                    ////if (handler->FW.data_rec.CRC16 == CRC16(handler->FW.data, handler->FW.data_rec.len))
                                    if (1)
                                    {
                                        res = handler->FW.file_write(handler->FW.fw_fd, handler->FW.data, handler->FW.data_rec.len);
                                        if (res == handler->FW.data_rec.len)
                                        {
                                            UITD_PRINTF("Writed file data, SN: %d\n", handler->FW.data_req.index);
                                            if (handler->FW.data_req.index < (handler->FW.req_pkt_num-1))
                                            {
                                                
                                                handler->FW.data_req.index++;
                                                UITD_file_req_pkt(handler, handler->FW.data_req.index);
                                            }
                                            else
                                            {
                                                
                                                handler->FW.status = FW_done;
                                            }
                                            
                                            
                                        }
                                        else // Write file data failed.
                                        {
                                            handler->FW.status = FW_failed;
                                        }
                                    }
                                    else
                                    {
                                        UITD_PRINTF("CRC16 verify is wrong ! file receive failed ! \n");
                                        handler->FW.status = FW_failed;
                                    }
                                }
                            
                            }
                            break;
                        case FW_done:
                            break;
                        default:
                            break;
                    }
                    break;
                case GB_TYPE_133_D_ACK_FW_INFO:
                    
                    node = UITD_server_search_node( handler, pkt->ctrl.SN);
                    if (node >= 0)
                    {
                        handler->node[node].ack_status = cmd_acked_affirm;
                        UITD_PRINTF("Searched the match node\n");
                    }

                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    UITD_PRINTF("Server return new firmware information\n");
                    UITD_PRINTF("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user);
                    UITD_PRINTF("FW name: %s\n", handler->FW.FW.name);
                    UITD_PRINTF("FW len: %d\n", handler->FW.FW.len);
                    handler->FW.return_FW_flag = 1;
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_SENSOR_SAMPLE:
            switch (pkt->data_type)
            {
                case JS_TYPE_141_D_SENSOR_DATA_CHECK:
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            if (p_sensor_cb->sensor[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_sensor_ack_rough_data(p_sensor_cb->handler, 
                                                            p_sensor_cb->sensor[i].ID, 
                                                            p_sensor_cb->sensor[i].data_rough, 
                                                            p_sensor_cb->sensor[i].out_type);
                                break;
                            }
                        }
                        if (i == p_sensor_cb->sensor_num) // ID is not exist
                        {
                            UITD_sensor_ack_rough_data(p_sensor_cb->handler, 
                                                            0, 
                                                            0x0000, 
                                                            0);
                        }
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                case JS_TYPE_143_D_SENSOR_CFG_CHECK:
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            if (p_sensor_cb->sensor[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_sensor_cfg_check_ack(p_sensor_cb->handler, 
                                                            p_sensor_cb->sensor[i].ID, 
                                                            p_sensor_cb->sensor[i].period);
                                break;
                            }
                        }
                        if (i == p_sensor_cb->sensor_num) // ID is not exist
                        {
                            UITD_sensor_cfg_check_ack(p_sensor_cb->handler, 
                                                            0, 
                                                            0);
                        }
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                case JS_TYPE_145_D_SENSOR_CFG_SET:
                    rt_memcpy(&sensor_cfg_temp, pkt->buf+2, sizeof(s_sensor_cfg_report));
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            if (p_sensor_cb->sensor[i].ID == sensor_cfg_temp.ID)
                            {
                                p_sensor_cb->sensor[i].period = sensor_cfg_temp.period;
                                UITD_affirm(handler, pkt);
                                break;
                            }
                        }
                        if (i == p_sensor_cb->sensor_num) // ID is not exist
                        {
                            UITD_deny(handler, pkt);
                        }
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                case JS_TYPE_146_D_SENSOR_ID_CHECK:
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            sensor_ID_temp[i].ID = p_sensor_cb->sensor[i].ID;
                            sensor_ID_temp[i].out_type = p_sensor_cb->sensor[i].out_type;
                        }
                        
                        UITD_sensor_ID_check_ack(p_sensor_cb->handler, &sensor_ID_temp[0], p_sensor_cb->sensor_num);
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_OUTPUT_CTRL:
            switch (pkt->data_type)
            {
                case JS_TYPE_150_D_OUTPUT_CTRL:
                    rt_memcpy(&handler->output.data , pkt->buf+2, sizeof(s_output_data));
                    UITD_PRINTF("Server pushed a output control CMD \n");
                    UITD_PRINTF("Output ID: %d, action: %d, delay: %d\n", 
                                handler->output.data.ID, 
                                handler->output.data.action, 
                                handler->output.data.delay);
                    handler->output.valid = 1;
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_151_D_OUTPUT_SEQ_ACTION:
                    
                    break;
                case JS_TYPE_152_D_OUTPUT_CHECK:
                    rt_mutex_take(&mt_output, 10);
                    if (p_output_ctrl_cb)
                    {
                        for (i=0;i<p_output_ctrl_cb->output_num;i++)
                        {
                            if (p_output_ctrl_cb->output[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_output_ctrl_check_ack(p_output_ctrl_cb->handler, 
                                                            p_output_ctrl_cb->output[i].ID,
                                                            p_output_ctrl_cb->output[i].action);
                                break;
                            }
                        }
                        if (i == p_output_ctrl_cb->output_num)
                        {
                            UITD_output_ctrl_check_ack(p_output_ctrl_cb->handler, 0, 0);
                        }
                    }
                    rt_mutex_release(&mt_output);
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_INPUT_REPORT:
            switch (pkt->data_type)
            {
                case JS_TYPE_161_R_INPUT_STATE_CHECK:
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        for (i=0;i<p_IO_input_cb->IO_num;i++)
                        {
                            if (p_IO_input_cb->state[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_IO_state_ack(p_IO_input_cb->handler, p_IO_input_cb->state[i].ID, p_IO_input_cb->state[i].state);
                            }
                        }
                    }
                    rt_mutex_release(&mt_IO_input);
                    break;
                case JS_TYPE_163_D_INPUT_CFG_SET:
                    rt_memcpy(&IO_input_cfg_temp , pkt->buf+2, sizeof(s_IO_input_cfg)); 
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        for (i=0;i<p_IO_input_cb->IO_num;i++)
                        {
                            if (p_IO_input_cb->state[i].ID == IO_input_cfg_temp.ID)
                            {
                                p_IO_input_cb->state[i].state_trig = IO_input_cfg_temp.trig;
                                p_IO_input_cb->state[i].period = IO_input_cfg_temp.period;
                            }
                        }
                    }
                    rt_mutex_release(&mt_IO_input);
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_164_R_INPUT_CFG_CHECK:
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        for (i=0;i<p_IO_input_cb->IO_num;i++)
                        {
                            if (p_IO_input_cb->state[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_IO_state_cfg_ack(p_IO_input_cb->handler, 
                                                        p_IO_input_cb->state[i].ID, 
                                                        p_IO_input_cb->state[i].state_trig, 
                                                        p_IO_input_cb->state[i].period);
                                
                            }
                        }
                    }
                    rt_mutex_release(&mt_IO_input);
                    break;
                default:
                    break;
			}
            break;
        default:
			UITD_PRINTF("Unsupported CMD : %d\n" , pkt->ctrl.cmd);
			break;
	}	
}

int UITD_send_pkg(t_server_handler *handler, int node)
{
	int res = 0;
//	int data_len = 0;
	handler->data_len = 0;
	
	rt_memcpy(&handler->send_buf[handler->data_len], GB_PKT_HEAD, strlen(GB_PKT_HEAD));
	handler->data_len += strlen(GB_PKT_HEAD);
	
	rt_memcpy(&handler->send_buf[handler->data_len], &handler->node[node].ctrl, sizeof(t_GB_ctrl_unit));
	handler->data_len += sizeof(t_GB_ctrl_unit);

	rt_memcpy(&handler->send_buf[handler->data_len], handler->node[node].data, handler->node[node].ctrl.data_len);
	handler->data_len += handler->node[node].ctrl.data_len;
	
	rt_memcpy(&handler->send_buf[handler->data_len], &handler->node[node].checksum, GB_CHECKSUM_LEN);
	handler->data_len += GB_CHECKSUM_LEN;

	rt_memcpy(&handler->send_buf[handler->data_len], GB_PKT_TAIL, strlen(GB_PKT_TAIL));
	handler->data_len += strlen(GB_PKT_TAIL);
	
	UITD_HEX_DUMP("pkg_send_buf ", handler->send_buf, handler->data_len);
	res = handler->send_data(handler->node[node].socket, handler->send_buf, handler->data_len);
	if (res != handler->data_len)
	{
		UITD_PRINTF("Net send data failed : %d\n", res);
		return -2;
	}
	
	return 0;
}



int UITD_send_handler(t_server_handler *handler)
{
	int i = 0;
	int res = 0;
	
	for (i=0;i<GB_SERVER_NODE_MAX;i++)
	{
		if (handler->node[i].node_status == GB_NODE_unused)
		{
			continue;
		}
		else if (handler->node[i].node_status == GB_NODE_sending)
		{
			if (handler->node[i].if_active_send == 0)
			{
				res = UITD_send_pkg(handler, i);
				if (res == 0)
				{
					handler->node[i].node_status = GB_NODE_send_success;
					UITD_PRINTF("Send pkg success.\n");
				}
				else
				{
					handler->node[i].node_status = GB_NODE_send_fail;
					UITD_PRINTF("Send pkg failed.\n");
				}
				
			}
			else  
			{
				// Song: if receive ack, set this node status as unused.
				if (((handler->node[i].retry <= SERVER_RETRY_MAX) && (handler->node[i].retry > 0)) 
					&& ((handler->node[i].ack_status == cmd_acked_affirm) || (handler->node[i].ack_status == cmd_acked_deny)))
				{
					if (handler->node[i].ack_status == cmd_acked_affirm)
					{
						UITD_PRINTF("Receive affirm packet\n");
						if (handler->node[i].ack_deal != NULL)
						{
							handler->node[i].ack_deal((void *)handler, 0, 1);
						}
					}
					else if (handler->node[i].ack_status == cmd_acked_deny)
					{
						UITD_PRINTF("Receive affirm deny\n");
						if (handler->node[i].ack_deal != NULL)
						{
							handler->node[i].ack_deal((void *)handler, 0, 0);
						}
					}
					
					handler->node[i].node_status = GB_NODE_send_acked;
					goto END;
				}
				
				// Song: send packet on the start and every SERVER_TIMEOUT_MAX system ticks.
				if ((handler->node[i].time > SERVER_TIMEOUT_MAX) || (handler->node[i].time == 0))
				{
					handler->node[i].time = 0;
					handler->node[i].retry ++;
					// Song: retry too many times, send failed.
					if (handler->node[i].retry > SERVER_RETRY_MAX)  
					{
						UITD_PRINTF("Send failed !\n");
						handler->node[i].node_status = GB_NODE_send_fail;
						//continue;
					}
					else
					{
						UITD_PRINTF("UITD pkg sending\n");
						res = UITD_send_pkg(handler, i);
						if (res == 0)
						{
							//handler->node[i].node_status = GB_NODE_send_success;
							
							// Song: set the ack_status to waiting state. start waiting the ack packet.
							UITD_PRINTF("Send pkg success. %d , waiting ack packet\n", handler->node[i].retry);
							handler->node[i].ack_status = cmd_acked_waiting;
							if (handler->node[i].send_success != NULL)
							{
								handler->node[i].send_success(handler, i);
							}
						}
						else
						{
								if (handler->node[i].send_retry != NULL)
								{
									handler->node[i].send_retry(handler, i, handler->node[i].retry);
								}
								
								// Song : send failed too many times, 
								if (handler->node[i].retry == SERVER_RETRY_MAX)
								{
									UITD_PRINTF("Send failed too many times, exit !\n");
									handler->node[i].node_status = GB_NODE_send_fail;
								}
						}
					}
				}
				handler->node[i].time ++;
			}
		}
		
//		if (handler->node[i].node_status == GB_NODE_send_success)
//		{
//			if (handler->node[i].send_success != NULL)
//			{
//				handler->node[i].send_success(NULL, i);
//			}
//						
//			if (handler->node[i].data != NULL)
//			{
//				free(handler->node[i].data);
//			}
//			handler->node[i].node_status = GB_NODE_unused;
//			UITD_PRINTF("UITD pkt send success");
//		}
		
		END:
		if (handler->node[i].node_status == GB_NODE_send_fail)
		{
			if (handler->node[i].send_fail != NULL)
			{
				handler->node[i].send_fail(handler, i);
			}
						
			if (handler->node[i].data != NULL)
			{
				free(handler->node[i].data);
			}
			UITD_PRINTF("UITD pkt send failed\n");
			handler->node[i].node_status = GB_NODE_unused;
		}
		else if (handler->node[i].node_status == GB_NODE_send_success)
		{
			if (handler->node[i].send_success != NULL)
			{
				handler->node[i].send_success(handler, i);
			}
			
			if (handler->node[i].data != NULL)
			{
				free(handler->node[i].data);
			}
			
			UITD_PRINTF("UITD pkt send success\n");
			handler->node[i].node_status = GB_NODE_unused;
		}
		else if (handler->node[i].node_status == GB_NODE_send_acked)
		{
			if (handler->node[i].data != NULL)
			{
				free(handler->node[i].data);
			}
			
			UITD_PRINTF("UITD pkt received ack packet.\n");
			handler->node[i].node_status = GB_NODE_unused;
		}
		else
		{
			// unkown status, error report.
			////UITD_PRINTF("Unkown node status !");
			//return -5;
		}
	}
}


int UITD_service_init(t_server_handler *handler)
{
    uint8_t *p = NULL;
    
    handler->status = UITD_svc_status_idle;
    handler->alive_cnt = 0;
    
    UITD_send_node_init(handler);
    
//    handler->FW.push_FW_flag = 0;
//    handler->FW.return_FW_flag = 0;
//    handler->FW.status = FW_idle;
    
    p = rt_malloc(FW_PKT_LEN_DEFAULT);
    if (p == NULL) return -1;
    
    handler->FW.data = p;
    
    return 0;
}



int UITD_dl_file_success(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;

    UITD_PRINTF("File download success !\n");
    return 0;
}


int UITD_dl_file_fail(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;
    
    UITD_PRINTF("File download failed !!!\n");
    return 0;
}


int UITD_dl_firmware_success(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;

    UITD_PRINTF("Firmware upgrade success, ready to upgrade the firmware, waiting reboot.\n");
    touch(FW_UPGRADE_FLAG);
    sys_fw_update_reboot = 1;
    
    return 0;
}


int UITD_dl_firmware_fail(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;
    
    UITD_PRINTF("Firmware upgrade retry ! \n");
    UITD_download_firmware(fw, fw->file_down_success, fw->file_down_fail);
    
    return 0;
}


int UITD_download_file( s_FW_update *fw,
                        uint8_t *filename, 
                        pFun_file_down_success file_success, 
                        pFun_file_down_fail file_fail)
{

    sprintf(fw->fw_path, "%s/%s", DOWN_FILE_DIR, filename);
    
    fw->file_down_success = file_success;
    fw->file_down_fail = file_fail;
    
    strcpy(fw->data_req.name, filename);
    
    fw->status = FW_init;
    
    return 0;
}    


int UITD_download_firmware(s_FW_update *fw,
                        uint8_t *filename,
                        pFun_file_down_success file_success, 
                        pFun_file_down_fail file_fail)
{
    
    strcpy(fw->fw_path, FW_FILE_PATH);
    
    strcpy(fw->data_req.name, filename);
    
    fw->file_down_success = file_success;
    fw->file_down_fail = file_fail;
    
    fw->status = FW_init;
    
    return 0;
}    

int UITD_firmware_upgrade(t_server_handler *handler)
{
    
    return UITD_file_req_info(handler);
    //return 0;
}


int UITD_firmware_parse(uint8_t *fw_name, uint8_t main_ver, uint8_t sub_ver, s_FW_support_dev *dev_type)
{
    // Song: TODO, parse the firmware information, check if need to be upgraded.
    
    
    
    
    
    return 1;
}


int UITD_service_handler(t_server_handler *handler)
{
    int i = 0;
    int res = 0;
    t_GB_pkt temp_pkt = {0};

    
    {
        switch (handler->status)
        {
            case UITD_svc_status_idle:
                handler->alive_cnt = 0;
                
                #ifdef UITD_SERVICE_TESTING
                handler->status = UITD_svc_status_testing;
                #else
            
                UITD_send_UITD_inital_status(handler);
                handler->status = UITD_svc_status_connect;
                #endif // UITD_SERVICE_TESTING
                break;
            case UITD_svc_status_connect:
            
                break;
            case UITD_svc_status_alive:
                if (handler->alive_cnt > SERVER_ALIVE_PERIOD)
                {
                    UITD_send_alive_pkt(handler);
                    handler->alive_cnt = 0;
                }
                
    //            // Song: Just for testing.
    //            if (handler->alive_cnt  == 200)
    //            {
    //                UITD_send_smoke_fire_alarm(handler, 0, 1, 2, data_unit_single);
    //            }
    //            if (handler->alive_cnt == 400)
    //            {
    //                UITD_send_smoke_fire_fault(handler, 3, 4, 5, data_unit_single);
    //            }
    //            if (handler->alive_cnt == 600)
    //            {
    //                UITD_send_smoke_fire_disable(handler, 6, 7, 8, data_unit_single);
    //            }
                handler->alive_cnt ++;
                break;
            case UITD_svc_status_testing:
                #ifdef UITD_SERVICE_TESTING
                if (handler->alive_cnt > RT_TICK_PER_SECOND*5)
                {
                    //UITD_send_alive_pkt(handler);
                    UITD_send_testing_data(handler, 0x87654321);
                    handler->alive_cnt = 0;
                }
                handler->alive_cnt ++;
                #endif // UITD_SERVICE_TESTING
                break;
            case UITD_svc_status_disconnected:
                handler->status = UITD_svc_status_idle;
            
                rt_kprintf("UITD_server socket %d have been disconnected !\n", handler->socket);
                UITD_disconnect(handler);
//                if (handler->FW.status != FW_idle)
//                {
//                    handler->FW.status = FW_stop;
//                }
                //handler->if_connected = 0;
                break;
            case UITD_svc_status_stoped:
                break;
            default:
                break;
        }

        switch (handler->FW.status)
        {
            case FW_idle:
//                if (handler->FW.push_FW_flag)
//                {
//                    handler->FW.push_FW_flag = 0;
//                    handler->FW.status = FW_init;
//                }
                break;
            case FW_init:
                handler->FW.fw_fd = handler->FW.file_open(handler->FW.fw_path);
                if (handler->FW.fw_fd < 0)
                {
                    handler->FW.status = FW_idle;
                    break;
                }
                
                UITD_PRINTF("Firmware file has been created.\n");
                
                handler->FW.rec_index = 0;
                handler->FW.rec_pkt_num = 0;
                handler->FW.rec_pkt_len = 0;
                handler->FW.req_index = 0;
                handler->FW.req_pkt_len = FW_PKT_LEN_DEFAULT;
                handler->FW.req_pkt_num = (handler->FW.FW.len % handler->FW.req_pkt_len)? 
                                            (handler->FW.FW.len / handler->FW.req_pkt_len + 1) : 
                                            (handler->FW.FW.len / handler->FW.req_pkt_len);
                
                //strcpy(handler->FW.data_req.name, handler->FW.FW.name);
                handler->FW.data_req.len = handler->FW.req_pkt_len;
                handler->FW.data_req.index = 0;
                
                handler->FW.status = FW_fetch;
                
                UITD_file_req_pkt(handler, handler->FW.data_req.index);
                break;
            case FW_fetch:
                
                
                break;
            case FW_done:
                handler->FW.file_close(handler->FW.fw_fd);
                handler->FW.status = FW_idle;
            
                if (handler->FW.file_down_success)
                {
                    handler->FW.file_down_success(&handler->FW);
                }
                ////handler->FW.FW_upgrade_success = 1;   //////
                //sys_fw_update_reboot = 1;
                break;
            case FW_failed: // if firmware trans failed, close file, remove file, and return to idle.
                handler->FW.file_close(handler->FW.fw_fd);
                rm(handler->FW.fw_path);
                handler->FW.status = FW_idle;
            
                UITD_PRINTF("Firmware upgrade failed !!!!!!!!!\n");
                if (handler->FW.file_down_fail)
                {
                    handler->FW.file_down_fail(&handler->FW);
                }
                ////handler->FW.FW_upgrade_failed = 1;  //////
                break;
            case FW_stop:
                handler->FW.file_close(handler->FW.fw_fd);
                rm(handler->FW.fw_path);
                UITD_PRINTF("Firmware upgrade stoped !!!!!!!!!\n");
                handler->FW.status = FW_stoped;
                break;
            case FW_stoped:
                break;
            default:
                break;
        }
    }
    
    
    if (handler->FW.push_FW_flag)
    {
        handler->FW.push_FW_flag = 0;
        
        res = UITD_firmware_parse(handler->FW.FW.name, sys_main_version, sys_sub_version, &handler->dev_type);
        if (res == 1)
        {
            UITD_download_firmware( &handler->FW,
                                handler->FW.FW.name,
                                &UITD_dl_firmware_success, 
                                &UITD_dl_firmware_fail);
            
        }
        else
        {
            rt_kprintf("Firmware is not the newest, no need upgrade \n");
        }
        
    }
    
    if (handler->FW.return_FW_flag)
    {
        handler->FW.return_FW_flag = 0;
        
        res = UITD_firmware_parse(handler->FW.FW.name, sys_main_version, sys_sub_version, &handler->dev_type);
        if (res == 1)
        {
            UITD_download_firmware( &handler->FW,
                                handler->FW.FW.name,
                                &UITD_dl_firmware_success, 
                                &UITD_dl_firmware_fail);
            
        }
        else
        {
            rt_kprintf("Firmware is not the newest, no need upgrade \n");
        }
    }
    
   
    if (p_sensor_cb)
    {
        res = rt_mq_recv(&mq_sensor_send, &sensor_data_temp, sizeof(sensor_data_temp), 0);
        if (res == RT_EOK)
        {
            UITD_sensor_upload_rough_data(handler, sensor_data_temp.ID, sensor_data_temp.out_type, sensor_data_temp.data);
        }
        
        if (handler->output.valid)
        {
            handler->output.valid = 0;
            
            rt_mq_send(&mq_output_ctrl, &handler->output.data, sizeof(handler->output.data));
        }
    }
    
    if (p_com_bus_cb)
    {
        res = rt_mq_recv(&mq_FA_fire, &FA_fire_temp, sizeof(s_com_bus_R_alarm), 0);
        if (res == RT_EOK)
        {
            UITD_send_smoke_fire_alarm(handler, FA_fire_temp.sys_addr, FA_fire_temp.addr_main, FA_fire_temp.addr_sub, data_unit_single);
        }
        
        res = rt_mq_recv(&mq_FA_fault, &FA_fault_temp, sizeof(s_com_bus_R_alarm), 0);
        if (res == RT_EOK)
        {
            UITD_send_smoke_fire_fault(handler, FA_fire_temp.sys_addr, FA_fire_temp.addr_main, FA_fire_temp.addr_sub, data_unit_single);
        }
    
    }
    
    if (p_IO_input_cb)
    {
        res = rt_mq_recv(&mq_IO_input, &IO_input_temp, sizeof(s_IO_input_report), 0);
        if (res == RT_EOK)
        {
            UITD_IO_state_report(p_IO_input_cb->handler, IO_input_temp.ID, IO_input_temp.state);
        }
        
    }
    
    
//    if (handler->FW.FW_upgrade_failed)
//    {
//        handler->FW.FW_upgrade_failed = 0;
//        
//        handler->FW.push_FW_flag = 1;
//        handler->FW.status = FW_idle;
//        UITD_PRINTF("Firmware upgrade retry ! \n");
//    }
//    
//    if (handler->FW.FW_upgrade_success)
//    {
//        handler->FW.FW_upgrade_success = 0;
//        UITD_PRINTF("Firmware upgrade success, ready to upgrade the firmware, waiting reboot.\n");
//        touch(FW_UPGRADE_FLAG);
//        sys_fw_update_reboot = 1;
//    }
    
    return 0;
}

e_GB_err UITD_server(t_server_handler *handler)
{
	e_GB_err GB_err = GB_E_OK;
	int res = 0;
	
	////UITD_PRINTF("Enter UITD_server ");
//	ring_buf = RingBuffer_create(GB_ROUGH_BUF_MAX);
//	if (ring_buf == NULL)
//	{
//		UITD_PRINTF("Failed to make ringbuffer.");
//		return ;
//	}
//	else
//	{
//		UITD_PRINTF("Successed to make ringbuffer !");
//	}
//	
//	res = RingBuffer_write(ring_buf, data, data_len);
//	if (res != data_len)
//	{
//		UITD_PRINTF("Failed to write ringbuffer.");
//	}

//	res = RingBuffer_read(ring_buf, data_buf, data_len);
//	if (res != data_len)
//	{
//		UITD_PRINTF("Failed to read ringbuffer.");
//	}
//	else
//	{
//		
//	}
//	
//	RingBuffer_destroy(ring_buf);
//	UITD_PRINTF("Successed to destroy ringbuffer !");

	////UITD_PRINTF("Ringbuf data number 01 : %d ", RingBuffer_available_data(ring_buf));

//	res = rt_ringbuffer_put(&handler->ring_buf, data, data_len);
//	if (res != data_len)
//	{
//		UITD_PRINTF("ringbuf write error : %d ", res);
//		return GB_E_BUF_WR;
//	}
	
	////UITD_PRINTF("Ringbuf data number 02 : %d ", RingBuffer_available_data(ring_buf));
	
	
	while(rt_ringbuffer_data_len(&handler->ring_buf) != 0)
	{
		////UITD_PRINTF("*");
		res = UITD_parse(handler, &handler->GB_parse);
		if (res == 0) // Parsed a valid packet.
		{
			////UITD_PRINTF("$");
			////UITD_PRINTF("UITD_parse valid : %d \n", GB_parse.len);
			
			GB_err = UITD_decode(handler, &handler->GB_pkt_rec, handler->GB_parse.buf, handler->GB_parse.len, handler->socket);
			if (GB_err == GB_E_OK)
			{
				UITD_deal(handler, &handler->GB_pkt_rec);

			}
			UITD_PRINTF("Error code: %d \n", GB_err);
			
			////TCP_Send_Data(a_new_client_sock_fd, GB_parse.buf, GB_parse.len, MSG_DONTWAIT);
			
			handler->GB_parse.valid = 0;
			handler->GB_parse.len = 0;
		}
		else
		{
			////UITD_PRINTF("#");
			continue;
		}
		
		
		
		

		
		
//		TCP_Send_Data(a_new_client_sock_fd, GB_parse.buf, GB_parse.len, MSG_DONTWAIT);
		////if (data_len > )
		
		////if ((data_len + GB_parse.head) > )
	
	}
	
	////UITD_PRINTF("Exit UITD_server ");
}


