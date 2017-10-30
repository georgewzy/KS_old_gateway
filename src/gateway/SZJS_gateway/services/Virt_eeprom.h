/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.h
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file contains all the functions prototypes for the
*                      EEPROM emulation firmware library.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VIRT_EEPROM_H__
#define __VIRT_EEPROM_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
//#include "platform_config.h"
////#include "sys_def.h"
#include "sys_misc.h"

//#include "polynomial_fitting.h" 

/* Exported constants --------------------------------------------------------*/
/* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)0x4000  /* Page size = 16KByte */

/* Device voltage range supposed to be [2.7V to 3.6V], the operation will 
   be done by word  */
#define VOLTAGE_RANGE           (uint8_t)VoltageRange_3

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  ((uint32_t)0x08008000) /* EEPROM emulation start address:
                                                  from sector2 : after 16KByte of used 
                                                  Flash memory */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))
#define PAGE0_ID               FLASH_Sector_2

#define PAGE1_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x4000))
#define PAGE1_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))
#define PAGE1_ID               FLASH_Sector_3

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
#define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL             ((uint8_t)0x80)

/* Variables' number */
//#define NumbOfVar               ((uint16_t)12)
#define NUM_OF_VAR_MAX					(sizeof(VirtAddVarTab_saved)/2)

extern struct VirtEeprom *pVirtEeprom;
extern uint16_t *VirtAddVarTab;
extern const uint16_t VirtAddVarTab_saved[];

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern uint16_t EE_Init(void);
extern uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
extern uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

extern void eeprom_test(void);

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
