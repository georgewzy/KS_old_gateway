/*
 * File      : gpio.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-05     Bernard      the first version
 */

#include <rthw.h>
#include <rtdevice.h>
#include <board.h>

#ifdef RT_USING_PIN

/* STM32 GPIO driver */
struct pin_index
{
    int index;
    uint32_t rcc;
    GPIO_TypeDef *gpio;
    uint32_t pin;
};




static const struct pin_index pins[] =
{
    { PIN_GSM_RING          , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_6},
    { PIN_GSM_PWR_SHUT      , RCC_AHB1Periph_GPIOC, GPIOD, GPIO_Pin_1},
    { PIN_GSM_ON_OFF        , RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_12},
    { PIN_GSM_RST           , RCC_AHB1Periph_GPIOA, GPIOA, GPIO_Pin_7},
    { PIN_GSM_STATE_LED     , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_7},
    { PIN_IO_IN             , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_5},
    { PIN_IO_OUT            , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_4},
    { PIN_GPIO_1            , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_13},
    { PIN_LED_PWR           , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_2},
    { PIN_LED_STATUS        , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_3},
    { PIN_EXT_RST           , RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_0},
    { PIN_TIM1_IN           , RCC_AHB1Periph_GPIOA, GPIOA, GPIO_Pin_8},
    { PIN_TIM3_OUT          , RCC_AHB1Periph_GPIOA, GPIOA, GPIO_Pin_6},
};


#define ITEM_NUM(items) sizeof(items)/sizeof(items[0])
const struct pin_index *get_pin(uint8_t pin)
{
    const struct pin_index *index;

    if (pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
    }
    else
    {
        index = RT_NULL;
    }

    return index;
};

void stm32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

    if (value == PIN_LOW)
    {
        GPIO_ResetBits(index->gpio, index->pin);
    }
    else
    {
        GPIO_SetBits(index->gpio, index->pin);
    }
}

int stm32_pin_read(rt_device_t dev, rt_base_t pin)
{
    int value;
    const struct pin_index *index;

    value = PIN_LOW;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }

    if (GPIO_ReadInputDataBit(index->gpio, index->pin) == Bit_RESET)
    {
        value = PIN_LOW;
    }
    else
    {
        value = PIN_HIGH;
    }

    return value;
}

void stm32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    GPIO_InitTypeDef  GPIO_InitStructure;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(index->rcc, ENABLE);

    /* Configure GPIO_InitStructure */
    GPIO_InitStructure.GPIO_Pin     = index->pin;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;

    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    }
    else
    {
        /* input setting:default. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    }
    GPIO_Init(index->gpio, &GPIO_InitStructure);
}

const static struct rt_pin_ops _stm32_pin_ops =
{
    stm32_pin_mode,
    stm32_pin_write,
    stm32_pin_read,
};

int stm32_hw_pin_init(void)
{
    rt_device_pin_register("pin", &_stm32_pin_ops, RT_NULL);
    return 0;
}
INIT_BOARD_EXPORT(stm32_hw_pin_init);

#endif
