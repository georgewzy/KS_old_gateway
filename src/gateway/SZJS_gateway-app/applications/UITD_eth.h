#ifndef __UITD_ETH_H__
#define __UITD_ETH_H__

#include <rtthread.h>
#include "board.h"
//#include "GSM_MG301.h"

#define ETH_REC_BUF_MAX     1500

#define ETH_LOCAL_PORT      59527
#define ETH_LOCAL_PORT_2    59528


typedef int (* pFun_eth_connect_fail)(void *data);

typedef int (* pFun_eth_connected)(void *data);
typedef int (* pFun_eth_disconnected)(void *data, int socket);
typedef int (* pFun_eth_received)(void *data, uint32_t len);
typedef int (* pFun_eth_rejected)(void *data);
typedef int (* pFun_eth_closed)(void *data);
typedef int (* pFun_eth_accepted)(void *data);
typedef int (* pFun_eth_report_IP)(uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port);



typedef enum
{
    eth_srv_UDP_client = 0,
    eth_srv_UDP_server,
    eth_srv_TCP_client,
    eth_srv_TCP_server,
    eth_srv_ftp_client,
    eth_srv_http_client

} e_eth_srv_type;

typedef struct
{
    uint8_t     addr[4];
    uint16_t    port;
} s_eth_svr_ip;

typedef struct
{
    uint8_t     name[SVR_IF_NAME_LEN];
    uint16_t    port;
} s_eth_svr_name;

typedef struct
{
    e_eth_srv_type     srv_type;
    uint8_t            if_use_name;
    s_eth_svr_ip       srv_ip;
    s_eth_svr_ip       local_ip;
    s_eth_svr_name     svr_name;
    uint16_t           listen_port;
} s_eth_srv_cfg;


typedef enum
{
    eth_idle = 0,
    eth_phy_up,
    eth_up,
    eth_release,
    eth_release_ack,
    eth_discon,
} e_eth_status;


typedef struct
{
    e_eth_status           status;
    
    s_eth_srv_cfg          cfg;
    
    //uint8_t                 rec_buf[GSM_GPRS_REC_BUF_SIZE];
    //e_GPRS_rec_status       rec_status;
    
    pFun_eth_accepted      accepted;
    pFun_eth_connect_fail  connect_fail;
    pFun_eth_connected     connected;
    pFun_eth_disconnected  disconnected;
    pFun_eth_received      received;
    pFun_eth_rejected      rejected;
    pFun_eth_closed        closed;
    pFun_eth_report_IP     report_IP;
} s_eth_service;



#define THREAD_ETH_RX_STACK_SIZE     512
extern rt_uint8_t prio_eth_rx;
extern rt_thread_t thread_eth_rx;

#define THREAD_ETH_RX_2_STACK_SIZE     512
extern rt_uint8_t prio_eth_rx_2;
extern rt_thread_t thread_eth_rx_2;


#define THREAD_UITD_ETH_STACK_SIZE     8192
extern rt_uint8_t prio_UITD_eth;
extern rt_thread_t thread_UITD_eth;

#define THREAD_UITD_ETH_2_STACK_SIZE     8192
extern rt_uint8_t prio_UITD_eth_2;
extern rt_thread_t thread_UITD_eth_2;


#define THREAD_UITD_ETH_REC_STACK_SIZE     4096
extern rt_uint8_t prio_UITD_eth_rec;
extern rt_thread_t thread_UITD_eth_rec;


extern struct rt_semaphore sem_UITD_eth_rx;
extern struct rt_semaphore sem_UITD_eth_tx;

//extern struct rt_device * device_sub_muc;

extern void rt_thread_entry_UITD_eth(void* parameter);
extern void rt_thread_entry_UITD_eth_2(void* parameter);
extern void rt_thread_entry_UITD_eth_rec(void* parameter);
extern void rt_thread_entry_eth_rx(void* parameter);
extern void rt_thread_entry_eth_rx_2(void* parameter);
//extern int UITD_server_info(void);

#endif // __UITD_ETH_H__
