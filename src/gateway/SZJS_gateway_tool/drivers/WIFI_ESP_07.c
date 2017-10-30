#include "WIFI_ESP_07.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>

rt_uint8_t prio_WIFI_ESP_07 = 12;
rt_thread_t thread_WIFI_ESP_07;

rt_uint8_t prio_WIFI_rx = 12;
rt_thread_t thread_WIFI_rx;

struct rt_device *device_WIFI_ESP_07;

struct rt_semaphore sem_WIFI_rx;
struct rt_semaphore sem_WIFI_tx;












rt_err_t WIFI_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_WIFI_rx);    
    
    //rt_snprintf();

}

rt_err_t WIFI_tx_complete(rt_device_t dev, void *buffer)
{

////    rt_sem_release(&sem_GSM_tx);

}

rt_err_t WIFI_rx_check(rt_device_t dev)
{
    
    uint8_t c = 0x00;
    
    
    while (1) 
    {
        if (rt_device_read(dev, 0, &c, 1) == 0)
        {
            return 0;
        }    
        
        rt_kprintf("%c", c);
        
        
        
    }
    
    
    return 0;
}



void rt_thread_entry_WIFI_rx(void* parameter)
{
    uint32_t e;
    int res = 0;

    //rt_thread_t A8_if_tid;
    
    
    rt_sem_init(&sem_WIFI_rx, "WIFI_rx", 0, RT_IPC_FLAG_FIFO);
  
    
   
    while(1)
    {
        
        // If sem is deleted already, return RT_ERROR. We need thread_delay for thread_idle to kill the dead thread !
        if (rt_sem_take(&sem_WIFI_rx, RT_WAITING_FOREVER) == -RT_ERROR)
        {
            rt_kprintf("Sem_take  WIFI_rx  failed !\n");
            rt_thread_delay(5);  
            rt_kprintf("Sem_take  WIFI_rx  over !\n");

        }
        else
        {
            WIFI_rx_check(device_WIFI_ESP_07);
        }
    }
}



void rt_thread_entry_WIFI_ESP_07(void* parameter)
{

//    rt_sem_init(&sem_GSM_MG301_tx, "GSM_MG301_tx", 0, RT_IPC_FLAG_FIFO);
//    rt_sem_init(&sem_GSM_MG301_rx, "GSM_MG301_rx", 0, RT_IPC_FLAG_FIFO);
    
    device_WIFI_ESP_07 = rt_device_find(UITD_UART_WIFI);
    if(device_WIFI_ESP_07 == RT_NULL)
    {
        rt_kprintf("Serial device %s not found!\r\n", UITD_UART_WIFI);
    }
    else
    {
		rt_device_open(device_WIFI_ESP_07, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(device_WIFI_ESP_07, WIFI_rx_ind);
        rt_device_set_tx_complete(device_WIFI_ESP_07, WIFI_tx_complete);

    }
 

    ////WIFI_ESP_07_init(device_WIFI_ESP_07);

    while(1)
    {
        ////rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT, sizeof(GSM_CMD_AT) - 1);
        rt_thread_delay(RT_TICK_PER_SECOND / 1);
    }
    
}



#ifdef RT_USING_FINSH
#include <finsh.h>

uint8_t WIFI_ESP_07_init(rt_device_t dev)
{

    // Song:　Initialize the WIFI ESP_07 power control and reset.
//    rt_pin_write(PIN_WIFI_SHUT, PIN_HIGH);
//    rt_pin_write(PIN_WIFI_RST, PIN_HIGH);
//    rt_thread_delay(RT_TICK_PER_SECOND / 10);
//    rt_pin_write(PIN_WIFI_RST, PIN_LOW);
//    rt_thread_delay(RT_TICK_PER_SECOND / 10);
//    rt_pin_write(PIN_WIFI_RST, PIN_HIGH);
//    rt_thread_delay(RT_TICK_PER_SECOND / 10);

    
    rt_device_write(dev, 0, WIFI_ESP_CMD_AT, sizeof(WIFI_ESP_CMD_AT) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    
    rt_device_write(dev, 0, WIFI_ESP_CMD_AT_CWMODE, sizeof(WIFI_ESP_CMD_AT_CWMODE) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);

    rt_device_write(dev, 0, WIFI_ESP_CMD_AT_CIPMUX, sizeof(WIFI_ESP_CMD_AT_CIPMUX) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    
    rt_device_write(dev, 0, WIFI_ESP_CMD_AT_RST, sizeof(WIFI_ESP_CMD_AT_RST) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 5);
    
    rt_device_write(dev, 0, WIFI_ESP_CMD_AT_CWJAP, sizeof(WIFI_ESP_CMD_AT_CWJAP) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 10);

    rt_device_write(dev, 0, WIFI_ESP_CMD_AT_CIPSTART, sizeof(WIFI_ESP_CMD_AT_CIPSTART) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);

    
    

    return 0;
}

uint8_t WIFI_init(void)
{
    WIFI_ESP_07_init(device_WIFI_ESP_07);
}
FINSH_FUNCTION_EXPORT_ALIAS(WIFI_init, WIFI_init, Initialize the WIFI module.);

void WIFI_send(char *cmd)
{
    
    if (device_WIFI_ESP_07 != RT_NULL)
    {
        rt_device_write(device_WIFI_ESP_07, 0, cmd, strlen(cmd));
    }
    else
    {
    
    }
    
}
FINSH_FUNCTION_EXPORT_ALIAS(WIFI_send, WIFI_send, Send the WIFI command.);


void WIFI_socket_send(char *data)
{
    
    uint32_t len_data = 0;
    uint8_t AT_CIPSEND[32] = {0};
    
    len_data = strlen(data);
    
    if (device_WIFI_ESP_07 != RT_NULL)
    {
        //memcpy(AT_SISW, GSM_CMD_AT_SISW, len_cmd);
        sprintf(AT_CIPSEND, "%s%d\r\n", WIFI_ESP_CMD_AT_CIPSEND, len_data);
        
        rt_device_write(device_WIFI_ESP_07, 0, AT_CIPSEND, strlen(AT_CIPSEND));
        rt_thread_delay(5);
        rt_device_write(device_WIFI_ESP_07, 0, data, len_data);
    }
    else
    {
        
    }    
}
FINSH_FUNCTION_EXPORT_ALIAS(WIFI_socket_send, WIFI_socket, Send the WIFI data.);

#endif // RT_USING_FINSH






