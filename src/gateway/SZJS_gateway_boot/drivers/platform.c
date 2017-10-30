#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "rtc.h"
#include "timer.h"
#include "ADC.h"

//#ifdef RT_USING_RTC
//#include "stm32f4_rtc.h"
//#endif /* RT_USING_RTC */

#include "version.h"

#ifdef RT_USING_SPI
#include "stm32f20x_40x_spi.h"
#include "spi_flash_w25qxx.h"

/*
SPI1_MOSI: PB5
SPI1_MISO: PB4
SPI1_SCK : PB3

CS0: PA15  SPI FLASH
CS1: PB2   SD_CARD
*/
static void rt_hw_spi1_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

        /*!< SPI MISO MOSI pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 ;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi10: PA15 */
        spi_cs.GPIOx = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_15;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi11: PB2 */
        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_2;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi11", "spi1", (void*)&spi_cs);
    }

//    /* attach cs */
//    {
//        static struct rt_spi_device spi_device;
//        static struct stm32_spi_cs  spi_cs;

//        GPIO_InitTypeDef GPIO_InitStructure;

//        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//        /* spi20: PG7 */
//        spi_cs.GPIOx = GPIOG;
//        spi_cs.GPIO_Pin = GPIO_Pin_7;
//        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

//        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
//        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
//        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

//        rt_spi_bus_attach_device(&spi_device, "spi22", "spi2", (void*)&spi_cs);
//    }
}

/*
SPI2_MOSI: PB15
SPI2_MISO: PB14
SPI2_SCK : PB13

CS0: PB0   
CS1: PB1   
*/
static void rt_hw_spi2_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

        /*!< SPI MISO MOSI pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi20: PB0 */
        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_0;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", (void*)&spi_cs);
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /* spi21: PB1 */
        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_1;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", (void*)&spi_cs);
    }

}
#endif /* RT_USING_SPI */

void rt_hw_gpio_init(void)
{

    // GSM : reset powerdown and ring. 
    rt_pin_mode(PIN_GSM_PWR_SHUT    , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_GSM_ON_OFF      , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_GSM_RST         , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_GSM_RING        , PIN_MODE_INPUT);
    rt_pin_mode(PIN_GSM_STATE_LED   , PIN_MODE_INPUT);
    
    rt_pin_write(PIN_GSM_RST        , GSM_RST_DIS);     // default is not reset.
    rt_pin_write(PIN_GSM_PWR_SHUT   , GSM_PWR_SHUT_EN); // default is GSM module power on.
    rt_pin_write(PIN_GSM_ON_OFF     , GSM_PIN_PWR_POP); // default is not power action.
    
    // GPIO :  
    rt_pin_mode(PIN_IO_OUT          , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_EXT_RST         , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_IO_IN           , PIN_MODE_INPUT);
    rt_pin_mode(PIN_GPIO_1          , PIN_MODE_INPUT);
    
    rt_pin_write(PIN_IO_OUT         , PIN_LOW);     // default is LOW.
    rt_pin_write(PIN_EXT_RST        , PIN_HIGH);    // default is HIGH.

    // LED :  
    rt_pin_mode(PIN_LED_PWR         , PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_STATUS      , PIN_MODE_OUTPUT);
    
    rt_pin_write(PIN_LED_PWR        , PIN_LOW);     // default is LOW.
    rt_pin_write(PIN_LED_STATUS     , PIN_LOW);     // default is LOW.
    
    // TIMER IN/OUT :  
    rt_pin_mode(PIN_TIM1_IN         , PIN_MODE_INPUT);
    rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
    
    rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.
  
}




void rt_platform_init(void)
{
    
    sscanf(SW_VER, "%d.%d", &sys_main_version, &sys_sub_version);
    rt_kprintf("\nSW version: %d.%02d\n", sys_main_version, sys_sub_version);
    
//    ADC_board_check(&sys_dev_type);
//    
//    #if DEBUG_SYS_TEST_EN
//    ////sys_dev_type = DEV_TYPE_SAMPLE_CTRL;
//    sys_dev_type = DEV_TYPE_SIMPLE_UITD;
//    #endif
//    
//    sys_board_type_checked = 1;
    
    // Song: bootloader not need gpio init. 2017-1-12 15:34:27
////    rt_hw_gpio_init();
    
    //tim2_init();
    //tim4_init();
    
    //ADC_init();
    
#ifdef RT_USING_SPI
    rt_hw_spi1_init();
    
    // Song: bootloader not need SPI2 init. 2017-1-12 15:34:27
    //rt_hw_spi2_init();
    
    /* init hardware device */
    if(w25qxx_init("flash0", "spi10") != RT_EOK)
    {
       rt_kprintf("[error] No such spi flash!\r\n");
    }
    
#endif /* RT_USING_SPI */

//#ifdef RT_USING_RTC
    rt_hw_rtc_init();
//#endif /* RT_USING_RTC */

#ifdef RT_USING_DFS
//    /* initilize sd card */
//#ifdef RT_USING_SDIO
//    //init_mmc();
//    rt_mmcsd_core_init();
//    rt_mmcsd_blk_init();
//    //stm32f4xx_sdio_init();
//    rt_thread_delay(RT_TICK_PER_SECOND/2);
//    //rt_thread_delay(RT_TICK_PER_SECOND);
//#endif

#endif /* RT_USING_DFS */



    ////rt_thread_delay(50);
    //rt_device_init_all();
}

