/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>

#include "version.h"
#include "sys_config.h"
//#include "UITD_main.h"
//#include "GSM_MG301.h"
//#include "WIFI_ESP_07.h"
//#include "data_trans.h"
#include "sys_status.h"
//#include "sensor_sample.h"
//#include "fire_alarm.h"

#include "STM32_IAP.h"

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f4xx_eth.h"
#endif

#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#include <dfs_posix.h>

#ifdef RT_USING_DFS_ROMFS 
#include <dfs_romfs.h> 
#endif
#endif

#include "rtc.h"    
#include "platform.h"
#include "UITD.h"


extern void rt_platform_init(void);





//void system_fs_check(void)
//{
//    int fd;
//    int res = 0;
//    DIR *dir;
//    char temp_char = 0;

//    
//    // Song: just a file expend test.
////    rm("/test.txt");
////    fd = open("/test.txt", O_CREAT | O_RDWR | O_TRUNC , 0);
////    if (fd >= 0)
////    {
////        lseek(fd, 0, DFS_SEEK_SET);
////        
////        lseek(fd, 6*1024*1024, DFS_SEEK_CUR);
////        
////        close(fd);
////    }
//    
//    fd = open(SYS_CFG_FILE_PATH, O_RDONLY, 0);
//    if (fd < 0)
//    {
//            dfs_unmount(ELMFS_DATAFLASH_DIR);
//            mkfs("elm","flash0");
//            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
//            {
//                rt_kprintf("Flash File System initialzation failed again !!! \n");
//                while(1)
//                {
//                    rt_thread_delay(10);
//                }
//            }
//            
//    }    
//    else
//    {
//        close(fd);
//    }      
//    
//    dir = opendir(ROMFS_DIR);
//    if (dir == NULL)
//    {
//        if (mkdir( ROMFS_DIR, 0) != 0)
//        {
//            dfs_unmount("/");
//            mkfs("elm","flash0");
//            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
//            {
//                rt_kprintf("Flash File System initialzation failed again !!! \n");
//                while(1)
//                {
//                    rt_thread_delay(10);
//                }
//            }
//            
//            mkdir(ROMFS_DIR, 0);
//        }
//    }
//    closedir(dir);    

//    dir = opendir(SD_DIR);
//    if (dir == NULL)
//    {
//        if (mkdir( SD_DIR, 0) != 0)
//        {
//            dfs_unmount("/");
//            mkfs("elm","flash0");
//            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
//            {
//                rt_kprintf("Flash File System initialzation failed again !!! \n");
//                while(1)
//                {
//                    rt_thread_delay(10);
//                }
//            }
//            
//            mkdir(SD_DIR, 0);
//        }
//    }
//    closedir(dir);    

//    
//    dir = opendir(SYS_CFG_DIR);
//    if (dir == NULL)
//    {
//        mkdir( SYS_CFG_DIR, 0);
//    }
//    closedir(dir);

////    dir = opendir(BIN_PATH);
////    if (dir == NULL)
////    {
////        mkdir( BIN_PATH, 0);
////    }
////    closedir(dir);

//    dir = opendir(FW_FILE_DIR);
//    if (dir == NULL)
//    {
//        mkdir( FW_FILE_DIR, 0);
//    }
//    closedir(dir);

//    dir = opendir(DOWN_FILE_DIR);
//    if (dir == NULL)
//    {
//        mkdir( DOWN_FILE_DIR, 0);
//    }
//    closedir(dir);

//    fd = open( SYS_INIT_FILE_PATH, O_RDWR, 0);
//    if (fd >= 0)
//    {
//        close(fd);
//    }
//    else
//    {
//        fd = open( SYS_INIT_FILE_PATH, O_CREAT | O_WRONLY, 0);
//        
//        write( fd, sys_init_file_content, strlen(sys_init_file_content));
//        close(fd);
//    }
//    
//    // Song: patch, if system config file is broken, we should rm it. Later , it will be created automatic.
//    fd = open( SYS_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
//    if (fd >= 0)
//    {
//        lseek(fd, 0, DFS_SEEK_SET);
//        res = read(fd , &temp_char, 1);
//        if (res == 1)
//        {
//            // If file is broken, the data is all "FF".
//            if (temp_char != '{')
//            {
//                close(fd);
//                rm(SYS_CFG_FILE_PATH);
//                json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
//                rt_kprintf("System config file is repaired with sys_cfg_init_data !!! \n");
//            }
//            else
//            {
//                close(fd);
//            }
//        }
//        else
//        {
//            close(fd);
//            rm(SYS_CFG_FILE_PATH);
//            json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
//            rt_kprintf("System config file is rewrite with sys_cfg_init_data !!! \n");
//        }
//    }
//    else
//    {
//        json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
//        rt_kprintf("System config file is recreated with sys_cfg_init_data !!! \n");
//    }
//}




void rt_init_thread_entry(void* parameter)
{
    // Song: show the software version.
//    rt_kprintf("Software version: %s\n", SW_VER);
    
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif

    
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif    
    
#ifdef RT_USING_I2C
    //rt_i2c_core_init();
    rt_hw_i2c_init();
    
    //codec_hw_init("i2c1");
#endif

    
    rt_platform_init();
	
#ifdef RT_USING_DFS
#ifdef RT_USING_DFS_ELMFAT
//	////rt_thread_delay(10);
//	if (dfs_mount("sd0", ELMFS_SD_DIR, "elm", 0, 0) == 0)
//	{
//        rt_kprintf("SDCard File System initialized!\n");
//        
//        /* download resource from www.rt-thread.org */
//        if (0)
//        {
//            resource_download();
//        }

//	}
//	else
//	{
//		rt_kprintf("SDCard File System initialzation failed!\nFormat the SD card.\n");
//        mkfs("elm", "sd0");
//        if (dfs_mount("sd0", ELMFS_SD_DIR, "elm", 0, 0) != 0)
//        {
//            rt_kprintf("SDCard File System initialzation failed again !!! \n");
//            while(1)
//            {
//                rt_thread_delay(10);
//            }
//        }
//	}

//        if (0)
//        {
//            mkfs("elm","flash0");
//            rt_kprintf("mkfs flash0 .\n");
//            while(1);
//        }
//        
//        if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
//        {
//            rt_kprintf("SPI File System initialized!\n");

//        }
//        else
//        {
//            rt_kprintf("SPI File System init failed!\n");
//        }

        if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) == 0)
        {
            rt_kprintf("SPI File System initialized!\n");
        }
		else
		{
			rt_kprintf("SDCard File System initialzation failed!\nFormat the SD card.\n");
            mkfs("elm","flash0");
            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
            {
                rt_kprintf("SPI File System initialzation failed again !!! \n");
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
		}
        
        //system_fs_check();        
        

#endif // 	RT_USING_DFS_ELMFAT
	
	
#ifdef RT_USING_DFS_ROMFS
    if (dfs_mount(RT_NULL, ROMFS_DIR, "rom", 0, &romfs_root) == 0)
    {
        rt_kprintf("ROM File System initialized!\n");
    }
    else
    {
        rt_kprintf("ROM File System initialzation failed!\n");
    }
#endif // RT_USING_DFS_ROMFS   	
#endif // RT_USING_DFS   	

    
//    json_cfg_load();
//    json_cfg_wr_sys_cfg(1);


#ifdef RT_USING_LOGTRACE    
    memlog_init();
#endif // RT_USING_LOGTRACE
    
    //cpu_usage_init();

#ifdef RT_USING_INIT_RC    
    /* do initialization script file */
    ////do_init();
#endif // RT_USING_INIT_RC     
    
    sys_inited_flag = 1;
}


//void rt_APP_init_thread_entry(void* parameter)
//{

//    while(sys_board_type_checked == 0)
//    {
//        rt_thread_delay(1);
//    }
//    
//    if (sys_dev_type == dev_type_sample_ctrl)
//    {
//         //------- create sensor_sample thread
//         thread_sensor_sample = rt_thread_create("sensor",
//                                            rt_thread_entry_sensor_sample,
//                                            RT_NULL,
//                                            THREAD_SENSOR_SAMPLE_STACK_SIZE,
//                                            prio_sensor_sample,
//                                            10);
//        if (thread_sensor_sample != RT_NULL)
//        {        
//            rt_thread_startup(thread_sensor_sample);     
//        } 
//    }
//    else if (sys_dev_type == dev_type_simple_UITD)
//    {
//         //------- create fire_alarm thread
//         thread_fire_alarm = rt_thread_create("fire_alarm",
//                                            rt_thread_entry_fire_alarm,
//                                            RT_NULL,
//                                            THREAD_FIRE_ALARM_STACK_SIZE,
//                                            prio_fire_alarm,
//                                            10);
//        if (thread_fire_alarm != RT_NULL)
//        {        
//            rt_thread_startup(thread_fire_alarm);     
//        }
//        
//         //------- create fire_alarm_rx thread
//         thread_fire_alarm_rx = rt_thread_create("fire_alarm_rx",
//                                            rt_thread_entry_fire_alarm_rx,
//                                            RT_NULL,
//                                            THREAD_FIRE_ALARM_RX_STACK_SIZE,
//                                            prio_fire_alarm_rx,
//                                            10);
//        if (thread_fire_alarm_rx != RT_NULL)
//        {        
//            rt_thread_startup(thread_fire_alarm_rx);     
//        }    
//    }
//    else if (sys_dev_type == dev_type_unknown)
//    {
//        rt_kprintf("Unknown board type !!! \n");
//    }
//    
//}



int rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        4096, RT_THREAD_PRIORITY_MAX/3, 5);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    
//    tid = rt_thread_create("APP_init",
//        rt_APP_init_thread_entry, RT_NULL,
//        4096, RT_THREAD_PRIORITY_MAX/3 - 1, 10);

//    if (tid != RT_NULL)
//        rt_thread_startup(tid);
    
     //------- create sys_status thread
     thread_sys_status = rt_thread_create("sys_status",
                                        rt_thread_entry_sys_status,
                                        RT_NULL,
                                        THREAD_SYS_STATUS_STACK_SIZE,
                                        prio_sys_status,
                                        10);
    if (thread_sys_status != RT_NULL)
    {        
        rt_thread_startup(thread_sys_status);     
    }

     //------- create sys_IAP thread
     thread_sys_IAP = rt_thread_create("sys_IAP",
                                        rt_thread_entry_sys_IAP,
                                        RT_NULL,
                                        4096,prio_sys_IAP,5);
     rt_thread_startup(thread_sys_IAP);   
    
    
//    
//     //------- create UITD_main thread
//     thread_UITD_main = rt_thread_create("UITD_main",
//                                        rt_thread_entry_UITD_main,
//                                        RT_NULL,
//                                        THREAD_UITD_MAIN_STACK_SIZE,
//                                        prio_UITD_main,
//                                        20);
//    if (thread_UITD_main != RT_NULL)
//    {        
//        rt_thread_startup(thread_UITD_main);     
//    }

//     //------- create UITD_main_rec thread
//     thread_UITD_main_rec = rt_thread_create("UITD_rec",
//                                        rt_thread_entry_UITD_main_rec,
//                                        RT_NULL,
//                                        THREAD_UITD_MAIN_REC_STACK_SIZE,
//                                        prio_UITD_main_rec,
//                                        30);
//    if (thread_UITD_main_rec != RT_NULL)
//    {        
//        rt_thread_startup(thread_UITD_main_rec);     
//    }

//     //------- create GSM_MG301 thread
//     thread_GSM_MG301 = rt_thread_create("GSM_MG301",
//                                        rt_thread_entry_GSM_MG301,
//                                        RT_NULL,
//                                        THREAD_GSM_MG301_STACK_SIZE,
//                                        prio_GSM_MG301,
//                                        100);
//    if (thread_GSM_MG301 != RT_NULL)
//    {        
//        rt_thread_startup(thread_GSM_MG301);     
//    }
//    
//    
//    
//     //------- create GSM_rx thread
//     thread_GSM_rx = rt_thread_create("GSM_rx",
//                                        rt_thread_entry_GSM_rx,
//                                        RT_NULL,
//                                        THREAD_GSM_RX_STACK_SIZE,
//                                        prio_GSM_rx,
//                                        20);
//    if (thread_GSM_rx != RT_NULL)
//    {        
//        rt_thread_startup(thread_GSM_rx);     
//    }
//    
//    
    

    
     //------- create WIFI_ESP_07 thread
//     thread_WIFI_ESP_07 = rt_thread_create("WIFI_ESP_07",
//                                        rt_thread_entry_WIFI_ESP_07,
//                                        RT_NULL,
//                                        THREAD_WIFI_ESP_07_STACK_SIZE,
//                                        prio_WIFI_ESP_07,
//                                        10);
//    if (thread_WIFI_ESP_07 != RT_NULL)
//    {        
//        rt_thread_startup(thread_WIFI_ESP_07);     
//    }
    
    
    
     //------- create WIFI_rx thread
//     thread_WIFI_rx = rt_thread_create("WIFI_rx",
//                                        rt_thread_entry_WIFI_rx,
//                                        RT_NULL,
//                                        THREAD_WIFI_RX_STACK_SIZE,
//                                        prio_WIFI_rx,
//                                        10);
//    if (thread_WIFI_rx != RT_NULL)
//    {        
//        rt_thread_startup(thread_WIFI_rx);     
//    }
    
//    thread_COM_if = rt_thread_create("COM_if",
//                                   rt_thread_entry_COM_if, RT_NULL,
//                                   THREAD_COM_IF_STACK_SIZE, prio_COM_if, 50);
//    if (thread_COM_if != RT_NULL)
//        rt_thread_startup(thread_COM_if);
    
    return 0;
}






/*@}*/
