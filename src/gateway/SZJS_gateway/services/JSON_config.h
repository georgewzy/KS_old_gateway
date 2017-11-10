#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include "sys_config.h"
//#include "platform_config.h"
#include "sys_misc.h"


struct t_json_value
{
    char name[32];
    int  value;
    char *string;
    int  type;
};


extern int json_cfg_load(void);


extern int json_cfg_create_AP01(char *dir, const s_AP01_cfg *p_cfg);
//extern int json_cfg_create_AP02(char *dir, const s_AP02_cfg *p_cfg);
extern int json_cfg_create(char *dir, const struct t_sys_cfg *p_cfg);

extern int json_cfg_load_PRO_CTRL(void);


#endif // __JSON_CONFIG_H__
