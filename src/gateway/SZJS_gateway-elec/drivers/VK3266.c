#include "VK3266.h"



static rt_uint8_t prio_VK32xx = 4;
static rt_thread_t thread_VK32xx;

//struct rt_semaphore sem_VK32xx_IRQ;

//static struct spi_VK_serial_device  VK_spi_device;
static volatile s_VK32XX_SPI_cb VK32XX_SPI_cb = {0};

static u_VK_R_rec  VK_R_rec = {0};
static u_VK_W_rec  VK_W_rec = {0};

static uint8_t temp_FIFO_num = 0;
static uint8_t temp_SIER = 0;

int VK32XX_configure(s_VK_serial *serial, struct serial_configure *cfg);
int VK32XX_control(s_VK_serial *serial, int cmd, void *arg);

static const struct VK_uart_ops VK32xx_uart_ops =
{
    VK32XX_configure,
    VK32XX_control,
};

static s_VK_uart VK_uart_1 = {1};
static s_VK_uart VK_uart_2 = {2};
static s_VK_uart VK_uart_3 = {3};
static s_VK_uart VK_uart_4 = {4};

s_VK_serial VK_serial_1 = {{0}, &VK32xx_uart_ops, VK32_USART_CFG_CONSOLE, NULL, NULL};
s_VK_serial VK_serial_2 = {{0}, &VK32xx_uart_ops, VK32_USART_CFG_CONSOLE, NULL, NULL};
s_VK_serial VK_serial_3 = {{0}, &VK32xx_uart_ops, VK32_USART_CFG_CONSOLE, NULL, NULL};
s_VK_serial VK_serial_4 = {{0}, &VK32xx_uart_ops, VK32_USART_CFG_CONSOLE, NULL, NULL};


void rt_thread_entry_VK32xx(void* parameter);



void VK32xx_IRQ(void)
{
    rt_sem_release(&VK32XX_SPI_cb.sem_IRQ);
}


/**
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTILineX_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  
  
    
  /* Connect EXTI Line10 to PD10 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);

  /* Configure EXTI Line10 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
    
  /* Enable and set EXTI Line10 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}
#define VK32XX_CHANNEL_NUM      4
#define VK32XX_REG_ADDR_MASK    0x0F

int VK32xx_write_reg(uint8_t channel, uint8_t addr, uint16_t data, uint8_t *rec_data)
{
    int res = -1;
    uint8_t send_buf[2] = {0};
    
    if (channel > VK32XX_CHANNEL_NUM)
    {
        return -1;
    }
    
    rt_mutex_take(&VK32XX_SPI_cb.spi_dev.lock, RT_WAITING_FOREVER);
    
    send_buf[0] = 0x80;
    send_buf[0] += ((channel-1) << 5);
    send_buf[0] += (addr & VK32XX_REG_ADDR_MASK)<<1;
    send_buf[0] += (data>>8 == 1)? 1:0;
    
    
    //send_buf[0] = 0x80 + (channel << 5) + (addr & VK32XX_REG_ADDR_MASK)<<1 + (data>>8 == 1)? 1:0;
    send_buf[1] = data;
    
    res = rt_spi_transfer(VK32XX_SPI_cb.spi_dev.rt_spi_device, send_buf, rec_data, 2);
    
    rt_mutex_release(&VK32XX_SPI_cb.spi_dev.lock);
    return res;
}

//int VK32xx_read_regs(uint8_t channel, uint8_t addr, uint8_t *data, uint32_t len)
//{
//    uint8_t res = 0;
//    
//    res = rt_spi_send_then_recv(VK32XX_SPI_cb.spi_dev.rt_spi_device, &addr, 1, data, len);
//    return 0;
//}

int VK32xx_read_reg(uint8_t channel, uint8_t addr, uint8_t *rec_data)
{
    int res = -1;
    uint8_t send_buf[2] = {0};
    
    if (channel > VK32XX_CHANNEL_NUM)
    {
        return -1;
    }
    
    rt_mutex_take(&VK32XX_SPI_cb.spi_dev.lock, RT_WAITING_FOREVER);
    
    send_buf[0] = ((channel-1) << 5);
    send_buf[0] += (addr & VK32XX_REG_ADDR_MASK)<<1;
    //send_buf[0] = (channel << 5) + (addr & VK32XX_REG_ADDR_MASK)<<1;
    //send_buf[1] = 0;
    
    res = rt_spi_transfer(VK32XX_SPI_cb.spi_dev.rt_spi_device, send_buf, rec_data, 2);
    
    rt_mutex_release(&VK32XX_SPI_cb.spi_dev.lock);
    return res;
}



int VK32XX_configure(s_VK_serial *serial, struct serial_configure *cfg)
{
    uint8_t temp_reg = 0;
    s_VK_uart *uart;
    
    uart = serial->parent.user_data;

    // Set the baudrate


    return 0;
}

int VK32XX_tx_FIFO_int_en(uint8_t channel)
{
    uint8_t temp_reg = 0;
    u_VK_R_rec  VK_R_rec = {0};
    u_VK_W_rec  VK_W_rec = {0};
    
    
    return 0;
}

int VK32XX_tx_FIFO_int_dis(uint8_t channel)
{
    uint8_t temp_reg = 0;
    u_VK_R_rec  VK_R_rec;
    u_VK_W_rec  VK_W_rec;
    
    return 0;
}


int VK32XX_tx_FIFO_int_clean(uint8_t channel)
{
    uint8_t temp_reg = 0;
    u_VK_R_rec  VK_R_rec;
    u_VK_W_rec  VK_W_rec;
    

    return 0;
}


int VK32XX_write_char(uint8_t channel, uint8_t data)
{

    return 0;
}

int VK32XX_write_string(uint8_t channel, uint8_t *string)
{
    int i = 0;
    int len = 0;
    
    return VK_W_rec.raw[0];
}


int VK32XX_write_FIFO(uint8_t channel, uint8_t *data, uint32_t len)
{
    int i = 0;

    VK32XX_tx_FIFO_int_en(channel);
    
    
    
    return 0;
}


int VK32XX_control(s_VK_serial *serial, int cmd, void *arg)
{
    uint8_t temp_reg = 0;
    s_VK_uart *uart;
    
    uart = serial->parent.user_data;
    
    switch (cmd)
    {
        case VK_CTRL_ENABLE_INT:
            VK32xx_read_reg(VK32XX_GLOBAL, VK32XX_GIR, (uint8_t *)&VK_R_rec);
            temp_reg = VK_R_rec.data.DATA;
            temp_reg |= (0x0010 << (uart->channel-1));
            VK32xx_write_reg(VK32XX_GLOBAL, VK32XX_GIR, temp_reg, (uint8_t *)&VK_W_rec); // Enable interrupt.
            break;
        case VK_CTRL_DISABLE_INT:
            VK32xx_read_reg(VK32XX_GLOBAL, VK32XX_GIR, (uint8_t *)&VK_R_rec);
            temp_reg = VK_R_rec.data.DATA;
            temp_reg &= ~(0x0010 << (uart->channel-1));
            VK32xx_write_reg(VK32XX_GLOBAL, VK32XX_GIR, temp_reg, (uint8_t *)&VK_W_rec); // Disable interrupt.
            break;
        case VK_CTRL_ENABLE_UART:
            VK32xx_read_reg(uart->channel, VK32XX_SIER, (uint8_t *)&VK_R_rec);
            temp_reg &= !VK32XX_TRIEN; // disable TXD FIFO interrupt
            temp_reg |= VK32XX_RFIEN;  // enable RXD FIFO interrupt
            VK32xx_write_reg(uart->channel, VK32XX_SIER, temp_reg, (uint8_t *)&VK_W_rec); 

            VK32xx_write_reg(uart->channel, VK32XX_SFOCR, 0x0F, (uint8_t *)&VK_W_rec);
            VK32xx_write_reg(uart->channel, VK32XX_SFOCR, 0x0C, (uint8_t *)&VK_W_rec);
        
            VK32xx_read_reg(uart->channel, VK32XX_SCTLR, (uint8_t *)&VK_R_rec);
            temp_reg = VK_R_rec.data.DATA;
            temp_reg |= VK32XX_UTEN;
            VK32xx_write_reg(uart->channel, VK32XX_SCTLR, temp_reg, (uint8_t *)&VK_W_rec); // Enable uart.
        
            VK32XX_SPI_cb.channel_en[uart->channel - 1] = 1;
            break;
        case VK_CTRL_DISABLE_UART:
            VK32xx_read_reg(uart->channel, VK32XX_SCTLR, (uint8_t *)&VK_R_rec);
            temp_reg = VK_R_rec.data.DATA;
            temp_reg &= ~VK32XX_UTEN;
            VK32xx_write_reg(uart->channel, VK32XX_SCTLR, temp_reg, (uint8_t *)&VK_W_rec); // Disable uart.

            VK32xx_write_reg(uart->channel, VK32XX_SFOCR, 0x03, (uint8_t *)&VK_W_rec); // clear the interrupt flag
        
            VK32xx_read_reg(uart->channel, VK32XX_SIER, (uint8_t *)&VK_R_rec);
            temp_reg &= ~VK32XX_TRIEN; // disable TXD FIFO interrupt
            temp_reg &= ~VK32XX_RFIEN;  // disable RXD FIFO interrupt
            VK32xx_write_reg(uart->channel, VK32XX_SIER, temp_reg, (uint8_t *)&VK_W_rec); 
        
            VK32XX_SPI_cb.channel_en[uart->channel - 1] = 0;
           break;
        case VK_CTRL_EN_TX_FIFO_INT:
            VK32XX_tx_FIFO_int_en(uart->channel);
            break;
        case VK_CTRL_DIS_TX_FIFO_INT:
            VK32XX_tx_FIFO_int_dis(uart->channel);
            break;
        default:
            break;
    }

    return 0;
}


int VK32xx_HW_init(void)
{
    
    EXTILineX_Config();
    
    return 0;
}



/*
 * Serial interrupt routines
 */
rt_inline int _VK_serial_fifo_rx(s_VK_serial *serial, rt_uint8_t *data, int length)
{
    int size;
    struct VK_serial_rx_fifo* rx_fifo;

    RT_ASSERT(serial != RT_NULL);
    size = length; 
    
    rx_fifo = (struct VK_serial_rx_fifo*) serial->rx_fifo;
    RT_ASSERT(rx_fifo != RT_NULL);

    /* read from software FIFO */
    while (length)
    {
        int ch;
        rt_base_t level;

        /* disable interrupt */
        level = rt_hw_interrupt_disable();
        if (rx_fifo->get_index != rx_fifo->put_index)
        {
            ch = rx_fifo->buffer[rx_fifo->get_index];
            rx_fifo->get_index += 1;
            if (rx_fifo->get_index >= serial->config.bufsz) rx_fifo->get_index = 0;
        }
        else
        {
            /* no data, enable interrupt and break out */
            rt_hw_interrupt_enable(level);
            break;
        }

        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        *data = ch & 0xff;
        data ++; length --;
    }

    return size - length;
}


rt_inline int _VK_serial_fifo_tx(s_VK_serial *serial, rt_uint8_t *data, int length)
{
    int size;
    struct VK_serial_tx_fifo *tx;
    s_VK_uart   *uart = (s_VK_uart   *)serial->parent.user_data;

    RT_ASSERT(serial != RT_NULL);

    size = length;
    tx = (struct VK_serial_tx_fifo*) serial->tx_fifo;
    RT_ASSERT(tx != RT_NULL);

    while (length)
    {
        if (length > VK32XX_FIFO_SIZE)
        {
            VK32XX_write_FIFO(uart->channel, data, VK32XX_FIFO_SIZE);
            rt_completion_wait(&(tx->completion), RT_WAITING_FOREVER);
            
            data += VK32XX_FIFO_SIZE;
            length -= VK32XX_FIFO_SIZE;
        }
        else
        {
            VK32XX_write_FIFO(uart->channel, data, length);
            rt_completion_wait(&(tx->completion), RT_WAITING_FOREVER);
            
            length = 0;
        }
        
    }

    return size - length;
}

static rt_err_t VK_serial_init (rt_device_t dev)
{

    //VK3266_init();

    return RT_EOK;
}

static rt_err_t VK_serial_open (struct rt_device *dev, rt_uint16_t oflag)
{
    s_VK_serial *serial;
    s_VK_uart   *uart;

    
    
    return RT_EOK;
}

static rt_err_t VK_serial_close (struct rt_device *dev)
{
    s_VK_serial *serial;


    return RT_EOK;
}


static rt_err_t VK_serial_read (struct rt_device *dev,
                                rt_off_t          pos,
                                void             *buffer,
                                rt_size_t         size)
{


}


static rt_err_t VK_serial_write (struct rt_device *dev,
                                 rt_off_t          pos,
                                 void             *buffer,
                                 rt_size_t         size)
{
    s_VK_serial *serial;


    return RT_EOK;
}


static rt_err_t VK_serial_control(struct rt_device *dev,
                                  rt_uint8_t        cmd,
                                  void             *args)
{
    s_VK_serial *serial;
    
    serial = (s_VK_serial *)dev;
    
    switch (cmd)
    {
        case RT_DEVICE_CTRL_CONFIG:
            break;
        default:
            break;
    }

    return RT_EOK;
}


rt_err_t rt_VK_serial_register(s_VK_serial *serial,
                               const char              *name,
                               rt_uint32_t              flag,
                               void                    *data)
{
    struct rt_device *device;
    RT_ASSERT(serial != RT_NULL);

    device = &(serial->parent);

    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = VK_serial_init;
    device->open        = VK_serial_open;
    device->close       = VK_serial_close;
    device->read        = VK_serial_read;
    device->write       = VK_serial_write;
    device->control     = VK_serial_control;
    device->user_data   = data;

    /* register a character device */
    return rt_device_register(device, name, flag);

}


rt_err_t VK32xx_init(const char * spi_device_name)
{
    struct rt_spi_device * rt_spi_device;
    int i = 0;
    //GPIO_InitTypeDef  GPIO_InitStructure;
    
    VK32xx_HW_init();
    
    VK32XX_SPI_cb.channel_num = VK32XX_CHANNEL_NUM;
    for (i=0;i<VK32XX_SPI_cb.channel_num;i++)
    {
        VK32XX_SPI_cb.channel_en[i] = 0;
    }
    
//    RCC_AHB1PeriphClockCmd(W25Q_HOLD_RCC, ENABLE);

//    GPIO_InitStructure.GPIO_Pin   = W25Q_HOLD_PIN;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; /* Push-pull or open drain */
//    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; /* None, Pull-up or pull-down */  
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //Ŧ
//    GPIO_Init(W25Q_HOLD_PORT, &GPIO_InitStructure);

//    GPIO_SetBits(W25Q_HOLD_PORT, W25Q_HOLD_PIN);
    

    /* initialize mutex */
    if (rt_mutex_init(&VK32XX_SPI_cb.spi_dev.lock, spi_device_name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init VK serial lock mutex failed\n");
        return -RT_ENOSYS;
    }

    rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(rt_spi_device == RT_NULL)
    {
        rt_kprintf("spi device %s not found!\r\n", spi_device_name);
        return -RT_ENOSYS;
    }
    VK32XX_SPI_cb.spi_dev.rt_spi_device = rt_spi_device;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
// // Song: modified.
//        cfg.max_hz = 25 * 1000 * 1000; /* 25M */
        cfg.max_hz = 5 * 1000 * 1000; /* 5M */
        rt_spi_configure(VK32XX_SPI_cb.spi_dev.rt_spi_device, &cfg);
    }

    // Initial the VK32xx register.
//    uint8_t rec_data[2] = {0};
//    uint8_t rec_data_2[2] = {0};
//    
//    VK32xx_write_reg(0,0x01, 0x0080, rec_data_2);
//    while(1)
//    {
//        VK32xx_read_reg(0,0x01, rec_data);
//        rt_thread_delay(1);
//    }
    

    
    rt_VK_serial_register(&VK_serial_1, 
                            "VK_uart1",
                            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            &VK_uart_1);

    
    rt_VK_serial_register(&VK_serial_2, 
                            "VK_uart2",
                            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            &VK_uart_2);
    
    
    
    rt_VK_serial_register(&VK_serial_3, 
                            "VK_uart3",
                            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            &VK_uart_3);
    
    
    
    rt_VK_serial_register(&VK_serial_4, 
                            "VK_uart4",
                            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            &VK_uart_4);
                            

    thread_VK32xx = rt_thread_create("VK32xx",
                                            rt_thread_entry_VK32xx,
                                            RT_NULL,
                                            THREAD_VK32XX_STACK_SIZE,
                                            prio_VK32xx,
                                            100);
    if (thread_VK32xx != RT_NULL)
    {        
        rt_thread_startup(thread_VK32xx);     
    }


    
    return RT_EOK;
}



void rt_thread_entry_VK32xx(void* parameter)
{
    rt_err_t res = 0;
    int i = 0;
    int j = 0;
    struct VK_serial_rx_fifo *rx_fifo;
    
    
//    rt_device_t serial_1 = NULL;
//    rt_device_t serial_2 = NULL;
//    rt_device_t serial_3 = NULL;
//    rt_device_t serial_4 = NULL;
    
    rt_sem_init(&VK32XX_SPI_cb.sem_IRQ, "VK_IRQ", 0, RT_IPC_FLAG_FIFO);

    
    while (1)
    {
        res = rt_sem_take(&VK32XX_SPI_cb.sem_IRQ, RT_WAITING_FOREVER);
        if (res == RT_EOK)
        {
            VK32xx_read_reg(VK32XX_GLOBAL, VK32XX_GIR, (uint8_t *)&VK_R_rec);
            VK32XX_SPI_cb.int_global = VK_R_rec.data.DATA;
            
            for (i=0;i<VK32XX_SPI_cb.channel_num;i++)
            {
                if ((VK32XX_SPI_cb.channel_en[i])&&(VK32XX_SPI_cb.int_global & (VK32XX_U1IF<<i)))
                {
                    VK32xx_read_reg(i+1, VK32XX_SIFR, (uint8_t *)&VK_R_rec);
                    VK32XX_SPI_cb.int_channel = VK_R_rec.data.DATA;
                    
                    if (VK32XX_SPI_cb.int_channel & VK32XX_RFINT)
                    {


                    }
                    
                    if ((VK32XX_SPI_cb.int_channel & VK32XX_TFINT ) && VK32XX_SPI_cb.FIFO_int_en[i])
                    {
                    }      
                    
                    if (VK32XX_SPI_cb.int_channel & VK32XX_FOEINT)
                    {
                        // Song: TODO
                        
                    }                
                }
            }
            
            
        }
    
        //rt_thread_delay(2);
    }

}

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT_ALIAS(VK32XX_write_char, VK_write, Write VK uart data --channel --data)
FINSH_FUNCTION_EXPORT_ALIAS(VK32XX_write_string, VK_string, Write VK uart data --channel --string less than 16bytes)

#endif // finsh.h


