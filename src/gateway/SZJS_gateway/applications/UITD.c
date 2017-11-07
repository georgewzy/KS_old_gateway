#define DEBUG_UITD

#ifdef DEBUG_UITD
#define UITD_PRINTF     rt_kprintf
#define UITD_HEX_DUMP   UITD_print_hex_dump
#else
#define UITD_PRINTF     
#define UITD_HEX_DUMP   
#endif // DEBUG_UITD

#include "UITD.h"
#include <rtdevice.h>
#include "GSM_mg301.h"
//#include <time.h>
#include "rtc.h"
#include <dfs_posix.h>

//#include "CRC16.h"
#include "sw_crc.h"
#include "sensor_sample.h"
#include "fire_alarm.h"
#include "pro_ctrl.h"


uint8_t	g_UITD_addr[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
uint64_t g_UITD_SN = 0;

s_file_trans_cb file_trans_cb = {0};

const s_dev_cmd_list device_cmd_list[] = {
    { // AP01
        DEV_TYPE_SAMPLE_CTRL, 
        12,
        {
            JS_TYPE_140_U_SENSOR_DATA,
            JS_TYPE_141_D_SENSOR_DATA_CHECK,
            JS_TYPE_142_U_SENSOR_DATA_ACK,
            JS_TYPE_143_D_SENSOR_CFG_CHECK,
            JS_TYPE_144_U_SENSOR_CFG_ACK,
            JS_TYPE_145_D_SENSOR_CFG_SET,
            JS_TYPE_146_D_SENSOR_ID_CHECK,
            JS_TYPE_147_U_SENSOR_ID_ACK,
            
            JS_TYPE_150_D_OUTPUT_CTRL,
            JS_TYPE_151_D_OUTPUT_SEQ_ACTION,
            JS_TYPE_152_D_OUTPUT_CHECK,
            JS_TYPE_153_U_OUTPUT_CHECK_ACK,
        }
    },
    { // AP02
        DEV_TYPE_SIMPLE_UITD, 
        6,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,
        }
    },
    { // AP03
        DEV_TYPE_CONTROLLER, 
        10,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,
            
            JS_TYPE_150_D_OUTPUT_CTRL,
            JS_TYPE_151_D_OUTPUT_SEQ_ACTION,
            JS_TYPE_152_D_OUTPUT_CHECK,
            JS_TYPE_153_U_OUTPUT_CHECK_ACK,
        }
    },    
    { // AP04
        DEV_TYPE_WIRE_EXTEND_BAT, 
        6,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,
        }
    },    
    { // full func
        DEV_TYPE_FULL_FUNC_BAT, 
        12,
        {
            JS_TYPE_140_U_SENSOR_DATA,
            JS_TYPE_141_D_SENSOR_DATA_CHECK,
            JS_TYPE_142_U_SENSOR_DATA_ACK,
            JS_TYPE_143_D_SENSOR_CFG_CHECK,
            JS_TYPE_144_U_SENSOR_CFG_ACK,
            JS_TYPE_145_D_SENSOR_CFG_SET,
            JS_TYPE_146_D_SENSOR_ID_CHECK,
            JS_TYPE_147_U_SENSOR_ID_ACK,
            
            JS_TYPE_150_D_OUTPUT_CTRL,
            JS_TYPE_151_D_OUTPUT_SEQ_ACTION,
            JS_TYPE_152_D_OUTPUT_CHECK,
            JS_TYPE_153_U_OUTPUT_CHECK_ACK,
        }
    },   
    { // 
        DEV_TYPE_UITD_3UART_BAT, 
        6,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,
        }
    },
    { // 
        DEV_TYPE_IOT_PRO_UITD, 
        6,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,
        }
    },
    { // 
        DEV_TYPE_IOT_PRO_CTRL, 
        18,
        {
            JS_TYPE_160_U_INPUT_REPORT,
            JS_TYPE_161_R_INPUT_STATE_CHECK,
            JS_TYPE_162_U_INPUT_STATE_ACK,
            JS_TYPE_163_D_INPUT_CFG_SET,
            JS_TYPE_164_R_INPUT_CFG_CHECK,
            JS_TYPE_165_U_INPUT_CFG_ACK,

            JS_TYPE_140_U_SENSOR_DATA,
            JS_TYPE_141_D_SENSOR_DATA_CHECK,
            JS_TYPE_142_U_SENSOR_DATA_ACK,
            JS_TYPE_143_D_SENSOR_CFG_CHECK,
            JS_TYPE_144_U_SENSOR_CFG_ACK,
            JS_TYPE_145_D_SENSOR_CFG_SET,
            JS_TYPE_146_D_SENSOR_ID_CHECK,
            JS_TYPE_147_U_SENSOR_ID_ACK,
            
            JS_TYPE_150_D_OUTPUT_CTRL,
            JS_TYPE_151_D_OUTPUT_SEQ_ACTION,
            JS_TYPE_152_D_OUTPUT_CHECK,
            JS_TYPE_153_U_OUTPUT_CHECK_ACK,
        }
    },    
};


static s_sensor_rough_data sensor_data_temp = {0};
static s_com_bus_R_alarm FA_fire_temp = {0};
static s_com_bus_R_alarm FA_elec_fire_temp = {0}; //wzy
static s_com_bus_R_alarm FA_fault_temp = {0};
static s_com_bus_R_reset FA_reset_temp = {0};
static s_com_bus_R_alarm FA_manul_fire_temp = {0};
static s_com_bus_R_power_off FA_power_off_temp = {0};
static s_IO_input_report IO_input_temp = {0};
static s_file_DATA file_data_request = {0};
static s_file_DATA *p_file_data_request = NULL;

//static s_IO_input_cfg IO_input_cfg_temp = {0};
static s_IO_input_cfg *p_IO_input_cfg = NULL;
static s_IO_input_cfg_report IO_input_cfg_report_temp = {0};
static s_sensor_cfg_report  sensor_cfg_temp = {0};
static s_sensor_ID_report   sensor_ID_temp[SENSOR_CHANNEL_MAX] = {0};
static s_file_info_ack  *p_file_info_ack = NULL;
//static s_sensor_data_report sensor_data_report_temp = {0};



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
	SYS_log( SYS_DEBUG_INFO, ("CB ack deal -- somke alarm, if_affirm : %d\n", if_affirm));
	return 0;
}

int UITD_cb_send_fail_somke_alarm(void *data, int node)
{
	SYS_log( SYS_DEBUG_INFO, ("CB send fail -- somke alarm, node : %d\n", node));
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

int UITD_file_close(int fd)
{
    return close(fd);
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
        case dev_type_controller:
            dev_type->dev_controller = 1;
            break;
        case dev_type_full_func:
            dev_type->dev_full_func = 1;
            break;
        case dev_type_IOT_PRO_UITD:
            dev_type->dev_IOT_PRO_UITD = 1;
            break;
        case dev_type_IOT_PRO_CTRL:
            dev_type->dev_IOT_PRO_CTRL = 1;
            break;
        default:
            break;
    }
    
    return 0;
}

int file_trans_init(s_file_trans_cb *file_trans)
{
    int i = 0;

    file_trans->buf_index = 0;
    file_trans->store_index = 0;
    file_trans->waiting_num = 0;
    
    if (file_trans->data_buf == NULL)
    {
        file_trans->data_buf = rt_malloc(sizeof(s_file_data_pkt) * file_trans->data_buf_num);
        if (file_trans->data_buf == NULL)
        {
            SYS_log(SYS_DEBUG_ERROR, ("File trans malloc failed!\n"));
            return -1;
        }
    }
    
    for (i=0;i<file_trans->data_buf_num;i++)
    {
        rt_memset(file_trans->data_buf[i].data, 0x00, FW_PKT_LEN_DEFAULT);
        file_trans->data_buf[i].if_valid = 0;
        file_trans->data_buf[i].index = 0;
    }
    
    return 0;
}


// Return the store index, if no valid space, return -1;
int file_trans_store_data(s_file_trans_cb *file_trans, uint32_t index, uint8_t *data, uint32_t len)
{
    int i = 0;
    
    if (len > FW_PKT_LEN_DEFAULT)
    {
        return -2;
    }
    
    for (i=0;i<file_trans->data_buf_num;i++)
    {
        if (file_trans->data_buf[i].if_valid == 0)
        {
            file_trans->data_buf[i].index = index;
            file_trans->data_buf[i].len = len;
            rt_memcpy(file_trans->data_buf[i].data, data, len);
            file_trans->data_buf[i].if_valid = 1;
            return i;
        }
    }

    return -1;
}

// Return the store index, if no valid space, return -1;
int file_trans_clean_repeat(s_file_trans_cb *file_trans, uint32_t index, uint8_t *data, uint32_t len)
{
    int i = 0;
    
    if (len > FW_PKT_LEN_DEFAULT)
    {
        return -2;
    }
    
    for (i=0;i<file_trans->data_buf_num;i++)
    {
        if (file_trans->data_buf[i].if_valid == 0)
        {
            file_trans->data_buf[i].index = index;
            file_trans->data_buf[i].len = len;
            rt_memcpy(file_trans->data_buf[i].data, data, len);
            file_trans->data_buf[i].if_valid = 1;
            return i;
        }
    }

    return -1;
}


int UITD_file_trans_init(t_server_handler *handler, s_file_trans_cb *file_trans, uint8_t buf_num)
{
    file_trans->data_buf_num = buf_num;
    file_trans->data_buf = NULL;
    
    if (handler->if_UITD == 0)
    {
        handler->file_trans = file_trans;
        file_trans_init(handler->file_trans);        
    }
    else
    {
        handler->file_trans = NULL;
        return -1;
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
	rt_memset(handler->server_addr, 0x00, sizeof(handler->server_addr));
    handler->if_testing_mode = 0;
    
	handler->ver_main = GB_SW_VER_MAIN;
	handler->ver_user = GB_SW_VER_USER;
    
    handler->FW.file_open = &UITD_file_open;
    handler->FW.file_write = &write;
    handler->FW.file_close = &UITD_file_close;
    
    UITD_create_dev_type(sys_config.dev_type, &handler->dev_type);
    
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

int UITD_connect(t_server_handler *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port, uint16_t listen_port)
{
    //handler->status = UITD_svc_status_idle;
    UITD_service_init(handler);
    
    
    handler->if_UDP = if_UDP;
    handler->if_use_name = if_use_name;
    handler->listen_port = listen_port;
    
    if (handler->if_use_name == 0)
    {
        sscanf(IP, "%d.%d.%d.%d", 
                    &handler->IP[0],
                    &handler->IP[1],
                    &handler->IP[2],
                    &handler->IP[3]
                    );
        handler->port = port;
        
        return handler->connect(handler, handler->if_UDP, handler->if_use_name, handler->IP, handler->port,
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
            SYS_log( SYS_DEBUG_ERROR, ("Server name is too long ! connect failed \n"));
            return -1;
        }
        strcpy(handler->svr_name, IP);
        handler->port = port;
        
        return handler->connect(handler, handler->if_UDP, handler->if_use_name, handler->svr_name, handler->port, 
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
    // Song: TODO: make sure this paragraph .
////    handler->status = UITD_svc_status_stoped;
    
    
//    if (handler->FW.data != NULL)
//    {
//        rt_free(handler->FW.data);
//    }
    
    if (handler->disconnect != NULL)
    {
        handler->disconnect(handler, handler->socket);
    }
    
    //GPRS_disconnect(handler->socket);
    
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
	////SYS_log( SYS_DEBUG_INFO, ("Ringbuf data number 03 : %d ", RingBuffer_available_data(ring_buf)));
	while(1)
	{
		////SYS_log( SYS_DEBUG_INFO, ("Ringbuf data number 04 : %d ", RingBuffer_available_data(ring_buf)));
		res = rt_ringbuffer_getchar(&handler->ring_buf, &data_temp);
		if (res == 0)
		{
			////SYS_log( SYS_DEBUG_INFO, ("!"));
			return -1;
		}
		////SYS_log( SYS_DEBUG_INFO, ("%02X , %d", data_temp, parse->status));
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
					////SYS_log( SYS_DEBUG_INFO, ("Parse valid "));
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
	
	
	SYS_log( SYS_DEBUG_DEBUG, ("SN      : %d \n", ctrl->SN));
	
	SYS_log( SYS_DEBUG_DEBUG, ("Ver_main: %d \n", ctrl->ver.ver_main));
	SYS_log( SYS_DEBUG_DEBUG, ("Ver_user: %d \n", ctrl->ver.ver_user));
	
	SYS_log( SYS_DEBUG_DEBUG, ("TimeStamp: %d-%d-%d %d:%d:%d \n", 
									ctrl->TS.year + 2000, 
									ctrl->TS.month, 
									ctrl->TS.day, 
									ctrl->TS.hour, 
									ctrl->TS.min, 
									ctrl->TS.sec));
	SYS_log( SYS_DEBUG_DEBUG, ("IP src : %d-%d-%d-%d-%d-%d \n", 
									ctrl->src.addr[5], 
									ctrl->src.addr[4], 
									ctrl->src.addr[3], 
									ctrl->src.addr[2], 
									ctrl->src.addr[1], 
									ctrl->src.addr[0]));

	SYS_log( SYS_DEBUG_DEBUG, ("IP des : %d-%d-%d-%d-%d-%d \n", 
									ctrl->des.addr[5], 
									ctrl->des.addr[4], 
									ctrl->des.addr[3], 
									ctrl->des.addr[2], 
									ctrl->des.addr[1], 
									ctrl->des.addr[0]));
	
	SYS_log( SYS_DEBUG_DEBUG, ("CMD : %d \n", ctrl->cmd));
	
	SYS_log( SYS_DEBUG_DEBUG, ("Checksum : %d , 0x%02X\n", pkt->checksum, pkt->checksum));
	
	SYS_log( SYS_DEBUG_DEBUG, ("Data_len : %d \n", ctrl->data_len));
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("Data_buf", pkt->buf, pkt->ctrl.data_len));

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

int UITD_trans_time(t_GB_ctrl_timestamp *TS, uint32_t time)
{
    struct tm *ti;
    
    ti = localtime(&time);
    
	TS->year = ti->tm_year + 1900 - 2000;
	TS->month = ti->tm_mon + 1;
	TS->day = ti->tm_mday;
	TS->hour = ti->tm_hour;
	TS->min = ti->tm_min;
	TS->sec = ti->tm_sec;
   
	return 0;
}

int UITD_set_time(t_GB_ctrl_timestamp *TS)
{
    uint32_t now = 0;
    struct tm time_tm= {0};
    
    time_tm.tm_year = TS->year + 2000;
    time_tm.tm_mon = TS->month - 1;
    time_tm.tm_mday = TS->day;
    time_tm.tm_hour = TS->hour;
    time_tm.tm_min = TS->min;
    time_tm.tm_sec = TS->sec;
    
    now = mktime(&time_tm);
    
    set_time_date(&now);
    
    return 0;
}

//int UITD_read_UITD_time(t_server_handler *handler, t_GB_pkt *pkt)
//{
//	int i = 0;
//    t_GB_ctrl_timestamp TS;
//    
//    UITD_get_time(&TS);

//	pkt->ack_cmd = GB_CMD_SEND_DATA;
//	pkt->data_unit.type = GB_TYPE_28_U_UITD_TIME;
//	pkt->data_unit.num	= 1;
//	pkt->data_unit.data = &TS;
//	pkt->data_unit.data_len = sizeof(t_GB_ctrl_timestamp);
//    
//	UITD_send_ack_data(handler, pkt, &pkt->data_unit);
//    return 0;
//}


int UITD_fill_ctrl(t_GB_ctrl_unit *ctrl_src, t_GB_ctrl_unit *ctrl_des, uint32_t data_len, uint8_t if_active_send)
{
	
	ctrl_des->ver.ver_main = GB_SW_VER_MAIN;
	ctrl_des->ver.ver_user = GB_SW_VER_USER;
	
	ctrl_des->SN = ctrl_src->SN;
    if (if_active_send)
    {
        rt_memcpy(ctrl_des->src.addr, ctrl_src->src.addr, sizeof(t_GB_ctrl_srcaddr));
        rt_memcpy(ctrl_des->des.addr, ctrl_src->des.addr, sizeof(t_GB_ctrl_desaddr));
    }
    else
    {
        rt_memcpy(ctrl_des->des.addr, ctrl_src->src.addr, sizeof(t_GB_ctrl_srcaddr));
        rt_memcpy(ctrl_des->src.addr, ctrl_src->des.addr, sizeof(t_GB_ctrl_desaddr));
    }
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
        UITD_fill_ctrl(&pkt->ctrl, &handler->node[index].ctrl, data_unit->data_len + 2, if_active_send);
	}
	else
	{
        UITD_fill_ctrl(&pkt->ctrl, &handler->node[index].ctrl, 0, if_active_send);
	}
	
	data = (uint8_t *)&handler->node[index].ctrl;
	for (i=0;i<sizeof(t_GB_ctrl_unit);i++)
	{
		checksum += data[i];
	}
	
	SYS_log( SYS_DEBUG_DEBUG, ("Ctrl checksum: 0x%02X\n", checksum));
	//handler->node[index].checksum = checksum%256;
	
	if (data_unit != NULL)
	{
		handler->node[index].data = rt_malloc(data_unit->data_len + 2);
		if (handler->node[index].data == NULL)
		{
			
			SYS_log( SYS_DEBUG_ERROR, ("No space to malloc !\n"));
			return -2;
		}
		
		rt_memcpy(&(handler->node[index].data[0]), &data_unit->type, 1);
		rt_memcpy(&(handler->node[index].data[1]), &data_unit->num, 1);
		
		rt_memcpy(&(handler->node[index].data[2]), data_unit->data, data_unit->data_len);
		
		
		SYS_log_HEX( SYS_DEBUG_DEBUG, ("Data unit : ", handler->node[index].data, handler->node[index].ctrl.data_len));
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

int UITD_send_ack_data(t_server_handler *handler, 
                        t_GB_pkt *pkt, 
                        t_GB_data_unit *data_unit)
{

	pkt->send_acked = NULL;
	pkt->send_fail = NULL;
	pkt->send_retry = NULL;
	pkt->send_success = NULL;
	pkt->ack_deal = NULL;
    
	return UITD_send_data(handler, pkt, data_unit, 0);
}

int UITD_send_data_no_SN(t_server_handler *handler, 
                        t_GB_pkt *pkt, 
                        t_GB_data_unit *data_unit)
{

	return UITD_send_data(handler, pkt, data_unit, 0);
}


int UITD_ack_88_28_U_UITD_TIME(t_server_handler *handler, t_GB_pkt *pkt)
{
	int i = 0;
    t_GB_ctrl_timestamp TS;
    
    UITD_get_time(&TS);

	pkt->ack_cmd = GB_CMD_SEND_DATA;
	pkt->data_unit.type = GB_TYPE_28_U_UITD_TIME;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &TS;
	pkt->data_unit.data_len = sizeof(t_GB_ctrl_timestamp);
    
	
    
    SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
    return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}


int UITD_send_2_U_FIRE_DEV_STATUS(t_server_handler *handler, t_GB_pkt *pkt, uint8_t	num)
{
	int i = 0;
	t_GB_dev_status_data *p_dev_status = NULL;
	
	if ((num == 0)|| (num >= GB_DEV_STATUS_DATA_MAX))
	{
		SYS_log( SYS_DEBUG_ERROR, ("Send fire device status parameter error ! num: %d \n", num));
		return -1;
	}

	p_dev_status = (t_GB_dev_status_data *)pkt->buf;
	SYS_log( SYS_DEBUG_DEBUG, (" num: %d , p_dev_status: 0x%08X\n", num, p_dev_status));
	
	for (i=0;i<num;i++)
	{
		UITD_get_time(&p_dev_status[i].timestamp);
	}
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_2_U_FIRE_DEV_STATUS;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = (uint8_t *)&p_dev_status[0];
	pkt->data_unit.data_len = sizeof(t_GB_dev_status_data) * num;
	
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
}

//int UITD_init_status_ack_affirm(char *data, int len, uint8_t if_affirm)
//{
//    t_server_handler *handler = (t_server_handler *)data;
//    
//    handler->if_connected = 2;
//}

int UITD_trans_data_failed(void *data, int node)
{
    t_server_handler *handler = (t_server_handler *)data;
    if ((handler->status == UITD_svc_status_alive) || (handler->status == UITD_svc_status_connect))
    {
        handler->status = UITD_svc_status_disconnected;
    }
    
    return 0;
}

int UITD_send_alive_pkt(t_server_handler *handler)
{
	int i = 0;
	t_GB_pkt pkt = {0};
    
	pkt.ctrl.cmd = JS_CMD_ALIVE;
	pkt.data_unit.type = JS_TYPE_128_ALIVE;
	pkt.data_unit.num	= 0;
	pkt.data_unit.data = NULL;
	pkt.data_unit.data_len = 0;
    
    pkt.send_fail = &UITD_trans_data_failed;
    
#if PATCH_UITD_ALIVE_EN
    patch_alive_cnt = 0;
#endif // PATCH_UITD_ALIVE_EN    
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}


int UITD_send_testing_data(t_server_handler *handler, uint32_t  data)
{
	int i = 0;
	t_GB_pkt pkt = {0};
    
	pkt.ctrl.cmd = JS_CMD_ALIVE;
	pkt.data_unit.type = JS_TYPE_128_ALIVE;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &data;
	pkt.data_unit.data_len = 4;
    
	return UITD_send_data_no_SN(handler, &pkt, &pkt.data_unit);
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

int UITD_dir_request_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t *dir_path)
{
    struct dfs_fd fd = {0};
    int res = 0;
    int length;
    struct stat stat;
    char *fullpath;
    uint8_t data_buf[FW_NAME_LEN_MAX + FW_PKT_LEN_DEFAULT] = {0};
    struct dirent dirent;
    uint8_t *dir_info = &data_buf[FW_NAME_LEN_MAX];
    int dir_len = 0;
    
    if (dir_path == NULL)
    {
        return -1;
    }
        
    strcpy(data_buf, dir_path);
    
    /* list directory */
    if (dfs_file_open(&fd, dir_path, DFS_O_DIRECTORY) == 0)
    {
        SYS_log(SYS_DEBUG_DEBUG, ("Directory %s:\n", dir_path));
        do
        {
            rt_memset(&dirent, 0, sizeof(struct dirent));
            length = dfs_file_getdents(&fd, &dirent, sizeof(struct dirent));
            if (length > 0)
            {
                rt_memset(&stat, 0, sizeof(struct stat));

                /* build full path for each file */
                fullpath = dfs_normalize_path(dir_path, dirent.d_name);
                if (fullpath == RT_NULL)
                    break;

                if (dfs_file_stat(fullpath, &stat) == 0)
                {
                    dir_len = sprintf(dir_info, "%s\t", dirent.d_name);
                    if (dir_len < 0) return -1;
                    else dir_info += dir_len;
                    
                    SYS_log(SYS_DEBUG_DEBUG, ("%s\t", dirent.d_name));
                    if ( DFS_S_ISDIR(stat.st_mode))
                    {
                        dir_len = sprintf(dir_info, "%s\n", "<DIR>");
                        if (dir_len < 0) return -1;
                        else dir_info += dir_len;
                        
                        SYS_log(SYS_DEBUG_DEBUG, ("%s\n", "<DIR>"));
                    }
                    else
                    {
                        dir_len = sprintf(dir_info, "%lu\n", stat.st_size);
                        if (dir_len < 0) return -1;
                        else dir_info += dir_len;
                        
                        SYS_log(SYS_DEBUG_DEBUG, ("%lu\n", stat.st_size));
                    }
                }
                else
                    SYS_log(SYS_DEBUG_DEBUG, ("BAD file: %s\n", dirent.d_name));
                rt_free(fullpath);
            }
        }while(length > 0);

        dfs_file_close(&fd);
    }
    else
    {
        SYS_log(SYS_DEBUG_DEBUG, ("No such directory : %s\n", dir_path));
        return -2;
    }
    
	pkt->ack_cmd = JS_CMD_DIR_FILE_UPLOAD;
	pkt->data_unit.type = JS_TYPE_194_U_DIR_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = data_buf;
    pkt->data_unit.data_len = dir_info - data_buf;
    
	
    
    SYS_log( SYS_DEBUG_DEBUG, ("Upload dir info : %s \n", dir_path));
    return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_file_data_request_ack(t_server_handler *handler, t_GB_pkt *pkt, s_file_DATA *file_data)
{
    int32_t len = 0;
    struct stat s;
    int res = 0;
    int fd = -1;
    uint32_t offset = 0;
    uint8_t data_buf[sizeof(s_file_DATA_ack) + FW_PKT_LEN_DEFAULT];
    s_file_DATA_ack *p_file_data_ack = (s_file_DATA_ack *)data_buf;
    //uint8_t *p_test = NULL;
    
    if (file_data->name == NULL)
    {
        UITD_deny(handler, pkt);
        return -1;
    }
    
    if (file_data->len > FW_PKT_LEN_DEFAULT)
    {
        UITD_deny(handler, pkt);
        return -2;
    }
    
    offset = file_data->index * file_data->len;
    
    fd = open(file_data->name, O_RDONLY, 0);
    if (fd >= 0)
    {
        res = lseek(fd, offset, DFS_SEEK_SET );
        if (res != offset)
        {
            len = -1;
            close(fd);
            UITD_deny(handler, pkt);
            return -3;
        }

        len = read(fd, data_buf + sizeof(s_file_DATA_ack), file_data->len );

        close(fd);

    }
    else
    {
        len = -1;
        UITD_deny(handler, pkt);
        return -3;
    }
    
    rt_memcpy(p_file_data_ack->name , file_data->name, sizeof(file_data->name));
    p_file_data_ack->index = file_data->index;
    p_file_data_ack->len = len;
    p_file_data_ack->CRC16 = Crc16(0, data_buf + sizeof(s_file_DATA_ack), p_file_data_ack->len);
    
	pkt->ack_cmd = JS_CMD_DIR_FILE_UPLOAD;
	pkt->data_unit.type = JS_TYPE_192_U_FILE_DATA_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = data_buf;
    if (len < 0)
    {
        pkt->data_unit.data_len = sizeof(s_file_DATA_ack);
    }
    else
    {
        pkt->data_unit.data_len = sizeof(s_file_DATA_ack) + len;
    }
    
    res = UITD_send_ack_data(handler, pkt, &pkt->data_unit);
	return res;
    
}

int UITD_file_info_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t *name)
{
    int32_t len = 0;
    struct stat s;
    int res = 0;
    int fd = -1;
    
    fd = open(name, O_RDONLY, 0);
    if (fd >= 0)
    {
        res = fstat(fd, &s);
        if (res == 0)
        {
            len = s.st_size;
        }
        else
        {
            len = -1;
        }
        close(fd);
    }
    else
    {
        len = -1;
    }
    
    
	pkt->ack_cmd = JS_CMD_FILE_SERVICE;
	pkt->data_unit.type = JS_TYPE_139_U_ACK_FILE_INFO;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &len;
	pkt->data_unit.data_len = sizeof(len);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}


int UITD_file_req_pkt(t_server_handler *handler, uint32_t index)
{
	int i = 0;
    t_GB_pkt pkt = {0};
    
    handler->FW.data_req.index = index;
    
	pkt.ctrl.cmd = JS_CMD_FILE_SERVICE;
	pkt.data_unit.type = JS_TYPE_130_U_REQ_FW_DATA;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &handler->FW.data_req;
	pkt.data_unit.data_len = sizeof(s_FW_DATA_req);
    
    pkt.send_fail = &UITD_file_req_pkt_failed;
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}

int UITD_file_req_info(t_server_handler *handler)
{
	int i = 0;
    s_FW_info_req fw_info = {0};
    t_GB_pkt pkt = {0};
    
    fw_info.fw_ver_main = sys_main_version;
    fw_info.fw_ver_user = sys_sub_version;
    
   
	pkt.ctrl.cmd = JS_CMD_FILE_SERVICE;
	pkt.data_unit.type = JS_TYPE_132_U_REQ_FW_INFO;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &fw_info;
	pkt.data_unit.data_len = sizeof(s_FW_info_req);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}



int UITD_file_req_length_info(t_server_handler *handler, uint8_t *name)
{
	int i = 0;
    s_file_info_req file_info = {0};
    t_GB_pkt pkt = {0};
    
    
    strcpy(file_info.name, name);
   
	pkt.ctrl.cmd = JS_CMD_FILE_SERVICE;
	pkt.data_unit.type = JS_TYPE_134_U_REQ_FILE_INFO;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &file_info;
	pkt.data_unit.data_len = sizeof(s_file_info_req);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}

int UITD_file_upload_file_info(t_server_handler *handler, uint8_t *name)
{
	int i = 0;
    s_file_info_ack file_info = {0};
    t_GB_pkt pkt = {0};
    int res = 0;
    
    strcpy(file_info.name, name);
    res = file_get_length(file_info.name, &file_info.length);
    if (res < 0)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Get file length failed : %s  --  %d\n", file_info.name, res));
        return -1;
    }

    res = file_get_CRC16(file_info.name, &file_info.CRC16);
    if (res < 0)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Get file CRC16 failed : %s  --  %d\n", file_info.name, res));
        return -1;
    }
    
	pkt.ctrl.cmd = JS_CMD_DIR_FILE_UPLOAD;
	pkt.data_unit.type = JS_TYPE_190_U_FILE_INFO_UPLOAD;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &file_info;
	pkt.data_unit.data_len = sizeof(s_file_info_ack);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}



int UITD_IOT_GW_info_report(t_server_handler *handler)
{
	int i = 0;
    s_IOT_GW_info GW_info = {0};
    t_GB_pkt pkt = {0};
    
    GW_info.ver_main = sys_main_version;
    GW_info.ver_sub = sys_sub_version;
    GW_info.dev_type = handler->dev_type;
    GW_info.port = handler->local_port;
    rt_memcpy(GW_info.IP, handler->local_IP, sizeof(GW_info.IP));
    rt_memcpy(GW_info.SN, handler->sys_addr, sizeof(GW_info.SN));
   
	pkt.ctrl.cmd = JS_CMD_IOT_GW_INFO;
	pkt.data_unit.type = JS_TYPE_170_U_IOT_GW_INFO_REPORT;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &GW_info;
	pkt.data_unit.data_len = sizeof(s_IOT_GW_info);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}


int UITD_IOT_GW_info_check_ack(t_server_handler *handler, t_GB_pkt *pkt)
{
	int i = 0;
    s_IOT_GW_info GW_info = {0};
    //t_GB_pkt pkt = {0};
    
    GW_info.ver_main = sys_main_version;
    GW_info.ver_sub = sys_sub_version;
    GW_info.dev_type = handler->dev_type;
    GW_info.port = handler->local_port;
    rt_memcpy(GW_info.IP, handler->local_IP, sizeof(GW_info.IP));
    rt_memcpy(GW_info.SN, handler->sys_addr, sizeof(GW_info.SN));
   
	pkt->ack_cmd = JS_CMD_IOT_GW_INFO;
	pkt->data_unit.type = JS_TYPE_172_U_IOT_GW_INFO_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &GW_info;
	pkt->data_unit.data_len = sizeof(s_IOT_GW_info);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}


int UITD_transparent_send_data(t_server_handler *handler, s_transparent_data *data)
{
    t_GB_pkt pkt = {0};
   
	pkt.ctrl.cmd = JS_CMD_ROUGH_DATA_TRANS;
	pkt.data_unit.type = JS_TYPE_200_U_ROUGH_DATA;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = data;
	pkt.data_unit.data_len = sizeof(s_transparent_data) - sizeof(data->data) + data->len;
    
    SYS_log(SYS_DEBUG_INFO, ("Transparent bytes %d :\n", data->len));
    SYS_log_HEX(SYS_DEBUG_DEBUG, ("DATA: ", data->data, data->len));
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}

int UITD_sensor_upload_rough_data(t_server_handler *handler, uint8_t ID, uint8_t out_type, uint16_t data, t_GB_ctrl_timestamp *TS)
{
	int i = 0;
    s_sensor_rough_data rough_data = {0};
    t_GB_pkt pkt = {0};
    
    rough_data.ID = ID;
    rough_data.out_type = out_type;
    rough_data.data = data;
    rt_memcpy(&rough_data.timestamp, TS, sizeof(t_GB_ctrl_timestamp));
    ////rough_data.timestamp = timestamp;
    //UITD_get_time(&rough_data.timestamp);
   
	pkt.ctrl.cmd = JS_CMD_SENSOR_SAMPLE;
	pkt.data_unit.type = JS_TYPE_140_U_SENSOR_DATA;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &rough_data;
	pkt.data_unit.data_len = sizeof(s_sensor_rough_data);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}



int UITD_sensor_ack_rough_data(t_server_handler *handler, t_GB_pkt *pkt, uint8_t ID, uint8_t out_type, uint16_t data)
{
	int i = 0;
    s_sensor_rough_data rough_data = {0};
    //t_GB_pkt pkt = {0};
    
    rough_data.ID = ID;
    rough_data.out_type = out_type;
    rough_data.data = data;

    UITD_get_time(&rough_data.timestamp);
    
    pkt->ack_cmd = JS_CMD_SENSOR_SAMPLE;
	pkt->data_unit.type = JS_TYPE_142_U_SENSOR_DATA_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &rough_data;
	pkt->data_unit.data_len = sizeof(s_sensor_rough_data);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_sensor_cfg_check_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t ID, uint32_t period)
{
	int i = 0;
    s_sensor_cfg_report cfg = {0};
    //t_GB_pkt pkt = {0};
    
    cfg.ID = ID;
    cfg.period = period;
   
	pkt->ack_cmd = JS_CMD_SENSOR_SAMPLE;
	pkt->data_unit.type = JS_TYPE_144_U_SENSOR_CFG_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &cfg;
	pkt->data_unit.data_len = sizeof(s_sensor_cfg_report);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_sensor_ID_check_ack(t_server_handler *handler, t_GB_pkt *pkt, s_sensor_ID_report *ID, uint32_t num)
{
    //t_GB_pkt pkt = {0};
   
	pkt->ack_cmd = JS_CMD_SENSOR_SAMPLE;
	pkt->data_unit.type = JS_TYPE_147_U_SENSOR_ID_ACK;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = ID;
	pkt->data_unit.data_len = sizeof(s_sensor_ID_report)*num;
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

//int UITD_output_ctrl_check_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t ID, uint8_t action)
//{
//    s_output_state s_output_data = {0};
//    //t_GB_pkt pkt = {0};
//   
//    output_state.ID = ID;
//    output_state.action = action;
//    
//	pkt->ack_cmd = JS_CMD_OUTPUT_CTRL;
//	pkt->data_unit.type = JS_TYPE_153_U_OUTPUT_CHECK_ACK;
//	pkt->data_unit.num	= 1;
//	pkt->data_unit.data = &output_state;
//	pkt->data_unit.data_len = sizeof(s_output_state);
//    
//	UITD_send_ack_data(handler, pkt, &pkt->data_unit);
//    return 0;
//}

int UITD_output_ctrl_check_mult_ack(t_server_handler *handler, t_GB_pkt *pkt, s_output_data *output_state, uint32_t num)
{
    
	pkt->ack_cmd = JS_CMD_OUTPUT_CTRL;
	pkt->data_unit.type = JS_TYPE_153_U_OUTPUT_CHECK_ACK;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = output_state;
	pkt->data_unit.data_len = sizeof(s_output_data)*num;
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}


int UITD_IO_state_report(t_server_handler *handler, uint8_t ID, e_IO_input_state state, t_GB_ctrl_timestamp *TS)
{
	int i = 0;
    s_IO_input_report IO_input_report = {0};
    t_GB_pkt pkt = {0};
    
    IO_input_report.ID = ID;
    IO_input_report.state = state;
    rt_memcpy(&IO_input_report.timestamp, TS, sizeof(t_GB_ctrl_timestamp));
    //UITD_get_time(&IO_input_report.timestamp);
  
	pkt.ctrl.cmd = JS_CMD_INPUT_REPORT;
	pkt.data_unit.type = JS_TYPE_160_U_INPUT_REPORT;
	pkt.data_unit.num	= 1;
	pkt.data_unit.data = &IO_input_report;
	pkt.data_unit.data_len = sizeof(s_IO_input_report);
    
	return UITD_send_actively(handler, &pkt, &pkt.data_unit);
}

int UITD_IO_state_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t ID, e_IO_input_state state)
{
	int i = 0;
    s_IO_input_report IO_input_report = {0};
    //t_GB_pkt pkt = {0};
    
    IO_input_report.ID = ID;
    IO_input_report.state = state;
    
    UITD_get_time(&IO_input_report.timestamp);
  
	pkt->ack_cmd = JS_CMD_INPUT_REPORT;
	pkt->data_unit.type = JS_TYPE_162_U_INPUT_STATE_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &IO_input_report;
	pkt->data_unit.data_len = sizeof(s_IO_input_report);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_IO_state_mult_ack(t_server_handler *handler, t_GB_pkt *pkt, s_IO_input_report *IO_input, uint32_t num)
{
	int i = 0;
    t_GB_ctrl_timestamp TS = {0};
    //t_GB_pkt pkt = {0};
    
    UITD_get_time(&TS);
    
    for (i=0;i<num;i++)
    {
        rt_memcpy(&IO_input[i].timestamp, &TS, sizeof(t_GB_ctrl_timestamp));
    }
  
	pkt->ack_cmd = JS_CMD_INPUT_REPORT;
	pkt->data_unit.type = JS_TYPE_162_U_INPUT_STATE_ACK;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = IO_input;
	pkt->data_unit.data_len = sizeof(s_IO_input_report)*num;
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_IO_state_cfg_ack(t_server_handler *handler, t_GB_pkt *pkt, uint8_t ID, e_IO_input_trig trig, uint16_t period)
{
	int i = 0;
    s_IO_input_cfg_report IO_input_cfg_report = {0};
    
    IO_input_cfg_report.ID = ID;
    IO_input_cfg_report.trig = trig;
    IO_input_cfg_report.period = period;
  
	pkt->ack_cmd = JS_CMD_INPUT_REPORT;
	pkt->data_unit.type = JS_TYPE_165_U_INPUT_CFG_ACK;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = &IO_input_cfg_report;
	pkt->data_unit.data_len = sizeof(s_IO_input_cfg_report);
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}


int UITD_IO_state_cfg_mult_ack(t_server_handler *handler, t_GB_pkt *pkt, s_IO_input_cfg_report *IO_input, uint32_t num)
{
  
	pkt->ack_cmd = JS_CMD_INPUT_REPORT;
	pkt->data_unit.type = JS_TYPE_165_U_INPUT_CFG_ACK;
	pkt->data_unit.num	= num;
	pkt->data_unit.data = IO_input;
	pkt->data_unit.data_len = sizeof(s_IO_input_report)*num;
    
	return UITD_send_ack_data(handler, pkt, &pkt->data_unit);
}

int UITD_send_dev_status(t_server_handler *handler, 
                            uint8_t dev_type, 
                            e_fire_dev_status dev_status, 
                            uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, 
                            e_GB_data_unit_status status, 
                            s_fire_dev_status_info *dev_info)
{
	int i = 0;
	int if_final = 0;
	static t_GB_dev_status_data *p_dev_status = NULL;
	t_GB_pkt *pkt = &handler->GB_pkt_send;

	
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
		SYS_log( SYS_DEBUG_ERROR, ("Data unit is too many !"));
		return -1;
	}
	
	// wzy
//	if(sys_config.server_elec_fire == 1)
//	{
//		p_dev_status[pkt->data_num].sys_type = GB_SYS_TYPE_FOAM_OUTFIRE;		
//	}
//	else
	{
		p_dev_status[pkt->data_num].sys_type = GB_SYS_TYPE_FIRE_ALARMER;
	}

	//p_dev_status[pkt->data_num].sys_addr = sys_addr;
    p_dev_status[pkt->data_num].sys_addr.SZJS_addr.port = port;
    p_dev_status[pkt->data_num].sys_addr.SZJS_addr.controller = sys_addr;
	p_dev_status[pkt->data_num].type	 = dev_type;
	rt_memcpy(&p_dev_status[pkt->data_num].addr[0], &addr_sub, 2);
	rt_memcpy(&p_dev_status[pkt->data_num].addr[2], &addr_main, 2);
//    if ((remark != NULL) && (strlen(remark) <= sizeof(p_dev_status->remark)))
    //if (remark != NULL)
    {
        rt_memcpy(&p_dev_status[pkt->data_num].dev_info, dev_info, sizeof(p_dev_status[pkt->data_num].dev_info));
    }    
    
	
    memset(&p_dev_status[pkt->data_num].status, 0x00, sizeof(p_dev_status[pkt->data_num].status) );
    
    switch (dev_status)
    {
		
		case fire_dev_status_working:	//wzy   正常运行
            p_dev_status[pkt->data_num].status.if_working = 1;
        break;
		
        case fire_dev_status_smoke_alarm:	//火警
            p_dev_status[pkt->data_num].status.if_fire = 1;
        break; 
		    
		case fire_dev_status_smoke_fault:	//故障
            p_dev_status[pkt->data_num].status.if_fault = 1;
        break;
        case fire_dev_status_smoke_disable:	//屏蔽
            p_dev_status[pkt->data_num].status.if_disable = 1;
        break;
        case fire_dev_status_smoke_resume:	//监管
            //p_dev_status[pkt->data_num].status.if_disable = 1;
        break;
        default:
            break;
    }
    
	//p_dev_status[pkt->data_num].status.if_fire = 1;
	
//	for (i=0;i<(sizeof(p_dev_status[pkt->data_num].remark)-1);i++)
//	{
//		p_dev_status[pkt->data_num].remark[i] = ' ';
//	}

	pkt->data_num ++;
	
		
	if (if_final)
	{	
		
		SYS_log( SYS_DEBUG_DEBUG, ("Sending the fire devices status ...\n"));
		UITD_send_2_U_FIRE_DEV_STATUS(handler, pkt, pkt->data_num);
	}
	
	return 0;
}

int UITD_send_manul_fire_alarm(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, GB_DEV_TYPE_MANUL_ALARM_BUTTON, fire_dev_status_smoke_alarm, port, sys_addr, addr_main, addr_sub, status, dev_info);
}
//系统类型标志  
int UITD_send_smoke_fire_alarm(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
	if(sys_config.server_elec_fire == 1) //wzy
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_ELEC_FIRE_ALARMER, fire_dev_status_smoke_alarm, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}
	else
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_SOMKE_ALARMER, fire_dev_status_smoke_alarm, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}
   
}
//wzy
int UITD_send_smoke_elec_data(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
	if(sys_config.server_elec_fire == 1)
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_ELEC_FIRE_ALARMER, fire_dev_status_working, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}
	else
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_SOMKE_ALARMER, fire_dev_status_working, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}
}

int UITD_send_smoke_fire_fault(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
	
	if(sys_config.server_elec_fire == 1)	//wzy
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_ELEC_FIRE_ALARMER, fire_dev_status_smoke_fault, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}
	else
	{
		return UITD_send_dev_status(handler, GB_DEV_TYPE_SOMKE_ALARMER, fire_dev_status_smoke_fault, port, sys_addr, addr_main, addr_sub, status, dev_info);
	}

}

int UITD_send_smoke_fire_resume(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, GB_DEV_TYPE_SOMKE_ALARMER, fire_dev_status_smoke_resume, port, sys_addr, addr_main, addr_sub, status, dev_info);
}

int UITD_send_smoke_fire_disable(t_server_handler *handler, uint8_t port, uint8_t sys_addr, uint16_t addr_main, uint16_t addr_sub, s_fire_dev_status_info *dev_info, e_GB_data_unit_status status)
{
    return UITD_send_dev_status(handler, GB_DEV_TYPE_SOMKE_ALARMER, fire_dev_status_smoke_disable, port, sys_addr, addr_main, addr_sub, status, dev_info);
}


//初始化用
int UITD_send_21_U_UITD_STATUS(t_server_handler *handler, t_GB_pkt *pkt, t_GB_UITD_status status)
{
	
	int i = 0;
	t_GB_UITD_status_data *p_UITD_status = NULL;
	

	p_UITD_status = (t_GB_UITD_status_data *)pkt->buf;
	
	p_UITD_status[0].status = status;
    if (handler->if_testing_mode)
    {
        p_UITD_status[0].status.if_working = 0;
    }
    else
    {
        p_UITD_status[0].status.if_working = 1;
    }
	SYS_log( SYS_DEBUG_INFO, (" UITD_status: 0x%02X\n", p_UITD_status->status));
	
    UITD_get_time(&p_UITD_status[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_21_U_UITD_STATUS;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_UITD_status;
	pkt->data_unit.data_len = sizeof(t_GB_UITD_status_data);

    pkt->send_fail = &UITD_trans_data_failed;
    
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
	
}


//没用到
int UITD_send_24_U_UITD_OPERATE(t_server_handler *handler, t_GB_pkt *pkt, t_GB_UITD_operate operate)
{
	
	int i = 0;
	t_GB_UITD_operate_data *p_UITD_operate = NULL;

	p_UITD_operate = (t_GB_UITD_operate_data *)pkt->buf;
	
	p_UITD_operate[0].operate = operate;
	p_UITD_operate[0].operator_ID = 0x00;
	SYS_log( SYS_DEBUG_INFO, (" UITD_status: 0x%02X\n", p_UITD_operate->operate));
	
    UITD_get_time(&p_UITD_operate[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_24_U_UITD_OPERATE;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_UITD_operate;
	pkt->data_unit.data_len = sizeof(t_GB_UITD_operate_data);

    pkt->send_fail = &UITD_trans_data_failed;
    
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
	
}
//没用到
int UITD_send_1_U_FIRE_SYS_STATUS(t_server_handler *handler, t_GB_pkt *pkt, uint8_t sys_addr, uint8_t port, t_GB_fire_sys_status status)
{
	int i = 0;
	t_GB_fire_sys_status_data *p_fire_sys_status = NULL;
	

	p_fire_sys_status = (t_GB_fire_sys_status_data *)pkt->buf;
	SYS_log( SYS_DEBUG_INFO, (" fire_sys_status: 0x%04X\n", status));
	
	p_fire_sys_status[0].status = status;
    
    p_fire_sys_status[0].status.if_working = 1;
    
    p_fire_sys_status[0].sys_type = GB_SYS_TYPE_FIRE_ALARMER;
    
    if ((port > 7)||(sys_addr > 31))
    {
        SYS_log(SYS_DEBUG_ERROR, ("Send FIRE_SYS_STATUS error ! port: %d, sys_addr %d \n", port, sys_addr));
        return -1;
    }
    //p_fire_sys_status[0].sys_addr.sys_addr = sys_addr;
    p_fire_sys_status[0].sys_addr.SZJS_addr.controller = sys_addr;
    p_fire_sys_status[0].sys_addr.SZJS_addr.port = port;
    
	UITD_get_time(&p_fire_sys_status[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_1_U_FIRE_SYS_STATUS;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_fire_sys_status;
	pkt->data_unit.data_len = sizeof(t_GB_fire_sys_status_data);

	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
}

//没用到
int UITD_send_4_U_FIRE_SYS_OPERATE(t_server_handler *handler, t_GB_pkt *pkt, uint8_t port, uint8_t sys_addr, t_GB_fire_sys_operate operate)
{
	int i = 0;
	t_GB_fire_sys_operate_data *p_fire_sys_operate = NULL;
	
	p_fire_sys_operate = (t_GB_fire_sys_operate_data *)pkt->buf;
	SYS_log( SYS_DEBUG_INFO, (" fire_sys_operate: 0x%02X\n", operate));
	
	p_fire_sys_operate[0].operate = operate;
    p_fire_sys_operate[0].sys_type = GB_SYS_TYPE_FIRE_ALARMER;
    //p_fire_sys_operate[0].sys_addr = sys_addr;
    p_fire_sys_operate[0].sys_addr.SZJS_addr.port = port;
    p_fire_sys_operate[0].sys_addr.SZJS_addr.controller = sys_addr;
	p_fire_sys_operate[0].operator_ID = 0x00;

	UITD_get_time(&p_fire_sys_operate[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_4_U_FIRE_DEV_OPERATE;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_fire_sys_operate;
	pkt->data_unit.data_len = sizeof(t_GB_fire_sys_operate_data);

	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
}

//没用到
int UITD_send_200_U_ROUGH_DATA(t_server_handler *handler, t_GB_pkt *pkt)
{
	int i = 0;
	t_GB_UITD_status_data *p_UITD_status = NULL;
	

	p_UITD_status = (t_GB_UITD_status_data *)pkt->buf;
	
//	p_UITD_status[0].status = 0;
    if (handler->if_testing_mode)
    {
        p_UITD_status[0].status.if_working = 0;
    }
    else
    {
        p_UITD_status[0].status.if_working = 1;
    }
	SYS_log( SYS_DEBUG_INFO, (" UITD_status: 0x%02X\n", p_UITD_status->status));
	
    UITD_get_time(&p_UITD_status[0].timestamp);
	
	pkt->ctrl.cmd = GB_CMD_SEND_DATA;
	
	pkt->data_unit.type = GB_TYPE_21_U_UITD_STATUS;
	pkt->data_unit.num	= 1;
	pkt->data_unit.data = (uint8_t *)p_UITD_status;
	pkt->data_unit.data_len = sizeof(t_GB_UITD_status_data);

    pkt->send_fail = &UITD_trans_data_failed;
    
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("TS: ", pkt->data_unit.data, pkt->data_unit.data_len));
	return UITD_send_actively(handler, pkt, &pkt->data_unit);
}
//没用到
int UITD_send_fire_sys_main_power_fault(t_server_handler *handler)
{
    t_GB_pkt pkt = {0};
	t_GB_fire_sys_status status = {0};
    
	status.if_main_pwr = 1;
	UITD_send_1_U_FIRE_SYS_STATUS(handler, &pkt, 0, 0, status);
    return 0;
}
//没用到
int UITD_send_UITD_main_power_fault(t_server_handler *handler, t_GB_pkt *pkt)
{
	t_GB_UITD_status status = {0};
	
	status.if_main_power = 1;
	UITD_send_21_U_UITD_STATUS(handler, pkt, status);
    return 0;
}
//没用到
int UITD_send_FA_reset(t_server_handler *handler,  uint8_t port, uint8_t sys_addr)
{
    t_GB_pkt pkt = {0};
	t_GB_fire_sys_operate operate = {0};
    
	operate.reset = 1;
	UITD_send_4_U_FIRE_SYS_OPERATE(handler, &pkt, port, sys_addr, operate);
    return 0;
}

int UITD_send_UITD_reset(t_server_handler *handler)
{
    t_GB_pkt pkt = {0};
	t_GB_UITD_operate operate = {0};
    
	operate.reset = 1;
	UITD_send_24_U_UITD_OPERATE(handler, &pkt, operate);
    return 0;
}
//没用到
int UITD_send_UITD_backup_power_fault(t_server_handler *handler, t_GB_pkt *pkt)
{
	t_GB_UITD_status status = {0};
	
	status.if_backup_power = 1;
	UITD_send_21_U_UITD_STATUS(handler, pkt, status);
    return 0;
}
//初始化用
int UITD_send_inital_status_ack_deal(char *data,int len, uint8_t if_affirm)
{
    t_server_handler *handler = NULL;
    
    handler = (t_server_handler *)data;
    handler->status = UITD_svc_status_alive;
    
    return 0;
}
//初始化用
int UITD_send_UITD_inital_status(t_server_handler *handler)
{
    t_GB_pkt pkt = {0};
	t_GB_UITD_status status = {0};
    
    pkt.ctrl.SN = 0x0000;
	pkt.ack_deal = &UITD_send_inital_status_ack_deal;
    //pkt.ack_deal = &UITD_init_status_ack_affirm;
    pkt.send_fail = &UITD_trans_data_failed;
    
	//status.if_working = 1;
	UITD_send_21_U_UITD_STATUS(handler, &pkt, status);
    return 0;
}
//没用到
int UITD_send_comm_rough_data(t_server_handler *handler)
{
    t_GB_pkt pkt = {0};
	t_GB_UITD_status status = {0};
    
    
	UITD_send_200_U_ROUGH_DATA(handler, &pkt);
    return 0;
}


int UITD_deal(t_server_handler *handler, t_GB_pkt *pkt)
{
    int i = 0;
    int j = 0;
	int res = 0;
	int node = 0;
    s_IO_input_report *p_IO_input = NULL;
    //s_IO_input_cfg_report *p_IO_input_cfg = NULL;
    
    //int file_index;
    //s_FW_DATA *p_data_rec = NULL;
	s_FW_DATA file_data_rec = {0};
    
	switch (pkt->ctrl.cmd)
	{
		case GB_CMD_CTRL:
			switch (pkt->data_type)
			{
				case GB_TYPE_89_C_UITD_INIT:
                    // UITD init.
					SYS_log( SYS_DEBUG_INFO, ("C_UITD_INIT :%d\n", pkt->data_type));
                    
                    res = UITD_affirm(handler, pkt);
                    SYS_log( SYS_DEBUG_INFO, ("UITD Node : %d\n", res));

                
					break;
				case GB_TYPE_90_C_UITD_SYNC:
					// Update the UITD time 
					SYS_log( SYS_DEBUG_INFO, ("C_UITD_SYNC :%d\n", pkt->data_type));
                    UITD_set_time((t_GB_ctrl_timestamp *)(pkt->buf+2));
					res = UITD_affirm(handler, pkt);
					SYS_log( SYS_DEBUG_INFO, ("UITD Node : %d\n", res));
				
				break;
				case GB_TYPE_91_C_UITD_CHECK:
                    // UITD handler checkout, Not support by GPRS_IOT_AP.
					SYS_log( SYS_DEBUG_INFO, ("C_UITD_CHECK :%d\n", pkt->data_type));
                    
					res = UITD_affirm(handler, pkt);
					SYS_log( SYS_DEBUG_INFO, ("UITD Node : %d\n", res));
					
					break;
				default:
					break;
			
			}
			break;
		case GB_CMD_SEND_DATA:
			
			break;
		case GB_CMD_AFFIRM:
			SYS_log( SYS_DEBUG_INFO, ("Rec affirm:%d\n", pkt->ctrl.SN));
			node = UITD_server_search_node( handler, pkt->ctrl.SN);
			if (node >= 0)
			{
				handler->node[node].ack_status = cmd_acked_affirm;
				SYS_log( SYS_DEBUG_DEBUG, ("Searched the match node\n"));
                rt_memcpy(handler->server_addr, &pkt->ctrl.src.addr[0], sizeof(handler->server_addr));
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
            SYS_log( SYS_DEBUG_INFO, ("Received a deny packet, SN:%d\n", pkt->ctrl.SN));
			node = UITD_server_search_node( handler, pkt->ctrl.SN);
			if (node >= 0)
			{
				handler->node[node].ack_status = cmd_acked_deny;
                SYS_log( SYS_DEBUG_DEBUG, ("Searched the match node\n"));
			}
			break;
        case JS_CMD_FILE_SERVICE:
            switch (pkt->data_type)
            {
                case JS_TYPE_129_D_PUSH_FW_INFO:
                    if (handler->if_UITD)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                
                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    SYS_log( SYS_DEBUG_INFO, ("Server pushed a new firmware\n"));
                    SYS_log( SYS_DEBUG_INFO, ("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user));
                    SYS_log( SYS_DEBUG_INFO, ("Support device: 0X%08x\n", *(uint32_t *)&handler->FW.FW.support_dev));
                    SYS_log( SYS_DEBUG_INFO, ("FW name: %s\n", handler->FW.FW.name));
                    SYS_log( SYS_DEBUG_INFO, ("FW len: %d\n", handler->FW.FW.len));
                    handler->FW.push_FW_flag = 1;
                    handler->FW.status = FW_idle;
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_138_D_REQ_FILE_INFO:
                    if (handler->if_UITD)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                
                    SYS_log( SYS_DEBUG_INFO, ("Server check this file: %s\n", pkt->buf+2));
                    UITD_file_info_ack(handler, pkt, pkt->buf+2);
                    break;
                case JS_TYPE_136_D_PUSH_FORCE_FW_INFO:
                    if (handler->if_UITD)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    SYS_log( SYS_DEBUG_INFO, ("Server pushed a force firmware\n"));
                    SYS_log( SYS_DEBUG_INFO, ("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user));
                    SYS_log( SYS_DEBUG_INFO, ("Support device: 0X%08x\n", *(uint32_t *)&handler->FW.FW.support_dev));
                    SYS_log( SYS_DEBUG_INFO, ("FW name: %s\n", handler->FW.FW.name));
                    SYS_log( SYS_DEBUG_INFO, ("FW len: %d\n", handler->FW.FW.len));
                    handler->FW.force_FW_flag = 1;
                    handler->FW.status = FW_idle;
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_137_D_PUSH_FORCE_FILE_INFO:
                    if (handler->if_UITD)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    p_file_info_ack = (s_file_info_ack *)(pkt->buf+2);
                    rt_memcpy(&handler->FW.FW.name, p_file_info_ack->name, sizeof(handler->FW.FW.name));
                    handler->FW.FW.len = p_file_info_ack->length;
                    handler->FW.FW.CRC16 = p_file_info_ack->CRC16;
                    //rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_file_info_ack));
                    SYS_log( SYS_DEBUG_INFO, ("Server pushed a force file\n"));
                    SYS_log( SYS_DEBUG_INFO, ("File name: %s\n", handler->FW.FW.name));
                    SYS_log( SYS_DEBUG_INFO, ("FW len: %d\n", handler->FW.FW.len));
                    handler->FW.force_file_flag = 1;
                    handler->FW.status = FW_idle;
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_131_D_ACK_FW_DATA:
                    node = UITD_server_search_node( handler, pkt->ctrl.SN);
                    if (node >= 0)
                    {
                        handler->node[node].ack_status = cmd_acked_affirm;
                        SYS_log( SYS_DEBUG_DEBUG, ("Searched the match node\n"));
                    }
                    else
                    {
                        break;
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
                                
                                SYS_log( SYS_DEBUG_INFO, ("SN: %d, Len: %d\n", 
                                                    handler->FW.data_rec.index, handler->FW.data_rec.len));
                                    
                                if (handler->FW.data_rec.len == -1)
                                {
                                    SYS_log( SYS_DEBUG_ERROR, 
                                                ("File data len error : %d\n", handler->FW.data_rec.len));
                                    handler->FW.status = FW_failed;
                                }
                                else
                                {
                                    handler->file_trans->buf_index = file_trans_store_data(handler->file_trans, 
                                                                            handler->FW.data_rec.index, 
                                                                            pkt->buf+2+sizeof(s_FW_DATA), 
                                                                            handler->FW.data_rec.len);
                                    
                                    if ((handler->file_trans->buf_index >= 0) && 
                                        (handler->FW.data_rec.CRC16 == 
                                            Crc16(0, handler->file_trans->data_buf[handler->file_trans->buf_index].data, handler->FW.data_rec.len)))
                                    {
                                        SYS_log( SYS_DEBUG_DEBUG, 
                                                ("Buffer file data, index: %d\n", 
                                                    handler->file_trans->buf_index));
                                        
                                        // If there is a sequence index data_pkt in the buffer, save it to file.
                                        for (i=0;i<handler->file_trans->data_buf_num;i++)
                                        {
                                            if ((handler->file_trans->store_index < handler->FW.req_pkt_num) && 
                                                (handler->file_trans->data_buf[i].if_valid))
                                            {
                                                if (handler->file_trans->data_buf[i].index == handler->file_trans->store_index)
                                                {
                                                    res = handler->FW.file_write(handler->FW.fw_fd, 
                                                                                handler->file_trans->data_buf[i].data, 
                                                                                handler->file_trans->data_buf[i].len);
                                                    handler->file_trans->data_buf[i].if_valid = 0;
                                                    if (handler->file_trans->waiting_num > 0)
                                                    {
                                                        handler->file_trans->waiting_num --;
                                                    }
                                                    else
                                                    {
                                                        SYS_log( SYS_DEBUG_ERROR, 
                                                                ("Buffer space error, waiting num less than 0\n"));
                                                        handler->FW.status = FW_failed;
                                                        break;
                                                    }
                                                    SYS_log( SYS_DEBUG_DEBUG, ("Saved file data, index: %d, waiting: %d\n",
                                                                            handler->file_trans->data_buf[i].index,
                                                                            handler->file_trans->waiting_num));
                                                    if (res == handler->file_trans->data_buf[i].len)
                                                    {
                                                        
                                                        handler->file_trans->store_index ++;
                                                        if (handler->file_trans->store_index == handler->FW.req_pkt_num)
                                                        {
                                                            handler->FW.status = FW_done;
                                                            break;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        handler->FW.status = FW_failed;
                                                        break;
                                                    }
                                                    //handler->file_trans->store_index ++;
                                                    // Once find a match sequence index, search sequence index again, untile can not find.
                                                    i = 0;
                                                    continue;
                                                }
                                                
                                            }
                                        }
                                        
                                        
                                        // Request new data_pkt when there are free buffer spaces.
                                        for (i=0;i<handler->file_trans->data_buf_num;i++)
                                        {
                                            if ((handler->file_trans->waiting_num < handler->file_trans->data_buf_num) && 
                                                (handler->FW.data_req.index < handler->FW.req_pkt_num) && 
                                                (handler->file_trans->data_buf[i].if_valid == 0))
                                            {
                                                UITD_file_req_pkt(handler, handler->FW.data_req.index);
                                                SYS_log( SYS_DEBUG_DEBUG, ("Fetch file data, index: %d, waiting: %d\n", 
                                                                        handler->FW.data_req.index,
                                                                        handler->file_trans->waiting_num + 1));
                                                handler->FW.data_req.index ++;
                                                handler->file_trans->waiting_num ++;
                                                if (handler->FW.data_req.index >= handler->FW.req_pkt_num)
                                                {
                                                    break;
                                                }
                                                handler->FW.data_req.len = (handler->FW.FW.len / FW_PKT_LEN_DEFAULT)? FW_PKT_LEN_DEFAULT:handler->FW.FW.len;
                                            }
                                        }
                                        
                                        
                                    }
                                    else // buffer file data failed.
                                    {
                                        SYS_log( SYS_DEBUG_ERROR, 
                                                ("Buffer file data or CRC verify failed. file receive failed !  buf_index: %d, CRC value: %d\n", 
                                                    handler->file_trans->buf_index, 
                                                    handler->FW.data_rec.CRC16));
                                        handler->FW.status = FW_failed;
                                        break;
                                    }
                                }
                                
                                
//                                if (handler->FW.data_req.index == handler->FW.data_rec.index)
//                                {
//                                        rt_memcpy(handler->FW.data, pkt->buf+2+sizeof(s_FW_DATA), handler->FW.data_rec.len);
//                                        // Song: TODO,Check the CRC16 value.
//                                        if (handler->FW.data_rec.CRC16 == Crc16(0, handler->FW.data, handler->FW.data_rec.len))
//                                        //if (1)
//                                        {
//                                            res = handler->FW.file_write(handler->FW.fw_fd, handler->FW.data, handler->FW.data_rec.len);
//                                            if (res == handler->FW.data_rec.len)
//                                            {
//                                                SYS_log( SYS_DEBUG_INFO, ("Writed file data, SN: %d\n", handler->FW.data_req.index));
//                                                if (handler->FW.data_req.index < (handler->FW.req_pkt_num-1))
//                                                {
//                                                    
//                                                    handler->FW.data_req.index++;
//                                                    UITD_file_req_pkt(handler, handler->FW.data_req.index);
//                                                }
//                                                else
//                                                {
//                                                    
//                                                    handler->FW.status = FW_done;
//                                                }
//                                                
//                                                
//                                            }
//                                            else // Write file data failed.
//                                            {
//                                                SYS_log( SYS_DEBUG_ERROR, 
//                                                        ("File write failed. file receive failed !  write: %d, writen: %d\n", 
//                                                            handler->FW.data_rec.len, 
//                                                            res));
//                                                handler->FW.status = FW_failed;
//                                            }
//                                        }
//                                        else
//                                        {
//                                            SYS_log( SYS_DEBUG_ERROR, ("CRC16 verify is wrong ! file receive failed ! \n"));
//                                            handler->FW.status = FW_failed;
//                                        }
//                                    }
//                                }
                            
                            }
                            break;
                        case FW_done:
                            break;
                        default:
                            break;
                    }
                    break;
                case JS_TYPE_133_D_ACK_FW_INFO:
                    
                    node = UITD_server_search_node( handler, pkt->ctrl.SN);
                    if (node >= 0)
                    {
                        handler->node[node].ack_status = cmd_acked_affirm;
                        SYS_log( SYS_DEBUG_DEBUG, ("Searched the match node\n"));
                    }
                    else
                    {
                        break;
                    }

                    rt_memcpy(&handler->FW.FW, pkt->buf+2, sizeof(s_FW_info));
                    if (handler->FW.FW.len == -1)
                    {
                        SYS_log( SYS_DEBUG_ERROR, ("Server return invalid firmware information !\n"));
                        if (handler->FW.file_down_fail) handler->FW.file_down_fail(handler);
                    }
                    else
                    {
                        SYS_log( SYS_DEBUG_INFO, ("Server return new firmware information\n"));
                        SYS_log( SYS_DEBUG_INFO, ("Version: %d.%02d\n", handler->FW.FW.fw_ver_main, handler->FW.FW.fw_ver_user));
                        SYS_log( SYS_DEBUG_INFO, ("FW name: %s\n", handler->FW.FW.name));
                        SYS_log( SYS_DEBUG_INFO, ("FW len: %d\n", handler->FW.FW.len));
                        handler->FW.return_FW_flag = 1;
                    }
                    break;
                case JS_TYPE_135_D_ACK_FILE_INFO:
                    node = UITD_server_search_node( handler, pkt->ctrl.SN);
                    if (node >= 0)
                    {
                        handler->node[node].ack_status = cmd_acked_affirm;
                        SYS_log( SYS_DEBUG_DEBUG, ("Searched the match node\n"));
                    }
                    else
                    {
                        break;
                    }
                    
                    p_file_info_ack = (s_file_info_ack *)(pkt->buf+2);
                    
                    if (p_file_info_ack->length == -1)
                    {
                        rt_kprintf("File not exist ! \n");
                        if (handler->FW.file_down_fail != NULL)
                        {
                            handler->FW.file_down_fail(handler);
                        }
                        
                    }
                    else if (p_file_info_ack->length == 0)
                    {
                        rt_kprintf("File length is 0 ! \n");
                        if (handler->FW.file_down_fail != NULL)
                        {
                            handler->FW.file_down_fail(handler);
                        }
                    }
                    else
                    {
                        //strcpy(handler->FW.fw_path, p_file_info_ack->name);
                        strcpy(handler->FW.FW.name, p_file_info_ack->name);
                        handler->FW.FW.len = p_file_info_ack->length;
                        handler->FW.FW.CRC16 = p_file_info_ack->CRC16;
                        
                        handler->FW.status = FW_init;
                    }
                    
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_DIR_FILE_UPLOAD:
            switch (pkt->data_type)
            {
                case JS_TYPE_191_R_FILE_DATA_REQUEST:
                    //rt_memcpy(&file_data_request, pkt->buf+2, sizeof(file_data_request));
                    p_file_data_request = (s_file_DATA *)(pkt->buf+2);
                    
                    res = UITD_file_data_request_ack(handler, pkt, p_file_data_request);
                    if (res < 0)
                    {
                        SYS_log(SYS_DEBUG_ERROR, ("UITD upload file data failed : %d\n", res));
                    }
                    
                    break;
                case JS_TYPE_193_R_DIR_REQUEST:
                    
                    res = UITD_dir_request_ack(handler, pkt, pkt->buf+2);
                
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_IOT_GW_INFO:
            switch (pkt->data_type)
            {
                case JS_TYPE_171_R_IOT_GW_INFO_CHECK:
                    UITD_IOT_GW_info_check_ack(handler, pkt);
                    break;
                case JS_TYPE_173_D_IOT_GW_RESTART:
                    SYS_log( SYS_DEBUG_INFO, ("Server pushed a RESTART command !!!  Restart 5 second later.\n"));
                    handler->AP_ctrl.restart = 1;
                    UITD_affirm(handler, pkt);
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_IOT_GW_SCRIPT:
            switch (pkt->data_type)
            {
                case JS_TYPE_180_D_IOT_GW_SCRIPT_EXECUTE:
                    if (handler->if_UITD)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    // Song: TODO
                    SYS_log( SYS_DEBUG_INFO, ("Execute script file : %s\n", pkt->buf+2));
                    res = script_exec((uint8_t *)(pkt->buf+2));
                    if (res == 0)
                    {
                        UITD_affirm(handler, pkt);
                    }
                    else // execute script file failed.
                    {
                        UITD_deny(handler, pkt);
                    }
                    break;
                default:
                    break;
            }
            break;        
        case JS_CMD_SENSOR_SAMPLE:
            switch (pkt->data_type)
            {
                case JS_TYPE_141_D_SENSOR_DATA_CHECK:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            if (p_sensor_cb->sensor[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_sensor_ack_rough_data(p_sensor_cb->handler, pkt,
                                                            p_sensor_cb->sensor[i].ID, 
                                                            p_sensor_cb->sensor[i].out_type, 
                                                            p_sensor_cb->sensor[i].data_rough);
                                rt_mutex_release(&mt_sensor);
                                break;
                            }
                        }
                        if (i == p_sensor_cb->sensor_num) // ID is not exist
                        {
                            UITD_sensor_ack_rough_data(p_sensor_cb->handler, pkt, 
                                                            0, 
                                                            0, 
                                                            0x0000);
                        }
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                case JS_TYPE_143_D_SENSOR_CFG_CHECK:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            if (p_sensor_cb->sensor[i].ID == *(uint8_t *)(pkt->buf+2))
                            {
                                UITD_sensor_cfg_check_ack(p_sensor_cb->handler, pkt, 
                                                            p_sensor_cb->sensor[i].ID, 
                                                            p_sensor_cb->sensor[i].period);
                                rt_mutex_release(&mt_sensor);
                                break;
                            }
                        }
                        if (i == p_sensor_cb->sensor_num) // ID is not exist
                        {
                            UITD_sensor_cfg_check_ack(p_sensor_cb->handler, pkt, 
                                                            0, 
                                                            0);
                        }
                    }
                    rt_mutex_release(&mt_sensor);
                    break;
                case JS_TYPE_145_D_SENSOR_CFG_SET:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
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
                                rt_mutex_release(&mt_sensor);
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
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    rt_mutex_take(&mt_sensor, 10);
                    if (p_sensor_cb)
                    {
                        for (i=0;i<p_sensor_cb->sensor_num;i++)
                        {
                            sensor_ID_temp[i].ID = p_sensor_cb->sensor[i].ID;
                            sensor_ID_temp[i].out_type = p_sensor_cb->sensor[i].out_type;
                        }
                        
                        UITD_sensor_ID_check_ack(p_sensor_cb->handler, pkt, &sensor_ID_temp[0], p_sensor_cb->sensor_num);
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
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    handler->output.num = pkt->data_num;
                    rt_memcpy(&handler->output.data , pkt->buf+2, sizeof(s_output_data)*handler->output.num);
                    
                    for (i=0;i<handler->output.num;i++)
                    {
                        SYS_log( SYS_DEBUG_INFO, ("Server pushed a output control CMD %d\n", i));
                        SYS_log( SYS_DEBUG_INFO, ("Output ID: %d, action: %d, delay: %d\n", 
                                    handler->output.data[i].ID, 
                                    handler->output.data[i].action, 
                                    handler->output.data[i].delay));
                    }
                    handler->output.valid = 1;
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_151_D_OUTPUT_SEQ_ACTION:
                    // Song: TODO: not support yet.
                
                    UITD_deny(handler, pkt);
                    break;
                case JS_TYPE_152_D_OUTPUT_CHECK:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    handler->output.num = pkt->data_num;
                    rt_memcpy(&handler->output.ID[0], pkt->buf+2, sizeof(handler->output.ID[0]) * handler->output.num);
                    
//                    if (p_PRO_output_ctrl_cb)
//                    {
//                        rt_mutex_take(&mt_PRO_output, 10);
//                        if (p_PRO_output_ctrl_cb)
//                        {
//                            if (handler->output.ID[0] == 0) // check all channal.
//                            {
//                                
//                                for (i=0;i<p_PRO_output_ctrl_cb->output_num;i++)
//                                {
//                                    rt_memcpy(&handler->output.data[i].ID, (void *)&p_PRO_output_ctrl_cb->output[i].ID, sizeof(handler->output.data[0].ID));
//                                    rt_memcpy(&handler->output.data[i].action, (void *)&p_PRO_output_ctrl_cb->output[i].action, sizeof(handler->output.data[0].action));
//                                    //rt_memcpy(&handler->output.data[i].delay, (void *)&p_PRO_output_ctrl_cb->output[i].delay, sizeof(handler->output.data[0].delay));
//                                    handler->output.data[i].delay = p_PRO_output_ctrl_cb->output[i].delay;
//                                }
//                                UITD_output_ctrl_check_mult_ack(p_PRO_output_ctrl_cb->handler, pkt, &handler->output.data[0], p_PRO_output_ctrl_cb->output_num);
//                            }
//                            else
//                            {   
//                                
//                                for (j=0;j<handler->output.num;j++)
//                                {
//                                    for (i=0;i<p_PRO_output_ctrl_cb->output_num;i++)
//                                    {
//                                        if (p_PRO_output_ctrl_cb->output[i].ID == handler->output.ID[j])
//                                        {
//                                            rt_memcpy(&handler->output.data[j].ID, (void *)&p_PRO_output_ctrl_cb->output[i].ID, sizeof(handler->output.data[0].ID));
//                                            rt_memcpy(&handler->output.data[j].action, (void *)&p_PRO_output_ctrl_cb->output[i].action, sizeof(handler->output.data[0].action));
//                                            //rt_memcpy(&handler->output.data[j].delay, (void *)&p_PRO_output_ctrl_cb->output[i].delay, sizeof(handler->output.data[0].delay));
//                                            handler->output.data[j].delay = p_PRO_output_ctrl_cb->output[i].delay;
//                                            break;
//                                        }
//                                    }
//                                }
//                                UITD_output_ctrl_check_mult_ack(p_PRO_output_ctrl_cb->handler, pkt, &handler->output.data[0], handler->output.num);
//                            }                        
//                        }
//                        rt_mutex_release(&mt_PRO_output);
//                    }
                    if (p_output_ctrl_cb)
                    {
                        rt_mutex_take(&mt_output, 10);
                        if (p_output_ctrl_cb)
                        {
                            if (handler->output.ID[0] == 0) // check all channal.
                            {
                                
                                for (i=0;i<p_output_ctrl_cb->output_num;i++)
                                {
                                    rt_memcpy(&handler->output.data[i].ID, (void *)&p_output_ctrl_cb->output[i].ID, sizeof(handler->output.data[0].ID));
                                    rt_memcpy(&handler->output.data[i].action, (void *)&p_output_ctrl_cb->output[i].action, sizeof(handler->output.data[0].action));
                                    //rt_memcpy(&handler->output.data[i].delay, (void *)&p_output_ctrl_cb->output[i].delay, sizeof(handler->output.data[0].delay));
                                    handler->output.data[i].delay = p_output_ctrl_cb->output[i].delay;
                                }
                                UITD_output_ctrl_check_mult_ack(p_output_ctrl_cb->handler, pkt, &handler->output.data[0], p_output_ctrl_cb->output_num);
                            }
                            else
                            {   
                                
                                for (j=0;j<handler->output.num;j++)
                                {
                                    for (i=0;i<p_output_ctrl_cb->output_num;i++)
                                    {
                                        if (p_output_ctrl_cb->output[i].ID == handler->output.ID[j])
                                        {
                                            rt_memcpy(&handler->output.data[j].ID, (void *)&p_output_ctrl_cb->output[i].ID, sizeof(handler->output.data[0].ID));
                                            rt_memcpy(&handler->output.data[j].action, (void *)&p_output_ctrl_cb->output[i].action, sizeof(handler->output.data[0].action));
                                            //rt_memcpy(&handler->output.data[j].delay, (void *)&p_output_ctrl_cb->output[i].delay, sizeof(handler->output.data[0].delay));
                                            handler->output.data[j].delay = p_output_ctrl_cb->output[i].delay;
                                            break;
                                        }
                                    }
                                }
                                UITD_output_ctrl_check_mult_ack(p_output_ctrl_cb->handler, pkt, &handler->output.data[0], handler->output.num);
                            }                        
                        }
                        rt_mutex_release(&mt_output);
                    }
                    break;
                default:
                    break;
            }
            break;
        case JS_CMD_INPUT_REPORT:
            switch (pkt->data_type)
            {
                case JS_TYPE_161_R_INPUT_STATE_CHECK:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    handler->input.num = pkt->data_num;
                    rt_memcpy(handler->input.ID, pkt->buf+2, 1*handler->input.num);
                    
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        if (handler->input.ID[0] == 0) // check all channal.
                        {
                            p_IO_input = rt_malloc(sizeof(s_IO_input_report) * p_IO_input_cb->IO_num);
                            if (p_IO_input == NULL)
                            {
                                rt_mutex_release(&mt_IO_input);
                                break;
                            }                            
                            
                            for (i=0;i<p_IO_input_cb->IO_num;i++)
                            {
                                rt_memcpy(&p_IO_input[i].ID, (void *)&p_IO_input_cb->state[i].ID, sizeof(p_IO_input[i].ID));
                                rt_memcpy(&p_IO_input[i].state, (void *)&p_IO_input_cb->state[i].state, sizeof(p_IO_input[i].state));
                            }
                            UITD_IO_state_mult_ack(p_IO_input_cb->handler, pkt, p_IO_input, p_IO_input_cb->IO_num);
                            rt_free(p_IO_input);
                        }
                        else
                        {   
                            
                            p_IO_input = rt_malloc(sizeof(s_IO_input_report) * handler->input.num);
                            if (p_IO_input == NULL)
                            {
                                rt_mutex_release(&mt_IO_input);
                                break;
                            } 
                            
                            for (j=0;j<handler->input.num;j++)
                            {
                                for (i=0;i<p_IO_input_cb->IO_num;i++)
                                {
                                    if (p_IO_input_cb->state[i].ID == handler->input.ID[j])
                                    {
                                        rt_memcpy(&p_IO_input[j].ID, (void *)&p_IO_input_cb->state[i].ID, sizeof(p_IO_input[i].ID));
                                        rt_memcpy(&p_IO_input[j].state, (void *)&p_IO_input_cb->state[i].state, sizeof(p_IO_input[i].state));
                                        break;
                                    }
                                }
    //                            if (i == p_IO_input_cb->IO_num)
    //                            {
    //                                UITD_deny(handler, pkt);
    //                                
    //                            }
                            }
                            UITD_IO_state_mult_ack(p_IO_input_cb->handler, pkt, p_IO_input, handler->input.num);
                            rt_free(p_IO_input);
                        }
                    }
//                    if (p_IO_input_cb)
//                    {
//                        for (i=0;i<p_IO_input_cb->IO_num;i++)
//                        {
//                            if (p_IO_input_cb->state[i].ID == *(uint8_t *)(pkt->buf+2))
//                            {
//                                UITD_IO_state_ack(p_IO_input_cb->handler, pkt, p_IO_input_cb->state[i].ID, p_IO_input_cb->state[i].state);
//                                rt_mutex_release(&mt_IO_input);
//                                break;
//                            }
//                        }
//                        if (i == p_IO_input_cb->IO_num)
//                        {
//                            UITD_deny(handler, pkt);
//                        }
//                    }
                    rt_mutex_release(&mt_IO_input);
                    break;
                case JS_TYPE_163_D_INPUT_CFG_SET:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    handler->input.num = pkt->data_num;
                    rt_memcpy(&handler->input.cfg , pkt->buf+2, sizeof(s_IO_input_cfg_report) * handler->input.num); 
                    
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        if (handler->input.cfg[0].ID == 0) // set all channal configuration.
                        {
                            for (i=0;i<p_IO_input_cb->IO_num;i++)
                            {
                                rt_memcpy((void *)&p_IO_input_cb->state[i].state_trig, &handler->input.cfg[0].trig, sizeof(handler->input.cfg[0].trig));
                                //rt_memcpy((void *)&p_IO_input_cb->state[i].period, &handler->input.cfg[0].period, sizeof(handler->input.cfg[0].period));
                                p_IO_input_cb->state[i].period = handler->input.cfg[0].period;
                            }
                            //UITD_IO_state_cfg_mult_ack(p_IO_input_cb->handler, pkt, &handler->input.cfg[0], p_IO_input_cb->IO_num);
                        }
                        else
                        {   
                            
                            for (j=0;j<handler->input.num;j++)
                            {
                                for (i=0;i<p_IO_input_cb->IO_num;i++)
                                {
                                    if (p_IO_input_cb->state[i].ID == handler->input.cfg[j].ID)
                                    {
                                        rt_memcpy((void *)&p_IO_input_cb->state[i].state_trig, &handler->input.cfg[0].trig, sizeof(handler->input.cfg[0].trig));
                                        //rt_memcpy((void *)&p_IO_input_cb->state[i].period, &handler->input.cfg[0].period, sizeof(handler->input.cfg[0].period));
                                        p_IO_input_cb->state[i].period = handler->input.cfg[0].period;
                                        break;
                                    }
                                }
                            }
                        }
                        

                    }
                    rt_mutex_release(&mt_IO_input);
                    UITD_affirm(handler, pkt);
                    break;
                case JS_TYPE_164_R_INPUT_CFG_CHECK:
                    if (UITD_CMD_if_support(sys_config.dev_type, pkt->data_type) < 0)
                    {
                        UITD_deny(handler, pkt);
                        break;
                    }
                    
                    handler->input.num = pkt->data_num;
                    rt_memcpy(&handler->input.ID[0] , pkt->buf+2, sizeof(handler->input.ID[0]) * handler->input.num); 
                    
                    rt_mutex_take(&mt_IO_input, 10);
                    if (p_IO_input_cb)
                    {
                        if (handler->input.ID[0] == 0) // check all channel's configuration.
                        {
                            for (i=0;i<p_IO_input_cb->IO_num;i++)
                            {
                                rt_memcpy(&handler->input.cfg[i].ID, (void *)&p_IO_input_cb->state[i].ID, sizeof(handler->input.cfg[0].ID));
                                rt_memcpy(&handler->input.cfg[i].trig, (void *)&p_IO_input_cb->state[i].state_trig, sizeof(handler->input.cfg[0].trig));
                                // rt_memcpy(&handler->input.cfg[i].period, (void *)&p_IO_input_cb->state[i].period , sizeof(handler->input.cfg[0].period));
                                handler->input.cfg[i].period = p_IO_input_cb->state[i].period;
                            }
                            UITD_IO_state_cfg_mult_ack(p_IO_input_cb->handler, pkt, &handler->input.cfg[0], p_IO_input_cb->IO_num);
                        }
                        else
                        {   
                            
                            for (j=0;j<handler->input.num;j++)
                            {
                                for (i=0;i<p_IO_input_cb->IO_num;i++)
                                {
                                    if (p_IO_input_cb->state[i].ID == handler->input.cfg[j].ID)
                                    {
                                        rt_memcpy(&handler->input.cfg[j].ID, (void *)&p_IO_input_cb->state[i].ID, sizeof(handler->input.cfg[0].ID));
                                        rt_memcpy(&handler->input.cfg[j].trig, (void *)&p_IO_input_cb->state[i].state_trig, sizeof(handler->input.cfg[0].trig));
                                        //rt_memcpy(&handler->input.cfg[j].period, (void *)&p_IO_input_cb->state[i].period, sizeof(handler->input.cfg[0].period));
                                        handler->input.cfg[j].period = p_IO_input_cb->state[i].period;
                                        break;
                                    }
                                }
                            }
                            UITD_IO_state_cfg_mult_ack(p_IO_input_cb->handler, pkt, &handler->input.cfg[0], handler->input.num);
                        }                        
                        
//                        for (i=0;i<p_IO_input_cb->IO_num;i++)
//                        {
//                            if (p_IO_input_cb->state[i].ID == *(uint8_t *)(pkt->buf+2))
//                            {
//                                UITD_IO_state_cfg_ack(p_IO_input_cb->handler, pkt, 
//                                                        p_IO_input_cb->state[i].ID, 
//                                                        p_IO_input_cb->state[i].state_trig, 
//                                                        p_IO_input_cb->state[i].period);
//                                rt_mutex_release(&mt_IO_input);
//                                break;
//                            }
//                        }
//                        if (i == p_IO_input_cb->IO_num)
//                        {
//                            UITD_deny(handler, pkt);
//                        }
                        
                    }
                    rt_mutex_release(&mt_IO_input);
                    break;
                default:
                    break;
			}
            break;
        default:
			SYS_log( SYS_DEBUG_ERROR, ("Unsupported CMD : %d\n" , pkt->ctrl.cmd));
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
	
	SYS_log_HEX( SYS_DEBUG_DEBUG, ("pkg_send_buf ", handler->send_buf, handler->data_len));
	res = handler->send_data(handler, handler->node[node].socket, handler->send_buf, handler->data_len);
	if (res != handler->data_len)
	{
		SYS_log( SYS_DEBUG_ERROR, ("Net send data failed : %d\n", res));
		return -2;
	}
	
	return 0;
}



int UITD_send_handler(t_server_handler *handler)
{
	int i = 0;
	int res = 0;
    
    if (handler == NULL)
    {
        return -1;
    }
	
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
					SYS_log( SYS_DEBUG_INFO, ("Send pkg success.\n"));
				}
				else
				{
					handler->node[i].node_status = GB_NODE_send_fail;
					SYS_log( SYS_DEBUG_INFO, ("Send pkg failed.\n"));
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
						SYS_log( SYS_DEBUG_DEBUG, ("Receive affirm packet\n"));
						if (handler->node[i].ack_deal != NULL)
						{
							handler->node[i].ack_deal((void *)handler, 0, 1);
						}
					}
					else if (handler->node[i].ack_status == cmd_acked_deny)
					{
						SYS_log( SYS_DEBUG_INFO, ("Receive affirm deny\n"));
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
						SYS_log( SYS_DEBUG_ERROR, ("Send failed !\n"));
						handler->node[i].node_status = GB_NODE_send_fail;
						//continue;
					}
					else
					{
						SYS_log( SYS_DEBUG_DEBUG, ("UITD pkg sending\n"));
						res = UITD_send_pkg(handler, i);
						if (res == 0)
						{
							//handler->node[i].node_status = GB_NODE_send_success;
							
							// Song: set the ack_status to waiting state. start waiting the ack packet.
							SYS_log( SYS_DEBUG_DEBUG, ("Send pkg success. %d , waiting ack packet\n", handler->node[i].retry));
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
									SYS_log( SYS_DEBUG_ERROR, ("Send failed too many times, exit !\n"));
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
//				rt_free(handler->node[i].data);
//			}
//			handler->node[i].node_status = GB_NODE_unused;
//			SYS_log( SYS_DEBUG_INFO, ("UITD pkt send success"));
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
				rt_free(handler->node[i].data);
			}
			SYS_log( SYS_DEBUG_ERROR, ("UITD pkt send failed\n"));
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
				rt_free(handler->node[i].data);
			}
			
			SYS_log( SYS_DEBUG_INFO, ("UITD pkt send success\n"));
			handler->node[i].node_status = GB_NODE_unused;
		}
		else if (handler->node[i].node_status == GB_NODE_send_acked)
		{
			if (handler->node[i].data != NULL)
			{
				rt_free(handler->node[i].data);
			}
			
			SYS_log( SYS_DEBUG_DEBUG, ("UITD pkt received ack packet.\n"));
			handler->node[i].node_status = GB_NODE_unused;
		}
		else
		{
			// unkown status, error report.
			////SYS_log( SYS_DEBUG_INFO, ("Unkown node status !"));
			//return -5;
		}
	}
}

//初始化用
int UITD_service_init(t_server_handler *handler)
{
    uint8_t *p = NULL;
    
    handler->status = UITD_svc_status_idle;
    handler->alive_cnt = 0;
    
    UITD_send_node_init(handler);
    
//    handler->FW.push_FW_flag = 0;
//    handler->FW.return_FW_flag = 0;
//    handler->FW.status = FW_idle;
    
//    p = rt_malloc(FW_PKT_LEN_DEFAULT);
//    if (p == NULL) return -1;
//    
//    handler->FW.data = p;
    
    return 0;
}



int UITD_dl_file_success(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;

    SYS_log( SYS_DEBUG_INFO, ("File %s download success !\n", fw->fw_path));
    return 0;
}


int UITD_dl_file_fail(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;
    
    SYS_log( SYS_DEBUG_INFO, ("File %s download failed !!!\n", fw->fw_path));
    return 0;
}


int UITD_dl_firmware_success(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;
    int res = -1;
    
//    res = close(fw->fw_fd);
//    if (res < 0)
//    {
//        SYS_log( SYS_DEBUG_ERROR, ("Close firmware.bin failed !!!\n"));
//    }
    
    rt_thread_delay(RT_TICK_PER_SECOND*1);
    
//    res = open(fw->fw_path, O_RDONLY, 0);
//    if (res >= 0)
//    {
//        close(res);
//    }
//    else
//    {
//        SYS_log( SYS_DEBUG_ERROR, ("Firmware file can not open !!!\n"));
//    }
    
    //list_thread();
    ////rt_thread_delay(RT_TICK_PER_SECOND*5);
    SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade success, ready to upgrade the firmware, waiting reboot.\n"));
    touch(FW_UPGRADE_FLAG, 1);
    
#if PATCH_FW_UPGRADE_FLAG
    // Set the flag of date sync.
    RTC_WaitForSynchro();
    RTC_WriteBackupRegister(SYS_RTC_FW_UPGRADE_ADDR, 0x5A5A);    

#endif // PATCH_FW_UPGRADE_FLAG
    
    sys_fw_update_reboot = 1;
    
    return 0;
}


int UITD_dl_firmware_fail(void *data)
{
    s_FW_update *fw = (s_FW_update *)data;
    
    SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade retry ! \n"));
    UITD_download_firmware(fw, fw->file_down_success, fw->file_down_fail);
    
    return 0;
}

int file_get_name(uint8_t *path, uint8_t *name)
{
    uint8_t *p = path;
    uint8_t *q = p;
    
    q = strstr(p, "/");
    while (q != NULL)
    {
        p = q+1;
        q = strstr(p, "/");
    }
    
    strcpy(name, p);
    
    return 0;
}


// Download file from server, save it to /download directory.
int UITD_download_file( t_server_handler *handler,
                        s_FW_update *fw,
                        uint8_t *file_path, 
                        pFun_file_down_success file_success, 
                        pFun_file_down_fail file_fail)
{
    uint8_t filename[256] = {0};
    
    // Get file name.
    file_get_name(file_path, filename);
    // Create entire download path which in the device.
    sprintf(fw->fw_path, "%s/%s", DOWN_FILE_DIR, filename);
    // File path while in the server.
    strcpy(fw->data_req.name, file_path);
    
    fw->file_down_success = file_success;
    fw->file_down_fail = file_fail;
    
    UITD_file_req_length_info(handler, file_path);
    //fw->status = FW_init;
    
    return 0;
}    

int UITD_update_file( t_server_handler *handler,
                        s_FW_update *fw,
                        pFun_file_down_success file_success, 
                        pFun_file_down_fail file_fail)
{

    //sprintf(fw->fw_path, "%s/%s", DOWN_FILE_DIR, filename);
    strcpy(fw->fw_path, fw->FW.name);
    
    fw->file_down_success = file_success;
    fw->file_down_fail = file_fail;

    strcpy(fw->data_req.name, fw->FW.name);
                        
    handler->FW.status = FW_init;

    
//    strcpy(fw->data_req.name, filepath);
    
//    UITD_file_req_length_info(handler, filepath);
    
    //fw->status = FW_init;
    
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
    uint8_t ver_main = 0;
    uint8_t ver_sub = 0;
    uint32_t  dev = {0};
    
    sscanf(fw_name, "%*[^'_']_%*[^'_']_%d_%d_%8X", &ver_main, &ver_sub, &dev);
    
    if ((ver_main > main_ver) || ((ver_main == main_ver) && (ver_sub > sub_ver)))
    {
        if (dev & *(uint32_t *)dev_type)
        {
            return 0;
        }
        else 
        {
            return -2;  // This firmware not affect this dev_type
        }
    }
    
    return -1;
}


int UITD_service_handler(t_server_handler *handler)
{
    int i = 0;
    int res = 0;
    t_GB_pkt temp_pkt = {0};

    if (handler == NULL)
    {
        return -1;
    }
    
    if (handler->AP_ctrl.restart)
    {
        handler->AP_ctrl.restart_cnt ++;
        if (handler->AP_ctrl.restart_cnt > RT_TICK_PER_SECOND*5) // waiting at least 5S before restart.
        {
            handler->AP_ctrl.restart_cnt = 0;
            handler->AP_ctrl.restart = 0;
            // Restart the system.
            sys_reboot();
            while(1) rt_thread_delay(10);
        }
    }
    else if (handler->AP_ctrl.shutdown)
    {
        handler->AP_ctrl.shutdown_cnt ++;
        if (handler->AP_ctrl.shutdown_cnt > RT_TICK_PER_SECOND*5) // waiting at least 5S before restart.
        {
            handler->AP_ctrl.shutdown_cnt = 0;
            handler->AP_ctrl.shutdown = 0;
            // TODO: Song: Shutdown the system. 
            while(1);
        }
    }
    
    {
        switch (handler->status)
        {
            case UITD_svc_status_idle:
                handler->alive_cnt = 0;
                
                #ifdef UITD_SERVICE_TESTING
                handler->status = UITD_svc_status_testing;
                #else
            
                if (handler->if_UITD)
                {
                    UITD_send_UITD_inital_status(handler);
                    //UITD_IOT_GW_info_report(handler);
                }
                else
                {
                    UITD_send_UITD_inital_status(handler);
                    //UITD_IOT_GW_info_report(handler);
                }
            
                handler->status = UITD_svc_status_connect;
                #endif // UITD_SERVICE_TESTING
                break;
            case UITD_svc_status_connect:
            
                break;
            case UITD_svc_status_alive:
                if (handler->alive_cnt > SERVER_ALIVE_PERIOD)
                {
                    UITD_send_alive_pkt(handler);
                    SYS_log( SYS_DEBUG_INFO, ("UITD send alive\n"));
                    handler->alive_cnt = 0;
                }
                
    //            // Song: Just for testing.
    //            if (handler->alive_cnt  == 200)
    //            {
    //                UITD_send_smoke_fire_alarm(handler, 1, 0, 1, 2, data_unit_single);
    //            }
    //            if (handler->alive_cnt == 400)
    //            {
    //                UITD_send_smoke_fire_fault(handler, 1, 3, 4, 5, data_unit_single);
    //            }
    //            if (handler->alive_cnt == 600)
    //            {
    //                UITD_send_smoke_fire_disable(handler, 1, 6, 7, 8, data_unit_single);
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
                //handler->status = UITD_svc_status_idle;
                handler->status = UITD_svc_status_stoped;
            
                SYS_log(SYS_DEBUG_ERROR, ("UITD_server socket %d have been disconnected !\n", handler->socket));
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
                
                SYS_log( SYS_DEBUG_INFO, ("Firmware file has been created.\n"));
                
                handler->FW.rec_index = 0;
                handler->FW.rec_pkt_num = 0;
                handler->FW.rec_pkt_len = 0;
                handler->FW.req_index = 0;
                handler->FW.req_pkt_num = (handler->FW.FW.len % FW_PKT_LEN_DEFAULT)? 
                                            (handler->FW.FW.len / FW_PKT_LEN_DEFAULT + 1) : 
                                            (handler->FW.FW.len / FW_PKT_LEN_DEFAULT);
                handler->FW.req_pkt_len = (handler->FW.FW.len / FW_PKT_LEN_DEFAULT)? FW_PKT_LEN_DEFAULT:handler->FW.FW.len;

                //strcpy(handler->FW.data_req.name, handler->FW.FW.name);
                handler->FW.data_req.len = handler->FW.req_pkt_len;
                handler->FW.data_req.index = 0;
                
                handler->file_trans->store_index = 0;
                handler->file_trans->buf_index = 0;
                handler->file_trans->waiting_num = 0;
                
                handler->FW.status = FW_fetch;
                
                for (i=0;i<handler->file_trans->data_buf_num;i++)
                {
                    if ((handler->file_trans->waiting_num < handler->file_trans->data_buf_num) && (handler->file_trans->data_buf[i].if_valid == 0))
                    {
                        UITD_file_req_pkt(handler, handler->FW.data_req.index);
                        SYS_log( SYS_DEBUG_DEBUG, ("Fetch file data, index: %d\n", handler->FW.data_req.index));
                        handler->FW.data_req.index ++;
                        handler->file_trans->waiting_num ++;
                        if (handler->FW.data_req.index >= handler->FW.req_pkt_num)
                        {
                            break;
                        }
                        handler->FW.data_req.len = (handler->FW.FW.len / FW_PKT_LEN_DEFAULT)? FW_PKT_LEN_DEFAULT:handler->FW.FW.len;
                    }
                }
                
                //UITD_file_req_pkt(handler, handler->FW.data_req.index);
                break;
            case FW_fetch:
                
                
                break;
            case FW_done:
                handler->FW.file_close(handler->FW.fw_fd);
                handler->FW.status = FW_idle;
                file_trans_init(handler->file_trans);
            
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
                file_trans_init(handler->file_trans);
            
                SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade failed !!!!!!!!!\n"));
                if (handler->FW.file_down_fail)
                {
                    handler->FW.file_down_fail(&handler->FW);
                }
                ////handler->FW.FW_upgrade_failed = 1;  //////
                break;
            case FW_stop:
                handler->FW.file_close(handler->FW.fw_fd);
                rm(handler->FW.fw_path);
                file_trans_init(handler->file_trans);
                SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade stoped !!!!!!!!!\n"));
                handler->FW.status = FW_stoped;
                break;
            case FW_stoped:
                break;
            default:
                break;
        }
    }
    
    if (handler->FW.force_file_flag)
    {
        handler->FW.force_file_flag = 0;

        UITD_update_file( handler, &handler->FW,
                                &UITD_dl_file_success, 
                                &UITD_dl_file_fail);
    }

    if (handler->FW.force_FW_flag)
    {
        handler->FW.force_FW_flag = 0;

        UITD_download_firmware( &handler->FW,
                                handler->FW.FW.name,
                                &UITD_dl_firmware_success, 
                                &UITD_dl_firmware_fail);
    }

    
    if (handler->FW.push_FW_flag)
    {
        handler->FW.push_FW_flag = 0;

        res = UITD_firmware_parse(handler->FW.FW.name, sys_main_version, sys_sub_version, &handler->dev_type);
        if (res == 0)
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
        if (res == 0)
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
    
   
    if (p_sensor_cb && (handler->status == UITD_svc_status_alive))
    {
        res = rt_mq_recv(mq_sensor_send, &sensor_data_temp, sizeof(sensor_data_temp), 0);
        if (res == RT_EOK)
        {
            UITD_sensor_upload_rough_data(handler, sensor_data_temp.ID, sensor_data_temp.out_type, sensor_data_temp.data, &sensor_data_temp.timestamp);
        }
        
        if (handler->output.valid)
        {
            handler->output.valid = 0;
            
            for (i=0;i<handler->output.num;i++)
            {
                rt_mq_send(mq_output_ctrl, &handler->output.data[i], sizeof(handler->output.data[0]));
            }
        }
    }
   
    
    if (p_output_ctrl_cb && (handler->status == UITD_svc_status_alive))
    {
        if (handler->output.valid)
        {
            handler->output.valid = 0;
            
            for (i=0;i<handler->output.num;i++)
            {
                rt_mq_send(mq_output_ctrl, &handler->output.data[i], sizeof(handler->output.data[0]));
            }
        }
    }
    
    
    if (p_com_bus_cb && (handler->status == UITD_svc_status_alive))
    {
        if (handler == p_com_bus_cb->handler)
        {
            res = rt_mq_recv(mq_FA_fire, &FA_fire_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {             
                UITD_send_smoke_fire_alarm(handler, FA_fire_temp.port, FA_fire_temp.sys_addr, FA_fire_temp.addr_main, FA_fire_temp.addr_sub, &FA_fire_temp.dev_info, data_unit_single);
            }
			
			//wzy			
			res = rt_mq_recv(mq_FA_elec_fire, &FA_elec_fire_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {               
                UITD_send_smoke_elec_data(handler, FA_elec_fire_temp.port, FA_elec_fire_temp.sys_addr, FA_elec_fire_temp.addr_main, FA_elec_fire_temp.addr_sub, &FA_elec_fire_temp.dev_info, data_unit_single);
            }
            
            res = rt_mq_recv(mq_FA_fault, &FA_fault_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {
                if (FA_fault_temp.valid == 0) // resume to normal status.
                {
                    UITD_send_smoke_fire_resume(handler, FA_fault_temp.port, FA_fault_temp.sys_addr, FA_fault_temp.addr_main, FA_fault_temp.addr_sub, &FA_fault_temp.dev_info, data_unit_single);
                }
                else // fault status.
                {
                    UITD_send_smoke_fire_fault(handler, FA_fault_temp.port, FA_fault_temp.sys_addr, FA_fault_temp.addr_main, FA_fault_temp.addr_sub, &FA_fault_temp.dev_info, data_unit_single);
                }
            }

            res = rt_mq_recv(mq_FA_reset, &FA_reset_temp, sizeof(s_com_bus_R_reset), 0);
            if (res == RT_EOK)
            {
                UITD_send_FA_reset(handler, FA_reset_temp.port, FA_reset_temp.sys_addr);
            }
            
            res = rt_mq_recv(mq_FA_manul_fire, &FA_manul_fire_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {
                UITD_send_manul_fire_alarm(handler, FA_manul_fire_temp.port, FA_manul_fire_temp.sys_addr, FA_manul_fire_temp.addr_main, FA_manul_fire_temp.addr_sub, &FA_manul_fire_temp.dev_info, data_unit_single);
            }

            res = rt_mq_recv(mq_FA_power_off, &FA_power_off_temp, sizeof(s_com_bus_R_power_off), 0);
            if (res == RT_EOK)
            {
                UITD_send_fire_sys_main_power_fault(handler);
            }
        }
        else if (handler == p_com_bus_cb->handler_2)
        {
            res = rt_mq_recv(mq_FA_2_fire, &FA_fire_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {	
                UITD_send_smoke_fire_alarm(handler, FA_fire_temp.port, FA_fire_temp.sys_addr, FA_fire_temp.addr_main, FA_fire_temp.addr_sub, &FA_fire_temp.dev_info, data_unit_single);
            }
            
            res = rt_mq_recv(mq_FA_2_fault, &FA_fault_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {
                UITD_send_smoke_fire_fault(handler, FA_fault_temp.port, FA_fault_temp.sys_addr, FA_fault_temp.addr_main, FA_fault_temp.addr_sub, &FA_fault_temp.dev_info, data_unit_single);
            }

            res = rt_mq_recv(mq_FA_2_reset, &FA_reset_temp, sizeof(s_com_bus_R_reset), 0);
            if (res == RT_EOK)
            {
                UITD_send_FA_reset(handler, FA_reset_temp.port, FA_reset_temp.sys_addr);
            }
            
            res = rt_mq_recv(mq_FA_2_manul_fire, &FA_manul_fire_temp, sizeof(s_com_bus_R_alarm), 0);
            if (res == RT_EOK)
            {
                UITD_send_manul_fire_alarm(handler, FA_manul_fire_temp.port, FA_manul_fire_temp.sys_addr, FA_manul_fire_temp.addr_main, FA_manul_fire_temp.addr_sub, &FA_manul_fire_temp.dev_info, data_unit_single);
            }
            
            res = rt_mq_recv(mq_FA_2_power_off, &FA_power_off_temp, sizeof(s_com_bus_R_power_off), 0);
            if (res == RT_EOK)
            {
                UITD_send_fire_sys_main_power_fault(handler);
            }
        }
    }
    
    if (p_IO_input_cb && (handler->status == UITD_svc_status_alive))
    {
        res = rt_mq_recv(mq_IO_input, &IO_input_temp, sizeof(s_IO_input_report), 0);
        if (res == RT_EOK)
        {
            UITD_IO_state_report(p_IO_input_cb->handler, IO_input_temp.ID, IO_input_temp.state, &IO_input_temp.timestamp);
        }
        
    }
    
    
//    if (handler->FW.FW_upgrade_failed)
//    {
//        handler->FW.FW_upgrade_failed = 0;
//        
//        handler->FW.push_FW_flag = 1;
//        handler->FW.status = FW_idle;
//        SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade retry ! \n"));
//    }
//    
//    if (handler->FW.FW_upgrade_success)
//    {
//        handler->FW.FW_upgrade_success = 0;
//        SYS_log( SYS_DEBUG_INFO, ("Firmware upgrade success, ready to upgrade the firmware, waiting reboot.\n"));
//        touch(FW_UPGRADE_FLAG);
//        sys_fw_update_reboot = 1;
//    }
    
    return 0;
}

e_GB_err UITD_server(t_server_handler *handler)
{
	e_GB_err GB_err = GB_E_OK;
	int res = 0;
    
    if (handler == NULL) 
    {
        return -1;
    }
	
	////SYS_log( SYS_DEBUG_INFO, ("Enter UITD_server "));
//	ring_buf = RingBuffer_create(GB_ROUGH_BUF_MAX);
//	if (ring_buf == NULL)
//	{
//		SYS_log( SYS_DEBUG_INFO, ("Failed to make ringbuffer."));
//		return ;
//	}
//	else
//	{
//		SYS_log( SYS_DEBUG_INFO, ("Successed to make ringbuffer !"));
//	}
//	
//	res = RingBuffer_write(ring_buf, data, data_len);
//	if (res != data_len)
//	{
//		SYS_log( SYS_DEBUG_INFO, ("Failed to write ringbuffer."));
//	}

//	res = RingBuffer_read(ring_buf, data_buf, data_len);
//	if (res != data_len)
//	{
//		SYS_log( SYS_DEBUG_INFO, ("Failed to read ringbuffer."));
//	}
//	else
//	{
//		
//	}
//	
//	RingBuffer_destroy(ring_buf);
//	SYS_log( SYS_DEBUG_INFO, ("Successed to destroy ringbuffer !"));

	////SYS_log( SYS_DEBUG_INFO, ("Ringbuf data number 01 : %d ", RingBuffer_available_data(ring_buf)));

//	res = rt_ringbuffer_put(&handler->ring_buf, data, data_len);
//	if (res != data_len)
//	{
//		SYS_log( SYS_DEBUG_INFO, ("ringbuf write error : %d ", res));
//		return GB_E_BUF_WR;
//	}
	
	////SYS_log( SYS_DEBUG_INFO, ("Ringbuf data number 02 : %d ", RingBuffer_available_data(ring_buf)));
	
	
	while(rt_ringbuffer_data_len(&handler->ring_buf) != 0)
	{
		////SYS_log( SYS_DEBUG_INFO, ("*"));
		res = UITD_parse(handler, &handler->GB_parse);
		if (res == 0) // Parsed a valid packet.
		{
			////SYS_log( SYS_DEBUG_INFO, ("$"));
			////SYS_log( SYS_DEBUG_INFO, ("UITD_parse valid : %d \n", GB_parse.len));
			
			GB_err = UITD_decode(handler, &handler->GB_pkt_rec, handler->GB_parse.buf, handler->GB_parse.len, handler->socket);
			if (GB_err == GB_E_OK)
			{
				UITD_deal(handler, &handler->GB_pkt_rec);
			}
            else
            {
                SYS_log( SYS_DEBUG_ERROR, ("Error code: %d \n", GB_err));
			}
			////TCP_Send_Data(a_new_client_sock_fd, GB_parse.buf, GB_parse.len, MSG_DONTWAIT);
			
			handler->GB_parse.valid = 0;
			handler->GB_parse.len = 0;
		}
		else
		{
			////SYS_log( SYS_DEBUG_INFO, ("#"));
			continue;
		}
		
	
	}
	
	////SYS_log( SYS_DEBUG_INFO, ("Exit UITD_server "));
}




int UITD_CMD_if_support(e_dev_type dev_type, uint8_t CMD)
{
    uint32_t num = 0;
    int i = 0;
    int j = 0;
    
    num = sizeof(device_cmd_list)/sizeof(device_cmd_list[0]);
    
    for (i=0;i<num;i++)
    {
        if (device_cmd_list[i].dev_type == dev_type)
        {
            for (j=0;j<device_cmd_list[i].CMD_list_num;j++)
            {
                if (device_cmd_list[i].CMD_list[j] == CMD)
                {
                    return 0;
                }
            }
        }
    }
    
    return -1;
}
