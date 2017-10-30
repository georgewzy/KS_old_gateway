

#include "UITD_main.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "sys_misc.h"
#include "string.h"

#include "data_trans.h"
#include "GSM_MG301.h"
#include "UITD.h"
#include "sys_status.h"

#include "sensor_sample.h"
#include "fire_alarm.h"

rt_uint8_t prio_UITD_main = 8;
rt_thread_t thread_UITD_main;

rt_uint8_t prio_UITD_main_rec = 7;
rt_thread_t thread_UITD_main_rec;


uint8_t f_UITD_server_inited = 0;

uint8_t SZJS_cfg_IP[4] = {114,216,166,66};
////uint16_t SZJS_cfg_port = 12345;
uint16_t SZJS_cfg_port = 23366;

uint8_t XFZD_cfg_IP[4] = {114,216,166,66};
////uint16_t XFZD_cfg_port = 12345;
uint16_t XFZD_cfg_port = 23366;


t_server_handler    *p_UITD_SZJS = NULL;
t_server_handler    *p_UITD_XFZD = NULL;
t_server_handler    *p_UITD_listen = NULL;


//struct rt_device_pin_mode pin_mode_wifi_shut = {PIN_WIFI_SHUT, PIN_MODE_OUTPUT};
//struct rt_device_pin_mode pin_mode_wifi_rst = {PIN_WIFI_RST, PIN_MODE_OUTPUT};

//struct rt_device_pin_status pin_status_wifi_shut = {PIN_WIFI_SHUT, PIN_HIGH};
//struct rt_device_pin_status pin_status_wifi_rst = {PIN_WIFI_RST, PIN_HIGH};

struct rt_semaphore sem_UITD_main_rx;
struct rt_semaphore sem_UITD_main_tx;

struct rt_device * device_sub_mcu;
struct rt_device * deviec_sub_mcu_gpio;

struct rt_device * device_com;
struct rt_device * deviec_com_gpio;


static rt_uint8_t test_hello[] = "AT";

static rt_uint8_t COM_pkt_01[] = {
'@','@',
0x02,
0x02,
0x05,0x00,
0x01,0x02,0x03,0x04,0x05,
0x18,
'#','#'
};

static rt_uint8_t COM_pkt_02[] = {
'@','@',
0x02,
0x03,
0x00,0x00,
    
0x05,
'#','#'
};

static rt_uint8_t COM_pkt_03[] = {
'@','@',
0x02,
0x01,
0x02,0x00,
0x03,0x00,
0x00,
'#','#'
};

static rt_uint8_t COM_pkt_04[] = {
'@','@',
0x02,
0x01,
0x02,0x00,
0x03,0x00,
0x08,
'#','#'
};
static rt_uint8_t COM_pkt_05[] = {
'@','@',
0x02,
0x04,
0x02,0x00,
0x05,0x00,    
0x0D,
'#','#'
};




int UITD_listen_send(int socket,char *data,int len)
{
    return GPRS_srv_write(socket, data, len);
}

int UITD_listen_listen(uint8_t if_UDP, uint16_t listen_port, 
                                pFun_srv_accepted accepted,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_closed closed
                                )
{
    s_GPRS_srv_cfg cfg = {0};
    
    if (if_UDP)
    {
        cfg.srv_type = GPRS_srv_UDP_server;
    }
    else
    {
        cfg.srv_type = GPRS_srv_TCP_server;
    }
    
    cfg.listen_port = listen_port;
    
    return GPRS_listen(&cfg, accepted, disconnected, received, closed);
}

int UITD_listen_disconnected(void *data)
{


}

int UITD_listen_received(void *data, uint32_t len)
{

}

int UITD_listen_accepted(void *data)
{
    
    
}

int UITD_listen_report_IP(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{


}

//int UITD_SJZS_send_data(int socket,char *data,int len)
//{
//    
//    return 0;
//}

int UITD_GPRS_connect(uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port, 
                                pFun_srv_connect_fail connect_fail,
                                pFun_srv_connected connected,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_rejected rejected,
                                pFun_srv_closed closed,
                                pFun_srv_report_IP report_IP
                                )
{
    s_GPRS_srv_cfg cfg = {0};
    
    cfg.srv_type = if_UDP? GPRS_srv_UDP_client:GPRS_srv_TCP_client;
    cfg.if_use_name = if_use_name;
    
    if (cfg.if_use_name == 0)
    {
        cfg.srv_ip.addr[0] = IP[0];
        cfg.srv_ip.addr[1] = IP[1];
        cfg.srv_ip.addr[2] = IP[2];
        cfg.srv_ip.addr[3] = IP[3];
        cfg.srv_ip.port = port;
    }
    else
    {
        strcpy(cfg.svr_name.name, IP);
        cfg.svr_name.port = port;
    }
    
    
    return GPRS_connect(&cfg,
                        connect_fail,
                        connected,
                        disconnected,
                        received,
                        rejected,
                        closed,
                        report_IP
                        );
}

int UITD_SZJS_connected(void *data)
{
    int socket = -1;
    int res = 0;
    
    socket = *(int *)data;
    
    p_UITD_SZJS->socket = socket;
    p_UITD_SZJS->if_connected = 1;
    
    // Init the sensor sample service
    if (p_sensor_cb != NULL)
    {
        p_sensor_cb->handler = p_UITD_SZJS;
        p_sensor_cb->status = e_sensor_sta_init;
    }

    if (p_com_bus_cb != NULL)
    {
        p_com_bus_cb->handler = p_UITD_SZJS;
        p_com_bus_cb->status = com_bus_status_init;
    }

    if (p_IO_input_cb != NULL)
    {
        p_IO_input_cb->handler = p_UITD_SZJS;
        p_IO_input_cb->status = IO_input_status_init;
    }
    
    if (p_output_ctrl_cb != NULL)
    {
        p_output_ctrl_cb->handler = p_UITD_SZJS;
        p_output_ctrl_cb->status = e_output_sta_init;
    }
    
    rt_kprintf("SZJS connected! \n");
    
    
    
//    
//    p_UITD_listen = UITD_init(&UITD_listen_send,
//                            NULL,
//                            NULL,
//                            NULL,
//                            &UITD_listen_disconnected,
//                            NULL,
//                            &UITD_listen_received,
//                            NULL,
//                            NULL,
//                            &UITD_listen_listen,
//                            &UITD_listen_accepted,
//                            &UITD_listen_report_IP
//                            );
//    if (p_UITD_listen == NULL)
//    {
//        rt_kprintf("UITD_listen init failed !\n");
//    }
//    rt_kprintf("Listen inited! \n");

//    res = UITD_listen(p_UITD_listen, 1, p_UITD_SZJS->local_port);
//            
//    if (res < 0)
//    {
//        rt_kprintf("UITD listen failed !\n");
//        //while(1) rt_thread_delay(5);
//    }

    
    return 0;
}

int UITD_SZJS_disconnect(int socket)
{
    rt_kprintf("UITD_SZJS disconnect. \n");
    return 0;
}

int UITD_SZJS_disconnected(void *data, int socket)
{

    rt_kprintf("UITD_SZJS disconnected !!! \n");
    
    p_UITD_SZJS->if_connected = 0;
    
    return 0;
}




int UITD_SZJS_send(int socket,char *data,int len)
{
    return GPRS_srv_write(socket, data, len);
}


int UITD_SZJS_received(void *data, uint32_t len)
{
    int res = 0;
    
	res = rt_ringbuffer_put(&p_UITD_SZJS->ring_buf, data, len);
	if (res != len)
	{
		rt_kprintf("ringbuf write error : %d ", res);
		return -GB_E_BUF_WR;
	}
}

int UITD_SZJS_report_IP(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{

    p_UITD_SZJS->local_IP[0] = local_IP[0];
    p_UITD_SZJS->local_IP[1] = local_IP[1];
    p_UITD_SZJS->local_IP[2] = local_IP[2];
    p_UITD_SZJS->local_IP[3] = local_IP[3];
    p_UITD_SZJS->local_port = local_port;
    
    rt_kprintf("UITD_SZJS IP port : %d.%d.%d.%d:%d \n", 
                p_UITD_SZJS->local_IP[0],
                p_UITD_SZJS->local_IP[1],
                p_UITD_SZJS->local_IP[2],
                p_UITD_SZJS->local_IP[3],
                p_UITD_SZJS->local_port
                );
}

int UITD_XFZD_connected(void *data)
{
    int socket = -1;
    
    socket = *(int *)data;
    
    p_UITD_XFZD->socket = socket;
    p_UITD_XFZD->if_connected = 1;

    rt_kprintf("XFZD connected! \n");
    return 0;
}

int UITD_XFZD_disconnect(int socket)
{
    
    return 0;
}

int UITD_XFZD_disconnected(void *data, int socket)
{

    return 0;
}
int UITD_XFZD_send(int socket,char *data,int len)
{
    return GPRS_srv_write(socket, data, len);
}


int UITD_XFZD_received(void *data, uint32_t len)
{
    int res = 0;
    
	res = rt_ringbuffer_put(&p_UITD_XFZD->ring_buf, data, len);
	if (res != len)
	{
		rt_kprintf("ringbuf write error : %d ", res);
		return -GB_E_BUF_WR;
	}
}

int UITD_XFZD_report_IP(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{


}




void rt_thread_entry_UITD_main(void* parameter)
{
    int res = 0;
    
    rt_sem_init(&sem_UITD_main_tx, "UITD_main_tx", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_UITD_main_rx, "UITD_main_rx", 0, RT_IPC_FLAG_FIFO);
    
 
//    rt_thread_delay(RT_TICK_PER_SECOND * 2);
//    ISP_test("/boot/SZJS_UITD_sub.hex");
    
    p_UITD_SZJS = UITD_init(&UITD_SZJS_send,
                            &UITD_GPRS_connect,
                            &UITD_SZJS_connected,
                            &UITD_SZJS_disconnect,
                            &UITD_SZJS_disconnected,
                            NULL,
                            &UITD_SZJS_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_SZJS_report_IP
                            );
    if (p_UITD_SZJS == NULL)
    {
        rt_kprintf("UITD_SZJS init failed !\n");
        while(1) rt_thread_delay(5);
    }

//    p_UITD_XFZD = UITD_init(&UITD_XFZD_send,
//                            &UITD_GPRS_connect,
//                            &UITD_XFZD_connected,
//                            &UITD_XFZD_disconnect,
//                            &UITD_XFZD_disconnected,
//                            NULL,
//                            &UITD_XFZD_received,
//                            NULL,
//                            NULL,
//                            NULL,
//                            NULL,
//                            &UITD_XFZD_report_IP
//                            );
//    if (p_UITD_XFZD == NULL)
//    {
//        rt_kprintf("UITD_XFZD init failed !\n");
//        while(1) rt_thread_delay(5);
//    }


    //sys_status_set(&sys_ctrl, SYS_state_inited);
    
    f_UITD_server_inited = 1;
    
    while(1)
    {
        res = rt_sem_take(&sem_GSM_up, 1);
        if (res == RT_EOK)
        {
            
            
            if (svr_use_name)
            {
                res = UITD_connect(p_UITD_SZJS, 1, 1, svr_if_name, svr_if_port);
            }
            else
            {
                res = UITD_connect(p_UITD_SZJS, 1, 0, svr_if_ip, svr_if_port);
            }
            
            if (res < 0)
            {
                rt_kprintf("UITD connect failed !\n");
                while(1) rt_thread_delay(5);
            }
            while(p_UITD_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            sys_status_set(&sys_ctrl, SYS_state_working);
            
//            if (svr_use_name)
//            {
//                res = UITD_connect(p_UITD_XFZD, 1, 1, svr_if_name, svr_if_port);
//            }
//            else
//            {
//                res = UITD_connect(p_UITD_XFZD, 1, 0, svr_if_ip, svr_if_port);
//            }
//            
//            if (res < 0)
//            {
//                rt_kprintf("UITD connect failed !\n");
//                while(1) rt_thread_delay(5);
//            }
//            while(p_UITD_XFZD->if_connected == 0)
//            {
//                rt_thread_delay(5);
//            }
            
            break;
        }
    }
    
    
    while(1)
    {
        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
        
        if (p_UITD_SZJS->if_connected == 0)
        {
            sys_status_set(&sys_ctrl, SYS_state_reconnect);
            
            if (svr_use_name)
            {
                res = UITD_connect(p_UITD_SZJS, 1, 1, svr_if_name, svr_if_port);
            }
            else
            {
                res = UITD_connect(p_UITD_SZJS, 1, 0, svr_if_ip, svr_if_port);
            }
            
            if (res < 0)
            {
                rt_kprintf("UITD_SZJS connect failed !\n");
                while(1) rt_thread_delay(5);
            }
            while(p_UITD_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else 
        {
            UITD_service_handler(p_UITD_SZJS);
            UITD_send_handler(p_UITD_SZJS);
        }
        
        ////COM_if_send_handler();
        ////UITD_send_handler(p_UITD_XFZD);
        ////UITD_service_handler(p_UITD_XFZD);
//        if (p_UITD_listen)
//        {
//            UITD_send_handler(p_UITD_listen);
//            if (p_UITD_SZJS->if_connected)
//            {
//                UITD_service_handler(p_UITD_listen);
//            }
//        }
        
        if (sys_fw_update_reboot)
        {
            sys_status_set(&sys_ctrl, SYS_state_PWR_OFF);
            rt_thread_delay(RT_TICK_PER_SECOND * 5);
            sys_reboot();
        }
        
        rt_thread_delay( 1);
    }
    
}



void rt_thread_entry_UITD_main_rec(void* parameter)
{
    int res = 0;

    while(f_UITD_server_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        UITD_server(p_UITD_SZJS);
        ////UITD_server(p_UITD_XFZD);
        rt_thread_delay(1);
    }

}


#ifdef RT_USING_FINSH
#include <finsh.h>

int sys_fw_upgrade(void)
{
    return UITD_firmware_upgrade(p_UITD_SZJS);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_fw_upgrade, fw_upgrade, Upgrade the firmware.);


int sys_file_download_success(void *data)
{
    rt_kprintf("File download success.\n");
    return 0;
}
int sys_file_download_fail(void *data)
{
    rt_kprintf("File download failed.\n");
    return 0;
}

int sys_file_download(uint8_t *filename)
{
    
    return UITD_download_file(&p_UITD_SZJS->FW, filename, &sys_file_download_success, &sys_file_download_fail);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_file_download, file_download, Download file from server.);

int sys_sensor_send_data(uint16_t data)
{
    return UITD_sensor_upload_rough_data(p_UITD_SZJS, 1, sensor_out_type_4_20mA, data);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_sensor_send_data, sensor_data, Send sensor rough data.);

#endif // 




















