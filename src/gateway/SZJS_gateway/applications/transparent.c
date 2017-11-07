#include "transparent.h"


struct rt_semaphore sem_TP_rx;
struct rt_device * device_transparent;

uint8_t transparent_inited = 0;

struct rt_mutex mt_TP_ringbuf;


rt_uint8_t prio_transparent = 10;
rt_thread_t thread_transparent;

rt_uint8_t prio_transparent_rx = 4;
rt_thread_t thread_transparent_rx;




volatile s_transparent_cb *p_transparent_cb = NULL;

s_transparent_cfg   transparent_cfg = {0};

rt_err_t transparent_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    //rt_sem_release(&sem_TP_rx);    
}

rt_err_t transparent_tx_complete(rt_device_t dev, void *buffer)
{


}


int transparent_config(s_transparent_cb *cb)
{
    struct serial_configure serial_cfg = {0};
    
    serial_cfg.baud_rate = cb->cfg.cfg.baudrate;
    serial_cfg.data_bits = cb->cfg.cfg.bits;
    serial_cfg.parity = cb->cfg.cfg.verify;
    serial_cfg.stop_bits = cb->cfg.cfg.stopbit;
    
    serial_cfg.bit_order = BIT_ORDER_LSB;
    serial_cfg.bufsz = UITD_USART_COM_BUS_SIZE;
    serial_cfg.invert = NRZ_NORMAL;
    serial_cfg.reserved = 0;
    
    rt_device_control(cb->dev, RT_DEVICE_CTRL_CONFIG, &serial_cfg);
    
    return 0;
}


int transparent_init(s_transparent_cb *cb, s_transparent_cfg *cfg)
{

    rt_memset(cb, 0x00, sizeof(s_transparent_cb));
    
    rt_memcpy(&cb->cfg, cfg, sizeof(s_transparent_cfg));
    
    
    if ((strlen(cb->cfg.uart) < 5) || (rt_memcmp(cb->cfg.uart, "uart", 4) != 0))
    {
        rt_memcpy(cb->cfg.uart, UITD_UART_TRANSPARENT, sizeof(UITD_UART_TRANSPARENT));
    }
    
    if (cb->cfg.cfg.baudrate == 0)
    {
        cb->cfg.cfg.baudrate = TP_CFG_BAUDRATE_DEFAULT;
    }
    
    if (cb->cfg.buffer_period < TP_CFG_BUF_PERIOD_MIN) 
    {
        cb->cfg.buffer_period = TP_CFG_BUF_PERIOD_DEF;
    }

    if (cb->cfg.trans_period < TP_CFG_TRANS_PERIOD_MIN) 
    {
        cb->cfg.trans_period = TP_CFG_TRANS_PERIOD_DEF;
    }
    
    cb->dev = rt_device_find(cb->cfg.uart);
    if(cb->dev == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Serial device %s not found!\r\n", cb->cfg.uart));
        while(1) {rt_thread_delay(10);}
    }
    else
    {
		rt_device_open(cb->dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(cb->dev, transparent_rx_ind);
        rt_device_set_tx_complete(cb->dev, transparent_tx_complete);
    }       
    
    transparent_config(cb);
    
    cb->rec_buf_size = (cb->cfg.trans_period*cb->cfg.cfg.baudrate*3)/2/8/1000;
    cb->rec_buf = rt_malloc(cb->rec_buf_size);
    if (cb->rec_buf == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc transparent rec_buf failed!\r\n"));
        while(1) {rt_thread_delay(10);}
    }

    cb->ring_buf_buf_size = (cb->cfg.buffer_period*cb->cfg.cfg.baudrate)/8/1000;
    cb->ring_buf_buf = rt_malloc(cb->ring_buf_buf_size);
    if (cb->rec_buf == RT_NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Malloc transparent ring_buf_buf failed!\r\n"));
        while(1) {rt_thread_delay(10);}
    }
    
    cb->status = TP_idle;
    cb->rec_len = 0;
    cb->TS = 0;
    cb->seg_index = 0;
    cb->seg_total = 0;
    
    cb->counter = 0;
    
    rt_ringbuffer_init(&cb->ring_buf, cb->ring_buf_buf, cb->ring_buf_buf_size);
    
    
    return 0;
}

int transparent_destroy(s_transparent_cb *cb)
{
    
    if (cb->rec_buf)
    {
        rt_free(cb->rec_buf);
    }
    
    if (cb->ring_buf_buf)
    {
        rt_free(cb->ring_buf_buf);
    }
    
    rt_free(cb);
    
    return 0;
}


int transparent_server_send_pkg(s_transparent_cb *cb)
{
    uint32_t TS = 0;
    uint32_t len = 0;
    int res = 0;

    if (rt_ringbuffer_data_len(&cb->ring_buf) > (sizeof(cb->TS)+sizeof(cb->rec_len)))
    {
        if (rt_mutex_take(&mt_TP_ringbuf, 10) == RT_EOK)
        {
            rt_ringbuffer_get(&cb->ring_buf, (uint8_t *)&TS, sizeof(cb->TS));
            rt_ringbuffer_get(&cb->ring_buf, (uint8_t *)&len, sizeof(cb->rec_len));
            
            UITD_trans_time(&cb->trans_data.timestamp, TS);
            cb->trans_data.index = 0;
            cb->trans_data.total = len%sizeof(cb->trans_data.data) ? (len/sizeof(cb->trans_data.data) + 1):(len/sizeof(cb->trans_data.data));
            
            for (;cb->trans_data.index < cb->trans_data.total; cb->trans_data.index++)
            {
                if (cb->trans_data.index == (cb->trans_data.total-1))
                {
                    cb->trans_data.len = len - sizeof(cb->trans_data.data)*(cb->trans_data.total-1);
                }
                else
                {
                    cb->trans_data.len = sizeof(cb->trans_data.data);
                }
                
                rt_ringbuffer_get(&cb->ring_buf, cb->trans_data.data, cb->trans_data.len);
                if (cb->handler) 
                {
                    res = UITD_transparent_send_data(cb->handler, &cb->trans_data);
                    if (res < 0)
                    {
                        SYS_log(SYS_DEBUG_ERROR, ("Transparent send data failed !\n"));
                    }
                }
                if (cb->handler_2) 
                {
                    res = UITD_transparent_send_data(cb->handler_2, &cb->trans_data);
                    if (res < 0)
                    {
                        SYS_log(SYS_DEBUG_ERROR, ("Transparent send data failed !\n"));
                    }
                }
                
            }
            
            rt_mutex_release(&mt_TP_ringbuf);
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("TP ringbuf mutex can not take !\n"));
        }
    }


    return 0;
}


int transparent_server(s_transparent_cb *cb)
{
    int res = 0;
    
    //while(1)
    {
        switch(cb->status)
        {
            case TP_idle:
                break;
            case TP_init:
                cb->status = TP_inited;
                break;
            case TP_inited:
                cb->status = TP_trans;
                break;
            case TP_trans:
                
                transparent_server_send_pkg(cb);
                break;
            case TP_stop:
                break;
            default:
                break;
        }
    
    }
    
    

}

int transparent_rx_parse(s_transparent_cb *cb)
{
    int res = 0;
    uint8_t data_temp = 0x00;

    while(1)
    {
        
        res = rt_device_read(cb->dev, 0, &data_temp, 1);
        if (res < 1)
        {
            break;
        }
        cb->rec_buf[cb->rec_len] = data_temp;
        cb->rec_len ++;

        if (cb->rec_len >= cb->rec_buf_size)
        {
            SYS_log(SYS_DEBUG_ERROR, ("Transparent rec_buf overflow ! \n"));
            
            cb->rec_len = 0;
            return -2;
        }
        
        
    }
    
    if (cb->tick_flag)
    {
        if ((cb->status == TP_trans) && (cb->rec_len > 0))
        {
            rt_time(&cb->TS);
            
            if (rt_mutex_take(&mt_TP_ringbuf, 10) == RT_EOK)
            {
                rt_ringbuffer_put(&cb->ring_buf, (uint8_t *)&cb->TS, sizeof(cb->TS));
                rt_ringbuffer_put(&cb->ring_buf, (uint8_t *)&cb->rec_len, sizeof(cb->rec_len));
                rt_ringbuffer_put(&cb->ring_buf, cb->rec_buf, cb->rec_len);
                rt_mutex_release(&mt_TP_ringbuf);
            }
            else
            {
                SYS_log(SYS_DEBUG_ERROR, ("TP ringbuf mutex can not take !\n"));
            }
        }
        cb->rec_len = 0;
        
        cb->tick_flag = 0;
    }
    
    return -1;
}




void rt_thread_entry_transparent(void* parameter)
{

    //rt_sem_init(&sem_TP_rx, "TP_rx", 0, RT_IPC_FLAG_FIFO);
    
    rt_mutex_init(&mt_TP_ringbuf, "mt_TP_ringbuf", RT_IPC_FLAG_FIFO);
    
    p_transparent_cb = rt_malloc(sizeof(s_transparent_cb));
    if (p_transparent_cb == NULL)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Transparent_cb malloc failed ! \n"));
        while(1) rt_thread_delay(10);
    }
    
    json_cfg_load_transparent(&transparent_cfg);
    
    transparent_init(p_transparent_cb, &transparent_cfg);
    
    transparent_inited = 1;

    while (1)
    {
        transparent_server(p_transparent_cb);
        rt_thread_delay(1);
    }
    
    
}



void rt_thread_entry_transparent_rx(void* parameter)
{
    int res = 0;

    while(transparent_inited == 0)
    {
        rt_thread_delay(5);
    }
    
    while(1)
    {
        //if (rt_sem_take(&sem_TP_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            transparent_rx_parse(p_transparent_cb);
        }
        rt_thread_delay(1);
    }
    

    
}



