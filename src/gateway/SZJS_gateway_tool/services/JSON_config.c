#include "JSON_config.h"

#include "cJSON.h"
#include <dfs.h>
#include <dfs_posix.h>
#include "board.h"
#include "sys_misc.h"
#include "sys_def.h"

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
    fd = open( SYS_CFG_FILE_PATH, O_CREAT | O_RDWR, 0);
    if (fd >= 0)
    {
        pJSON = cJSON_CreateObject();
        // Add the system config item to cJSON.
        cJSON_AddNumberToObject( pJSON, "sys_cfg" , p_cfg->sys_cfg);
//        cJSON_AddNumberToObject( pJSON, "A8_if_BR", p_cfg->A8_if_BR);
//        cJSON_AddNumberToObject( pJSON, "uart2_BR", p_cfg->uart2_BR);
//        cJSON_AddStringToObject( pJSON, "SSID", p_cfg->SSID);
//        cJSON_AddStringToObject( pJSON, "key", p_cfg->key);
        cJSON_AddNumberToObject( pJSON, "svr_use_name", p_cfg->svr_use_name);
        cJSON_AddStringToObject( pJSON, "svr_if_name", p_cfg->svr_if_name);
        cJSON_AddStringToObject( pJSON, "svr_if_ip", p_cfg->svr_if_ip);
        cJSON_AddNumberToObject( pJSON, "svr_if_port", p_cfg->svr_if_port);
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
            free(json_buf);
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


int json_cfg_load(void)
{
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    int value = 0;
    
    //cfg = 0;

    res = json_cfg_open( SYS_CFG_FILE_PATH, O_RDONLY);
    if (res >= 0)
    {
        // Parse the config.
        pJSON = cJSON_Parse(cfg);
        
        JSON_item = cJSON_GetObjectItem(pJSON, "sys_cfg");
        if (JSON_item != NULL) 
            sys_cfg = JSON_item->valueint;
        
//        JSON_item = cJSON_GetObjectItem(pJSON, "A8_if_BR");
//        if (JSON_item != NULL) 
//            A8_if_BR = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "uart2_BR");
//        if (JSON_item != NULL) 
//            uart2_BR = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "SSID");
//        if (JSON_item != NULL) 
//            memcpy( wifi_rssid, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);
// 
//        JSON_item = cJSON_GetObjectItem(pJSON, "key");
//        if (JSON_item != NULL) 
//            memcpy( wifi_key, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_use_name");
        if (JSON_item != NULL) 
            svr_use_name = JSON_item->valueint;

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_name");
        if (JSON_item != NULL) 
            memcpy( svr_if_name, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_ip");
        if (JSON_item != NULL) 
            memcpy( svr_if_ip, JSON_item->valuestring, strlen(JSON_item->valuestring)+1);

        JSON_item = cJSON_GetObjectItem(pJSON, "svr_if_port");
        if (JSON_item != NULL) 
            svr_if_port = JSON_item->valueint;

//        JSON_item = cJSON_GetObjectItem(pJSON, "auto_update");
//        if (JSON_item != NULL) 
//            auto_update = JSON_item->valueint;

//        JSON_item = cJSON_GetObjectItem(pJSON, "force_check_up");
//        if (JSON_item != NULL) 
//            force_check_up = JSON_item->valueint;

//        JSON_item = cJSON_GetObjectItem(pJSON, "force_up_wifi");
//        if (JSON_item != NULL) 
//            force_up_wifi = JSON_item->valueint;

//        JSON_item = cJSON_GetObjectItem(pJSON, "force_up_sensor");
//        if (JSON_item != NULL) 
//            force_up_sensor = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "server_if_en");
//        if (JSON_item != NULL) 
//            server_if_en = JSON_item->valueint;
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "wifi_auto_en");
//        if (JSON_item != NULL) 
//            wifi_auto_en = JSON_item->valueint;
//        
//        
//        JSON_item = cJSON_GetObjectItem(pJSON, "RT_peri");
//        if (JSON_item != NULL) 
//            RT_peri = JSON_item->valueint;
//        JSON_item = cJSON_GetObjectItem(pJSON, "RT_len");
//        if (JSON_item != NULL) 
//            RT_len = JSON_item->valueint;
//        JSON_item = cJSON_GetObjectItem(pJSON, "Nor_len");
//        if (JSON_item != NULL) 
//            Nor_len = JSON_item->valueint;
//        JSON_item = cJSON_GetObjectItem(pJSON, "wifi_roam");
//        if (JSON_item != NULL) 
//            wifi_roam = JSON_item->valueint;

        json_cfg_close(res);
        
        // Reset the config.
//        value = 0;
//        json_cfg_write("force_check_up", &value, 1);
//        json_cfg_write("force_up_wifi", &value, 1);
//        json_cfg_write("force_up_sensor", &value, 1);
        
    }
    else if (res == -2) // config file is not exist.
    {
        // Create a default config file.
        json_cfg_create(SYS_CFG_FILE_PATH, &sys_cfg_init_data);
        
        
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
    cJSON *JSON_item ;
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
            
            free(new_cfg);
        }
        else
        {
            rt_kprintf("System config write update failed !\n");
        }
        
    }
    
    return 0;
}




// 
int json_cfg_write_mult(struct t_json_value *json_value, int num)
{
    //uint8_t *cfg = NULL;
    int res = 0;
    cJSON *pJSON;
    cJSON *JSON_item ;
    char *new_cfg;
    int i;
    

    
    res = json_cfg_open( SYS_CFG_FILE_PATH, DFS_O_RDWR);
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
            rm(SYS_CFG_FILE_PATH);
            res = open( SYS_CFG_FILE_PATH, DFS_O_CREAT | DFS_O_WRONLY, 0);
            // Write string to config file.
            write(res, new_cfg, strlen(new_cfg));
            close(res);
            
            free(new_cfg);
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
    
    json_cfg_write_mult(json_value, 2);
    
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
    
    json_cfg_write_mult(json_value, 2);
    
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

int json_cfg_read_mult(struct t_json_value *json_value, int num)
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
    
    json_cfg_read_mult(json_value, 2);
    
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
    
    json_cfg_read_mult(json_value, 2);
    
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
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_svr_use_name, WR_svr_use_name, Write the svr_use_name);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_port, WR_server_if, Write the server and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_wr_server_ip_port, WR_server_ip_if, Write the server IP and port);
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
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_svr_use_name, RD_svr_use_name, Read the svr_use_name);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_server_port, RD_server_if, Read the server and port);
FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_server_ip_port, RD_server_ip_if, Read the server ip and port);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_RT_NOR_para, RD_RT_NOR_para, Read the RT_peri RT_len and Nor_len.);
//FINSH_FUNCTION_EXPORT_ALIAS(json_cfg_print_wifi_roam, RD_wifi_roam, Read the wifi roam trigger  unit dbm.);

#endif // RT_USING_FINSH
