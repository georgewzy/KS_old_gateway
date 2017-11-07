// ¸³°²JB-QBH-FS5101W

#include "fire_alarm.h"
#include "sys_config.h"
//#include <stdio.h>
#include <rtdevice.h>
#include "board.h"

//#define FAC_DBG_EN

#define FAC_MSG_FILETER_EN	0


#define FAC_MSG_BUF_NUM		100

struct rt_device * device_FAC; 


static uint32_t sys_addr = 0;
static uint32_t addr_area = 0;
static uint32_t  addr_line = 0;
static uint32_t  dev_ID = 0;
static uint32_t tmp = 0;
static uint8_t *p_data_meta = NULL;
static uint8_t  data_meta_num = 0;


#define FAC_MSG_HEAD			0x01
#define FAC_MSG_TAIL			0x00

#define FAC_MSG_CMD_ALARM			(u8)0xC3
#define FAC_MSG_CMD_FAULT			(u8)0xC2
#define FAC_MSG_CMD_RESET			(u8)0xA1


#define FAC_MSG_TYPE_NONE  		0
#define FAC_MSG_TYPE_ALARM		1
#define FAC_MSG_TYPE_FAULT		2
#define FAC_MSG_TYPE_RESET		3
#define FAC_MSG_TYPE_SHIELD		4
#define FAC_MSG_TYPE_FEEDBACK	5
#define FAC_MSG_TYPE_OUTING		6



static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	
static uint32_t	FAC_msg_len = 0;
const static uint8_t fac_send_data[19] = {0x01,0x54,0x31,0x02,0x57,0x31,0x31,0x31,0x00,0x00,0x00,0x00,0x4E,0x00,0x00,0x01,0x01,0x03,0x4F};



static u8 reply_data[8] = {0xF0,0x00, 0x03, 0xA2, 0x00, 0x00, 0x00, 0x00};


#define MAX_HP_BUF	20
typedef struct _HADDSHAKE_PRO
{
	uint8_t nStatus;
	uint8_t nBuf[MAX_HP_BUF];
	uint8_t nLen;
}HandshakePro;

#define THREAD_HAND_SHAKE_DATA_STACK_SIZE     4096

static rt_uint8_t prio_HandShake = 9;
static rt_thread_t thread_HandShake;
static HandshakePro m_HandShakePro; 




static uu_status = 0;

s_com_bus_cfg FAC_com_cfg = {1200, 7, PARITY_EVEN, 1};

s_com_bus_R_alarm *FAC_msg_buf = NULL;
int FAC_msg_buf_num = 0;

int FAC_unusual_rx_handler(s_com_bus_cb * cb);
void rt_thread_entry_HandShake(void* parameter);

u16 crc16(u16 crc, const u8 *data, u32 len);




int FAC_unusual_rx_init(s_com_bus_cb * cb)
{
	
	rt_memcpy(&cb->FAC_config.cfg, &FAC_com_cfg, sizeof(s_com_bus_cfg));
	//cb->FA_listen = 1;
	
	#if FAC_MSG_FILETER_EN
	FAC_msg_buf = rt_malloc(sizeof(s_com_bus_R_alarm)*FAC_MSG_BUF_NUM);
	if (FAC_msg_buf == NULL)
	{
		SYS_log(SYS_DEBUG_ERROR, ("FAC_msg_buf malloc failed ! \n"));
		while(1)
		{
			rt_thread_delay(5);	
		}
	}
	#endif // FAC_MSG_FILETER_EN
	
	
	thread_HandShake = rt_thread_create("HAND_SHAKE",
										rt_thread_entry_HandShake,
										(void*)cb,
										THREAD_HAND_SHAKE_DATA_STACK_SIZE,
										prio_HandShake,
										60);
	if (thread_HandShake != RT_NULL)
	{        
		rt_thread_startup(thread_HandShake);     
	}
	
	
	return 0;
	
}

int atoi1(uint8_t s[])  
{  
    int i;  
    int n = 0;  
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)  
    {  
        n = 10 * n + (s[i] - '0');  
    }  
    return n;  
} 


void rt_thread_entry_HandShake(void* parameter)
{

	s_com_bus_cb *cb1;
	cb1 = (s_com_bus_cb*)parameter;
	
	while(1)
	{
		
		// rt_memcpy(p_com_bus_cb->parse.buf, fac_send_data, 19);
		// p_com_bus_cb->parse.len = 19;
	
		// rt_device_write(p_com_bus_cb->dev, 0, fac_send_data, 19);
		// p_com_bus_cb->parse.valid = 1;
		rt_thread_delay(200);
		
    } 			
}



int FAC_StrCLear(uint8_t *vTarget, uint8_t vLen)
{
	uint8_t i = 0;
	
	for (i=0; i<vLen; i++)
	{
		vTarget[i] = 0;
	}
	
	return 0;
}


int FAC_unusual_rx_parser(s_com_bus_cb *cb)
{
 
    uint8_t data_temp = 0x00;
	uint8_t ddd = 0;
    int res = 0;
	u16 crc_val = 0;
	u8 data1[4] = {0x41,0x31,0x43,0x44};
	
	addr_line = ((data1[0]-0x30)<<4)  + (data1[1]-0x30);
	dev_ID = ((data1[2]-0x30)<<4) + (data1[3]-0x30);
	
	
    while(1)
    {
	
        res = rt_device_read(cb->dev, 0, &data_temp, 1);
        if (res < 1)
        {
            return -1;
        }
		
        cb->rec_buf[cb->rec_len] = data_temp&0x7F;
		ddd = data_temp&0x7F;
		
	//	rt_device_write(cb->dev, 0, &ddd, 1);
        cb->rec_len++;
				
		switch (uu_status)
		{
			case 0:
				
				if (ddd == 0x01)
				{
					uu_status = 1;
				}
				else
				{
					cb->rec_len = 0;				
				}
			break;
			case 1:	
				if(ddd == 0x4C) 
				{
					uu_status = 3;
				}
				else
				{
					uu_status = 2;
				}
			break;

			case 2:				
				if (cb->rec_len >= 19)								
				{
					rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
					cb->parse.len = cb->rec_len;
					rt_device_write(cb->dev, 0, cb->rec_buf, cb->rec_len);
					FAC_StrCLear(cb->rec_buf, cb->rec_len);
					
					uu_status = 0;
					cb->rec_len = 0;
					
					cb->parse.valid = 1;

					#ifdef FAC_DBG_EN
					rt_kprintf("msg valid \n");
					#endif // FAC_DBG_EN

					return 0;
				}
			
				
				if (cb->rec_len >= (sizeof(cb->rec_buf) - 2))
				{
					cb->parse.len = 0;
					cb->rec_len = 0;
					uu_status = 0;
				}
														
				break;

			case 3:
				if (cb->rec_len >= 6)								
				{
					rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
					cb->parse.len = cb->rec_len;
					rt_device_write(cb->dev, 0, cb->rec_buf, cb->rec_len);
					FAC_StrCLear(cb->rec_buf, cb->rec_len);
					
					uu_status = 0;
					cb->rec_len = 0;
					
					cb->parse.valid = 1;

					#ifdef FAC_DBG_EN
					rt_kprintf("msg valid \n");
					#endif // FAC_DBG_EN

					return 0;
				}
			
				
				if (cb->rec_len >= (sizeof(cb->rec_buf) - 2))
				{
					cb->parse.len = 0;
					cb->rec_len = 0;
					uu_status = 0;
				}
		
		break;
		case FAC_parse_end:
		
		break;
		default:
			
		break;
	}
}
    
    return -1;		
}






int FAC_msg_parse(s_com_bus_cb * cb, uint8_t *data, uint32_t len)
{

	int i = 0;
	FAC_msg_type = 0;

	uint8_t s1=0, s2=0;
	sys_addr = 0;
	addr_area = 0;
	addr_line = 0;
	dev_ID = 0;
	tmp = 0;
	#ifdef FAC_DBG_EN
	rt_kprintf("msg cmd : %d \n", data[10]);
	#endif // FAC_DBG_EN
	

	sys_addr = 0;
	addr_area = 0;

	
	s1 = data[8]-0x30;
	if(s1>9)
	s1 -= 7l;
	s2 = data[9]-0x30;
	if(s2>9)
	s2 -= 7l;
	
	addr_line = s1*16 + s2;

	s1 = data[10]-0x30;
	if(s1>9)
	s1 -= 7l;
	s2 = data[11]-0x30;
	if(s2>9)
	s2 -= 7l;
	dev_ID = s1*16 + s2;

	
	if (data[13] == 0x30 && data[14] == 0x36 && data[15] == 0x38 && data[16] == 0x35)
	{
		FAC_msg_type = FAC_MSG_TYPE_RESET;
		
		cb->alarm_reset.valid = 1;
		cb->alarm_reset.port = cb->port;
		cb->alarm_reset.sys_addr = sys_addr;
		
		// clean the FAC_msg_buf.
		#if FAC_MSG_FILETER_EN
		FAC_msg_buf_clean(FAC_msg_buf);
		#endif // FAC_MSG_FILETER_EN
		
		FA_mq_reset(&cb->alarm_reset, sizeof(s_com_bus_R_reset));
		FA_mq_reset_2(&cb->alarm_reset, sizeof(s_com_bus_R_reset));

		rt_kprintf("Controller reset : %04X.         RESET .\n\n", cb->alarm_reset.sys_addr);
		
	}
	
	else if (data[1] == 0x4C && data[2] == 0x30)	//Ñ²¼ì
	{
		FAC_msg_type = FAC_MSG_TYPE_OUTING;
		
		cb->alarm_outing.valid = 1;
		cb->alarm_outing.port = cb->port;
		cb->alarm_outing.sys_addr = sys_addr;
		
		// clean the FAC_msg_buf.
		#if FAC_MSG_FILETER_EN
		FAC_msg_buf_clean(FAC_msg_buf);
		#endif // FAC_MSG_FILETER_EN
		
		FA_mq_reset(&cb->alarm_outing, sizeof(s_com_bus_R_reset));
		FA_mq_reset_2(&cb->alarm_outing, sizeof(s_com_bus_R_reset));

		rt_kprintf("Controller reset : %04X.         RESET .\n\n", cb->alarm_outing.sys_addr);
	}
	else if (data[15] == 0x35 && data[16] == 0x36)	//ÆÁ±Î
	{
		FAC_msg_type = FAC_MSG_TYPE_SHIELD;
	
	}
	// else if (data[15] == 0x34 && data[16] == 0x46)	//·´À¡
	// {
		// FAC_msg_type = FAC_MSG_TYPE_FEEDBACK;
			
	// }	
	else if ((data[4] == 0x57 && data[13] == 0x30 && data[14] == 0x31 && data[15] == 0x30 && data[16] == 0x30)
			||(data[4] == 0x57 && data[13] == 0x30 && data[14] == 0x31 && data[15] == 0x30 && data[16] == 0x31)
			||(data[4] == 0x57 && data[13] == 0x30 && data[14] == 0x31 && data[15] == 0x30 && data[16] == 0x32)
			||(data[4] == 0x57 && data[13] == 0x30 && data[14] == 0x31 && data[15] == 0x38 && data[16] == 0x35)
			||(data[4] == 0x57 && data[13] == 0x30 && data[14] == 0x31 && data[15] == 0x34 && data[16] == 0x46)
			||(data[4] == 0x57 && data[13] == 0x36 && data[14] == 0x32 && data[15] == 0x34 && data[16] == 0x46) 
			||(data[4] == 0x57 && data[13] == 0x37 && data[14] == 0x32 && data[15] == 0x34 && data[16] == 0x46)
			||(data[4] == 0x50 && data[13] == 0x36 && data[14] == 0x32 && data[15] == 0x34 && data[16] == 0x46)
	)
	{
		FAC_msg_type = FAC_MSG_TYPE_ALARM;
	}
	else if (data[15] == 0x34 && data[16] == 0x36)				//¹ÊÕÏ FAT
	{
		FAC_msg_type = FAC_MSG_TYPE_FAULT;
	}
	
	
	if (FAC_msg_type == FAC_MSG_TYPE_ALARM || FAC_msg_type==FAC_MSG_TYPE_FAULT)
	{
		if (FAC_msg_type == FAC_MSG_TYPE_ALARM)
		{
			
			cb->alarm_fire.valid = 1;
			cb->alarm_fire.sys_addr  = sys_addr;
			cb->alarm_fire.addr_main = addr_line;
			cb->alarm_fire.addr_sub  = dev_ID;
			
			fire_alarm_struct_init(&cb->alarm_fire.dev_info);
			cb->alarm_fire.dev_info.port = addr_area;
			cb->alarm_fire.dev_info.controller = addr_area;
			cb->alarm_fire.dev_info.loop = addr_line;
			cb->alarm_fire.dev_info.device_ID = dev_ID;
			
			#if FAC_MSG_FILETER_EN
			if (FAC_msg_buf_match(FAC_msg_buf, &cb->alarm_fire) == 0)
			#endif // FAC_MSG_FILETER_EN
			{
			FA_mq_fire(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
			FA_mq_fire_2(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
			rt_kprintf("FireAlarm device  %02X--%02X--%02X--%02X    ALARM .\n\n", sys_addr, addr_area, addr_line, dev_ID );
			
			}

		}
		else if (FAC_msg_type == FAC_MSG_TYPE_FAULT)
		{
			cb->alarm_fault.valid = 1;
			cb->alarm_fault.sys_addr  = sys_addr;
			cb->alarm_fault.addr_main = addr_line;
			cb->alarm_fault.addr_sub  = dev_ID;
			
			fire_alarm_struct_init(&cb->alarm_fault.dev_info);
			cb->alarm_fault.dev_info.port = addr_area;
			cb->alarm_fault.dev_info.controller = addr_area;
			cb->alarm_fault.dev_info.loop = addr_line;
			cb->alarm_fault.dev_info.device_ID = dev_ID;
			
			#if FAC_MSG_FILETER_EN
			if (FAC_msg_buf_match(FAC_msg_buf, &cb->alarm_fault) == 0)
			#endif // FAC_MSG_FILETER_EN
			{
			FA_mq_fault(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));
			FA_mq_fault_2(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));
			rt_kprintf("FireAlarm device  %02X--%02X--%02X--%02X    FAULT .\n\n", sys_addr, addr_area, addr_line, dev_ID );
			}				
		}
		
	}
	
	else if(FAC_msg_type == FAC_MSG_TYPE_SHIELD)
	{
		cb->alarm_shield.valid = 1;
		cb->alarm_shield.sys_addr  = sys_addr;
		cb->alarm_shield.addr_main = addr_line;
		cb->alarm_shield.addr_sub  = dev_ID;
		
		fire_alarm_struct_init(&cb->alarm_shield.dev_info);
		cb->alarm_shield.dev_info.port = addr_area;
		cb->alarm_shield.dev_info.controller = addr_area;
		cb->alarm_shield.dev_info.loop = addr_line;
		cb->alarm_shield.dev_info.device_ID = dev_ID;
		
		#if FAC_MSG_FILETER_EN
		if (FAC_msg_buf_match(FAC_msg_buf, &cb->alarm_shield) == 0)
		#endif // FAC_MSG_FILETER_EN
		{
		FA_mq_fire(&cb->alarm_shield, sizeof(s_com_bus_R_alarm));
		FA_mq_fire_2(&cb->alarm_shield, sizeof(s_com_bus_R_alarm));
		rt_kprintf("FireAlarm device  %02X--%02X--%02X--%02X    ALARM .\n\n", sys_addr, addr_area, addr_line, dev_ID );
		}
	}
	else if(FAC_msg_type == FAC_MSG_TYPE_FEEDBACK)
	{
		cb->alarm_feedback.valid = 1;
		cb->alarm_feedback.sys_addr  = sys_addr;
		cb->alarm_feedback.addr_main = addr_line;
		cb->alarm_feedback.addr_sub  = dev_ID;
		
		fire_alarm_struct_init(&cb->alarm_feedback.dev_info);
		cb->alarm_feedback.dev_info.port = addr_area;
		cb->alarm_feedback.dev_info.controller = addr_area;
		cb->alarm_feedback.dev_info.loop = addr_line;
		cb->alarm_feedback.dev_info.device_ID = dev_ID;
		
		#if FAC_MSG_FILETER_EN
		if (FAC_msg_buf_match(FAC_msg_buf, &cb->alarm_feedback) == 0)
		#endif // FAC_MSG_FILETER_EN
		{
		FA_mq_fire(&cb->alarm_feedback, sizeof(s_com_bus_R_alarm));
		FA_mq_fire_2(&cb->alarm_feedback, sizeof(s_com_bus_R_alarm));
		rt_kprintf("FireAlarm device  %02X--%02X--%02X--%02X    ALARM .\n\n", sys_addr, addr_area, addr_line, dev_ID );
		}
	}

	
	FAC_StrCLear(data, len);
	return 0;
}







// return 1:match the buffer , 0: new msg, not match buffer.
int FAC_msg_buf_match(s_com_bus_R_alarm *buf, s_com_bus_R_alarm *msg)
{
	// if match ,return 0. if not match record the msg, FAC_msg_buf_num ++.
	int i = 0;
	int j = 0;
	uint8_t *p_buf = (uint8_t *)buf;
	uint8_t *p_msg = (uint8_t *)msg;
	
	for (i=0;i<FAC_msg_buf_num;i++)
	{
		p_buf = (uint8_t *)&buf[i];
		for (j=0;j<sizeof(s_com_bus_R_alarm);j++)
		{
			if (p_buf[j] != p_msg[j])	
			break;
		}
		if (j != sizeof(s_com_bus_R_alarm))
		{
			continue;	
		}
		else
		{
			break;	
		}
		
	}
	
	if (i != FAC_msg_buf_num) // matched ,return 1;
	{
		SYS_log(SYS_DEBUG_INFO, ("FAC_msg_buf matched %d / %d  \n", i+1, FAC_msg_buf_num));
		return 1;
	}
	else  // not matched , return 0, copy new msg to buf.
	{
		if (FAC_msg_buf_num >= (FAC_MSG_BUF_NUM-1))
		{
			return 0;
		}
		
		p_buf = (uint8_t *)&buf[FAC_msg_buf_num];
		
		rt_memcpy(p_buf, p_msg, sizeof(s_com_bus_R_alarm));
		
		FAC_msg_buf_num ++;
		
		return 0;
	}
	

}


int FAC_msg_buf_clean(s_com_bus_R_alarm *buf)
{
	
	rt_memset(buf, 0x00, sizeof(s_com_bus_R_alarm)*FAC_MSG_BUF_NUM);
	FAC_msg_buf_num = 0;

	SYS_log(SYS_DEBUG_INFO, ("FAC_msg_buf clean !\n"));
  return 0;
}

int FAC_StrCmp(uint8_t *vTarget1, uint8_t *vTarget2, uint8_t vLen)
{
	uint8_t i = 0;
	
	for (i=0; i<vLen; i++)
	{
		if (vTarget1[i] != vTarget2[i])
		{
			return 1;
		}
	}
	
	return 0;
}





int FAC_unusual_rx_handler(s_com_bus_cb * cb)
{

    int res = 0;
    
    if (cb->parse.valid)
    {
        cb->parse.valid = 0;
        
        res = FAC_msg_parse(cb, cb->parse.buf, cb->parse.len);
        
    }
    
    return 0;
}

int FAC_unusual_server(s_com_bus_cb *cb)
{
		
    return 0;
}



