#include "sys_status.h"
#include "timer.h"
#include "fire_alarm.h"
#include "selftest.h"

//#define THREAD_SYS_STATUS_STACK_SIZE     1024

rt_uint8_t prio_sys_status = 16;
rt_thread_t thread_sys_status;

static s_com_bus_R_power_off   alarm_power_off = {0};

t_sys_ctrl sys_ctrl = {0};

void sys_power_manage_init(t_sys_ctrl *ctrl)
{
    // Song: TODO: init the ext power and battery volt detect, ADC pin init etc.
    
    if ((sys_config.dev_type != dev_type_sample_ctrl) &&
        (sys_config.dev_type != dev_type_simple_UITD) &&
        (sys_config.dev_type != dev_type_controller) &&
        (sys_config.dev_type != dev_type_full_func))        
    {
        if (((sys_config.mother_type == mother_type_GPRS_1_2) ||
            (sys_config.mother_type == mother_type_GPRS)))
        {
            
            
        }
        else if ((sys_config.mother_type == mother_type_GPRS_ETH) ||
                 (sys_config.mother_type == mother_type_ETH))
        {
            ////
            
        }
        else if (sys_config.mother_type == mother_type_IOT_PRO)
        {
            // TODO: battery shutdown.
        
        }
    }

}

//int sys_power_manage_server(t_sys_ctrl *ctrl)
//{
//    // Song: TODO: detect the volt periodly.
//    if ((sys_config.dev_type != dev_type_sample_ctrl) &&
//        (sys_config.dev_type != dev_type_simple_UITD) &&
//        (sys_config.dev_type != dev_type_controller) &&
//        (sys_config.dev_type != dev_type_full_func))        
//    {
//        if (((sys_config.mother_type == mother_type_GPRS_1_2) ||
//            (sys_config.mother_type == mother_type_GPRS)))
//        {
//            
//            
//        }
//        else if ((sys_config.mother_type == mother_type_GPRS_ETH) ||
//                 (sys_config.mother_type == mother_type_ETH))
//        {
//            
//            
//        }
//    }
//}


void sys_ctrl_init(t_sys_ctrl *ctrl)
{

    ctrl->sys_state = SYS_state_PWR_ON;
    ctrl->sys_state_last = SYS_state_PWR_ON;

    ctrl->LED_green.mode = LED_off;
    ctrl->LED_green.mode_last = LED_off;
    ctrl->LED_green.count = 0;
    ctrl->LED_green.index = 0;
    
    ctrl->LED_red.mode = LED_on;
    ctrl->LED_red.mode_last = LED_on;
    ctrl->LED_red.count = 0;
    ctrl->LED_red.index = 0;

}

void sys_status_set(t_sys_ctrl *ctrl, e_sys_state status)
{
    ctrl->sys_state_last = ctrl->sys_state;
    ctrl->sys_state = status;
}

void sys_ctrl_handler(t_sys_ctrl *ctrl)
{
    
    if (ctrl->sys_state == SYS_state_PWR_ON)
    {
        //ctrl->LED_green.mode = LED_on;
        //ctrl->vibrator.mode = vibrator_shake_slow_1;
    }
    else if (ctrl->sys_state == SYS_state_inited)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_flash_slow;
            ctrl->sys_state_last = ctrl->sys_state;
        }
    }
    else if (ctrl->sys_state == SYS_state_working)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_on;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_reconnect)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_flash_fast;
            ctrl->sys_state_last = ctrl->sys_state;
        }
    }
    else if (ctrl->sys_state == SYS_state_PWR_OFF)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_flash_fast_3;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_testing)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_flash_fast;
            ctrl->LED_red.mode = LED_flash_fast;
            ctrl->sys_state_last = ctrl->sys_state;
        }
    }
    else if (ctrl->sys_state == SYS_state_test_OK)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_on;
            ctrl->LED_red.mode = LED_on;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_test_ERROR_all)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_off;
            ctrl->LED_red.mode = LED_off;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_test_ERROR_main)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_off;
            ctrl->LED_red.mode = LED_on;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_test_ERROR_extend)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_on;
            ctrl->LED_red.mode = LED_off;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_power_ext)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_red.mode = LED_on;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_battery_relay)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_red.mode = LED_flash_slow;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_battery_very_low)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_red.mode = LED_flash_fast;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_battery_shut)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_red.mode = LED_off;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else
    {
        
    }

    // handle the LED_green mode.
    switch (ctrl->LED_green.mode)
    {
        case LED_off:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
            }
            LED_GREEN_OFF();
            break;
        case LED_on:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
            }
            LED_GREEN_ON();
            break;
        case LED_flash_slow_1:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 1;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_SLOW_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_SLOW_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            
            break;
        case LED_flash_slow_2:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 2;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_SLOW_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_SLOW_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            
            break;
        case LED_flash_slow_3:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 3;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_SLOW_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_SLOW_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            break;
        case LED_flash_slow:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_SLOW_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_SLOW_MAX )
            {
                ctrl->LED_green.count = 0;
                break;
            }
            ctrl->LED_green.count ++;
            break;
        case LED_flash_fast_1:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 1;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_FAST_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_FAST_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            
            break;
        case LED_flash_fast_2:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 2;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_FAST_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_FAST_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            
            break;
        case LED_flash_fast_3:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index = 3;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_FAST_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_FAST_MAX )
            {
                ctrl->LED_green.count = 0;
                ctrl->LED_green.index --;
                if (ctrl->LED_green.index == 0)
                {
                    ctrl->LED_green.mode = LED_off;
                }
                break;
            }
            ctrl->LED_green.count ++;
            break;
        case LED_flash_fast:
            if (ctrl->LED_green.mode != ctrl->LED_green.mode_last)
            {
                ctrl->LED_green.mode_last = ctrl->LED_green.mode;
                ctrl->LED_green.count = 0;
            }
            
            if ( ctrl->LED_green.count == 0 )
            {
                LED_GREEN_ON();
            }
            else if ( ctrl->LED_green.count ==  LED_GREEN_FAST_CNT )
            {
                LED_GREEN_OFF();
            }
            else if (ctrl->LED_green.count >= LED_GREEN_FAST_MAX )
            {
                ctrl->LED_green.count = 0;
                break;
            }
            ctrl->LED_green.count ++;
            break;

        default :
            ctrl->LED_green.mode = LED_off;
            break;

    }

// handle the LED_red mode.
    switch (ctrl->LED_red.mode)
    {
        case LED_off:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
            }
            LED_RED_OFF();
            break;
        case LED_on:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
            }
            LED_RED_ON();
            break;
        case LED_flash_slow_1:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 1;
            }
            
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_SLOW_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_SLOW_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            
            break;
        case LED_flash_slow_2:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 2;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_SLOW_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_SLOW_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            
            break;
        case LED_flash_slow_3:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 3;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_SLOW_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_SLOW_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            break;
        case LED_flash_slow:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_SLOW_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_SLOW_MAX )
            {
                ctrl->LED_red.count = 0;
                break;
            }
            ctrl->LED_red.count ++;
            break;
        case LED_flash_fast_1:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 1;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_FAST_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_FAST_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            
            break;
        case LED_flash_fast_2:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 2;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_FAST_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_FAST_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            
            break;
        case LED_flash_fast_3:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index = 3;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_FAST_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_FAST_MAX )
            {
                ctrl->LED_red.count = 0;
                ctrl->LED_red.index --;
                if (ctrl->LED_red.index == 0)
                {
                    ctrl->LED_red.mode = LED_off;
                }
                break;
            }
            ctrl->LED_red.count ++;
            break;
        case LED_flash_fast:
            if (ctrl->LED_red.mode != ctrl->LED_red.mode_last)
            {
                ctrl->LED_red.mode_last = ctrl->LED_red.mode;
                ctrl->LED_red.count = 0;
            }
            
            if ( ctrl->LED_red.count == 0 )
            {
                LED_RED_ON();
            }
            else if ( ctrl->LED_red.count ==  LED_RED_FAST_CNT )
            {
                LED_RED_OFF();
            }
            else if (ctrl->LED_red.count >= LED_RED_FAST_MAX )
            {
                ctrl->LED_red.count = 0;
                break;
            }
            ctrl->LED_red.count ++;
            break;

        default :
            ctrl->LED_red.mode = LED_off;
            break;

    }
    
    
}


void sys_ctrl_server(t_sys_ctrl *ctrl, void *p_cb)
{
    
    ////struct t_quanta_cb *cb = p_cb;
    
    if (ctrl->sys_state == SYS_state_PWR_OFF) // stop the data sample and close the interface.
    {
        

        //ctrl->sys_state = SYS_state_shutdown;  // ready to shutdown
    }
    else if (ctrl->sys_state == SYS_state_shutdown)
    {
        
        
        
    }

}

void sys_battery_server(void)
{
    static uint32_t cnt = 0;
    static int16_t battery_volt = 0;
    
    cnt ++;
    if (cnt > RT_TICK_PER_SECOND)
    {
        cnt = 0;
    
        battery_volt = ADC_power_battery_sample();
        
        if (battery_volt > 5800)
        {
            sys_ctrl.power_status.status = power_unknown;
        }
        else if (battery_volt > 4600)
        {
            sys_ctrl.power_status.status = power_ext;
        }
        else if (battery_volt > 3900)
        {
            sys_ctrl.power_status.status = power_batt_full;
        }
        else if (battery_volt > 3700)
        {
            sys_ctrl.power_status.status = power_batt_half;
        }
        else if (battery_volt > 3500)
        {
            sys_ctrl.power_status.status = power_batt_low;
        }
        else if (battery_volt > 3200)
        {
            sys_ctrl.power_status.status = power_batt_very_low;
        }
        
        if ((sys_ctrl.power_status.status_last == power_ext) && 
            ((sys_ctrl.power_status.status == power_batt_full) ||
             (sys_ctrl.power_status.status == power_batt_half) ||
             (sys_ctrl.power_status.status == power_batt_low)
            ))
        {
            sys_ctrl.power_status.state = power_state_battery_relay;
        }
        else if ((sys_ctrl.power_status.status == power_ext) && 
            ((sys_ctrl.power_status.status_last == power_batt_full) ||
             (sys_ctrl.power_status.status_last == power_batt_half) ||
             (sys_ctrl.power_status.status_last == power_batt_low)
            ))
        {
            sys_ctrl.power_status.state = power_state_extern_resume;
        }        
        else if ((sys_ctrl.power_status.status == power_batt_very_low) &&
                 (sys_ctrl.power_status.status != power_batt_shutting))
        {
            sys_ctrl.power_status.status = power_batt_shutting;
            sys_ctrl.power_status.status_last = sys_ctrl.power_status.status;
            
            sys_ctrl.power_status.state = power_state_shutting;
        }
        
        
        
        sys_ctrl.power_status.status_last = sys_ctrl.power_status.status;
        
    }
    
    switch (sys_ctrl.power_status.state)
    {
        case power_state_extern:
            
            break;
        case power_state_battery_relay:
            // TODO: report the extern power lost alarm.
            
            alarm_power_off.valid = 1;
            rt_mq_send(mq_FA_power_off, &alarm_power_off, sizeof(s_com_bus_R_power_off));
            rt_mq_send(mq_FA_2_power_off, &alarm_power_off, sizeof(s_com_bus_R_power_off));

        
            sys_ctrl.power_status.state = power_state_battery;
            sys_status_set(&sys_ctrl, SYS_state_battery_relay);
            break;
        case power_state_battery:
            
            break;
        case power_state_extern_resume:
            
            sys_ctrl.power_status.state = power_state_extern;
            sys_status_set(&sys_ctrl, SYS_state_power_ext);
            break;
        case power_state_shutting:
            // TODO: Save the files, report the shutdown to server
            
            sys_ctrl.power_status.state = power_state_shut;
            sys_status_set(&sys_ctrl, SYS_state_battery_very_low);
            break;
        case power_state_shut:
            
            sys_ctrl.power_status.state = power_state_shut_down;
            sys_status_set(&sys_ctrl, SYS_state_battery_shut);
            break;
        case power_state_shut_down:
            
            break;
        case power_state_charging:
            
            break;
        case power_state_charge_full:
            
            break;
        default:
            break;
    }
    
    
}



void rt_thread_entry_sys_status(void* parameter)
{
    int res = 0;
    
    while(1)
    {
        if (sys_config.board_type_checked) break;
        rt_thread_delay(1);
    }
    
    sys_ctrl_init(&sys_ctrl);
    
    sys_power_manage_init(&sys_ctrl);
    
    sys_status_set(&sys_ctrl, SYS_state_inited);
    while(1)
    {

        //if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
        if ((sys_config.sys_mode == sys_mode_testing))
        {
            testing_status_check();
        }
        
        if (sys_config.battery_check)
        {
            sys_battery_server();
        }
        
        sys_ctrl_handler(&sys_ctrl);
        sys_ctrl_server(&sys_ctrl, NULL);
        //sys_power_manage_server(&sys_ctrl);
        rt_thread_delay(1);
    }
    
}






