#ifndef __SYS_DEF_H__
#define __SYS_DEF_H__

#include "version.h"

#include "stm32f2xx.h"

#include <stdint.h>
//#include "proj_arch.h"
#include "sys_misc.h"

#include "rtconfig.h"

//#include "server_if.h"

//#include "lwip/err.h"
//#include "polynomial_fitting.h" 



//#include <string.h>

#define ENABLE_IRQ()     __enable_irq()
#define DISABLE_IRQ() 	 __enable_irq()


#define enable_interrupt() 	 ENABLE_IRQ()
#define disable_interrupt()  DISABLE_IRQ()

#ifndef NULL
#define NULL 0
#endif

#define SWAP_16(n)	((n<<8)|(n>>8))
#define SWAP_32(n)	(((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24))

#define MEM_ALIGN_SIZE(size)  LWIP_MEM_ALIGN_SIZE(size)
#define MEM_ALIGN(addr)       LWIP_MEM_ALIGN(addr)



#if defined ( __CC_ARM )
    #define PACK_STRUCT_BEGIN		
    #define PACK_STRUCT_STRUCT		 __attribute__((packed))
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) 	 x

#elif defined ( __GNUC__ )
    #define PACK_STRUCT_BEGIN		
    #define PACK_STRUCT_STRUCT		 __attribute__((packed))
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) 	 x
#endif


//#define msleep(time)	delay_ms(time)



#define TIMX_ENABLE(TIMx)  TIMx->CR1 |= TIM_CR1_CEN
#define TIMX_DISABLE(TIMx) TIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN))



#define WIFI_SSID_LEN      16
#define WIFI_KEY_LEN        16


//#define WIFI_SSID_DEFAULT  "TP-LINK_QT"
//#define WIFI_KEY_DEFAULT    "13738088485"

#define SVR_IF_NAME_LEN     32
#define FILE_PATH_LEN_MAX   64


#define SYS_DEBUG_ERROR          0x01
#define SYS_DEBUG_WARNING        0x02
#define SYS_DEBUG_INFO           0x04
#define SYS_DEBUG_DEBUG          0x08
////#define SYS_DEBUG_LEVEL          (SYS_DEBUG_INFO | SYS_DEBUG_WARNING | SYS_DEBUG_ERROR | SYS_DEBUG_DEBUG)
#define SYS_DEBUG_LEVEL          (SYS_DEBUG_INFO | SYS_DEBUG_WARNING | SYS_DEBUG_ERROR )


struct VirtEeprom
{
// Add user's var here //
// struct number must be uint16_t or uint32_t type //
    uint16_t sys_cfg;
    
//    uint16_t A8_if_BR_L;
//    uint16_t A8_if_BR_H;
//    uint16_t uart2_BR_L;
//    uint16_t uart2_BR_H;
//    
//    uint16_t SSID[WIFI_SSID_LEN+1];
//    uint16_t key[WIFI_KEY_LEN+1];
    
//    uint16_t svr_if_name[SVR_IF_NAME_LEN+1];
//    uint16_t svr_if_port;

};



typedef enum
{
    sys_mode_normal = 0,
    sys_mode_testing = 1,
    sys_mode_tested = 2,
    sys_mode_config = 3,
    sys_mode_transparent = 4,
} e_sys_mode;

struct t_sys_cfg
{
// Add user's var here //
    uint8_t sys_cfg;
	uint8_t  sys_elec_fire;	//wzy
    e_sys_mode sys_mode;

//    uint32_t A8_if_BR;
//    uint32_t uart2_BR;
//    
//    uint8_t SSID[WIFI_SSID_LEN+1];
//    uint8_t key[WIFI_KEY_LEN+1];
//    
    
    uint8_t svr_num;
    
    uint8_t svr_use_name;  // 1: use server name,  0: use server IP.
    uint8_t svr_if_name[SVR_IF_NAME_LEN+1];
    uint8_t svr_if_ip[15+1];
    uint16_t svr_if_port;
    uint8_t if_UITD;

    uint8_t svr_2_use_name;  // 1: use server name,  0: use server IP.
    uint8_t svr_2_if_name[SVR_IF_NAME_LEN+1];
    uint8_t svr_2_if_ip[15+1];
    uint16_t svr_2_if_port;
    uint8_t if_UITD_2;
    
    uint8_t debug_level;
    uint8_t log_file_en;
    uint8_t log_file_path[FILE_PATH_LEN_MAX];
    
    //uint8_t batt_check; // battery check support

};


#define PACKET_BATTVOLT_BIT_LEN	    16
#define PACKET_TEMP_BIT_LEN			16
#define PACKET_RES_BIT_LEN			16
#define PACKET_ACC_BIT_LEN			10
#define PACKET_GYRO_BIT_LEN			16
#define PACKET_ECG_BIT_LEN			12
#define PACKET_PPG_BIT_LEN			16


typedef struct bs
{
    unsigned int error_code : 4;
    unsigned int blet_drop  : 1;
    unsigned int batt_empty : 1;
    unsigned int HR_invalid : 1;
    unsigned int            : 9;
}PACK_STRUCT_STRUCT tSYS_state;

typedef struct bs2
{
    unsigned int user_trig  : 1;
    unsigned int user_fall  : 1;
    unsigned int            : 14;
}PACK_STRUCT_STRUCT tSYS_report;


//typedef struct
//{
//	tSYS_state  State;
//    tSYS_report Report;
//    uint16_t    BattVolt; // BattVolt/100 = Battery Volt.
//	uint16_t    Temp; // Temp/10 = temperature value.
//	uint16_t    Res; // 16bit , respiratory rate
//    uint16_t    HR;		   // 16bits,  heart rate
//    uint16_t    ECG_R_num;  // 16bits,  the number of ECG R wave
//    int16_t     ECG_R[ECG_R_MAX];    // 16bits, signed , HR max is 300bpm. 300/60 = 5.
//	uint16_t    ACCx[ACC_SAMPLE_RATE]; // 10bit
//	uint16_t    ACCy[ACC_SAMPLE_RATE]; // 10bit
//	uint16_t    ACCz[ACC_SAMPLE_RATE]; // 10bit
//	uint16_t    GYROx[GYRO_SAMPLE_RATE];// 16bit
//	uint16_t    GYROy[GYRO_SAMPLE_RATE];// 16bit
//	uint16_t    GYROz[GYRO_SAMPLE_RATE];// 16bit
//	uint16_t    ECG[ECG_SAMPLE_RATE]; // 12bit
//}PACK_STRUCT_STRUCT tPacket_data;



//typedef struct
//{
//  uint16_t user_id;
//  uint8_t  data;
//  uint8_t  data_nov;
//}PACK_STRUCT_STRUCT st_IR_data ;

//typedef struct
//{
//    st_IR_data IR_data;
//    uint8_t    repeat;
//    uint8_t    valid;
//}PACK_STRUCT_STRUCT st_IR_state;


//enum bt_profile {
//  LAP_PROFILE, DUN_PROFILE, RFCOMM_PROFILE, SPP_PROFILE, NAP_PROFILE, PAN_PROFILE, 
//  HS_PROFILE, AG_PROFILE, OBEX_PUSH_PROFILE, OBEX_FTP_PROFILE,
//};


/* day in the year is NOT used in this implementation yet,
   daylight saving time is NOT used in this implementation yet */
struct tm {
  int tm_sec;         /* seconds */
  int tm_min;         /* minutes */
  int tm_hour;        /* hours */
  int tm_mday;        /* day of the month */
  int tm_mon;         /* month */
  int tm_year;        /* year */
  int tm_wday;        /* day of the week */
  int tm_yday;        /* day in the year, not used in this implemantation yet */
  int tm_isdst;       /* daylight saving time, not used in this implemantation yet */
};


struct Date {
  uint8_t year;		// Year = 1900 + year
  uint8_t month;	        //
  uint8_t day;		//
  uint8_t hour;		//
  uint8_t min;		//
  uint8_t sec;		//
  uint8_t ssec;		//
};

// Song: added 2016-7-1 18:08:25 , Take care of this declaration.
extern struct tm *localtime(const uint32_t * );


// RFCOMM command, used to adjust parameter.

//typedef struct {
//	uint16_t CMD_head;
//	uint8_t  CMD_type;  // Command type
//	uint16_t CMD_par1;  // common para   (0-65535)
//	uint8_t  CMD_par2;  // common scale  (100%)  (0-100)
//	uint8_t  CMD_par3;  // the number of adjust steps. (4-20)
//	uint8_t  CMD_par4;  // R wave amplitude / R wave peak   (100%)  (0-100)
//	uint8_t  CMD_par5;  // R detect value at the position between peak value and noise value. (100%)  (0-100)
//	uint8_t  working_mode;  // 0:user mode; 1:debug mode.
//	uint16_t server_port;
//	uint8_t  server_ip_1;
//	uint8_t  server_ip_2;
//	uint8_t  server_ip_3;
//	uint8_t  server_ip_4;
//	uint8_t  BD_addr[6];
//	uint8_t  shutdown;
//}PACK_STRUCT_STRUCT t_RFCOMM_cmd;


//typedef struct {
//	uint8_t  server_ip_1;
//	uint8_t  server_ip_2;
//	uint8_t  server_ip_3;
//	uint8_t  server_ip_4;
//	uint8_t  BD_addr[6];
//	uint16_t server_port;
//	uint8_t  working_mode;  // 0:user mode; 1:debug mode.
//	uint8_t  shutdown;
//}PACK_STRUCT_STRUCT t_COM_cmd;


//extern struct rfcomm_pcb *rfcomm_pcb_globle;

//extern volatile uint8_t gf_APMS_TEST_full_speed;
//extern volatile uint8_t gf_APMS_BT_find_mode;

#endif  // End of sys_def.h
