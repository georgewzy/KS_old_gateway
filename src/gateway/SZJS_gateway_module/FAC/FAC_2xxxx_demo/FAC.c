// FAC: 

/* Rough data



*/

#include "fire_alarm.h"
#include "sys_config.h"
//#include <stdio.h>
#include <rtdevice.h>
#include "board.h"
 


s_FAC_config FAC_config = 
{
        FAC_type_V6851,
        "V6851",
        {9600, 8, PARITY_NONE, 1}, // or 9600
        0, // alive_period 
//        {0, "", 0, 0}, // pre_send_1
//        {0, "", 0, 0}, // pre_send_2
        {0, "", 0, "", 0, 0}, // rec_alive_1
//        {0, "", 0, "", 0, 0}, // rec_alive_2
        {0, "", 0, "", 0, 0}, // send_alive_1
        {0, "", 0, "", 0, 0}, // send_alive_2
        {0, "", 0, "", 0, 0}, // reset_1
//        {0, "", 0, "", 0, 0}, // reset_2
//        {0, "", 0, "", 0, 0}, // reset_3
        { // alarm
            {1, {0x66, 0xF0}, 2}, // head
            {1, {0xFD}, 1}, // tail
            0, 8,  // if_fix_len, fix_len
            { // alarm_alarm
                1,  // if_alarm
                {0x01}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                }
            },
            { // alarm_fault
                1,  // if_alarm
                {0x03}, // buf
                3, // index
                1, // len
                { // sys_addr
                    1, // if_data
                    2, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_main
                    1, // if_data
                    4, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                },
                { // addr_sub
                    1, // if_data
                    5, // data_index
                    FAC_data_HEX, // data_type
                    {1,{0}}, // data_HEX: num, index[]
                    {0,{0}}, // data_BCD: num, index[]
                    {0,{0}}  // data_ASCII: num, index[]
                }

            }
        },
        { // sys_reset
            1,  // if_reset
            {0x20}, // buf
            3, // index
            1, // len
            1, // if_sys_addr
            0, // sys_addr
            2, // sys_addr_index
        }  
};


int FAC_usual_cfg_init(s_com_bus_cb * cb)
{
	
    rt_memcpy(&cb->FAC_config, &FAC_config, sizeof(s_FAC_config));

	return 0;
}





//#define FAC_TYPE_UNKNOWN            0
//#define FAC_TYPE_JB_QB_GST_100      1
//#define FAC_TYPE_JB_QB_GST_200      2
//#define FAC_TYPE_JB_QB_GST_500      3
//#define FAC_TYPE_JB_QB_GST_4000     4
//#define FAC_TYPE_JB_QB_GST_5000     5
//#define FAC_TYPE_JB_QB_GST_9000     6
//#define FAC_TYPE_JB_3101G           7
//#define FAC_TYPE_JB_3101            8
//#define FAC_TYPE_JB_3101B           9
//#define FAC_TYPE_JB_3201A           10
//#define FAC_TYPE_JB_3208G_B         11
//#define FAC_TYPE_JB_QB_5Ei          12
//#define FAC_TYPE_JB_QX_5Li          13
//#define FAC_TYPE_JB_1501_A          14
//#define FAC_TYPE_JB_QB_21S_VFC3010A 15
//#define FAC_TYPE_JB_TG_JBF_11S      16
//#define FAC_TYPE_JB_TG_JBF_11S_2013 17
//#define FAC_TYPE_BT8002             18
//#define FAC_TYPE_FC_720R            19
//#define FAC_TYPE_CHNTAD_8000        20
//#define FAC_TYPE_JB_Q100GZ2L_LA040  21
//#define FAC_TYPE_TX3000             22
//#define FAC_TYPE_SD2100_2200        23
//#define FAC_TYPE_V6851              24
//
//
//
//#define FAC_TYPE_UNUSUAL            10000  // use external parse program to parse communication protocol.
//#define FAC_TYPE_UNUSUAL_01         (1 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_UNUSUAL_02         (2 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_UNUSUAL_03         (3 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_UNUSUAL_04         (4 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_UNUSUAL_05         (5 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_JB_QT_LD_128K_HB   (6 + FAC_TYPE_UNUSUAL)
//#define FAC_TYPE_FC18R              (7 + FAC_TYPE_UNUSUAL)
//
//#define FAC_TYPE_USUAL_CFG          20000 // use usual config to parse the communication protocol.
//


//// fire device information table
//const s_FAC_config FAC_config_table[] = {
//    
//    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_100
//        FAC_type_JB_QB_GST_100,
//        "JB-QB-GST-100",
//        {2400, 8, PARITY_NONE, 1},
//        3000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xAA}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 24,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x15}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_200
//        FAC_type_JB_QB_GST_200,
//        "JB-QB-GST-200",
//        {2400, 8, PARITY_NONE, 1},
//        3000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xAA}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 24,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x15}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_500
//        FAC_type_JB_QB_GST_500,
//        "JB-QB-GST-500",
//        {2400, 8, PARITY_NONE, 1},
//        3000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xAA}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 24,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x15}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_5000
//        FAC_type_JB_QB_GST_5000,
//        "JB-QB-GST-5000",
//        {2400, 8, PARITY_NONE, 1},
//        3000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xAA}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 24,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x15}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_QB_GST_9000
//        FAC_type_JB_QB_GST_9000,
//        "JB-QB-GST-9000",
//        {2400, 8, PARITY_NONE, 1},
//        3000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xAA, 0x08, 0x09, 0x20, 0x09, 0x20, 0x04}, 7, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xAA}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 24,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x15}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//    
//    {/////////////////////////////////////////////// FAC_type_JB_3101G
//        FAC_type_JB_3101G,
//        "JB-3101G",
//        {9600, 8, PARITY_NONE, 1},
//        200, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFE}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 10,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x01}, // buf
//                2, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//    
//    {/////////////////////////////////////////////// FAC_type_JB_3101
//        FAC_type_JB_3101,
//        "JB-3101",
//        {9600, 8, PARITY_NONE, 1},
//        200, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFE}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 10,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x01}, // buf
//                2, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_3101B
//        FAC_type_JB_3101B,
//        "JB-3101B",
//        {9600, 8, PARITY_NONE, 1},
//        200, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFE, 0x64, 0xFF}, 3, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFE}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 10,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x01}, // buf
//                2, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//    {/////////////////////////////////////////////// FAC_type_JB_QB_5Ei
//        FAC_type_JB_QB_5Ei,
//        "JB-QB-5Ei",
//        {9600, 8, PARITY_NONE, 1},
//        1000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {1, {0xF5,0xF5,0xF5,0xF5,0xF5}, 5, "", 0, 100}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0x23}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 23,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x23}, // buf
//                0, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    14, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    16, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    18, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//    
//    {/////////////////////////////////////////////// FAC_type_JB_QX_5Li
//        FAC_type_JB_QX_5Li,
//        "JB-QX-5Li",
//        {9600, 8, PARITY_NONE, 1},
//        1000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {1, {0xF5,0xF5,0xF5,0xF5,0xF5}, 5, "", 0, 100}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0x23}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 23,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x23}, // buf
//                0, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    14, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    16, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    18, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x21}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {2,{0,1}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {1,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    3, // data_index
//                    FAC_data_BCD, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {3,{1,2,3}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_1501_A
//        FAC_type_JB_1501_A,
//        "JB-1501(A)",
//        {9600, 8, PARITY_NONE, 1},
//        200, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFE,0x64,0xFF}, 3, {0xFE,0x64,0xFF}, 3, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFE}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 7,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x02}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    14, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x01}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    2, // data_index
//                    FAC_data_HEX, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },
//
//    {/////////////////////////////////////////////// FAC_type_JB_QB_21S_VFC3010A
//        FAC_type_JB_QB_21S_VFC3010A,
//        "JB-QB-21S-VFC3010A",
//        {2400, 8, PARITY_NONE, 1},
//        1000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x09 ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFF,0xFF,0xFF}, 3}, // head
//            {0, {0}, 0}, // tail
//            1, 30,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x0A}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x0B}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },    
//
//    {/////////////////////////////////////////////// FAC_type_JB_TG_JBF_11S
//        FAC_type_JB_TG_JBF_11S,
//        "JB-TG-JBF-11S",
//        {2400, 8, PARITY_NONE, 1},
//        1000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x09 ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFF,0xFF,0xFF}, 3}, // head
//            {0, {0}, 0}, // tail
//            1, 30,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x0A}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x0B}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },    
//
//    {/////////////////////////////////////////////// FAC_type_JB_TG_JBF_11S_2013
//        FAC_type_JB_TG_JBF_11S_2013,
//        "JB-TG-JBF-11S(2013)",
//        {2400, 8, PARITY_NONE, 1},
//        1000, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFF ,0xFF ,0xFF ,0x13 ,0x01 ,0x01 ,0x6E ,0x01 ,0x01 ,0x00 ,0x00 ,0x85}, 12, {0xFF ,0xFF ,0xFF ,0x0A ,0x00 ,0x00 ,0x01 ,0x6E ,0x01 ,0x0A ,0x00 ,0x83}, 12, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFF,0xFF,0xFF}, 3}, // head
//            {0, {0}, 0}, // tail
//            1, 30,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x80}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x0B}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    }, 
//
//    {/////////////////////////////////////////////// FAC_type_BT8002
//        FAC_type_BT8002,
//        "BT8002",
//        {9600, 8, PARITY_NONE, 1},
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0x68}, 1}, // head
//            {0, {0}, 0}, // tail
//            1, 25,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x68}, // buf
//                0, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    13, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    14, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x0B}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },     
//    
//    {/////////////////////////////////////////////// FAC_type_FC_720R
//        FAC_type_FC_720R,
//        "FC-720R",
//        {9600, 8, PARITY_NONE, 1},
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {'*'}, 1}, // head
//            {0, {'#'}, 1}, // tail
//            1, 23,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x00}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    6, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {4,{3,2,1,0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x01}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    6, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {4,{3,2,1,0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },      
// 
//    {/////////////////////////////////////////////// FAC_type_CHNTAD_8000
//        FAC_type_CHNTAD_8000,
//        "CHNTAD8000",
//        {2400, 8, PARITY_NONE, 1},
//        0, // alive_period ?????????????????????????
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {1, {0xFF, 0xFF, 0xFF, 0x09, 0x00, 0x00, 0x01, 0x6E, 0x01, 0x0A, 0x00, 0x83}, 12, {0xFF, 0xFF, 0xFF, 0x13, 0x01, 0x01, 0x6E, 0x00, 0x01, 0x00, 0x00, 0x84}, 12, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0xFF,0xFF,0xFF}, 3}, // head
//            {0, {'#'}, 1}, // tail
//            1, 30,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x0B}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    10, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    11, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    12, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x01}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    6, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {4,{3,2,1,0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },      
//   
//    {/////////////////////////////////////////////// FAC_type_JB_Q100GZ2L_LA040
//        FAC_type_JB_Q100GZ2L_LA040,
//        "JB-Q100GZ2L-LA040",
//        {22800, 8, PARITY_NONE, 1},
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {'@'}, 1}, // head
//            {0, {'#'}, 1}, // tail
//            1, 32,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {'1'}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    10, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    14, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{0,1}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    16, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {3,{0,1,2}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x01}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    6, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {4,{3,2,1,0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    }, 
//
//    {/////////////////////////////////////////////// FAC_type_TX3000
//        FAC_type_TX3000,
//        "TX3000",
//        {22800, 8, PARITY_NONE, 1},
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {'@'}, 1}, // head
//            {0, {'#'}, 1}, // tail
//            1, 32,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {'1'}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    0, // if_data
//                    10, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    14, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{0,1}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    16, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {3,{0,1,2}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                0,  // if_alarm
//                {0x01}, // buf
//                1, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{1,0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    6, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {4,{3,2,1,0}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            0,  // if_reset
//            {0x22}, // buf
//            1, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    }, 
//    {/////////////////////////////////////////////// FAC_type_SD2100_2200
//        FAC_type_SD2100_2200,
//        "SD2100_2200",
//        {1200, 8, PARITY_NONE, 1}, // or 9600
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0x82}, 1}, // head
//            {1, {0x83}, 1}, // tail
//            0, 8,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x50}, // buf
//                6, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{0,1}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    7, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {1,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    8, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {3,{0,1,2}} // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x52}, // buf
//                6, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    1, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {2,{0,1}} // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {1,{0}} // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_ASCII, // data_type
//                    {0,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {3,{0,1,2}} // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x6F}, // buf
//            6, // index
//            1, // len
//            0, // if_sys_addr
//            0, // sys_addr
//            0, // sys_addr_index
//        }        
//    },     
//    {/////////////////////////////////////////////// FAC_type_V6851
//        FAC_type_V6851,
//        "V6851",
//        {9600, 8, PARITY_NONE, 1}, // or 9600
//        0, // alive_period 
////        {0, "", 0, 0}, // pre_send_1
////        {0, "", 0, 0}, // pre_send_2
//        {0, "", 0, "", 0, 0}, // rec_alive_1
////        {0, "", 0, "", 0, 0}, // rec_alive_2
//        {0, "", 0, "", 0, 0}, // send_alive_1
//        {0, "", 0, "", 0, 0}, // send_alive_2
//        {0, "", 0, "", 0, 0}, // reset_1
////        {0, "", 0, "", 0, 0}, // reset_2
////        {0, "", 0, "", 0, 0}, // reset_3
//        { // alarm
//            {1, {0x66, 0xF0}, 2}, // head
//            {1, {0xFD}, 1}, // tail
//            0, 8,  // if_fix_len, fix_len
//            { // alarm_alarm
//                1,  // if_alarm
//                {0x01}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                }
//            },
//            { // alarm_fault
//                1,  // if_alarm
//                {0x03}, // buf
//                3, // index
//                1, // len
//                { // sys_addr
//                    1, // if_data
//                    2, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                },
//                { // addr_main
//                    1, // if_data
//                    4, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                },
//                { // addr_sub
//                    1, // if_data
//                    5, // data_index
//                    FAC_data_HEX, // data_type
//                    {1,{0}}, // data_HEX: num, index[]
//                    {0,{0}}, // data_BCD: num, index[]
//                    {0,{0}}  // data_ASCII: num, index[]
//                }
//
//            }
//        },
//        { // sys_reset
//            1,  // if_reset
//            {0x20}, // buf
//            3, // index
//            1, // len
//            1, // if_sys_addr
//            0, // sys_addr
//            2, // sys_addr_index
//        }        
//    },      
//    
//};



