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

#include "msd.h"
#include "rtc.h"    
#include "platform.h"
#include "UITD.h"


extern void rt_platform_init(void);

struct serial_configure com_testing_cfg = 
{
    COM_TEST_BAUDRATE, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
};

//uint64_t device_SN = 101163500001;
uint64_t device_SN = 0x1234567890AB;
uint8_t ID_temp[32] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2};

    
struct rt_device * device_com_bus;
struct rt_semaphore sem_com_rx;    
    
uint8_t com_buf[256] = {0};
uint32_t com_buf_index = 0;
uint32_t com_buf_len = 0;

rt_uint8_t prio_com_rx = 4;
rt_thread_t thread_com_rx;
    
void rt_thread_entry_com_rx(void* parameter);    
    
rt_err_t com_bus_rx_ind(rt_device_t dev, rt_size_t size)
{
    
    rt_sem_release(&sem_com_rx);    
}

rt_err_t com_bus_tx_complete(rt_device_t dev, void *buffer)
{


}    
    
    
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
                rt_kprintf("Flash File System initialzation failed again !!! \n");
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
                rt_kprintf("Flash File System initialzation failed again !!! \n");
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
            dfs_unmount("/");
            mkfs("elm","flash0");
            if (dfs_mount("flash0", ELMFS_DATAFLASH_DIR, "elm", 0, 0) != 0)
            {
                rt_kprintf("Flash File System initialzation failed again !!! \n");
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
            
            mkdir(SD_DIR, 0);
        }
    }
    closedir(dir);    

    
    dir = opendir(SYS_CFG_DIR);
    if (dir == NULL)
    {
        mkdir( SYS_CFG_DIR, 0);
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
                json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
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
            json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
            rt_kprintf("System config file is rewrite with sys_cfg_init_data !!! \n");
        }
    }
    else
    {
        json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
        rt_kprintf("System config file is recreated with sys_cfg_init_data !!! \n");
    }
}




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
        rt_thread_delay(RT_TICK_PER_SECOND/2);
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
        
        rt_thread_delay(RT_TICK_PER_SECOND/2);
		/* mount FAT file system on SD card */
        msd_init("sd0", "spi11");
        if (dfs_mount("sd0", SD_DIR, "elm", 0, 0) == 0)
        {
            rt_kprintf("SDCard File System initialized!\n");
        }
		else
		{
            rt_kprintf("SDCard File System initialzation failed!\n");
//			rt_kprintf("SDCard File System initialzation failed!\nFormat the SD card.\n");
//            mkfs("elm", "sd0");
//            if (dfs_mount("sd0", SD_DIR, "elm", 0, 0) != 0)
//            {
//                rt_kprintf("SDCard File System initialzation failed again !!! \n");
//                while(1)
//                {
//                    rt_thread_delay(10);
//                }
//            }
		}
        
        system_fs_check();        
        

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
        
    
    json_cfg_load();
    json_cfg_wr_sys_cfg(1);


#ifdef RT_USING_LOGTRACE    
    memlog_init();
#endif // RT_USING_LOGTRACE
    
    //cpu_usage_init();

#ifdef RT_USING_INIT_RC    
    /* do initialization script file */
    do_init();
#endif // RT_USING_INIT_RC     
    
#if TOOL_USE_EXT_FW
    copy(SYS_ISP_SD_BOOT_FW_FILE, SYS_ISP_BOOT_FW_FILE);
#endif // TOOL_USE_EXT_FW
    
    sys_inited_flag = 1;
}

int ISP_force_test(uint32_t address, uint32_t value)
{
    int fd = -1;
    int ret = 0;
    uint8_t test_mode_file[] = "/fw/test_mode.bin";
    
    
    touch(test_mode_file, 1);
    
    fd = open(test_mode_file, O_RDWR, 0);
    if (fd < 0)
    {
        rt_kprintf("test_mode file open failed !\n");
        return -2;
    }
    
    write(fd, &value, sizeof(value));
    
    close(fd);
    
    ret = ISP_flash(test_mode_file, 0, address, 0); 
    
    return ret;
    
}


int ISP_otp_ID(uint8_t *ID, uint32_t len)
{
    int fd = -1;
    uint8_t ID_buf[512] = {0};
    uint32_t lock_byte = 0x00000000;
    int ret = 0;
    uint8_t ID_file_1[] = "/fw/ID_1.bin";
    uint8_t ID_file_2[] = "/fw/ID_2.bin";
    uint8_t ID_lock_file[] = "/fw/ID_lock.bin";
    
    if (len > 512)
    {
        rt_kprintf("ID lenght is too long !\n");
        return -1;
    }
    rt_memcpy(ID_buf, ID, len);
    
    touch(ID_file_1, 1);
    touch(ID_file_2, 1);
    touch(ID_lock_file, 1);
    
    fd = open(ID_file_1, O_RDWR, 0);
    if (fd < 0)
    {
        rt_kprintf("ID_1 file open failed !\n");
        return -2;
    }
    
    write(fd, ID_buf, 4);
    
    close(fd);

    fd = open(ID_file_2, O_RDWR, 0);
    if (fd < 0)
    {
        rt_kprintf("ID_2 file open failed !\n");
        return -2;
    }
    
    write(fd, ID_buf+4, 4);
    
    close(fd);

    fd = open(ID_lock_file, O_RDWR, 0);
    if (fd < 0)
    {
        rt_kprintf("ID_lock file open failed !\n");
        return -2;
    }
    
    write(fd, &lock_byte, 4);
    
    close(fd);

    
    ret = ISP_flash(ID_file_1, 0, SYS_ID_ADDRESS, 0); 
    
    ret = ISP_flash(ID_file_2, 0, SYS_ID_ADDRESS+4, 0); 
    
    ret = ISP_flash(ID_lock_file, 0, SYS_ID_LOCK_ADDRESS, 0);
    
    return ret;
    
}

void rt_ISP_thread_entry(void* parameter)
{
    int ret = 0;
    uint32_t counter = 0;
    
    while(1)
    {
        if (sys_inited_flag) break;
        rt_thread_delay(1);
    }
    
    
    device_com_bus = rt_device_find(UITD_UART_COM_BUS);
    if(device_com_bus == RT_NULL)
    {
        rt_kprintf("Serial device %s not found!\r\n", UITD_UART_COM_BUS);
    }
    else
    {
		rt_device_open(device_com_bus, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_set_rx_indicate(device_com_bus, com_bus_rx_ind);
        rt_device_set_tx_complete(device_com_bus, com_bus_tx_complete);
    }

    
    while(1)
    {
        
        switch (sys_ctrl.sys_state)
        {
            case SYS_state_inited:
                
            
                sys_status_set(&sys_ctrl, SYS_state_read_ID);
                break;
            case SYS_state_read_ID:
                
                if (rt_pin_read(PIN_TIM1_IN) == 0)
                {
                    counter ++;
                }
                else if (counter && (rt_pin_read(PIN_TIM1_IN) == 1))
                {
                    if ((counter > 100/10/5) && (counter < 1000/10/5))  // 100mS - 1S
                    {
                        #if TOOL_SUPPORT_TESTING
                        rt_device_control(device_com_bus, RT_DEVICE_CTRL_CONFIG, &com_testing_cfg);
                        sys_status_set(&sys_ctrl, SYS_status_board_testing);
                        #endif //
                    }
                    else if (counter > 2000/10/5)  // >2S
                    {
                        sys_status_set(&sys_ctrl, SYS_state_APP_programing);
                    }
                    
                    counter = 0;
                }
                
                //sys_status_set(&sys_ctrl, SYS_state_IAP_programing);
                break;
            case SYS_state_IAP_programing:
                //rt_thread_delay(RT_TICK_PER_SECOND);
                //if (rt_pin_read(PIN_TIM1_IN) == 0)
                {
                    //sys_status_set(&sys_ctrl, SYS_state_IAP_programing);
                    ret = ISP_unreadprot();
    //                if (ret != 0)
    //                {
    //                    goto ISP_CLOSE;
    //                }

    //                rt_pin_write(PIN_ISP_BOOT, 1);
    //                rt_thread_delay(RT_TICK_PER_SECOND/10);
    //                rt_pin_write(PIN_ISP_RESET, 0);
    //                rt_thread_delay(RT_TICK_PER_SECOND);
    //                rt_pin_write(PIN_ISP_RESET, 1);
                    
                    #if SYS_ISP_ID_EN
                    ret = ISP_otp_ID((uint8_t *)&device_SN, sizeof(device_SN));
                    //ret = ISP_otp_ID((uint8_t *)&ID_temp, sizeof(ID_temp));
                    #endif // SYS_ISP_ID_EN
                    
                    ret = ISP_flash(SYS_ISP_BOOT_FW_FILE, 0, 0, 0);
                    if (ret != 0)
                    {
                        goto ISP_CLOSE;
                    }
                    
                    //ISP_force_test(SYS_TESTING_MODE_ADDR, SYS_TESTING_MODE_CODE);
                    
                    ISP_readprot();
                    
                    
ISP_CLOSE:
                    if (ret != 0)
                    {
                        rt_kprintf("Program failed, reset thr MCU. %d\n", ret);
                        sys_status_set(&sys_ctrl, SYS_state_IAP_failed);
                    }
                    else
                    {
                        rt_kprintf("Program finished, reset thr MCU. \n");
                        sys_status_set(&sys_ctrl, SYS_state_IAP_success);
                    }
                    
                    
                    rt_pin_write(PIN_ISP_BOOT, 0);
                    rt_thread_delay(RT_TICK_PER_SECOND/10);
                    rt_pin_write(PIN_ISP_RESET, 0);
                    rt_thread_delay(RT_TICK_PER_SECOND);
                    rt_pin_write(PIN_ISP_RESET, 1);
                    
                    rt_thread_delay(RT_TICK_PER_SECOND*10);
                    rt_pin_write(PIN_ISP_RESET, 0);
                    rt_thread_delay(RT_TICK_PER_SECOND);
                    rt_pin_write(PIN_ISP_RESET, 1);
                }
                        
                break;
            case SYS_state_IAP_failed:
                    
                break;
            case SYS_state_IAP_success:
            
                rt_kprintf("ISP_flash completed, enter testing mode \n");
            
                rt_device_control(device_com_bus, RT_DEVICE_CTRL_CONFIG, &com_testing_cfg);
            
                sys_status_set(&sys_ctrl, SYS_status_board_testing);
                break;
            case SYS_state_APP_programing:
                ret = ISP_unreadprot();

                //ret = ISP_flash(SYS_ISP_FIRMWARE_FILE, 0, SYS_ISP_FW_START_ADDR, 0);
                ret = ISP_flash(SYS_ISP_BOOT_FW_FILE, 0, 0, 0);
                if (ret != 0)
                {
                    rt_kprintf("Program APP failed, reset thr MCU. %d\n", ret);
                    sys_status_set(&sys_ctrl, SYS_state_APP_failed);
                }
                else 
                {
                    rt_kprintf("Program APP finished, reset thr MCU. \n");
                    sys_status_set(&sys_ctrl, SYS_state_APP_success);
                }
                    
                ISP_readprot();
                
                rt_pin_write(PIN_ISP_BOOT, 0);
                rt_thread_delay(RT_TICK_PER_SECOND/10);
                rt_pin_write(PIN_ISP_RESET, 0);
                rt_thread_delay(RT_TICK_PER_SECOND);
                rt_pin_write(PIN_ISP_RESET, 1);
                
                rt_thread_delay(RT_TICK_PER_SECOND*10);
                rt_pin_write(PIN_ISP_RESET, 0);
                rt_thread_delay(RT_TICK_PER_SECOND);
                rt_pin_write(PIN_ISP_RESET, 1);
            
                break;
            case SYS_state_APP_failed:
                
                break;
            case SYS_state_APP_success:
                
                break;
            case SYS_status_board_testing:
                
                counter ++;
                if (counter == 5)
                {
                    rt_pin_write(PIN_IO_OUT, PIN_HIGH);
                }
                else if (counter >= 10)
                {
                    rt_pin_write(PIN_IO_OUT, PIN_LOW);
                    counter = 0;
                }
            
                break;
            default:
                break;
        }
        
        rt_thread_delay(5);
    }
}

int com_rx_parser(rt_device_t dev)
{
    uint8_t data_temp = 0x00;
    int res = 0;
    int i = 0;
    uint64_t ID_temp = 0;
    uint8_t *p_uint8 = NULL;
    uint8_t SN_error = 0;
    
    while(1)
    {
        res = rt_device_read(dev, 0, &data_temp, 1);
        if (res < 1)
        {
            return -1;
        }
        com_buf[com_buf_len] = data_temp;
        com_buf_len ++;

        if (com_buf_len > (sizeof(com_buf)-4))
        {
            com_buf_len = 0;
            return -2;
        }
        else
        {
            if (data_temp == 0x0D)
            {
                
                if (sys_ctrl.sys_state == SYS_state_read_ID)
                {
                    com_buf[com_buf_len] = 0x00;
                    
                    if (com_buf_len != (13 + 1))
                    {
                        rt_kprintf("Scanned number is not 13 ! : %s\n", com_buf);
                        com_buf_len = 0;
                        break;
                    }
                    
                    for (i=0;i<(com_buf_len-1);i++)
                    {
                        if ((com_buf[i] > '9') || (com_buf[i] < '0'))
                        {
                            SN_error = 1;
                        }
                    }
                    if (SN_error) 
                    {
                        rt_kprintf("Scanned number is not number ! : %s\n", com_buf);
                        com_buf_len = 0;
                        break;
                    }
                    
                    ID_temp = 0;
                    for (i=0;i<(com_buf_len-1);i++)
                    {
                        ID_temp *= 10;
                        ID_temp += (com_buf[i] - '0');
                    }
                    device_SN = ID_temp;
                    p_uint8 = (uint8_t *)&device_SN;
                    rt_kprintf("Scanned barcode : %s\n   HEX:  %02X %02X %02X %02X %02X %02X\n", 
                            com_buf, p_uint8[0], p_uint8[1], p_uint8[2], p_uint8[3], p_uint8[4], p_uint8[5]);
                    
                    
                    sys_status_set(&sys_ctrl, SYS_state_IAP_programing);
                }
                else if (sys_ctrl.sys_state == SYS_status_board_testing)
                {
                    rt_thread_delay(5); // delay 30ms, feedback the data. 
                    for (i=0;i<(com_buf_len-1);i++)
                    {
                        com_buf[i] -= 1;
                        
                    }
                    rt_device_write(dev, 0, com_buf, com_buf_len);
                }
                
                com_buf_len = 0;
                
                return 0;
            }
        }
    }
    
    return -1;
}

int com_rx_handler(void)
{

    return 0;
}


void rt_thread_entry_com_rx(void* parameter)
{
    int res = 0;
    
    rt_sem_init(&sem_com_rx, "sem_com_rx", 0, RT_IPC_FLAG_FIFO);

    while(1)
    {
        if (sys_inited_flag) break;
        rt_thread_delay(1);
    }
    
    while(1)
    {
        
        if (rt_sem_take(&sem_com_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (com_rx_parser(device_com_bus) == 0)
            {
//                res = com_rx_handler();
//                if (res)
//                {
//                    
//                }
            }
        }
        else
        {
            
        }
    }
}

int rt_application_init(void)
{
    rt_thread_t tid;
    rt_thread_t thread_ISP;

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

     //------- create sys_status thread
     thread_ISP = rt_thread_create("ISP",
                                        rt_ISP_thread_entry,
                                        RT_NULL,
                                        10240,
                                        prio_sys_status + 1,
                                        50);
    if (thread_ISP != RT_NULL)
    {        
        rt_thread_startup(thread_ISP);     
    }    

     //------- create com_rx thread
     thread_com_rx = rt_thread_create("com_rx",
                                        rt_thread_entry_com_rx,
                                        RT_NULL,
                                        2048,
                                        prio_com_rx,
                                        20);
    if (thread_com_rx != RT_NULL)
    {        
        rt_thread_startup(thread_com_rx);     
    }    
    
    
    
    return 0;
}






/*@}*/
