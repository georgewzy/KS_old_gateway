

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
#include "transparent.h"
#include "pro_ctrl.h"

rt_uint8_t prio_UITD_main = 8;
rt_thread_t thread_UITD_main;

rt_uint8_t prio_UITD_2_main = 8;
rt_thread_t thread_UITD_2_main;


rt_uint8_t prio_UITD_GPRS_rec = 7;
rt_thread_t thread_UITD_GPRS_rec;


uint8_t f_UITD_server_inited = 0;

uint8_t SZJS_cfg_IP[4] = {114,216,166,66};
////uint16_t SZJS_cfg_port = 12345;
uint16_t SZJS_cfg_port = 23366;

uint8_t XFZD_cfg_IP[4] = {114,216,166,66};
////uint16_t XFZD_cfg_port = 12345;
uint16_t XFZD_cfg_port = 23367;


t_server_handler    *p_UITD_SZJS = NULL;
t_server_handler    *p_UITD_XFZD = NULL;
t_server_handler    *p_UITD_listen = NULL;



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




int UITD_sensor_handler_init(t_server_handler *handler)
{
    int i = 0;
    
    // Init the sensor sample service
    if (p_sensor_cb != NULL)
    {
        p_sensor_cb->handler = handler;
        if (p_sensor_cb->status == e_sensor_sta_idle)
        {
            p_sensor_cb->status = e_sensor_sta_init;
        }
    }

    
    for (i=0;i<FA_COM_BUS_MAX;i++)
    {
        if (p_FA_com_bus_cb[i] != NULL)
        {
            p_FA_com_bus_cb[i]->handler = handler;
            if (p_FA_com_bus_cb[i]->status == com_bus_status_idle)
            {
                p_FA_com_bus_cb[i]->status = com_bus_status_init;
            }
        }
    }    
    
    if (p_transparent_cb != NULL)
    {
        p_transparent_cb->handler = handler;
        if (p_transparent_cb->status == TP_idle)
        {
            p_transparent_cb->status = TP_init;
        }
    }
    
    if (p_IO_input_cb != NULL)
    {
        p_IO_input_cb->handler = handler;
        if (p_IO_input_cb->status == IO_input_status_idle)
        {
            p_IO_input_cb->status = IO_input_status_init;
        }
    }

    
    if (p_output_ctrl_cb != NULL)
    {
        p_output_ctrl_cb->handler = handler;
        if (p_output_ctrl_cb->status == e_output_sta_idle)
        {
            p_output_ctrl_cb->status = e_output_sta_init;
        }
    }
    
    
    return 0;
}


int UITD_sensor_handler_2_init(t_server_handler *handler)
{
    int i = 0;
    
    // Init the sensor sample service
    if (p_sensor_cb != NULL)
    {
        p_sensor_cb->handler_2 = handler;
        if (p_sensor_cb->status == e_sensor_sta_idle)
        {
            p_sensor_cb->status = e_sensor_sta_init;
        }
    }

    
    for (i=0;i<FA_COM_BUS_MAX;i++)
    {
        if (p_FA_com_bus_cb[i] != NULL)
        {
            p_FA_com_bus_cb[i]->handler_2 = handler;
            if (p_FA_com_bus_cb[i]->status == com_bus_status_idle)
            {
                p_FA_com_bus_cb[i]->status = com_bus_status_init;
            }
        }
    }
    
    if (p_transparent_cb != NULL)
    {
        p_transparent_cb->handler_2 = handler;
        if (p_transparent_cb->status == TP_idle)
        {
            p_transparent_cb->status = TP_init;
        }
    }    
    
    if (p_IO_input_cb != NULL)
    {
        p_IO_input_cb->handler_2 = handler;
        if (p_IO_input_cb->status == IO_input_status_idle)
        {
            p_IO_input_cb->status = IO_input_status_init;
        }
    }


    
    if (p_output_ctrl_cb != NULL)
    {
        p_output_ctrl_cb->handler_2 = handler;
        if (p_output_ctrl_cb->status == e_output_sta_idle)
        {
            p_output_ctrl_cb->status = e_output_sta_init;
        }
    }
    
//    if (p_PRO_output_ctrl_cb != NULL)
//    {
//        p_PRO_output_ctrl_cb->handler_2 = handler;
//        if (p_PRO_output_ctrl_cb->status == e_PRO_output_sta_idle)
//        {
//            p_PRO_output_ctrl_cb->status = e_PRO_output_sta_init;
//        }
//    }    

    return 0;
}


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

int UITD_GPRS_connect(void *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port,
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
    
    
    return GPRS_connect(handler,
                        &cfg,
                        connect_fail,
                        connected,
                        disconnected,
                        received,
                        rejected,
                        closed,
                        report_IP
                        );
}

int UITD_GPRS_SZJS_connected(void *data)
{
    int socket = -1;
    int res = 0;
    
    socket = *(int *)data;
    
    p_UITD_SZJS->socket = socket;
    p_UITD_SZJS->if_connected = 1;
    
    UITD_sensor_handler_init(p_UITD_SZJS);
    
    SYS_log( SYS_DEBUG_INFO, ("SZJS connected! \n"));

    return 0;
}

int UITD_GPRS_disconnect(void *handler, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    GPRS_disconnect(socket);
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_GPRS %d disconnect. \n", svr_handler->socket));
    return 0;
}

int UITD_GPRS_disconnected(void *handler, void *data, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    if (svr_handler->if_connected)
    {
        SYS_log( SYS_DEBUG_ERROR, ("UITD_GPRS %d disconnected !!! \n", svr_handler->socket));
    }
    svr_handler->if_connected = 0;
    
    return 0;
}


int UITD_GPRS_send(void *handler, int socket,char *data,int len)
{
    int res = 0;
    
    rt_mutex_take(&mt_GSM_send, RT_TICK_PER_SECOND * 5);
    res = GPRS_srv_write(socket, data, len);
    rt_mutex_release(&mt_GSM_send);
    
    return res;
}


int UITD_GPRS_received(void *handler, void *data, uint32_t len)
{
    int res = 0;
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
	res = rt_ringbuffer_put(&svr_handler->ring_buf, data, len);
	if (res != len)
	{
		SYS_log( SYS_DEBUG_ERROR, ("ringbuf %d write error : %d ", svr_handler->socket, res));
		return -GB_E_BUF_WR;
	}
}

int UITD_GPRS_report_IP(void *handler, uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;

    svr_handler->local_IP[0] = local_IP[0];
    svr_handler->local_IP[1] = local_IP[1];
    svr_handler->local_IP[2] = local_IP[2];
    svr_handler->local_IP[3] = local_IP[3];
    svr_handler->local_port = local_port;
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_GPRS %d IP port : %d.%d.%d.%d:%d \n", 
                svr_handler->socket,
                svr_handler->local_IP[0],
                svr_handler->local_IP[1],
                svr_handler->local_IP[2],
                svr_handler->local_IP[3],
                svr_handler->local_port
                ));
}

int UITD_GPRS_XFZD_connected(void *data)
{
    int socket = -1;
    
    socket = *(int *)data;
    
    p_UITD_XFZD->socket = socket;
    p_UITD_XFZD->if_connected = 1;

    UITD_sensor_handler_2_init(p_UITD_XFZD);

    
    SYS_log( SYS_DEBUG_INFO, ("XFZD connected! \n"));
    return 0;
}




void rt_thread_entry_UITD_main(void* parameter)
{
    int res = 0;
    
    rt_sem_init(&sem_UITD_main_tx, "UITD_main_tx", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_UITD_main_rx, "UITD_main_rx", 0, RT_IPC_FLAG_FIFO);


    while(sys_config.board_type_checked == 0)
    {
        rt_thread_delay(2);
    }
    
    
 
//    rt_thread_delay(RT_TICK_PER_SECOND * 2);
//    ISP_test("/boot/SZJS_UITD_sub.hex");
    
    p_UITD_SZJS = UITD_init(&UITD_GPRS_send,
                            &UITD_GPRS_connect,
                            &UITD_GPRS_SZJS_connected,
                            &UITD_GPRS_disconnect,
                            &UITD_GPRS_disconnected,
                            NULL,
                            &UITD_GPRS_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_GPRS_report_IP
                            );
    
    if (p_UITD_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("UITD_SZJS init failed !\n"));
        while(1) rt_thread_delay(5);
    }
    
    p_UITD_SZJS->if_UITD = sys_config.server_SZJS.if_UITD;
    
    UITD_file_trans_init(p_UITD_SZJS, &file_trans_cb, 3);
    //sys_status_set(&sys_ctrl, SYS_state_inited);
    
    //sys_config.server_main_inited = 1;
    sys_config.server_1_inited = 1;
    
    
    while(1)
    {
        res = rt_sem_take(&sem_GSM_up, 1);
        if (res == RT_EOK)
        {
            
            if (sys_config.server_SZJS.svr_use_name)
            {
                res = UITD_connect(p_UITD_SZJS, 1, 1, sys_config.server_SZJS.svr_if_name, sys_config.server_SZJS.svr_if_port, 0);
            }
            else
            {
                res = UITD_connect(p_UITD_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD connect failed !\n"));
                //while(1) rt_thread_delay(5);
                break;
            }
            while(p_UITD_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
            {
                //sys_status_set(&sys_ctrl, SYS_state_testing);
            }
            else
            {
                //sys_status_set(&sys_ctrl, SYS_state_working);
            }
            sys_config.server_main_connected = 1;
            break;
        }
    }
    
    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        while(1) rt_thread_delay(10);
    }

    
    while(1)
    {
        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
        
        if ((p_UITD_SZJS != NULL) && (p_UITD_SZJS->if_connected == 0))
        {
            sys_status_set(&sys_ctrl, SYS_state_reconnect);
            
            if (sys_config.server_SZJS.svr_use_name)
            {
                res = UITD_connect(p_UITD_SZJS, 1, 1, sys_config.server_SZJS.svr_if_name, sys_config.server_SZJS.svr_if_port, 0);
            }
            else
            {
                res = UITD_connect(p_UITD_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD_SZJS connect failed !\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*30);

                p_UITD_SZJS->if_connected = 0;
                continue;
            }
            while(p_UITD_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            //sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else if ((p_UITD_SZJS != NULL) && (p_UITD_SZJS->if_connected == 1) && (p_UITD_SZJS->status == UITD_svc_status_alive))
        {
            sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_SZJS->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_SZJS);
            UITD_send_handler(p_UITD_SZJS);
        }
   

        
        if (sys_fw_update_reboot)
        {
            sys_status_set(&sys_ctrl, SYS_state_PWR_OFF);
            rt_thread_delay(RT_TICK_PER_SECOND * 5);
            sys_reboot();
        }
        
        rt_thread_delay( 1);
    }
    
}




void rt_thread_entry_UITD_2_main(void* parameter)
{
    int res = 0;
    
    while(sys_config.board_type_checked == 0)
    {
        rt_thread_delay(2);
    }
 
//    rt_thread_delay(RT_TICK_PER_SECOND * 2);
//    ISP_test("/boot/SZJS_UITD_sub.hex");

    

    p_UITD_XFZD = UITD_init(&UITD_GPRS_send,
                            &UITD_GPRS_connect,
                            &UITD_GPRS_XFZD_connected,
                            &UITD_GPRS_disconnect,
                            &UITD_GPRS_disconnected,
                            NULL,
                            &UITD_GPRS_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_GPRS_report_IP
                            );
    if (p_UITD_XFZD == NULL)
    {
        SYS_log( SYS_DEBUG_ERROR, ("UITD_XFZD init failed !\n"));
        //while(1) rt_thread_delay(5);
    }

    p_UITD_XFZD->if_UITD = sys_config.server_XFZD.if_UITD;

    UITD_file_trans_init(p_UITD_XFZD, &file_trans_cb, 3);
    
    //sys_status_set(&sys_ctrl, SYS_state_inited);
    
    sys_config.server_2_inited = 1;
    
    
    while(1)
    {
        rt_thread_delay(10);
        if ((sys_config.server_main_connected) || (sys_config.server_eth_connected))
        {
            
            if (sys_config.server_XFZD.svr_use_name)
            {
                res = UITD_connect(p_UITD_XFZD, 1, 1, sys_config.server_XFZD.svr_if_name, sys_config.server_XFZD.svr_if_port, 0);
            }
            else
            {
                res = UITD_connect(p_UITD_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD connect failed !\n"));
                //while(1) rt_thread_delay(5);
                break;
            }
            while(p_UITD_XFZD->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            break;
        }
    }

    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        while(1) rt_thread_delay(10);
    }    
    
    while(1)
    {

        if ((p_UITD_XFZD != NULL) && (p_UITD_XFZD->if_connected == 0))
        {
            
            if (sys_config.server_XFZD.svr_use_name)
            {
                res = UITD_connect(p_UITD_XFZD, 1, 1, sys_config.server_XFZD.svr_if_name, sys_config.server_XFZD.svr_if_port, 0);
            }
            else
            {
                res = UITD_connect(p_UITD_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_INFO, ("UITD_XFZD connect failed ! retry ...\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*30);
                
                p_UITD_XFZD->if_connected = 0;
                continue;
            }
            while(p_UITD_XFZD->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            //sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else if ((p_UITD_XFZD != NULL) && (p_UITD_XFZD->if_connected == 1) && (p_UITD_XFZD->status == UITD_svc_status_alive))
        {
            sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_XFZD->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_XFZD);
            UITD_send_handler(p_UITD_XFZD);
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

        
//        if (sys_fw_update_reboot)
//        {
//            sys_status_set(&sys_ctrl, SYS_state_PWR_OFF);
//            rt_thread_delay(RT_TICK_PER_SECOND * 5);
//            sys_reboot();
//        }
        
        rt_thread_delay( 1);
    }
    
}



void rt_thread_entry_UITD_GPRS_rec(void* parameter)
{
    int res = 0;

    while(sys_config.server_1_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        UITD_server(p_UITD_SZJS);
        UITD_server(p_UITD_XFZD);
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
    SYS_log( SYS_DEBUG_INFO, ("File download success.\n"));
    return 0;
}
int sys_file_download_fail(void *data)
{
    SYS_log( SYS_DEBUG_INFO, ("File download failed.\n"));
    return 0;
}

int sys_file_download(uint8_t *filename)
{
    
    return UITD_download_file(p_UITD_SZJS, &p_UITD_SZJS->FW, filename, &sys_file_download_success, &sys_file_download_fail);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_file_download, file_download, Download file from server.);

int sys_sensor_send_data(uint16_t data)
{
    t_GB_ctrl_timestamp TS;   
    
    UITD_get_time(&TS);
    return UITD_sensor_upload_rough_data(p_UITD_SZJS, 1, sensor_out_type_4_20mA, data, &TS);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_sensor_send_data, sensor_data, Send sensor rough data.);

int sys_upload_file_info(uint8_t *name)
{
    
    return UITD_file_upload_file_info(p_UITD_SZJS, name);
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_upload_file_info, file_upload, Upload file to server.);

int UITD_server_info(void)
{
    
    if (p_UITD_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD SZJS is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD SZJS server info: \n"));

        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_SZJS->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_SZJS->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_SZJS->IP[0],
                                    p_UITD_SZJS->IP[1],
                                    p_UITD_SZJS->IP[2],
                                    p_UITD_SZJS->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_SZJS->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_SZJS->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_SZJS->local_IP[0],
                                    p_UITD_SZJS->local_IP[1],
                                    p_UITD_SZJS->local_IP[2],
                                    p_UITD_SZJS->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_SZJS->local_port));
        
        
    }
    
    if (p_UITD_XFZD == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD XFZD is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD XFZD server info: \n"));
        
        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_XFZD->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_XFZD->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_XFZD->IP[0],
                                    p_UITD_XFZD->IP[1],
                                    p_UITD_XFZD->IP[2],
                                    p_UITD_XFZD->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_XFZD->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_XFZD->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_XFZD->local_IP[0],
                                    p_UITD_XFZD->local_IP[1],
                                    p_UITD_XFZD->local_IP[2],
                                    p_UITD_XFZD->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_XFZD->local_port));
        
    }
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(UITD_server_info, UITD_info, Print the UITD servers infomation.);

#endif // 




















