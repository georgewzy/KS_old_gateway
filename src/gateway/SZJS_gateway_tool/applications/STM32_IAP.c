/*
    使用说明： 
        支持2种升级方式， 一种为自动升级，一种为手动升级。
        自动升级方式（优先）： 
            由应用程序在文件系统指定目录新建一个update flag 文件。 bootloader检测到该文件后，
            开始寻找有效的firmware文件并IAP。
        手动升级方式：
            用户在按下Reset键前先按住 IAP 按键，并保持至少3s。 系统开始寻找firmware文件并IAP。
            
        进入系统控制台：
            用户在按下Reset键前先按住 IAP 按键，并保持不超过3s。 系统将在3s后进入控制台。
*/


#include "STM32_IAP.h"

#include <dfs.h>
#include <dfs_posix.h>

#include "flash_if.h"
#include "en_de_code.h"

#include "sys_status.h"

#define IAP_EN_DATA         0x12345678

#define IAP_FILE_PATH       FW_FILE_PATH
#define IAP_UPDATE_FLAG     FW_UPGRADE_FLAG


#define STM_PAGE_SIZE		2048			//注意：STM32F103ZET6的FLASH页大小为2K


uint8_t prio_sys_IAP = 8;

extern uint8_t sys_inited_flag;

typedef  void (*pFunction)(void);

pFunction Jump_To_Application;
uint32_t JumpAddress;
volatile uint32_t FlashProtection = 0;

uint8_t buffer[STM_PAGE_SIZE]; 
uint8_t buffer2[STM_PAGE_SIZE]; 

int file_program(int fd)
{
    struct stat file_state;
    uint32_t n = 0;
    uint8_t i = 0;
    uint32_t addr = 0;
    uint32_t err = 0;
    int br = 0;
    
    fstat( fd, &file_state);
    if (file_state.st_size == 0)
    {
        rt_kprintf("Firmware file is invaliad !\n");
        close(fd);
        return -1;
    }
    
    FLASH_If_Init();
    
//  /* Test if any sector of Flash memory where user application will be loaded is write protected */
//  if (FLASH_If_GetWriteProtectionStatus() == 0)   
//  {
//    FlashProtection = 1;
//  }
//  else
//  {
//    FlashProtection = 0;
//  }

    rt_kprintf("Erasing the flash .\n");
////  FLASH_If_Erase(APPLICATION_START_ADDRESS);   
  FLASH_If_Erase(APPLICATION_START_ADDRESS, APPLICATION_START_ADDRESS + file_state.st_size); 

//    // Song:temp. for test.
//    FLASH_Lock();
//    return 0;
    
    
    
    rt_kprintf("Writing the flash .\n");
    rt_kprintf("\n|");
    
    
	/*执行主要的IAP功能*/
	while(1)
	{
		/*每次读取一个页的数据到内存缓冲区，注意：STM32F103ZE的页大小为2K*/
////	    res = f_read(&file, buffer, STM_PAGE_SIZE, &br);
	    br = read(fd, buffer, 512);
        
	    //if (res != 512) break;   

// 		/*然后就是永恒的4步骤：解锁、擦除、更新、上锁*/
// 		FLASH_Unlock();
// 		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
// 		FLASH_ErasePage(FLASH_APP_ADDR + PageOffest);
// 		for(ByteOffest = 0; ByteOffest < STM_PAGE_SIZE; ByteOffest += 2)
// 		{
// 			/*更新FLASH，注意当前操作的实际位置：APP基地址FLASH_APP_ADDR+页偏移字节PageOffest+当前页内的字节偏移ByteOffest*/
// 			FLASH_ProgramHalfWord(FLASH_APP_ADDR + PageOffest + ByteOffest, *(u16*)(buffer + ByteOffest));
// 		} 
// 		FLASH_Lock();
// 		PageOffest += STM_PAGE_SIZE;
        
        #if 1  // 1: Decode; 0: No need decode.
        decode(buffer, buffer2, br);
        #else
        memcpy(buffer2, buffer, br);
        #endif 
        
        
//        FLASH_Unlock();
        addr = APPLICATION_START_ADDRESS + n*512;
        err = FLASH_If_Write(&addr, (uint32_t *)buffer2, br);
        if(err != 0) 
        {
            FLASH_Lock();
            
            rt_kprintf("Flash write error !\n");
            return -1;
        }
        
        // If read date less than 512, means that it is the last date page, exit.
        if (br != 512) 
        {
            rt_kprintf("|\n");
            rt_kprintf("Flash writed done .\n");
            break;
        }
//        FLASH_Lock();
        
        n ++;
        
        
        if (n%10 == 0)
        {
            rt_kprintf(">");
        }
        
////		/*每更新完1页，让LED状态翻转一次*/
////		i = !i;
        
////		if(i)
////			LED_BLUE_OFF();
////		else
////			LED_BLUE_ON();
	}
    
	/*关闭文件，卸载文件系统*/
//	f_close(&file);
//	f_mount(0, 0);
    FLASH_Lock();
    
    return 0;
}




void NVIC_DeInit(void)
{
  u32 index = 0;
  
  NVIC->ICER[0] = 0xFFFFFFFF;
  NVIC->ICER[1] = 0x000007FF;
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICPR[1] = 0x000007FF;
  
  for(index = 0; index < 240; index++)
  {
     NVIC->IP[index] = 0x00;
  }
}

void start_APP(void)
{
    
	NVIC_InitTypeDef NVIC_InitStructure;

    
    // Clear and close all intterupts, disable the used functions.
    ////rt_hw_interrupt_disable();
    rt_kprintf("\n<<++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>\n");
    rt_thread_delay(10);
    
    
//	/* Disable the DMA2_Stream7 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//	NVIC_Init(&NVIC_InitStructure);
//    
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//    
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//    
//    
//    USART1->CR1 = 0;    
//    USART2->CR1 = 0;    
//    NVIC_usart_deinit();
//    

    
//    
//    SPI1->CR1 = 0;
//    
//    SysTick->LOAD = 0;
//    SysTick->VAL = 0;
//    SysTick->CTRL = 0;
    
    NVIC_DeInit();
    RCC_DeInit();

//    // Set the new VectorTable address. 
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);

//    
//    __set_PRIMASK(0);
    
    /* Test if user code is programmed starting from address "APPLICATION_START_ADDRESS" */
    if (((*(__IO uint32_t*)APPLICATION_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    { 
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (APPLICATION_START_ADDRESS + 4);
      
      // Song: for test.      
      //JumpAddress = JumpAddress - 1;  
        
      Jump_To_Application = (pFunction) JumpAddress;
        
      //__set_PSP(*(__IO uint32_t*) 0x00000000);
        
       // Song: As the RTT run with PSP, we need switch it to MSP first. 
       __set_CONTROL(0);
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_START_ADDRESS);
        
      Jump_To_Application();
    }
  

  while (1)
  {}

}



//char thread_wifi_app_stack[4096];
rt_thread_t thread_sys_IAP;
//struct rt_mutex mut_file_RW;
//struct rt_mutex mut_caching;
//struct rt_semaphore sem_wifi_up;
//struct rt_semaphore sem_server_start;

void rt_thread_entry_sys_IAP(void* parameter)
{
    int fd;
    int res = 0;
    int cnt = 0;
    
    /* Enable the PWR APB1 Clock Interface */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    
    while(1)
    {
        if (sys_inited_flag)
        {
            break;
        }
        rt_thread_delay(10);
        
        cnt ++;
        if (cnt >= 20)
        {
            break;
        }
    }
    
    
    fd = open(IAP_UPDATE_FLAG, O_RDWR, 0);
    if ( fd >= 0 )
    {
        close(fd);
        rt_kprintf("Firmwair update flag is set.\n");
        
        fd = open( IAP_FILE_PATH, O_RDONLY, 0);
        if (fd >= 0)
        {
            rt_kprintf("Firmwair file have found.\n");
            
            //LED_GREEN_ON();
            
            res = file_program(fd);
            if (res != 0)
            {
                
                sys_ctrl.sys_state = SYS_state_IAP_failed;
                rt_thread_delay(RT_TICK_PER_SECOND*3);
                // Song: LED_GREEN off, LED_BLUE flash 3 times.
//                LED_GREEN_OFF();
//                
//                LED_BLUE_ON();
//                rt_thread_delay(RT_TICK_PER_SECOND/2);
//                LED_BLUE_OFF();
//                rt_thread_delay(RT_TICK_PER_SECOND/2);
//                LED_BLUE_ON();
//                rt_thread_delay(RT_TICK_PER_SECOND/2);
//                LED_BLUE_OFF();
//                rt_thread_delay(RT_TICK_PER_SECOND/2);
//                LED_BLUE_ON();
//                rt_thread_delay(RT_TICK_PER_SECOND/2);
//                LED_BLUE_OFF();
                //return ;
            }
            else
            {
                rt_kprintf("Firmwair updated successfully !\n");
                close(fd);
                
                // Clear the firmware update flag file.
                rm(IAP_UPDATE_FLAG);
                
                rt_thread_delay(10);
                
                sys_ctrl.sys_state = SYS_state_IAP_success;
                rt_thread_delay(RT_TICK_PER_SECOND*3);
                
//                // Song: LED_GREEN off, LED_BLUE light 3 second.
//                LED_GREEN_OFF();
//                
//                LED_BLUE_ON();
//                rt_thread_delay(RT_TICK_PER_SECOND*3);
//                LED_BLUE_OFF();
                
                // Enter the APP code.
                start_APP();
                
                while(1)
                {
                    rt_thread_delay(5);
                }
            }
            
            
            
        }
        else
        {
            rt_kprintf("No firmwair file !\n");
        }

    }
    else
    {
        
        #if IAP_TRIG_KEY_EN
        // If IAP key is push down and keep more than 3s, find the firmware file and IAP.
        if (IF_IAP_TRIG())
        {
            rt_thread_delay(300);  // At least 3s.
            if (IF_IAP_TRIG())
            {
                rt_kprintf("IAP key active.\n");
                
                LED_GREEN_ON();
                
                fd = open( IAP_FILE_PATH, O_RDONLY, 0);
                if (fd >= 0)
                {
                    rt_kprintf("Firmwair file have found.\n");
                    
                    res = file_program(fd);
                    if (res != 0)
                    {
                        sys_ctrl.sys_state = SYS_state_IAP_failed;
                        rt_thread_delay(RT_TICK_PER_SECOND*3);
                        // Song: LED_GREEN off, LED_BLUE flash 3 times.
//                        LED_GREEN_OFF();
//                        
//                        LED_BLUE_ON();
//                        rt_thread_delay(RT_TICK_PER_SECOND/2);
//                        LED_BLUE_OFF();
//                        rt_thread_delay(RT_TICK_PER_SECOND/2);
//                        LED_BLUE_ON();
//                        rt_thread_delay(RT_TICK_PER_SECOND/2);
//                        LED_BLUE_OFF();
//                        rt_thread_delay(RT_TICK_PER_SECOND/2);
//                        LED_BLUE_ON();
//                        rt_thread_delay(RT_TICK_PER_SECOND/2);
//                        LED_BLUE_OFF();
                        //return ;
                    }
                    else
                    {
                        rt_kprintf("Firmwair updated successfully !\n");
                        close(fd);
                        rt_thread_delay(10);
                        
                        sys_ctrl.sys_state = SYS_state_IAP_success;
                        rt_thread_delay(RT_TICK_PER_SECOND*3);
                        // Song: LED_GREEN off, LED_BLUE light 3 second.
//                        LED_GREEN_OFF();
//                        
//                        LED_BLUE_ON();
//                        rt_thread_delay(RT_TICK_PER_SECOND*3);
                        
                        // Clear the firmware update flag file.
                        //rm(IAP_UPDATE_FLAG);
                        
                        // Enter the APP code.
                        start_APP();
                        
                        while(1)
                        {
                            rt_thread_delay(5);
                        }
                    }
                    
                    
                    
                }
                else
                {
                    rt_kprintf("No firmwair file !\n");
                }

            }
            else
            {
                while(1)
                {
                    rt_thread_delay(10);
                }
            }
        }
        #endif // IAP_TRIG_KEY_EN

        
    }
    
    
    rt_kprintf("No active input, auto-run the APP !\n");
    rt_thread_delay(10);
    
    // Enter the APP code.
    start_APP();
    
    
    while(1)
    {
        rt_thread_delay(2);
    }
    
    
    
}
