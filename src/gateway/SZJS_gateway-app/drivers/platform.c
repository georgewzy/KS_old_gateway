#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "rtc.h"
#include "timer.h"
#include "ADC.h"
#include "UITD.h"

//#ifdef RT_USING_RTC
//#include "stm32f4_rtc.h"
//#endif /* RT_USING_RTC */

#include "version.h"

#ifdef RT_USING_SPI
#include "stm32f20x_40x_spi.h"
#include "spi_flash_w25qxx.h"

#include "sw_crc.h"
/*
SPI1_MOSI: PB5
SPI1_MISO: PB4
SPI1_SCK : PB3

CS0: PA15  SPI FLASH
CS1: PB2   SD_CARD
*/
static void rt_hw_spi1_init(e_mother_type mother_type)
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

        if (mother_type == MOTHER_TYPE_GPRS_1_2)
        {
            /* spi11: PB2 */
            spi_cs.GPIOx = GPIOB;
            spi_cs.GPIO_Pin = GPIO_Pin_2;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        }
        else if ((mother_type == mother_type_GPRS))
        {
            /* spi11: PC13 */
            spi_cs.GPIOx = GPIOC;
            spi_cs.GPIO_Pin = GPIO_Pin_13;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        }
        else if ((mother_type == mother_type_ETH) || (mother_type == mother_type_GPRS_ETH) || (mother_type == mother_type_IOT_PRO))
        {
            /* spi11: PD7 */
            spi_cs.GPIOx = GPIOD;
            spi_cs.GPIO_Pin = GPIO_Pin_7;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        }
        else
        {
            /* spi11: PD7 */
            spi_cs.GPIOx = GPIOD;
            spi_cs.GPIO_Pin = GPIO_Pin_7;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        }
        
        
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

NSS: PE14   // VK3266 -- 4 usarts
CS2: PE15   // MCP3208 -- 8 channels ADC 
CS3: PE13   // MBI5030 -- 8 channels PWM controller
CS4: PE12   // MCP23X17 -- 16 GPIOs
CS5: PE11   // MCP23X17 -- 16 GPIOs
*/
static void rt_hw_spi2_init(e_mother_type mother_type)
{

    if ((mother_type == mother_type_GPRS_1_2) || (mother_type == mother_type_GPRS) || (mother_type == mother_type_unknown))
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
    else if ((mother_type == mother_type_ETH) || (mother_type == mother_type_GPRS_ETH) || (mother_type == mother_type_IOT_PRO))
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
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 ;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            /* Connect alternate function */
            GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
            GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
            GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

            stm32_spi_register(SPI2, &stm32_spi, "spi2");
        }

        /* attach NSS */
        {

            static struct rt_spi_device spi_device;
            static struct stm32_spi_cs  spi_cs;
            GPIO_InitTypeDef GPIO_InitStructure;
            
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

            /* spi20: PE14 */
            spi_cs.GPIOx = GPIOE;
            spi_cs.GPIO_Pin = GPIO_Pin_14;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
            GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
            GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

            rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", (void*)&spi_cs);
        }

        /* attach cs2 */
        {

            static struct rt_spi_device spi_device;
            static struct stm32_spi_cs  spi_cs;
            GPIO_InitTypeDef GPIO_InitStructure;

            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

            /* spi21: PE15 */
            spi_cs.GPIOx = GPIOE;
            spi_cs.GPIO_Pin = GPIO_Pin_15;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
            GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
            GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

            rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", (void*)&spi_cs);
        }
        
        /* attach cs3 */
        {
            static struct rt_spi_device spi_device;
            static struct stm32_spi_cs  spi_cs;
            GPIO_InitTypeDef GPIO_InitStructure;

            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

            /* spi21: PE15 */
            spi_cs.GPIOx = GPIOE;
            spi_cs.GPIO_Pin = GPIO_Pin_13;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
            GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
            GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

            rt_spi_bus_attach_device(&spi_device, "spi22", "spi2", (void*)&spi_cs);
        }

        /* attach cs4 */
        {
            static struct rt_spi_device spi_device;
            static struct stm32_spi_cs  spi_cs;
            GPIO_InitTypeDef GPIO_InitStructure;

            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

            /* spi21: PE15 */
            spi_cs.GPIOx = GPIOE;
            spi_cs.GPIO_Pin = GPIO_Pin_12;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
            GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
            GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

            rt_spi_bus_attach_device(&spi_device, "spi23", "spi2", (void*)&spi_cs);
        }

        /* attach cs5 */
        {
            static struct rt_spi_device spi_device;
            static struct stm32_spi_cs  spi_cs;
            GPIO_InitTypeDef GPIO_InitStructure;

            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

            /* spi21: PE15 */
            spi_cs.GPIOx = GPIOE;
            spi_cs.GPIO_Pin = GPIO_Pin_11;
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
            GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
            GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

            rt_spi_bus_attach_device(&spi_device, "spi24", "spi2", (void*)&spi_cs);
        }
        
    }

}
#endif /* RT_USING_SPI */

void rt_hw_gpio_init(e_mother_type mother_type)
{

    
            // GSM : reset powerdown and ring. 
//    #if USE_GSM_PWR_SHUT    
//        rt_pin_mode(PIN_GSM_PWR_SHUT    , PIN_MODE_OUTPUT);
//    #endif // USE_GSM_PWR_SHUT
    //    rt_pin_mode(PIN_GSM_ON_OFF      , PIN_MODE_OUTPUT);
    //    rt_pin_mode(PIN_GSM_RST         , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_GSM_RING        , PIN_MODE_INPUT);
        rt_pin_mode(PIN_GSM_STATE_LED   , PIN_MODE_INPUT);

//    #if USE_GSM_PWR_SHUT    
//        rt_pin_write(PIN_GSM_PWR_SHUT   , GSM_PWR_OFF); // default is GSM module power off.
//    #endif // USE_GSM_PWR_SHUT    
        
    //    rt_pin_write(PIN_GSM_RST        , GSM_RST_DIS);     // default is not reset.
    //    rt_pin_write(PIN_GSM_ON_OFF     , GSM_PIN_PWR_POP); // default is not power action.
        
        switch (sys_config.dev_type)
        {
            case dev_type_sample_ctrl:
                
                //break;
            case dev_type_simple_UITD:
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
                //rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT);
                //rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
                
                //rt_pin_write(PIN_ID_DETECT      , PIN_LOW);     // default is LOW.
                //rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.            
                
                break;
            case dev_type_controller:
                // GPIO :  
                rt_pin_mode(PIN_EXT_RST         , PIN_MODE_OUTPUT);
                rt_pin_mode(PIN_GPIO_1          , PIN_MODE_INPUT);
                rt_pin_mode(PIN_INPUT_01        , PIN_MODE_INPUT);
                rt_pin_mode(PIN_INPUT_02        , PIN_MODE_INPUT);
                rt_pin_mode(PIN_INPUT_03        , PIN_MODE_INPUT);
                rt_pin_mode(PIN_INPUT_04        , PIN_MODE_INPUT);
                rt_pin_mode(PIN_OUTPUT_01       , PIN_MODE_OUTPUT);
                rt_pin_mode(PIN_OUTPUT_02       , PIN_MODE_OUTPUT);
                rt_pin_mode(PIN_OUTPUT_03       , PIN_MODE_OUTPUT);
                
                rt_pin_write(PIN_OUTPUT_01      , PIN_LOW);     // default is LOW.
                rt_pin_write(PIN_OUTPUT_02      , PIN_LOW);     // default is LOW.
                rt_pin_write(PIN_OUTPUT_03      , PIN_LOW);     // default is LOW.
                rt_pin_write(PIN_EXT_RST        , PIN_HIGH);    // default is HIGH.

                // LED :  
                rt_pin_mode(PIN_LED_PWR         , PIN_MODE_OUTPUT);
                rt_pin_mode(PIN_LED_STATUS      , PIN_MODE_OUTPUT);
                
                rt_pin_write(PIN_LED_PWR        , PIN_LOW);     // default is LOW.
                rt_pin_write(PIN_LED_STATUS     , PIN_LOW);     // default is LOW.
                
                // TIMER IN/OUT :  
                //rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT);
                //rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
                
                //rt_pin_write(PIN_ID_DETECT      , PIN_LOW);     // default is LOW.
                //rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.            
                break;
            case dev_type_full_func:
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
                //rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT);
                //rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
                
                //rt_pin_write(PIN_ID_DETECT      , PIN_LOW);     // default is LOW.
                //rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.            
                
                break;
            case dev_type_IOT_PRO_UITD:
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
                //rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT);
                //rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
                
                //rt_pin_write(PIN_ID_DETECT      , PIN_LOW);     // default is LOW.
                //rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.            
                
                break;                
            case dev_type_IOT_PRO_CTRL:
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
                //rt_pin_mode(PIN_ID_DETECT       , PIN_MODE_OUTPUT);
                //rt_pin_mode(PIN_TIM3_OUT        , PIN_MODE_OUTPUT);
                
                //rt_pin_write(PIN_ID_DETECT      , PIN_LOW);     // default is LOW.
                //rt_pin_write(PIN_TIM3_OUT       , PIN_LOW);     // default is LOW.            
                
                break;
            default:
                break;
        }
        
    // Just type_GPRS_1_2 disable the GSM power shut function.    
    if (mother_type != mother_type_GPRS_1_2)
    {
        rt_pin_mode(PIN_GSM_PWR_SHUT    , PIN_MODE_OUTPUT);
        rt_pin_write(PIN_GSM_PWR_SHUT   , GSM_PWR_OFF); // default is GSM module power off.
    }        
        
    if (mother_type == mother_type_GPRS_1_2)
    {
        
    }
    else if (mother_type == mother_type_GPRS)
    {
        
    }
    else if ((mother_type == mother_type_ETH) || (mother_type == mother_type_GPRS_ETH))
    {

        rt_pin_mode(PIN_OUTPUT_04       , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_OUTPUT_05       , PIN_MODE_OUTPUT);

        rt_pin_write(PIN_OUTPUT_04      , PIN_LOW);     // default is LOW.
        rt_pin_write(PIN_OUTPUT_05      , PIN_LOW);     // default is LOW.
        
        rt_pin_mode(PIN_TIM4_OUT        , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_TIM9_OUT        , PIN_MODE_OUTPUT);
        
        rt_pin_write(PIN_TIM4_OUT       , PIN_LOW);     // default is LOW.            
        rt_pin_write(PIN_TIM9_OUT       , PIN_LOW);     // default is LOW.            

        // WIFI module init.
        rt_pin_mode(PIN_WIFI_SHUT       , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_WIFI_RESET      , PIN_MODE_OUTPUT);
        
        rt_pin_write(PIN_WIFI_SHUT      , PIN_LOW);     // default is shutdown.            
        rt_pin_write(PIN_WIFI_RESET     , PIN_HIGH);     // default is not reset.            
        
//        // BLE module init.
//        rt_pin_mode(PIN_BLE_RESET       , PIN_MODE_OUTPUT);
//        rt_pin_write(PIN_BLE_RESET      , PIN_HIGH);     // default is not reset.            
        
    }
    else if (mother_type == mother_type_IOT_PRO)
    {
        rt_pin_mode(PIN_TIM4_OUT        , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_TIM9_OUT        , PIN_MODE_OUTPUT);
        
        rt_pin_write(PIN_TIM4_OUT       , PIN_LOW);     // default is LOW.            
        rt_pin_write(PIN_TIM9_OUT       , PIN_LOW);     // default is LOW.            

        // WIFI module init.
        rt_pin_mode(PIN_WIFI_SHUT       , PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_WIFI_RESET      , PIN_MODE_OUTPUT);
        
        rt_pin_write(PIN_WIFI_SHUT      , PIN_LOW);     // default is shutdown.            
        rt_pin_write(PIN_WIFI_RESET     , PIN_HIGH);     // default is not reset.

        
        rt_pin_mode(PIN_BAT_CTRL       , PIN_MODE_OUTPUT);
        rt_pin_write(PIN_BAT_CTRL      , PIN_HIGH);     // default is battery working.            

        rt_pin_mode(PIN_MCP23017_RST       , PIN_MODE_OUTPUT);
        rt_pin_write(PIN_MCP23017_RST      , PIN_HIGH);     // default is not reset.            

        rt_pin_mode(PIN_MCP23017_INTA       , PIN_MODE_INPUT_PULLUP);
        rt_pin_mode(PIN_MCP23017_INTB       , PIN_MODE_INPUT_PULLUP);

        
    }
    else if (mother_type == mother_type_unknown)
    {
    
    }
    
}


void rt_platform_init(void)
{
    
    sscanf(SW_VER, "%d.%d", &sys_main_version, &sys_sub_version);
    rt_kprintf("\n SW version: %d.%d\n", sys_main_version, sys_sub_version);
    rt_kprintf(" Protocol version: %d.%d\n\n", GB_SW_VER_MAIN, GB_SW_VER_USER);

    //rt_kprintf("sys_testing_mode : 0x%08X\n", sys_testing_mode);
    
//    if (sys_testing_mode == SYS_TESTING_MODE_CODE)
//    {
//        //sys_testing_mode = 0;
//        sys_config.sys_testing_mode = 1;
//        rt_kprintf("Enter testing mode by force ! \n ");
//    }
    
    

//    if (sys_config.dev_type == DEV_TYPE_TESTING_MODE)
//    {
//        sys_config.sys_testing_mode = 1;
//        rt_kprintf("Testing tool detected ! \n ");
//    }
    
    #if DEBUG_SYS_TEST_EN
    ////sys_config.dev_type = DEV_TYPE_SAMPLE_CTRL;
    sys_config.dev_type = DEV_TYPE_MODULE_WIFI;
    ////sys_config.dev_type = DEV_TYPE_SIMPLE_UITD;
    #endif
    
    #if 0
    uint8_t test_crc16[] = {0x02, 0x05, 0x00, 0x03, 0xff, 0x00};
    uint16_t CRC16_res = 0;
    CRC16_res = Crc16(0,test_crc16,sizeof(test_crc16));
    #endif
    
    
    
    stm32_hw_usart_assign(sys_config.mother_type);
    rt_hw_gpio_init(sys_config.mother_type);
    
    tim2_init();
    //tim4_init();
    
    //ADC_init();
    
//#ifdef RT_USING_RTC
    rt_hw_rtc_init();
    list_date();
//#endif /* RT_USING_RTC */
    
#ifdef RT_USING_SPI
    rt_hw_spi1_init(sys_config.mother_type);
    rt_hw_spi2_init(sys_config.mother_type);
    
    /* init hardware device */
    if(w25qxx_init("flash0", "spi10") != RT_EOK)
    {
       rt_kprintf("[error] No such spi flash!\r\n");
    }
    
    
#endif /* RT_USING_SPI */


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

