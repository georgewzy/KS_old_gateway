

#include "UITD_eth.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "sys_misc.h"
#include "string.h"

#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/netif.h"


#include "data_trans.h"
//#include "GSM_MG301.h"
#include "UITD.h"
#include "sys_status.h"

#include "sensor_sample.h"
#include "fire_alarm.h"





struct rt_semaphore sem_eth_up;
struct rt_semaphore sem_eth_down;

rt_uint8_t prio_UITD_eth = 8;
rt_thread_t thread_UITD_eth;

rt_uint8_t prio_UITD_eth_2 = 8;
rt_thread_t thread_UITD_eth_2;


rt_uint8_t prio_UITD_eth_rec = 7;
rt_thread_t thread_UITD_eth_rec;

rt_uint8_t prio_eth_rx = 12;
rt_thread_t thread_eth_rx;

rt_uint8_t prio_eth_rx_2 = 12;
rt_thread_t thread_eth_rx_2;


struct rt_mutex mt_eth_send;

uint8_t f_UITD_eth_server_inited = 0;

static uint8_t eth_SZJS_cfg_IP[4] = {114,216,166,66};
////uint16_t SZJS_cfg_port = 12345;
uint16_t eth_SZJS_cfg_port = 23366;

uint8_t eth_XFZD_cfg_IP[4] = {114,216,166,66};
////uint16_t XFZD_cfg_port = 12345;
uint16_t eth_XFZD_cfg_port = 23367;


t_server_handler    *p_UITD_eth_SZJS = NULL;
t_server_handler    *p_UITD_eth_XFZD = NULL;
t_server_handler    *p_UITD_eth_listen = NULL;

uint8_t *p_eth_SZJS_buf = NULL;
uint8_t *p_eth_XFZD_buf = NULL;
int eth_SZJS_rec_len = 0;
int eth_XFZD_rec_len = 0;



struct rt_semaphore sem_UITD_eth_rx;
struct rt_semaphore sem_UITD_eth_tx;


static rt_uint8_t test_hello[] = "AT";



int eth_connect(void *handler,
                s_eth_srv_cfg *cfg,
                pFun_eth_connect_fail connect_fail,
                pFun_eth_connected connected,
                pFun_eth_disconnected  disconnected,
                pFun_eth_received  received,
                pFun_eth_rejected  rejected,
                pFun_eth_closed    closed,
                pFun_eth_report_IP report_IP
                )
{
    t_server_handler *srv_handler = (t_server_handler *)handler;
    int res = 0;
    int sock = -1;
	struct sockaddr_in local;
	struct sockaddr_in to;
//    s_eth_service *eth_service = NULL;
    struct netif * netif;
    struct hostent *host = NULL;
    
//    while(sys_config.eth_link_up == 0)
//    {
//        rt_thread_delay(1);
//    }
    
    
//    eth_service = eth_srv_create(cfg);
//    if (eth_service == NULL) return -1;
    
//    res = GPRS_srv_attach(eth_service);
//    if (res < 0) return -2;
    
    if (cfg->if_use_name)
    {
        host = gethostbyname(cfg->svr_name.name);
        if (host != NULL)
        {
//            //host->h_addr_list[0]
//            rt_memcpy(eth_service->cfg.srv_ip.addr, host->h_addr_list[0], 4);
//            eth_service->cfg.srv_ip.port = eth_service->cfg.svr_name.port;
            rt_memcpy(srv_handler->IP, host->h_addr_list[0], 4);
            srv_handler->port = cfg->svr_name.port;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Get host by name failed !\n Use the IP address\n"));
            //eth_srv_destroy(eth_service);
            ////return -2;
            
            rt_memcpy(srv_handler->IP, &cfg->srv_ip.addr[0], 4);
            srv_handler->port = cfg->svr_name.port;
        }
    }
    //srv_handler->listen_port = cfg->listen_port;
    
    // update the local and remote IP_port.
    if (srv_handler->report_IP != NULL)
    {
        srv_handler->report_IP(srv_handler, (uint8_t *)&netif_default->ip_addr.addr, srv_handler->local_port, srv_handler->IP, srv_handler->port);
    }
    
    
    if (cfg->srv_type == eth_srv_UDP_client)
    {
        /* create new socket */
		sock = socket( AF_INET, SOCK_DGRAM, 0);
		if (sock>=0)
		{
			/* prepare local address */
			memset(&local, 0, sizeof(local));
			local.sin_family      = AF_INET;
			local.sin_port        = htons(srv_handler->listen_port);
			local.sin_addr.s_addr = htonl(INADDR_ANY);

			/* bind to local address */
			bind( sock, (struct sockaddr *)&local, sizeof(local));
            ////srv_handler->socket = sock;
            //eth_service->cfg.
            
            // Song: We need bind this socket to server_handler, then if connect server failed, we can close this socket correctly.
            srv_handler->socket = sock;
            
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Creat socket failed !\n"));
            //eth_srv_destroy(eth_service);

            // Song: We need bind this socket to server_handler, then if connect server failed, we can close this socket correctly.
////            srv_handler->socket = -1;

            return -1;
        }
    }
    else if (cfg->srv_type == eth_srv_TCP_client)
    {
        
        
    }
    
//    eth_service->connect_fail = connect_fail;
//    eth_service->connected = connected;
//    eth_service->disconnected = disconnected;
//    eth_service->received = received;
//    eth_service->rejected = rejected;
//    eth_service->closed = closed;
//    eth_service->report_IP = report_IP;
    

    return sock;
}

int eth_disconnect(int sock)
{
    if (sock >=0)
    {
        lwip_close(sock);
    }
    return 0;
}


int UITD_eth_send(void *handler, int socket,char *data,int len)
{
    int res = 0;
    t_server_handler *p_handler = (t_server_handler *)handler;
    struct sockaddr_in to = {0};

    to.sin_family      = AF_INET;
	to.sin_port        = htons(p_handler->port);
	to.sin_addr.s_addr = *(uint32_t *)p_handler->IP;
    
    rt_mutex_take(&mt_eth_send, RT_TICK_PER_SECOND * 5);
    //res = GPRS_srv_write(socket, data, len);
////    rt_thread_delay(RT_TICK_PER_SECOND*4);
    res = sendto(socket, data, len, 0, (struct sockaddr *)&to, sizeof(to));
    rt_mutex_release(&mt_eth_send);
    
    return res;
}



int UITD_eth_connect( void *handler, uint8_t if_UDP, uint8_t if_use_name, uint8_t *IP, uint16_t port,
                                pFun_srv_connect_fail connect_fail,
                                pFun_srv_connected connected,
                                pFun_srv_disconnected disconnected,
                                pFun_srv_received received,
                                pFun_srv_rejected rejected,
                                pFun_srv_closed closed,
                                pFun_srv_report_IP report_IP
                                )
{
    s_eth_srv_cfg cfg = {0};
    
    cfg.srv_type = if_UDP? eth_srv_UDP_client:eth_srv_TCP_client;
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
    
    
    return eth_connect( handler,
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





int UITD_eth_SZJS_connected(void *data)
{
    int socket = -1;
    int res = 0;
    
    socket = *(int *)data;
    SYS_log(SYS_DEBUG_INFO, ("UITD_eth_SZJS connected , socket : %d\n", socket));
    
    p_UITD_eth_SZJS->socket = socket;
    p_UITD_eth_SZJS->if_connected = 1;
    
    UITD_sensor_handler_init(p_UITD_eth_SZJS);
    
    SYS_log( SYS_DEBUG_INFO, ("eth_SZJS connected! \n"));
    
    return 0;
}

int UITD_eth_disconnect(void *handler, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_eth %d disconnect. \n", socket));
    eth_disconnect(socket);
////    svr_handler->socket = -1;
    
    if (svr_handler->disconnected != NULL)
    {
        svr_handler->disconnected(handler, NULL, socket);
    }
    return 0;
}

int UITD_eth_disconnected(void *handler, void *data, int socket)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;
    
    if (svr_handler->if_connected)
    {
        SYS_log( SYS_DEBUG_ERROR, ("UITD_eth %d disconnected !!! \n", socket));
    }
    svr_handler->if_connected = 0;
    
    return 0;
}

int UITD_eth_received(void *handler, void *data, uint32_t len)
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

int UITD_eth_report_IP(void *handler, uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port)
{
    t_server_handler *svr_handler = (t_server_handler *)handler;

    svr_handler->local_IP[0] = local_IP[0];
    svr_handler->local_IP[1] = local_IP[1];
    svr_handler->local_IP[2] = local_IP[2];
    svr_handler->local_IP[3] = local_IP[3];
    svr_handler->local_port = local_port;
    
    SYS_log( SYS_DEBUG_INFO, ("UITD_eth socket %d IP port : %d.%d.%d.%d:%d \n", 
                svr_handler->socket,
                svr_handler->local_IP[0],
                svr_handler->local_IP[1],
                svr_handler->local_IP[2],
                svr_handler->local_IP[3],
                svr_handler->local_port
                ));
    return 0;
}

int UITD_eth_XFZD_connected(void *data)
{
    int socket = -1;
    int res = 0;
    
    socket = *(int *)data;
    SYS_log(SYS_DEBUG_INFO, ("UITD_eth_XFZD connected , socket : %d\n", socket));
    
    p_UITD_eth_XFZD->socket = socket;
    p_UITD_eth_XFZD->if_connected = 1;
    
    UITD_sensor_handler_2_init(p_UITD_eth_XFZD);
    
    SYS_log( SYS_DEBUG_INFO, ("eth_XFZD connected! \n"));
    
    return 0;
}



void rt_thread_entry_UITD_eth(void* parameter)
{
    int res = 0;
    int i = 0;


    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(2);
    }
    
    while(sys_config.eth_inited_flag == 0)
    {
        rt_thread_delay(2);
    }    
    
    
    while(!(netif_default->flags & NETIF_FLAG_LINK_UP))
    {
        rt_thread_delay(1);
    }
    
//    if (eth_cfg.if_DHCP == 0)
//    {
//        //lwip_init();
//        set_if("e0", eth_cfg.IP, eth_cfg.GW, eth_cfg.MASK);
//        set_dns(eth_cfg.DNS);
//    }
    
    
    p_eth_SZJS_buf = rt_malloc(ETH_REC_BUF_MAX);
    if (p_eth_SZJS_buf == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc eth buf failed !\n"));
        while(1);
    }
    
    p_UITD_eth_SZJS = UITD_init(&UITD_eth_send,
                            &UITD_eth_connect,
                            &UITD_eth_SZJS_connected,
                            &UITD_eth_disconnect,
                            &UITD_eth_disconnected,
                            NULL,
                            &UITD_eth_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_eth_report_IP
                            );
    
    if (p_UITD_eth_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("UITD_eth_SZJS init failed !\n"));
        while(1) rt_thread_delay(5);
    }
    
    p_UITD_eth_SZJS->if_UITD = sys_config.server_SZJS.if_UITD;
    p_UITD_eth_SZJS->local_port = ETH_LOCAL_PORT;
    
    UITD_file_trans_init(p_UITD_eth_SZJS, &file_trans_cb, 1);
    //sys_status_set(&sys_ctrl, SYS_state_inited);
    
    //sys_config.server_eth_inited = 1;
    sys_config.server_1_inited = 1;
    
    
    
    // First connect, waiting forever until the net is "UP", then get ready to connect the server.
    while(1)
    {
        
        if (netif_default->flags & NETIF_FLAG_UP)
        {
            if (sys_config.server_SZJS.svr_use_name)
            {
                res = UITD_connect(p_UITD_eth_SZJS, 1, 1, sys_config.server_SZJS.svr_if_name, sys_config.server_SZJS.svr_if_port, p_UITD_eth_SZJS->local_port);
            }
            else
            {
                res = UITD_connect(p_UITD_eth_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, p_UITD_eth_SZJS->local_port);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD connect failed !\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*1);
                //while(1) rt_thread_delay(5);
                break;
            }
            else
            {
                p_UITD_eth_SZJS->connected(&res);
                //p_UITD_eth_SZJS->if_connected = 1;
            }
            
            if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
            {
                sys_status_set(&sys_ctrl, SYS_state_testing);
            }
            else
            {
                sys_status_set(&sys_ctrl, SYS_state_working);
            }
            sys_config.server_eth_connected = 1;
            break;
        }
        else
        {
            rt_thread_delay(1);
            continue;
        }
    }
    
    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        sys_config.test_flag.ethernet = 1;
        while(1) rt_thread_delay(10);
    }
    
//    // Just for test, send a test string, and print the received data.
//      if (1)
//      {      
//        int size = 0;
//        uint8_t test_buf[1024] = {0};
//        struct sockaddr_in to = {0};
//        int tolen = 0;
//                    /* prepare SNTP server address */
//                    memset(&to, 0, sizeof(to));
//                    to.sin_family      = AF_INET;
//                    to.sin_port        = htons(p_UITD_eth_SZJS->port);
//                    //to.sin_addr.s_addr = *(uint32_t *)p_UITD_eth_SZJS->IP;
//                    memcpy(&to.sin_addr.s_addr, p_UITD_eth_SZJS->IP, 4);
//                    
//                    tolen = sizeof(to);

//        
//    //    while (!(netif_default->flags & NETIF_FLAG_UP))
//    //    {
//    //        rt_thread_delay(1);
//    //    }
//    //    
//        sendto(p_UITD_eth_SZJS->socket, "Hello !\n", 8, 0, (struct sockaddr *)&to, sizeof(to));
//        
//        while(1)
//        {
//            size  = recvfrom( p_UITD_eth_SZJS->socket, test_buf, sizeof(test_buf), 0, (struct sockaddr *)&to, (socklen_t *)&tolen);
//            if (size)
//            {
//                rt_kprintf("Eth port %d receive %d bytes :\n", p_UITD_eth_SZJS->port, size);
//                for (i=0;i<size;i++)
//                {
//                    rt_kprintf(" %02X", test_buf[i]);
//                }
//                rt_kprintf("\n");
//            }
//            rt_thread_delay(1);
//        }    
//      } 
  
  
    while(1)
    {
        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
        
        if ((p_UITD_eth_SZJS != NULL) && (p_UITD_eth_SZJS->if_connected == 0))
        {
            sys_status_set(&sys_ctrl, SYS_state_reconnect);
            
            //p_UITD_eth_SZJS->status = UITD_svc_status_idle;
            
            // Reconnect, waiting forever until the net is "UP", then get ready to reconnect the server.
            if ((netif_default->flags & NETIF_FLAG_UP ) && (netif_default->flags & NETIF_FLAG_LINK_UP))
            {
                if (sys_config.server_SZJS.svr_use_name)
                {
                    res = UITD_connect(p_UITD_eth_SZJS, 1, 1, sys_config.server_SZJS.svr_if_name, sys_config.server_SZJS.svr_if_port, p_UITD_eth_SZJS->local_port);
                }
                else
                {
                    res = UITD_connect(p_UITD_eth_SZJS, 1, 0, sys_config.server_SZJS.svr_if_ip, sys_config.server_SZJS.svr_if_port, p_UITD_eth_SZJS->local_port);
                }
                
                if (res < 0)
                {
                    SYS_log( SYS_DEBUG_ERROR, ("UITD_SZJS connect failed !\n"));
                    rt_thread_delay(RT_TICK_PER_SECOND*1);

                    p_UITD_eth_SZJS->if_connected = 0;
                    continue;
                }
                else
                {
                    p_UITD_eth_SZJS->if_connected = 1;
                }
                while(p_UITD_eth_SZJS->if_connected == 0)
                {
                    rt_thread_delay(5);
                }
            }
//            else
//            {
//                rt_thread_delay(1);
//            }
            //sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else if ((p_UITD_eth_SZJS != NULL) && (p_UITD_eth_SZJS->if_connected == 1) && (p_UITD_eth_SZJS->status == UITD_svc_status_alive))
        {
            sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_eth_SZJS->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_eth_SZJS);
            UITD_send_handler(p_UITD_eth_SZJS);
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


void rt_thread_entry_eth_rx(void* parameter)
{
    uint32_t e;
    int res = 0;
    struct sockaddr_in to = {0};
    int tolen = 0;
    
    while(sys_config.server_eth_connected == 0)
    {
        rt_thread_delay(1);
    }

    memset(&to, 0, sizeof(to));
    to.sin_family      = AF_INET;
    to.sin_port        = htons(p_UITD_eth_SZJS->listen_port);
    memcpy(&to.sin_addr.s_addr, p_UITD_eth_SZJS->IP, 4);
                    
    tolen = sizeof(to);
    
    while(1)
    {
        if ((p_UITD_eth_SZJS != NULL) && (p_UITD_eth_SZJS->if_connected) && (p_UITD_eth_SZJS->socket >= 0))
        {
            eth_SZJS_rec_len  = recvfrom( p_UITD_eth_SZJS->socket, p_eth_SZJS_buf, ETH_REC_BUF_MAX-1, MSG_DONTWAIT, (struct sockaddr *)&to, (socklen_t *)&tolen);
            if (eth_SZJS_rec_len > 0)
            {
                p_UITD_eth_SZJS->received(p_UITD_eth_SZJS, p_eth_SZJS_buf, eth_SZJS_rec_len);
                
//                rt_kprintf("Eth port %d receive %d bytes :\n", p_UITD_eth_SZJS->port, size);
//                for (i=0;i<size;i++)
//                {
//                    rt_kprintf(" %02X", test_buf[i]);
//                }
//                rt_kprintf("\n");
            }
        }
        rt_thread_delay(1);

    }
}


void rt_thread_entry_UITD_eth_2(void* parameter)
{
    int res = 0;
    int i = 0;

    //rt_mutex_init(&mt_eth_send, "eth_send", RT_IPC_FLAG_FIFO);

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(2);
    }
    
    while(sys_config.eth_inited_flag == 0)
    {
        rt_thread_delay(2);
    }

    while(!(netif_default->flags & NETIF_FLAG_LINK_UP))
    {
        rt_thread_delay(1);
    }
    
    p_eth_XFZD_buf = rt_malloc(ETH_REC_BUF_MAX);
    if (p_eth_XFZD_buf == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc eth buf failed !\n"));
        while(1);
    }
    
    p_UITD_eth_XFZD = UITD_init(&UITD_eth_send,
                            &UITD_eth_connect,
                            &UITD_eth_XFZD_connected,
                            &UITD_eth_disconnect,
                            &UITD_eth_disconnected,
                            NULL,
                            &UITD_eth_received,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            &UITD_eth_report_IP
                            );
    
    if (p_UITD_eth_XFZD == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("UITD_eth_XFZD init failed !\n"));
        while(1) rt_thread_delay(5);
    }
    
    p_UITD_eth_XFZD->if_UITD = sys_config.server_XFZD.if_UITD;
    
    // Song: Listen port must same as the local_port;
    p_UITD_eth_XFZD->local_port = ETH_LOCAL_PORT_2;

    UITD_file_trans_init(p_UITD_eth_XFZD, &file_trans_cb, 1);
    //sys_status_set(&sys_ctrl, SYS_state_inited);
    
    //sys_config.server_eth_inited = 1;
    sys_config.server_2_inited = 1;
    
    
    // First connect, waiting forever until the net is "UP", then get ready to connect the server.
    while(1)
    {
//        res = rt_sem_take(&sem_eth_up, 1);
//        if (res == RT_EOK)
        
        if (netif_default->flags & NETIF_FLAG_UP)
        {
            if (sys_config.server_XFZD.svr_use_name)
            {
                res = UITD_connect(p_UITD_eth_XFZD, 1, 1, sys_config.server_XFZD.svr_if_name, sys_config.server_XFZD.svr_if_port, p_UITD_eth_XFZD->local_port);
            }
            else
            {
                res = UITD_connect(p_UITD_eth_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, p_UITD_eth_XFZD->local_port);
            }
            
            if (res < 0)
            {
                SYS_log( SYS_DEBUG_ERROR, ("UITD connect failed !\n"));
                rt_thread_delay(RT_TICK_PER_SECOND*1);
                //while(1) rt_thread_delay(5);
                break;
            }
            else
            {
                p_UITD_eth_XFZD->connected(&res);
                //p_UITD_eth_SZJS->if_connected = 1;
            }
            
//            while(p_UITD_eth_SZJS->if_connected == 0)
//            {
//                rt_thread_delay(5);
//            }
            
            if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
            {
                ////sys_status_set(&sys_ctrl, SYS_state_testing);
            }
            else
            {
                ////sys_status_set(&sys_ctrl, SYS_state_working);
            }
            //sys_config.server_eth_connected = 1;
            break;
        }
        else
        {
            rt_thread_delay(1);
            continue;
        }
    }
    
    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        while(1) rt_thread_delay(10);
    }

  
    while(1)
    {
        //rt_device_write(device_sub_mcu, 0, test_hello, sizeof(test_hello) - 1);
        
        if ((p_UITD_eth_XFZD != NULL) && (p_UITD_eth_XFZD->if_connected == 0))
        {
            ////sys_status_set(&sys_ctrl, SYS_state_reconnect);
            
            //p_UITD_eth_XFZD->status = UITD_svc_status_idle;
            
            // Reconnect, waiting forever until the net is "UP", then get ready to reconnect the server.
            if ((netif_default->flags & NETIF_FLAG_UP) && (netif_default->flags & NETIF_FLAG_LINK_UP))
            {
                if (sys_config.server_XFZD.svr_use_name)
                {
                    res = UITD_connect(p_UITD_eth_XFZD, 1, 1, sys_config.server_XFZD.svr_if_name, sys_config.server_XFZD.svr_if_port, p_UITD_eth_XFZD->local_port);
                }
                else
                {
                    res = UITD_connect(p_UITD_eth_XFZD, 1, 0, sys_config.server_XFZD.svr_if_ip, sys_config.server_XFZD.svr_if_port, p_UITD_eth_XFZD->local_port);
                }
                
                if (res < 0)
                {
                    SYS_log( SYS_DEBUG_ERROR, ("UITD_XFZD connect failed !\n"));
                    rt_thread_delay(RT_TICK_PER_SECOND*1);

                    p_UITD_eth_XFZD->if_connected = 0;
                    continue;
                }
                else
                {
                    p_UITD_eth_XFZD->if_connected = 1;
                }
                while(p_UITD_eth_XFZD->if_connected == 0)
                {
                    rt_thread_delay(5);
                }
            }
//            else
//            {
//                rt_thread_delay(1);
//            }
            //sys_status_set(&sys_ctrl, SYS_state_working);
            
        }
        else if ((p_UITD_eth_XFZD != NULL) && (p_UITD_eth_XFZD->if_connected == 1) && (p_UITD_eth_XFZD->status == UITD_svc_status_alive))
        {
            ////sys_status_set(&sys_ctrl, SYS_state_working);
            p_UITD_eth_XFZD->if_connected = 2;
        }
        else 
        {
            UITD_service_handler(p_UITD_eth_XFZD);
            UITD_send_handler(p_UITD_eth_XFZD);
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


void rt_thread_entry_eth_rx_2(void* parameter)
{
    uint32_t e;
    int res = 0;
    struct sockaddr_in to = {0};
    int tolen = 0;
    
    while(sys_config.server_eth_connected == 0)
    {
        rt_thread_delay(1);
    }

    memset(&to, 0, sizeof(to));
    to.sin_family      = AF_INET;
    to.sin_port        = htons(p_UITD_eth_XFZD->listen_port);
    memcpy(&to.sin_addr.s_addr, p_UITD_eth_XFZD->IP, 4);
                    
    tolen = sizeof(to);
    
    while(1)
    {
        if ((p_UITD_eth_XFZD != NULL) && (p_UITD_eth_XFZD->if_connected) && (p_UITD_eth_XFZD->socket >= 0))
        {
            eth_XFZD_rec_len  = recvfrom( p_UITD_eth_XFZD->socket, p_eth_XFZD_buf, ETH_REC_BUF_MAX-1, MSG_DONTWAIT, (struct sockaddr *)&to, (socklen_t *)&tolen);
            if (eth_XFZD_rec_len > 0)
            {
                p_UITD_eth_XFZD->received(p_UITD_eth_XFZD, p_eth_XFZD_buf, eth_XFZD_rec_len);
                
//                rt_kprintf("Eth port %d receive %d bytes :\n", p_UITD_eth_SZJS->port, size);
//                for (i=0;i<size;i++)
//                {
//                    rt_kprintf(" %02X", test_buf[i]);
//                }
//                rt_kprintf("\n");
            }
        }
        rt_thread_delay(1);

    }
}

void rt_thread_entry_UITD_eth_rec(void* parameter)
{
    int res = 0;

    rt_mutex_init(&mt_eth_send, "eth_send", RT_IPC_FLAG_FIFO);

    
    while(sys_config.server_1_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        UITD_server(p_UITD_eth_SZJS);
        UITD_server(p_UITD_eth_XFZD);
        rt_thread_delay(1);
    }

}





#ifdef RT_USING_FINSH
#include <finsh.h>

//int sys_fw_upgrade(void)
//{
//    return UITD_firmware_upgrade(p_UITD_SZJS);
//}
//FINSH_FUNCTION_EXPORT_ALIAS(sys_fw_upgrade, fw_upgrade, Upgrade the firmware.);


//int sys_file_download_success(void *data)
//{
//    SYS_log( SYS_DEBUG_INFO, ("File download success.\n"));
//    return 0;
//}
//int sys_file_download_fail(void *data)
//{
//    SYS_log( SYS_DEBUG_INFO, ("File download failed.\n"));
//    return 0;
//}

//int sys_file_download(uint8_t *filename)
//{
//    
//    return UITD_download_file(p_UITD_SZJS, &p_UITD_SZJS->FW, filename, &sys_file_download_success, &sys_file_download_fail);
//}
//FINSH_FUNCTION_EXPORT_ALIAS(sys_file_download, file_download, Download file from server.);

//int sys_sensor_send_data(uint16_t data)
//{
//    t_GB_ctrl_timestamp TS;   
//    
//    UITD_get_time(&TS);
//    return UITD_sensor_upload_rough_data(p_UITD_SZJS, 1, sensor_out_type_4_20mA, data, &TS);
//}
//FINSH_FUNCTION_EXPORT_ALIAS(sys_sensor_send_data, sensor_data, Send sensor rough data.);

//int sys_upload_file_info(uint8_t *name)
//{
//    
//    return UITD_file_upload_file_info(p_UITD_SZJS, name);
//}
//FINSH_FUNCTION_EXPORT_ALIAS(sys_upload_file_info, file_upload, Upload file to server.);

int UITD_eth_server_info(void)
{
    
    if (p_UITD_eth_SZJS == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD eth SZJS is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n =======  UITD eth SZJS server info ============================ \n"));

        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_eth_SZJS->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_eth_SZJS->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_eth_SZJS->IP[0],
                                    p_UITD_eth_SZJS->IP[1],
                                    p_UITD_eth_SZJS->IP[2],
                                    p_UITD_eth_SZJS->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_eth_SZJS->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_eth_SZJS->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_eth_SZJS->local_IP[0],
                                    p_UITD_eth_SZJS->local_IP[1],
                                    p_UITD_eth_SZJS->local_IP[2],
                                    p_UITD_eth_SZJS->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_eth_SZJS->local_port));
        SYS_log(SYS_DEBUG_INFO, ("listen_port: %d\n", p_UITD_eth_SZJS->listen_port));
        
    }
    
    if (p_UITD_eth_XFZD == NULL)
    {
        SYS_log(SYS_DEBUG_INFO, ("\n UITD eth XFZD is not used \n"));
    }
    else
    {
        SYS_log(SYS_DEBUG_INFO, ("\n =======  UITD eth XFZD server info ============================ \n"));
        
        SYS_log(SYS_DEBUG_INFO, ("status: %d \n  ---- %d:idle, %d:connect, %d:alive, %d:disconnect, %d:stoped, %d:testing\n", 
                                    p_UITD_eth_XFZD->status,
                                    UITD_svc_status_idle,
                                    UITD_svc_status_connect,
                                    UITD_svc_status_alive,
                                    UITD_svc_status_disconnected,
                                    UITD_svc_status_stoped,
                                    UITD_svc_status_testing
                                    ));
        
        SYS_log(SYS_DEBUG_INFO, ("if_use_name: %d\n", p_UITD_eth_XFZD->if_use_name));
        SYS_log(SYS_DEBUG_INFO, ("IP: %d.%d.%d.%d\n", 
                                    p_UITD_eth_XFZD->IP[0],
                                    p_UITD_eth_XFZD->IP[1],
                                    p_UITD_eth_XFZD->IP[2],
                                    p_UITD_eth_XFZD->IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("server_name: %s\n", p_UITD_eth_XFZD->svr_name));
        SYS_log(SYS_DEBUG_INFO, ("port: %d\n", p_UITD_eth_XFZD->port));
        SYS_log(SYS_DEBUG_INFO, ("local_IP: %d.%d.%d.%d\n", 
                                    p_UITD_eth_XFZD->local_IP[0],
                                    p_UITD_eth_XFZD->local_IP[1],
                                    p_UITD_eth_XFZD->local_IP[2],
                                    p_UITD_eth_XFZD->local_IP[3]));
        SYS_log(SYS_DEBUG_INFO, ("local_port: %d\n", p_UITD_eth_XFZD->local_port));
        SYS_log(SYS_DEBUG_INFO, ("listen_port: %d\n", p_UITD_eth_XFZD->listen_port));
        
    }
    
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(UITD_eth_server_info, UITD_eth_info, Print the UITD ethernet servers infomation.);

#endif // 




















