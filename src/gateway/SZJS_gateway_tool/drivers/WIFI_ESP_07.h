#ifndef __WIFI_ESP_07_H__
#define __WIFI_ESP_07_H__

#include <rtthread.h>
#include "board.h"


#define THREAD_WIFI_ESP_07_STACK_SIZE     2048
extern rt_uint8_t prio_WIFI_ESP_07;
extern rt_thread_t thread_WIFI_ESP_07;

#define THREAD_WIFI_RX_STACK_SIZE        2048
extern rt_uint8_t prio_WIFI_rx;
extern rt_thread_t thread_WIFI_rx;



#define WIFI_ESP_CMD_AT             "AT\r\n"

#define WIFI_ESP_CMD_AT_CWMODE      "AT+CWMODE=1\r\n"
#define WIFI_ESP_CMD_AT_RST         "AT+RST\r\n"
#define WIFI_ESP_CMD_AT_CWJAP       "AT+CWJAP=\"SZJS_789C\",\"szjs1234\"\r\n"

#define WIFI_ESP_CMD_AT_CIPMUX      "AT+CIPMUX=0\r\n"  // single connection.
//#define WIFI_ESP_CMD_AT_CIPMUX    "AT+CIPMUX=1\r\n"  // multiple connection.
#define WIFI_ESP_CMD_AT_CIPSTART    "AT+CIPSTART=\"TCP\",\"192.168.0.86\",12345\r\n"
#define WIFI_ESP_CMD_AT_CIPSEND     "AT+CIPSEND="





extern struct rt_semaphore sem_WIFI_ESP_07_rx;
extern struct rt_semaphore sem_WIFI_ESP_07_tx;

extern struct rt_device * device_WIFI_ESP_07;

extern void rt_thread_entry_WIFI_ESP_07(void* parameter);
extern void rt_thread_entry_WIFI_rx(void* parameter);


extern uint8_t WIFI_ESP_07_init(rt_device_t dev);
extern void WIFI_send(char *cmd);



#endif // __WIFI_ESP_07_H__

