#include "COM_if.h"
#include "rtthread.h"
#include <rtdevice.h>
#include "platform.h"

#define DEBUG_COM_IF_EN     0


//rt_uint8_t prio_COM_IO = 3;
//rt_thread_t thread_COM_IO;


//rt_uint8_t prio_COM_if = 5;

//rt_thread_t thread_COM_if;


//struct rt_device_pin_mode pin_mode_wifi_shut = {PIN_WIFI_SHUT, PIN_MODE_OUTPUT};
//struct rt_device_pin_mode pin_mode_wifi_rst = {PIN_WIFI_RST, PIN_MODE_OUTPUT};

//struct rt_device_pin_status pin_status_wifi_shut = {PIN_WIFI_SHUT, PIN_HIGH};
//struct rt_device_pin_status pin_status_wifi_rst = {PIN_WIFI_RST, PIN_HIGH};

struct rt_semaphore sem_COM_if_rx;
//struct rt_semaphore sem_UITD_sub_main_tx;



static t_COM_pkt *pkt_ctrl = NULL;



t_COM_parse COM_parse = {0};

t_COM_pkt COM_pkt_send = {0};

uint8_t COM_pkt_buff[COM_PACKET_LEN_MAX] = {0};
uint8_t COM_rough_buff[COM_ROUGH_BUF_MAX] = {0};


rt_mailbox_t mail_COM_send;


t_COM_send COM_PKT_SEND_AFFIRM = {
.ctrl = {
        {0,0},
        COM_CMD_AFFIRM,
        0x00
    },
.buf = NULL,
.checksum = 0x00,
.if_malloc = 0,
.if_none_data = 1,
.data_type = 0x00,  
.data_num = 0x00,
.data_len = 0x0000    
};

t_COM_send COM_PKT_SEND_DENY = {
.ctrl = {
        {0,0},
        COM_CMD_DENY,
        0x00
    },
.buf = NULL,
.checksum = 0x00,
.if_malloc = 0,
.if_none_data = 1,
.data_type = 0x00,  
.data_num = 0x00,
.data_len = 0x0000    
};

//struct rt_ringbuffer COM_if_ringbuf;
//uint8_t COM_if_ring[UITD_COM_IF_RING_SIZE] = {0};


uint8_t checksum_uint8(void *data, uint32_t len)
{
    uint32_t sum = 0;
    int i = 0;
    uint8_t *p = data;
    
    for (i=0;i<len;i++)
    {
        sum += p[i];
    }
    
    return (uint8_t)(sum%0x0100);

}











int COM_send_data(t_COM_send *send, uint32_t timeout)
{

    return rt_mb_send_wait(mail_COM_send, (rt_uint32_t)send, timeout);
    
}

int COM_check_send_idle(void)
{
    if (pkt_ctrl->status == COM_idle) return 1;
    else return 0;
}


// Song: you need pass these parameter of t_COM_send: ctrl.cmd, 
//       if_none_data {if 0, also need : data_type, data_num, buf, if_malloc, data_len}
//       Before send data, you must check if the COM_idle by function COM_check_send_idle().
int COM_send_CMD_data(t_COM_send *pkt , uint32_t timeout)
{
    t_COM_send *send = NULL;
    uint32_t sum = 0;
    
    if (pkt_ctrl->status != COM_idle) return -1;
    
    send = rt_malloc(sizeof(t_COM_send));
    if (send == NULL)
    {
        return -1;
    }
    
    rt_memcpy(send, pkt, sizeof(t_COM_send));
    
    send->ctrl.ver.ver_main = COM_VERSION_MAIN;
    send->ctrl.ver.ver_user = COM_VERSION_USER;
    
    send->ctrl.data_len = (send->if_none_data == 0) ? send->data_len + COM_PACKET_DATA_HEAD_LEN : 0;
    
    sum += checksum_uint8(&send->ctrl, sizeof(send->ctrl));
    if (send->if_none_data == 0)
    {
        sum += send->data_type;
        sum += send->data_num;
        
        if (send->data_len)
        {
            sum += checksum_uint8(send->buf, send->data_len);
        }
    }
    
    send->checksum = sum%256;
    pkt_ctrl->status = COM_sending;
    pkt_ctrl->timer = 0;
    pkt_ctrl->timeout = timeout;
    
    send->pkt = pkt_ctrl;
    
    return COM_send_data(send, COM_SEND_DATA_MAILBOX_TIMEOUT);
}


int COM_send_CMD_affirm(void)
{
    return COM_send_CMD_data(&COM_PKT_SEND_AFFIRM, 0);
}

int COM_send_CMD_deny(void)
{
    return COM_send_CMD_data(&COM_PKT_SEND_DENY, 0);
}



int COM_if_parser(t_COM_parse *parse)
{
    //uint8_t data = 0x00;
    uint8_t data_temp = 0x00;
    int res = 0;
    t_COM_ctrl_unit *ctrl = NULL;
    
    while(1)
    {
        res = rt_device_read(pkt_ctrl->dev, 0, &data_temp, 1);
        if (res < 1)
        {
            return -1;
        }

		switch (parse->status)
		{
			case parse_idle:
				if (data_temp == COM_PKT_HEAD_CHAR)
				{
					parse->status = parse_head;
					parse->len = 0;
					parse->valid = 0;
				}
				else
				{
					
				}
				break;
			case parse_head:
				if (data_temp == COM_PKT_HEAD_CHAR)
				{
					parse->status = parse_data;
				}
				else
				{
					parse->status = parse_idle;
				}
				
				break;
			case parse_data:
				if (data_temp == COM_PKT_TAIL_CHAR)
				{
					parse->status = parse_tail;
					
				}
				else
				{
					parse->buf[parse->len] = data_temp;
					parse->len ++;
					if (parse->len > COM_PACKET_LEN_MAX)
					{
						parse->status = parse_idle;
					}
				}
				break;
			case parse_tail:
				if (data_temp == COM_PKT_TAIL_CHAR)
				{
                    // Checking the packet lenght, if too short, discard.
                    if (parse->len < COM_PACKET_LEN_MIN)
                    {
                        parse->status = parse_idle;
                        break;
                    }
                    
                    // If data body lenght is not equal the actual packet, go on receiving the data.
                    ctrl =  (t_COM_ctrl_unit *)parse->buf;
                    if (ctrl->data_len != (parse->len - COM_PACKET_NO_DATA_LEN))
                    {
                        parse->buf[parse->len] = COM_PKT_TAIL_CHAR;
                        parse->len ++;
                        parse->buf[parse->len] = COM_PKT_TAIL_CHAR;
                        parse->len ++;
                        if (parse->len > COM_PACKET_LEN_MAX)
                        {
                            parse->status = parse_idle;
                            break;
                        }
                        parse->status = parse_data;
                        break;
                    }
                    
                    // Packet is parsed correctly, exit.
					parse->valid = 1;
					////rt_kprintf("Parse valid ");
					parse->status = parse_idle;
					return 0;
				}
				else
				{
					parse->buf[parse->len] = COM_PKT_TAIL_CHAR;
					parse->len ++;
					parse->buf[parse->len] = data_temp;
					parse->len ++;
					if (parse->len > COM_PACKET_LEN_MAX)
					{
						parse->status = parse_idle;
                        break;
					}
					parse->status = parse_data;
				}
				
				break;
			default:
				
				break;
		}
        
    
    }
    
    
    
    return 0;
}


e_COM_err COM_decode(uint8_t *data, uint32_t data_len)
{
	t_COM_ctrl_unit *ctrl = NULL;
	int k=0;
	uint32_t checksum = 0;
	uint8_t *p_ctrl = NULL;
	
	if (data_len < COM_PACKET_LEN_MIN)
	{
		
		return COM_E_LEN_LESS;
	}
	if (data_len > COM_PACKET_LEN_MAX)
	{
		
		return COM_E_LEN_OVER;
	}
	if (data_len)
	{
		rt_memcpy(COM_pkt_buff, data, data_len);
	}
	////ctrl = &pkt->ctrl;
	
	ctrl = (t_COM_ctrl_unit *)COM_pkt_buff;
	
	if (ctrl->data_len > COM_DATA_LEN_MAX)
	{
	
		return COM_E_ARG_ERR;
	}
	
	if ( (ctrl->data_len + COM_CTRL_LEN + COM_CHECKSUM_LEN) != data_len)
	{
	
		return COM_E_LEN_ERR;
	}
	else
	{
		rt_memcpy(&pkt_ctrl->ctrl, COM_pkt_buff, COM_CTRL_LEN);
		if (ctrl->data_len >= 2)
		{
			memcpy(&pkt_ctrl->buf, &COM_pkt_buff[COM_CTRL_LEN], ctrl->data_len);
			pkt_ctrl->data_type = pkt_ctrl->buf[0];
			pkt_ctrl->data_num = pkt_ctrl->buf[1];
			
		}
		else
		{
			pkt_ctrl->data_type = COM_TYPE_RESERVE;
			pkt_ctrl->data_num = 0;
		}
		
		pkt_ctrl->checksum = COM_pkt_buff[COM_CTRL_LEN + pkt_ctrl->ctrl.data_len];
		checksum = 0;
		p_ctrl = (uint8_t *)&pkt_ctrl->ctrl;
		for (k=0;k<COM_CTRL_LEN;k++)
		{
			checksum += p_ctrl[k];
		}
		for (k=0;k<pkt_ctrl->ctrl.data_len;k++)
		{
			checksum += pkt_ctrl->buf[k];
		}
		checksum %= 256;
		if (checksum != pkt_ctrl->checksum)
		{
			return COM_E_CHECKSUM;
		}
		
		//pkt->socket = socket;
		
	}
	
#if  DEBUG_COM_IF_EN
	
	//rt_kprintf("SN      : %d \n", ctrl->SN);
	
	rt_kprintf("Ver_main: %d \n", ctrl->ver.ver_main);
	rt_kprintf("Ver_user: %d \n", ctrl->ver.ver_user);
	
//	rt_kprintf("TimeStamp: %d-%d-%d %d:%d:%d \n", 
//									ctrl->TS.year + 2000, 
//									ctrl->TS.month, 
//									ctrl->TS.day, 
//									ctrl->TS.hour, 
//									ctrl->TS.min, 
//									ctrl->TS.sec);
//	rt_kprintf("IP src : %d-%d-%d-%d-%d-%d \n", 
//									ctrl->src.addr[5], 
//									ctrl->src.addr[4], 
//									ctrl->src.addr[3], 
//									ctrl->src.addr[2], 
//									ctrl->src.addr[1], 
//									ctrl->src.addr[0]);

//	rt_kprintf("IP des : %d-%d-%d-%d-%d-%d \n", 
//									ctrl->des.addr[5], 
//									ctrl->des.addr[4], 
//									ctrl->des.addr[3], 
//									ctrl->des.addr[2], 
//									ctrl->des.addr[1], 
//									ctrl->des.addr[0]);
	
	rt_kprintf("CMD : %d \n", ctrl->cmd);
	
	rt_kprintf("Checksum : %d , 0x%02X\n", pkt_ctrl->checksum, pkt_ctrl->checksum);
	
	rt_kprintf("Data_len : %d \n", ctrl->data_len);
	UITD_HEX_DUMP("Data_buf", pkt->buf, pkt->ctrl.data_len);
#endif // DEBUG_COM_IF_EN

	return COM_E_OK;
}


int COM_deal(void)
{
	int res = 0;
	int node = 0;
	
	switch (pkt_ctrl->ctrl.cmd)
	{
		case COM_CMD_CTRL:
            pkt_ctrl->deal_ctrl(pkt_ctrl->data_type);

			break;
		case COM_CMD_SEND_DATA:
            pkt_ctrl->deal_send_data(pkt_ctrl->data_type);
			
			break;
		case COM_CMD_AFFIRM:
            if (pkt_ctrl->status == COM_sended)
            {
                if (pkt_ctrl->send_affirmed != NULL)
                {
                    pkt_ctrl->send_affirmed(pkt_ctrl);
                    pkt_ctrl->send_affirmed = NULL;
                }
                pkt_ctrl->status = COM_rec_ack;
            }

            break;
		case COM_CMD_REQUIRE:
            pkt_ctrl->deal_require(pkt_ctrl->data_type);
			
			break;
		case COM_CMD_ACK:
            if (pkt_ctrl->status == COM_sended)
            {
                if (pkt_ctrl->require_acked != NULL)
                {
                    pkt_ctrl->require_acked(pkt_ctrl);
                    pkt_ctrl->require_acked = NULL;
                }
                pkt_ctrl->status = COM_rec_ack;
            }
            
            break;
		case COM_CMD_DENY:
            if (pkt_ctrl->status == COM_sended)
            {
                if (pkt_ctrl->send_denyed != NULL)
                {
                    pkt_ctrl->send_denyed(pkt_ctrl);
                    pkt_ctrl->send_denyed = NULL;
                }
                pkt_ctrl->status = COM_rec_ack;
            }
			break;
			
		default:
			
			rt_kprintf("Unsupported CMD : %d" , pkt_ctrl->ctrl.cmd);
			break;
	}	
	
}



int COM_if_init(t_COM_pkt *pkt, rt_device_t dev, fun_deal_require require, fun_deal_ctrl ctrl, fun_deal_send_data send_data)
{
    rt_memset(pkt, 0x00, sizeof(t_COM_pkt));
    
    
    if (pkt == NULL)
    {
        rt_kprintf("Not valid COM_pkt !\n");
        return -1;
    }
    pkt_ctrl = pkt;
    
    pkt_ctrl->status = COM_idle;
    pkt_ctrl->timeout = COM_ACK_TIMEOUT_DEFAULT;
    
    
    if (require == NULL)
    {
        rt_kprintf("Not valid deal_require function\n");
        return -2;
    }
    pkt->deal_require = require;

    if (ctrl == NULL)
    {
        rt_kprintf("Not valid deal_ctrl function\n");
        return -3;
    }
    pkt->deal_ctrl = ctrl;
    
    if (send_data == NULL)
    {
        rt_kprintf("Not valid deal_send_data function\n");
        return -3;
    }
    pkt->deal_send_data = send_data;    
    
    if (dev == NULL)
    {
        rt_kprintf("Not valid rt_device \n");
        return -4;
    }
    pkt->dev = dev;
    
    return 0;
}

int COM_if_CMD_send_affrim_cb(fun_send_affirmed affirmed)
{
    pkt_ctrl->send_affirmed = affirmed;
    return 0;
}


int COM_if_CMD_send_denyed_cb(fun_send_denyed denyed)
{
    pkt_ctrl->send_denyed = denyed;
    return 0;
}


int COM_if_require_acked_cb(fun_require_acked acked)
{
    pkt_ctrl->require_acked = acked;
    return 0;
}


int COM_if_determine_wait_ACK(void)
{
    pkt_ctrl->status = COM_idle;
    pkt_ctrl->timer = 0;
    
    pkt_ctrl->ack_timeout = NULL;
    pkt_ctrl->send_affirmed = NULL;
    pkt_ctrl->send_denyed = NULL;

    return 0;
}

int COM_if_ack_timeout_cb(fun_ack_timeout ack_timeout)
{
    pkt_ctrl->ack_timeout = ack_timeout;
    return 0;
}

int COM_if_rec_handler()
{
    int res = 0;
    e_COM_err COM_err = COM_E_OK;
    
    
    switch (pkt_ctrl->status)
    {
        case COM_idle:
            break;
        case COM_sending:
            if (pkt_ctrl->timeout)
            {
                pkt_ctrl->timer ++;
                if (pkt_ctrl->timer >= pkt_ctrl->timeout)
                {
                    pkt_ctrl->status = COM_ack_timeout;
                }
            }
            break;
        case COM_sended:
            if (pkt_ctrl->timeout)
            {
                pkt_ctrl->timer ++;
                if (pkt_ctrl->timer >= pkt_ctrl->timeout)
                {
                    pkt_ctrl->status = COM_ack_timeout;
                }
            }
            break;
        case COM_rec_ack:
            pkt_ctrl->timer = 0;
            pkt_ctrl->status = COM_idle;
            break;
        //NOTE: If set the parameter timeout as 0 , sended CMD will NOT wait for ACK.
        case COM_ack_timeout:  
            if (pkt_ctrl->ack_timeout != NULL) pkt_ctrl->ack_timeout(pkt_ctrl);
            pkt_ctrl->timer = 0;
            pkt_ctrl->status = COM_idle;
            break;
        case COM_send_fault:
            
            // Song: send sequence is wrong. TODO.
            break;
        default:
            break;
    }
    
    res = COM_if_parser(&COM_parse);
    if (res == 0) // Parsed a valid packet.
    {
        COM_err = COM_decode(COM_parse.buf, COM_parse.len);
        if (COM_err == COM_E_OK)
        {
            COM_deal();
        }
        else if (COM_err == COM_E_CHECKSUM)
        {
            if ((pkt_ctrl->ctrl.cmd == COM_CMD_CTRL) || (pkt_ctrl->ctrl.cmd == COM_CMD_REQUIRE))
            {
                COM_send_CMD_deny();
            }
        }
        
        #if DEBUG_COM_IF_EN
        rt_kprintf("Error code: %d \n", COM_err);
        #endif // DEBUG_COM_IF_EN
    }
    
    
}

int COM_if_send_handler(void)
{
    rt_err_t err = RT_EOK;
    rt_uint32_t p_pkt = 0;
    t_COM_send *send = NULL;
    
    while(1)
    {
        err = rt_mb_recv(mail_COM_send, &p_pkt, RT_WAITING_FOREVER);
        if (err != RT_EOK)
        {
            break;
        }
        
        send = (t_COM_send *)p_pkt;
        
        if (send->pkt->status == COM_sending)
        {
            send->pkt->status = COM_sended;
        }
        else
        {
            send->pkt->status = COM_send_fault;
            break;
        }
        
        rt_device_write(pkt_ctrl->dev, 0, COM_PKT_HEAD, COM_PKT_HEAD_LEN);
        rt_device_write(pkt_ctrl->dev, 0, &((send)->ctrl), sizeof(t_COM_ctrl_unit));
////        if ((*send)->)
        if ((send)->if_none_data == 0)
        {
            rt_device_write(pkt_ctrl->dev, 0, &(send)->data_type, 1);
            rt_device_write(pkt_ctrl->dev, 0, &(send)->data_num, 1);
            if ((send)->data_len != 0)
            {
                rt_device_write(pkt_ctrl->dev, 0, (send)->buf, (send)->data_len);
            }
        }
        else
        {
            
        }
        rt_device_write(pkt_ctrl->dev, 0, &(send)->checksum, COM_CHECKSUM_LEN);
        rt_device_write(pkt_ctrl->dev, 0, COM_PKT_TAIL, COM_PKT_TAIL_LEN);
        
        if (send->pkt->timeout == 0)
        {
            send->pkt->status = COM_rec_ack;
        }
        
        if ((send)->if_malloc)
        {
            rt_free((send)->buf);
        }
        rt_free(send);
        
        //rt_kprintf("*\n");
    }

    return 0;
}























