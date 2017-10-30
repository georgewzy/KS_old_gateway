#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include "sys_config.h"
//#include "platform_config.h"



struct t_json_value
{
    char name[32];
    int  value;
    char *string;
    int  type;
};


extern int json_cfg_load(void);







#endif // __JSON_CONFIG_H__
