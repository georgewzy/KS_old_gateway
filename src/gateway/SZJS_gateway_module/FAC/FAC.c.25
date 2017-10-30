// GST5000 --485  2400 8N1
// modified£ºexchange the loopNo and posNo

#include "fire_alarm.h"
#include "sys_config.h"
#include <rtdevice.h>
#include "board.h"

struct rt_device * device_FAC; 

static uint32_t sys_addr = 0;
static uint32_t addr_area = 0;
static uint32_t  addr_line = 0;
static uint32_t  dev_ID = 0;

#define FAC_MSG_HEAD					0xAA

#define FAC_MSG_CMD_ALARM			0x15
#define FAC_MSG_CMD_FAULT			0x21
#define FAC_MSG_CMD_RESET			0x04

#define FAC_MSG_TYPE_NONE  		0
#define FAC_MSG_TYPE_ALARM		1
#define FAC_MSG_TYPE_FAULT		2
#define FAC_MSG_TYPE_RESET		3

#define FAC_MSG_LEN_ALARM			24
#define FAC_MSG_LEN_FAULT			24
#define FAC_MSG_LEN_RESET			7


static uint8_t	FAC_msg_type = FAC_MSG_TYPE_NONE;	
static uint8_t  FAC_msg_len = FAC_MSG_LEN_ALARM;



s_com_bus_cfg FAC_com_cfg = {2400, 8, PARITY_NONE, 1};




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
											
											cb->parse.status = FAC_parse_head;
										}
										else
										{
											cb->rec_len = 0;
											
										}
										break;
						
					case FAC_parse_head:
										if (data_temp == FAC_MSG_CMD_ALARM)
										{
											FAC_msg_type = FAC_MSG_TYPE_ALARM;
											FAC_msg_len = FAC_MSG_LEN_ALARM;
											
											cb->parse.status = FAC_parse_data;
										}
										else if (data_temp == FAC_MSG_CMD_FAULT)
										{
											FAC_msg_type = FAC_MSG_TYPE_FAULT;
											FAC_msg_len = FAC_MSG_LEN_FAULT;
											
											cb->parse.status = FAC_parse_data;
										}
										else if (data_temp == FAC_MSG_CMD_RESET)
										{
											FAC_msg_type = FAC_MSG_TYPE_RESET;
											FAC_msg_len = FAC_MSG_LEN_RESET;
											
											cb->parse.status = FAC_parse_data;
										}
										else
										{
											cb->parse.status = FAC_parse_idle;
											cb->rec_len = 0;
										}
										break;
										
					case FAC_parse_data:

										if (cb->rec_len >= FAC_msg_len)
										{
											rt_memcpy(cb->parse.buf, cb->rec_buf, FAC_msg_len);
											cb->parse.len = FAC_msg_len;
											
											cb->parse.status = FAC_parse_idle;
											cb->rec_len = 0;
											
											cb->parse.valid = 1;
											return 0;
											
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
	uint32_t vTemp = 0;
	uint64_t vNum = 0;

	sys_addr = 0;
	addr_line = data[2];
// 0x91 10 66
	// dev_ID = data[3]*0x100 + data[4] ;
		
	// vTemp = addr_line*0x1000 + dev_ID;
			
	// vNum += vTemp%10;					//1
	// vTemp = vTemp/10;						
	// vNum += (vTemp%10)*0x10;			//2
	// vTemp = vTemp/10;
	// vNum += (vTemp%10)*0x100;			//3
	// vTemp = vTemp/10;
	// vNum += (vTemp%1)*0x1000;			//4
	// vTemp = vTemp/10;
	// vNum += (vTemp%10)*0x10000;			//5
	// vTemp = vTemp/10;
	// vNum += (vTemp%10)*0x100000;		//6
	
	vNum += data[4];					//1
	vNum += data[3]<<8;			//3
	vNum += data[2]<<16;			//5


	
	
	// vNum += data[4]%10;
	// vNum += data[4]/10*10;
	// vNum += data[3]%10*100;
	// vNum += data[3]/10*1000;
	// vNum += data[2]%10*10000;
	// vNum += data[2]/10*100000;
	
	
	
  if (FAC_msg_type == FAC_MSG_TYPE_ALARM)
  {
  	
  	
						cb->alarm_fire.valid = 1;
						cb->alarm_fire.sys_addr  = 0;
						cb->alarm_fire.addr_main = 0;
						cb->alarm_fire.addr_sub  = vNum;
						
						fire_alarm_struct_init(&cb->alarm_fire.dev_info);
						cb->alarm_fire.dev_info.port = cb->port;
						cb->alarm_fire.dev_info.controller = 1;
						cb->alarm_fire.dev_info.loop = 0;
						cb->alarm_fire.dev_info.device_ID = vNum; //0x911066
						
		  			FA_mq_fire(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
		  			FA_mq_fire_2(&cb->alarm_fire, sizeof(s_com_bus_R_alarm));
		  			
					rt_kprintf("FireAlarm device  %02X %04X %04X ---  %08X   ALARM .\n\n", cb->alarm_fire.sys_addr, cb->alarm_fire.addr_main, cb->alarm_fire.addr_sub, cb->alarm_fire.dev_info.device_ID);
  		
  }
  else if (FAC_msg_type == FAC_MSG_TYPE_FAULT)
  {

						cb->alarm_fault.valid = 1;
						cb->alarm_fault.sys_addr  = 0;  
						cb->alarm_fault.addr_main = 0;
						cb->alarm_fault.addr_sub  = vNum;
						
						fire_alarm_struct_init(&cb->alarm_fault.dev_info);
						cb->alarm_fault.dev_info.port = cb->port;
						cb->alarm_fault.dev_info.controller = 1;
						cb->alarm_fault.dev_info.loop = 0;
						cb->alarm_fault.dev_info.device_ID = vNum;
							
				  	FA_mq_fault(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));
				  	FA_mq_fault_2(&cb->alarm_fault, sizeof(s_com_bus_R_alarm));

						rt_kprintf("FireAlarm device  %02X %04X %04X ----  %08X    FAULT .\n\n", cb->alarm_fault.sys_addr, cb->alarm_fault.addr_main, cb->alarm_fault.addr_sub, cb->alarm_fault.dev_info.device_ID);
  					
  		
  }
  else if (FAC_msg_type == FAC_MSG_TYPE_RESET)
  {
			cb->alarm_reset.valid = 1;
			cb->alarm_reset.port = cb->port;
			cb->alarm_reset.sys_addr = 0x00;
							
			FA_mq_reset(&cb->alarm_reset, sizeof(s_com_bus_R_reset));
			FA_mq_reset_2(&cb->alarm_reset, sizeof(s_com_bus_R_reset));

		  rt_kprintf("Controller reset : %d.         RESET .\n\n", cb->alarm_reset.sys_addr);
  		
  }

	return 0;
}



int FAC_unusual_rx_handler(s_com_bus_cb * cb)
{

    int res = 0;
    
    if (cb->parse.valid)
    {
        cb->parse.valid = 0;
        
        #if 0
        int i = 0;
        rt_kprintf("Parse data :\n");
        for (i=0;i<cb->parse.len;i++)
        {
        	rt_kprintf("%02X ", cb->parse.buf[i]);
        }
        rt_kprintf("\n\n");
        #endif 
        
        res = FAC_msg_parse(cb, cb->parse.buf, cb->parse.len);
        

    }
    
    return 0;
}

int FAC_unusual_server(s_com_bus_cb *cb)
{

    

    return 0;
}


