#include "WIFI_ESP_07.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>

rt_uint8_t prio_WIFI_ESP_07 = 12;
rt_thread_t thread_WIFI_ESP_07;

rt_uint8_t prio_WIFI_rx = 12;
rt_thread_t thread_WIFI_rx;

struct rt_device *device_WIFI_ESP_07;

struct rt_semaphore sem_WIFI_rx;
struct rt_semaphore sem_WIFI_tx;

uint8_t WIFI_rx_if_OK = 0;
uint8_t WIFI_rx_if_OK_temp = 0;
uint8_t WIFI_rx_OK_buf[16] = {0};

s_ESP_CB ESP_CB = {0};
s_ESP_CB *p_ESP_CB = NULL;

struct rt_semaphore sem_WIFI_up;
struct rt_semaphore sem_WIFI_down;
struct rt_mutex mt_WIFI_send;
struct rt_semaphore sem_ESP_rx_data;
//struct rt_semaphore sem_ESP_rx_data_2;
struct rt_mailbox mb_WIFI_rx;

int ESP_srv_init(rt_device_t dev)
{
    if (dev == NULL) return -2;
    
    p_ESP_CB = rt_malloc(sizeof(s_ESP_CB));
    if (p_ESP_CB == NULL)
    {
        return -1;
    }
    
    rt_memset(p_ESP_CB, 0x00, sizeof(s_ESP_CB));
    
    p_ESP_CB->dev = dev;
    
    p_ESP_CB->status.sys = ESP_idle;
    
    rt_memcpy(p_ESP_CB->SSID, WIFI_cfg.SSID, sizeof(p_ESP_CB->SSID));
    rt_memcpy(p_ESP_CB->PW, WIFI_cfg.PW, sizeof(p_ESP_CB->PW));
    
    return 0;
}






rt_err_t WIFI_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_WIFI_rx);    
    
    //rt_snprintf();

}

rt_err_t WIFI_tx_complete(rt_device_t dev, void *buffer)
{

////    rt_sem_release(&sem_GSM_tx);

}


int ESP_srv_power_on(void)
{
    p_ESP_CB->status.term_on = ESP_term_on_power_on;
    p_ESP_CB->status.sys = ESP_term_on;
}

int ESP_srv_shutdown(void)
{
    p_ESP_CB->status.term_off = ESP_term_off_OFF;
    p_ESP_CB->status.sys = ESP_term_off;
}

int ESP_srv_phaser(void)
{
    uint8_t data_temp = 0x00;
    int res = 0;
    static uint8_t *p = NULL;
    //static uint32_t SISR_data_len = 0;
//    static uint32_t SISR_head_len = 0;
    static uint8_t SISR_flag = 0;
    
    while(1)
    {
        res = rt_device_read(p_ESP_CB->dev, 0, &data_temp, 1);
        if (res <= 0)
        {
            break;
        }
        
        p_ESP_CB->rec_buf[p_ESP_CB->rec_len] = data_temp;
        p_ESP_CB->rec_len ++;
        
        // Song: if send data receive "> ", ready to send actual data.
        if ((p_ESP_CB->data_sending) && (p_ESP_CB->rec_len >= 2) &&
            (p_ESP_CB->rec_buf[p_ESP_CB->rec_len-2] == '>') &&
            (p_ESP_CB->rec_buf[p_ESP_CB->rec_len-1] == ' '))
        {
            return 0;
        }
        
        // Song: if receive data from "+IPD,", ignore the "0D 0A" in the data packet. 
        if ( (data_temp == ':') &&
            (p_ESP_CB->rec_len > (sizeof(ESP_AT_IPD) + 2)) // +IDP,0,135:
            )
        {
            
            p = strstr(p_ESP_CB->rec_buf, ESP_AT_IPD);
            if (p != NULL)
            {
                p += (sizeof(ESP_AT_IPD)-1);
                sscanf(p, "%d,%d", &p_ESP_CB->data_handle, &p_ESP_CB->data_len);
                //sscanf(p, "%*d,%d", &SISR_data_len);
                p = strstr(p, ",");
                if (p == NULL) return -2;
                p++;
                p = strstr(p, ":");
                if (p == NULL) return -2;
                p++;
                
                p_ESP_CB->rec_index = p - p_ESP_CB->rec_buf;
                //SISR_head_len = p - p_ESP_CB->rec_buf;
                SISR_flag = 1;
            }
        }
        
        
        if ((SISR_flag == 0) && 
            (p_ESP_CB->rec_len >= 2) && 
            (data_temp == 0x0A) && 
            (p_ESP_CB->rec_buf[p_ESP_CB->rec_len - 2] == 0x0D))
        {
            if (p_ESP_CB->rec_len == 2)
            {
                // Song:　Just only 0x0D 0x0A, ignore it.
                p_ESP_CB->rec_len = 0;
                return -1;
            }
            else
            {
                return 0;
            }
        }
        else if ((SISR_flag) &&
                 (p_ESP_CB->rec_len >= (p_ESP_CB->data_len + p_ESP_CB->rec_index)))
        {
                // Song:　The end of data packet 0x0D 0x0A.
                //p_ESP_CB->rec_len = p_ESP_CB->rec_len - 2;
                p_ESP_CB->ack.ack_stru.DATA_REC = 1;
                SISR_flag = 0;
                
                return 0;
        }
    }
    return -1;
}


rt_err_t WIFI_rx_check(rt_device_t dev)
{
    
    uint8_t c = 0x00;
    int res = 0;
   
    
    
    if (ESP_srv_phaser() == 0)
    {
        res = ESP_srv_rec_handler();
        if (res)
        {
            
        }
    }
    
    
    return 0;
}



int ESP_srv_send_data(uint8_t *data, uint32_t len)
{
    int res = 0;
    int res_2 = 0;
    int counter = 0;
    uint8_t *p = NULL;
    
    ////rt_mutex_take(&mt_GSM_send, RT_TICK_PER_SECOND * 5);
    
    p_ESP_CB->send_buf[p_ESP_CB->send_len] = 0x00;
    SYS_log( SYS_DEBUG_DEBUG, ("Send: %s\n", p_ESP_CB->send_buf));
    
    res = rt_device_write(p_ESP_CB->dev, 0, p_ESP_CB->send_buf, p_ESP_CB->send_len);
    
    if (len && (data != NULL))
    {
        // Waiting the SISW acked.
        p_ESP_CB->data_sending = 1;
        res = rt_sem_take(&sem_ESP_rx_data, RT_TICK_PER_SECOND * 5);
        
        if (res == RT_EOK)
        {
            p_ESP_CB->data_sending = 2;
            res = rt_device_write(p_ESP_CB->dev, 0, data, len);
            p_ESP_CB->send_len = 0;
            
            counter = 0;
            
            res_2 = rt_mb_recv(&mb_WIFI_rx, (uint32_t *)&p, RT_TICK_PER_SECOND * 5);
            if (res_2 == RT_EOK)
            {
                if (*p == 0)
                {
                    SYS_log( SYS_DEBUG_ERROR, ("WIFI data send error !\n"));
                    p_ESP_CB->data_sending = 0;
                    return -4;
                }
                else
                {
                    p_ESP_CB->data_sending = 0;
                    return res;
                }
            }
            else if (res_2 == -RT_ETIMEOUT)
            {
                SYS_log(SYS_DEBUG_ERROR, ("WIFI send data timeout !!!\n\n"));
                rt_thread_delay(RT_TICK_PER_SECOND * 2);
                sys_reboot();
                while(1) rt_thread_delay(1);
            }
            else
            {
                p_ESP_CB->data_sending = 0;
                return -2;
            }
            
//            while(p_ESP_CB->data_sending == 2)
//            {
//                if (counter > (RT_TICK_PER_SECOND*5))  // Special for WIFI, If does not receive "OK" within 15s, Error, report and restart system.
//                {
//                    SYS_log(SYS_DEBUG_ERROR, ("WIFI send data timeout !!!\n\n"));
//                    rt_thread_delay(RT_TICK_PER_SECOND * 2);
//                    sys_reboot();
//                    while(1) rt_thread_delay(1);
//                }
//                rt_thread_delay(1);
//                counter ++;
//            }
//            
//            if (p_ESP_CB->data_sending == 3)
//            {
//                p_ESP_CB->data_sending = 0;
//                return res;
//            }
//            else if (p_ESP_CB->data_sending == 4)
//            {
//                SYS_log( SYS_DEBUG_ERROR, ("WIFI data send error !\n"));
//                p_ESP_CB->data_sending = 0;
//                return -4;
//            }
//            else 
//            {
//                return -5;
//            }
            
            ////rt_mutex_release(&mt_GSM_send);
        }
        else if (res == -RT_ETIMEOUT)
        {
            p_ESP_CB->data_sending = 0;
            
            ////rt_mutex_release(&mt_GSM_send);
            return -1;
        }
        else
        {
            p_ESP_CB->data_sending = 0;
            
            ////rt_mutex_release(&mt_GSM_send);
            return -2;
        }
    }
    else 
    {
        ////rt_mutex_release(&mt_GSM_send);
        return res;
    }
}


int ESP_srv_AT_SEND_data(uint8_t index, uint8_t *data, uint32_t len)
{
    int res = 0;
    
    if (index >= ESP_WIFI_SERVICE_MAX) return -3;
    if (data == NULL) return -2;
    if (len > ESP_WIFI_SEND_MAX) return -1;
    
    p_ESP_CB->send_len = sprintf(p_ESP_CB->send_buf, ESP_AT_SEND, index, len);
        
    return ESP_srv_send_data(data, len);
}



int WIFI_srv_release(int handle)
{
    IF_NOT_WIFI_CONN_NUM(handle);

    p_ESP_CB->service[handle] = NULL;
    return 0;
    
}

int WIFI_srv_write(int handle, uint8_t *data, uint32_t len)
{
    IF_NOT_WIFI_CONN_NUM(handle);
    
    return ESP_srv_AT_SEND_data(handle, data, len);
}


void WIFI_srv_destroy(int handle)
{
    if ((handle >= 0) && (handle < ESP_WIFI_SERVICE_MAX))
    {
        rt_free(p_ESP_CB->service[handle]);
    }
}




int ESP_srv_AT_AT(void)
{

    p_ESP_CB->send_len = sizeof(ESP_AT_AT) - 1;
    rt_memcpy(p_ESP_CB->send_buf, ESP_AT_AT, p_ESP_CB->send_len);
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int ESP_srv_AT_CWMODE(void)
{

    p_ESP_CB->send_len = sizeof(ESP_AT_CWMODE) - 1;
    rt_memcpy(p_ESP_CB->send_buf, ESP_AT_CWMODE, p_ESP_CB->send_len);
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int ESP_srv_AT_RST(void)
{

    p_ESP_CB->send_len = sizeof(ESP_AT_RST) - 1;
    rt_memcpy(p_ESP_CB->send_buf, ESP_AT_RST, p_ESP_CB->send_len);
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int ESP_srv_AT_CWJAP(uint8_t *SSID, uint8_t *PW)
{
    p_ESP_CB->send_len = sprintf(p_ESP_CB->send_buf, 
                        ESP_AT_CWJAP, 
                        SSID, 
                        PW);
    
    ESP_srv_send_data(NULL, 0);
    return 0;
}


int ESP_srv_AT_CIPMUX(void)
{
    p_ESP_CB->send_len = sizeof(ESP_AT_CIPMUX_MULT) - 1;
    rt_memcpy(p_ESP_CB->send_buf, ESP_AT_CIPMUX_MULT, p_ESP_CB->send_len);
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int WIFI_srv_AT_CIPSTART(uint8_t index, e_WIFI_srv_type type, s_svr_ip *ip)
{
    //int res = 0;
    
    if (index >= ESP_WIFI_SERVICE_MAX) return -3;
    if (ip == NULL) return -2;


    if (type == WIFI_srv_UDP_client)
    {
        p_ESP_CB->send_len = sprintf(p_ESP_CB->send_buf, 
                        ESP_AT_CIPSTART_UDP, 
                        index, 
                        ip->addr[0], ip->addr[1], ip->addr[2], ip->addr[3],
                        ip->port);
    }
    else if (type == WIFI_srv_TCP_client)
    {
        p_ESP_CB->send_len = sprintf(p_ESP_CB->send_buf, 
                        ESP_AT_CIPSTART_TCP, 
                        index, 
                        ip->addr[0], ip->addr[1], ip->addr[2], ip->addr[3],
                        ip->port);
    }
    else
    {
        
        return -1;
    }
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int ESP_srv_AT_CIPCLOSE(int index)
{
    if (index >= ESP_WIFI_SERVICE_MAX) return -1;
 
    p_ESP_CB->send_len = sprintf(p_ESP_CB->send_buf, 
                        ESP_AT_CIPCLOSE, 
                        index);
    
    ESP_srv_send_data(NULL, 0);
    
    return 0;
}

int ESP_srv_ack_parser(void)
{
    uint8_t *p = NULL;
    int temp_1, temp_2, temp_3, temp_4;
    int temp_int = 0;
    
        if (strstr(p_ESP_CB->rec_buf, "> ") != NULL)
        {
            // TODO: deal with the internet connectioned report.
            if (p_ESP_CB->data_sending)
            {
                rt_sem_release(&sem_ESP_rx_data);
            }
        }
        else if (strstr(p_ESP_CB->rec_buf, "OK") != NULL)
        {
            // If data sending, ignore the "OK"
            if (p_ESP_CB->data_sending == 1)
            {
            
            }
            else if (p_ESP_CB->data_sending == 2)
            {
                //rt_sem_release(&sem_ESP_rx_data_2);
                WIFI_rx_if_OK = 1;
                rt_mb_send(&mb_WIFI_rx, (uint32_t)&WIFI_rx_if_OK);
                
            }
            else
            {
                p_ESP_CB->ack.ack_stru.OK = 1;
            }
        }
        else if (strstr(p_ESP_CB->rec_buf, "ERROR") != NULL)
        {
            // If data sending, ignore the "ERROR"
            if (p_ESP_CB->data_sending == 1)
            {
            
            }
            else if (p_ESP_CB->data_sending == 2)
            {
                //rt_sem_release(&sem_ESP_rx_data_2);
                WIFI_rx_if_OK = 0;
                rt_mb_send(&mb_WIFI_rx, (uint32_t)&WIFI_rx_if_OK);
            }
            else
            {
                p_ESP_CB->ack.ack_stru.ERROR = 1;
            }
        }
        else if ((p=strstr(p_ESP_CB->rec_buf, "CONNECT")) != NULL)
        {
            p_ESP_CB->ack.ack_stru.CONNECT = 1;
        }
        else if (strstr(p_ESP_CB->rec_buf, "ready") != NULL)
        {
            p_ESP_CB->ack.ack_stru.READY = 1;
        }
        else if ((p = strstr(p_ESP_CB->rec_buf, "FAIL")) != NULL)
        {
            p_ESP_CB->ack.ack_stru.FAIL = 1;
        }
        else if (p_ESP_CB->ack.ack_stru.DATA_REC)
        {
        
        }
//        else if (((p = strstr(p_ESP_CB->rec_buf, ESP_AT_IPD)) != NULL) && (p_ESP_CB->rec_len >= (SISR_data_len + p_ESP_CB->rec_index +2)))
//        {
//            // TODO: deal with the internet service report.
//            p += (sizeof(ESP_AT_IPD)-1);
//            sscanf(p, "%d,%d", &p_ESP_CB->data_handle, &p_ESP_CB->data_len);
//            p = strstr(p, ",");
//            p++;
//            p = strstr(p, ":");
//            p++;
//            //p_ESP_CB->rec_index = p - p_ESP_CB->rec_buf;
//            
//            p_ESP_CB->ack.ack_stru.DATA_REC = 1;
//        }
    
    else
    {
        p_ESP_CB->ack.ack_char = 0;
        //p_ESP_CB->ack.ack_stru.UNKNOWN = 1;
    }
    return 0;
}

int ESP_srv_rec_handler(void)
{
    uint8_t *p = NULL;
    uint32_t i = 0;
    
    p_ESP_CB->rec_buf[p_ESP_CB->rec_len] = 0;
    
    SYS_log( SYS_DEBUG_DEBUG, ("Rec : %s", p_ESP_CB->rec_buf));
    

    ESP_srv_ack_parser();
    
//    if ((p_ESP_CB->data_sending == 2) && p_ESP_CB->ack.ack_stru.OK)
//    {
//        p_ESP_CB->data_sending = 3; // OK, 
//        p_ESP_CB->ack.ack_stru.OK = 0;
//    }
//    else if ((p_ESP_CB->data_sending == 2) && p_ESP_CB->ack.ack_stru.ERROR)
//    {
//        p_ESP_CB->data_sending = 4;
//        p_ESP_CB->ack.ack_stru.ERROR = 0;
//    }
    
    
    switch (p_ESP_CB->status.sys)
    {
        case ESP_idle:
            
            break;
        case ESP_term_on:
            switch (p_ESP_CB->status.term_on)
            {
                case ESP_term_on_power_on:
                    break;
                case ESP_term_on_power_on_ack:
                    break;
                case ESP_term_on_reset:
                    break;
                case ESP_term_on_ON:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            // Jump to ESP_WIFI_active.
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_0;
                            p_ESP_CB->status.sys = ESP_WIFI_active;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                        else if (p_ESP_CB->ack.ack_stru.ERROR)
                        {
                            
                            p_ESP_CB->ack.ack_stru.ERROR = 0;
                        }
                        
                        
                    }
                    break;
                default:
                    break;
            }
            break;
        case ESP_term_reset:
            p_ESP_CB->status.term_on = ESP_term_on_reset;
            p_ESP_CB->status.sys = ESP_term_on;
            break;
        case ESP_WIFI_active:
            switch (p_ESP_CB->status.wifi_active)
            {
                case ESP_WIFI_phase_0_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_1;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case ESP_WIFI_phase_1_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_2;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case ESP_WIFI_phase_2_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.READY)
                        {
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_3;
                            p_ESP_CB->ack.ack_stru.READY = 0;
                        }
                    }
                    break;
                case ESP_WIFI_phase_3_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_4;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                        else if (p_ESP_CB->ack.ack_stru.FAIL)
                        {
                            SYS_log(SYS_DEBUG_ERROR, ("WIFI connect error ! Restart.\n"));
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_0;  // restart the wifi
                            p_ESP_CB->ack.ack_stru.FAIL = 0;
                        }
                        else if (p_ESP_CB->ack.ack_stru.ERROR)
                        {
                            SYS_log(SYS_DEBUG_ERROR, ("WIFI connect error ! Restart.\n"));
                            p_ESP_CB->status.wifi_active = ESP_WIFI_phase_0;  // restart the wifi
                            p_ESP_CB->ack.ack_stru.ERROR = 0;
                        }
                    }
                    break;
                case ESP_WIFI_phase_4_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->status.sys = ESP_WIFI_up;
                            p_ESP_CB->status.wifi_up = ESP_WIFI_init;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                        else if (p_ESP_CB->ack.ack_stru.ERROR)
                        {
                            
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case ESP_WIFI_up:
            switch (p_ESP_CB->status.wifi_up)
            {
                case ESP_WIFI_init:
                    break;
                case ESP_WIFI_idle:
                    WIFI_server_rec_handler();
                    break;
                default:
                    break;
            }
            break;
        case ESP_WIFI_down:
            switch (p_ESP_CB->status.wifi_down)
            {
                case ESP_WIFI_close:
                    break;
                case ESP_WIFI_close_ack:
                    p_ESP_CB->status.wifi_down = ESP_WIFI_closed;
                    break;
                case ESP_WIFI_closed:
                    break;
                default:
                    break;
            }
            break;
        case ESP_term_off:
            switch (p_ESP_CB->status.term_off)
            {
                case ESP_term_off_OFF_ack:
                    break;
                case ESP_term_off_power_off:
                    
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    p_ESP_CB->rec_len = 0;
    return 0;
}



int WIFI_server_rec_handler(void)
{

    int i = 0;
    
    for (i=0;i<ESP_WIFI_SERVICE_MAX;i++)
    {
        if (p_ESP_CB->service[i] != NULL)
        {
            switch (p_ESP_CB->service[i]->status)
            {
                case WIFI_idle:
                    
                    break;
                case WIFI_CIPSTART_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.FAIL)
                        {
                            //p_ESP_CB->service[i]->status = WIFI_SISS_addr;
                            p_ESP_CB->ack.ack_stru.FAIL = 0;
                            
                            SYS_log(SYS_DEBUG_ERROR, ("WIFI connect server failed !\r\n"));
                            while(1) rt_thread_delay(5);
                            // Song: TODO
                            
                        }
                        else if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->service[i]->status = WIFI_attached;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
//                        else if (p_ESP_CB->ack.ack_stru.CONNECT)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_attached;
//                            p_ESP_CB->ack.ack_stru.CONNECT = 0;
//                        }
                    }
                    break;
//                case WIFI_SISS_addr_ack:
//                    if (p_ESP_CB->ack.ack_char)
//                    {
//                        if (p_ESP_CB->ack.ack_stru.OK)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_SISO;
//                            p_ESP_CB->ack.ack_stru.OK = 0;
//                        }
//                    }
//                    break;
//                case WIFI_SISO_ack:
//                    if (p_ESP_CB->ack.ack_char)
//                    {
//                        
//                        if (p_ESP_CB->ack.ack_stru.OK)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_SISO_INFO;
//                            p_ESP_CB->ack.ack_stru.OK = 0;
//                        }
//                        else if (p_ESP_CB->ack.ack_stru.ERROR)
//                        {
//                            if (p_ESP_CB->ack.ack_stru.SIS)
//                            {
//                                // TODO: check the fail information.
//                                if ((p_ESP_CB->SIS.handle == i) && 
//                                    (p_ESP_CB->SIS.URC == 0) && 
//                                    (p_ESP_CB->SIS.error == WIFI_SIS_TCP_REJECT))
//                                {
//                                    if (p_ESP_CB->service[i]->rejected != NULL)
//                                    {
//                                        p_ESP_CB->service[i]->rejected(NULL);
//                                    }
//                                }
//                                p_ESP_CB->ack.ack_stru.SIS = 0;
//                            }
//                        
//                            p_ESP_CB->service[i]->status = WIFI_discon;
//                            p_ESP_CB->ack.ack_stru.ERROR = 0;
//                        }
//                    }
//                    break;
//                case WIFI_SISO_INFO:
//                    break;
//                case WIFI_SISO_INFO_ack:
//                    if (p_ESP_CB->ack.ack_char)
//                    {
//                        if (p_ESP_CB->ack.ack_stru.OK)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_attached;
//                            p_ESP_CB->ack.ack_stru.OK = 0;
//                        }
//                    }
//                    break;
//                case WIFI_SISS_type_listen:
//                    break;
//                case WIFI_SISS_type_listen_ack:
//                    if (p_ESP_CB->ack.ack_char)
//                    {
//                        if (p_ESP_CB->ack.ack_stru.OK)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_SISS_listen;
//                            p_ESP_CB->ack.ack_stru.OK = 0;
//                        }
//                    }
//                    break;
//                case WIFI_SISS_listen:
//                    break;
//                case WIFI_SISS_listen_ack:
//                    if (p_ESP_CB->ack.ack_char)
//                    {
//                        if (p_ESP_CB->ack.ack_stru.OK)
//                        {
//                            p_ESP_CB->service[i]->status = WIFI_SISO;
//                            p_ESP_CB->ack.ack_stru.OK = 0;
//                        }
//                    }
//                    break;
                case WIFI_attached:
                    break;
                case WIFI_up:
                    if (p_ESP_CB->ack.ack_stru.DATA_REC)
                    {
                        if (p_ESP_CB->data_handle == i)
                        {
                            if (p_ESP_CB->service[i]->received != NULL)
                            {
                                p_ESP_CB->service[i]->received(p_ESP_CB->service[i]->handler, &p_ESP_CB->rec_buf[p_ESP_CB->rec_index], p_ESP_CB->data_len);
                            }
                            p_ESP_CB->ack.ack_stru.DATA_REC = 0;
                        }                            
                    }
//                    if (p_ESP_CB->ack.ack_stru.SIS)
//                    {
//                        if ((p_ESP_CB->SIS.handle == i) && 
//                            (p_ESP_CB->SIS.URC == 0) && 
//                            (p_ESP_CB->SIS.error == WIFI_SIS_TCP_DISCONNECT))
//                        {
//                            if (p_ESP_CB->service[i]->closed != NULL)
//                            {
//                                p_ESP_CB->service[i]->closed(NULL);
//                            }
//                            
//                            //p_ESP_CB->service[i]->status = GPRS_discon;
//                            p_ESP_CB->ack.ack_stru.SIS = 0;
//                            
//                            // Song: TODO: when server cut off the connection. need to deal with.
//                            SYS_log(SYS_DEBUG_ERROR,("Server cut off the connection !!! Restart the system.\n "));
//                            sys_reboot();
//                            while(1) rt_thread_delay(5);
//                            
//                        }
//                    }
                    break;
                case WIFI_release_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.OK)
                        {
                            p_ESP_CB->service[i]->status = WIFI_discon;
                            p_ESP_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    //p_ESP_CB->service[i]->status = GPRS_discon;
                    break;
                case WIFI_discon:
                    break;
                default:
                    break;
            }
        }
    }
    
    return 0;
}



int WIFI_server_handler(void)
{

    int i = 0;
    
    for (i=0;i<ESP_WIFI_SERVICE_MAX;i++)
    {
        if (p_ESP_CB->service[i] != NULL)
        {
            switch (p_ESP_CB->service[i]->status)
            {
                case WIFI_idle:
                    
                    break;
                case WIFI_CIPSTART:
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    WIFI_srv_AT_CIPSTART(i, p_ESP_CB->service[i]->cfg.srv_type, &p_ESP_CB->service[i]->cfg.srv_ip);
                    p_ESP_CB->service[i]->status = WIFI_CIPSTART_ack;
                    break;
//                case WIFI_SISS_addr:
//                    rt_thread_delay(RT_TICK_PER_SECOND/10);
//                    if (p_ESP_CB->service[i]->cfg.if_use_name == 0)
//                    {
//                        WIFI_srv_AT_CIPSTART(i, p_ESP_CB->service[i]->cfg.srv_type, &p_ESP_CB->service[i]->cfg.srv_ip);
//                    }
//                    else
//                    {
//                        SYS_log(SYS_DEBUG_ERROR, ("WIFI module not support host name !\r\n"));
//                        while(1) {rt_thread_delay(5);}
//                        //GSM_srv_AT_SISS_name(i, p_ESP_CB->service[i]->cfg.srv_type, &p_ESP_CB->service[i]->cfg.svr_name);
//                    }
//                    p_ESP_CB->service[i]->status = WIFI_SISS_addr_ack;
//                    break;
//                case WIFI_SISO:
//                    rt_thread_delay(RT_TICK_PER_SECOND/10);
//                    //GSM_srv_AT_SISO(i);
//                    p_ESP_CB->service[i]->status = WIFI_SISO_ack;
//                    break;
//                case WIFI_SISO_INFO:
//                    rt_thread_delay(RT_TICK_PER_SECOND/10);
//                    //GSM_srv_AT_SISO_info();
//                    p_ESP_CB->service[i]->status = WIFI_SISO_INFO_ack;
//                    break;
//                case WIFI_SISO_INFO_ack:
//                    break;
//                case WIFI_SISS_type_listen:
//                    //GSM_srv_AT_SISS_TYPE(i);
//                    p_ESP_CB->service[i]->status = WIFI_SISS_type_listen_ack;
//                    break;
//                case WIFI_SISS_type_listen_ack:
//                    break;
//                case WIFI_SISS_listen:
//                    //GSM_srv_AT_SISS_listen(i, p_ESP_CB->service[i]->cfg.srv_type, p_ESP_CB->service[i]->cfg.listen_port);
//                    p_ESP_CB->service[i]->status = WIFI_SISS_listen_ack;
//                    break;
//                case WIFI_SISS_listen_ack:
//                    break;
                case WIFI_attached:
                    if (p_ESP_CB->service[i]->connected != NULL)
                    {
                        p_ESP_CB->service[i]->connected(&i);
                    }
                    p_ESP_CB->service[i]->status = WIFI_up;
                    break;
                case WIFI_up:
                    // GPRS_up
//                    if (((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested)) && (sys_config.test_flag.GSM == 0))
//                    {
//                        sys_config.test_flag.GSM = 1;
//                        while (1) rt_thread_delay(5); // infinite loop 
//                    }
                    
                    break;
                case WIFI_release:
                    ESP_srv_AT_CIPCLOSE(i);
                    p_ESP_CB->service[i]->status = WIFI_release_ack;
                    break;
                case WIFI_discon:
                    if (p_ESP_CB->service[i]->disconnected != NULL)
                    {
                        p_ESP_CB->service[i]->disconnected(p_ESP_CB->service[i]->handler, NULL, i);
                    }
                    WIFI_srv_destroy(i);
                    WIFI_srv_release(i);
                    break;
                default:
                    break;
            }
        }
    }
    
    return 0;
}


s_WIFI_service* WIFI_srv_create(s_WIFI_srv_cfg *cfg, void *handler)
{
    s_WIFI_service *WIFI = NULL;
    
    WIFI = rt_malloc(sizeof(s_WIFI_service));

    if (WIFI == NULL)
    {
        return NULL;
    }
    else
    {
        rt_memset(WIFI, 0, sizeof(s_WIFI_service));
        
        rt_memcpy(&WIFI->cfg, cfg, sizeof(s_WIFI_srv_cfg));
        WIFI->status = WIFI_idle;
        WIFI->handler = handler;
        return WIFI;
    }
    
}

int WIFI_srv_attach(s_WIFI_service *WIFI)
{
    int i = 0;
    
    for (i=0;i<ESP_WIFI_SERVICE_MAX;i++)
    {
        if (p_ESP_CB->service[i] == NULL)
        {
            p_ESP_CB->service[i] = WIFI;
            return i;
        }
    }

    return -1;
}

void *WIFI_srv_get(int handle)
{
    return p_ESP_CB->service[handle];
}


int WIFI_connect(void * handler ,
                s_WIFI_srv_cfg *cfg,
                pFun_WIFI_connect_fail connect_fail,
                pFun_WIFI_connected connected,
                pFun_WIFI_disconnected  disconnected,
                pFun_WIFI_received  received,
                pFun_WIFI_rejected  rejected,
                pFun_WIFI_closed    closed,
                pFun_WIFI_report_IP report_IP
                )
{
    int res = 0;
    s_WIFI_service *WIFI_service = NULL;
    
    WIFI_service = WIFI_srv_create(cfg, handler);
    if (WIFI_service == NULL) return -1;
    
    res = WIFI_srv_attach(WIFI_service);
    if (res < 0) return -2;
    
    if (cfg->srv_type == WIFI_srv_UDP_client)
    {
        WIFI_service->status = WIFI_CIPSTART;
    }
    else if (cfg->srv_type == WIFI_srv_TCP_client)
    {
        WIFI_service->status = WIFI_CIPSTART;
    }
    
    WIFI_service->connect_fail = connect_fail;
    WIFI_service->connected = connected;
    WIFI_service->disconnected = disconnected;
    WIFI_service->received = received;
    WIFI_service->rejected = rejected;
    WIFI_service->closed = closed;
    WIFI_service->report_IP = report_IP;
    

    return res;
}

int WIFI_disconnect(int handle)
{

    p_ESP_CB->service[handle]->status = WIFI_release;
    
}

int ESP_srv_service(void)
{

    switch (p_ESP_CB->status.sys)
    {
        case ESP_idle:
            break;
        case ESP_term_on:
            switch (p_ESP_CB->status.term_on)
            {
                case ESP_term_on_power_on:
                
                    // Song:　Initialize the WIFI ESP_07 power control and reset.
                    rt_pin_write(PIN_WIFI_SHUT, PIN_HIGH);
                    rt_pin_write(PIN_WIFI_RESET, PIN_HIGH);
                    rt_thread_delay(RT_TICK_PER_SECOND / 10);
                    rt_pin_write(PIN_WIFI_RESET, PIN_LOW);
                    rt_thread_delay(RT_TICK_PER_SECOND / 10);
                    rt_pin_write(PIN_WIFI_RESET, PIN_HIGH);
                    rt_thread_delay(RT_TICK_PER_SECOND / 10);

                
                    p_ESP_CB->status.term_on = ESP_term_on_power_on_ack;
                    break;
                case ESP_term_on_power_on_ack:
                    if (p_ESP_CB->ack.ack_char)
                    {
                        if (p_ESP_CB->ack.ack_stru.READY)
                        {
                            p_ESP_CB->status.term_on = ESP_term_on_ON;
                            p_ESP_CB->ack.ack_stru.SHUTDOWN = 0;
                        }
                    }
                    
//                    #if PATCH_GSM_SISW_EN
//                    rt_device_write(p_ESP_CB->dev, 0, GSM_AT_DUMMY, sizeof(GSM_AT_DUMMY)-1);
//                    #endif // PATCH_GSM_SISW_EN
                    break;
                case ESP_term_on_reset:
//                    rt_pin_write(PIN_GSM_RST    , GSM_RST_EN); 
//                    rt_thread_delay(RT_TICK_PER_SECOND / 10);
//                    rt_pin_write(PIN_GSM_RST    , GSM_RST_DIS); 
//                    rt_thread_delay(RT_TICK_PER_SECOND / 1);
                
                    p_ESP_CB->status.term_on = ESP_term_on_ON;
                    break;
                case ESP_term_on_ON:
                    // Jump to GSM_GPRS_active
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_0;
                    p_ESP_CB->status.sys = ESP_WIFI_active;
                    break;
                default:
                    break;
            }
            break;
        case ESP_term_reset:
            p_ESP_CB->status.term_on = ESP_term_on_reset;
            p_ESP_CB->status.sys = ESP_term_on;
            break;
        case ESP_WIFI_active:
            switch (p_ESP_CB->status.wifi_active)
            {
                case ESP_WIFI_phase_0:
                    ESP_srv_AT_AT();
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_0_ack;
                    break;
                case ESP_WIFI_phase_1:
                    ESP_srv_AT_CWMODE();
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_1_ack;
                    break;
                case ESP_WIFI_phase_2:
                    ESP_srv_AT_RST();
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_2_ack;
                    break;
                case ESP_WIFI_phase_3:
                    ESP_srv_AT_CWJAP(p_ESP_CB->SSID, p_ESP_CB->PW);
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_3_ack;
                    break;
                case ESP_WIFI_phase_4:
                    //rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    ESP_srv_AT_CIPMUX();
                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_4_ack;
                    break;
//                case ESP_WIFI_phase_5:
//                    //GSM_srv_AT_CGSN();
//                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_5_ack;
//                    break;
//                case ESP_WIFI_phase_6:
//                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
//                    //GSM_srv_AT_CSQ();
//                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_6_ack;
//                    break;
//                case ESP_WIFI_phase_6_retry:
//                    //rt_thread_delay(RT_TICK_PER_SECOND * 3);
//                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
//                    p_ESP_CB->status.wifi_active = ESP_WIFI_phase_6;
//                    break;
                default:
                    break;
            }
            break;
        case ESP_WIFI_up:
            switch (p_ESP_CB->status.wifi_up)
            {
                case ESP_WIFI_init:
                    p_ESP_CB->status.wifi_up = ESP_WIFI_idle;
                    rt_sem_release(&sem_WIFI_up);
                
                    break;
                case ESP_WIFI_idle:
                    WIFI_server_handler();
                    
                    break;
                default:
                    break;
            }
            break;
        case ESP_WIFI_down:
            switch (p_ESP_CB->status.wifi_down)
            {
                case ESP_WIFI_close:
                    p_ESP_CB->status.wifi_down = ESP_WIFI_close_ack;
                    
                    break;
                case ESP_WIFI_closed:
                    rt_sem_release(&sem_WIFI_down);
                    p_ESP_CB->status.term_off = ESP_term_off_OFF;
                    p_ESP_CB->status.sys = ESP_term_off;
                    break;
                default:
                    break;
            }
            break;
        case ESP_term_off:
            switch (p_ESP_CB->status.term_off)
            {
                case ESP_term_off_OFF:
                    rt_pin_write(PIN_WIFI_SHUT, PIN_LOW);
                
                    p_ESP_CB->status.term_off = ESP_term_off_power_off;
                    break;
                case ESP_term_off_power_off:
////                    if (sys_config.mother_type != mother_type_GPRS_1_2)
////                    {
//                        rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_OFF);
////                    }
////#if USE_GSM_PWR_SHUT    
////                    rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_OFF);
////#endif // USE_GSM_PWR_SHUT    

                    SYS_log( SYS_DEBUG_INFO, ("WIFI module power off !\n"));
                
                    // Jump to ESP_WIFI_active
                    p_ESP_CB->status.sys = ESP_idle;
                    break;
                default:
                    break;
            }
            
            break;
        default:
            break;
    }
    
    return 0;
}


void rt_thread_entry_WIFI_rx(void* parameter)
{
    uint32_t e;
    int res = 0;

    //rt_thread_t A8_if_tid;
    
    rt_sem_init(&sem_WIFI_rx, "WIFI_rx", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_ESP_rx_data, "WIFI_rx_data", 0, RT_IPC_FLAG_FIFO);
    //rt_sem_init(&sem_ESP_rx_data_2, "WIFI_rx_data_2", 0, RT_IPC_FLAG_FIFO);
    rt_mb_init( &mb_WIFI_rx,
                "mb_WIFI_rx",
                &WIFI_rx_OK_buf,
                sizeof(WIFI_rx_OK_buf)/4,
                RT_IPC_FLAG_FIFO
                );  
    
   
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
 
    rt_sem_init(&sem_WIFI_up, "WIFI_up", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_WIFI_down, "WIFI_down", 0, RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_WIFI_send, "WIFI_send", RT_IPC_FLAG_FIFO);
    ////WIFI_ESP_07_init(device_WIFI_ESP_07);

    if (ESP_srv_init(device_WIFI_ESP_07) < 0)
    {
        SYS_log(SYS_DEBUG_ERROR, ("WIFI callback malloc failed! \n"));
        while(1) rt_thread_delay(10);
    }    
    
    ESP_srv_power_on();
    
    while(1)
    {
        ////rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT, sizeof(GSM_CMD_AT) - 1);
        ESP_srv_service();
        rt_thread_delay(1);
    }
    
}



#ifdef RT_USING_FINSH
#include <finsh.h>

uint8_t WIFI_ESP_07_init(rt_device_t dev)
{

    // Song:　Initialize the WIFI ESP_07 power control and reset.
    rt_pin_write(PIN_WIFI_SHUT, PIN_HIGH);
    rt_pin_write(PIN_WIFI_RESET, PIN_HIGH);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    rt_pin_write(PIN_WIFI_RESET, PIN_LOW);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    rt_pin_write(PIN_WIFI_RESET, PIN_HIGH);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);

    
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






