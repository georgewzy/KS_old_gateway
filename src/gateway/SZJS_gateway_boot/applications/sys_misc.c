#include "sys_misc.h"
//#include "APMS_config.h"

#include "rtthread.h"
#include "rtconfig.h"
#include "sys_def.h"

#include <dfs.h>
#include <dfs_def.h>
#include <dfs_posix.h>

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


//struct t_sys_cfg sys_cfg_buf = {0};


const struct t_sys_cfg sys_cfg_init_data = 
{

    0,
    
//    A8_IF_BANDRATE,
//    QUANTA_DEBUG_BANDRATE,

//    WIFI_SSID_DEFAULT,
//    WIFI_KEY_DEFAULT,

    SVR_IF_USE_NAME,  // server interface use DNS.
    SVR_IF_NAME,
    SVR_IF_IP,
    SVR_IF_PORT,
//    
//    0,  // auto_update
//    0,  // force_check_up
//    0,  // force_up_wifi
//    0,  // force_up_sensor
//    
//    1,  // server_if_en
//    1,  // wifi_auto_en
//    
//    MODE_RT_PERIOD,  // RT_peri
//    MODE_RT_LENGTH, // RT_len
//    MODE_30M_LENGTH, // Nor_len
//    WIFI_ROAM_TRRIG_DBM,  // wifi_roam
};


//struct VirtEeprom eeprom_buf = 
//{



//    
//};


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


//const char *
//lwip_strerr(err_t err)
//{
//  return err_strerr[-err];
//}

#ifdef RT_USING_FINSH
#include <finsh.h>

void SYS_SoftReset(void)
{
	uint16_t temp_u16;

//#if APMS_AUTO_RESET_EN	
//	temp_u16 = BKP_ReadBackupRegister(RTC_BKP_DR2);
//	temp_u16++;
//	BKP_WriteBackupRegister(RTC_BKP_DR2,temp_u16);
//#endif 
	
    
	__set_FAULTMASK(1);
	NVIC_SystemReset();
	while(1);
}

void sys_reboot(void)
{

    //module_DSP_ecg_unload();
    //file_cache_unmount();
    
    //Power_off_dev();
    
//    POWER_UP_OFF();
    
    SYS_SoftReset();
}
FINSH_FUNCTION_EXPORT_ALIAS(sys_reboot, reboot, Reboot the system);


void sys_shutdown(void)
{

    //module_DSP_ecg_unload();
    //file_cache_unmount();
    
    //Power_off_dev();
    
//    POWER_LDO1_OFF();
//    POWER_LDO2_OFF();
//    POWER_DCDC2_OFF();
//    POWER_UP_OFF();
    
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


//int print_md5(uint8_t *string)
//{
//    uint8_t md5[16];
//    uint8_t i;
//    
//    md5_hex(string, strlen(string), md5);
//    rt_kprintf("MD5:  ");
//    
//    for (i=0;i<16;i++)
//    {
//        rt_kprintf("%02X", md5[i]);
//    }
//    rt_kprintf("\n");
//    
//    return 0;
//}

//FINSH_FUNCTION_EXPORT_ALIAS(print_md5, print_md5, Print the input strings MD5 result.);



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
        rt_kprintf("Create file failed : %d \n", fd);
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

#endif // RT_USING_FINSH