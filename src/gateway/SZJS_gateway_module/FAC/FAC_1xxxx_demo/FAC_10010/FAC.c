// FAC: Î÷ÃÅ×Ó FC18R ´òÓ¡»ú¿Ú

/* Rough data

2011-06-21 10:10:22
 Î÷×ßµÀÊÖ±¨55
 04.007.000cddd8
+020 ÊÖ¶¯»ğ¾¯



@&
 2011-06-21 10:30:12
 ÈıÈË²¡·¿ÑÌ¸Ğ63
 02.007.000dd058
+001 Í¨Ñ¶¹ÊÕÏ

 2011-06-21 10:30:13
 ÈıÈË²¡·¿ÑÌ¸Ğ63
 02.011.000ea6d7
+002 Í¨Ñ¶¹ÊÕÏ

*/

#include "fire_alarm.h"
#include "sys_config.h"
//#include <stdio.h>
#include <rtdevice.h>
#include "board.h"
#include "string.h"
#include "stdio.h"

struct rt_device * device_FAC; 

//static uint32_t  cmd_status = 0;
//static uint8_t *remark = 0;

//static uint8_t data_trans[256] = {0};
//static uint32_t data_trans_len = 0;

//static uint8_t *p_data_meta = NULL;
//static uint8_t  data_meta_num = 0;

//const uint8_t FAC_alarm_head[] = {0x0D, 0xDD};
//const uint8_t FAC_fault_head[] = {0x0E, 0xEE};
//const uint8_t FAC_reset_head[] = {0x0A, 0xAA};
//
//const uint8_t FAC_tail_buf[] = {0xBB};
//
//static uint8_t	FAC_2nd_ID[4] = {0};
//static uint8_t  FAC_status = 0;
//
//static uint8_t	FAC_controller_ID = 0;
//
//static uint8_t FAC_loop_ID = 0;
//static uint8_t FAC_pos_ID = 0;

//#define FAC_MSG_TYPE_NONE  		0
//#define FAC_MSG_TYPE_ALARM		1
//#define FAC_MSG_TYPE_FAULT		2
//#define FAC_MSG_TYPE_RESET		3


//static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	



#define FAC_PARSE_STRING_ALARM		

uint8_t	FAC_parse_string_alarm[] = { 0xBB, 0xF0, 0xBE, 0xAF ,0x00};  // "»ğ¾¯"
uint8_t	FAC_parse_string_fault[] = { 0xB9, 0xCA, 0xD5, 0xCF ,0x00};  // "¹ÊÕÏ"
uint8_t	FAC_parse_string_fault_reset[] = { 0xB9, 0xCA, 0xD5, 0xCF ,0xBB, 0xD6, 0xB8, 0xB4 ,0x00};  // "¹ÊÕÏ»Ö¸´"
uint8_t	FAC_parse_string_reset[] = { 0xB8, 0xB4, 0xCE, 0xBB ,0x00};  // "¸´Î»"

typedef enum
{
	FAC_msg_type_none	= 0,
	FAC_msg_type_alarm,
	FAC_msg_type_fault,
	FAC_msg_type_reset,
} e_msg_type;

typedef struct
{
	uint8_t	port;
	uint8_t	controller;
	uint8_t loop;
	uint32_t ID;
	
	e_msg_type	msg_type;
	
	uint8_t msg_line_index;
	uint8_t	msg_line_buf[4][64];
	
} s_FAC_parse_cb;

s_com_bus_cfg FAC_com_cfg = {9600, 8, PARITY_NONE, 1};

s_FAC_parse_cb FAC_parse_cb = {0};


int FAC_unusual_rx_handler(s_com_bus_cb * cb);


int FAC_unusual_rx_init(s_com_bus_cb * cb)
{
	
	rt_memcpy(&cb->FAC_config.cfg, &FAC_com_cfg, sizeof(s_com_bus_cfg));
	//cb->FA_listen = 1;
	return 0;
}


int FAC_unusual_rx_parser(s_com_bus_cb * cb)
{
    //uint8_t data = 0x00;
    uint8_t data_temp = 0x00;
    int res = 0;
    int i = 0;
    
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
						
										if (data_temp == 0x0A) // -- 2011-06-21 10:30:12
		                {
		                    if (cb->rec_len >= 19)
		                    {
														for (i=0;i<cb->rec_len-12;i++)
														{
															if ((cb->rec_buf[i] == '-') && (cb->rec_buf[i+3] == '-'))	
															{
																FAC_parse_cb.msg_line_index = 0;
																rt_memcpy(&FAC_parse_cb.msg_line_buf[FAC_parse_cb.msg_line_index][0], cb->rec_buf, cb->rec_len);
																FAC_parse_cb.msg_line_buf[FAC_parse_cb.msg_line_index][cb->rec_len] = 0x00;
																cb->parse.status = FAC_parse_data;
																	
																//rt_kprintf("1\n");   /////////  testing
															}
														}
		                        cb->rec_len = 0;
		                    }
		                    else
		                    {
		                        cb->rec_len = 0;
		                    }
		                }
		                else if (cb->rec_len >= sizeof(cb->rec_buf)-1)
		                {
		                		cb->rec_len = 0;
		                }
						break;
					case FAC_parse_data:
						
										if (data_temp == 0x0A) // -- 2011-06-21 10:30:12
		                {
		                	//rt_kprintf("2\n");   /////////  testing
		                	FAC_parse_cb.msg_line_index ++;
											rt_memcpy(&FAC_parse_cb.msg_line_buf[FAC_parse_cb.msg_line_index][0], cb->rec_buf, cb->rec_len);
											FAC_parse_cb.msg_line_buf[FAC_parse_cb.msg_line_index][cb->rec_len] = 0x00;
											
											if (FAC_parse_cb.msg_line_index == 3)
											{
												//rt_kprintf("3\n");   /////////  testing
												cb->rec_len = 0;
												cb->parse.status = FAC_parse_idle;
												cb->parse.valid = 1;
												return 0;
											}
											else
											{
													cb->rec_len = 0;
											}
											
										}	
		                else if (cb->rec_len >= sizeof(cb->rec_buf)-1)
		                {
		                		cb->rec_len = 0;
		                		cb->parse.status = FAC_parse_idle;
		                }
															
//		                        {
//		                            
//		                            cb->rec_len = 0;
//		                            cb->parse.status = FAC_parse_idle;
//		                            cb->parse.valid = 1;
//		                            return 0;
//		                        }
						
//		                //if (cb->FAC_config.alarm.tail.if_tail)
//		                {
//		                    cb->parse.buf[cb->parse.len] = data_temp;
//		                    cb->parse.len ++;
//		                    
//		                    if (cb->parse.len >= sizeof(cb->parse.buf))
//		                    {
//		                        cb->rec_len = 0;
//		                        cb->parse.len = 0;
//		                        cb->parse.status = FAC_parse_idle;
//		                        break;
//		                    }
//		                    
//		                    if (cb->parse.len >= (sizeof(FAC_alarm_head) + sizeof(FAC_tail_buf)))
//		                    {
//		                        if (rt_memcmp(&cb->parse.buf[cb->parse.len - sizeof(FAC_tail_buf)], 
//		                                       FAC_tail_buf,
//		                                       sizeof(FAC_tail_buf)) == 0)
//		                        {
//		                            
//		                            cb->rec_len = 0;
//		                            cb->parse.status = FAC_parse_idle;
//		                            cb->parse.valid = 1;
//		                            return 0;
//		                        }
//		                        
//		                    }
//		                    
//		                }
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


//char *_strstr(char *haystack, char *needle)
//{
//    char *p,*q;
//    while (*haystack)
//    {
//        p=needle;
//        q=haystack;
//        while (*p==*q&&*p&&*q)
//        {
//            p++;
//            q++;
//        }
//        if (!*p)
//            break;
//        haystack++;
//    }
//    if (!*haystack)
//        return NULL;
//    else
//        return haystack;
//}


//int _atoh(char* str)
//{
//	int var=0;
// int t;
// int len = strlen(str);
// 
// if (var > 8) //×î³¤8Î»
//  return -1;
//// strupr(str);//Í³Ò»´óĞ´
// for (; *str; str++)
// {
//  if (*str>='a' && *str <='f')
//   t = *str-87;//a-fÖ®¼äµÄasciiÓë¶ÔÓ¦ÊıÖµÏà²î55Èç'A'Îª65,65-55¼´ÎªA
//  else
//   t = *str-48;
//  var<<=4;
//  var|=t;
// }
// return var;
//}

//int _strlen(uint8_t *str)
//{
//	int i = 0;
//	for (i=0;i<65534;i++)
//	{
//		if (str[i] == 0x00)
//		{
//			break;
//		}
//	}
//	if (i == 65534)
//	{
//		return -1;		
//	}
//	
//	return i;	
//}

//#define INT_MAX	2147413648
//#define INT_MIN	-2147413647
//enum Ret                                              //×´Ì¬£¬ÓÃÀ´ÊäÈëÊÇ·ñºÏÀí  
//{  
//    VALID,  
//    INVALID,  
//};  
//enum Ret state = INVALID;  
//int _atoi(const char *str)  
//{  
//    int flag = 1;                                 //ÓÃÀ´¼ÇÂ¼ÊÇÕıÊı»¹ÊÇ¸ºÊı  
//    long long ret = 0;  
////    assert(str);                                    
//    if (str == NULL)  
//    {  
//        return 0;  
//    }  
//    if (*str == '\0')  
//    {  
//        return (int)ret;  
//    }  
////    while (isspace(*str))                        //ÈôÊÇ¿Õ×Ö·û´®¾Í¼ÌĞøÍùºó  
////    {  
////        str++;  
////    }  
//    if (*str == '-')  
//    {  
//        flag = -1;  
//    }  
//    if (*str == '+' || *str == '-')  
//    {  
//        str++;  
//    }  
//    while (*str)  
//    {  
//        if (*str >= '0' && *str <= '9')  
//        {  
//            ret = ret * 10 + flag * (*str - '0');                  
//            if (ret>INT_MAX||ret<INT_MIN)                 //ÅĞ¶¨ÊÇ·ñÒç³öÁË  
//            {  
//                ret = 0;  
//                break;  
//            }  
//        }  
//        else  
//        {  
//            break;  
//        }  
//        str++;  
//    }  
//    if (*str == '\0')                  //ÕâÀïwhileÑ­»·½áÊøºó£¬´ËÊ±Ö»ÓĞ*str == '\0'²ÅÊÇºÏ·¨µÄÊäÈë  
//    {  
//        state = VALID;  
//    }  
//    return ret;  
//} 

//
//int FC18R_ID_parse(uint8_t *data, uint8_t *controller, uint8_t *loop, uint32_t *ID)
//{
//	int i = 0;
//	int j = 0;
//	int parse = 0;
//	uint8_t *p_start = NULL;
//	uint8_t *p_end = NULL;
//	int len = 0;
//	
//	uint8_t controller_buf[4] = {0};
//	uint8_t loop_buf[5] = {0};
//	uint8_t ID_buf[10] = {0};
//	
//	//rt_kprintf("12\n");   /////////  testing
//	len = strlen(data);
//	
//	//rt_kprintf("13, Len = %d\n", len);   /////////  testing
//	
//	
//	parse = 0;
//	p_start = 0;
//	// parse controller
//	for (i=parse;i<len;i++)
//	{
//		if ((data[i]>='0') && (data[i]<='9'))
//		{
//			if (p_start == NULL) p_start = data+i;		
//		}
//		else if (p_start) // '.'
//		{
//			p_end = data+i;
//			rt_memcpy(controller_buf, p_start, p_end-p_start);
//			parse = i+1;
//			break;
//		}	
//	}
//	
//	// parse loop
//	p_start = 0;
//	for (i=parse;i<len;i++)
//	{
//		if ((data[i]>='0') && (data[i]<='9'))
//		{
//			if (p_start == NULL) p_start = data+i;		
//		}	
//		else if (p_start) // '.'
//		{
//			p_end = data+i;
//			rt_memcpy(loop_buf, p_start, p_end-p_start);
//			parse = i+1;
//			break;
//		}	
//	}
//	
//	// parse ID
//	p_start = 0;
//	for (i=parse;i<len;i++)
//	{
//		if (((data[i]>='0') && (data[i]<='9')) || ((data[i]>='a') && (data[i]<='f')))
//		{
//			if (p_start == NULL) p_start = data+i;		
//		}	
//		else if (p_start) // '.'
//		{
//			p_end = data+i;
//			rt_memcpy(ID_buf, p_start, p_end-p_start);
//			parse = i+1;
//			break;
//		}	
//	}
//
//	//rt_kprintf("14\n %s\n%s\n%s\n\n", controller_buf, loop_buf, ID_buf);   /////////  testing
//	
//	
//	*controller = _atoi(controller_buf);
//	*loop = _atoi(loop_buf);
//	*ID = _atoh(ID_buf);
//	
//	//rt_kprintf("15\n res: %d\n%d\n%d\n\n", *controller, *loop, *ID);   /////////  testing
//	
//	
//	return 0;
//}



int FC18R_printer_parse(s_com_bus_cb * cb, uint8_t *data, uint32_t len)
{
	int i = 0;
	uint8_t *p;
	
	//rt_kprintf("5\n");   /////////  testing
	
	//rt_kprintf("%s\n%s\n%s\n%s\n", &FAC_parse_cb.msg_line_buf[0][0],&FAC_parse_cb.msg_line_buf[1][0],&FAC_parse_cb.msg_line_buf[2][0],&FAC_parse_cb.msg_line_buf[3][0]);  // testing
	
	
	// parse the msg_type
	if ((p = strstr(&FAC_parse_cb.msg_line_buf[3][0], FAC_parse_string_alarm)) != NULL)
	{
		//rt_kprintf("6\n");   /////////  testing
		FAC_parse_cb.msg_type = FAC_msg_type_alarm;
		
	}
	else if ((p = strstr(&FAC_parse_cb.msg_line_buf[3][0], FAC_parse_string_fault)) != NULL)
	{
		//rt_kprintf("7\n");   /////////  testing
		if ((p = strstr(&FAC_parse_cb.msg_line_buf[3][0], FAC_parse_string_fault_reset)) != NULL)
		{
			//rt_kprintf("8\n");   /////////  testing
			return -1;	
		}
		else
		{
			//rt_kprintf("9\n");   /////////  testing
			FAC_parse_cb.msg_type = FAC_msg_type_fault;
		}
	}
	else if ((p = strstr(&FAC_parse_cb.msg_line_buf[3][0], FAC_parse_string_reset)) != NULL)
	{
			//rt_kprintf("9\n");   /////////  testing
			FAC_parse_cb.msg_type = FAC_msg_type_reset;
	}
	else
	{
		return -1;	
	}
	
	//rt_kprintf("10\n");   /////////  testing
	// parse the ID info.
  sscanf(&FAC_parse_cb.msg_line_buf[2][0], "%d.%d.%x", &FAC_parse_cb.controller, &FAC_parse_cb.loop, &FAC_parse_cb.ID);
////	FC18R_ID_parse(&FAC_parse_cb.msg_line_buf[2][0], &FAC_parse_cb.controller, &FAC_parse_cb.loop, &FAC_parse_cb.ID);
	//rt_kprintf("11\n");   /////////  testing
  
  if (FAC_parse_cb.msg_type == FAC_msg_type_alarm)
  {
  	
  		//if ((FAC_2nd_ID[3] <= 0x11) && (FAC_status == 0x01))
  		{
						cb->alarm_fire.valid = 1;
						
						//rt_memcpy(cb->alarm_fire.remark, &data[2], 5);
						fire_alarm_struct_init(&cb->alarm_fire.dev_info);
						cb->alarm_fire.dev_info.port = cb->port;
						cb->alarm_fire.dev_info.controller = FAC_parse_cb.controller;
						cb->alarm_fire.dev_info.loop = FAC_parse_cb.loop;
						cb->alarm_fire.dev_info.device_ID = FAC_parse_cb.ID;
						//cb->alarm_fire.dev_info.device_port = channel_num;
						
		  			FA_mq_fire(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
		  			FA_mq_fire_2(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
		  			
						rt_kprintf("ALARM .\n  %d -- %d -- %08X \n\n", FAC_parse_cb.controller, FAC_parse_cb.loop, FAC_parse_cb.ID);
  		}	
  		
  }
  else if (FAC_parse_cb.msg_type == FAC_msg_type_fault)
  {

  		//if (FAC_status == 0x00)
  		{
						cb->alarm_fault.valid = 1;
						
						//rt_memcpy(cb->alarm_fault.remark, &data[2], 5);
						fire_alarm_struct_init(&cb->alarm_fault.dev_info);
						cb->alarm_fault.dev_info.port = cb->port;
						cb->alarm_fault.dev_info.controller = FAC_parse_cb.controller;
						cb->alarm_fault.dev_info.loop = FAC_parse_cb.loop;
						cb->alarm_fault.dev_info.device_ID = FAC_parse_cb.ID;
						//cb->alarm_fault.dev_info.device_port = channel_num;
													
				  	FA_mq_fault(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));
				  	FA_mq_fault_2(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));

						rt_kprintf("FAULT .\n  %d -- %d -- %08X \n\n", FAC_parse_cb.controller, FAC_parse_cb.loop, FAC_parse_cb.ID);
  					
  		}
  		
  }
  else if (FAC_parse_cb.msg_type == FAC_msg_type_reset)
  {
			cb->alarm_reset.valid = 1;
			cb->alarm_reset.sys_addr = FAC_parse_cb.controller;
							
			FA_mq_reset(&cb->alarm_reset, sizeof(s_com_bus_R_reset));
			FA_mq_reset_2(&cb->alarm_reset, sizeof(s_com_bus_R_reset));

		  rt_kprintf("RESET .  %d  \n\n", FAC_parse_cb.controller);
  		
  }

	return 0;
}



int FAC_unusual_rx_handler(s_com_bus_cb * cb)
{

    int res = 0;
    
    if (cb->parse.valid)
    {
        cb->parse.valid = 0;
        
        res = FC18R_printer_parse(cb, cb->parse.buf, cb->parse.len);
        

    }
    
    return 0;
}

int FAC_unusual_server(s_com_bus_cb *cb)
{

    

    return 0;
}
