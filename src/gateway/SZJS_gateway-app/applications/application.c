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
#include "UITD_main.h"
#include "UITD_eth.h"
#include "UITD_BLE.h"
#include "UITD_WIFI.h"
#include "GSM_MG301.h"
#include "WIFI_ESP_07.h"
#include "data_trans.h"
#include "sys_status.h"
#include "sensor_sample.h"
#include "fire_alarm.h"
#include "controller.h"

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f2xx_eth.h"
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

#include "msd.h"
#include "rtc.h"    
#include "platform.h"
#include "UITD.h"
#include "sys_misc.h"
#include "transparent.h"
#include "pro_ctrl.h"
#include "pro_UITD.h"

//int testing_fd = 0;

extern void rt_platform_init(void);

uint8_t testing_TF = 0;

int mode_thread_start(s_mode_thread *thread);


void system_fs_check(void)
{
    int fd;
    int res = 0;
    DIR *dir;
    char temp_char = 0;

    
    // Song: just a file expend test.
//    rm("/test.txt");
//    fd = open("/test.txt", O_CREAT | O_RDWR | O_TRUNC , 0);
//    if (fd >= 0)
//    {
//        lseek(fd, 0, DFS_SEEK_SET);
//        
//        lseek(fd, 6*1024*1024, DFS_SEEK_CUR);
//        
//        close(fd);
//    }
    
    fd = open(SYS_CFG_FILE_PATH, O_RDONLY, 0);
    if (fd < 0)
    {
            dfs_unmount(ELMFS_DATAFLASH_DIR);
            mkfs("elm","flash0");
            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Flash File System initialzation failed again !!! \n"));
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
    }    
    else
    {
        close(fd);
    }      
    
    dir = opendir(ROMFS_DIR);
    if (dir == NULL)
    {
        if (mkdir( ROMFS_DIR, 0) != 0)
        {
            dfs_unmount("/");
            mkfs("elm","flash0");
            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
            {
                SYS_log(SYS_DEBUG_ERROR, ("Flash File System initialzation failed again !!! \n"));
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
            
            mkdir(ROMFS_DIR, 0);
        }
    }
    closedir(dir);    

    dir = opendir(SD_DIR);
    if (dir == NULL)
    {
        if (mkdir( SD_DIR, 0) != 0)
        {
            rt_kprintf("Make /SD dir failed \n");
//            dfs_unmount(SD_DIR);
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
        }
    }
    closedir(dir);    

    
    dir = opendir(SYS_CFG_DIR);
    if (dir == NULL)
    {
        mkdir( SYS_CFG_DIR, 0);
    }
    closedir(dir);

    dir = opendir(SCRIPT_DIR);
    if (dir == NULL)
    {
        mkdir( SCRIPT_DIR, 0);
    }
    closedir(dir);    
    
    dir = opendir(MODULE_DIR);
    if (dir == NULL)
    {
        mkdir( MODULE_DIR, 0);
    }
    closedir(dir);     

    dir = opendir(MODULE_LIB_DIR);
    if (dir == NULL)
    {
        mkdir( MODULE_LIB_DIR, 0);
    }
    closedir(dir);      
//    dir = opendir(BIN_PATH);
//    if (dir == NULL)
//    {
//        mkdir( BIN_PATH, 0);
//    }
//    closedir(dir);

    dir = opendir(FW_FILE_DIR);
    if (dir == NULL)
    {
        mkdir( FW_FILE_DIR, 0);
    }
    closedir(dir);

    dir = opendir(DOWN_FILE_DIR);
    if (dir == NULL)
    {
        mkdir( DOWN_FILE_DIR, 0);
    }
    closedir(dir);

    fd = open( SYS_INIT_FILE_PATH, O_RDWR, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        fd = open( SYS_INIT_FILE_PATH, O_CREAT | O_WRONLY, 0);
        
        write( fd, sys_init_file_content, strlen(sys_init_file_content));
        close(fd);
    }
    
    // Song: patch, if system config file is broken, we should rm it. Later , it will be created automatic.
    fd = open( SYS_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        lseek(fd, 0, DFS_SEEK_SET);
        res = read(fd , &temp_char, 1);
        if (res == 1)
        {
            // If file is broken, the data is all "FF".
            if (temp_char != '{')
            {
                close(fd);
                rm(SYS_CFG_FILE_PATH);

                fd = open( SYS_CFG_FILE_PATH, O_RDONLY, 0);
                if (fd < 0)
                {
                    // Can not remove the broken sys.cfg file. You have to mkfs() and reboot.
                    dfs_unmount("/");
                    mkfs("elm","flash0");
                    dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0);
                    json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
                    json_cfg_wr_sys_mode(sys_mode_normal);
                    rt_thread_delay(3*RT_TICK_PER_SECOND);
                    sys_reboot();
                }
                else
                {
                    close(fd);
                }

                json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
                json_cfg_wr_sys_mode(sys_mode_normal);
                rt_kprintf("System config file is repaired with sys_cfg_init_data !!! \n");
            }
            else
            {
                close(fd);
            }
        }
        else
        {
            close(fd);
            rm(SYS_CFG_FILE_PATH);
            
            fd = open( SYS_CFG_FILE_PATH, O_RDONLY, 0);
            if (fd < 0)
            {
                // Can not remove the broken sys.cfg file. You have to mkfs() and reboot.
                mkfs("elm","flash0");
                rt_thread_delay(3*RT_TICK_PER_SECOND);
                sys_reboot();
            }
            else
            {
                close(fd);
            }
            
            json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
            json_cfg_wr_sys_mode(sys_mode_normal);
            rt_kprintf("System config file is rewrite with sys_cfg_init_data !!! \n");
        }
    }
    else
    {
        json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
        json_cfg_wr_sys_mode(sys_mode_normal);
        rt_kprintf("System config file is recreated with sys_cfg_init_data !!! \n");
    }

    fd = open( MODE_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_mode(MODE_CFG_FILE_PATH);
        rt_kprintf("mode config file is recreated with mode_cfg_init_data !!! \n");
    } 

    fd = open( TRANS_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_transparent(TRANS_CFG_FILE_PATH, &transparent_cfg_init_data);
        rt_kprintf("transparent config file is recreated with transparent_cfg_init_data !!! \n");
    } 

    fd = open( PRO_UITD_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_PRO_UITD(PRO_UITD_CFG_FILE_PATH, &PRO_UITD_cfg_init_data);
        rt_kprintf("PRO_UITD config file is recreated with PRO_UITD_cfg_init_data !!! \n");
    }   
    
    fd = open( PRO_CTRL_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_PRO_CTRL(PRO_CTRL_CFG_FILE_PATH, &PRO_CTRL_cfg_init_data);
        rt_kprintf("PRO_CTRL config file is recreated with PRO_CTRL_cfg_init_data !!! \n");
    }    
    
    fd = open( AP01_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_AP01(AP01_CFG_FILE_PATH, &AP01_cfg_init_data);
        rt_kprintf("AP01 config file is recreated with AP01_cfg_init_data !!! \n");
    }    

//    fd = open( AP02_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
//    if (fd >= 0)
//    {
//        close(fd);
//    }
//    else
//    {
//        json_cfg_create_AP02(AP02_CFG_FILE_PATH, &AP02_cfg_init_data);
//        rt_kprintf("AP02 config file is recreated with AP02_cfg_init_data !!! \n");
//    }    

    fd = open( FA_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_FA(FA_CFG_FILE_PATH, &FA_cfg_init_data);
        rt_kprintf("FA config file is recreated with FA_cfg_init_data !!! \n");
    }   
    
    fd = open( ETH_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_eth(ETH_CFG_FILE_PATH, &eth_cfg_init_data);
        rt_kprintf("eth config file is recreated with eth_cfg_init_data !!! \n");
    }    
    
    fd = open( WIFI_CFG_FILE_PATH, O_RDONLY | O_BINARY, 0);
    if (fd >= 0)
    {
        close(fd);
    }
    else
    {
        json_cfg_create_WIFI(WIFI_CFG_FILE_PATH, &WIFI_cfg_init_data);
        rt_kprintf("WIFI config file is recreated with WIFI_cfg_init_data !!! \n");
    }    
    
}


int file_zero_clean_up(uint8_t *filename)
{
    int fd ;
    struct stat stat;
    
    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        rt_err_t err = rt_get_errno();
        rt_kprintf("Open file %s failed : %d \n", filename, err);
        //return fd;
    }
    else
    {
        fstat(fd, &stat);
        if (stat.st_size == 0)
        {
            close(fd);  
            rm(filename);
            rt_kprintf("File %s size is zero, remove it .\n", filename);
        }
        else
        {
            close(fd);  
        }
    }
}

int system_fs_clean_up(void)
{
    int fd ;
    struct stat stat;
    
    // Song: just remove these trash files.
    rm("/firmware.bin");


    // Song: check if these file size is 0, remove them.
    file_zero_clean_up("/fw/firmware.bin");
    
    return 0;
}

int sys_init_cfg(void)
{
    rm(SYS_INIT_FILE_PATH);
    rm(SYS_CFG_FILE_PATH);
    rm(AP01_CFG_FILE_PATH);
    rm(AP02_CFG_FILE_PATH);
    rm(ETH_CFG_FILE_PATH);
    
    system_fs_check();
    return 0;
}

int sys_ID_read(uint8_t *buf, uint32_t start, uint32_t len)
{
    uint8_t *p = (uint8_t *)SYS_ID_ADDRESS;
    int i = 0;
    
    if (len > 512) return -1;
    
    for (i=0;i<len;i++)
    {
        buf[i] = *(p+start+i);
    }
    
    return 0;
}


void rt_init_thread_entry(void* parameter)
{
    int i = 0;
    uint8_t SN_buf[SYS_SN_LEN+1] = {0};
    uint64_t SN_temp = 0;
//    uint8_t file_path_buf[FILE_PATH_LEN_MAX] = {0};
//    uint8_t test_file[] = "FW_GprsIotAP_1_10_00000003.bin";
//    uint8_t test_ver_main = 1;
//    uint8_t test_ver_sub = 9;
//    s_FW_support_dev test_dev = {0};
//    
//    test_dev.dev_simple_UITD = 1;
//    
//    UITD_firmware_parse(test_file, test_ver_main, test_ver_sub, &test_dev);
    
    // Song: show the software version.
//    rt_kprintf("Software version: %s\n", SW_VER);
    sys_ID_read(g_UITD_addr, 0, sizeof(g_UITD_addr));   //从OTP中 读取网关唯一地址
    
    #if ASSIGN_BOARD_ID		//初始化设备地址
    static uint64_t assign_ID = 102163300024L;
    rt_memcpy(g_UITD_addr, &assign_ID, sizeof(g_UITD_addr));
    #endif // ASSIGN_BOARD_ID
    
    rt_kprintf("UITD_ID: %02X %02X %02X %02X %02X %02X \n ", 
                g_UITD_addr[0], g_UITD_addr[1], g_UITD_addr[2], g_UITD_addr[3], g_UITD_addr[4], g_UITD_addr[5]);
                
    g_UITD_SN = 0;
    rt_memcpy(&g_UITD_SN, g_UITD_addr, sizeof(g_UITD_addr));		//初始化网关地址
    if (g_UITD_SN > SYS_SN_MAX)
    {
        rt_kprintf("SN too large , error !!!\n");
    }
    else
    {
        SN_temp = g_UITD_SN;
        for (i=0;i<SYS_SN_LEN;i++)
        {
            SN_buf[SYS_SN_LEN-1-i] = SN_temp%10 + '0';
            SN_temp /= 10;
        }
        rt_kprintf("SN : %s\n", SN_buf);
    }
    
    
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif
    
    
			
		
		//网关主板类型 判断为 0 通过外部ADC 采集确定主板类型
    #if ASSIGN_MOTHER_BOARD_TYPE
        #if ASSIGN_MOTHER_BOARD_TYPE == 1
        sys_config.mother_type = mother_type_GPRS_1_2;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 2
        sys_config.mother_type = mother_type_GPRS;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 3
        sys_config.mother_type = mother_type_ETH;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 4
        sys_config.mother_type = mother_type_GPRS_ETH;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 5
        sys_config.mother_type = mother_type_IOT_PRO;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 6
        sys_config.mother_type = mother_type_EC_AP;
        #elif ASSIGN_MOTHER_BOARD_TYPE == 7
        sys_config.mother_type = mother_type_EC_TERM;
        #endif
    #else
    ADC_mother_board_check(&sys_config.mother_type);
    #endif // ASSIGN_MOTHER_BOARD_TYPE
    
    ADC_ext_board_module_check(&sys_config.dev_type, &sys_config.module_type);       //采样扩展板的ADC判断 主板类型确定应用类型

    sys_config.board_type_checked = 1;			//标志主板类型已经确定
    
    if ((sys_config.dev_type != dev_type_sample_ctrl) && (sys_config.dev_type != dev_type_controller))		
    {
        ADC_power_battery_init();						//初始化锂电池接口检查
    }
    
    // If module_type is dev_type_mode_test, enter testing mode by force !
    if (sys_config.module_type == dev_type_mode_test)
    {
        sys_config.sys_mode = sys_mode_testing;
        //SYS_log( SYS_DEBUG_INFO, ("\n  Enter testing mode !!! \n\n"));
    }
    
    // If use extern module BLE WIFI WIFI_BLE, should not init the finsh. Disable the USART1 as finsh console.
		//如果使用外部模块的无线wifi_ble, 不能初始化完成，禁用USART1为了完成控制台
    if (sys_config.module_type == dev_type_module_BLE)    
    {
#ifdef  RT_USING_CONSOLE
        rt_console_close_device();
#endif        
        module_BLE_init(CONSOLE_DEVICE);
        sys_config.sys_mode = sys_mode_config;
    }
    else if (sys_config.module_type == dev_type_module_WIFI)
    {
#ifdef  RT_USING_CONSOLE
        rt_console_close_device();
#endif        
        //module_WIFI_init(CONSOLE_DEVICE);
        sys_config.sys_mode = sys_mode_config;
    }
    else if (sys_config.module_type == dev_type_module_WIFI_BLE)
    {
#ifdef  RT_USING_CONSOLE
        rt_console_close_device();
#endif        
        //module_WIFI_BLE_init(CONSOLE_DEVICE);
        sys_config.sys_mode = sys_mode_config;
    }
    else if (sys_config.module_type == dev_type_mode_debug)
    {
        // Debug mode , not need close the console output.
				//调试模式，不需要关闭控制台输出
        
        finsh_system_init();
    }
    else
    {
#if SYS_CONSOLE_OUTPUT_DISABLE
        
        // mother_type_GPRS_1_2 Not support Debug module in case of history.
        if (sys_config.mother_type == mother_type_GPRS_1_2)
        {
            finsh_system_init();
        }
        else
        {
            finsh_system_init();
            rt_kprintf("Please login ...\n");
            rt_thread_delay(RT_TICK_PER_SECOND * 3);

            if (console_output_enable == 0)
            {
                #ifdef  RT_USING_CONSOLE
                rt_console_close_device();
                #endif        
            }
        }
#else        
        finsh_system_init();
#endif // SYS_CONSOLE_DEFAULT_DISABLE        
        
    }    
    
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#else
   
//    if (sys_config.mother_type == mother_type_GPRS_1_2)
//    {
//        
//    }
//    else if (sys_config.mother_type == mother_type_GPRS)
//    {
//        
//    }
//    else if ((sys_config.mother_type == mother_type_ETH) 
//            || (sys_config.mother_type == mother_type_GPRS_ETH) 
//            || (sys_config.mother_type == mother_type_IOT_PRO))
//    {
//        if (sys_config.sys_mode != sys_mode_config)
//        { // Song: TODO: If not used ethernet, we should not initialize the eth.
//            eth_system_device_init();
//            lwip_system_init();
//        }
//    }
    
    
    log_trace_init();					//日志初始化
    rt_system_module_init();
    dfs_init();								//文件系统初始化
    rt_i2c_core_init();
    elm_init();
    dfs_romfs_init();					//文件系统初始化
    
#endif    
    
    
    
#ifdef RT_USING_I2C
    //rt_i2c_core_init();
    rt_hw_i2c_init();
    
    //codec_hw_init("i2c1");
#endif

    rt_platform_init();			 //平台初始化

    
    #if 0  // Song: just for testing.
    VK32xx_init("spi20");
    #endif 


    if (sys_config.dev_type == DEV_TYPE_IOT_PRO_UITD)
    {
        VK32xx_init("spi20");
    }


#ifdef RT_USING_DFS
#ifdef RT_USING_DFS_ELMFAT


        #if 0 // Just for formate system in case.
        sys_config.module_type = dev_type_mode_format;
        #endif 

        if (sys_config.module_type == dev_type_mode_format)
        {
            mkfs("elm","flash0");
            SYS_log(SYS_DEBUG_ERROR, ("mkfs flash0 finish, Please reset the device.\n"));
            while(1);
        }
        

        if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) == 0)
        {
            rt_kprintf("SPI File System initialized!\n");
        }
		else
		{
			rt_kprintf("Dataflash File System initialzation failed!\nFormat the Dataflash.\n");
            mkfs("elm","flash0");
            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
            {
                SYS_log(SYS_DEBUG_ERROR, ("SPI File System initialzation failed again !!! \n"));
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
		}

        
        rt_thread_delay(10);
		/* mount FAT file system on SD card */
        msd_init("sd0", "spi11");
        if (dfs_mount("sd0", SD_DIR, "elm", 0, 0) == 0)
        {
            rt_kprintf("SDCard File System initialized!\n");
            testing_TF = 1;
        }
		else
		{
			rt_kprintf("SDCard File System initialization failed!\n");

		}

        if (sys_config.module_type == dev_type_mode_reconfig)
        {
            sys_init_cfg();
            SYS_log(SYS_DEBUG_ERROR, ("Init configuration finished, Please reset the device.\n"));
            while(1);
        }

        system_fs_check(); 
        system_fs_clean_up();

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

    
#if PATCH_FW_UPGRADE_FLAG
    if (RTC_ReadBackupRegister(SYS_RTC_FW_UPGRADE_ADDR) == 0x5A5A)
    {
        RTC_WaitForSynchro();
        RTC_WriteBackupRegister(SYS_RTC_FW_UPGRADE_ADDR, 0x0000);    

        touch("/fw/upgrade.flg", 1);
        sys_reboot();
        
        while(1);
    }
#endif // PATCH_FW_UPGRADE_FLAG
    
    
    /* LwIP Initialization */    
#ifdef RT_USING_LWIP

#endif	    

    
    json_cfg_load();								//json配置加载
    json_cfg_wr_sys_cfg(1);					//写系统配置

    if ((sys_config.mother_type == mother_type_ETH) ||
        (sys_config.mother_type == mother_type_GPRS_ETH) || 
        (sys_config.mother_type == mother_type_IOT_PRO))
    {
        json_cfg_load_WIFI();				//wifi参数配置读取
    }

    
    
#ifdef RT_USING_LOGTRACE    
    //memlog_init();
    if (sys_config.log_file_en)
    {
        // Only if free space more than 1000KB, enable the log trace.
        if (disk_free(ELMFS_DATAFLASH_DIR) > 1000*1000)
        {
            // path must be end with '/', ie. "/", "/xxxx/" .
            if (sys_config.log_file_path[strlen(sys_config.log_file_path)-1] != '/')
            {
                SYS_log(SYS_DEBUG_ERROR, ("Log file path error : %s , use default path\n", sys_config.log_file_path));
                rt_memcpy(sys_config.log_file_path, SYS_LOG_FILE_PATH, sizeof(SYS_LOG_FILE_PATH));
            }
            else
            {
                //rt_memcpy(file_path_buf, sys_config.log_file_path, strlen(sys_config.log_file_path)+1);
            }                
            
            strcat(sys_config.log_file_path, SYS_LOG_INFO_NAME);
            
            log_trace_set_file(sys_config.log_file_path);
            ////log_trace_set_file("/log_tmp.txt");
            log_trace("\n\n\nLog start ===============================\n\n");
            log_trace_list_date();
        }
    }
#endif // RT_USING_LOGTRACE 
    
    cpu_usage_init();

#ifdef RT_USING_INIT_RC    
    /* do initialization script file */
    do_init();
#endif // RT_USING_INIT_RC     
    
    if ((sys_config.sys_mode == sys_mode_testing) || (sys_config.sys_mode == sys_mode_tested))
    {
        SYS_log( SYS_DEBUG_INFO, ("\n  Enter testing mode !!! \n\n"));
        sys_status_set(&sys_ctrl, SYS_state_testing);
        
        if (testing_TF)
        {
            sys_config.test_flag.TF = 1;
        }
    }
    
    sys_config.sys_inited_flag = 1;
    
}

void  rt_UITD_BLE_init(void *parameter)
{

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(1);
    }
    
    
    {
        //------- create UITD_BLE thread
         thread_UITD_BLE = rt_thread_create("UITD_BLE",
                                            rt_thread_entry_UITD_BLE,
                                            RT_NULL,
                                            THREAD_UITD_BLE_STACK_SIZE,
                                            prio_UITD_BLE,
                                            20);
        if (thread_UITD_BLE != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_BLE);     
        }    
        
        //------- create UITD_BLE_rx thread
         thread_UITD_BLE_rx = rt_thread_create("UITD_BLE_rx",
                                            rt_thread_entry_UITD_BLE_rx,
                                            RT_NULL,
                                            THREAD_UITD_BLE_RX_STACK_SIZE,
                                            prio_UITD_BLE_rx,
                                            20);
        if (thread_UITD_BLE_rx != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_BLE_rx);     
        }                   
    }
    
    //------- create UITD_BLE_rec thread
    thread_UITD_BLE_rec = rt_thread_create("UITD_BLE_rec",
                                            rt_thread_entry_UITD_BLE_rec,
                                            RT_NULL,
                                            THREAD_UITD_BLE_REC_STACK_SIZE,
                                            prio_UITD_BLE_rec,
                                            30);
    if (thread_UITD_BLE_rec != RT_NULL)
    {        
        rt_thread_startup(thread_UITD_BLE_rec);     
    }    
    
}

void  rt_UITD_GPRS_init(void *parameter)
{

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(1);
    }
    
    // Just support GPRS, no need load the mode.cfg
		// 只支持GPRS类型，无需加载mode.cfg配置文件
    mode_cfg.mode = MODE_CFG_SINGLE_WAY;
    mode_cfg.GPRS = 1;
    mode_cfg.ETHERNET = 0;
    mode_cfg.WIFI = 0;
    
        {
            mode_cfg.thread.GPRS_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.GPRS_XFZD = 1;
            }
        }
       
    mode_thread_start(&mode_cfg.thread);  //不同模式处理线程 选择打开主板支持的通讯模式外设
    
}

void  rt_UITD_eth_init(void *parameter)
{

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(1);
    }
    
#if SYS_DEBUG_WIFI_TEST_EN
    
    mode_cfg.mode = MODE_CFG_SINGLE_WAY;
    mode_cfg.thread.WIFI_com = 1;
    mode_cfg.thread.WIFI_SZJS = 1;
    mode_cfg.thread.WIFI_XFZD = 1;
    
#else 
        
    mode_cfg.GPRS = 0;
    if (mode_cfg.mode == MODE_CFG_SINGLE_WAY)
    {
        if (mode_cfg.ETHERNET)
        {
            mode_cfg.thread.ETH_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.ETH_XFZD = 1;
            }
        }
        else if (mode_cfg.WIFI)
        {
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
                mode_cfg.thread.WIFI_XFZD = 1;
            }
        }
        
    }
    else if (mode_cfg.mode == MODE_CFG_MULT_WAY)
    {
        if ((mode_cfg.ETHERNET == 1) && (mode_cfg.WIFI == 2))
        {
            mode_cfg.thread.ETH_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_XFZD = 1;
            }
        }
        else if ((mode_cfg.WIFI == 1) && (mode_cfg.ETHERNET == 2))
        {
            mode_cfg.thread.ETH_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
                mode_cfg.thread.ETH_XFZD = 1;
            }
        }
    }
#endif // SYS_DEBUG_WIFI_TEST_EN    
       
    mode_thread_start(&mode_cfg.thread);		//不同模式处理线程 选择打开主板支持的通讯模式外设
}


void  rt_UITD_GPRS_ETH_init(void *parameter)
{

    while(sys_config.sys_inited_flag == 0)
    {
        rt_thread_delay(1);
    }
    
#if SYS_DEBUG_WIFI_TEST_EN
    
    mode_cfg.mode = MODE_CFG_SINGLE_WAY;
    mode_cfg.thread.WIFI_com = 1;
    mode_cfg.thread.WIFI_SZJS = 1;
    mode_cfg.thread.WIFI_XFZD = 1;
    
#else 
        
    if (mode_cfg.mode == MODE_CFG_SINGLE_WAY)
    {
        if (mode_cfg.GPRS)
        {
            mode_cfg.thread.GPRS_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.GPRS_XFZD = 1;
            }
        }
        else if (mode_cfg.ETHERNET)
        {
            mode_cfg.thread.ETH_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.ETH_XFZD = 1;
            }
        }
        else if (mode_cfg.WIFI)
        {
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
                mode_cfg.thread.WIFI_XFZD = 1;
            }
        }
        
    }
    else if (mode_cfg.mode == MODE_CFG_MULT_WAY)
    {
        if ((mode_cfg.GPRS == 1) && (mode_cfg.ETHERNET == 2))
        {
            mode_cfg.thread.GPRS_com = 1;
            mode_cfg.thread.ETH_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.ETH_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.ETH_XFZD = 1;
            }
        }
        else if ((mode_cfg.ETHERNET == 1) && (mode_cfg.GPRS == 2))
        {
            mode_cfg.thread.GPRS_com = 1;
            mode_cfg.thread.ETH_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.ETH_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.GPRS_XFZD = 1;
            }
        }
        else if ((mode_cfg.ETHERNET == 1) && (mode_cfg.WIFI == 2))
        {
            mode_cfg.thread.ETH_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_XFZD = 1;
            }
        }
        else if ((mode_cfg.WIFI == 1) && (mode_cfg.ETHERNET == 2))
        {
            mode_cfg.thread.ETH_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.ETH_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
                mode_cfg.thread.ETH_XFZD = 1;
            }
        }
        else if ((mode_cfg.GPRS == 1) && (mode_cfg.WIFI == 2))
        {
            mode_cfg.thread.GPRS_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.WIFI_XFZD = 1;
            }
        }
        else if ((mode_cfg.WIFI == 1) && (mode_cfg.GPRS == 2))
        {
            mode_cfg.thread.GPRS_com = 1;
            mode_cfg.thread.WIFI_com = 1;
            if (sys_config.server_num == 1)
            {
                mode_cfg.thread.GPRS_SZJS = 1;
                mode_cfg.thread.WIFI_SZJS = 1;
            }
            else if (sys_config.server_num == 2)
            {
                mode_cfg.thread.WIFI_SZJS = 1;
                mode_cfg.thread.GPRS_XFZD = 1;
            }
        }        
        
    }
    
#endif // SYS_DEBUG_WIFI_TEST_EN    
       
    mode_thread_start(&mode_cfg.thread);			//不同模式处理线程 选择打开主板支持的通讯模式外设

}

int mode_thread_start(s_mode_thread *thread)			//不同模式处理线程
{
    
    if (thread->GPRS_com)
    {
         //------- create UITD_main_rec thread		//GPRS接收数据处理线程
         thread_UITD_GPRS_rec = rt_thread_create("UITD_GPRS_rec",
                                            rt_thread_entry_UITD_GPRS_rec,
                                            RT_NULL,
                                            THREAD_UITD_GPRS_REC_STACK_SIZE,
                                            prio_UITD_GPRS_rec,
                                            30);
        if (thread_UITD_GPRS_rec != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_GPRS_rec);     
        }
        
        //------- create GSM_MG301 thread   // GSM_MG301 控制处理理线程
         thread_GSM_MG301 = rt_thread_create("GSM_MG301",
                                            rt_thread_entry_GSM_MG301,
                                            RT_NULL,
                                            THREAD_GSM_MG301_STACK_SIZE,
                                            prio_GSM_MG301,
                                            100);
        if (thread_GSM_MG301 != RT_NULL)
        {        
            rt_thread_startup(thread_GSM_MG301);     
        }
        
         //------- create GSM_rx thread			//GSM_MG301 //接收处理线程
         thread_GSM_rx = rt_thread_create("GSM_rx",
                                            rt_thread_entry_GSM_rx,
                                            RT_NULL,
                                            THREAD_GSM_RX_STACK_SIZE,
                                            prio_GSM_rx,
                                            20);
        if (thread_GSM_rx != RT_NULL)
        {        
            rt_thread_startup(thread_GSM_rx);     
        }
    }
    
    if (thread->ETH_com)
    {
        
        // Initialize the eth.
        eth_system_device_init();
        lwip_system_init();
        
        mco_config();
        json_cfg_load_eth();
        
        /* initialize eth interface */
        rt_hw_stm32_eth_init();
        rt_kprintf("TCP/IP initialized!\n");
        
        sys_config.eth_inited_flag = 1;


        //------- create UITD_eth_rec thread  //以太网数据接收处理线程
        thread_UITD_eth_rec = rt_thread_create("UITD_eth_rec",
                                            rt_thread_entry_UITD_eth_rec,
                                            RT_NULL,
                                            THREAD_UITD_ETH_REC_STACK_SIZE,
                                            prio_UITD_eth_rec,
                                            30);
        if (thread_UITD_eth_rec != RT_NULL)
        {
            rt_thread_startup(thread_UITD_eth_rec);     
        }
    }

    if (thread->WIFI_com)
    {
         //------- create WIFI_ESP_07 thread		//wifi 模块对接处理线程
         thread_WIFI_ESP_07 = rt_thread_create("WIFI_ESP07",
                                            rt_thread_entry_WIFI_ESP_07,
                                            RT_NULL,
                                            THREAD_WIFI_ESP_07_STACK_SIZE,
                                            prio_WIFI_ESP_07,
                                            20);
        if (thread_WIFI_ESP_07 != RT_NULL)
        {
            rt_thread_startup(thread_WIFI_ESP_07);     
        }

         //------- create WIFI_rx thread			//wifi 模块接收处理线程
         thread_WIFI_rx = rt_thread_create("WIFI_rx",
                                            rt_thread_entry_WIFI_rx,
                                            RT_NULL,
                                            THREAD_WIFI_RX_STACK_SIZE,
                                            prio_WIFI_ESP_07,
                                            20);
        if (thread_WIFI_rx != RT_NULL)
        {        
            rt_thread_startup(thread_WIFI_rx);     
        }
    
         //------- create UITD_WIFI_rec thread		//wifi 数据接收处理线程
         thread_UITD_WIFI_rec = rt_thread_create("UITD_WIFI_rec",
                                            rt_thread_entry_UITD_WIFI_rec,
                                            RT_NULL,
                                            THREAD_UITD_WIFI_REC_STACK_SIZE,
                                            prio_UITD_WIFI_rec,
                                            30);
        if (thread_UITD_WIFI_rec != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_WIFI_rec);     
        }
    }

    if (thread->GPRS_SZJS)
    {
        //------- create UITD_main thread
         thread_UITD_main = rt_thread_create("UITD_main",
                                            rt_thread_entry_UITD_main,
                                            RT_NULL,
                                            THREAD_UITD_MAIN_STACK_SIZE,
                                            prio_UITD_main,
                                            20);
        if (thread_UITD_main != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_main);     
        }              
    }

    if (thread->GPRS_XFZD)
    {
        //------- create UITD_2_main thread
        thread_UITD_2_main = rt_thread_create("UITD_2_main",
                                            rt_thread_entry_UITD_2_main,
                                            RT_NULL,
                                            THREAD_UITD_2_MAIN_STACK_SIZE,
                                            prio_UITD_2_main,
                                            20);
        if (thread_UITD_2_main != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_2_main);     
        }
    }

    if (thread->ETH_SZJS)
    {
        //------- create UITD_eth thread
        thread_UITD_eth = rt_thread_create("UITD_eth",
                                                rt_thread_entry_UITD_eth,
                                                RT_NULL,
                                                THREAD_UITD_ETH_STACK_SIZE,
                                                prio_UITD_eth,
                                                20);
        if (thread_UITD_eth != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_eth);     
        }

        //------- create eth_rx thread
        thread_eth_rx = rt_thread_create("eth_rx",
                                            rt_thread_entry_eth_rx,
                                            RT_NULL,
                                            THREAD_ETH_RX_STACK_SIZE,
                                            prio_eth_rx,
                                            20);
        if (thread_eth_rx != RT_NULL)
        {        
            rt_thread_startup(thread_eth_rx);     
        }
    }

    if (thread->ETH_XFZD)
    {
        //------- create UITD_eth_2 thread
        thread_UITD_eth_2 = rt_thread_create("UITD_eth_2",
                                                rt_thread_entry_UITD_eth_2,
                                                RT_NULL,
                                                THREAD_UITD_ETH_2_STACK_SIZE,
                                                prio_UITD_eth_2,
                                                20+10);
        if (thread_UITD_eth_2 != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_eth_2);     
        }         
            

        //------- create eth_rx_2 thread
        thread_eth_rx_2 = rt_thread_create("eth_rx_2",
                                                rt_thread_entry_eth_rx_2,
                                                RT_NULL,
                                                THREAD_ETH_RX_2_STACK_SIZE,
                                                prio_eth_rx_2,
                                                20+10);
        if (thread_eth_rx_2 != RT_NULL)
        {        
            rt_thread_startup(thread_eth_rx_2);     
        }
    }

    if (thread->WIFI_SZJS)
    {
        //------- create UITD_WIFI thread
         thread_UITD_WIFI = rt_thread_create("UITD_WIFI",
                                            rt_thread_entry_UITD_WIFI,
                                            RT_NULL,
                                            THREAD_UITD_WIFI_STACK_SIZE,
                                            prio_UITD_WIFI,
                                            20);
        if (thread_UITD_WIFI != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_WIFI);     
        }              
        
    }

    if (thread->WIFI_XFZD)
    {
        //------- create UITD_2_WIFI thread
         thread_UITD_2_WIFI = rt_thread_create("UITD_2_WIFI",
                                            rt_thread_entry_UITD_2_WIFI,
                                            RT_NULL,
                                            THREAD_UITD_2_WIFI_STACK_SIZE,
                                            prio_UITD_2_WIFI,
                                            60);
        if (thread_UITD_2_WIFI != RT_NULL)
        {        
            rt_thread_startup(thread_UITD_2_WIFI);     
        }
    }

    return 0;
}



void rt_COM_init_thread_entry(void *parameter)
{
    rt_thread_t tid;
    
//    while(sys_config.board_type_checked == 0)
//    {
//        rt_thread_delay(1);
//    }
    
    while(sys_config.sys_inited_flag == 0)				//等待系统完成
    {
        rt_thread_delay(2);
    }
    
    if (sys_config.sys_mode == sys_mode_testing)				 //测试模式
    {
        if ((sys_config.mother_type == mother_type_GPRS_1_2) ||
            (sys_config.mother_type == mother_type_GPRS))
        {
            mode_cfg.mode = MODE_CFG_SINGLE_WAY;
            mode_cfg.GPRS = 1;
            mode_cfg.ETHERNET = 0;
            mode_cfg.WIFI = 0;
            sys_config.server_num = 1;
        }
        else if (sys_config.mother_type == mother_type_ETH)
        {
            mode_cfg.mode = MODE_CFG_MULT_WAY;
            mode_cfg.GPRS = 0;
            mode_cfg.ETHERNET = 1;
            mode_cfg.WIFI = 2;
            sys_config.server_num = 2;
            
            strcpy(WIFI_cfg.SSID, WIFI_SSID_TEST);
            strcpy(WIFI_cfg.PW, WIFI_PW_TEST);
            
        }
        else if ((sys_config.mother_type == mother_type_GPRS_ETH)
                 || (sys_config.mother_type == mother_type_IOT_PRO)) // For now, not support test 3 communications one time, just test GPRS and ETH.
        {
            mode_cfg.mode = MODE_CFG_MULT_WAY;
            mode_cfg.GPRS = 1;
            mode_cfg.ETHERNET = 2;
            mode_cfg.WIFI = 0;
            sys_config.server_num = 2;

            strcpy(WIFI_cfg.SSID, WIFI_SSID_TEST);
            strcpy(WIFI_cfg.PW, WIFI_PW_TEST);
        }
        
    }
    else
    {
        json_cfg_load_mode();																//读取系统参数配置文件 加载配置参数
    }
    
    if (sys_config.module_type == dev_type_module_BLE)			//配置了蓝牙模块
    {
         //------- create UITD_BLE_init thread
         tid = rt_thread_create("UITD_BLE_init",
                                            rt_UITD_BLE_init,
                                            RT_NULL,
                                            1024,
                                            RT_THREAD_PRIORITY_MAX/3,
                                            5);
        if (tid != RT_NULL)
        {        
            rt_thread_startup(tid);     
        }
        
        return ;
    }

    if ((sys_config.mother_type == mother_type_GPRS_1_2) ||				//母板类型为GPRS类型
        (sys_config.mother_type == mother_type_GPRS))
    {
        
         //------- create UITD_GPRS_init thread
         tid = rt_thread_create("UITD_GPRS_init",
                                            rt_UITD_GPRS_init,
                                            RT_NULL,
                                            1024,
                                            RT_THREAD_PRIORITY_MAX/3,
                                            5);
        if (tid != RT_NULL)
        {        
            rt_thread_startup(tid);     
        }

    }
    else if (sys_config.mother_type == mother_type_ETH)						//母板类型为以太网类型
    {
        #if USE_SNTP_TIME
        sntp_init();
        #endif // USE_SNTP_TIME
        
         //------- create UITD_eth_init thread
         tid = rt_thread_create("UITD_eth_init",
                                            rt_UITD_eth_init,
                                            RT_NULL,
                                            2048,
                                            RT_THREAD_PRIORITY_MAX/3,
                                            5);
        if (tid != RT_NULL)
        {        
            rt_thread_startup(tid);     
        }
        
    }
    else if ((sys_config.mother_type == mother_type_GPRS_ETH) || (sys_config.mother_type == mother_type_IOT_PRO))		//母板为GPRS和以太网 或 网关类型
    {
        // Song: TODO:

         //------- create UITD_GPRS_init thread
         tid = rt_thread_create("UITD_GPRS_ETH_init",
                                            rt_UITD_GPRS_ETH_init,
                                            RT_NULL,
                                            2048,
                                            RT_THREAD_PRIORITY_MAX/3,
                                            5);
        if (tid != RT_NULL)
        {        
            rt_thread_startup(tid);     
        }

    }
    
}



void rt_APP_init_thread_entry(void* parameter)
{
    rt_thread_t tid;

//    while(sys_config.board_type_checked == 0)
//    {
//        rt_thread_delay(1);
//    }
    
    while(sys_config.sys_inited_flag == 0)								//等待系统初始化
    {
        rt_thread_delay(2);
    }
    
    
    if (sys_config.sys_mode == sys_mode_transparent)			//为透传模式
    {
        if ((sys_config.dev_type == dev_type_simple_UITD) 
            || (sys_config.dev_type == dev_type_full_func) 
            || (sys_config.dev_type == dev_type_UITD_3uart) 
            || (sys_config.dev_type == dev_type_IOT_PRO_UITD))
        {
             //------- create transparent thread
             thread_transparent = rt_thread_create("TP",
                                                rt_thread_entry_transparent,
                                                RT_NULL,
                                                THREAD_TRANSPARENT_STACK_SIZE,
                                                prio_transparent,
                                                10);
            if (thread_transparent != RT_NULL)
            {        
                rt_thread_startup(thread_transparent);     
            }
            
             //------- create transparent_rx thread
             thread_transparent_rx = rt_thread_create("TP_rx",
                                                rt_thread_entry_transparent_rx,
                                                RT_NULL,
                                                THREAD_TRANSPARENT_RX_STACK_SIZE,
                                                prio_transparent_rx,
                                                10);
            if (thread_transparent_rx != RT_NULL)
            {        
                rt_thread_startup(thread_transparent_rx);     
            }
        }    
    }
    else
    {
    
        if (sys_config.dev_type == dev_type_sample_ctrl)			//传感器采样应用
        {
             //------- create sensor_sample thread
             thread_sensor_sample = rt_thread_create("sensor",
                                                rt_thread_entry_sensor_sample,
                                                RT_NULL,
                                                THREAD_SENSOR_SAMPLE_STACK_SIZE,
                                                prio_sensor_sample,
                                                10);
            if (thread_sensor_sample != RT_NULL)
            {        
                rt_thread_startup(thread_sensor_sample);     
            } 
        }
        else if (sys_config.dev_type == dev_type_simple_UITD)			//标准报警数据采集应用
        {
             //------- create fire_alarm thread  报警数据处理线程(上发服务器处理线程)
             thread_fire_alarm = rt_thread_create("FA",
                                                rt_thread_entry_fire_alarm,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_STACK_SIZE,
                                                prio_fire_alarm,
                                                10);
            if (thread_fire_alarm != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm);     
            }
            
             //------- create fire_alarm_rx thread 报警数据接收处理线程
             thread_fire_alarm_rx = rt_thread_create("FA_rx",
                                                rt_thread_entry_fire_alarm_rx,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_RX_STACK_SIZE,
                                                prio_fire_alarm_rx,
                                                10);
            if (thread_fire_alarm_rx != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm_rx);     
            }
        }
        else if (sys_config.dev_type == dev_type_controller)			//对接消防主机
        {
             //------- create controller input thread //输入采集处理
             thread_ctrl_in = rt_thread_create("ctrl_in",
                                                rt_thread_entry_ctrl_in,
                                                RT_NULL,
                                                THREAD_CTRL_IN_STACK_SIZE,
                                                prio_ctrl_in,
                                                10);
            if (thread_ctrl_in != RT_NULL)
            {        
                rt_thread_startup(thread_ctrl_in);     
            }
            
             //------- create controller output thread	//输出处理
             thread_ctrl_out = rt_thread_create("ctrl_out",
                                                rt_thread_entry_ctrl_out,
                                                RT_NULL,
                                                THREAD_CTRL_OUT_STACK_SIZE,
                                                prio_ctrl_out,
                                                10);
            if (thread_ctrl_out != RT_NULL)
            {        
                rt_thread_startup(thread_ctrl_out);     
            }
        }
        else if (sys_config.dev_type == dev_type_full_func)				//应用 同上 dev_type_simple_UITD
        {

             //------- create fire_alarm thread
             thread_fire_alarm = rt_thread_create("FA",
                                                rt_thread_entry_fire_alarm,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_STACK_SIZE,
                                                prio_fire_alarm,
                                                10);
            if (thread_fire_alarm != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm);     
            }
            
             //------- create fire_alarm_rx thread
             thread_fire_alarm_rx = rt_thread_create("FA_rx",
                                                rt_thread_entry_fire_alarm_rx,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_RX_STACK_SIZE,
                                                prio_fire_alarm_rx,
                                                10);
            if (thread_fire_alarm_rx != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm_rx);     
            }        


            
        }
        else if (sys_config.dev_type == dev_type_UITD_3uart)			//uart3报警接入应用
        {
             //------- create fire_alarm thread
             thread_fire_alarm = rt_thread_create("FA",
                                                rt_thread_entry_fire_alarm,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_STACK_SIZE,
                                                prio_fire_alarm,
                                                10);
            if (thread_fire_alarm != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm);     
            }
            
             //------- create fire_alarm_rx thread
             thread_fire_alarm_rx = rt_thread_create("FA_rx",
                                                rt_thread_entry_fire_alarm_rx,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_RX_STACK_SIZE,
                                                prio_fire_alarm_rx,
                                                10);
            if (thread_fire_alarm_rx != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm_rx);     
            }
        }
        else if (sys_config.dev_type == dev_type_IOT_PRO_UITD)			//网关应用
        {
             //------- create fire_alarm thread
             thread_fire_alarm = rt_thread_create("FA",
                                                rt_thread_entry_fire_alarm,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_STACK_SIZE,
                                                prio_fire_alarm,
                                                10);
            if (thread_fire_alarm != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm);     
            }
            
             //------- create fire_alarm_rx thread
             thread_fire_alarm_rx = rt_thread_create("FA_rx",
                                                rt_thread_entry_fire_alarm_rx,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_RX_STACK_SIZE,
                                                prio_fire_alarm_rx,
                                                10);
            if (thread_fire_alarm_rx != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm_rx);     
            }
            
            //------- create sensor_sample thread
             thread_PRO_UITD = rt_thread_create("PRO_UITD",
                                                rt_thread_entry_PRO_UITD,
                                                RT_NULL,
                                                THREAD_PRO_UITD_STACK_SIZE,
                                                prio_PRO_UITD,
                                                10);
            if (thread_PRO_UITD != RT_NULL)
            {        
                rt_thread_startup(thread_PRO_UITD);     
            }             
        }
        else if (sys_config.dev_type == dev_type_IOT_PRO_CTRL)			//网关控制应用
        {
             //------- create fire_alarm thread
             thread_fire_alarm = rt_thread_create("FA",
                                                rt_thread_entry_fire_alarm,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_STACK_SIZE,
                                                prio_fire_alarm,
                                                10);
            if (thread_fire_alarm != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm);     
            }
            
             //------- create fire_alarm_rx thread
             thread_fire_alarm_rx = rt_thread_create("FA_rx",
                                                rt_thread_entry_fire_alarm_rx,
                                                RT_NULL,
                                                THREAD_FIRE_ALARM_RX_STACK_SIZE,
                                                prio_fire_alarm_rx,
                                                10);
            if (thread_fire_alarm_rx != RT_NULL)
            {        
                rt_thread_startup(thread_fire_alarm_rx);
            }

            //------- create sensor_sample thread  //传感器、开关信号输入输出、处理线程 ()
             thread_PRO_CTRL = rt_thread_create("PRO_CTRL",
                                                rt_thread_entry_PRO_CTRL,
                                                RT_NULL,
                                                THREAD_PRO_CTRL_STACK_SIZE,
                                                prio_PRO_CTRL,
                                                10);
            if (thread_PRO_CTRL != RT_NULL)
            {        
                rt_thread_startup(thread_PRO_CTRL);     
            } 

        }
        else if (sys_config.dev_type == dev_type_unknown)
        {
            rt_kprintf("Unknown board type !!! \n");
        }
        else
        {
            rt_kprintf("Unknown board type !!! \n");
        }
    }
    
		
		
}


int rt_application_init(void)
{
    rt_thread_t tid;

		//--------初始化线程
    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        10240, RT_THREAD_PRIORITY_MAX/3, 5);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    
    
     //-------create sys_status thread  系统状态线程
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
    
    //--------应用程序
    tid = rt_thread_create("APP_init",
            rt_APP_init_thread_entry, RT_NULL,
            256, RT_THREAD_PRIORITY_MAX/3 - 1, 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
		//--------通讯程序
    tid = rt_thread_create("COM_init",
        rt_COM_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3 - 1, 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    

    
    return 0;
}






/*@}*/
