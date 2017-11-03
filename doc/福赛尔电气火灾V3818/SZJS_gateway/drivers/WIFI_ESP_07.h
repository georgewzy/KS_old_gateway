#ifndef __WIFI_ESP_07_H__
#define __WIFI_ESP_07_H__

#include <rtthread.h>
#include "board.h"
#include "GSM_MG301.h"
#include "UITD.h"


#define THREAD_WIFI_ESP_07_STACK_SIZE     2048
extern rt_uint8_t prio_WIFI_ESP_07;
extern rt_thread_t thread_WIFI_ESP_07;

#define THREAD_WIFI_RX_STACK_SIZE        512
extern rt_uint8_t prio_WIFI_rx;
extern rt_thread_t thread_WIFI_rx;




#define ESP_WIFI_SERVICE_MAX    5

#define ESP_WIFI_SEND_MAX       (GB_ROUGH_LEN_MAX + 256)
#define ESP_WIFI_REC_MAX        (GB_ROUGH_LEN_MAX + 256)



#define WIFI_ESP_CMD_AT             "AT\r\n"

#define WIFI_ESP_CMD_AT_CWMODE      "AT+CWMODE=1\r\n"
#define WIFI_ESP_CMD_AT_RST         "AT+RST\r\n"
#define WIFI_ESP_CMD_AT_CWJAP       "AT+CWJAP=\"SZJS_BF4B\",\"szjs1234\"\r\n"

#define WIFI_ESP_CMD_AT_CIPMUX      "AT+CIPMUX=0\r\n"  // single connection.
//#define WIFI_ESP_CMD_AT_CIPMUX    "AT+CIPMUX=1\r\n"  // multiple connection.
#define WIFI_ESP_CMD_AT_CIPSTART    "AT+CIPSTART=\"TCP\",\"192.168.0.86\",12345\r\n"
#define WIFI_ESP_CMD_AT_CIPSEND     "AT+CIPSEND="



#define ESP_AT_RST                  "AT+RST\r\n"
#define ESP_AT_AT                   "AT\r\n"
#define ESP_AT_CWMODE               "AT+CWMODE=1\r\n"  // 1-station, 2-AP, 3-station+AP
#define ESP_AT_CIPMUX_SINGLE        "AT+CIPMUX=0\r\n"  // 0-single, 1-multiple connection.
#define ESP_AT_CIPMUX_MULT          "AT+CIPMUX=1\r\n"  // 0-single, 1-multiple connection.
#define ESP_AT_CWJAP                "AT+CWJAP=\"%s\",\"%s\"\r\n"  // "SSID" "PW"

#define ESP_AT_SEND                 "AT+CIPSEND=%d,%d\r\n"
#define ESP_AT_IPD                  "+IPD,"
#define ESP_AT_CIPSTART_UDP         "AT+CIPSTART=%d,\"UDP\",\"%d.%d.%d.%d\",%d\r\n"
#define ESP_AT_CIPSTART_TCP         "AT+CIPSTART=%d,\"TCP\",\"%d.%d.%d.%d\",%d\r\n"
#define ESP_AT_CIPCLOSE             "AT+CIPCLOSE=%d\r\n"


#define IF_NOT_WIFI_CONN_NUM(index)  if ((index < 0) || (index >= ESP_WIFI_SERVICE_MAX)) return -1;



typedef enum
{
    ESP_idle = 0,
    ESP_term_on,
    ESP_term_reset,
    ESP_WIFI_active,
    ESP_WIFI_up,
    ESP_WIFI_down,
    ESP_term_off
} e_ESP_status;

typedef enum
{
    ESP_term_on_power_on = 0,
    ESP_term_on_power_on_ack,
    ESP_term_on_reset,
    ESP_term_on_ON

} e_ESP_term_on_status;


typedef enum
{
    ESP_WIFI_phase_0 = 0,
    ESP_WIFI_phase_0_ack,
    ESP_WIFI_phase_1,
    ESP_WIFI_phase_1_ack,
    ESP_WIFI_phase_2,
    ESP_WIFI_phase_2_ack,
    ESP_WIFI_phase_3,
    ESP_WIFI_phase_3_ack,
    ESP_WIFI_phase_4,
    ESP_WIFI_phase_4_ack,
    ESP_WIFI_phase_5,
    ESP_WIFI_phase_5_ack,
    ESP_WIFI_phase_6,
    ESP_WIFI_phase_6_ack,
    ESP_WIFI_phase_6_retry,
    ESP_WIFI_phase_7,
    
} e_ESP_WIFI_active;

typedef enum
{
    ESP_WIFI_init = 0,
    ESP_WIFI_idle
} e_ESP_WIFI_up;

typedef enum
{
    ESP_WIFI_close = 0,
    ESP_WIFI_close_ack,
    ESP_WIFI_closed
} e_ESP_WIFI_down;

typedef enum
{
    ESP_term_off_OFF = 0,
    ESP_term_off_OFF_ack,
    ESP_term_off_power_off

} e_ESP_term_off_status;



typedef struct
{
    uint8_t OK          :1;
    uint8_t ERROR       :1;
    uint8_t UNKNOWN     :1;
    uint8_t START       :1;
    uint8_t SHUTDOWN    :1;
    uint8_t CONNECT     :1;
    uint8_t FAIL        :1;
    uint8_t READY       :1;
    uint8_t DATA_REC    :1;
    uint8_t res_1      :1;
    
    uint8_t res_2    :1;
    uint8_t res_3         :1;
    uint8_t res_4   :1;
    uint8_t res_5       :1;
    
    uint16_t res        :4;
} s_ESP_ack;

typedef union
{
    uint16_t    ack_char;
    s_ESP_ack   ack_stru;
} u_ESP_ack;

typedef struct
{
    e_ESP_status            sys;
    e_ESP_term_on_status    term_on;
    e_ESP_WIFI_active       wifi_active;
    e_ESP_WIFI_up           wifi_up;
    e_ESP_WIFI_down         wifi_down;
    e_ESP_term_off_status   term_off;
} s_ESP_status;


typedef enum
{
    WIFI_idle = 0,
    WIFI_CIPSTART,
    WIFI_CIPSTART_ack,
    WIFI_attached,
    WIFI_up,
    WIFI_release,
    WIFI_release_ack,
    WIFI_discon,
} e_WIFI_status;


typedef enum
{
    WIFI_srv_UDP_client = 0,
    WIFI_srv_UDP_server,
    WIFI_srv_TCP_client,
    WIFI_srv_TCP_server,
    WIFI_srv_ftp_client,
    WIFI_srv_http_client

} e_WIFI_srv_type;

typedef enum
{
    WIFI_rec_idle = 0,
    WIFI_rec_transing,
    WIFI_rec_stop,
    WIFI_rec_end,
    WIFI_rec_timeout
} e_WIFI_rec_status;


typedef struct
{
    e_WIFI_srv_type     srv_type;
    uint8_t             if_use_name;
    s_svr_ip            srv_ip;
    s_svr_ip            local_ip;
    uint16_t            listen_port;
//    uint8_t             SSID[32];
//    uint8_t             PW[32];
} s_WIFI_srv_cfg;

typedef int (* pFun_WIFI_connect_fail)(void *data);

typedef int (* pFun_WIFI_connected)(void *data);
typedef int (* pFun_WIFI_disconnected)(void *handler, void *data, int socket);
typedef int (* pFun_WIFI_received)(void *handler, void *data, uint32_t len);
typedef int (* pFun_WIFI_rejected)(void *data);
typedef int (* pFun_WIFI_closed)(void *data);
typedef int (* pFun_WIFI_accepted)(void *data);
typedef int (* pFun_WIFI_report_IP)(void *handler, uint8_t *local_IP, uint16_t local_port, uint8_t *remote_IP, uint16_t remote_port);

typedef struct  
{
    int     handle;
    int     URC;
    int     error;

} s_WIFI_SIS;

typedef struct
{
    e_WIFI_status           status;
    
    s_WIFI_srv_cfg          cfg;
    
    void                    *handler;
    
    pFun_WIFI_accepted      accepted;
    pFun_WIFI_connect_fail  connect_fail;
    pFun_WIFI_connected     connected;
    pFun_WIFI_disconnected  disconnected;
    pFun_WIFI_received      received;
    pFun_WIFI_rejected      rejected;
    pFun_WIFI_closed        closed;
    pFun_WIFI_report_IP     report_IP;
} s_WIFI_service;


typedef struct
{
    rt_device_t     dev;
    s_ESP_status    status;
    uint8_t         rec_buf[ESP_WIFI_REC_MAX];
    uint32_t        rec_len;
    uint32_t        rec_index;
    uint8_t         send_buf[ESP_WIFI_SEND_MAX];
    uint32_t        send_len;
    uint32_t        send_index;
    
    u_ESP_ack       ack;
    uint8_t         srv_num;
    uint32_t        retry;
    
    uint8_t         data_handle;
    uint32_t        data_len;
    uint8_t         data_sending;
    s_WIFI_SIS      SIS;    
    
    uint8_t         SISO_index;
    uint8_t         local_IP[4];
    uint16_t        local_port;
    
    uint8_t         if_attached;
    
    uint8_t         SSID[32];
    uint8_t         PW[32];
//    uint8_t         rssi;
//    uint8_t         ber;
//    uint8_t         IMEI[GSM_IMEI_LEN + 1];
//    //struct tm       time;
//    int8_t          time_zone;
//    rt_time_t       time_UTC;
    
    s_WIFI_service  *service[ESP_WIFI_SERVICE_MAX];
    
} s_ESP_CB;

extern s_ESP_CB ESP_CB;

extern struct rt_semaphore sem_WIFI_up;
extern struct rt_semaphore sem_WIFI_down;
extern struct rt_mutex mt_WIFI_send;

extern struct rt_semaphore sem_WIFI_ESP_07_rx;
extern struct rt_semaphore sem_WIFI_ESP_07_tx;

extern struct rt_device * device_WIFI_ESP_07;

extern void rt_thread_entry_WIFI_ESP_07(void* parameter);
extern void rt_thread_entry_WIFI_rx(void* parameter);


extern uint8_t WIFI_ESP_07_init(rt_device_t dev);
extern void WIFI_send(char *cmd);



#endif // __WIFI_ESP_07_H__

