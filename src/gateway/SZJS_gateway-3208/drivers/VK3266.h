#ifndef __VK3266_H__
#define __VK3266_H__

#include "board.h"
#include "sys_misc.h"
#include <rtdevice.h>

#define VK32XX_PORT     GPIOD
#define VK32XX_PIN      GPIO_Pin_10
//#define VK32XX_RCC      

#define THREAD_VK32XX_STACK_SIZE    1024

#define VK32XX_CHANNEL_NUM      4
#define VK32XX_FIFO_SIZE        16


#define VK_CTRL_ENABLE_INT          1
#define VK_CTRL_DISABLE_INT         2
#define VK_CTRL_ENABLE_UART         3
#define VK_CTRL_DISABLE_UART        4
#define VK_CTRL_EN_TX_FIFO_INT      5
#define VK_CTRL_DIS_TX_FIFO_INT     6



#define     VK32XX_CH1    1
#define     VK32XX_CH2    2
#define     VK32XX_CH3    3
#define     VK32XX_CH4    4
#define     VK32XX_GLOBAL 1

//EINT15 GPG7
//vk32xx rigister address defines
#define 	VK32XX_GCR      0X01
#define 	VK32XX_GUCR     0X02
#define		VK32XX_GIR     	0X03
#define 	VK32XX_XOFF     0X04
#define 	VK32XX_X0N      0X05

#define 	VK32XX_SCTLR    0X06
#define 	VK32XX_SCONR    0X07
#define 	VK32XX_SFWCR    0X08
#define 	VK32XX_SFOCR    0X09
#define 	VK32XX_SADR     0X0A
#define 	VK32XX_SIER     0X0B
#define 	VK32XX_SIFR     0X0C
#define 	VK32XX_SSR      0X0D
#define 	VK32XX_SFSR     0X0E
#define 	VK32XX_SFDR     0X0F

//vk32xx register bit defines
//GIR
#define 	VK32XX_U4IEN		0x80
#define 	VK32XX_U3IEN		0x40
#define 	VK32XX_U2IEN		0x20
#define 	VK32XX_U1IEN		0x10
#define 	VK32XX_U4IF		    0x08
#define 	VK32XX_U3IF		    0x04
#define 	VK32XX_U2IF		    0x02
#define 	VK32XX_U1IF		    0x01

//SIENR
#define 	VK32XX_RFIEN		0x01
#define 	VK32XX_TRIEN		0x02

//SSR
#define 	VK32XX_OE		    0x80
#define		VK32XX_FE		    0x40
#define 	VK32XX_PE		    0x20
#define		VK32XX_RX8		    0x10
#define 	VK32XX_TFFL 		0x08		//子串口发送FIFO 满标志
#define 	VK32XX_TFEM 		0x04		//子串口发送FIFO 空标志
#define 	VK32XX_TXBY 		0x02		//子串口发送TX 忙标志
#define 	VK32XX_RFEM 		0x01		//子串口接收FIFO 空标志

//SCTLR
#define 	VK32XX_UTEN		    0x08

//SCONR
#define		VK32XX_SSTPL		0x80
#define 	VK32XX_SPAEN		0x40
#define 	VK32XX_SFPAEN		0x20
#define 	VK32XX_PAM1		    0x10
#define 	VK32XX_PAM0		    0x08

//SIFR
#define 	VK32XX_CTSR		    0x80
#define		VK32XX_FOEINT		0x40
#define 	VK32XX_RAINT		0x20
#define 	VK32XX_XFINT		0x10
#define 	VK32XX_RSTINT		0X08
#define 	VK32XX_CTSINT		0x04
#define 	VK32XX_TFINT		0x02
#define 	VK32XX_RFINT		0x01


//vk32xx hardware configuration
#define 	VK_CRASTAL_CLK		3686400



typedef struct
{
    uint8_t     INT1 :1;
    uint8_t     INT2 :1;
    uint8_t     INT3 :1;
    uint8_t     INT4 :1;
    uint8_t     TXF  :1;
    uint8_t     TXE  :1;
    uint8_t     TXB  :1;
    uint8_t     RXE  :1;
    uint8_t     RXB  :1;
    uint8_t     TC2  :1;
    uint8_t     TC1  :1;
    uint8_t     TC0  :1;
    uint8_t     RC3  :1;
    uint8_t     RC2  :1;
    uint8_t     RC1  :1;
    uint8_t     RC0  :1;
} s_VK_W_rec_data;

typedef union
{
    uint8_t             raw[2];
    s_VK_W_rec_data     data;
} u_VK_W_rec;

typedef struct
{
    uint8_t     INT1 :1;    
    uint8_t     INT2 :1;
    uint8_t     INT3 :1;
    uint8_t     INT4 :1;
    uint8_t     OE   :1;
    uint8_t     FE   :1;
    uint8_t     PE   :1;
    uint8_t     RX8  :1;
    uint8_t     DATA :8;
} s_VK_R_rec_data;

typedef union
{
    uint8_t             raw[2];
    s_VK_R_rec_data     data;
} u_VK_R_rec;

/**
 * uart operators
 */


/*
 * Serial FIFO mode 
 */
struct VK_serial_rx_fifo
{
	/* software fifo */
	rt_uint8_t *buffer;

	rt_uint16_t put_index, get_index;
};

struct VK_serial_tx_fifo
{
    uint8_t     buf[VK32XX_FIFO_SIZE];
    uint8_t     len;
	struct rt_completion completion;
};

typedef struct VK_serial_device
{
    struct rt_device            parent;

    const struct VK_uart_ops    *ops;
    struct serial_configure     config;
    
    //struct rt_mutex           lock;
    
    struct VK_serial_rx_fifo    *rx_fifo;
    struct VK_serial_tx_fifo    *tx_fifo;

	//void *serial_rx;
	//void *serial_tx;
} s_VK_serial;


struct VK_uart_ops
{
    rt_err_t (*configure)(s_VK_serial *serial, struct serial_configure *cfg);
    rt_err_t (*control)(s_VK_serial *serial, int cmd, void *arg);

    //int (*putc)(struct rt_serial_device *serial, char c);
    //int (*getc)(struct rt_serial_device *serial);

    //rt_size_t (*dma_transmit)(struct rt_serial_device *serial, const rt_uint8_t *buf, rt_size_t size, int direction);
};


typedef struct VK_uart
{
    uint8_t         channel;
    
    //uint8_t         VK32XX_UXIF;
    //struct rt_device          parent;

//    const struct rt_uart_ops *ops;
    //struct serial_configure   config;
    
    //struct rt_mutex           lock;

//	void *serial_rx;
//	void *serial_tx;
} s_VK_uart;



struct spi_VK_serial_device
{
//    struct rt_device                VK_serial_device;
    struct rt_spi_device *          rt_spi_device;
    struct rt_mutex                 lock;
};


typedef struct VK32XX_SPI_cb
{
    struct spi_VK_serial_device     spi_dev;
    uint8_t                         channel_en[VK32XX_CHANNEL_NUM];
    uint8_t                         channel_num;
    
    struct rt_semaphore             sem_IRQ;
    uint8_t                         int_global;
    uint8_t                         int_channel;
    
    uint8_t                         FIFO_int_en[VK32XX_CHANNEL_NUM];
    
    s_VK_serial                     *serial[VK32XX_CHANNEL_NUM];
    
} s_VK32XX_SPI_cb;


//extern s_VK32XX_SPI_cb VK32XX_SPI_cb;

//extern struct rt_semaphore sem_VK32xx_IRQ;




//extern struct spi_VK_serial_device  VK_spi_device;

extern void VK32xx_IRQ(void);



#endif // __VK3266_H__
