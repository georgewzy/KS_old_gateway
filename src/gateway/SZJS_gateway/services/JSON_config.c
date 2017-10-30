#include "JSON_config.h"

#include "cJSON.h"
#include <dfs.h>
#include <dfs_posix.h>
#include "board.h"
//#include "sys_misc.h"
#include "sys_def.h"

#include "fire_alarm.h"
#include "sys_misc.h"
#include "transparent.h"
#include "pro_ctrl.h"

//#include "A8_if.h"
//#include "data_store.h"
//#include "wifi_app.h"

static uint8_t *cfg;


int json_cfg_open(char *dir, int flags)
{
    int fd;
    struct stat state;
    //uint8_t *cfg = NULL;
    
    // Read the config file.
    fd = open( dir, flags, 0);
    if (fd >= 0)
    {
        
        if (fstat(fd, &state) != DFS_STATUS_OK)
        {
            close(fd);
            return -1;
        }
        
        if (state.st_size == 0)
        {
            close(fd);
            return -2;
        }   
        
        cfg = (uint8_t *)rt_malloc(state.st_size + 1);
        if (cfg == NULL)
        {
            rt_kprintf("malloc system config failed ! \r");
            close(fd);
            return -3;
        }
        
        read(fd, cfg, state.st_size);
        //close(fd);
    }
    else
    {
        rt_kprintf("System config file is not exist ! \r");
        
        return -2;
    }
    
    
    return fd;
}


int json_cfg_close(int file)
{
    
    rt_free(cfg);
    close(file);
    return 0;
}


int json_cfg_create_PRO_UITD(char *dir, const s_PRO_UITD_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "SEN01_report_period" , p_cfg->SEN01_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN02_report_period" , p_cfg->SEN02_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN03_report_period" , p_cfg->SEN03_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN04_report_period" , p_cfg->SEN04_report_period);

        cJSON_AddNumberToObject( pJSON, "SW01_trig_cfg" , p_cfg->SW01_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW01_report_period" , p_cfg->SW01_report_period);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_PRO_CTRL(char *dir, const s_PRO_CTRL_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "SEN01_report_period" , p_cfg->SEN01_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN02_report_period" , p_cfg->SEN02_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN03_report_period" , p_cfg->SEN03_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN04_report_period" , p_cfg->SEN04_report_period);

        cJSON_AddNumberToObject( pJSON, "SW01_trig_cfg" , p_cfg->SW01_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW02_trig_cfg" , p_cfg->SW02_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW03_trig_cfg" , p_cfg->SW03_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW04_trig_cfg" , p_cfg->SW04_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW05_trig_cfg" , p_cfg->SW05_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW06_trig_cfg" , p_cfg->SW06_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW07_trig_cfg" , p_cfg->SW07_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW08_trig_cfg" , p_cfg->SW08_trig_cfg);

        cJSON_AddNumberToObject( pJSON, "SW01_report_period" , p_cfg->SW01_report_period);
        cJSON_AddNumberToObject( pJSON, "SW02_report_period" , p_cfg->SW02_report_period);
        cJSON_AddNumberToObject( pJSON, "SW03_report_period" , p_cfg->SW03_report_period);
        cJSON_AddNumberToObject( pJSON, "SW04_report_period" , p_cfg->SW04_report_period);
        cJSON_AddNumberToObject( pJSON, "SW05_report_period" , p_cfg->SW05_report_period);
        cJSON_AddNumberToObject( pJSON, "SW06_report_period" , p_cfg->SW06_report_period);
        cJSON_AddNumberToObject( pJSON, "SW07_report_period" , p_cfg->SW07_report_period);
        cJSON_AddNumberToObject( pJSON, "SW08_report_period" , p_cfg->SW08_report_period);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_AP01(char *dir, const s_AP01_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "SEN01_report_period" , p_cfg->SEN01_report_period);
        cJSON_AddNumberToObject( pJSON, "SEN02_report_period" , p_cfg->SEN02_report_period);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_eth(char *dir, const s_eth_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "if_DHCP" , p_cfg->if_DHCP);
        cJSON_AddStringToObject(pJSON, "IP" , p_cfg->IP);
        cJSON_AddStringToObject(pJSON, "GW" , p_cfg->GW);
        cJSON_AddStringToObject(pJSON, "MASK" , p_cfg->MASK);
        cJSON_AddStringToObject(pJSON, "DNS" , p_cfg->DNS);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_transparent(char *dir, const s_transparent_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject(pJSON, "trans_period" , p_cfg->trans_period);
        cJSON_AddNumberToObject(pJSON, "buffer_period" , p_cfg->buffer_period);
        cJSON_AddStringToObject(pJSON, "uart" , p_cfg->uart);
        cJSON_AddNumberToObject(pJSON, "baudrate" , p_cfg->cfg.baudrate);
        cJSON_AddNumberToObject(pJSON, "bits" , p_cfg->cfg.bits);
        cJSON_AddNumberToObject(pJSON, "verify" , p_cfg->cfg.verify);
        cJSON_AddNumberToObject(pJSON, "stopbit" , p_cfg->cfg.stopbit);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_mode(char *dir)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        
        if ((sys_config.mother_type == mother_type_GPRS_1_2) || (sys_config.mother_type == mother_type_GPRS))
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_GPRS.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_GPRS.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_GPRS.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_GPRS.WIFI);
        }
        else if (sys_config.mother_type == mother_type_ETH)
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_ETH.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_ETH.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_ETH.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_ETH.WIFI);
        }
        else if (sys_config.mother_type == mother_type_GPRS_ETH)
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_GPRS_ETH.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_GPRS_ETH.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_GPRS_ETH.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_GPRS_ETH.WIFI);
        }
        else if (sys_config.mother_type == mother_type_IOT_PRO)
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_IOT_PRO.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_IOT_PRO.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_IOT_PRO.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_IOT_PRO.WIFI);
        }
        else if (sys_config.mother_type == mother_type_EC_AP)
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_EC_AP.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_EC_AP.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_EC_AP.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_EC_AP.WIFI);
        }
        else if (sys_config.mother_type == mother_type_EC_TERM)
        {
            cJSON_AddNumberToObject( pJSON, "mode" , mode_cfg_init_data_EC_TERM.mode);
            cJSON_AddNumberToObject( pJSON, "GPRS" , mode_cfg_init_data_EC_TERM.GPRS);
            cJSON_AddNumberToObject( pJSON, "ETHERNET" , mode_cfg_init_data_EC_TERM.ETHERNET);
            cJSON_AddNumberToObject( pJSON, "WIFI" , mode_cfg_init_data_EC_TERM.WIFI);
        }
        
        
        
//        cJSON_AddNumberToObject( pJSON, "mode" , p_cfg->mode);
//        cJSON_AddNumberToObject( pJSON, "GPRS" , p_cfg->GPRS);
//        cJSON_AddNumberToObject( pJSON, "ETHERNET" , p_cfg->ETHERNET);
//        cJSON_AddNumberToObject( pJSON, "WIFI" , p_cfg->WIFI);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

int json_cfg_create_WIFI(char *dir, const s_WIFI_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddStringToObject(pJSON, "SSID" , p_cfg->SSID);
        cJSON_AddStringToObject(pJSON, "PW" , p_cfg->PW);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

//int json_cfg_create_AP02(char *dir, const s_AP02_cfg *p_cfg)
//{
//    
//    int res = 0;
//    int wr_res = 0;
//    int len = 0;
//    cJSON *pJSON = NULL;
//    char *json_buf = NULL;
//    int fd;
//    struct stat *state = NULL;
//    DIR *cfg_dir;
//    //uint8_t *cfg = NULL;
//    
//    cfg_dir = opendir(SYS_CFG_DIR);
//    if (cfg_dir == NULL)
//    {
//        mkdir(SYS_CFG_DIR, 0);
//    }
//    closedir(cfg_dir);
//    
//    // Create the config file.
//    fd = open( dir, O_CREAT | O_RDWR, 0);
//    if (fd >= 0)
//    {
//        pJSON = cJSON_CreateObject();
//        // Add the system config item to cJSON.
//        cJSON_AddNumberToObject( pJSON, "FA_type" , p_cfg->FA_type);
//        cJSON_AddNumberToObject( pJSON, "FA_baud" , p_cfg->FA_baud);
//        cJSON_AddNumberToObject( pJSON, "FA_listen" , p_cfg->FA_listen);
//        cJSON_AddNumberToObject( pJSON, "SW01_trig_cfg" , p_cfg->SW01_trig_cfg);
//        cJSON_AddNumberToObject( pJSON, "SW01_report_period" , p_cfg->SW01_report_period);

//        // Print the cJSON to string.
//        json_buf = cJSON_Print(pJSON);
//        cJSON_Delete(pJSON);
//        
//        if (json_buf != NULL)
//        {
//            len = strlen(json_buf);
//            // Write string to config file.
//            wr_res = write(fd, json_buf, len);
//            if (wr_res != len)
//            {
//                rt_kprintf("%s write error!\n", dir);
//            }
//            rt_free(json_buf);
//        }
//        else
//        {
//            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
//            close(fd);
//            return -8;
//        }
//    }
//    else
//    {
//        rt_kprintf("Create config file failed ! \n");
//        return fd;
//    }
//    
//    close(fd);
//    
//    return 0;
//}


int json_cfg_create_FA(char *dir, const s_FA_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        
        cJSON_AddNumberToObject( pJSON, "FA_en_uart0" ,     p_cfg->FA_en_uart0);
        cJSON_AddStringToObject( pJSON, "FA_name_uart0" ,   p_cfg->FA_name_uart0);
        cJSON_AddNumberToObject( pJSON, "FA_type_uart0" ,   p_cfg->FA_type_uart0);
        cJSON_AddNumberToObject( pJSON, "FA_baud_uart0" ,   p_cfg->FA_baud_uart0);
        cJSON_AddNumberToObject( pJSON, "FA_listen_uart0" , p_cfg->FA_listen_uart0);

        cJSON_AddNumberToObject( pJSON, "FA_en_uart1" ,     p_cfg->FA_en_uart1);
        cJSON_AddStringToObject( pJSON, "FA_name_uart1" ,   p_cfg->FA_name_uart1);
        cJSON_AddNumberToObject( pJSON, "FA_type_uart1" ,   p_cfg->FA_type_uart1);
        cJSON_AddNumberToObject( pJSON, "FA_baud_uart1" ,   p_cfg->FA_baud_uart1);
        cJSON_AddNumberToObject( pJSON, "FA_listen_uart1" , p_cfg->FA_listen_uart1);

        cJSON_AddNumberToObject( pJSON, "FA_en_uart2" ,     p_cfg->FA_en_uart2);
        cJSON_AddStringToObject( pJSON, "FA_name_uart2" ,   p_cfg->FA_name_uart2);
        cJSON_AddNumberToObject( pJSON, "FA_type_uart2" ,   p_cfg->FA_type_uart2);
        cJSON_AddNumberToObject( pJSON, "FA_baud_uart2" ,   p_cfg->FA_baud_uart2);
        cJSON_AddNumberToObject( pJSON, "FA_listen_uart2" , p_cfg->FA_listen_uart2);

        cJSON_AddNumberToObject( pJSON, "FA_en_uart3" ,     p_cfg->FA_en_uart3);
        cJSON_AddStringToObject( pJSON, "FA_name_uart3" ,   p_cfg->FA_name_uart3);
        cJSON_AddNumberToObject( pJSON, "FA_type_uart3" ,   p_cfg->FA_type_uart3);
        cJSON_AddNumberToObject( pJSON, "FA_baud_uart3" ,   p_cfg->FA_baud_uart3);
        cJSON_AddNumberToObject( pJSON, "FA_listen_uart3" , p_cfg->FA_listen_uart3);

        cJSON_AddNumberToObject( pJSON, "FA_en_uart4" ,     p_cfg->FA_en_uart4);
        cJSON_AddStringToObject( pJSON, "FA_name_uart4" ,   p_cfg->FA_name_uart4);
        cJSON_AddNumberToObject( pJSON, "FA_type_uart4" ,   p_cfg->FA_type_uart4);
        cJSON_AddNumberToObject( pJSON, "FA_baud_uart4" ,   p_cfg->FA_baud_uart4);
        cJSON_AddNumberToObject( pJSON, "FA_listen_uart4" , p_cfg->FA_listen_uart4);

        cJSON_AddNumberToObject( pJSON, "SW01_trig_cfg" ,   p_cfg->SW01_trig_cfg);
        cJSON_AddNumberToObject( pJSON, "SW01_report_period" , p_cfg->SW01_report_period);

        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("%s write error!\n", dir);
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}



int json_cfg_create(char *dir, const struct t_sys_cfg *p_cfg)
{
    
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON = NULL;
    char *json_buf = NULL;
    int fd;
    struct stat *state = NULL;
    DIR *cfg_dir;
    //uint8_t *cfg = NULL;
    
    cfg_dir = opendir(SYS_CFG_DIR);
    if (cfg_dir == NULL)
    {
        mkdir(SYS_CFG_DIR, 0);
    }
    closedir(cfg_dir);
    
    // Create the config file.
    fd = open( dir, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "sys_cfg" , p_cfg->sys_cfg);
        cJSON_AddNumberToObject( pJSON, "sys_mode" , p_cfg->sys_mode);
//        cJSON_AddNumberToObject( pJSON, "A8_if_BR", p_cfg->A8_if_BR);
//        cJSON_AddNumberToObject( pJSON, "uart2_BR", p_cfg->uart2_BR);
//        cJSON_AddStringToObject( pJSON, "SSID", p_cfg->SSID);
//        cJSON_AddStringToObject( pJSON, "key", p_cfg->key);
        cJSON_AddNumberToObject( pJSON, "server_num" , p_cfg->svr_num);
        cJSON_AddNumberToObject( pJSON, "svr_use_name", p_cfg->svr_use_name);
        cJSON_AddStringToObject( pJSON, "svr_if_name", p_cfg->svr_if_name);
        cJSON_AddStringToObject( pJSON, "svr_if_ip", p_cfg->svr_if_ip);
        cJSON_AddNumberToObject( pJSON, "svr_if_port", p_cfg->svr_if_port);
        cJSON_AddNumberToObject( pJSON, "if_UITD", p_cfg->if_UITD);
        cJSON_AddNumberToObject( pJSON, "svr_2_use_name", p_cfg->svr_2_use_name);
        cJSON_AddStringToObject( pJSON, "svr_2_if_name", p_cfg->svr_2_if_name);
        cJSON_AddStringToObject( pJSON, "svr_2_if_ip", p_cfg->svr_2_if_ip);
        cJSON_AddNumberToObject( pJSON, "svr_2_if_port", p_cfg->svr_2_if_port);
        cJSON_AddNumberToObject( pJSON, "if_UITD_2", p_cfg->if_UITD_2);
        
        cJSON_AddNumberToObject( pJSON, "debug_level", p_cfg->debug_level);
        cJSON_AddNumberToObject( pJSON, "log_file_en", p_cfg->log_file_en);
        cJSON_AddStringToObject( pJSON, "log_file_path", p_cfg->log_file_path);
        
//        cJSON_AddNumberToObject( pJSON, "sensor_period_1", p_cfg->sensor_period_1);
//        cJSON_AddNumberToObject( pJSON, "sensor_period_2", p_cfg->sensor_period_2);
//        cJSON_AddNumberToObject( pJSON, "auto_update", p_cfg->auto_update);
//        cJSON_AddNumberToObject( pJSON, "force_check_up", p_cfg->force_check_up);
//        cJSON_AddNumberToObject( pJSON, "force_up_wifi", p_cfg->force_up_wifi);
//        cJSON_AddNumberToObject( pJSON, "force_up_sensor", p_cfg->force_up_sensor);
//        cJSON_AddNumberToObject( pJSON, "server_if_en", p_cfg->server_if_en);
//        cJSON_AddNumberToObject( pJSON, "wifi_auto_en", p_cfg->wifi_auto_en);
//        
//        cJSON_AddNumberToObject( pJSON, "RT_peri", p_cfg->RT_peri);
//        cJSON_AddNumberToObject( pJSON, "RT_len", p_cfg->RT_len);
//        cJSON_AddNumberToObject( pJSON, "Nor_len", p_cfg->Nor_len);
//        cJSON_AddNumberToObject( pJSON, "wifi_roam", p_cfg->wifi_roam);
        
        // Print the cJSON to string.
        json_buf = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (json_buf != NULL)
        {
            len = strlen(json_buf);
            // Write string to config file.
            wr_res = write(fd, json_buf, len);
            if (wr_res != len)
            {
                rt_kprintf("sys.cfg write error!\n");
            }
            rt_free(json_buf);
        }
        else
        {
            rt_kprintf("cJSON_Print output buf malloc failed ! \n");
            close(fd);
            return -8;
        }
    }
    else
    {
        rt_kprintf("Create config file failed ! \n");
        return fd;
    }
    
    close(fd);
    
    return 0;
}

// Song: TODO
int json_cfg_load_PRO_UITD(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

PRO_UITD_cfg_load_start:
    
    res = json_cfg_open( PRO_UITD_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN01_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN02_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[1] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SEN03_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[2] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SEN04_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[3] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[0] = JSON_item->valueint;


        
        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_PRO_UITD(PRO_UITD_CFG_FILE_PATH, &PRO_UITD_cfg_init_data);
        
        res = json_cfg_open( PRO_UITD_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto PRO_UITD_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create PRO_UITD.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

int json_cfg_load_PRO_CTRL(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

PRO_CTRL_cfg_load_start:
    
    res = json_cfg_open( PRO_CTRL_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN01_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN02_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[1] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SEN03_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[2] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SEN04_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[3] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[0] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW02_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[1] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW02_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[1] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW03_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[2] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW03_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[2] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW04_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[3] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW04_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[3] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW05_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[4] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW05_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[4] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW06_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[5] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW06_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[5] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW07_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[6] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW07_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[6] = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW08_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[7] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW08_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[7] = JSON_item->valueint;
        
        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_PRO_CTRL(PRO_CTRL_CFG_FILE_PATH, &PRO_CTRL_cfg_init_data);
        
        res = json_cfg_open( PRO_CTRL_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto PRO_CTRL_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create PRO_CTRL.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

int json_cfg_load_AP01(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

AP01_cfg_load_start:
    
    res = json_cfg_open( AP01_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN01_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SEN02_report_period");
        if (JSON_item != NULL) 
            g_sensor_period[1] = JSON_item->valueint;

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_AP01(AP01_CFG_FILE_PATH, &AP01_cfg_init_data);
        
        res = json_cfg_open( AP01_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto AP01_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create AP01.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

int json_cfg_load_eth(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

eth_cfg_load_start:
    
    res = json_cfg_open( ETH_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "if_DHCP");
        if (JSON_item != NULL) 
            eth_cfg.if_DHCP = 0;//JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "IP");
        if (JSON_item != NULL) 
            rt_memcpy(eth_cfg.IP, JSON_item->valuestring, sizeof(eth_cfg.IP));
				//rt_memcpy(eth_cfg.IP, "192.168.3.217", sizeof(eth_cfg.IP));

        JSON_item = cJSON_GetObjectItem(pJSON, "GW");
        if (JSON_item != NULL) 
            rt_memcpy(eth_cfg.GW, JSON_item->valuestring, sizeof(eth_cfg.GW));
				//rt_memcpy(eth_cfg.GW, "192.168.3.1", sizeof(eth_cfg.GW));

        JSON_item = cJSON_GetObjectItem(pJSON, "MASK");
        if (JSON_item != NULL) 
            rt_memcpy(eth_cfg.MASK, JSON_item->valuestring, sizeof(eth_cfg.MASK));
				//rt_memcpy(eth_cfg.MASK, "255.255.255.0", sizeof(eth_cfg.MASK));
        
        JSON_item = cJSON_GetObjectItem(pJSON, "DNS");
        if (JSON_item != NULL) 
            rt_memcpy(eth_cfg.DNS, JSON_item->valuestring, sizeof(eth_cfg.DNS));
				//rt_memcpy(eth_cfg.DNS, "202.96.209.5", sizeof(eth_cfg.DNS));

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_eth(ETH_CFG_FILE_PATH, &eth_cfg_init_data);
        
        res = json_cfg_open( ETH_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto eth_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create eth.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

int json_cfg_load_mode(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

mode_cfg_load_start:
    
    res = json_cfg_open( MODE_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "mode");
        if (JSON_item != NULL) 
            mode_cfg.mode = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "GPRS");
        if (JSON_item != NULL) 
            mode_cfg.GPRS = 0;//JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "ETHERNET");
        if (JSON_item != NULL) 
            mode_cfg.ETHERNET = 1;//JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "WIFI");
        if (JSON_item != NULL) 
            mode_cfg.WIFI = JSON_item->valueint;
        

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_mode(MODE_CFG_FILE_PATH);
        
        res = json_cfg_open( MODE_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto mode_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create mode.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

int json_cfg_load_WIFI(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

WIFI_cfg_load_start:
    
    res = json_cfg_open( WIFI_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SSID");
        if (JSON_item != NULL) 
            rt_memcpy(WIFI_cfg.SSID, JSON_item->valuestring, sizeof(WIFI_cfg.SSID));

        JSON_item = cJSON_GetObjectItem(pJSON, "PW");
        if (JSON_item != NULL) 
            rt_memcpy(WIFI_cfg.PW, JSON_item->valuestring, sizeof(WIFI_cfg.PW));

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_WIFI(WIFI_CFG_FILE_PATH, &WIFI_cfg_init_data);
        
        res = json_cfg_open( WIFI_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto WIFI_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create WIFI.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}

//int json_cfg_load_AP02(s_FA_uart_cfg *cfg)
//{
//    int res = 0;
//    cJSON *pJSON;
//    cJSON *JSON_item ;
//    int value = 0;

//AP02_cfg_load_start:
//    
//    res = json_cfg_open( AP02_CFG_FILE_PATH, O_RDONLY);
//    if (res >= 0)
//    {
//        // Parse the config.
//        pJSON = cJSON_Parse(cfg);
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "FA_type");
//        if (JSON_item != NULL) 
//            g_FA_type = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "FA_baud");
//        if (JSON_item != NULL) 
//            g_FA_baud = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "FA_listen");
//        if (JSON_item != NULL) 
//            g_FA_listen = JSON_item->valueint;

//        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_trig_cfg");
//        if (JSON_item != NULL) 
//            g_INPUT_trig_cfg[0] = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_report_period");
//        if (JSON_item != NULL) 
//            g_INPUT_report_period[0] = JSON_item->valueint;

//        json_cfg_close(res);
//    }
//    else if (res == -2) // config file is not exist.
//    {
//        // Create a default config file.
//        json_cfg_create_AP02(AP02_CFG_FILE_PATH, &AP02_cfg_init_data);
//        
//        res = json_cfg_open( AP02_CFG_FILE_PATH, O_RDONLY);
//        if (res >= 0)
//        {
//            json_cfg_close(res);
//            goto AP02_cfg_load_start;
//        }
//        else
//        {
//            SYS_log(SYS_DEBUG_ERROR, ("Create AP02.cfg failed !!!\n"));
//            return -1;
//        }
//    }
//    
//    
//    strcpy(cfg[0].FA_name, UITD_UART_COM_BUS); 
//    cfg[0].FA_baud = g_FA_baud;
//    cfg[0].FA_listen = g_FA_listen;
//    
//    FA_uart_num = 1;
//    
//    return 0;
//}

int json_cfg_load_FA(s_FA_uart_cfg *p_cfg, uint8_t *FA_uart_num)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;
    int i = 0;
    int j = 0;
    uint8_t json_buf[32] = {0};
    int FA_en = 0;

FA_cfg_load_start:
    
    res = json_cfg_open( FA_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        for (i=0;i<FA_COM_BUS_MAX;i++)
        {
            
            sprintf(json_buf, "FA_en_uart%d", i);
            JSON_item = cJSON_GetObjectItem(pJSON, json_buf);
            if (JSON_item != NULL) 
                FA_en = JSON_item->valueint;
            else
                continue;
            
            if (FA_en)
            {
                sprintf(json_buf, "FA_name_uart%d", i);
                JSON_item = cJSON_GetObjectItem(pJSON, json_buf);
                if (JSON_item != NULL) 
                    rt_memcpy(p_cfg[j].FA_name, JSON_item->valuestring, sizeof(p_cfg[j].FA_name));
                    //cfg[j].FA_name = JSON_item->valueint;

                sprintf(json_buf, "FA_type_uart%d", i);
                JSON_item = cJSON_GetObjectItem(pJSON, json_buf);
                if (JSON_item != NULL) 
                    p_cfg[j].FA_type = JSON_item->valueint;

                sprintf(json_buf, "FA_baud_uart%d", i);
                JSON_item = cJSON_GetObjectItem(pJSON, json_buf);
                if (JSON_item != NULL) 
                    p_cfg[j].FA_baud = JSON_item->valueint;
                
                sprintf(json_buf, "FA_listen_uart%d", i);
                JSON_item = cJSON_GetObjectItem(pJSON, json_buf);
                if (JSON_item != NULL) 
                    p_cfg[j].FA_listen = JSON_item->valueint;

                j ++;
            }
        }
        *FA_uart_num = j;

        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_trig_cfg");
        if (JSON_item != NULL) 
            g_INPUT_trig_cfg[0] = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "SW01_report_period");
        if (JSON_item != NULL) 
            g_INPUT_report_period[0] = JSON_item->valueint;

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_FA(FA_CFG_FILE_PATH, &FA_cfg_init_data);
        
        res = json_cfg_open( FA_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto FA_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create %s failed !!!\n", FA_CFG_FILE_PATH));
            return -1;
        }
    }
    
    return 0;
}




int json_cfg_load_transparent(s_transparent_cfg *p_cfg)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;

trans_cfg_load_start:
    
    res = json_cfg_open( TRANS_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "trans_period");
        if (JSON_item != NULL) 
            p_cfg->trans_period = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "buffer_period");
        if (JSON_item != NULL) 
            p_cfg->buffer_period = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "uart");
        if (JSON_item != NULL) 
            rt_memcpy(p_cfg->uart, JSON_item->valuestring, sizeof(p_cfg->uart));

        JSON_item = cJSON_GetObjectItem(pJSON, "baudrate");
        if (JSON_item != NULL) 
            p_cfg->cfg.baudrate = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "bits");
        if (JSON_item != NULL) 
            p_cfg->cfg.bits = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "verify");
        if (JSON_item != NULL) 
            p_cfg->cfg.verify = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "stopbit");
        if (JSON_item != NULL) 
            p_cfg->cfg.stopbit = JSON_item->valueint;

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create_transparent(TRANS_CFG_FILE_PATH, &transparent_cfg_init_data);
        
        res = json_cfg_open( TRANS_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto trans_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create transparent.cfg failed !!!\n"));
            return -1;
        }
    }
    
    return 0;
}



int json_cfg_load(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;
    
    //cfg = 0;
sys_cfg_load_start:
    
    res = json_cfg_open( SYS_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "sys_cfg");
        if (JSON_item != NULL) 
            sys_cfg = JSON_item->valueint;
        
        if (sys_config.sys_mode != sys_mode_testing) // If detected a testing tool, no care the config file.
        {
            JSON_item = cJSON_GetObjectItem(pJSON, "sys_mode");
            if (JSON_item != NULL) 
                sys_config.sys_mode = JSON_item->valueint;
        }
        
        JSON_item = cJSON_GetObjectItem(pJSON, "server_num");
        if (JSON_item != NULL) 
            sys_config.server_num = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_use_name");
        if (JSON_item != NULL) 
            sys_config.server_SZJS.svr_use_name = 0;//JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_name");
        if (JSON_item != NULL) 
            memcpy( sys_config.server_SZJS.svr_if_name, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);
				//memcpy( sys_config.server_SZJS.svr_if_name, "platform.kingsum.biz", strlen("platform.kingsum.biz")+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_ip");
        if (JSON_item != NULL) 
            memcpy( sys_config.server_SZJS.svr_if_ip, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_port");
        if (JSON_item != NULL) 
            sys_config.server_SZJS.svr_if_port = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "if_UITD");
        if (JSON_item != NULL) 
            sys_config.server_SZJS.if_UITD = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_2_use_name");
        if (JSON_item != NULL) 
            sys_config.server_XFZD.svr_use_name = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_2_if_name");
        if (JSON_item != NULL) 
            memcpy( sys_config.server_XFZD.svr_if_name, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_2_if_ip");
        if (JSON_item != NULL) 
            memcpy( sys_config.server_XFZD.svr_if_ip, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_2_if_port");
        if (JSON_item != NULL) 
            sys_config.server_XFZD.svr_if_port = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "if_UITD_2");
        if (JSON_item != NULL) 
            sys_config.server_XFZD.if_UITD = JSON_item->valueint;
        
        JSON_item = cJSON_GetObjectItem(pJSON, "debug_level");
        if (JSON_item != NULL) 
            sys_config.debug_level = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "log_file_en");
        if (JSON_item != NULL) 
            sys_config.log_file_en = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "log_file_path");
        if (JSON_item != NULL) 
            memcpy( sys_config.log_file_path, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);
        
        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
        
        res = json_cfg_open( SYS_CFG_FILE_PATH, O_RDONLY);
        if (res >= 0)
        {
            json_cfg_close(res);
            goto sys_cfg_load_start;
        }
        else
        {
            SYS_log(SYS_DEBUG_ERROR, ("Create sys.cfg failed !!!\n"));
            return -1;
        }
        
    }
    
    return 0;
}

// type: 1:int; 2:string;
int json_cfg_write(char *name, void *value, int type)
{
    int res = 0;
    int wr_res = 0;
    int len = 0;
    cJSON *pJSON;
    cJSON *JSON_item;
    char *new_cfg;
    
    
    res = json_cfg_open( SYS_CFG_FILE_PATH, DFS_O_RDWR);
    if (res >= 0)
    {

        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        json_cfg_close(res);
        
        
        JSON_item = cJSON_GetObjectItem(pJSON, name);
        
        if (type == 1)
        {
            // Song: note: you must change the valuedouble at least to change the item value.
            JSON_item->valuedouble = *(int *)value;
        }
        else if (type == 2)
        {
            memcpy(JSON_item->valuestring, value, strlen(value) + 1);
        }
            
      
        new_cfg = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (new_cfg != NULL)
        {
            //rm(SYS_CFG_FILE_PATH);
            res = open( SYS_CFG_FILE_PATH, DFS_O_WRONLY | DFS_O_TRUNC, 0);
            lseek(res, 0, DFS_SEEK_SET );
            // Write string to config file.
            len = strlen(new_cfg);
            wr_res = write(res, new_cfg, len);
            if (wr_res != len)
            {
                rt_kprintf("sys.cfg file write error ! \n");
            }
            close(res);
            
            rt_free(new_cfg);
        }
        else
        {
            rt_kprintf("System config write update failed !\n");
        }
        
    }
    
    return 0;
}




// 
//int json_cfg_write_mult(struct t_json_value *json_value, int num)
//{
//    //uint8_t *cfg = NULL;
//    int res = 0;
//    cJSON *pJSON;
//    cJSON *JSON_item ;
//    char *new_cfg;
//    int i;
//    

//    
//    res = json_cfg_open( SYS_CFG_FILE_PATH, DFS_O_RDWR);
//    if (res >= 0)
//    {

//        // Parse the config.
//        pJSON = cJSON_Parse(cfg);
//        json_cfg_close(res);
//        
//        
//        for (i=0;i<num;i++)
//        {
//            JSON_item = cJSON_GetObjectItem(pJSON, json_value[i].name);
//            
//            if (json_value[i].type == 1)
//            {
//                // Song: note: you must change the valuedouble at least to change the item value.
//                JSON_item->valuedouble = json_value[i].value;
//            }
//            else if (json_value[i].type == 2)
//            {
//                memcpy(JSON_item->valuestring, json_value[i].string, strlen(json_value[i].string) + 1);
//            }
//            
//        }
//        
////        JSON_item = cJSON_GetObjectItem(pJSON, name);
////        // Song: note: you must change the valuedouble at least to change the item value.
////        JSON_item->valuedouble = update;
//        
//        //cJSON_ReplaceItemInObject(pJSON, "sys_cfg", JSON_item);
//       
//        new_cfg = cJSON_Print(pJSON);
//        cJSON_Delete(pJSON);
//        
//        if (new_cfg != NULL)
//        {
//            rm(SYS_CFG_FILE_PATH);
//            res = open( SYS_CFG_FILE_PATH, DFS_O_CREAT | DFS_O_WRONLY, 0);
//            // Write string to config file.
//            write(res, new_cfg, strlen(new_cfg));
//            close(res);
//            
//            rt_free(new_cfg);
//        }
//        else
//        {
//            rt_kprintf("System config write update failed !\n");
//        }
//        
//    }
//    
//    return 0;
//}


int json_cfg_write_mult_ext(char *cfg_file, struct t_json_value *json_value, int num)
{
    //uint8_t *cfg = NULL;
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    char *new_cfg;
    int i;
    

    
    res = json_cfg_open( cfg_file, DFS_O_RDWR);
    if (res >= 0)
    {

        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        json_cfg_close(res);
        
        
        for (i=0;i<num;i++)
        {
            JSON_item = cJSON_GetObjectItem(pJSON, json_value[i].name);
            
            if (json_value[i].type == 1)
            {
                // Song: note: you must change the valuedouble at least to change the item value.
                JSON_item->valuedouble = json_value[i].value;
            }
            else if (json_value[i].type == 2)
            {
                memcpy(JSON_item->valuestring, json_value[i].string, strlen(json_value[i].string) + 1);
            }
            
        }
        
//        JSON_item = cJSON_GetObjectItem(pJSON, name);
//        // Song: note: you must change the valuedouble at least to change the item value.
//        JSON_item->valuedouble = update;
        
        //cJSON_ReplaceItemInObject(pJSON, "sys_cfg", JSON_item);
       
        new_cfg = cJSON_Print(pJSON);
        cJSON_Delete(pJSON);
        
        if (new_cfg != NULL)
        {
            rm(cfg_file);
            res = open( cfg_file, DFS_O_CREAT | DFS_O_WRONLY, 0);
            // Write string to config file.
            write(res, new_cfg, strlen(new_cfg));
            close(res);
            
            rt_free(new_cfg);
        }
        else
        {
            rt_kprintf("System config write update failed !\n");
        }
        
    }
    
    return 0;
}

int json_cfg_wr_sys_cfg(int sys_cfg)
{
    return json_cfg_write("sys_cfg", &sys_cfg, 1);
}

//int json_cfg_wr_A8_if_BR(int A8_if_BR)
//{
//    return json_cfg_write("A8_if_BR", &A8_if_BR, 1);
//}

//int json_cfg_wr_uart2_BR(int uart2_BR)
//{
//    return json_cfg_write("uart2_BR", &uart2_BR, 1);
//}

//int json_cfg_wr_auto_update(int auto_update)
//{
//    return json_cfg_write("auto_update", &auto_update, 1);
//}

//int json_cfg_wr_force_check_up(int force_check_up)
//{
//    return json_cfg_write("force_check_up", &force_check_up, 1);
//}

//int json_cfg_wr_force_up_wifi(int force_up_wifi)
//{
//    return json_cfg_write("force_up_wifi", &force_up_wifi, 1);
//}

//int json_cfg_wr_force_up_sensor(int force_up_sensor)
//{
//    return json_cfg_write("force_up_sensor", &force_up_sensor, 1);
//}

//int json_cfg_wr_server_if_en(int server_if_en)
//{
//    return json_cfg_write("server_if_en", &server_if_en, 1);
//}

//int json_cfg_wr_wifi_auto_en(int wifi_auto_en)
//{
//    return json_cfg_write("wifi_auto_en", &wifi_auto_en, 1);
//}

//int json_cfg_wr_SSID_KEY(char *SSID, char *key)
//{
//    struct t_json_value *json_value;
//    
//    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
//    
//    json_value[0].type = 2;
//    strcpy(json_value[0].name, "SSID");
//    json_value[0].string = SSID;

//    json_value[1].type = 2;
//    strcpy(json_value[1].name, "key");
//    json_value[1].string = key;
//    
//    json_cfg_write_mult(json_value, 2);
//    
//    rt_free(json_value);
//    
//    return 0;
//}


int json_cfg_wr_sensor_period_1(int sensor_period_1)
{
    return json_cfg_write("sensor_period_1", &sensor_period_1, 1);
}


int json_cfg_wr_sensor_period_2(int sensor_period_2)
{
    return json_cfg_write("sensor_period_2", &sensor_period_2, 1);
}


int json_cfg_wr_debug_level(int debug_level)
{
    return json_cfg_write("debug_level", &debug_level, 1);
}

int json_cfg_wr_server_num(int num)
{
    return json_cfg_write("server_num", &num, 1);
}

int json_cfg_wr_log_file_en(int log_file_en)
{
    return json_cfg_write("log_file_en", &log_file_en, 1);
}

int json_cfg_wr_log_file_path(char *log_file_path)
{
    return json_cfg_write("log_file_path", log_file_path, 2);
}

int json_cfg_wr_sys_mode(int sys_mode)
{
//    if ((sys_mode != 0) && (sys_mode != 1))
//    {
//        rt_kprintf("Input data is invalid !\n");
//        return -1;
//    }
    return json_cfg_write("sys_mode", &sys_mode, 1);
}

int json_cfg_wr_svr_use_name(int svr_use_name)
{
    if ((svr_use_name != 0) && (svr_use_name != 1))
    {
        rt_kprintf("Input data is invalid !\n");
        return -1;
    }
    return json_cfg_write("svr_use_name", &svr_use_name, 1);
}

int json_cfg_wr_server_port(char *server, uint16_t port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_if_name");
    json_value[0].string = server;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_if_port");
    json_value[1].value = port;
    
    json_cfg_write_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_server_ip_port(char *server_ip, uint16_t port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_if_ip");
    json_value[0].string = server_ip;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_if_port");
    json_value[1].value = port;
    
    json_cfg_write_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_svr_use_name_2(int svr_use_name)
{
    if ((svr_use_name != 0) && (svr_use_name != 1))
    {
        rt_kprintf("Input data is invalid !\n");
        return -1;
    }
    return json_cfg_write("svr_2_use_name", &svr_use_name, 1);
}

int json_cfg_wr_server_port_2(char *server, uint16_t port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_2_if_name");
    json_value[0].string = server;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_2_if_port");
    json_value[1].value = port;
    
    json_cfg_write_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_server_ip_port_2(char *server_ip, uint16_t port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_2_if_ip");
    json_value[0].string = server_ip;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_2_if_port");
    json_value[1].value = port;
    
    json_cfg_write_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_PRO_UITD_output(uint32_t SEN01_report_period, uint32_t SEN02_report_period, uint32_t SEN03_report_period, uint32_t SEN04_report_period)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 4);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "SEN01_report_period");
    json_value[0].value = SEN01_report_period;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "SEN02_report_period");
    json_value[1].value = SEN02_report_period;

    json_value[2].type = 1;
    strcpy(json_value[2].name, "SEN03_report_period");
    json_value[2].value = SEN03_report_period;

    json_value[3].type = 1;
    strcpy(json_value[3].name, "SEN04_report_period");
    json_value[3].value = SEN04_report_period;
    
    json_cfg_write_mult_ext(PRO_UITD_CFG_FILE_PATH, json_value, 4);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_PRO_CTRL_output(uint32_t SEN01_report_period, uint32_t SEN02_report_period, uint32_t SEN03_report_period, uint32_t SEN04_report_period)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 4);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "SEN01_report_period");
    json_value[0].value = SEN01_report_period;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "SEN02_report_period");
    json_value[1].value = SEN02_report_period;

    json_value[2].type = 1;
    strcpy(json_value[2].name, "SEN03_report_period");
    json_value[2].value = SEN03_report_period;

    json_value[3].type = 1;
    strcpy(json_value[3].name, "SEN04_report_period");
    json_value[3].value = SEN04_report_period;
    
    json_cfg_write_mult_ext(PRO_CTRL_CFG_FILE_PATH, json_value, 4);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_PRO_UITD_input(uint32_t input_num, uint32_t trig_cfg, uint32_t trig_period)
{
    struct t_json_value *json_value;
    int i = 0;
    
    if (input_num > 0)
    {
        json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1 * 2);

        if (input_num == 1)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW01_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW01_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 2)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW02_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW02_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 3)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW03_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW03_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 4)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW04_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW04_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 5)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW05_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW05_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 6)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW06_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW06_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 7)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW07_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW07_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 8)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW08_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW08_report_period");
            json_value[1].value = trig_period;
        }
    }

    json_cfg_write_mult_ext(PRO_UITD_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_FA_uart( uint8_t FA_index, uint8_t FA_en, uint8_t *FA_name, uint32_t FA_type, uint32_t FA_baud, uint32_t FA_listen)
{
    uint8_t json_buf[32] = {0};
    struct t_json_value *json_value;
    
    if (FA_index >= FA_COM_BUS_MAX)
    {
        return -1;
    }

    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1 * 5);

    
    sprintf(json_buf, "FA_en_uart%d", FA_index);
    json_value[0].type = 1;
    strcpy(json_value[0].name, json_buf);
    json_value[0].value = FA_en;

    sprintf(json_buf, "FA_name_uart%d", FA_index);
    json_value[1].type = 2;
    strcpy(json_value[1].name, json_buf);
    json_value[1].string = FA_name;

    sprintf(json_buf, "FA_type_uart%d", FA_index);
    json_value[2].type = 1;
    strcpy(json_value[2].name, json_buf);
    json_value[2].value = FA_type;

    sprintf(json_buf, "FA_baud_uart%d", FA_index);
    json_value[3].type = 1;
    strcpy(json_value[3].name, json_buf);
    json_value[3].value = FA_baud;

    sprintf(json_buf, "FA_listen_uart%d", FA_index);
    json_value[4].type = 1;
    strcpy(json_value[4].name, json_buf);
    json_value[4].value = FA_listen;

    json_cfg_write_mult_ext(FA_CFG_FILE_PATH, json_value, 5);
    
    rt_free(json_value);
    
    return 0;
}
int json_cfg_wr_FA_input( uint32_t SW01_trig_cfg, uint32_t SW01_report_period)
{
    struct t_json_value *json_value;
    

    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1 * 2);

    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "SW01_trig_cfg");
    json_value[0].value = SW01_trig_cfg;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "SW01_report_period");
    json_value[1].value = SW01_report_period;

    json_cfg_write_mult_ext(FA_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_PRO_CTRL_input(uint32_t input_num, uint32_t trig_cfg, uint32_t trig_period)
{
    struct t_json_value *json_value;
    int i = 0;
    
    if (input_num > 0)
    {
        json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1 * 2);

        if (input_num == 1)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW01_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW01_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 2)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW02_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW02_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 3)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW03_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW03_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 4)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW04_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW04_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 5)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW05_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW05_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 6)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW06_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW06_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 7)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW07_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW07_report_period");
            json_value[1].value = trig_period;
        }
        else if (input_num == 8)
        {
            json_value[0].type = 1;
            strcpy(json_value[0].name, "SW08_trig_cfg");
            json_value[0].value = trig_cfg;

            json_value[1].type = 1;
            strcpy(json_value[1].name, "SW08_report_period");
            json_value[1].value = trig_period;
        }
    }

    json_cfg_write_mult_ext(PRO_CTRL_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_AP01(uint32_t SEN01_report_period, uint32_t SEN02_report_period)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 2);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "SEN01_report_period");
    json_value[0].value = SEN01_report_period;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "SEN02_report_period");
    json_value[1].value = SEN02_report_period;
    
    json_cfg_write_mult_ext(AP01_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}

//int json_cfg_wr_AP02(uint32_t FA_type, uint32_t FA_baud, uint32_t FA_listen, uint8_t SW01_trig_cfg, uint32_t SW01_report_period)
//{
//    struct t_json_value *json_value;
//    
//    // Malloc json_value struct buffer * 5
//    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 5);
//    
//    json_value[0].type = 1;
//    strcpy(json_value[0].name, "FA_type");
//    json_value[0].value = FA_type;

//    json_value[1].type = 1;
//    strcpy(json_value[1].name, "FA_baud");
//    json_value[1].value = FA_baud;
//    
//    json_value[2].type = 1;
//    strcpy(json_value[2].name, "FA_listen");
//    json_value[2].value = FA_listen;

//    json_value[3].type = 1;
//    strcpy(json_value[3].name, "SW01_trig_cfg");
//    json_value[3].value = SW01_trig_cfg;

//    json_value[4].type = 1;
//    strcpy(json_value[4].name, "SW01_report_period");
//    json_value[4].value = SW01_report_period;

//    json_cfg_write_mult_ext(AP02_CFG_FILE_PATH, json_value, 5);
//    
//    rt_free(json_value);
//    
//    return 0;
//}

int json_cfg_wr_WIFI(uint8_t *SSID, uint8_t *PW)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 5
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "SSID");
    json_value[0].string = SSID;

    json_value[1].type = 2;
    strcpy(json_value[1].name, "PW");
    json_value[1].string = PW;
    
    json_cfg_write_mult_ext(WIFI_CFG_FILE_PATH, json_value, 2);
    
    rt_free(json_value);
    
    return 0;
}


int json_cfg_wr_transparent(uint16_t trans_period, 
                            uint16_t buffer_period, 
                            uint8_t *uart, 
                            uint32_t baudrate, 
                            uint8_t bits, 
                            uint8_t verify, 
                            uint8_t stopbit)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 5
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 7);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "trans_period");
    json_value[0].value = trans_period;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "buffer_period");
    json_value[1].value = buffer_period;

    json_value[2].type = 2;
    strcpy(json_value[2].name, "uart");
    json_value[2].string = uart;

    json_value[3].type = 1;
    strcpy(json_value[3].name, "baudrate");
    json_value[3].value = baudrate;

    json_value[4].type = 1;
    strcpy(json_value[4].name, "bits");
    json_value[4].value = bits;

    json_value[5].type = 1;
    strcpy(json_value[5].name, "verify");
    json_value[5].value = verify;

    json_value[6].type = 1;
    strcpy(json_value[6].name, "stopbit");
    json_value[6].value = stopbit;

    json_cfg_write_mult_ext(TRANS_CFG_FILE_PATH, json_value, 7);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_eth_DHCP(uint32_t if_DHCP)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "if_DHCP");
    json_value[0].value = if_DHCP;
    
    json_cfg_write_mult_ext(ETH_CFG_FILE_PATH, json_value, 1);
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_wr_eth_DNS(uint8_t *DNS)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 1);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "DNS");
    json_value[0].string = DNS;

    
    json_cfg_write_mult_ext(ETH_CFG_FILE_PATH, json_value, 1);
    
    rt_free(json_value);
    
    return 0;
}


int json_cfg_wr_eth_IP(uint8_t *IP, uint8_t *GW, uint8_t *MASK)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 3);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "IP");
    json_value[0].string = IP;

    json_value[1].type = 2;
    strcpy(json_value[1].name, "GW");
    json_value[1].string = GW;

    json_value[2].type = 2;
    strcpy(json_value[2].name, "MASK");
    json_value[2].string = MASK;
    
    json_cfg_write_mult_ext(ETH_CFG_FILE_PATH, json_value, 3);
    
    rt_free(json_value);
    
    return 0;
}


int json_cfg_wr_mode(uint32_t mode, uint32_t GPRS, uint32_t ETHERNET, uint32_t WIFI)
{
    struct t_json_value *json_value;
    
    // Malloc json_value struct buffer * 2
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 4);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "mode");
    json_value[0].value = mode;
    
    json_value[1].type = 1;
    strcpy(json_value[1].name, "GPRS");
    json_value[1].value = GPRS;

    json_value[2].type = 1;
    strcpy(json_value[2].name, "ETHERNET");
    json_value[2].value = ETHERNET;

    json_value[3].type = 1;
    strcpy(json_value[3].name, "WIFI");
    json_value[3].value = WIFI;

    json_cfg_write_mult_ext(MODE_CFG_FILE_PATH, json_value, 4);
    
    rt_free(json_value);
    
    return 0;
}

//int json_cfg_wr_RT_NOR_para(uint32_t RT_peri, uint32_t RT_len, uint32_t Nor_len)
//{
//    
//    json_cfg_write("RT_peri", &RT_peri, 1);
//    json_cfg_write("RT_len", &RT_len, 1);
//    
//    if (Nor_len % DATA_STORE_BUF_NUM)
//    {
//        rt_kprintf("Nor_len write failed ! Nor_len must be times of %d", DATA_STORE_BUF_NUM);
//    }
//    else
//    {
//        json_cfg_write("Nor_len", &Nor_len, 1);
//    }
//    return 0;
//}


//int json_cfg_wr_wifi_roam(int32_t wifi_roam)
//{
//    json_cfg_write("wifi_roam", &wifi_roam, 1);
//    return 0;
//}

// type: 1:int; 2:string;
int json_cfg_read(char *name, void *value, int type)
{
    //uint8_t *cfg = NULL;
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    char *new_cfg;
    int i;
    
    res = json_cfg_open( SYS_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        //for (i=0;i<num;i++)
        {
            JSON_item = cJSON_GetObjectItem(pJSON, name);
            
            if (type == 1)
            {
               *(int *)value = JSON_item->valuedouble;
            }
            else if (type == 2)
            {
                memcpy( value, JSON_item->valuestring, strlen(JSON_item->valuestring) + 1);
            }
        }

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        rt_kprintf("System config read failed !\n");
    }
    
    return 0;
}

int json_cfg_read_mult_ext(char *cfg_file, struct t_json_value *json_value, int num)
{
    //uint8_t *cfg = NULL;
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    char *new_cfg;
    int i;
    
    res = json_cfg_open( cfg_file, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        for (i=0;i<num;i++)
        {
            JSON_item = cJSON_GetObjectItem(pJSON, json_value[i].name);
            
            if (json_value[i].type == 1)
            {
               json_value[i].value = JSON_item->valuedouble;
            }
            else if (json_value[i].type == 2)
            {
                memcpy( json_value[i].string, JSON_item->valuestring, strlen(JSON_item->valuestring) + 1);
            }
        }

        json_cfg_close(res);
    }
    else if (res == -2) // config file is not exist.
    {
        rt_kprintf("System config read mult failed !\n");
    }
    
    return 0;
}


//int json_cfg_read_mult(struct t_json_value *json_value, int num)
//{
//    //uint8_t *cfg = NULL;
//    int res = 0;
//    cJSON *pJSON;
//    cJSON *JSON_item ;
//    char *new_cfg;
//    int i;
//    
//    res = json_cfg_open( SYS_CFG_FILE_PATH, O_RDONLY);
//    if (res >= 0)
//    {
//        // Parse the config.
//        pJSON = cJSON_Parse(cfg);
//        
//        for (i=0;i<num;i++)
//        {
//            JSON_item = cJSON_GetObjectItem(pJSON, json_value[i].name);
//            
//            if (json_value[i].type == 1)
//            {
//               json_value[i].value = JSON_item->valuedouble;
//            }
//            else if (json_value[i].type == 2)
//            {
//                memcpy( json_value[i].string, JSON_item->valuestring, strlen(JSON_item->valuestring) + 1);
//            }
//        }

//        json_cfg_close(res);
//    }
//    else if (res == -2) // config file is not exist.
//    {
//        rt_kprintf("System config read mult failed !\n");
//    }
//    
//    return 0;
//}

int json_cfg_rd_sys_cfg(int *sys_cfg)
{
    return json_cfg_read("sys_cfg", sys_cfg, 1);
}

//int json_cfg_rd_auto_update(int *auto_update)
//{
//    return json_cfg_read("auto_update", auto_update, 1);
//}

//int json_cfg_rd_force_check_up(int *force_check_up)
//{
//    return json_cfg_read("force_check_up", force_check_up, 1);
//}

//int json_cfg_rd_force_up_wifi(int *force_up_wifi)
//{
//    return json_cfg_read("force_up_wifi", force_up_wifi, 1);
//}

//int json_cfg_rd_force_up_sensor(int *force_up_sensor)
//{
//    return json_cfg_read("force_up_sensor", force_up_sensor, 1);
//}

//void json_cfg_print_auto_update(void)
//{
//    int auto_update = 0;
//    json_cfg_rd_auto_update(&auto_update);
//    
//    rt_kprintf("auto_update: %d\n", auto_update);
//}

//void json_cfg_print_force_check_up(void)
//{
//    int force_check_up = 0;
//    json_cfg_rd_force_check_up(&force_check_up);
//    
//    rt_kprintf("force_check_up: %d\n", force_check_up);
//}

//void json_cfg_print_force_up_wifi(void)
//{
//    int force_up_wifi = 0;
//    json_cfg_rd_force_up_wifi(&force_up_wifi);
//    
//    rt_kprintf("force_up_wifi: %d\n", force_up_wifi);
//}

//void json_cfg_print_force_up_sensor(void)
//{
//    int force_up_sensor = 0;
//    json_cfg_rd_force_up_sensor(&force_up_sensor);
//    
//    rt_kprintf("force_up_sensor: %d\n", force_up_sensor);
//}

//int json_cfg_rd_server_if_en(int *server_if_en)
//{
//    return json_cfg_read("server_if_en", server_if_en, 1);
//}

//void json_cfg_print_server_if_en(void)
//{
//    int server_if_en = 0;
//    json_cfg_rd_server_if_en(&server_if_en);
//    
//    rt_kprintf("server_if_en: %d\n", server_if_en);
//}


//int json_cfg_rd_wifi_auto_en(int *wifi_auto_en)
//{
//    return json_cfg_read("wifi_auto_en", wifi_auto_en, 1);
//}

//void json_cfg_print_wifi_auto_en(void)
//{
//    int wifi_auto_en = 0;
//    json_cfg_rd_wifi_auto_en(&wifi_auto_en);
//    
//    rt_kprintf("wifi_auto_en: %d\n", wifi_auto_en);
//}

//int json_cfg_rd_A8_if_BR(int *A8_if_BR)
//{
//    return json_cfg_read("A8_if_BR", A8_if_BR, 1);
//}
//void json_cfg_print_A8_if_BR(void)
//{
//    int A8_if_BR = 0;
//    json_cfg_rd_A8_if_BR(&A8_if_BR);
//    
//    rt_kprintf("A8_if_BR: %d\n", A8_if_BR);
//}

//int json_cfg_rd_uart2_BR(int *uart2_BR)
//{
//    return json_cfg_read("uart2_BR", uart2_BR, 1);
//}
//void json_cfg_print_uart2_BR(void)
//{
//    int uart2_BR = 0;
//    json_cfg_rd_A8_if_BR(&uart2_BR);
//    
//    rt_kprintf("uart2_BR: %d\n", uart2_BR);
//}

//int json_cfg_rd_SSID_KEY(char *SSID, char *key)
//{
//    struct t_json_value *json_value;
//    
//    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value) * 2);
//    
//    json_value[0].type = 2;
//    strcpy(json_value[0].name, "SSID");
//    json_value[0].string = SSID;

//    json_value[1].type = 2;
//    strcpy(json_value[1].name, "key");
//    json_value[1].string = key;
//    
//    json_cfg_read_mult(json_value, 2);
//    
//    rt_free(json_value);
//    
//    return 0;
//}

//void json_cfg_print_SSID_KEY(void)
//{
//    char ssid[WIFI_SSID_LEN+1] = {0};
//    char key[WIFI_KEY_LEN+1] = {0};

//    json_cfg_rd_SSID_KEY(ssid, key);

//    rt_kprintf(" SSID: %s\n KEY: %s\n", ssid, key);

//}


int json_cfg_rd_sensor_period_1(int *sensor_period_1)
{
    return json_cfg_read("sensor_period_1", sensor_period_1, 1);
}

void json_cfg_print_sensor_period_1(void)
{
    int32_t sensor_period_1 = 0;
    
    json_cfg_rd_sensor_period_1(&sensor_period_1);
    rt_kprintf("sensor_period_1 : %d\n", sensor_period_1);
}

int json_cfg_rd_sensor_period_2(int *sensor_period_2)
{
    return json_cfg_read("sensor_period_2", sensor_period_2, 1);
}

void json_cfg_print_sensor_period_2(void)
{
    int32_t sensor_period_2 = 0;
    
    json_cfg_rd_sensor_period_2(&sensor_period_2);
    rt_kprintf("sensor_period_2 : %d\n", sensor_period_2);
}

int json_cfg_rd_debug_level(int *debug_level)
{
    return json_cfg_read("debug_level", debug_level, 1);
}

void json_cfg_print_debug_level(void)
{
    int32_t debug_level = 0;
    
    json_cfg_rd_debug_level(&debug_level);
    rt_kprintf("debug_level : %d\n", debug_level);
}

int json_cfg_rd_sys_mode(int *sys_mode)
{
    return json_cfg_read("sys_mode", sys_mode, 1);
}

void json_cfg_print_sys_mode(void)
{
    int32_t sys_mode = 0;
    
    json_cfg_rd_sys_mode(&sys_mode);
    rt_kprintf("sys_mode : %d\n", sys_mode);
}


int json_cfg_rd_svr_use_name(int *svr_use_name)
{
    return json_cfg_read("svr_use_name", svr_use_name, 1);
}

void json_cfg_print_svr_use_name(void)
{
    int32_t svr_use_name = 0;
    
    json_cfg_rd_svr_use_name(&svr_use_name);
    rt_kprintf("svr_use_name : %d\n", svr_use_name);
}

int json_cfg_rd_server_port(char *server, uint16_t *port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_if_name");
    json_value[0].string = server;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_if_port");
    json_value[1].value = 0;
    
    json_cfg_read_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    *port = json_value[1].value;
    
    rt_free(json_value);
    
    return 0;
}

int json_cfg_rd_server_ip_port(char *server_ip, uint16_t *port)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 2;
    strcpy(json_value[0].name, "svr_if_ip");
    json_value[0].string = server_ip;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "svr_if_port");
    json_value[1].value = 0;
    
    json_cfg_read_mult_ext(SYS_CFG_FILE_PATH, json_value, 2);
    
    *port = json_value[1].value;
    
    rt_free(json_value);
    
    return 0;
}


void json_cfg_print_server_port(void)
{
    char server[SVR_IF_NAME_LEN+1] = {0};
    uint16_t port  = 0;

    json_cfg_rd_server_port(server, &port);

    rt_kprintf(" server: %s\n port: %d\n", server, port);

}

void json_cfg_print_server_ip_port(void)
{
    char server_ip[15+1] = {0};
    uint16_t port  = 0;

    json_cfg_rd_server_ip_port(server_ip, &port);

    rt_kprintf(" server_ip: %s\n port: %d\n", server_ip, port);

}


int json_cfg_rd_AP01(uint32_t *SEN01_report_period, uint32_t *SEN02_report_period)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*2);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "SEN01_report_period");
    json_value[0].value = 0;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "SEN02_report_period");
    json_value[1].value = 0;
    
    json_cfg_read_mult_ext(AP01_CFG_FILE_PATH, json_value, 2);
    
    *SEN01_report_period = json_value[0].value;
    *SEN02_report_period = json_value[1].value;
    
    rt_free(json_value);
    
    return 0;
}


int json_cfg_rd_AP02(uint32_t *FA_type, uint32_t *FA_baud, uint8_t *SW01_trig_cfg, uint32_t *SW01_report_period)
{
    struct t_json_value *json_value;
    
    json_value = (struct t_json_value *)rt_malloc(sizeof(struct t_json_value)*4);
    
    json_value[0].type = 1;
    strcpy(json_value[0].name, "FA_type");
    json_value[0].value = 0;

    json_value[1].type = 1;
    strcpy(json_value[1].name, "FA_baud");
    json_value[1].value = 0;
    
    json_value[2].type = 1;
    strcpy(json_value[2].name, "SW01_trig_cfg");
    json_value[2].value = 0;

    json_value[3].type = 1;
    strcpy(json_value[3].name, "SW01_report_period");
    json_value[3].value = 0;

    json_cfg_read_mult_ext(AP02_CFG_FILE_PATH, json_value, 4);
    
    *FA_type = json_value[0].value;
    *FA_baud = json_value[1].value;
    *SW01_trig_cfg = json_value[2].value;
    *SW01_report_period = json_value[3].value;
    
    rt_free(json_value);
    
    return 0;
}

void json_cfg_print_AP01(void)
{
    uint32_t SEN01_report_period  = 0;
    uint32_t SEN02_report_period  = 0;

    json_cfg_rd_AP01(&SEN01_report_period, &SEN02_report_period);

    rt_kprintf(" SEN01_report_period: %d\n SEN02_report_period: %d\n", SEN01_report_period, SEN02_report_period);

}

//void json_cfg_print_AP02(void)
//{
//    uint32_t FA_type  = 0;
//    uint32_t FA_baud  = 0;
//    uint8_t SW01_trig_cfg  = 0;
//    uint32_t SW01_report_period  = 0;

//    json_cfg_rd_AP02(&FA_type, &FA_baud, &SW01_trig_cfg, &SW01_report_period);

//    rt_kprintf(" FA_type: %d\n FA_baud: %d\n SW01_trig_cfg: %d\n SW01_report_period: %d\n", 
//                FA_type, FA_baud, SW01_trig_cfg, SW01_report_period);

//}

//int json_cfg_rd_RT_NOR_para(uint32_t *RT_peri, uint32_t *RT_len, uint32_t *Nor_len)
//{
//    
//    json_cfg_read("RT_peri", RT_peri, 1);
//    json_cfg_read("RT_len", RT_len, 1);
//    json_cfg_read("Nor_len", Nor_len, 1);
//    
//    return 0;
//}


//int json_cfg_print_RT_NOR_para(void)
//{
//    uint32_t RT_peri,RT_len,Nor_len;
//    
//    json_cfg_rd_RT_NOR_para(&RT_peri, &RT_len, &Nor_len);
//    
//    rt_kprintf(" RT_peri = %d\n RT_len = %d\n Nor_len = %d\n", RT_peri, RT_len, Nor_len);
//    
//    return 0;
//}

//int json_cfg_rd_wifi_roam(int32_t *wifi_roam)
//{
//    json_cfg_read("wifi_roam", wifi_roam, 1);
//    return 0;
//}

//int json_cfg_print_wifi_roam(void)
//{
//    int32_t wifi_roam;
//    
//    json_cfg_rd_wifi_roam(&wifi_roam);
//    rt_kprintf(" wifi_roam = %d\n", wifi_roam);
//    
//    return 0;
//}



#ifdef RT_USING_FINSH
#include <finsh.h>
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_auto_update, WR_auto_update, Write the auto_update);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_force_check_up, WR_f_check_up, Write the force_check_up);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_force_up_wifi, WR_f_up_wifi, Write the force_up_wifi);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_force_up_sensor, WR_f_up_sensor, Write the force_up_sensor);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_if_en, WR_server_if_en, Write the server_if_en);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_wifi_auto_en, WR_wifi_auto_en, Write the wifi_auto_en);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_A8_if_BR, WR_A8_if_BR, Write the A8_if_BR);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_uart2_BR, WR_uart2_BR, Write the uart2_BR);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_SSID_KEY, WR_ssid_key, Write the SSID and KEY);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_sensor_period_1, WR_sensor_per_1, Write the sensor_period_1);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_sensor_period_2, WR_sensor_per_2, Write the sensor_period_2);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_debug_level, WR_debug_level, Write the debug_level 1_3_7_15); 
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_num, WR_server_num, Write the server numbers);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_log_file_en, WR_log_file_en, Write the system log store to file enable);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_log_file_path, WR_log_file_dir, Write the system log storage path);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_sys_mode, WR_sys_mode, Write the sys_mode 0_normal 1_testing 2_tested 3_conifg);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_svr_use_name, WR_svr_name, Write the svr_use_name);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_svr_use_name_2, WR_svr_name_2, Write the svr_2_use_name);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_port, WR_svr_if, Write the server and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_ip_port, WR_svr_ip_if, Write the server IP and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_port_2, WR_svr_if_2, Write the second server and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_ip_port_2, WR_svr_ip_if_2, Write the second server IP and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_AP01, WR_cfg_AP01, SEN01_period  SEN02_period);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_AP02, WR_cfg_AP02, FA_type  FA_baud  FA_listen  SW01_trig_cfg  SW01_report_period); 
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_WIFI, WR_cfg_WIFI, --SSID  --PW);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_eth_DHCP, WR_cfg_eth_DHCP, if_DHCP);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_eth_DNS, WR_cfg_eth_DNS, DNS);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_eth_IP, WR_cfg_eth_IP, IP  GW  MASK);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_mode, WR_cfg_mode, --if_single_way --GPRS --ETHERNET --WIFI);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_transparent, WR_cfg_TP, --trans_period --buffer_period --uart --baudrate --bits --verify --stopbit);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_PRO_CTRL_output, WR_cfg_CTRL_SEN, --SEN01_period  --SEN02_period --SEN03_period --SEN04_period);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_PRO_CTRL_input, WR_cfg_CTRL_IN, --input_num  --trig_cfg 0-Null 1-O2C 2-C2O 3-Either 4-period --trig_period);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_PRO_UITD_output, WR_cfg_UITD_SEN, --SEN01_period  --SEN02_period --SEN03_period --SEN04_period);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_PRO_UITD_input, WR_cfg_UITD_IN, --input_num  --trig_cfg 0-Null 1-O2C 2-C2O 3-Either 4-period --trig_period);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_FA_uart, WR_cfg_FA_uart, --FA_index  --FA_en --FA_name --FA_type --FA_baud --FA_listen);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_FA_input, WR_cfg_FA_input, --SW01_trig_cfg  --SW01_report_period);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_RT_NOR_para, WR_RT_NOR_para, Write the RT_peri RT_len and Nor_len.);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_wifi_roam, WR_wifi_roam, Write the wifi roam trigger  unit dbm.);



//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_auto_update, RD_auto_update, Read the auto_update);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_force_check_up, RD_f_check_up, Read the force_check_up);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_force_up_wifi, RD_f_up_wifi, Read the force_up_wifi);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_force_up_sensor, RD_f_up_sensor, Read the force_up_sensor);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_server_if_en, RD_server_if_en, Read the server_if_en);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_wifi_auto_en, RD_wifi_auto_en, Read the wifi_auto_en);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_A8_if_BR, RD_A8_if_BR, Read the A8_if_BR);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_uart2_BR, RD_uart2_BR, Read the uart2_BR);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_SSID_KEY, RD_ssid_key, Read the SSID and KEY);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_sensor_period_1, RD_sensor_per_1, Read the sensor_period_1);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_sensor_period_2, RD_sensor_per_2, Read the sensor_period_2);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_debug_level, RD_debug_level, Read the debug level);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_sys_mode, RD_sys_mode, Read the sys mode 0_normal 1_testing 2_tested 3_config 4_transparent);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_svr_use_name, RD_svr_use_name, Read the svr_use_name);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_server_port, RD_server_if, Read the server and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_server_ip_port, RD_server_ip_if, Read the server ip and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_AP01, RD_cfg_AP01, Read the AP01 config);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_AP02, RD_cfg_AP02, Read the AP02 config);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_RT_NOR_para, RD_RT_NOR_para, Read the RT_peri RT_len and Nor_len.);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_wifi_roam, RD_wifi_roam, Read the wifi roam trigger  unit dbm.);

#endif // RT_USING_FINSH
