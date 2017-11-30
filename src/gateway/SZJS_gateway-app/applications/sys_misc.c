#include "sys_misc.h"
#include "sys_config.h"
#include "board.h"

#include "rtthread.h"
#include "rtconfig.h"
#include "sys_def.h"

#include <dfs.h>
#include <dfs_def.h>
#include <dfs_posix.h>


#include "fire_alarm.h"
#include "transparent.h"

#include <string.h>
//#include "core_cm3.h"
//#include "server_if.h"

//#include "A8_if.h"
//#include "Virt_eeprom.h"


//#define ERR_OK          0    /* No error, everything OK. */
//#define ERR_MEM        -1    /* Out of memory error.     */
//#define ERR_BUF        -2    /* Buffer error.            */
//#define ERR_TIMEOUT    -3    /* Timeout.                 */
//#define ERR_RTE        -4    /* Routing problem.         */
//
//#define ERR_IS_FATAL(e) ((e) < ERR_RTE)
//
//#define ERR_ABRT       -5    /* Connection aborted.      */
//#define ERR_RST        -6    /* Connection reset.        */
//#define ERR_CLSD       -7    /* Connection closed.       */
//#define ERR_CONN       -8    /* Not connected.           */
//
//#define ERR_VAL        -9    /* Illegal value.           */
//
//#define ERR_ARG        -10   /* Illegal argument.        */
//
//#define ERR_USE        -11   /* Address in use.          */
//
//#define ERR_IF         -12   /* Low-level netif error    */
//#define ERR_ISCONN     -13   /* Already connected.       */
//
//#define ERR_INPROGRESS -14   /* Operation in progress    */

uint8_t sys_version_flag = 0;
uint8_t sys_version_buf[3] = {0};
uint8_t sensor_fw_updated = 0;

uint8_t console_output_enable = 0;

uint8_t sys_dbg_level = SYS_DEBUG_LEVEL;

const char *err_strerr[] = {"Ok.",
  "Out of memory error.",
  "Buffer error.",
  "Timeout.",
  "Routing problem.",
  "Connection aborted.",
  "Connection reset.",
  "Connection closed.",
  "Not connected.",
  "Illegal value.",
  "Illegal argument.",
  "Address in use.",
  "Low-level netif error",
  "Already connected.",
  "Operation in progress",
};



// const struct VirtEeprom eeprom_init_data = {
// 																						0,
// 																							{
// 																								{0,0,0},
// 																								{0,0,0},
// 																								{0,0,0},
// 																							}
// 																						};

const struct VirtEeprom eeprom_init_data = 
{
    0,
//    
//    A8_IF_BANDRATE%0x10000,
//    A8_IF_BANDRATE/0x10000,
//    QUANTA_DEBUG_BANDRATE%0x10000,
//    QUANTA_DEBUG_BANDRATE/0x10000,

//    {0},
//    {0},

//    {0},
//    SVR_IF_PORT,
    
};


const s_PRO_UITD_cfg PRO_UITD_cfg_init_data = 
{
    20,  // SEN01_report_period
    0,  // SEN02_report_period
    0,  // SEN03_report_period
    0,  // SEN04_report_period
    0,  // SW01_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW01_report_period, unit: S.
};

const s_PRO_CTRL_cfg PRO_CTRL_cfg_init_data = 
{
    20,  // SEN01_report_period
    0,  // SEN02_report_period
    0,  // SEN03_report_period
    0,  // SEN04_report_period
    0,  // SW01_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW01_report_period, unit: S.
    0,  // SW02_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW02_report_period, unit: S.
    0,  // SW03_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW03_report_period, unit: S.
    0,  // SW04_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW04_report_period, unit: S.
    0,  // SW05_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW05_report_period, unit: S.
    0,  // SW06_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW06_report_period, unit: S.
    0,  // SW07_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW07_report_period, unit: S.
    0,  // SW08_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW08_report_period, unit: S.
};

//struct t_sys_cfg sys_cfg_buf = {0};
const s_AP01_cfg AP01_cfg_init_data = 
{
    20,  // SEN01_report_period
    0,  // SEN02_report_period
};

//const s_AP02_cfg AP02_cfg_init_data = 
//{
//    FA_TYPE_DEFAULT,  // FA_type
//    0,  // FA_baud, 0: default baudrate, >0: use this baudrate.
//    1,  // FA_listen, 0: communicate mode, 1: listen mode (not send alive packet to FAC)
//    0,  // SW01_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
//    60, // SW01_report_period, unit: S.
//};

const s_FA_cfg FA_cfg_init_data = 
{
    1,
    UITD_UART_COM_BUS, 
    FA_TYPE_DEFAULT, // FA_type
    0, // FA_baud, 0: default baudrate, >0: use this baudrate.
    1, // FA_listen, 0: communicate mode, 1: listen mode (not send alive packet to FAC)

    1,
    UITD_UART_VK_1,
    FA_TYPE_DEFAULT,
    0,
    1,

    1,
    UITD_UART_VK_2,
    FA_TYPE_DEFAULT,
    0,
    1,

    1,
    UITD_UART_VK_3,
    FA_TYPE_DEFAULT,
    0,
    1,

    1,
    UITD_UART_VK_4,
    FA_TYPE_DEFAULT,
    0,
    1,
    
    0,  // SW01_trig_cfg, 0:none, 1:close, 2:open, 3:either, 4:period
    60, // SW01_report_period, unit: S.
};


const s_eth_cfg eth_cfg_init_data = 
{
    1,
    ETH_IP_DEFAULT,
    ETH_GW_DEFAULT,
    ETH_MASK_DEFAULT,
    DNS_SERVER_DEFAULT,
};


const s_transparent_cfg transparent_cfg_init_data = 
{
    TP_CFG_TRANS_PERIOD_DEF,
    TP_CFG_BUF_PERIOD_DEF,
    "", // null, use default uart port.
    {
        TP_CFG_BAUDRATE_DEFAULT,
        TP_CFG_BITS_DEFAULT,
        TP_CFG_VERIFY_DEFAULT,
        TP_CFG_STOPBIT_DEFAULT,
    }
};

const s_mode_cfg mode_cfg_init_data_GPRS = 
{
    MODE_CFG_SINGLE_WAY,// mode: if single communication way.
    1,                  // GPRS
    0,                  // ethernet
    0,                  // WIFI
};

const s_mode_cfg mode_cfg_init_data_ETH = 
{
    MODE_CFG_SINGLE_WAY,  // mode: if single communication way.
    0,                  // GPRS
    1,                  // ethernet
    0,                  // WIFI
};

const s_mode_cfg mode_cfg_init_data_GPRS_ETH = 
{
    MODE_CFG_SINGLE_WAY,  // mode: if single communication way.
    0,                  // GPRS
    1,                  // ethernet
    0,                  // WIFI
};

const s_mode_cfg mode_cfg_init_data_IOT_PRO = 
{
    MODE_CFG_SINGLE_WAY,  // mode: if single communication way.
    0,                  // GPRS
    1,                  // ethernet
    0,                  // WIFI
};

const s_mode_cfg mode_cfg_init_data_EC_AP = 
{
    MODE_CFG_SINGLE_WAY,  // mode: if single communication way.
    1,                  // GPRS
    0,                  // ethernet
    0,                  // WIFI
};

const s_mode_cfg mode_cfg_init_data_EC_TERM = 
{
    MODE_CFG_SINGLE_WAY,  // mode: if single communication way.
    0,                  // GPRS
    0,                  // ethernet
    0,                  // WIFI
};

const s_WIFI_cfg WIFI_cfg_init_data = 
{
    WIFI_SSID_DEFAULT,
    WIFI_PW_DEFAULT,
};


const struct t_sys_cfg sys_cfg_init_data = 
{

    0,
    0,  // default 0, only set testing mode when factory first programming firmware. 
    
    1,
    
    SVR_IF_USE_NAME,  // server interface use DNS.
    SVR_IF_NAME,
    SVR_IF_IP,
    SVR_IF_PORT,
    0,

    SVR_2_IF_USE_NAME,  // server interface use DNS.
    SVR_2_IF_NAME,
    SVR_2_IF_IP,
    SVR_2_IF_PORT,
    1,
    
    SYS_DEBUG_LEVEL,  // debug_level
    SYS_LOG_FILE_EN,  // log_file_en
    SYS_LOG_FILE_PATH, // log_file_path
    
    //0,  // battery check support
};


//struct VirtEeprom eeprom_buf = 
//{



//    
//};


void SYS_print_hex_dump(uint8_t *str, uint8_t *buf, uint32_t len)
{
    int i = 0;
    
    rt_kprintf("%s:", str);
    for (i=0;i<len;i++)
    {
        rt_kprintf(" %02X", buf[i]);
    }
    rt_kprintf("\n");
}


void Print_caller_adr(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: rt_kprintf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	rt_kprintf("DEBUG: Wrong parameters value: file %s on line %d\r\n", file, line);

// Todo song :
	//while(1);   
}


uint8_t parity_check_uchar(uint8_t x)
{
    uint8_t val=0;
    while(x)
    {
        val^=x;
        x>>=1;
    }
    return val&0x1;
}



#ifdef RT_USING_FINSH
#include <finsh.h>

void SYS_SoftReset(void)
{
	uint16_t temp_u16;


    
	__set_FAULTMASK(1);
	NVIC_SystemReset();
	while(1);
}

void sys_reboot(void)
{


    
    SYS_SoftReset();
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_reboot, reboot, Reboot the system);


void sys_shutdown(void)
{


    
    while(1);
    //SYS_SoftReset();
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_shutdown, shutdown, Shutdown the system);



// if flag is not zero, reboot.
void sys_reboot_if(uint32_t flag)
{
    if (flag)
    {
        sys_reboot();
    }
}

FINSH_FUNCTION_EXPORT_ALIAS(sys_reboot_if, reboot_if, Reboot the system when flag is not zero.);

// if flag is zero, reboot.
void sys_reboot_if_0(uint32_t flag)
{
    if (flag == 0)
    {
        sys_reboot();
    }
}

FINSH_FUNCTION_EXPORT_ALIAS(sys_reboot_if_0, reboot_if_0, Reboot the system when flag is zero.);





void cat_bin(const char* filename,rt_uint32_t start, rt_uint32_t size, rt_uint32_t len)
{
    struct dfs_fd fd;
    rt_uint32_t length;
    char buffer[3*17];
    rt_uint32_t cnt = 0;
    rt_uint32_t i = 0;
    rt_uint32_t pos = 0;
    
    if (len != 0)
    {
        if (len > 16)
        {
            len = 16;
        }
    }
    else
    {
        len = 16;
    }
    
    

    if (dfs_file_open(&fd, filename, DFS_O_RDONLY) < 0)
    {
        rt_kprintf("Open %s failed\n", filename);
        
        
    }
    else
    {
        
        if (start != 0)
        {
            pos = dfs_file_lseek(&fd, start);
            if (pos != start)
            {
                rt_kprintf("Start position is overflow! \n");
                goto exit;
            }
        }
        
        while(1)
        {
            rt_memset(buffer, 0x00, sizeof(buffer));
            length = dfs_file_read(&fd, buffer, len );
            if (length < len)
            {
                for (i=0;i<length;i++)
                {
                    rt_kprintf("%02X ", buffer[i]);
                    cnt ++;
                    if (size != 0)
                    {
                        if (cnt >= size)
                        {
                            goto exit;
                        }
                    }
                }
                rt_kprintf("\n");
                break;
            }
            else
            {
                for (i=0;i<length;i++)
                {
                    rt_kprintf("%02X ", buffer[i]);
                    cnt ++;
                    if (size != 0)
                    {
                        if (cnt >= size)
                        {
                            goto exit;
                        }
                    }
                }
                rt_kprintf("\n");
            }
        }
exit:        
        dfs_file_close(&fd);
        //return;
    }

}
FINSH_FUNCTION_EXPORT(cat_bin, print file -- filename -start -size -line_len)


int cat_flash(uint8_t *pw, rt_uint32_t start, rt_uint32_t size, rt_uint32_t len)
{
    uint8_t *p = (uint8_t *)start;
    int i = 0;
    int j = 0;
    int line_num = 0;
    int pw_len = strlen(pw);
    
    if (pw_len > 256)
    {
        return -3;
    }
    
    if (memcmp(SYS_PASSWORD, pw, strlen(SYS_PASSWORD)) != 0)
    {
        return -2;
    }
    
    if (len != 0)
    {
        if (len > 16)
        {
            len = 16;
        }
    }
    else
    {
        len = 16;
    }
    
    line_num = size/len;
    
    for (j=0;j<line_num;j++)
    {
        rt_kprintf("0x%08X :\t", start+j*len);
        for (i=0;i<len;i++)
        {
            rt_kprintf(" %02X", *(p+j*len+i));
        }
        rt_kprintf("\n");
    }
    
    if (size%len)
    {
        rt_kprintf("0x%08X :\t", start+j*len);
        for (i=0;i<len;i++)
        {
            rt_kprintf(" %02X", *(p+j*len+i));
        }
        rt_kprintf("\n");
    }
    
    return 0;
}
FINSH_FUNCTION_EXPORT(cat_flash, print flash --pw --address -size -line_len)


int touch(const char* filename, int force_create)
{
    int fd ;
    struct stat stat;
    
    if (force_create)
    {
        rm(filename);
    }
    
    fd = open(filename, O_CREAT | O_RDWR, 0);
    if (fd < 0)
    {
        rt_err_t err = rt_get_errno();
        rt_kprintf("Create file failed : %d \n", err);
        return fd;
    }
    else
    {
        fstat(fd, &stat);
        if (stat.st_size != 0)
        {
            rt_kprintf("File is exist , size : %d\n", stat.st_size);
        }
    }
    
    close(fd);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(touch, touch, Create a file. support force create  .);



int append(const char* filename, const char *string)
{
    int fd;
    int res;
    int len;
    
    fd = open(filename, O_WRONLY | O_APPEND, 0);
    if (fd < 0)
    {
        rt_kprintf("File open failed : %d\n", fd);
        return -1;
    }
    
    len = strlen(string);
    res = write(fd, string, len);
    if (res != len)
    {
        rt_kprintf("File add error, writed %d bytes\n", res);
        close(fd);
        return -2;
    }
    
    close(fd);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(append, append, Add string to the file tail.);

int upgrade(void)
{
    touch(FW_UPGRADE_FLAG, 1);
    rt_thread_delay(RT_TICK_PER_SECOND*1);
    sys_reboot();
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(upgrade, upgrade, Upgrade the firmware.);


int sys_delay(uint32_t seconds)
{
    rt_thread_delay(seconds * RT_TICK_PER_SECOND);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_delay, DELAY, Delay some seconds  --seconds.);


// Song: added 2016-10-11 17:25:01
void log_trace_reset(void)
{
    log_trace_close();
    rm(sys_config.log_file_path);
    //rt_device_control(_traceout_device, LOG_TRACE_CTRL_RESET, RT_NULL);
}
FINSH_FUNCTION_EXPORT_ALIAS(log_trace_reset, log_reset, Reset the log to empty);

void sys_log_level_amend(uint8_t level)
{
  
    SYS_log(SYS_DEBUG_ERROR, ("System log level : \n old: %d , new : %d \n", sys_config.debug_level, level));
    sys_config.debug_level = level;
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_log_level_amend, sys_log_amend, Amend the log level temporary -1:ERROR -2:WARNING -4:INFO -8:DEBUG);


uint64_t disk_free(const char *path)
{
    int result;
    long long cap;
    struct statfs buffer;

    result = dfs_statfs(path ? path : RT_NULL, &buffer);
    if (result != 0)
    {
        rt_kprintf("dfs_statfs failed.\n");
        return 0;
    }

    cap = buffer.f_bsize * buffer.f_bfree ;
//    rt_kprintf("disk free: %d KB [ %d block, %d bytes per block ]\n",
//    (unsigned long)(cap/1024), buffer.f_bfree, buffer.f_bsize);
    return cap;
}


int file_get_length(const char *path, uint32_t *length)
{
    int result;
    //uint32_t length = 0;
    //struct statfs buffer;
    struct stat buffer;
    
    if (path == NULL)
    {
        return -1;
    }
    
    result = dfs_file_stat(path, &buffer);
    if (result != 0)
    {
        rt_kprintf("Get file stat failed.\n");
        return 0;
    }

    *length = buffer.st_size ;
//    rt_kprintf("disk free: %d KB [ %d block, %d bytes per block ]\n",
//    (unsigned long)(cap/1024), buffer.f_bfree, buffer.f_bsize);
    return 0;
}


int file_get_CRC16(const char *path, uint16_t *CRC16)
{
    int result;
    int fd;
    uint8_t buf[512];
    uint16_t CRC_temp = 0;
    //uint32_t length = 0;
    //struct statfs buffer;
    //struct stat buffer;
    
    if (path == NULL)
    {
        return -1;
    }
    
    fd = open(path, O_RDONLY | O_BINARY, 0);
    if (fd < 0)
    {
        SYS_log(SYS_DEBUG_ERROR, ("Open file %s failed : %d.\n", path, fd));
        return -2;
    }
    
    CRC_temp = 0x0000;
    while(1)
    {
        result = read(fd, buf, sizeof(buf));
        if (result == sizeof(buf))
        {
            CRC_temp = Crc16(CRC_temp, buf, result);
        }
        else
        {
            CRC_temp = Crc16(CRC_temp, buf, result);
            *CRC16 = CRC_temp;
            break;
        }
    }
    
    close(fd);
    return 0;
}

int console_enable_login(uint8_t *PW)
{
    rt_device_t p_device = NULL;
    int res = 0;

    
    res = strcmp(PW, SYS_LOGIN_PW);
    if (res == 0)
    {
    
//        p_device = rt_console_get_device();
//        
//        if (p_device == NULL)
//        {
//            
//        #ifdef RT_USING_CONSOLE
//            rt_console_set_device(CONSOLE_DEVICE);
//        #endif
//                
//            finsh_system_init();
        
            console_output_enable = 1;
            return 0;
//        }
//        else
//        {
//            return -2;
//        }
    }
    else
    {
        return -1;
    }
    
}
FINSH_FUNCTION_EXPORT_ALIAS(console_enable_login, login, --password);


int rt_ext_strlen(char *str)
{
    return strlen(str);
}
RTM_EXPORT(rt_ext_strlen);

int rt_ext_atoi(char *str)
{
    return atoi(str);
}
RTM_EXPORT(rt_ext_atoi);

char *rt_ext_strstr(char *str_1, char *str_2)
{
    return strstr(str_1, str_2);
}
RTM_EXPORT(rt_ext_strstr);


RTM_EXPORT(sscanf);
RTM_EXPORT(strstr);
RTM_EXPORT(atoi);
RTM_EXPORT(sprintf);


#endif // RT_USING_FINSH