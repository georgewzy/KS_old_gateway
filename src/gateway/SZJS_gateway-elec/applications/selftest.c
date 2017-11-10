#include "selftest.h"

#include <dfs_posix.h>
#include "sys_status.h"
#include "sys_config.h"


int create_testing_log(char *log_file)
{
    uint8_t log_buf[256] = {0};
    uint32_t now;
    int res = 0;

    rt_time(&now);
    
    
    touch(log_file, 1);
        
    sys_config.test_flag.fd = open(log_file, O_WRONLY, 0);
    if (sys_config.test_flag.fd < 0)
    {
        SYS_log( SYS_DEBUG_ERROR, ("Create testing file failed ! \n"));
        return -1;
    }
    
    res = sprintf(log_buf, "Mother_board_type : %s\n", sys_get_mother_board_string(sys_config.mother_type));
    if (res < 0)
    {
        SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
    }
    else
    {
        write(sys_config.test_flag.fd, log_buf, res);
    }

    res = sprintf(log_buf, "Device_board_type : %s\n\n", sys_get_ext_dev_module_string(sys_config.dev_type));
    if (res < 0)
    {
        SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
    }
    else
    {
        write(sys_config.test_flag.fd, log_buf, res);
    }

    res = sprintf(log_buf, "Time : %s\n\n", ctime(&now));
    if (res < 0)
    {
        SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
    }
    else
    {
        write(sys_config.test_flag.fd, log_buf, res);
    }


//    res = sprintf(log_buf, "TF : %d\n", sys_config.test_flag.TF);
//    if (res < 0)
//    {
//        SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
//    }
//    else
//    {
//        write(sys_config.test_flag.fd, log_buf, res);
//    }
    
    
    if ((sys_config.mother_type == mother_type_GPRS_1_2) || (sys_config.mother_type == mother_type_GPRS))
    {
        res = sprintf(log_buf, "GSM : %d\n", sys_config.test_flag.GSM);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
    }
    else if (sys_config.mother_type == mother_type_ETH)
    {
        res = sprintf(log_buf, "ethernet : %d\n", sys_config.test_flag.ethernet);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
        
        res = sprintf(log_buf, "WIFI : %d\n", sys_config.test_flag.WIFI);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
    }
    else if ((sys_config.mother_type == mother_type_GPRS_ETH) || (sys_config.mother_type == mother_type_IOT_PRO))
    {
        res = sprintf(log_buf, "GSM : %d\n", sys_config.test_flag.GSM);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }

        res = sprintf(log_buf, "ethernet : %d\n", sys_config.test_flag.ethernet);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
        
//        res = sprintf(log_buf, "WIFI : %d\n", sys_config.test_flag.WIFI);
//        if (res < 0)
//        {
//            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
//        }
//        else
//        {
//            write(sys_config.test_flag.fd, log_buf, res);
//        }
        
    }
    
    
    if (sys_config.dev_type == dev_type_simple_UITD)
    {
        res = sprintf(log_buf, "RS232 :\t %d\n", sys_config.test_flag.RS232);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }

        res = sprintf(log_buf, "RS485 :\t %d\n", sys_config.test_flag.RS485);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
        
        res = sprintf(log_buf, "input :\t %d\n", sys_config.test_flag.input);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }

    }
    else if (sys_config.dev_type == dev_type_sample_ctrl)
    {
        res = sprintf(log_buf, "sensor_4_20mA :\t %d\n", sys_config.test_flag.sensor_4_20mA);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }

        res = sprintf(log_buf, "sensor_5V :\t %d\n", sys_config.test_flag.sensor_5V);
        if (res < 0)
        {
            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
        }
        else
        {
            write(sys_config.test_flag.fd, log_buf, res);
        }
        
//        res = sprintf(log_buf, "sensor_Hz :\t %d\n", sys_config.test_flag.sensor_Hz);
//        if (res < 0)
//        {
//            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
//        }
//        else
//        {
//            write(sys_config.test_flag.fd, log_buf, res);
//        }
        
//        res = sprintf(log_buf, "output :\t %d\n", sys_config.test_flag.output);
//        if (res < 0)
//        {
//            SYS_log( SYS_DEBUG_ERROR, ("Log sprintf failed ! \n"));
//        }
//        else
//        {
//            write(sys_config.test_flag.fd, log_buf, res);
//        }
    }
    else if (sys_config.dev_type == dev_type_IOT_PRO_UITD)
    {
        // Song: TODO
        
    }    
    else if (sys_config.dev_type == dev_type_IOT_PRO_CTRL)
    {
        // Song: TODO
        
    }    
    
    close(sys_config.test_flag.fd);
    
    return 0;
}

// Song: Testing -- LED green and red flash.  All pass --- LED all on,  
//      Main board error --- LED green off , red on.   Extend board error --- LED red off, green on.  
//      ALL error --- LED all off.
int testing_status_check(void)
{
    
    if ((sys_config.mother_type == mother_type_GPRS_1_2) || (sys_config.mother_type == mother_type_GPRS))
    {
        if (sys_config.test_flag.GSM)
        {
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);

                }
            }     
            else if (sys_config.dev_type == dev_type_full_func)
            {
                // Song: TODO:
                
            }
                    
        }
    }
    else if (sys_config.mother_type == mother_type_ETH)
    {
        if (sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
        {
            // Waiting for a while, make sure the other testing is over.
            ////rt_thread_delay(RT_TICK_PER_SECOND * 10); 
            
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);

                }
            }     
            else if (sys_config.dev_type == dev_type_full_func)
            {
                // Song: TODO:
                
            }
                    
        }
    }
    else if (sys_config.mother_type == mother_type_GPRS_ETH)
    {
        if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
        //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
        {
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);

                }
            }     
            else if (sys_config.dev_type == dev_type_full_func)
            {
                // Song: TODO:
                
            }
                    
        }    
    }
    else if (sys_config.mother_type == mother_type_IOT_PRO)
    {
        if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
        //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
        {
            if (sys_config.dev_type == dev_type_IOT_PRO_UITD)
            {
                if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);
                }
            }
            else if (sys_config.dev_type == dev_type_IOT_PRO_CTRL)
            {
                if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    create_testing_log(SYS_TESTING_FILE_PATH);
                    rt_kprintf("\nTesting passed \n\n");
                    cat(SYS_TESTING_FILE_PATH);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    //sys_testing_mode = 0;
                    sys_status_set(&sys_ctrl, SYS_state_test_OK);
                    sys_config.sys_mode = sys_mode_tested;
                    //while(1) rt_thread_delay(5);

                }
            }     
                    
        }    
    }
    
    if (sys_config.test_flag.counter >= 1000*60) // More than 60 second , timeout.
    {
        create_testing_log(SYS_TESTING_FILE_PATH);
        rt_kprintf("\nTesting timeout \n\n");
        cat(SYS_TESTING_FILE_PATH);
        //json_cfg_wr_testing_mode(0);
        //sys_testing_mode = 0;
        
        if ((sys_config.mother_type == mother_type_GPRS_1_2) || (sys_config.mother_type == mother_type_GPRS))
        {
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.GSM)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.GSM)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }        
            else if (sys_config.dev_type == dev_type_full_func)
            {

            } 
        }
        else if (sys_config.mother_type == mother_type_ETH)
        {
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }        
            else if (sys_config.dev_type == dev_type_full_func)
            {

            } 
        }
        else if (sys_config.mother_type == mother_type_GPRS_ETH)
        {   
            if (sys_config.dev_type == dev_type_simple_UITD)
            {
                if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
                //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }
            else if (sys_config.dev_type == dev_type_sample_ctrl)
            {
                if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
                //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }        
            else if (sys_config.dev_type == dev_type_full_func)
            {

            } 
        }
        else if (sys_config.mother_type == mother_type_IOT_PRO)
        {   
            if (sys_config.dev_type == dev_type_IOT_PRO_UITD)
            {
                if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
                //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.input && sys_config.test_flag.RS232 && sys_config.test_flag.RS485)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }
            else if (sys_config.dev_type == dev_type_IOT_PRO_CTRL)
            {
                if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet)
                //if (sys_config.test_flag.GSM && sys_config.test_flag.ethernet && sys_config.test_flag.WIFI)
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_extend);
                }
                else if (sys_config.test_flag.sensor_4_20mA && sys_config.test_flag.sensor_5V )
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_main);
                }
                else
                {
                    sys_status_set(&sys_ctrl, SYS_state_test_ERROR_all);
                }
            }        
        }        

        sys_config.sys_mode = sys_mode_tested;
        //while(1) rt_thread_delay(5);
    }
    
    
}















