#ifndef __TRANSPARENT_H__
#define __TRANSPARENT_H__

#include "board.h"
//#include "sys_def.h"
#include <rtdevice.h>

#include "UITD.h"

#define THREAD_TRANSPARENT_STACK_SIZE        2048
#define THREAD_TRANSPARENT_RX_STACK_SIZE     512

#define TP_CFG_BUF_PERIOD_MIN           1000 // 1S
#define TP_CFG_BUF_PERIOD_DEF           10000 // 10S
#define TP_CFG_TRANS_PERIOD_MIN         500 // 0.5S
#define TP_CFG_TRANS_PERIOD_DEF         1000 // 1S

#define TP_CFG_BAUDRATE_DEFAULT         9600
#define TP_CFG_BITS_DEFAULT             8
#define TP_CFG_VERIFY_DEFAULT           0
#define TP_CFG_STOPBIT_DEFAULT          1



typedef enum
{
    TP_idle = 0, TP_init, TP_inited, TP_trans, TP_stop,

} e_transparent_status;

//typedef struct
//{
//    uint32_t    baudrate;
//    uint8_t     bits;
//    uint8_t     verify;
//    uint8_t     stopbit;
//} s_transparent_com_cfg;

//typedef struct
//{
//    uint32_t                trans_period;  // unit ms.
//    uint32_t                buffer_period; // unit ms.
//    
//    uint8_t                 uart[8];
//    s_transparent_com_cfg   cfg;
//    
//} s_transparent_cfg;

typedef struct
{
    e_transparent_status    status;
    rt_device_t             dev;
    void                    *handler;
    void                    *handler_2;
    
    s_transparent_cfg       cfg;
    uint8_t                 *rec_buf;
    uint32_t                rec_buf_size;
    uint16_t                rec_len;
    
    uint32_t                TS;
    uint8_t                 seg_index;
    uint8_t                 seg_total;
    uint8_t                 *data;
    
    struct rt_ringbuffer    ring_buf;
    uint8_t                 *ring_buf_buf;
    uint32_t                ring_buf_buf_size;
    s_transparent_data      trans_data;
    
    uint32_t                counter;
    uint8_t                 tick_flag;
} s_transparent_cb;


extern volatile s_transparent_cb *p_transparent_cb;

extern rt_uint8_t prio_transparent;
extern rt_thread_t thread_transparent;

extern rt_uint8_t prio_transparent_rx;
extern rt_thread_t thread_transparent_rx;

extern struct rt_semaphore sem_TP_rx;
extern struct rt_device * device_transparent;

extern void rt_thread_entry_transparent(void* parameter);
extern void rt_thread_entry_transparent_rx(void* parameter);

#endif  // __TRANSPARENT_H__
