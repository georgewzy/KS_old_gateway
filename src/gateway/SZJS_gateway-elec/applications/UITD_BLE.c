#include "UITD_BLE.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "sys_misc.h"
#include "string.h"
#include "sys_status.h"

#include "UITD.h"

t_server_handler    *p_UITD_BLE_SZJS = NULL;



rt_uint8_t prio_UITD_BLE = 8;
rt_thread_t thread_UITD_BLE;

rt_uint8_t prio_UITD_BLE_rx = 6;
rt_thread_t thread_UITD_BLE_rx;

rt_uint8_t prio_UITD_BLE_rec = 12;
rt_thread_t thread_UITD_BLE_rec;

struct rt_semaphore sem_BLE_rx;
struct rt_device * device_module_BLE;

static uint8_t module_BLE_inited = 0;


rt_err_t module_BLE_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_BLE_rx);    

}

rt_err_t module_BLE_tx_complete(rt_device_t dev, void *buffer)
{


}




int module_BLE_init(uint8_t *uart)
{

    //rt_sem_init(&sem_BLE_rx, "BLE_rx", 0, RT_IPC_FLAG_FIFO);
    
    
//    // Reset the module_BLE.
//    rt_pin_write(PIN_ID_DETECT      , PIN_HIGH);
//    rt_thread_delay(RT_TICK_PER_SECOND/50);  //delay 20mS.
//    // Disable the ext_module ID output.
//    rt_pin_write(PIN_ID_DETECT      , PIN_LOW);
 

////    rt_thread_delay(RT_TICK_PER_SECOND/5);  //delay 200mS.
    
    device_module_BLE = rt_device_find(uart);
    if(device_module_BLE == RT_NULL)
    {
        SYS_log( SYS_DEBUG_WARNING, ("Serial device %s not found!\r\n", uart));
    }
    else
    {
		rt_device_open(device_module_BLE, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(device_module_BLE, module_BLE_rx_ind);
        rt_device_set_tx_complete(device_module_BLE, module_BLE_tx_complete);

    }

////    rt_device_write(device_module_BLE, 0, BLE_CMD_RESET,sizeof(BLE_CMD_RESET)-1);
    

    module_BLE_inited = 1;
    
    return 0;
}




int BLE_connect(void *handler,
                pFun_BLE_connect_fail connect_fail,
                pFun_BLE_connected connected,
                pFun_BLE_disconnected  disconnected,
                pFun_BLE_received  received,
                pFun_BLE_rejected  rejected,
                pFun_BLE_closed    closed,
                pFun_BLE_report_IP report_IP
                )
{
    t_server_handler *srv_handler = (t_server_handler *)handler;
    int res = 0;
    int sock = -1;
    

    return sock;
}



int UITD_BLE_connect( void *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port,
                                pFun_srv_connect_fail connect_fail,
                                pFun_srv_connected connected,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_rejected rejected,
                                pFun_srv_closed closed,
                                pFun_srv_report_IP report_IP
                                )
{

    
    
    return BLE_connect( handler,
                        connect_fail,
                        connected,
                        disconnected,
                        received,
                        rejected,
                        closed,
                        report_IP
                        );
}

int UITD_BLE_connected(void *data)
{

    return 0;
}

int UITD_BLE_disconnect(void *handler, int socket)
{
    t_server_handler *server_handler = (t_server_handler *)handler;
    
    server_handler->status = UITD_svc_status_idle;
    return 0;
}

int UITD_BLE_disconnected(void *handler, void *data, int socket)
{

    return 0;
}

// Song: NOTICE: BLE module HM-13 can only send 20 bytes per packet.
int UITD_BLE_send(void *handler, int socket, char *data,int len)
{
    int res = 0;
    int i = 0;
    
    if (len > 20)
    {
        for (i=0;i<(len/20);i++)
        {
            res += rt_device_write(device_module_BLE, 0, data+i*20, 20);
        }
        if (len % 20)
        {
            res += rt_device_write(device_module_BLE, 0, data+i*20, len%20);
        }
        rt_thread_delay(1); // delay 10ms
        
    }
    else
    {
        res += rt_device_write(device_module_BLE, 0, data, len);
        rt_thread_delay(1); // delay 10ms
    }
    
    return res;
}

int UITD_BLE_received(void *handler, void *data, uint32_t len)
{
    int res = 0;
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
	res = rt_ringbuffer_put(&svr_handler->ring_buf, data, len);
	if (res != len)
	{
		SYS_log( SYS_DEBUG_ERROR, ("ringbuf %d write error : %d ", svr_handler->socket, res));
		return -1;
	}
    
    return 0;
}



void rt_thread_entry_UITD_BLE(void* parameter)
{
    int res = 0;
    int i = 0;

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(2);
    }
    
    while(module_BLE_inited != 1)
    {
        rt_thread_delay(1);
    }


    p_UITD_BLE_SZJS = UITD_init(&UITD_BLE_send,
                            &UITD_BLE_connect,
                            &UITD_BLE_connected,
                            &UITD_BLE_disconnect,
                            &UITD_BLE_disconnected,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL
                            );    
    
    module_BLE_inited = 2;
    
    UITD_sensor_handler_init(p_UITD_BLE_SZJS);
    sys_status_set(&sys_ctrl, SYS_state_working);
    p_UITD_BLE_SZJS->if_connected = 2;
    
    p_UITD_BLE_SZJS->if_UITD = 1;
    
    
    while(1)
    {
        
        {
            UITD_service_handler(p_UITD_BLE_SZJS);
            UITD_send_handler(p_UITD_BLE_SZJS);
        }
   
        rt_thread_delay(1);
    }
    
}

void rt_thread_entry_UITD_BLE_rx(void* parameter)
{
    uint32_t e;
    int res = 0;
    
    uint8_t BLE_buf[GB_PACKET_LEN_MAX] = {0};

    //rt_thread_t A8_if_tid;
    
    
    rt_sem_init(&sem_BLE_rx, "BLE_rx", 0, RT_IPC_FLAG_FIFO);

    
    while(module_BLE_inited != 2)
    {
        rt_thread_delay(1);
    }
    
    while(1)
    {
        
        if (rt_sem_take(&sem_BLE_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            res = rt_device_read(device_module_BLE, 0, BLE_buf, GB_PACKET_LEN_MAX);
            if (res <= 0)
            {
                continue;
            }
            
            UITD_BLE_received(p_UITD_BLE_SZJS, BLE_buf, res);
            
        }
        else
        {
        
        }
        
    }
}


void rt_thread_entry_UITD_BLE_rec(void* parameter)
{
    int res = 0;

    while(module_BLE_inited != 2)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        UITD_server(p_UITD_BLE_SZJS);
        rt_thread_delay(1);
    }

}