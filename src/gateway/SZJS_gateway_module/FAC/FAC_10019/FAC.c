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


#define FAC_MSG_HEAD		0x40
#define FAC_MSG_TAIL		0x23

#define FAC_MSG_CMD_ALARM			"HJ1"
#define FAC_MSG_CMD_FAULT			"GZ1"
#define FAC_MSG_CMD_RESET			"FW1"

#define FAC_MSG_TYPE_NONE  		0
#define FAC_MSG_TYPE_ALARM		1
#define FAC_MSG_TYPE_FAULT		2
#define FAC_MSG_TYPE_RESET		3

#define FAC_MSG_LEN_ALARM			6
#define FAC_MSG_LEN_FAULT			6
#define FAC_MSG_LEN_RESET			1


#define CMD_LEN			22

struct rt_device * device_FAC; 


static uint32_t sys_addr = 0;
static uint32_t addr_area = 0;
static uint32_t  addr_line = 0;
static uint32_t  dev_ID = 0;


static uint8_t *p_data_meta = NULL;
static uint8_t  data_meta_num = 0;


static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	
static uint32_t	FAC_msg_len = 0;



s_com_bus_cfg FAC_com_cfg = {9600, 8, PARITY_NONE, 1};

s_com_bus_R_alarm *FAC_msg_buf = NULL;
int FAC_msg_buf_num = 0;
int FAC_unusual_rx_handler(s_com_bus_cb * cb);
int FAC_strcmp(const uint8_t *str1, const uint8_t *str2);
int FAC_decipher2(const uint8_t *str1, uint8_t *str2, uint8_t str1_len);
uint32_t FAC_atoi(const uint8_t *str);
uint32_t FAC_atoi2(const uint8_t *str, uint8_t str_len);


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
	
	return 0;
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
    //uint8_t data = 0x00;
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

				if(data_temp == FAC_MSG_TAIL)
				{
						rt_memcpy(cb->parse.buf, cb->rec_buf, cb->rec_len);
						cb->parse.len = cb->rec_len;
												
						cb->parse.status = FAC_parse_idle;
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

	uint8_t ret = 0;
	uint8_t i = 0, a = 0, b = 0, c = 0, d = 0;
	uint8_t data_tmp[40] = {0};
	uint8_t data_cmd[3] = {0};
	uint8_t data_random = 0;
	uint8_t data_sys_addr[2] = {0};	
	uint8_t data_addr_main[3] = {0};
	uint8_t data_addr_sub[3] = {0};
	
	
	FAC_msg_type = 0;	
	sys_addr = 0;
	addr_area = 0;
	addr_line = 0;
	dev_ID = 0;


	#ifdef FAC_DBG_EN
	rt_kprintf("msg cmd : %d \n", data[10]);
	#endif // FAC_DBG_EN
	
	
	FAC_decipher2(data, data_tmp, len);
	
		
	for(i=1; i<len; i++) //第一个字节为起始位 这里用不到  从时间到结束字符最后17个字符用不到
	{
		if (i <= 3)		// 取出cmd
		{
			data_cmd[a++] = data_tmp[i];	
		}
		else if(i == 4)		// 取出随机数
		{
			data_random = data_tmp[i];
		}
		else if((i > 4) && (i <= 6))
		{
			data_sys_addr[b++] = data_tmp[i];	// 主机号	2bytes
		}
		else if((i > 6) && (i <= 9))
		{
			data_addr_main[c++] = data_tmp[i];	//区号	3bytes
		}	
		else if(i > 9 && i<= 12)
		{
			data_addr_sub[d++] = data_tmp[i];	// 路号 3bytes
		}
		else
		{
			break;
		}
	}
	
	
	sys_addr = FAC_atoi2(data_sys_addr, 2);
	addr_line = FAC_atoi2(data_addr_main, 3);
	dev_ID = FAC_atoi2(data_addr_sub, 3);
	
	
	if (data_cmd[0]==0x46 && data_cmd[1]==0x57 && data_cmd[2]==0x31)
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
	//else if(strcmp(data_cmd, "HJ1") == 0)
	else if(data_cmd[0]==0x48 && data_cmd[1]==0x4A && data_cmd[2]==0x31)	//火警
	{
		FAC_msg_type = FAC_MSG_TYPE_ALARM;
	}
	else if(data_cmd[0]==0x47 && data_cmd[1]==0x5A && data_cmd[2]==0x31)  //故障 
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
				cb->alarm_fire.dev_info.port = sys_addr;
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
				cb->alarm_fault.dev_info.port = sys_addr;
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
			//p_data_meta += 3;
		//}
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


