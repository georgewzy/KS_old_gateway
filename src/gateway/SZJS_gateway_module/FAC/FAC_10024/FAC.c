//OXX20数据转换器协议
// 20170719
//   -- 232  9600 8N1

#include "fire_alarm.h"
#include "sys_config.h"
#include <stdio.h>
#include <rtdevice.h>
#include "board.h"



#define FAC_MSG_FILETER_EN	0


#define FAC_MSG_BUF_NUM		100


#define FAC_MSG_HEAD		0x66
#define FAC_MSG_TAIL		0xFD

#define FAC_MSG_CMD_ALARM				(uint8_t)0x01
#define FAC_MSG_CMD_FAULT				(uint8_t)0x02
#define FAC_MSG_CMD_RESET				(uint8_t)0x50



#define FAC_MSG_TYPE_NONE  					0
#define FAC_MSG_TYPE_ALARM					1
#define FAC_MSG_TYPE_FAULT					2
#define FAC_MSG_TYPE_RESET					3



#define FAC_MSG_LEN_ALARM			6
#define FAC_MSG_LEN_FAULT			6
#define FAC_MSG_LEN_RESET			1

#define CMD_LEN			22


#define THREAD_HAND_SHAKE_DATA_STACK_SIZE     4096



struct rt_device * device_FAC; 

static uint32_t sys_addr = 0;
static uint32_t addr_area = 0;
static uint32_t  addr_line = 0;
static uint32_t  dev_ID = 0;
static uint8_t cnt1 = 0, cnt2 = 0;


static uint8_t *p_data_meta = NULL;
static uint8_t  data_meta_num = 0;


static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	
static uint32_t	FAC_msg_len = 0;


static rt_uint8_t prio_HandShake = 9;
static rt_thread_t thread_HandShake;




s_com_bus_cfg FAC_com_cfg = {9600, 8, PARITY_NONE, 1};

s_com_bus_R_alarm *FAC_msg_buf = NULL;
int FAC_msg_buf_num = 0;
int FAC_unusual_rx_handler(s_com_bus_cb * cb);
int FAC_strcmp(const uint8_t *str1, const uint8_t *str2);
void rt_thread_entry_HandShake(void* parameter);



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
	uint8_t n_data[1] = {0x20};
	s_com_bus_cb *cb;
	cb = (s_com_bus_cb*)parameter;

	while(1)
	{
		rt_device_write(cb->dev, 0, n_data, 1);			
		rt_thread_delay(80);
    } 			
}




int FAC_StrClear(uint8_t *vTarget, uint8_t vLen)
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
			
				if(data_temp == 0x7F)
				{
					cb->rec_len = 0;
				}				
				else if (cb->rec_len >= 3)
				{
					rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
					cb->parse.len = cb->rec_len;
					
					cb->rec_len = 0;
					cb->parse.valid = 1;
					return 0;
				}

			break;
			case FAC_parse_head:

			break;

			case FAC_parse_data:
													
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

	uint8_t ret = 0;


	
	FAC_msg_type = 0;	

	addr_area = 0;
	sys_addr = 0;
	addr_line = data[1];
	dev_ID = data[2];
	
	
	#ifdef FAC_DBG_EN
	rt_kprintf("msg cmd : %d \n", data[10]);
	#endif // FAC_DBG_EN
	
	
	if (data[0] == FAC_MSG_CMD_RESET)
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
	else if(data[0] == FAC_MSG_CMD_ALARM)	//火警
	{
		FAC_msg_type = FAC_MSG_TYPE_ALARM;
	}
	else if(data[0] == FAC_MSG_CMD_FAULT)  //故障 
	{
		FAC_msg_type = FAC_MSG_TYPE_FAULT;
	}
	
	
	if (FAC_msg_type == FAC_MSG_TYPE_ALARM)
	{
		cb->alarm_fire.valid = 1;
		cb->alarm_fire.sys_addr  = sys_addr;	//主机地址
		cb->alarm_fire.addr_main = addr_line;	//
		cb->alarm_fire.addr_sub  = dev_ID;
		
		fire_alarm_struct_init(&cb->alarm_fire.dev_info);
		cb->alarm_fire.dev_info.port = addr_area;
		cb->alarm_fire.dev_info.controller = sys_addr;		//主机地址
		cb->alarm_fire.dev_info.loop = addr_line;			//回路
		cb->alarm_fire.dev_info.device_ID = dev_ID;			//地址号
				
				
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


// 比较两个字符串是否相同
// 0表示相同

int FAC_strcmp(const uint8_t *str1, const uint8_t *str2)
{

	while (*(str1) == *(str2))
	{
		if(*str1 == '\0')
			return 0;
		str1++;
		str2++;
	}
	
	if (*str1 > *str2)
	{
		return 1;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

// 解码函数 A加密方式(A+X-9)   X为随机数
int FAC_decipher2(const uint8_t *str1, uint8_t *str2, uint8_t str1_len)
{
	uint8_t i = 0, j = 0;
	uint8_t rand_ch = 0;

	if (str1 == NULL)
	{
		return -1;
	}
	
	rand_ch = str1[4];	//第五个字节为随机数
	
	for (i=0; i<str1_len; i++)
	{	
		if (i < 5)				//前五个字节不用解密
		{
			str2[j++] = str1[i];	// 
		}
		else if (i >= 5 && i < (str1_len-1))
		{
			str2[j++] = str1[i] + 9 - rand_ch; // 处理加密过的数据
		}
		else	//最后一个字节不用解密
		{
			str2[j] = str1[i];
		}		
	}
	
	return 0;
}


uint32_t FAC_atoi2(const uint8_t *str, uint8_t str_len)
{
	uint32_t ret = 0;
	uint8_t i = 1;
	
	if(str == NULL)
	{
		return 0;
	}
	
	// while(*str == ' ')
	// {
		// str++;
	// }
	
	for(i=0; i<str_len; i++)
	{
		ret = ret * 10 + str[i];
	}
	
	return ret; 
}

// 
uint32_t FAC_atoi(const uint8_t *str)
{
	uint32_t ret = 0;
	int interger_sign = 1;
	
	if(str == NULL)
	{
		return 0;
	}
	
	while(*str == ' ')
	{
		str++;
	}
	
	// if(*str == '-' || *str == '+')1
	// {
		// if(*str == '-')
		// {
			// interger_sign = -1;
		// }
		// str++;
	// }
	
	while(*str >= '0' && *str <= '9')
	{
		ret = ret * 10 + *str - '0';
		str++;
	}
//	ret = ret * interger_sign;
	
	return ret; 
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


