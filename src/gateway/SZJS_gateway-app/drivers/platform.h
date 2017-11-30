#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "board.h"


//#define UITD_UART_BUF_SIZE      256
//#define UITD_COM_IF_BUF_SIZE    2049

//#define UITD_COM_IF_RING_SIZE   2049

extern uint8_t UITD_debug_usart_en;

extern struct serial_configure COM_if_config;


extern void rt_hw_gpio_init(void);

#endif // __PLATFORM_H__
