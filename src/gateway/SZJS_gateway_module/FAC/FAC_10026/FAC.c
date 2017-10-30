// 赋安JB-QBH-FS5101W

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

static uint8_t *p_data_meta = NULL;
static uint8_t  data_meta_num = 0;


#define FAC_MSG_HEAD			0xF0
#define FAC_MSG_TAIL			0x00

#define FAC_MSG_CMD_ALARM			(u8)0xC3
#define FAC_MSG_CMD_FAULT			(u8)0xC2
#define FAC_MSG_CMD_RESET			(u8)0xA1


#define FAC_MSG_TYPE_NONE  		0
#define FAC_MSG_TYPE_ALARM		1
#define FAC_MSG_TYPE_FAULT		2
#define FAC_MSG_TYPE_RESET		3




static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	
static uint32_t	FAC_msg_len = 0;
const static uint8_t fac_send_data[12] = {0xFF,0xFF,0xFF,0x0A,0x00,0x00,0x01,0x6E,0x01,0x0A,0x00,0x83};

const u16 crc_table[256] = {
0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50a5U, 0x60c6U, 0x70e7U,
0x8108U, 0x9129U, 0xa14aU, 0xb16bU, 0xc18cU, 0xd1adU, 0xe1ceU, 0xf1efU,
0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52b5U, 0x4294U, 0x72f7U, 0x62d6U,
0x9339U, 0x8318U, 0xb37bU, 0xa35aU, 0xd3bdU, 0xc39cU, 0xf3ffU, 0xe3deU,
0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64e6U, 0x74c7U, 0x44a4U, 0x5485U,
0xa56aU, 0xb54bU, 0x8528U, 0x9509U, 0xe5eeU, 0xf5cfU, 0xc5acU, 0xd58dU,
0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76d7U, 0x66f6U, 0x5695U, 0x46b4U,
0xb75bU, 0xa77aU, 0x9719U, 0x8738U, 0xf7dfU, 0xe7feU, 0xd79dU, 0xc7bcU,
0x48c4U, 0x58e5U, 0x6886U, 0x78a7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
0xc9ccU, 0xd9edU, 0xe98eU, 0xf9afU, 0x8948U, 0x9969U, 0xa90aU, 0xb92bU,
0x5af5U, 0x4ad4U, 0x7ab7U, 0x6a96U, 0x1a71U, 0x0a50U, 0x3a33U, 0x2a12U,
0xdbfdU, 0xcbdcU, 0xfbbfU, 0xeb9eU, 0x9b79U, 0x8b58U, 0xbb3bU, 0xab1aU,
0x6ca6U, 0x7c87U, 0x4ce4U, 0x5cc5U, 0x2c22U, 0x3c03U, 0x0c60U, 0x1c41U,
0xedaeU, 0xfd8fU, 0xcdecU, 0xddcdU, 0xad2aU, 0xbd0bU, 0x8d68U, 0x9d49U,
0x7e97U, 0x6eb6U, 0x5ed5U, 0x4ef4U, 0x3e13U, 0x2e32U, 0x1e51U, 0x0e70U,
0xff9fU, 0xefbeU, 0xdfddU, 0xcffcU, 0xbf1bU, 0xaf3aU, 0x9f59U, 0x8f78U,
0x9188U, 0x81a9U, 0xb1caU, 0xa1ebU, 0xd10cU, 0xc12dU, 0xf14eU, 0xe16fU,
0x1080U, 0x00a1U, 0x30c2U, 0x20e3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
0x83b9U, 0x9398U, 0xa3fbU, 0xb3daU, 0xc33dU, 0xd31cU, 0xe37fU, 0xf35eU,
0x02b1U, 0x1290U, 0x22f3U, 0x32d2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
0xb5eaU, 0xa5cbU, 0x95a8U, 0x8589U, 0xf56eU, 0xe54fU, 0xd52cU, 0xc50dU,
0x34e2U, 0x24c3U, 0x14a0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
0xa7dbU, 0xb7faU, 0x8799U, 0x97b8U, 0xe75fU, 0xf77eU, 0xc71dU, 0xd73cU,
0x26d3U, 0x36f2U, 0x0691U, 0x16b0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
0xd94cU, 0xc96dU, 0xf90eU, 0xe92fU, 0x99c8U, 0x89e9U, 0xb98aU, 0xa9abU,
0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18c0U, 0x08e1U, 0x3882U, 0x28a3U,
0xcb7dU, 0xdb5cU, 0xeb3fU, 0xfb1eU, 0x8bf9U, 0x9bd8U, 0xabbbU, 0xbb9aU,
0x4a75U, 0x5a54U, 0x6a37U, 0x7a16U, 0x0af1U, 0x1ad0U, 0x2ab3U, 0x3a92U,
0xfd2eU, 0xed0fU, 0xdd6cU, 0xcd4dU, 0xbdaaU, 0xad8bU, 0x9de8U, 0x8dc9U,
0x7c26U, 0x6c07U, 0x5c64U, 0x4c45U, 0x3ca2U, 0x2c83U, 0x1ce0U, 0x0cc1U,
0xef1fU, 0xff3eU, 0xcf5dU, 0xdf7cU, 0xaf9bU, 0xbfbaU, 0x8fd9U, 0x9ff8U,
0x6e17U, 0x7e36U, 0x4e55U, 0x5e74U, 0x2e93U, 0x3eb2U, 0x0ed1U, 0x1ef0U	
};

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






s_com_bus_cfg FAC_com_cfg = {9600, 8, PARITY_NONE, 2};

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



void rt_thread_entry_HandShake(void* parameter)
{

	s_com_bus_cb *cb;
	cb = (s_com_bus_cb*)parameter;

	while(1)
	{
	//	rt_device_write(cb->dev, 0, fac_send_data, 12);
		
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


int FAC_unusual_rx_parser(s_com_bus_cb * cb)
{
 
    uint8_t data_temp = 0x00;
    int res = 0;
	u16 crc_val = 0;
	u8 d[2] = {0x01,0xA2};
    u8 crc_data[20] = {0};
	
    while(1)
    {
        res = rt_device_read(cb->dev, 0, &data_temp, 1);
        if (res < 1)
        {
            return -1;
        }
        cb->rec_buf[cb->rec_len] = data_temp;
        cb->rec_len++;

		switch (cb->parse.status)
		{
			case FAC_parse_idle:
				if (data_temp == FAC_MSG_HEAD)
				{
					
					cb->parse.status = FAC_parse_data;
				}
				else
				{
					cb->rec_len = 0;
					
				}
				break;
			case FAC_parse_head:
					

			break;

			case FAC_parse_data:
				if (cb->rec_buf[3] == 0xA1)		//主机复位
				{
					if (cb->rec_len > 5)								
					{
						rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
						cb->parse.len = cb->rec_len;
							
						FAC_StrCLear(cb->rec_buf, cb->rec_len);
						
						cb->parse.status = FAC_parse_idle;
						cb->rec_len = 0;
						
						cb->parse.valid = 1;

						#ifdef FAC_DBG_EN
						rt_kprintf("msg valid \n");
						#endif // FAC_DBG_EN

						return 0;
					}
				}
				else if(cb->rec_buf[3] == 0xA2)	//	巡检码
				{
					if (cb->rec_len > 5)							
					{
						rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
						cb->parse.len = cb->rec_len;
											
						reply_data[0] = 0xF0;
						reply_data[1] = cb->rec_buf[1];
						reply_data[2] = 0x03;
						reply_data[3] = 0xA2;
						reply_data[4] = 0x00;
						reply_data[5] = 0x00;
						
						crc_data[0] = reply_data[2];
						crc_data[1] = reply_data[3];
						crc_data[2] = reply_data[4];
						crc_data[3] = reply_data[5];
						crc_val = crc16(0, crc_data, 4);
												
						reply_data[6] = (u8)(crc_val>>8);
						reply_data[7] = (u8)crc_val;
						
						
						rt_device_write(cb->dev, 0, reply_data, 8);
						
						
							
						FAC_StrCLear(cb->rec_buf, cb->rec_len);
						
						cb->parse.status = FAC_parse_idle;
						cb->rec_len = 0;
						
						cb->parse.valid = 1;

						#ifdef FAC_DBG_EN
						rt_kprintf("msg valid \n");
						#endif // FAC_DBG_EN

						return 0;
					}
					
				}
				else if(cb->rec_buf[3] == 0xA5)	// 发送事件
				{
					
					if (cb->rec_len > 24)							
					{
						rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
												
						cb->parse.len = cb->rec_len;
					//	rt_device_write(cb->dev, 0, cb->rec_buf, cb->rec_len);	
							
						FAC_StrCLear(cb->rec_buf, cb->rec_len);
						
						cb->parse.status = FAC_parse_idle;
						cb->rec_len = 0;
						
						cb->parse.valid = 1;

						#ifdef FAC_DBG_EN
						rt_kprintf("msg valid \n");
						#endif // FAC_DBG_EN

						return 0;
					}
					
				}
				else
				{
					if (cb->rec_len > 24)								//主机火警 或 故障
					{
						rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
						cb->parse.len = cb->rec_len;
						
						FAC_StrCLear(cb->rec_buf, cb->rec_len);
						
						cb->parse.status = FAC_parse_idle;
						cb->rec_len = 0;
						
						cb->parse.valid = 1;

						#ifdef FAC_DBG_EN
						rt_kprintf("msg valid \n");
						#endif // FAC_DBG_EN

						return 0;
					}
				}
				
				if (cb->rec_len >= (sizeof(cb->rec_buf) - 2))
				{
					cb->parse.len = 0;
					cb->rec_len = 0;
					cb->parse.status = FAC_parse_idle;
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






int FAC_msg_parse(s_com_bus_cb * cb, uint8_t *data, uint32_t len)
{

	int i = 0;
	FAC_msg_type = 0;
	
	sys_addr = 0;
	addr_area = 0;
	addr_line = 0;
	dev_ID = 0;

	#ifdef FAC_DBG_EN
	rt_kprintf("msg cmd : %d \n", data[10]);
	#endif // FAC_DBG_EN
	

	sys_addr = 0;
	addr_area = 0;
	
	addr_line = data[15];
	dev_ID = data[16];
	
	
	if (data[3] == FAC_MSG_CMD_RESET)
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
	else if (data[3] == 0xA5 && data[4] == FAC_MSG_CMD_ALARM)
	{
		FAC_msg_type = FAC_MSG_TYPE_ALARM;
	}
	else if (data[3] == 0xA5 && data[4] == FAC_MSG_CMD_FAULT)				//故障 FAT
	{
		FAC_msg_type = FAC_MSG_TYPE_FAULT;
	}
	
	
	if (FAC_msg_type==FAC_MSG_TYPE_ALARM || FAC_msg_type==FAC_MSG_TYPE_FAULT)
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
	
	FAC_StrCLear(data, len);
	return 0;
}





u16 crc16(u16 crc,const u8 *data, u32 len )//len 可以为 u8,u16,u32
{
while (len--)
crc = crc_table[(crc >> 8 ^ *(data++)) & 0xffU] ^ (crc << 8);
return crc;
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



