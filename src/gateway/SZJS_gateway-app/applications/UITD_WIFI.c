

#include "UITD_WIFI.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "sys_misc.h"
#include "string.h"

#include "data_trans.h"
#include "WIFI_ESP_07.h"
#include "UITD.h"
#include "sys_status.h"

#include "sensor_sample.h"
#include "fire_alarm.h"

rt_uint8_t prio_UITD_WIFI = 8;
rt_thread_t thread_UITD_WIFI;

rt_uint8_t prio_UITD_2_WIFI = 8;
rt_thread_t thread_UITD_2_WIFI;


rt_uint8_t prio_UITD_WIFI_rec = 7;
rt_thread_t thread_UITD_WIFI_rec;


uint8_t f_UITD_WIFI_server_inited = 0;

uint8_t SZJS_WIFI_cfg_IP[4] = {114,216,166,66};
////uint16_t SZJS_cfg_port = 12345;
uint16_t SZJS_WIFI_cfg_port = 23366;

uint8_t XFZD_WIFI_cfg_IP[4] = {114,216,166,66};
////uint16_t XFZD_cfg_port = 12345;
uint16_t XFZD_WIFI_cfg_port = 23367;


t_server_handler    *p_UITD_WIFI_SZJS = NULL;
t_server_handler    *p_UITD_WIFI_XFZD = NULL;
t_server_handler    *p_UITD_WIFI_listen = NULL;



struct rt_semaphore sem_UITD_WIFI_rx;
struct rt_semaphore sem_UITD_WIFI_tx;




int UITD_WIFI_listen_send(int socket,char *data,int len)
{
    return WIFI_srv_write(socket, data, len);
}

int UITD_WIFI_listen_listen(uint8_t if_UDP, uint16_t listen_port, 
                                pFun_srv_accepted accepted,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_closed closed
                                )
{

}

int UITD_WIFI_listen_disconnected(void *data)
{


}

int UITD_WIFI_listen_received(void *data, uint32_t len)
{

}

int UITD_WIFI_listen_accepted(void *data)
{
    
    
}

int UITD_WIFI_listen_report_IP(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{


}


int UITD_WIFI_connect(void *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port,
                                pFun_srv_connect_fail connect_fail,
                                pFun_srv_connected connected,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_rejected rejected,
                                pFun_srv_closed closed,
                                pFun_srv_report_IP report_IP
                                )
{
    s_WIFI_srv_cfg cfg = {0};
    
    cfg.srv_type = if_UDP? WIFI_srv_UDP_client:WIFI_srv_TCP_client;
    //cfg.if_use_name = if_use_name;
    
//    if (cfg.if_use_name == 0)
    {
        cfg.srv_ip.addr[0] = IP[0];
        cfg.srv_ip.addr[1] = IP[1];
        cfg.srv_ip.addr[2] = IP[2];
        cfg.srv_ip.addr[3] = IP[3];
        cfg.srv_ip.port = port;
    }
//    else
//    {
//        return -1;
//    }
    
    
    return WIFI_connect(handler,
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

int UITD_WIFI_SZJS_connected(void *data)
{
    int socket = -1;
    int res = 0;
    
    socket = *(int *)data;
    
    p_UITD_WIFI_SZJS->socket = socket;
    p_UITD_WIFI_SZJS->if_connected = 1;
    
    UITD_sensor_handler_init(p_UITD_WIFI_SZJS);
    
    SYS_log( SYS_DEBUG_INFO, ("WIFI_SZJS connected! \n"));

    return 0;
}

int UITD_WIFI_disconnect(void *handler, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    WIFI_disconnect(socket);
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_WIFI %d disconnect. \n", svr_handler->socket));
    return 0;
}

int UITD_WIFI_disconnected(void *handler, void *data, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    if (svr_handler->if_connected)
    {
        SYS_log( SYS_DEBUG_ERROR, ("UITD_WIFI %d disconnected !!! \n", svr_handler->socket));
    }
    svr_handler->if_connected = 0;
    
    return 0;
}


int UITD_WIFI_send(void *handler, int socket,char *data,int len)
{
    int res = 0;
    
    rt_mutex_take(&mt_WIFI_send, RT_TICK_PER_SECOND * 5);
    res = WIFI_srv_write(socket, data, len);
    rt_mutex_release(&mt_WIFI_send);
    
    return res;
}


int UITD_WIFI_received(void *handler, void *data, uint32_t len)
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

int UITD_WIFI_report_IP(void *handler, uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;

    svr_handler->local_IP[0] = local_IP[0];
    svr_handler->local_IP[1] = local_IP[1];
    svr_handler->local_IP[2] = local_IP[2];
    svr_handler->local_IP[3] = local_IP[3];
    svr_handler->local_port = local_port;
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_WIFI %d IP port : %d.%d.%d.%d:%d \n", 
                svr_handler->socket,
                svr_handler->local_IP[0],
                svr_handler->local_IP[1],
                svr_handler->local_IP[2],
                svr_handler->local_IP[3],
                svr_handler->local_port
                ));
}

int UITD_WIFI_XFZD_connected(void *data)
{
    int socket = -1;
    
    socket = *(int *)data;
    
    p_UITD_WIFI_XFZD->socket = socket;
    p_UITD_WIFI_XFZD->if_connected = 1;

    UITD_sensor_handler_2_init(p_UITD_WIFI_XFZD);

    
    SYS_log( SYS_DEBUG_INFO, ("WIFI_XFZD connected! \n"));
    return 0;
}





void rt_thread_entry_UITD_WIFI(void* parameter)
{
    int res = 0;
    
    rt_sem_init(&sem_UITD_WIFI_tx, "UITD_WIFI_tx", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_UITD_WIFI_rx, "UITD_WIFI_rx", 0, RT_IPC_FLAG_FIFO);


    while(sys_config.board_type_checked == 0)
    {
        rt_thread_delay(2);
    }
    
    
    
    p_UITD_WIFI_SZJS = UITD_init(&UITD_WIFI_send,
                            &UITD_WIFI_connect,
                            &UITD_WIFI_SZJS_connected,
                            &UITD_WIFI_disconnect,
                            &UITD_WIFI_disconnected,
                            NULL,
                            &UITD_WIFI_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_WIFI_report_IP
                            );
    
    if (p_UITD_WIFI_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("UITD_WIFI_SZJS init failed !\n"));
        while(1) rt_thread_delay(5);
    }
    
    p_UITD_WIFI_SZJS->if_UITD = sys_config.server_SZJS.if_UITD;
    
    UITD_file_trans_init(p_UITD_WIFI_SZJS, &file_trans_cb, 1);
    sys_config.server_1_inited = 1;
    
    
    while(1)
    {
        res = rt_sem_take(&sem_WIFI_up, 1);
        if (res == RT_EOK)
        {
            
            {
                res = UITD_connect(p_UITD_WIFI_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD connect failed !\n"));
                //while(1) rt_thread_delay(5);
                break;
            }
            while(p_UITD_WIFI_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
            {
                sys_status_set(&sys_ctrl, SYS_state_testing);
            }
            else
            {
                sys_status_set(&sys_ctrl, SYS_state_working);
            }
            sys_config.server_WIFI_connected = 1;
            break;
        }
    }
    
    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        sys_config.test_flag.WIFI = 1;
        while(1) rt_thread_delay(10);
    }

    
    while(1)
    {
        
        if ((p_UITD_WIFI_SZJS != NULL) && (p_UITD_WIFI_SZJS->if_connected == 0))
        {
            sys_status_set(&sys_ctrl, SYS_state_reconnect);
            

            {
                res = UITD_connect(p_UITD_WIFI_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD_WIFI_SZJS connect failed !\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*30);

                p_UITD_WIFI_SZJS->if_connected = 0;
                continue;
            }
            while(p_UITD_WIFI_SZJS->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            
        }
        else if ((p_UITD_WIFI_SZJS != NULL) && (p_UITD_WIFI_SZJS->if_connected == 1) && (p_UITD_WIFI_SZJS->status == UITD_svc_status_alive))
        {
            ////sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_WIFI_SZJS->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_WIFI_SZJS);
            UITD_send_handler(p_UITD_WIFI_SZJS);
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




void rt_thread_entry_UITD_2_WIFI(void* parameter)
{
    int res = 0;
    
    while(sys_config.board_type_checked == 0)
    {
        rt_thread_delay(2);
    }
 
    

    p_UITD_WIFI_XFZD = UITD_init(&UITD_WIFI_send,
                            &UITD_WIFI_connect,
                            &UITD_WIFI_XFZD_connected,
                            &UITD_WIFI_disconnect,
                            &UITD_WIFI_disconnected,
                            NULL,
                            &UITD_WIFI_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_WIFI_report_IP
                            );
    if (p_UITD_WIFI_XFZD == NULL)
    {
        SYS_log( SYS_DEBUG_ERROR, ("UITD_WIFI_XFZD init failed !\n"));
    }

    p_UITD_WIFI_XFZD->if_UITD = sys_config.server_XFZD.if_UITD;

    UITD_file_trans_init(p_UITD_WIFI_XFZD, &file_trans_cb, 1);
    

    sys_config.server_2_inited = 1;
    
    
    while(1)
    {
        rt_thread_delay(10);
        {
            

            {
                res = UITD_connect(p_UITD_WIFI_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD WIFI connect failed !\n"));
                //while(1) rt_thread_delay(5);
                break;
            }
            while(p_UITD_WIFI_XFZD->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            break;
        }
    }

    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        sys_config.test_flag.WIFI = 1;
        while(1) rt_thread_delay(10);
    }
    
    
    while(1)
    {

        if ((p_UITD_WIFI_XFZD != NULL) && (p_UITD_WIFI_XFZD->if_connected == 0))
        {
            sys_status_set(&sys_ctrl, SYS_state_reconnect);
            

            {
                res = UITD_connect(p_UITD_WIFI_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, 0);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_INFO, ("UITD_WIFI_XFZD connect failed ! retry ...\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*30);
                
                p_UITD_WIFI_XFZD->if_connected = 0;
                continue;
            }
            while(p_UITD_WIFI_XFZD->if_connected == 0)
            {
                rt_thread_delay(5);
            }
            
            sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else if ((p_UITD_WIFI_XFZD != NULL) && (p_UITD_WIFI_XFZD->if_connected == 1) && (p_UITD_WIFI_XFZD->status == UITD_svc_status_alive))
        {
            sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_WIFI_XFZD->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_WIFI_XFZD);
            UITD_send_handler(p_UITD_WIFI_XFZD);
        }        

        
        rt_thread_delay( 1);
    }
    
}



void rt_thread_entry_UITD_WIFI_rec(void* parameter)
{
    int res = 0;

    while(sys_config.server_1_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        UITD_server(p_UITD_WIFI_SZJS);
        UITD_server(p_UITD_WIFI_XFZD);
        rt_thread_delay(1);
    }

}


#ifdef RT_USING_FINSH
#include <finsh.h>


int UITD_WIFI_server_info(void)
{
    
    if (p_UITD_WIFI_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD WIFI SZJS is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD WIFI SZJS server info: \n"));

        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_WIFI_SZJS->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_WIFI_SZJS->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_WIFI_SZJS->IP[0],
                                    p_UITD_WIFI_SZJS->IP[1],
                                    p_UITD_WIFI_SZJS->IP[2],
                                    p_UITD_WIFI_SZJS->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_WIFI_SZJS->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_WIFI_SZJS->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_WIFI_SZJS->local_IP[0],
                                    p_UITD_WIFI_SZJS->local_IP[1],
                                    p_UITD_WIFI_SZJS->local_IP[2],
                                    p_UITD_WIFI_SZJS->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_WIFI_SZJS->local_port));
        
        
    }
    
    if (p_UITD_WIFI_XFZD == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD WIFI XFZD is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD WIFI XFZD server info: \n"));
        
        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_WIFI_XFZD->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_WIFI_XFZD->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_WIFI_XFZD->IP[0],
                                    p_UITD_WIFI_XFZD->IP[1],
                                    p_UITD_WIFI_XFZD->IP[2],
                                    p_UITD_WIFI_XFZD->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_WIFI_XFZD->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_WIFI_XFZD->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_WIFI_XFZD->local_IP[0],
                                    p_UITD_WIFI_XFZD->local_IP[1],
                                    p_UITD_WIFI_XFZD->local_IP[2],
                                    p_UITD_WIFI_XFZD->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_WIFI_XFZD->local_port));
        
    }
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(UITD_WIFI_server_info, UITD_WIFI_info, Print the UITD WIFI servers infomation.);

#endif // 

