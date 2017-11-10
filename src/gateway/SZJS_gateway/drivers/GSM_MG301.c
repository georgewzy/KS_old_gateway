#include "GSM_mg301.h"
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>


#define DEBUG_GSM_EN    1

#if DEBUG_GSM_EN
    #define GSM_PRINTF   rt_kprintf
#else
    #define GSM_PRINTF(s)
#endif // DEBUG_GSM_EN



s_GPRS_srv_cfg  GPRS_cfg_SZJS = {
.srv_type = GPRS_srv_UDP_client,
.srv_ip = {
        {121,236,56,212},
        12345
    }
};

s_GPRS_srv_cfg  GPRS_cfg_XFZD = {
.srv_type = GPRS_srv_TCP_client,
.srv_ip = {
        {121,236,56,212},
        12345
    }
};

//s_GPRS_service  *GPRS_srv_SZJS = NULL;
//s_GPRS_service  *GPRS_srv_XFZD = NULL;


//s_GSM_CB GSM_CB = {0};

s_GSM_CB *p_GSM_CB = NULL;

rt_uint8_t prio_GSM_MG301 = 9;
rt_thread_t thread_GSM_MG301;


struct rt_device *device_GSM_MG301;

struct rt_semaphore sem_GSM_rx;
struct rt_semaphore sem_GSM_tx;

struct rt_semaphore sem_GSM_rx_data;

struct rt_semaphore sem_GSM_up;
struct rt_semaphore sem_GSM_down;

struct rt_mutex mt_GSM_send;

rt_uint8_t prio_GSM_rx = 5;
rt_thread_t thread_GSM_rx;

static uint8_t GSM_srv_inited = 0;

static struct tm GSM_time = {0};

int con_SZJS = -1;
int con_XFZD = -1;


void GSM_cmd_init(uint8_t if_tcp, uint8_t *IP_port);
int GPRS_server_rec_handler(void);
void GPRS_srv_destroy(int handle);
rt_err_t GSM_MG301_rx_ind(rt_device_t dev, rt_size_t size);
rt_err_t GSM_MG301_tx_complete(rt_device_t dev, void *buffer);

int GSM_srv_init(rt_device_t dev)
{
    if (dev == NULL) return -2;
    
    p_GSM_CB = rt_malloc(sizeof(s_GSM_CB));
    if (p_GSM_CB == NULL)
    {
        return -1;
    }
 
    rt_memset(p_GSM_CB, 0x00, sizeof(s_GSM_CB));
    
    p_GSM_CB->dev = dev;
    
    p_GSM_CB->status.sys = GSM_idle;
    
    
    return 0;
}



int GSM_srv_phaser(void)
{
    uint8_t data_temp = 0x00;
    int res = 0;
    static uint8_t *p = NULL;
    static uint32_t SISR_data_len = 0;
    static uint32_t SISR_head_len = 0;
    static uint8_t SISR_flag = 0;
    
    while(1)
    {
        res = rt_device_read(p_GSM_CB->dev, 0, &data_temp, 1);
        if (res <= 0)
        {
            break;
        }
        
        p_GSM_CB->rec_buf[p_GSM_CB->rec_len] = data_temp;
        p_GSM_CB->rec_len ++;
        
        
        // Song: if receive data from "^SISR:", ignore the "0D 0A" in the data packet. 
        if ( (data_temp == ',') &&
            (p_GSM_CB->rec_len > (sizeof("^SISR:") + 2)) // ^SISR:0,1024,
            )
        {
            
            p = strstr(p_GSM_CB->rec_buf, "^SISR:");
            if (p != NULL)
            {
                p += (sizeof("^SISR:")-1);
                sscanf(p, "%*d,%d", &SISR_data_len);
                p = strstr(p, ",");
                if (p == NULL) return -2;
                p++;
                p = strstr(p, ",");
                if (p == NULL) return -2;
                p++;
                
                SISR_head_len = p - p_GSM_CB->rec_buf;
                SISR_flag = 1;
            }
        }
        
        
        if ((SISR_flag == 0) && 
            (p_GSM_CB->rec_len >= 2) && 
            (data_temp == 0x0A) && 
            (p_GSM_CB->rec_buf[p_GSM_CB->rec_len - 2] == 0x0D))
        {
            if (p_GSM_CB->rec_len == 2)
            {
                // Song:　Just only 0x0D 0x0A, ignore it.
                p_GSM_CB->rec_len = 0;
                return -1;
            }
            else
            {
                return 0;
            }
        }
        else if ((SISR_flag) &&
                 (p_GSM_CB->rec_len >= (SISR_data_len + SISR_head_len +2)) &&
                (data_temp == 0x0A) && 
                (p_GSM_CB->rec_buf[p_GSM_CB->rec_len - 2] == 0x0D))
        {
                // Song:　The end of data packet 0x0D 0x0A.
                //p_GSM_CB->rec_len = p_GSM_CB->rec_len - 2;
                SISR_flag = 0;
                
                return 0;
        }
    }
    return -1;
}


int GSM_srv_send_data(uint8_t *data, uint32_t len)
{
    int res = 0;
    int counter = 0;
    
    ////rt_mutex_take(&mt_GSM_send, RT_TICK_PER_SECOND * 5);
    
    p_GSM_CB->send_buf[p_GSM_CB->send_len] = 0x00;
    SYS_log( SYS_DEBUG_DEBUG, ("Send: %s\n", p_GSM_CB->send_buf));
    
    res = rt_device_write(p_GSM_CB->dev, 0, p_GSM_CB->send_buf, p_GSM_CB->send_len);
    
    if (len && (data != NULL))
    {
        // Waiting the SISW acked.
        p_GSM_CB->data_sending = 1;
        res = rt_sem_take(&sem_GSM_rx_data, RT_TICK_PER_SECOND * 5);
        
        if (res == RT_EOK)
        {
            p_GSM_CB->data_sending = 2;
            res = rt_device_write(p_GSM_CB->dev, 0, data, len);
            p_GSM_CB->send_len = 0;
            
            counter = 0;
            while(p_GSM_CB->data_sending == 2)
            {
                if (counter > (RT_TICK_PER_SECOND*3))  // If does not receive "OK" within 3s, Error, report and restart system.
                {
                    SYS_log(SYS_DEBUG_ERROR, ("GSM send data timeout !!!\n\n"));
                    rt_thread_delay(RT_TICK_PER_SECOND * 2);
                    sys_reboot();
                    while(1) rt_thread_delay(1);
                }
                rt_thread_delay(1);
                counter ++;
            }
            
            if (p_GSM_CB->data_sending == 3)
            {
                p_GSM_CB->data_sending = 0;
                return res;
            }
            else if (p_GSM_CB->data_sending == 4)
            {
                SYS_log( SYS_DEBUG_ERROR, ("GSM data send error !\n"));
                p_GSM_CB->data_sending = 0;
                return -4;
            }
            else 
            {
                return -5;
            }
            
            ////rt_mutex_release(&mt_GSM_send);
        }
        else if (res == -RT_ETIMEOUT)
        {
            p_GSM_CB->data_sending = 0;
            
            ////rt_mutex_release(&mt_GSM_send);
            return -1;
        }
        else
        {
            p_GSM_CB->data_sending = 0;
            
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


int GSM_srv_AT_AT(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_AT) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_AT, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_ATE(void)
{

    p_GSM_CB->send_len = sizeof(GSM_ATE) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_ATE, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_ATI(void)
{

    p_GSM_CB->send_len = sizeof(GSM_ATI) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_ATI, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}


int GSM_srv_AT_SWSPATH(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_SWSPATH) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_SWSPATH, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_CPIN(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_CPIN) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_CPIN, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}


// Song: check the IMEI
int GSM_srv_AT_CGSN(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_CGSN) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_CGSN, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_CSQ(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_CSQ) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_CSQ, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_CGATT(void)
{

    p_GSM_CB->send_len = sizeof(GSM_AT_CGATT) - 1;
    rt_memcpy(p_GSM_CB->send_buf, GSM_AT_CGATT, p_GSM_CB->send_len);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}


int GSM_srv_AT_SICS(uint8_t index)
{
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;

    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SICS, index);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SICS_APN(uint8_t index, uint8_t *APN)
{
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;

    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SICS_APN, index, APN);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISS_TYPE(uint8_t index)
{
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;

    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SISS_TYPE, index);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_IOMODE(uint8_t if_BCD, uint8_t iomode)
{

    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_IOMODE, if_BCD, iomode);
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISS_listen(uint8_t index, e_GPRS_srv_type type, uint16_t listen_port)
{
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;


    if (type == GPRS_srv_UDP_server)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_LISTEN_UDP, 
                        index, 
                        listen_port);
    }
    else if (type == GPRS_srv_TCP_server)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_LISTEN_TCP, 
                        index, 
                        listen_port);
    }
    else
    {
        
        return -1;
    }
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}


int GSM_srv_AT_SISS_ip(uint8_t index, e_GPRS_srv_type type, s_svr_ip *ip)
{
    //int res = 0;
    
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;
    if (ip == NULL) return -2;


    if (type == GPRS_srv_UDP_client)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_ADDR_UDP, 
                        index, 
                        ip->addr[0], ip->addr[1], ip->addr[2], ip->addr[3],
                        ip->port);
    }
    else if (type == GPRS_srv_TCP_client)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_ADDR_TCP, 
                        index, 
                        ip->addr[0], ip->addr[1], ip->addr[2], ip->addr[3],
                        ip->port);
    }
    else
    {
        
        return -1;
    }
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISS_name(uint8_t index, e_GPRS_srv_type type, s_svr_name *name)
{
    //int res = 0;
    
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;
    if (name == NULL) return -2;


    if (type == GPRS_srv_UDP_client)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_NAME_UDP, 
                        index, 
                        name->name,
                        name->port);
    }
    else if (type == GPRS_srv_TCP_client)
    {
        p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, 
                        GSM_AT_SISS_NAME_TCP, 
                        index, 
                        name->name,
                        name->port);
    }
    else
    {
        
        return -1;
    }
    
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISO(uint8_t index)
{
    int res = 0;
    
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;
    
    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SISO, index);
        
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISO_info(void)
{
    
    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SISO_INFO);
        
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISC(uint8_t index)
{
    int res = 0;
    
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;
    
    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SISC, index);
        
    GSM_srv_send_data(NULL, 0);
    
    return 0;
}

int GSM_srv_AT_SISW_data(uint8_t index, uint8_t *data, uint32_t len)
{
    int res = 0;
    
    if (index >= GSM_GPRS_SERVICE_MAX) return -3;
    if (data == NULL) return -2;
    if (len > GSM_GPRS_SEND_MAX) return -1;
    
    p_GSM_CB->send_len = sprintf(p_GSM_CB->send_buf, GSM_AT_SISW, index, len);
        
    return GSM_srv_send_data(data, len);
}


int GSM_srv_power_on(void)
{
    p_GSM_CB->status.term_on = GSM_term_on_power_on;
    p_GSM_CB->status.sys = GSM_term_on;
}

int GSM_srv_shutdown(void)
{
    p_GSM_CB->status.term_off = GSM_term_off_OFF;
    p_GSM_CB->status.sys = GSM_term_off;
}

int GSM_srv_gprs_down(void)
{
    p_GSM_CB->status.gprs_down = GSM_gprs_close;
    p_GSM_CB->status.sys = GSM_GPRS_down;
}

int GSM_srv_gprs_down_force(void)
{
    int i = 0;
    
    for (i=0;i<GSM_GPRS_SERVICE_MAX;i++)
    {
        if (p_GSM_CB->service[i] != NULL)
        {
            GPRS_srv_destroy(i);
            GPRS_srv_release(i);
        }
        
    }
    GSM_srv_gprs_down();
}

// srv_num
int GSM_srv_ack_parser(void)
{
    uint8_t *p = NULL;
    int temp_1, temp_2, temp_3, temp_4;
    int temp_int = 0;
    
    if (strstr(p_GSM_CB->rec_buf, "OK") != NULL)
    {
        p_GSM_CB->ack.ack_stru.OK = 1;
    }
    else if (strstr(p_GSM_CB->rec_buf, "ERROR") != NULL)
    {
        p_GSM_CB->ack.ack_stru.ERROR = 1;
    }
    //else if (strstr(p_GSM_CB->rec_buf, "+") != NULL)
    //{
    else if ((p=strstr(p_GSM_CB->rec_buf, "+CPIN:")) != NULL)
        {
            p += (sizeof("+CPIN:")-1);
            if (strstr(p, "NOT READY") != NULL)
            {
                p_GSM_CB->ack.ack_stru.CPIN_NOT_RD = 1;
            }
            else if (strstr(p, "READY") != NULL)
            {
                p_GSM_CB->ack.ack_stru.CPIN_NOT_RD = 0;
            }
        }
        else if ((p=strstr(p_GSM_CB->rec_buf, "+CSQ:")) != NULL)
        {
            p += (sizeof("+CSQ:")-1);
            sscanf(p, "%d,%d", &p_GSM_CB->rssi, &p_GSM_CB->ber);
            p_GSM_CB->ack.ack_stru.SIGNAL = 1;
        }
        else if ((p=strstr(p_GSM_CB->rec_buf, "+CGATT:")) != NULL)
        {
            p += (sizeof("+CGATT:")-1);
            sscanf(p, "%d", &p_GSM_CB->if_attached);
            p_GSM_CB->ack.ack_stru.ATTACH = 1;
        }
        
    //}
    //else if (strstr(p_GSM_CB->rec_buf, "^") != NULL)
    //{
        else if (strstr(p_GSM_CB->rec_buf, "^SYSSTART") != NULL)
        {
            p_GSM_CB->ack.ack_stru.START = 1;
        }
        else if (strstr(p_GSM_CB->rec_buf, "^SHUTDOWN") != NULL)
        {
            p_GSM_CB->ack.ack_stru.SHUTDOWN = 1;
        }
        else if ((p = strstr(p_GSM_CB->rec_buf, "^NWTIME:")) != NULL)
        {
            p += (sizeof("^NWTIME:")-1);
            p = strstr(p, "\"");
            
            sscanf(p, "\"%d/%d/%d,%d:%d:%d+%d,%*d\"",
                    &GSM_time.tm_year,
                    &GSM_time.tm_mon,
                    &GSM_time.tm_mday,
                    &GSM_time.tm_hour,
                    &GSM_time.tm_min,
                    &GSM_time.tm_sec,
                    &temp_int
                    );
            p_GSM_CB->time_zone = temp_int/4;
            GSM_time.tm_year += (2000 - 1900);
            GSM_time.tm_mon -= 1;
            p_GSM_CB->time_UTC = rt_mktime(&GSM_time);
            
            p_GSM_CB->ack.ack_stru.TIME = 1;
        }
        else if ((p = strstr(p_GSM_CB->rec_buf, "^SIS:")) != NULL)
        {
            // TODO: deal with the internet information report.
            p += (sizeof("^SIS:")-1);
            sscanf(p, "%d,%d,%d", &p_GSM_CB->SIS.handle, &p_GSM_CB->SIS.URC, &p_GSM_CB->SIS.error);
            if (p_GSM_CB->SIS.URC == 0)
            {
                if (p_GSM_CB->service[p_GSM_CB->SIS.handle] != NULL)
                {
                    p_GSM_CB->ack.ack_stru.SIS = 1;
//                    if (p_GSM_CB->SIS.error == GPRS_SIS_TCP_REJECT)
//                    {
//                        
//                    }
//                    else if (p_GSM_CB->SIS.error == GPRS_SIS_TCP_DISCONNECT)
//                    {
//                        if (p_GSM_CB->service[p_GSM_CB->SIS.handle]->status == )
//                        {
//                            p_GSM_CB->service[p_GSM_CB->SIS.handle]->status = ;
//                        }
//                    }
                    
                }
            }
            
        }
        else if ((p = strstr(p_GSM_CB->rec_buf, "^SISO:")) != NULL)
        {
            // TODO: parse the internet connection information.
            p += (sizeof("^SISO:")-1);
            p_GSM_CB->local_port = 0;
            
//            sscanf(p, "%d,%*s,%*s,%*s,%*s,%*s,\"%d.%d.%d.%d:%d\",%*s",
//                    &p_GSM_CB->SISO_index,
//                    &p_GSM_CB->local_IP[0],
//                    &p_GSM_CB->local_IP[1],
//                    &p_GSM_CB->local_IP[2],
//                    &p_GSM_CB->local_IP[3],
//                    &p_GSM_CB->local_port
//                    );
//                                    
//            if (p_GSM_CB->local_port)
//            {
//                p_GSM_CB->service[p_GSM_CB->SISO_index]->cfg.local_ip.addr[0] = p_GSM_CB->local_IP[0];
//                p_GSM_CB->service[p_GSM_CB->SISO_index]->cfg.local_ip.addr[1] = p_GSM_CB->local_IP[1];
//                p_GSM_CB->service[p_GSM_CB->SISO_index]->cfg.local_ip.addr[2] = p_GSM_CB->local_IP[2];
//                p_GSM_CB->service[p_GSM_CB->SISO_index]->cfg.local_ip.addr[3] = p_GSM_CB->local_IP[3];
//                p_GSM_CB->service[p_GSM_CB->SISO_index]->cfg.local_ip.port = p_GSM_CB->local_port;
//            }
            
            sscanf(p, "%d",
                    &p_GSM_CB->SISO_index
                    );
            p = strstr(p+1, ",");
            if (p != NULL) 
            {
                p = strstr(p+1, ",");
                if (p != NULL) 
                {
                    p = strstr(p+1, ",");
                    if (p != NULL) 
                    {
                        p = strstr(p+1, ",");
                        if (p != NULL) 
                        {
                            p = strstr(p+1, ",");
                            if (p != NULL) 
                            {
                                p = strstr(p+1, ",");
                                if (p != NULL) 
                                {
                                    p+=1;
                                    
                                    sscanf(p, "\"%d.%d.%d.%d:%d\"",
                                            &p_GSM_CB->local_IP[0],
                                            &p_GSM_CB->local_IP[1],
                                            &p_GSM_CB->local_IP[2],
                                            &p_GSM_CB->local_IP[3],
                                            &p_GSM_CB->local_port
                                            );
                                    
                                    if (p_GSM_CB->local_port)
                                    {
                                        if (p_GSM_CB->service[p_GSM_CB->SISO_index]->report_IP != NULL)
                                        {
                                            p_GSM_CB->service[p_GSM_CB->SISO_index]->report_IP(
                                                        p_GSM_CB->service[p_GSM_CB->SISO_index]->handler, 
                                                        p_GSM_CB->local_IP, 
                                                        p_GSM_CB->local_port, 
                                                        NULL, 
                                                        0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            

        }
        else if (strstr(p_GSM_CB->rec_buf, "^SISW:") != NULL)
        {
            // TODO: deal with the internet connectioned report.
            if (p_GSM_CB->data_sending)
            {
                rt_sem_release(&sem_GSM_rx_data);
            }
        }
        else if ((p = strstr(p_GSM_CB->rec_buf, "^SISR:")) != NULL)
        {
            // TODO: deal with the internet service report.
            p += (sizeof("^SISR:")-1);
            sscanf(p, "%d,%d", &p_GSM_CB->data_handle, &p_GSM_CB->data_len);
            p = strstr(p, ",");
            p++;
            p = strstr(p, ",");
            p++;
            p_GSM_CB->rec_index = p - p_GSM_CB->rec_buf;
            
            p_GSM_CB->ack.ack_stru.DATA_REC = 1;
        }
    //}
    else
    {
        p_GSM_CB->ack.ack_char = 0;
        //p_GSM_CB->ack.ack_stru.UNKNOWN = 1;
    }
    return 0;
}



int GSM_srv_rec_handler(void)
{
    uint8_t *p = NULL;
    uint32_t i = 0;
    
    p_GSM_CB->rec_buf[p_GSM_CB->rec_len] = 0;
    
    SYS_log( SYS_DEBUG_DEBUG, ("Rec : %s", p_GSM_CB->rec_buf));
    

    GSM_srv_ack_parser();
    
    if ((p_GSM_CB->data_sending == 2) && p_GSM_CB->ack.ack_stru.OK)
    {
        p_GSM_CB->data_sending = 3; // OK, 
        p_GSM_CB->ack.ack_stru.OK = 0;
    }
    else if ((p_GSM_CB->data_sending == 2) && p_GSM_CB->ack.ack_stru.ERROR)
    {
        p_GSM_CB->data_sending = 4;
        p_GSM_CB->ack.ack_stru.ERROR = 0;
    }
    
    
    switch (p_GSM_CB->status.sys)
    {
        case GSM_idle:
            
            break;
        case GSM_term_on:
            switch (p_GSM_CB->status.term_on)
            {
                case GSM_term_on_power_on:
                    break;
                case GSM_term_on_power_on_ack:
                    break;
                case GSM_term_on_reset:
                    break;
                case GSM_term_on_ON:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            // Jump to GSM_GPRS_active.
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_0;
                            p_GSM_CB->status.sys = GSM_GPRS_active;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                        else if (p_GSM_CB->ack.ack_stru.ERROR)
                        {
                            
                            p_GSM_CB->ack.ack_stru.ERROR = 0;
                        }
                        
                        
                    }
                    break;
                default:
                    break;
            }
            break;
        case GSM_term_reset:
            p_GSM_CB->status.term_on = GSM_term_on_reset;
            p_GSM_CB->status.sys = GSM_term_on;
            break;
        case GSM_GPRS_active:
            switch (p_GSM_CB->status.gprs_active)
            {
                case GSM_gsm_phase_0_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_1;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GSM_gsm_phase_1_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_2;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GSM_gsm_phase_2_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_3;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GSM_gsm_phase_3_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_4;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GSM_gsm_phase_4_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            if (p_GSM_CB->ack.ack_stru.CPIN_NOT_RD)
                            {
                                p_GSM_CB->status.gprs_active = GSM_gsm_phase_4;
                                p_GSM_CB->ack.ack_stru.CPIN_NOT_RD = 0;
                                p_GSM_CB->ack.ack_stru.ERROR = 0;
                                p_GSM_CB->retry ++;
                                if (p_GSM_CB->retry > 20)
                                {
                                    p_GSM_CB->status.term_off = GSM_term_off_OFF;
                                    p_GSM_CB->status.sys = GSM_term_off;
                                }
                            }
                            else
                            {
                                p_GSM_CB->status.gprs_active = GSM_gsm_phase_5;
                                p_GSM_CB->ack.ack_stru.OK = 0;
                            }
                        }
                        else if (p_GSM_CB->ack.ack_stru.ERROR)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_4;
                            p_GSM_CB->ack.ack_stru.ERROR = 0;
                            p_GSM_CB->retry ++;
                            if (p_GSM_CB->retry > 20)
                            {
                                p_GSM_CB->status.term_off = GSM_term_off_OFF;
                                p_GSM_CB->status.sys = GSM_term_off;
                            }
                        }
                    }
                    break;
                case GSM_gsm_phase_5_ack:
                    
                    if (p_GSM_CB->ack.ack_stru.IMEI_REC == 0)
                    {
                        i = 0;
                        for (p_GSM_CB->rec_index=0;p_GSM_CB->rec_index < p_GSM_CB->rec_len;p_GSM_CB->rec_index++)
                        {
                            if ((p_GSM_CB->rec_buf[p_GSM_CB->rec_index] >= '0') && (p_GSM_CB->rec_buf[p_GSM_CB->rec_index] <= '9'))
                            {
                                p_GSM_CB->IMEI[i] = p_GSM_CB->rec_buf[p_GSM_CB->rec_index];
                                i++;
                                if (i >= GSM_IMEI_LEN)
                                {
                                    break;
                                }
                            }
                        }
                        p_GSM_CB->IMEI[GSM_IMEI_LEN] = 0x00;
                        if (i == GSM_IMEI_LEN)
                        {
                            p_GSM_CB->ack.ack_stru.IMEI_REC = 1;
                        }
                    }   
                    
                
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_6;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                        
                    }
                    break;
                case GSM_gsm_phase_6_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            if (p_GSM_CB->ack.ack_stru.SIGNAL)
                            {
                                if (p_GSM_CB->rssi >= 10)
                                {
                                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_7;
                                }
                                else
                                {
                                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_6;
                                }
                                
                                p_GSM_CB->ack.ack_stru.SIGNAL = 0;
                            }
                            else
                            {
                                p_GSM_CB->status.gprs_active = GSM_gsm_phase_6;
                            }
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                case GSM_gsm_phase_6_retry:
                    break;
                case GSM_gsm_phase_7_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            if (p_GSM_CB->if_attached)
                            {
                                p_GSM_CB->status.gprs_active = GSM_gsm_phase_8;
                            }
                            else
                            {
                                p_GSM_CB->status.gprs_active = GSM_gsm_phase_7;
                            }
                            p_GSM_CB->ack.ack_stru.ATTACH = 0;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                        
                    }
                    break;
                case GSM_gsm_phase_8_ack:
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_9;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    break;
                case GSM_gsm_phase_9_ack:
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->status.gprs_active = GSM_gsm_phase_10;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    break;
                case GSM_gsm_phase_10_ack:
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            // Jump to GSM_GPRS_up.
                            p_GSM_CB->status.gprs_up = GSM_gprs_init;
                            p_GSM_CB->status.sys = GSM_GPRS_up;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    break;
                default:
                    break;
            }
            break;
        case GSM_GPRS_up:
            switch (p_GSM_CB->status.gprs_up)
            {
                case GSM_gprs_init:
                    break;
                case GSM_gprs_idle:
                    GPRS_server_rec_handler();
                    break;
                default:
                    break;
            }
            break;
        case GSM_GPRS_down:
            switch (p_GSM_CB->status.gprs_down)
            {
                case GSM_gprs_close:
                    break;
                case GSM_gprs_close_ack:
                    p_GSM_CB->status.gprs_down = GSM_gprs_closed;
                    break;
                case GSM_gprs_closed:
                    break;
                default:
                    break;
            }
            break;
        case GSM_term_off:
            switch (p_GSM_CB->status.term_off)
            {
                case GSM_term_off_OFF_ack:
//                    if (p_GSM_CB->ack.ack_char)
//                    {
//                        if (p_GSM_CB->ack.ack_stru.SHUTDOWN)
//                        {
//                            p_GSM_CB->status.term_off = GSM_term_off_power_off;
//                            p_GSM_CB->ack.ack_stru.SHUTDOWN = 0;
//                        }
//                        else if (p_GSM_CB->ack.ack_stru.START)
//                        {
//                            p_GSM_CB->status.term_off = GSM_term_off_OFF;
//                            p_GSM_CB->ack.ack_stru.START = 0;
//                        }
//                    }
                    ////p_GSM_CB->status.term_off = GSM_term_off_power_off;
                    break;
                case GSM_term_off_power_off:
                    
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    p_GSM_CB->rec_len = 0;
    return 0;
}


int GSM_srv_report_deal(void)
{
    uint32_t time = 0;
    
    if (p_GSM_CB->ack.ack_stru.TIME)
    {
        // TODO: update the system time.
        SYS_log( SYS_DEBUG_DEBUG, ("TIME: %u\n", p_GSM_CB->time_UTC));
        time = p_GSM_CB->time_UTC + p_GSM_CB->time_zone*60*60;
        set_time_date(&time);
        list_date();
        
        p_GSM_CB->ack.ack_stru.TIME = 0;
    }
    else if (p_GSM_CB->ack.ack_stru.IMEI_REC)
    {
        // TODO: Store the IMEI.
        SYS_log( SYS_DEBUG_INFO, ("IMEI: %s\n", p_GSM_CB->IMEI));
        p_GSM_CB->ack.ack_stru.IMEI_REC = 0;
    }


}

int GSM_srv_service(void)
{
    static uint8_t test_char = 1;
    
    
    GSM_srv_report_deal();
    
    
    switch (p_GSM_CB->status.sys)
    {
        case GSM_idle:
//            // Song: just for testing.
//            if (test_char)
//            {
//                GSM_srv_power_on();
//                test_char = 0;
//            }
            break;
        case GSM_term_on:
            switch (p_GSM_CB->status.term_on)
            {
                case GSM_term_on_power_on:
                
                    // Song:　Initialize the GSM power control.
                    if (sys_config.mother_type != mother_type_GPRS_1_2)
                    {
                        rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_ON);
                        rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    }
//#if USE_GSM_PWR_SHUT    
//                    rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_ON);
//                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
//#endif // USE_GSM_PWR_SHUT    
               
                    rt_pin_mode(PIN_GSM_ON_OFF      , PIN_MODE_OUTPUT);
                    rt_pin_mode(PIN_GSM_RST         , PIN_MODE_OUTPUT);
                
                    rt_pin_write(PIN_GSM_RST        , GSM_RST_DIS);     // default is not reset.
                
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
                    rt_thread_delay(RT_TICK_PER_SECOND / 2);
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_PUSH);
                    rt_thread_delay(RT_TICK_PER_SECOND * 2);
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
                    //rt_thread_delay(RT_TICK_PER_SECOND / 1);
                
                    p_GSM_CB->status.term_on = GSM_term_on_power_on_ack;
                    break;
                case GSM_term_on_power_on_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.SHUTDOWN)
                        {
                            p_GSM_CB->status.term_on = GSM_term_on_power_on;
                            p_GSM_CB->ack.ack_stru.SHUTDOWN = 0;
                        }
                        else if (p_GSM_CB->ack.ack_stru.START)
                        {
                            p_GSM_CB->status.term_on = GSM_term_on_reset;
                            p_GSM_CB->ack.ack_stru.START = 0;
                        }
                    }
                    
                    if (sys_config.mother_type == mother_type_GPRS_1_2)
                    {
                        rt_device_write(p_GSM_CB->dev, 0, GSM_AT_DUMMY, sizeof(GSM_AT_DUMMY)-1);
                    }
                    
//                    #if PATCH_GSM_SISW_EN
//                    rt_device_write(p_GSM_CB->dev, 0, GSM_AT_DUMMY, sizeof(GSM_AT_DUMMY)-1);
//                    #endif // PATCH_GSM_SISW_EN
                    break;
                case GSM_term_on_reset:
//                    rt_pin_write(PIN_GSM_RST    , GSM_RST_EN); 
//                    rt_thread_delay(RT_TICK_PER_SECOND / 10);
//                    rt_pin_write(PIN_GSM_RST    , GSM_RST_DIS); 
//                    rt_thread_delay(RT_TICK_PER_SECOND / 1);
                
                    p_GSM_CB->status.term_on = GSM_term_on_ON;
                    break;
                case GSM_term_on_ON:
                    // Jump to GSM_GPRS_active
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_0;
                    p_GSM_CB->status.sys = GSM_GPRS_active;
                    break;
                default:
                    break;
            }
            break;
        case GSM_term_reset:
            p_GSM_CB->status.term_on = GSM_term_on_reset;
            p_GSM_CB->status.sys = GSM_term_on;
            break;
        case GSM_GPRS_active:
            switch (p_GSM_CB->status.gprs_active)
            {
                case GSM_gsm_phase_0:
                    GSM_srv_AT_AT();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_0_ack;
                    break;
                case GSM_gsm_phase_1:
                    GSM_srv_ATE();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_1_ack;
                    break;
                case GSM_gsm_phase_2:
                    GSM_srv_ATI();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_2_ack;
                    break;
                case GSM_gsm_phase_3:
                    GSM_srv_AT_SWSPATH();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_3_ack;
                    break;
                case GSM_gsm_phase_4:
                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    GSM_srv_AT_CPIN();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_4_ack;
                    break;
                case GSM_gsm_phase_5:
                    GSM_srv_AT_CGSN();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_5_ack;
                    break;
                case GSM_gsm_phase_6:
                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    GSM_srv_AT_CSQ();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_6_ack;
                    break;
                case GSM_gsm_phase_6_retry:
                    //rt_thread_delay(RT_TICK_PER_SECOND * 3);
                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_6;
                    break;
                case GSM_gsm_phase_7:
                    rt_thread_delay(RT_TICK_PER_SECOND * 1);
                    GSM_srv_AT_CGATT();
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_7_ack;
                    break;
                case GSM_gsm_phase_8:
                    GSM_srv_AT_SICS(0);
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_8_ack;
                    break;
                case GSM_gsm_phase_9: 
                    GSM_srv_AT_SICS_APN(0, GSM_GPRS_APN);
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_9_ack;
                    break;
                case GSM_gsm_phase_10:
                    GSM_srv_AT_IOMODE(0, 1);
                    p_GSM_CB->status.gprs_active = GSM_gsm_phase_10_ack;
                    break;
                default:
                    break;
            }
            break;
        case GSM_GPRS_up:
            switch (p_GSM_CB->status.gprs_up)
            {
                case GSM_gprs_init:
                    p_GSM_CB->status.gprs_up = GSM_gprs_idle;
                    rt_sem_release(&sem_GSM_up);
                
                    break;
                case GSM_gprs_idle:
                    GPRS_server_handler();
                    
                    break;
                default:
                    break;
            }
            break;
        case GSM_GPRS_down:
            switch (p_GSM_CB->status.gprs_down)
            {
                case GSM_gprs_close:
                    p_GSM_CB->status.gprs_down = GSM_gprs_close_ack;
                    
                    break;
                case GSM_gprs_closed:
                    rt_sem_release(&sem_GSM_down);
                    p_GSM_CB->status.term_off = GSM_term_off_OFF;
                    p_GSM_CB->status.sys = GSM_term_off;
                    break;
                default:
                    break;
            }
            break;
        case GSM_term_off:
            switch (p_GSM_CB->status.term_off)
            {
                case GSM_term_off_OFF:
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
                    rt_thread_delay(RT_TICK_PER_SECOND / 2);
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_PUSH);
                    rt_thread_delay(RT_TICK_PER_SECOND * 2);
                    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
                    rt_thread_delay(RT_TICK_PER_SECOND / 1);
                
                    p_GSM_CB->status.term_off = GSM_term_off_OFF_ack;
                    break;
                case GSM_term_off_OFF_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.SHUTDOWN)
                        {
                            p_GSM_CB->status.term_off = GSM_term_off_power_off;
                            p_GSM_CB->ack.ack_stru.SHUTDOWN = 0;
                        }
                        else if (p_GSM_CB->ack.ack_stru.START)
                        {
                            p_GSM_CB->status.term_off = GSM_term_off_OFF;
                            p_GSM_CB->ack.ack_stru.START = 0;
                        }
                    }
                    ////p_GSM_CB->status.term_off = GSM_term_off_power_off;
                    break;
                case GSM_term_off_power_off:
                    if (sys_config.mother_type != mother_type_GPRS_1_2)
                    {
                        rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_OFF);
                    }
//#if USE_GSM_PWR_SHUT    
//                    rt_pin_write(PIN_GSM_PWR_SHUT, GSM_PWR_OFF);
//#endif // USE_GSM_PWR_SHUT    

                    SYS_log( SYS_DEBUG_INFO, ("GSM module power off !\n"));
                
                    // Jump to GSM_GPRS_active
                    p_GSM_CB->status.sys = GSM_idle;
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

s_GPRS_service* GPRS_srv_create(s_GPRS_srv_cfg *cfg, void *handler)
{
    s_GPRS_service *gprs = NULL;
    
    gprs = rt_malloc(sizeof(s_GPRS_service));

    if (gprs == NULL)
    {
        return NULL;
    }
    else
    {
        rt_memset(gprs, 0, sizeof(s_GPRS_service));
        
        rt_memcpy(&gprs->cfg, cfg, sizeof(s_GPRS_srv_cfg));
        gprs->status = GPRS_idle;
        gprs->handler = handler;
        return gprs;
    }
    
}

int GPRS_srv_attach(s_GPRS_service *gprs)
{
    int i = 0;
    
    for (i=0;i<GSM_GPRS_SERVICE_MAX;i++)
    {
        if (p_GSM_CB->service[i] == NULL)
        {
            p_GSM_CB->service[i] = gprs;
            return i;
        }
    }

    return -1;
}

void *GPRS_srv_get(int handle)
{
    return p_GSM_CB->service[handle];
}

int GPRS_srv_release(int handle)
{
    IF_NOT_GSM_CONN_NUM(handle);

    p_GSM_CB->service[handle] = NULL;
    return 0;
    
}

int GPRS_srv_write(int handle, uint8_t *data, uint32_t len)
{
    IF_NOT_GSM_CONN_NUM(handle);
    
    return GSM_srv_AT_SISW_data(handle, data, len);
}


void GPRS_srv_destroy(int handle)
{
    if ((handle >= 0) && (handle < GSM_GPRS_SERVICE_MAX))
    {
        rt_free(p_GSM_CB->service[handle]);
    }
}

int GPRS_connect(void * handler ,
                s_GPRS_srv_cfg *cfg,
                pFun_GPRS_connect_fail connect_fail,
                pFun_GPRS_connected connected,
                pFun_GPRS_disconnected  disconnected,
                pFun_GPRS_received  received,
                pFun_GPRS_rejected  rejected,
                pFun_GPRS_closed    closed,
                pFun_GPRS_report_IP report_IP
                )
{
    int res = 0;
    s_GPRS_service *gprs_service = NULL;
    
    gprs_service = GPRS_srv_create(cfg, handler);
    if (gprs_service == NULL) return -1;
    
    res = GPRS_srv_attach(gprs_service);
    if (res < 0) return -2;
    
    if (cfg->srv_type == GPRS_srv_UDP_client)
    {
        gprs_service->status = GPRS_SISS_type;
    }
    else if (cfg->srv_type == GPRS_srv_TCP_client)
    {
        gprs_service->status = GPRS_SISS_type;
    }
    
    gprs_service->connect_fail = connect_fail;
    gprs_service->connected = connected;
    gprs_service->disconnected = disconnected;
    gprs_service->received = received;
    gprs_service->rejected = rejected;
    gprs_service->closed = closed;
    gprs_service->report_IP = report_IP;
    

    return res;
}

int GPRS_disconnect(int handle)
{

    p_GSM_CB->service[handle]->status = GPRS_release;
    
}

int GPRS_listen(s_GPRS_srv_cfg *cfg,
                pFun_GPRS_accepted accepted,
                pFun_GPRS_disconnected  disconnected,
                pFun_GPRS_received  received,
                pFun_GPRS_closed    closed
                )
{
    int res = 0;
    s_GPRS_service *gprs_service = NULL;
    
    gprs_service = GPRS_srv_create(cfg, NULL);
    if (gprs_service == NULL) return -1;
    
    res = GPRS_srv_attach(gprs_service);
    if (res < 0) return -2;
    
    if (cfg->srv_type == GPRS_srv_UDP_server)
    {
        gprs_service->status = GPRS_SISS_type_listen;
    }
    else if (cfg->srv_type == GPRS_srv_TCP_server)
    {
        gprs_service->status = GPRS_SISS_type_listen;
    }
    
    gprs_service->accepted = accepted;
    gprs_service->disconnected = disconnected;
    gprs_service->received = received;
    gprs_service->closed = closed;

    return res;
}



int GPRS_server_rec_handler(void)
{
    int i = 0;
    
    for (i=0;i<GSM_GPRS_SERVICE_MAX;i++)
    {
        if (p_GSM_CB->service[i] != NULL)
        {
            switch (p_GSM_CB->service[i]->status)
            {
                case GPRS_idle:
                    
                    break;
                case GPRS_SISS_type_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_SISS_addr;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GPRS_SISS_addr_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_SISO;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GPRS_SISO_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_SISO_INFO;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                        else if (p_GSM_CB->ack.ack_stru.ERROR)
                        {
                            if (p_GSM_CB->ack.ack_stru.SIS)
                            {
                                // TODO: check the fail information.
                                if ((p_GSM_CB->SIS.handle == i) && 
                                    (p_GSM_CB->SIS.URC == 0) && 
                                    (p_GSM_CB->SIS.error == GPRS_SIS_TCP_REJECT))
                                {
                                    if (p_GSM_CB->service[i]->rejected != NULL)
                                    {
                                        p_GSM_CB->service[i]->rejected(NULL);
                                    }
                                }
                                p_GSM_CB->ack.ack_stru.SIS = 0;
                            }
                        
                            p_GSM_CB->service[i]->status = GPRS_discon;
                            p_GSM_CB->ack.ack_stru.ERROR = 0;
                        }
                    }
                    break;
                case GPRS_SISO_INFO:
                    break;
                case GPRS_SISO_INFO_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_attached;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GPRS_SISS_type_listen:
                    break;
                case GPRS_SISS_type_listen_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_SISS_listen;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GPRS_SISS_listen:
                    break;
                case GPRS_SISS_listen_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_SISO;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    break;
                case GPRS_attached:
                    break;
                case GPRS_up:
                    if (p_GSM_CB->ack.ack_stru.DATA_REC)
                    {
                        if (p_GSM_CB->data_handle == i)
                        {
                            if (p_GSM_CB->service[i]->received != NULL)
                            {
                                p_GSM_CB->service[i]->received(p_GSM_CB->service[i]->handler, &p_GSM_CB->rec_buf[p_GSM_CB->rec_index], p_GSM_CB->data_len);
                            }
                            p_GSM_CB->ack.ack_stru.DATA_REC = 0;
                        }                            
                    }
                    if (p_GSM_CB->ack.ack_stru.SIS)
                    {
                        if ((p_GSM_CB->SIS.handle == i) && 
                            (p_GSM_CB->SIS.URC == 0) && 
                            (p_GSM_CB->SIS.error == GPRS_SIS_TCP_DISCONNECT))
                        {
                            if (p_GSM_CB->service[i]->closed != NULL)
                            {
                                p_GSM_CB->service[i]->closed(NULL);
                            }
                            
                            //p_GSM_CB->service[i]->status = GPRS_discon;
                            p_GSM_CB->ack.ack_stru.SIS = 0;
                            
                            // Song: TODO: when server cut off the connection. need to deal with.
                            SYS_log(SYS_DEBUG_ERROR,("Server cut off the connection !!! Restart the system.\n "));
                            sys_reboot();
                            while(1) rt_thread_delay(5);
                            
                        }
                    }
                    break;
                case GPRS_release_ack:
                    if (p_GSM_CB->ack.ack_char)
                    {
                        if (p_GSM_CB->ack.ack_stru.OK)
                        {
                            p_GSM_CB->service[i]->status = GPRS_discon;
                            p_GSM_CB->ack.ack_stru.OK = 0;
                        }
                    }
                    //p_GSM_CB->service[i]->status = GPRS_discon;
                    break;
                case GPRS_discon:
                    break;
                default:
                    break;
            }
        }
    }
    
}

int GPRS_server_handler(void)
{
    int i = 0;
    
    for (i=0;i<GSM_GPRS_SERVICE_MAX;i++)
    {
        if (p_GSM_CB->service[i] != NULL)
        {
            switch (p_GSM_CB->service[i]->status)
            {
                case GPRS_idle:
                    
                    break;
                case GPRS_SISS_type:
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    GSM_srv_AT_SISS_TYPE(i);
                    p_GSM_CB->service[i]->status = GPRS_SISS_type_ack;
                    break;
                case GPRS_SISS_addr:
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    if (p_GSM_CB->service[i]->cfg.if_use_name == 0)
                    {
                        GSM_srv_AT_SISS_ip(i, p_GSM_CB->service[i]->cfg.srv_type, &p_GSM_CB->service[i]->cfg.srv_ip);
                    }
                    else
                    {
                        GSM_srv_AT_SISS_name(i, p_GSM_CB->service[i]->cfg.srv_type, &p_GSM_CB->service[i]->cfg.svr_name);
                    }
                    p_GSM_CB->service[i]->status = GPRS_SISS_addr_ack;
                    break;
                case GPRS_SISO:
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    GSM_srv_AT_SISO(i);
                    p_GSM_CB->service[i]->status = GPRS_SISO_ack;
                    break;
                case GPRS_SISO_INFO:
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    GSM_srv_AT_SISO_info();
                    p_GSM_CB->service[i]->status = GPRS_SISO_INFO_ack;
                    break;
                case GPRS_SISO_INFO_ack:
                    break;
                case GPRS_SISS_type_listen:
                    GSM_srv_AT_SISS_TYPE(i);
                    p_GSM_CB->service[i]->status = GPRS_SISS_type_listen_ack;
                    break;
                case GPRS_SISS_type_listen_ack:
                    break;
                case GPRS_SISS_listen:
                    GSM_srv_AT_SISS_listen(i, p_GSM_CB->service[i]->cfg.srv_type, p_GSM_CB->service[i]->cfg.listen_port);
                    p_GSM_CB->service[i]->status = GPRS_SISS_listen_ack;
                    break;
                case GPRS_SISS_listen_ack:
                    break;
                case GPRS_attached:
                    if (p_GSM_CB->service[i]->connected != NULL)
                    {
                        p_GSM_CB->service[i]->connected(&i);
                    }
                    p_GSM_CB->service[i]->status = GPRS_up;
                    break;
                case GPRS_up:
                    // GPRS_up
                    if (((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested)) && (sys_config.test_flag.GSM == 0))
                    {
                        sys_config.test_flag.GSM = 1;
                        while (1) rt_thread_delay(5); // infinite loop 
                    }
                    
                    break;
                case GPRS_release:
                    GSM_srv_AT_SISC(i);
                    p_GSM_CB->service[i]->status = GPRS_release_ack;
                    break;
                case GPRS_discon:
                    if (p_GSM_CB->service[i]->disconnected != NULL)
                    {
                        p_GSM_CB->service[i]->disconnected(p_GSM_CB->service[i]->handler, NULL, i);
                    }
                    GPRS_srv_destroy(i);
                    GPRS_srv_release(i);
                    break;
                default:
                    break;
            }
        }
    }
    
    
}




////////////////////////////////////////////////////////////////////////////////////////////////////


rt_err_t GSM_MG301_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_GSM_rx);    
    
    //rt_snprintf();

}

rt_err_t GSM_MG301_tx_complete(rt_device_t dev, void *buffer)
{

////    rt_sem_release(&sem_GSM_tx);

}

rt_err_t GSM_rx_check(void)
{
    
    uint8_t c = 0x00;
    int res = 0;
   
    
    
    if (GSM_srv_phaser() == 0)
    {
        res = GSM_srv_rec_handler();
        if (res)
        {
            
        }
    }
    
    
    return 0;
}



void rt_thread_entry_GSM_rx(void* parameter)
{
    uint32_t e;
    int res = 0;

    //rt_thread_t A8_if_tid;
    
    
    rt_sem_init(&sem_GSM_rx, "GSM_rx", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_GSM_rx_data, "GSM_rx_data", 0, RT_IPC_FLAG_FIFO);

    
    while(GSM_srv_inited == 0)
    {
        rt_thread_delay(1);
    }
   
    while(1)
    {
        
        if (rt_sem_take(&sem_GSM_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            GSM_rx_check();
        }
        else
        {
        
        }
        
//        // If sem is deleted already, return RT_ERROR. We need thread_delay for thread_idle to kill the dead thread !
//        if (rt_sem_take(&sem_GSM_rx, RT_WAITING_FOREVER) == -RT_ERROR)
//        {
//            SYS_log( SYS_DEBUG_INFO, ("Sem_take  GSM_rx  failed !\n"));
//            rt_thread_delay(5);  
//            SYS_log( SYS_DEBUG_INFO, ("Sem_take  GSM_rx  over !\n"));

//        }
//        else
//        {
//            GSM_rx_check();
//        }
    }
}



void rt_thread_entry_GSM_MG301(void* parameter)
{

//    rt_sem_init(&sem_GSM_MG301_tx, "GSM_MG301_tx", 0, RT_IPC_FLAG_FIFO);
//    rt_sem_init(&sem_GSM_MG301_rx, "GSM_MG301_rx", 0, RT_IPC_FLAG_FIFO);

    rt_sem_init(&sem_GSM_up, "GSM_up", 0, RT_IPC_FLAG_FIFO);
    rt_sem_init(&sem_GSM_down, "GSM_down", 0, RT_IPC_FLAG_FIFO);
    rt_mutex_init(&mt_GSM_send, "GSM_send", RT_IPC_FLAG_FIFO);
 

    while(1)
    {
        if (sys_config.sys_inited_flag) break;
        rt_thread_delay(5);
    }

    
    device_GSM_MG301 = rt_device_find(UITD_UART_GSM);
    if(device_GSM_MG301 == RT_NULL)
    {
        SYS_log( SYS_DEBUG_WARNING, ("Serial device %s not found!\r\n", UITD_UART_GSM));
    }
    else
    {
		rt_device_open(device_GSM_MG301, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(device_GSM_MG301, GSM_MG301_rx_ind);
        rt_device_set_tx_complete(device_GSM_MG301, GSM_MG301_tx_complete);

    }


    
    if (GSM_srv_init(device_GSM_MG301) < 0)
    {
        SYS_log(SYS_DEBUG_ERROR, ("GSM callback malloc failed! \n"));
        while(1) rt_thread_delay(10);
    }

    GSM_srv_inited = 1;
    
    GSM_srv_power_on();

    ////GSM_cmd_init(0, GSM_SRV_UDP_SONG);

    while(1)
    {
        ////rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT, sizeof(GSM_CMD_AT) - 1);
        ////rt_thread_delay(RT_TICK_PER_SECOND / 1);
        
        GSM_srv_service();
        rt_thread_delay(1);
    }
    
}


#ifdef RT_USING_FINSH
#include <finsh.h>



void GSM_cmd_init(uint8_t if_tcp, uint8_t *IP_port)
{
    uint8_t cmd_buf[64] = {0};

    // Song:　Initialize the GSM power control.
    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_PUSH);
    rt_thread_delay(RT_TICK_PER_SECOND * 2);
    rt_pin_write(PIN_GSM_ON_OFF, GSM_PIN_PWR_POP);
    rt_thread_delay(RT_TICK_PER_SECOND * 1);

    
    // AT
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT, sizeof(GSM_CMD_AT) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
 
    // ATE0
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_ATE, sizeof(GSM_CMD_ATE) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
    
    // ATI
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_ATI, sizeof(GSM_CMD_ATI) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);

    // AT^SWSPATH=1  Use the 2nd audio channel.
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_SWSPATH, sizeof(GSM_CMD_AT_SWSPATH) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);


    // AT+CPIN?
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_CPIN, sizeof(GSM_CMD_AT_CPIN) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
    
    // AT+CGSN
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_CGSN, sizeof(GSM_CMD_AT_CGSN) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
    
    
    
    // AT+CSQ
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_CSQ, sizeof(GSM_CMD_AT_CSQ) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 2);
    

//    // ATD*99#
//    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_COPS, sizeof(GSM_CMD_AT_COPS) - 1);
//    rt_thread_delay(RT_TICK_PER_SECOND / 2);


    // ATD+CGDCONT
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_CGATT, sizeof(GSM_CMD_AT_CGATT) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 1);
    
    // AT%ETCPIP 
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_SICS, sizeof(GSM_CMD_AT_SICS) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 1);
    
    // AT%ETCPIP 
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_SISS_TYPE, sizeof(GSM_CMD_AT_SISS_TYPE) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 1);

    if (if_tcp == 0)
    {
        // AT^SISS Address
        sprintf(cmd_buf, "%s%s\n", GSM_CMD_AT_SISS_ADDR_UDP, IP_port);
        rt_device_write(device_GSM_MG301, 0, cmd_buf, strlen(cmd_buf));
        rt_thread_delay(RT_TICK_PER_SECOND * 2);
    
    }
    else
    {
        // AT^SISS Address
        sprintf(cmd_buf, "%s%s\n", GSM_CMD_AT_SISS_ADDR_TCP, IP_port);
        
        rt_device_write(device_GSM_MG301, 0, cmd_buf, strlen(cmd_buf));
        rt_thread_delay(RT_TICK_PER_SECOND * 2);
    }
    
    SYS_log( SYS_DEBUG_INFO, ("SISS: %s", cmd_buf));

    // AT^IOMODE=1,1 
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_IOMODE, sizeof(GSM_CMD_AT_IOMODE) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 1);
    
    
    // AT+IPOPEN 
    rt_device_write(device_GSM_MG301, 0, GSM_CMD_AT_SISO, sizeof(GSM_CMD_AT_SISO) - 1);
    rt_thread_delay(RT_TICK_PER_SECOND * 2);


}
FINSH_FUNCTION_EXPORT_ALIAS(GSM_cmd_init, GSM_init, Init the GSM module : if_tcp IP_port.);

void GSM_send(char *cmd)
{
    
    if (device_GSM_MG301 != RT_NULL)
    {
        rt_device_write(device_GSM_MG301, 0, cmd, strlen(cmd));
    }
    else
    {
    
    }
    
}
FINSH_FUNCTION_EXPORT_ALIAS(GSM_send, GSM_send, Send the GSM AT command.);

void GSM_socket_send(char *cmd)
{
    //uint32_t len_cmd = 0;
    uint32_t len_data = 0;
    uint8_t AT_SISW[32] = {0};
    
    len_data = strlen(cmd);
    
    if (device_GSM_MG301 != RT_NULL)
    {
        //memcpy(AT_SISW, GSM_CMD_AT_SISW, len_cmd);
        sprintf(AT_SISW, "%s%d\n\r", GSM_CMD_AT_SISW, len_data);
        
        rt_device_write(device_GSM_MG301, 0, AT_SISW, strlen(AT_SISW));
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
        rt_device_write(device_GSM_MG301, 0, cmd, len_data);
    }
    else
    {
    
    }
    
}
FINSH_FUNCTION_EXPORT_ALIAS(GSM_socket_send, GSM_socket, Send data by socket.);

void GSM_socket_read(uint32_t num)
{
    //uint32_t len_cmd = 0;
    uint8_t AT_SISR[32] = {0};
    
    if (num == 0)
    {
        num = 1500;
    }
    
    if (device_GSM_MG301 != RT_NULL)
    {
        //memcpy(AT_SISW, GSM_CMD_AT_SISW, len_cmd);
        sprintf(AT_SISR, "AT^SISR=0,%d\n", num);
        
        rt_device_write(device_GSM_MG301, 0, AT_SISR, strlen(AT_SISR));
    }
    else
    {
    
    }
    
}
FINSH_FUNCTION_EXPORT_ALIAS(GSM_socket_read, GSM_read, Read data by socket: num.);

//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_SZJS_connect, SZJS_connect, Connect to the SZJS server.);
//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_SZJS_disconnect, SZJS_disconnect, Disconnect to the SZJS server.);
//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_SZJS_write, SZJS_write, Write to the SZJS server.);

//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_XFZD_connect, XFZD_connect, Connect to the XFZD server.);
//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_XFZD_disconnect, XFZD_disconnect, Disconnect to the XFZD server.);
//FINSH_FUNCTION_EXPORT_ALIAS(GPRS_XFZD_write, XFZD_write, Write to the XFZD server.);
FINSH_FUNCTION_EXPORT_ALIAS(GSM_srv_gprs_down, GPRS_down, Set the GPRS down.);
FINSH_FUNCTION_EXPORT_ALIAS(GSM_srv_power_on, GSM_power_on, Power the GSM module on.);
FINSH_FUNCTION_EXPORT_ALIAS(GSM_srv_shutdown, GSM_shut, Shut the GSM module down.);
FINSH_FUNCTION_EXPORT_ALIAS(GSM_srv_gprs_down_force, GPRS_shut_force, Shut the GPRS down by force.);


#endif // RT_USING_FINSH

















