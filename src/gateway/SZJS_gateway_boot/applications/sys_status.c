#include "sys_status.h"
#include "timer.h"


//#define THREAD_SYS_STATUS_STACK_SIZE     1024

rt_uint8_t prio_sys_status = 16;
rt_thread_t thread_sys_status;


t_sys_ctrl sys_ctrl = {0};


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
    else if (ctrl->sys_state == SYS_state_IAP_failed)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_flash_fast_3;
            ctrl->sys_state_last = ctrl->sys_state;
        }
        
    }
    else if (ctrl->sys_state == SYS_state_IAP_success)
    {
        if (ctrl->sys_state != ctrl->sys_state_last)
        {
            ctrl->LED_green.mode = LED_on;
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



void rt_thread_entry_sys_status(void* parameter)
{
    int res = 0;
    
    while(1)
    {
        if (sys_inited_flag) break;
        rt_thread_delay(1);
    }
    
    sys_ctrl_init(&sys_ctrl);
    
    sys_status_set(&sys_ctrl, SYS_state_inited);
    while(1)
    {
        sys_ctrl_handler(&sys_ctrl);
        sys_ctrl_server(&sys_ctrl, NULL);
        rt_thread_delay(1);
    }
    
}






