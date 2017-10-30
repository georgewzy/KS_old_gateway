#ifndef __GSM_MG301_H__
#define __GSM_MG301_H__


#include <rtthread.h>
#include "board.h"
#include "sys_def.h"

//#include "time.h"

#define GSM_GPRS_APN            "CMMTM"


#ifndef GSM_GPRS_APN
#define GSM_GPRS_APN    "CMNET"
#endif // 


#define GSM_IMEI_LEN            15

#define GSM_AT_DUMMY            "\n\r"

#define GSM_AT_AT               "AT\n\r"
#define GSM_ATE                 "ATE0\n\r"
#define GSM_ATI                 "ATI\n\r"
#define GSM_AT_SWSPATH          "AT^SWSPATH=1\n\r"

#define GSM_AT_CPIN             "AT+CPIN?\n\r"
#define GSM_AT_CGSN             "AT+CGSN\n\r"

#define GSM_AT_CSQ              "AT+CSQ\n\r"

#define GSM_AT_CGATT            "AT+CGATT?\n\r"
#define GSM_AT_SICS			    "AT^SICS=%d,\"conType\",\"GPRS0\"\n\r"
#define GSM_AT_SICS_APN		    "AT^SICS=%d,\"apn\",\"%s\"\n\r"
#define GSM_AT_SISS_TYPE		"AT^SISS=%d,srvType,Socket\n\r"
#define GSM_AT_IOMODE 			"AT^IOMODE=%d,%d\n\r"
#define GSM_AT_SISS_ADDR_UDP 	"AT^SISS=%d,address,sockudp://%d.%d.%d.%d:%d\n\r"
#define GSM_AT_SISS_ADDR_TCP 	"AT^SISS=%d,address,socktcp://%d.%d.%d.%d:%d\n\r"
#define GSM_AT_SISS_NAME_UDP 	"AT^SISS=%d,address,sockudp://%s:%d\n\r"
#define GSM_AT_SISS_NAME_TCP 	"AT^SISS=%d,address,socktcp://%s:%d\n\r"

#define GSM_AT_SISS_LISTEN_UDP 	"AT^SISS=%d,address,sockudp://listener:%d\n\r"
#define GSM_AT_SISS_LISTEN_TCP 	"AT^SISS=%d,address,socktcp://listener:%d\n\r"

#define GSM_AT_SISO             "AT^SISO=%d\n\r"
#define GSM_AT_SISC             "AT^SISC=%d\n\r"
#define GSM_AT_SISO_INFO        "AT^SISO?\n\r"

#define GSM_AT_SISW             "AT^SISW=%d,%d\n\r"


#define GPRS_SIS_TCP_REJECT         21
#define GPRS_SIS_TCP_DISCONNECT     48


////////////////////////////////////////////////////////////////////////////

#define GSM_CMD_AT          "AT\n"
#define GSM_CMD_ATE         "ATE0\n"
#define GSM_CMD_ATI         "ATI\n"
#define GSM_CMD_AT_SWSPATH  "AT^SWSPATH=1\n"
#define GSM_CMD_AT_CPIN     "AT+CPIN?\n"
#define GSM_CMD_AT_CGSN     "AT+CGSN\n"


#define GSM_CMD_AT_CSQ      "AT+CSQ\n"

#define GSM_CMD_AT_COPS     "AT+COPS?\n"

#define GSM_CMD_AT_CGATT      "AT+CGATT?\n"

#define GSM_CMD_AT_SICS			    "AT^SICS=%d,conType,GPRS0\n"
#define GSM_CMD_AT_SISS_TYPE		"AT^SISS=0,srvType,Socket\n"
#define GSM_CMD_AT_SISS_ADDR_UDP    "AT^SISS=0,address,sockudp://"
#define GSM_CMD_AT_SISS_ADDR_TCP    "AT^SISS=0,address,socktcp://"

#define GSM_CMD_AT_IOMODE 			"AT^IOMODE=0,1\n"

#define GSM_CMD_AT_SISO 			"AT^SISO=0\r"
#define GSM_CMD_AT_SISC 			"AT^SISC=0\r"

#define GSM_CMD_AT_SISW 			"AT^SISW=0,"


#define GSM_CMD_AT_IPCLOSE  "AT%IPCLOSE=5\n"

#define GSM_CMD_AT_CGDCONT  "AT+CGDCONT=1,\"IP\",\"CMNET\"\n"
//#define GSM_CMD_AT_CGDCONT  "AT+CGDCONT=1,\"IP\",\"CMWAP\"\n"
#define GSM_CMD_AT_ETCPIP          "AT%ETCPIP\n"

#define GSM_CMD_AT_IPOPEN   "AT%IPOPEN=\"TCP\",\"58.208.162.117\"\n"

#define GSM_SRV_UDP_SONG    "58.208.162.117:12345"
#define GSM_SRV_TCP_SONG    "58.208.162.117:12345"


#define GSM_GPRS_SERVICE_MAX    10

#define GSM_GPRS_SEND_MAX       1470
#define GSM_GPRS_REC_MAX        1470

#define GSM_GPRS_REC_BUF_SIZE   (GSM_GPRS_REC_MAX + 64)


#define IF_NOT_GSM_CONN_NUM(index)  if ((index < 0) || (index >= GSM_GPRS_SERVICE_MAX)) return -1;

typedef struct
{
    uint8_t     addr[4];
    uint16_t    port;
} s_svr_ip;

typedef struct
{
    uint8_t     name[SVR_IF_NAME_LEN];
    uint16_t    port;
} s_svr_name;

typedef enum
{
    GSM_idle = 0,
    GSM_term_on,
    GSM_term_reset,
    GSM_GPRS_active,
    GSM_GPRS_up,
    GSM_GPRS_down,
    GSM_term_off
} e_GSM_status;

typedef enum
{
    GSM_term_on_power_on = 0,
    GSM_term_on_power_on_ack,
    GSM_term_on_reset,
    GSM_term_on_ON

} e_GSM_term_on_status;


typedef enum
{
    GSM_gsm_phase_0 = 0,
    GSM_gsm_phase_0_ack,
    GSM_gsm_phase_1,
    GSM_gsm_phase_1_ack,
    GSM_gsm_phase_2,
    GSM_gsm_phase_2_ack,
    GSM_gsm_phase_3,
    GSM_gsm_phase_3_ack,
    GSM_gsm_phase_4,
    GSM_gsm_phase_4_ack,
    GSM_gsm_phase_5,
    GSM_gsm_phase_5_ack,
    GSM_gsm_phase_6,
    GSM_gsm_phase_6_ack,
    GSM_gsm_phase_6_retry,
    GSM_gsm_phase_7,
    GSM_gsm_phase_7_ack,
    GSM_gsm_phase_8,
    GSM_gsm_phase_8_ack,
    GSM_gsm_phase_9,
    GSM_gsm_phase_9_ack,
    GSM_gsm_phase_10,
    GSM_gsm_phase_10_ack,
    GSM_gsm_phase_11,
    GSM_gsm_phase_11_ack,
    
} e_GSM_gprs_active;

typedef enum
{
    GSM_gprs_init = 0,
    GSM_gprs_idle
} e_GSM_gprs_up;

typedef enum
{
    GSM_gprs_close = 0,
    GSM_gprs_close_ack,
    GSM_gprs_closed
} e_GSM_gprs_down;

typedef enum
{
    GSM_term_off_OFF = 0,
    GSM_term_off_OFF_ack,
    GSM_term_off_power_off

} e_GSM_term_off_status;


typedef struct
{
    e_GSM_status            sys;
    e_GSM_term_on_status    term_on;
    e_GSM_gprs_active       gprs_active;
    e_GSM_gprs_up           gprs_up;
    e_GSM_gprs_down         gprs_down;
    e_GSM_term_off_status   term_off;
} s_GSM_status;


typedef enum
{
    GPRS_idle = 0,
    GPRS_SISS_type,
    GPRS_SISS_type_ack,
    GPRS_SISS_addr,
    GPRS_SISS_addr_ack,
    //GPRS_IOMODE,
    //GPRS_IOMODE_ack,
    GPRS_SISO,
    GPRS_SISO_ack,
    GPRS_SISS_type_listen,
    GPRS_SISS_type_listen_ack,
    GPRS_SISS_listen,
    GPRS_SISS_listen_ack,
    GPRS_SISO_INFO,
    GPRS_SISO_INFO_ack,
    GPRS_attached,
    GPRS_up,
    GPRS_release,
    GPRS_release_ack,
    GPRS_discon,
    //GPRS_disconnected
} e_GPRS_status;


typedef enum
{
    GPRS_srv_UDP_client = 0,
    GPRS_srv_UDP_server,
    GPRS_srv_TCP_client,
    GPRS_srv_TCP_server,
    GRPS_srv_ftp_client,
    GPRS_srv_http_client

} e_GPRS_srv_type;

typedef enum
{
    GPRS_rec_idle = 0,
    GPRS_rec_transing,
    GPRS_rec_stop,
    GPRS_rec_end,
    GPRS_rec_timeout
} e_GPRS_rec_status;


typedef struct
{
    e_GPRS_srv_type     srv_type;
    uint8_t             if_use_name;
    s_svr_ip            srv_ip;
    s_svr_ip            local_ip;
    s_svr_name          svr_name;
    uint16_t            listen_port;
} s_GPRS_srv_cfg;

typedef int (* pFun_GPRS_connect_fail)(void *data);

typedef int (* pFun_GPRS_connected)(void *data);
typedef int (* pFun_GPRS_disconnected)(void *handler, void *data, int socket);
typedef int (* pFun_GPRS_received)(void *handler, void *data, uint32_t len);
typedef int (* pFun_GPRS_rejected)(void *data);
typedef int (* pFun_GPRS_closed)(void *data);
typedef int (* pFun_GPRS_accepted)(void *data);
typedef int (* pFun_GPRS_report_IP)(void *handler, uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port);

typedef struct  
{
    int     handle;
    int     URC;
    int     error;

} s_GPRS_SIS;

typedef struct
{
    e_GPRS_status           status;
    
    s_GPRS_srv_cfg          cfg;
    
    void                    *handler;
    
    //uint8_t                 rec_buf[GSM_GPRS_REC_BUF_SIZE];
    //e_GPRS_rec_status       rec_status;
    
    pFun_GPRS_accepted      accepted;
    pFun_GPRS_connect_fail  connect_fail;
    pFun_GPRS_connected     connected;
    pFun_GPRS_disconnected  disconnected;
    pFun_GPRS_received      received;
    pFun_GPRS_rejected      rejected;
    pFun_GPRS_closed        closed;
    pFun_GPRS_report_IP     report_IP;
} s_GPRS_service;







typedef struct
{
    uint8_t OK          :1;
    uint8_t ERROR       :1;
    uint8_t UNKNOWN     :1;
    uint8_t START       :1;
    uint8_t SHUTDOWN    :1;
    uint8_t TIME        :1;
    uint8_t CPIN_NOT_RD :1;
    uint8_t IMEI_REC    :1;
    uint8_t SIGNAL      :1;
    uint8_t ATTACH      :1;
    
    uint8_t DATA_REC    :1;
    uint8_t SIS         :1;
    uint8_t SISO_INFO   :1;
    
    uint16_t res        :5;
} s_GSM_ack;

typedef union
{
    uint16_t    ack_char;
    s_GSM_ack   ack_stru;
} u_GSM_ack;


typedef struct
{
    rt_device_t     dev;
    s_GSM_status    status;
    uint8_t         rec_buf[GSM_GPRS_REC_MAX];
    uint32_t        rec_len;
    uint32_t        rec_index;
    uint8_t         send_buf[GSM_GPRS_SEND_MAX];
    uint32_t        send_len;
    uint32_t        send_index;
    
    u_GSM_ack       ack;
    uint8_t         srv_num;
    uint32_t        retry;
    
    uint8_t         data_handle;
    uint32_t        data_len;
    uint8_t         data_sending;
    s_GPRS_SIS      SIS;    
    
    uint8_t         SISO_index;
    uint8_t         local_IP[4];
    uint16_t        local_port;
    
    uint8_t         if_attached;
    uint8_t         rssi;
    uint8_t         ber;
    uint8_t         IMEI[GSM_IMEI_LEN + 1];
    //struct tm       time;
    int8_t          time_zone;
    rt_time_t       time_UTC;
    
    s_GPRS_service  *service[GSM_GPRS_SERVICE_MAX];
    
} s_GSM_CB;

//extern s_GSM_CB GSM_CB;

//extern s_GPRS_service  *GPRS_srv_SZJS;
//extern s_GPRS_service  *GPRS_srv_XFZD;

extern struct rt_semaphore sem_GSM_up;
extern struct rt_semaphore sem_GSM_down;



#define THREAD_GSM_MG301_STACK_SIZE     512
extern rt_uint8_t prio_GSM_MG301;
extern rt_thread_t thread_GSM_MG301;

#define THREAD_GSM_RX_STACK_SIZE        512
extern rt_uint8_t prio_GSM_rx;
extern rt_thread_t thread_GSM_rx;



extern int GPRS_disconnect(int handle);





extern struct rt_semaphore sem_GSM_MG301_rx;
extern struct rt_semaphore sem_GSM_MG301_tx;

extern struct rt_mutex mt_GSM_send;

extern struct rt_device * device_GSM_MG301;

extern void rt_thread_entry_GSM_MG301(void* parameter);
extern void rt_thread_entry_GSM_rx(void* parameter);



#endif // __GSM_MG301_H__
