#include "data_trans.h"

#include "COM_if.h"

#include "rtthread.h"
#include <rtdevice.h>
//#include "platform.h"

rt_uint8_t prio_COM_IO = 3;
rt_thread_t thread_COM_IO;


rt_uint8_t prio_COM_if = 5;
rt_thread_t thread_COM_if;

static uint8_t COM_IO_buf[UITD_COM_IO_BUF_SIZE] = {0};
static uint32_t COM_IO_buf_len = 0;

struct rt_device * device_COM_if;



uint8_t test_string[] = "Hello";
uint32_t    denyed_cnt = 0;

t_COM_pkt COM_pkt = {0};


int COM_trans_data_affirmed(void *param)
{
    t_COM_pkt *pkt = param;
    
    
    return 0;
}


int COM_trans_data_denyed(void *param)
{
    t_COM_pkt *pkt = param;
    
    denyed_cnt ++;
    
    return 0;
}

int COM_trans_data_timeout(void *param)
{
    t_COM_pkt *pkt = param;
    
    rt_kprintf("Trans_data timeout !\n");
    
    return 0;
}


void rt_thread_entry_COM_IO(void* parameter)
{
    int res = 0;
    int send_res = 0;
    int timer = 10;
//    rt_sem_init(&sem_UITD_sub_main_tx, "UITD_tx", 0, RT_IPC_FLAG_FIFO);
//    rt_sem_init(&sem_UITD_sub_main_rx, "UITD_rx", 0, RT_IPC_FLAG_FIFO);


    while(1)
    {
//        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
//        rt_device_read(IO_device, 0, &TM1638_key,sizeof(t_TM1638_key));
        
//        res = COM_IO_check_rec(COM_IO_buf, &COM_IO_buf_len);
//        if (res == 0)
//        {
//            if (COM_IO_buf_len) 
//            {
//                //rt_kprintf("COM_IO_rec num: %d\n", COM_IO_buf_len);
//                timer = 10;
//                while(COM_check_send_idle() == 0)
//                {
//                    if (timer <= 0) break;
//                    rt_thread_delay(1);
//                    timer --;
//                    
//                }
//                
////                COM_if_CMD_send_affrim_cb(&COM_trans_data_affirmed);
////                COM_if_CMD_send_denyed_cb(&COM_trans_data_denyed);
////                COM_if_ack_timeout_cb(COM_trans_data_timeout);
////                send_res = COM_send_COM_data(COM_IO_buf, COM_IO_buf_len, COM_ACK_TIMEOUT_DEFAULT);
////                if (send_res < 0)
////                {
////                    rt_kprintf("COM data send error : %d\n", send_res);
////                }

//                // Song ,just for testing.
//                COM_if_CMD_send_affrim_cb(&COM_trans_data_affirmed);
//                COM_if_CMD_send_denyed_cb(&COM_trans_data_denyed);
//                COM_if_ack_timeout_cb(NULL);
//                send_res = COM_send_COM_data(COM_IO_buf, COM_IO_buf_len, 0);
//                if (send_res < 0)
//                {
//                    rt_kprintf("COM data send error : %d\n", send_res);
//                }

//                
//            }
//            
//        }
//        else
//        {
//            rt_kprintf("COM_IO_rec error: %d\n", res);
//        }
        
        rt_thread_delay(1);
    }
    
}

int COM_R_sysinfo_affirmed(void *param)
{
    rt_kprintf("sysinfo affirmed\n");
    return 0;
}

int COM_R_sysinfo_denyed(void *param)
{
    rt_kprintf("sysinfo denyed\n");

    return 0;
}

int COM_R_sysinfo_timeout(void *param)
{
    rt_kprintf("sysinfo timeout\n");

    return 0;
}



int COM_send_sys_info(void)
{
    t_COM_send send = {0};
    
    send.ctrl.cmd = COM_CMD_SEND_DATA;
    send.if_none_data = 0;
    send.if_malloc = 0;
    send.buf = test_string;
    send.data_len = sizeof(test_string) - 1;
    send.data_type = COM_TYPE_4_U_SYS_INFO;
    send.data_num = 1;
    
    return COM_send_CMD_data(&send, COM_ACK_TIMEOUT_DEFAULT);
}


int COM_require_sys_info(void)
{
    t_COM_send send = {0};
    
    send.ctrl.cmd = COM_CMD_REQUIRE;
    send.if_none_data = 0;
    send.if_malloc = 0;
    //send.buf = test_string;
    send.data_len = 0;
    send.data_type = COM_TYPE_5_R_SYS_INFO;
    send.data_num = 0;
    return COM_send_CMD_data(&send, COM_ACK_TIMEOUT_DEFAULT);
}






int COM_dev_uart_cfg(void *cfg, uint8_t num)
{
    t_COM_send send = {0};
    
    send.ctrl.cmd = COM_CMD_CTRL;
    send.if_none_data = 0;
    send.if_malloc = 1;
    send.buf = cfg;
    send.data_len = 0;
    send.data_type = COM_TYPE_7_C_UARTS_INIT;
    send.data_num = num;
    return COM_send_CMD_data(&send, COM_ACK_TIMEOUT_DEFAULT);
}


int COM_if_dev_uart_init(void)
{
    
    
}


int COM_send_COM_data(uint8_t *data, uint32_t len, uint32_t timeout)
{
    t_COM_send send = {0};
    uint8_t *buf = NULL;
    
    if ((data == NULL) || (len == 0)) return -1;
    
    buf = rt_malloc(len);
    if (buf == NULL) return -2;
    
    rt_memcpy(buf, data, len);
    
    send.ctrl.cmd = COM_CMD_SEND_DATA;
    send.if_none_data = 0;
    send.if_malloc = 1;
    send.buf = buf;
    send.data_len = len;
    send.data_type = COM_TYPE_6_U_COM_DATA;
    send.data_num = 1;
    
    return COM_send_CMD_data(&send, timeout);
}



int COM_deal_require(uint8_t cmd)
{
    
    switch(cmd)
    {
        case COM_TYPE_5_R_SYS_INFO:
            COM_send_sys_info();
            break;
        
        default:
            break;
    }

    return 0;
}

int COM_deal_ctrl(uint8_t cmd)
{
    
    switch(cmd)
    {
        case COM_TYPE_3_C_SYS_INIT:
            
            COM_send_CMD_affirm();
            break;
//        case COM_TYPE_6_U_COM_DATA:
//            
//            COM_send_CMD_affirm();
        default:
            break;
    }

    return 0;
}

int COM_deal_send_data(uint8_t cmd)
{
    
    switch(cmd)
    {
        case COM_TYPE_6_U_COM_DATA:
            
            COM_send_CMD_affirm();
        default:
            break;
    }

    return 0;
}

rt_err_t COM_if_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    //rt_sem_release(&sem_COM_if_rx);    

}

void rt_thread_entry_COM_if(void* parameter)
{
    mail_COM_send = rt_mb_create( "MB_send",16 , RT_IPC_FLAG_FIFO);
    //rt_sem_init(&sem_COM_if_rx, "COM_if_rx", 0, RT_IPC_FLAG_FIFO);
//    rt_sem_init(&sem_UITD_sub_main_rx, "UITD_rx", 0, RT_IPC_FLAG_FIFO);
    
    //rt_ringbuffer_init(&COM_if_ringbuf, COM_if_ring, sizeof(COM_if_ring));
    
    
    device_COM_if = rt_device_find("uart2");
    if(device_COM_if == RT_NULL)
    {
        rt_kprintf("uart2 not found!\r\n");
        return;
    }
    else
    {
        rt_device_open(device_COM_if, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        ////rt_device_set_rx_indicate(device_COM_if, COM_if_rx_ind);
        //rt_device_control(device_COM_if, RT_DEVICE_CTRL_CONFIG, &COM_if_config);
    }
    
    
    COM_if_init(&COM_pkt, device_COM_if, 
                &COM_deal_require,
                &COM_deal_ctrl,
                &COM_deal_send_data);
    

    while(1)
    {
//        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
//        rt_device_read(IO_device, 0, &TM1638_key,sizeof(t_TM1638_key));
        
        //if (rt_sem_take(&sem_COM_if_rx, 1) == RT_EOK)
        {
            COM_if_rec_handler();
        }
        
        rt_thread_delay(1);
    }
    
}
