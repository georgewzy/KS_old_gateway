#ifndef __VERSION_H__
#define __VERSION_H__


#define SW_VER "1.09"


/*=====================================================================================================
SW version:

    1.00 : 2016-7-20 19:13:31   Inital version.
    1.01 : 2016-7-28 11:06:28   Support SD card. Push key to start the ISP flashing.
    1.02 : 2016-7-29 18:22:11   backup
    1.03 : 2016-8-4 18:48:32    Support the lasar barcode reader. Once read a barcode, start programming
                                the firmware.
    1.04 : 2016-8-5 21:05:12    Added testing mode after the programming firmware.
    1.05 : 2016-8-11 19:25:23   Tested the testing mode, program the SN to OTP flash and lock OTP. 
    1.06 : 2016-8-26 17:58:31   Fix the auto-selftest bug.
    1.07 : 2016-12-30 12:32:58  Add some remarks. Set the optimization level to -O2.
    1.08 : 2017-3-28 16:51:09   Upgrade to support the AP_TOOL_V1_0 board, program IF usart3-->usart1, add PIN_BAT_DET and PIN_BAT_CTRL.
                                STM32_CONSOLE_USART 5 --> 3.  SPI1_CS2 PB2--> PC13. 
                                Add tool support testing ,see more detail about TOOL_SUPPORT_TESTING.
    1.09 ：2017-3-28 20:08:49   Add function which read fw_boot.bin from extern TF card. See MACRO TOOL_USE_EXT_FW.
    
=====================================================================================================*/







#endif // __VERSION_H__
