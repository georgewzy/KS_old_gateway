#ifndef __SYS_STATUS_H__
#define __SYS_STATUS_H__


#include <board.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include "gpio.h"

#define LED_GREEN_OFF()   rt_pin_write(PIN_LED_STATUS, PIN_LOW)
#define LED_GREEN_ON()    rt_pin_write(PIN_LED_STATUS, PIN_HIGH)

#define LED_RED_OFF()     rt_pin_write(PIN_LED_PWR, PIN_LOW)
#define LED_RED_ON()      rt_pin_write(PIN_LED_PWR, PIN_HIGH)


//#define BATT_VOLT_VERY_LOW  330
//#define BATT_VOLT_LOW       350

#define LED_GREEN_SLOW_CNT   (200/10) // 200ms,  unit 10ms
#define LED_GREEN_SLOW_MAX   (2000/10) // 2000ms,  unit 10ms
#define LED_GREEN_FAST_CNT   (100/10) // 100ms,  unit 10ms
#define LED_GREEN_FAST_MAX   (500/10) // 500ms,  unit 10ms

#define LED_RED_SLOW_CNT   (200/10) // 200ms,  unit 10ms
#define LED_RED_SLOW_MAX   (2000/10) // 2000ms,  unit 10ms
#define LED_RED_FAST_CNT   (100/10) // 100ms,  unit 10ms
#define LED_RED_FAST_MAX   (500/10) // 500ms,  unit 10ms

typedef enum
{
    SYS_state_PWR_ON = 0, SYS_state_inited, SYS_state_idle, 
    SYS_state_working, SYS_state_reconnect, SYS_state_PWR_OFF, SYS_state_shutdown, 
    SYS_state_testing, SYS_state_test_ERROR_all, SYS_state_test_ERROR_main, SYS_state_test_ERROR_extend, SYS_state_test_OK,
    SYS_state_power_ext, SYS_state_battery_relay, SYS_state_battery_very_low, SYS_state_battery_shut,
} e_sys_state;


typedef enum
{
    batt_uninit = 0, batt_ok, batt_low, batt_very_low, batt_off, batt_charging, batt_charged,
} e_batt_state;


typedef enum
{
    LED_off = 0, LED_on, LED_flash_slow, LED_flash_slow_1, LED_flash_slow_2, LED_flash_slow_3, 
    LED_flash_fast, LED_flash_fast_1, LED_flash_fast_2, LED_flash_fast_3, LED_on_off, LED_breath, 
} e_LED_mode;


typedef struct
{
    e_LED_mode      mode;
    e_LED_mode      mode_last;
    uint16_t        count;
    uint16_t        index;
    
} t_LED_ctrl;



typedef volatile struct
{
    e_sys_state         sys_state;
    e_sys_state         sys_state_last;
    
    e_batt_state        batt_state;
    e_batt_state        batt_state_last;
    
    t_LED_ctrl          LED_red;
    t_LED_ctrl          LED_green;
    
    uint16_t            timer;
    
    uint16_t            power_cnt;
    
    s_power_status      power_status;
  
} t_sys_ctrl;





extern t_sys_ctrl sys_ctrl;

extern void sys_ctrl_init(t_sys_ctrl *ctrl);
extern void sys_ctrl_handler(t_sys_ctrl *ctrl);
extern void sys_ctrl_cb(t_sys_ctrl *ctrl, void *p_data, void *p_if);
extern void sys_ctrl_server(t_sys_ctrl *ctrl, void *p_cb);






#define THREAD_SYS_STATUS_STACK_SIZE     2048

extern rt_uint8_t prio_sys_status;
extern rt_thread_t thread_sys_status;



extern void rt_thread_entry_sys_status(void* parameter);





#endif // __SYS_STATUS_H__
