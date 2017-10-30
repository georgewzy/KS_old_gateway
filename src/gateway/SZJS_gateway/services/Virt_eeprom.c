/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file provides all the EEPROM emulation firmware functions.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Virt_eeprom.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



#ifdef __GNUC__
// reference
const uint8_t VirtEeprom_table[PAGE_SIZE*2] __attribute__ ((section(¡°.MyZone¡±)));
#elif defined (__CC_ARM)
const uint8_t VirtEeprom_table[PAGE_SIZE*2] __attribute__((at(EEPROM_START_ADDRESS)));
#endif


//////static ErrorStatus  HSEStartUpStatus;
FLASH_Status FlashStatus;
uint16_t VarValue = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t *VirtAddVarTab	= (void *)VirtAddVarTab_saved;
struct VirtEeprom *pVirtEeprom = (void *)VirtAddVarTab_saved;

const uint16_t VirtAddVarTab_saved[] = {

0xF000,0xF001,0xF002,0xF003,0xF004,0xF005,0xF006,0xF007,
0xF008,0xF009,0xF00A,0xF00B,0xF00C,0xF00D,0xF00E,0xF00F,
0xF010,0xF011,0xF012,0xF013,0xF014,0xF015,0xF016,0xF017,
0xF018,0xF019,0xF01A,0xF01B,0xF01C,0xF01D,0xF01E,0xF01F,
0xF020,0xF021,0xF022,0xF023,0xF024,0xF025,0xF026,0xF027,
0xF028,0xF029,0xF02A,0xF02B,0xF02C,0xF02D,0xF02E,0xF02F,
0xF030,0xF031,0xF032,0xF033,0xF034,0xF035,0xF036,0xF037,
0xF038,0xF039,0xF03A,0xF03B,0xF03C,0xF03D,0xF03E,0xF03F,
0xF040,0xF041,0xF042,0xF043,0xF044,0xF045,0xF046,0xF047,
0xF048,0xF049,0xF04A,0xF04B,0xF04C,0xF04D,0xF04E,0xF04F,
0xF050,0xF051,0xF052,0xF053,0xF054,0xF055,0xF056,0xF057,
0xF058,0xF059,0xF05A,0xF05B,0xF05C,0xF05D,0xF05E,0xF05F,
0xF060,0xF061,0xF062,0xF063,0xF064,0xF065,0xF066,0xF067,
0xF068,0xF069,0xF06A,0xF06B,0xF06C,0xF06D,0xF06E,0xF06F,
0xF070,0xF071,0xF072,0xF073,0xF074,0xF075,0xF076,0xF077,
0xF078,0xF079,0xF07A,0xF07B,0xF07C,0xF07D,0xF07E,0xF07F,

0xF100,0xF101,0xF102,0xF103,0xF104,0xF105,0xF106,0xF107,
0xF108,0xF109,0xF10A,0xF10B,0xF10C,0xF10D,0xF10E,0xF10F,
0xF110,0xF111,0xF112,0xF113,0xF114,0xF115,0xF116,0xF117,
0xF118,0xF119,0xF11A,0xF11B,0xF11C,0xF11D,0xF11E,0xF11F,
0xF120,0xF121,0xF122,0xF123,0xF124,0xF125,0xF126,0xF127,
0xF128,0xF129,0xF12A,0xF12B,0xF12C,0xF12D,0xF12E,0xF12F,
0xF130,0xF131,0xF132,0xF133,0xF134,0xF135,0xF136,0xF137,
0xF138,0xF139,0xF13A,0xF13B,0xF13C,0xF13D,0xF13E,0xF13F,
0xF140,0xF141,0xF142,0xF143,0xF144,0xF145,0xF146,0xF147,
0xF148,0xF149,0xF14A,0xF14B,0xF14C,0xF14D,0xF14E,0xF14F,
0xF150,0xF151,0xF152,0xF153,0xF154,0xF155,0xF156,0xF157,
0xF158,0xF159,0xF15A,0xF15B,0xF15C,0xF15D,0xF15E,0xF15F,
0xF160,0xF161,0xF162,0xF163,0xF164,0xF165,0xF166,0xF167,
0xF168,0xF169,0xF16A,0xF16B,0xF16C,0xF16D,0xF16E,0xF16F,
0xF170,0xF171,0xF172,0xF173,0xF174,0xF175,0xF176,0xF177,
0xF178,0xF179,0xF17A,0xF17B,0xF17C,0xF17D,0xF17E,0xF17F,

};

/* Global variable used to store variable value in read sequence */
static uint16_t DataVar = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
////extern uint16_t VirtAddVarTab[sizeof(struct VirtEeprom)/2];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static FLASH_Status EE_Format(void);
static uint16_t EE_FindValidPage(uint8_t Operation);
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data);

/*******************************************************************************
* Function Name  : EE_Init
* Description    : Restore the pages to a known good state in case of pages'
*                  status corruption after a power loss.
* Input          : None.
* Output         : None.
* Return         : - Flash error code: on write Flash error
*                  - FLASH_COMPLETE: on success
*******************************************************************************/
uint16_t EE_Init(void)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;
  uint16_t VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;
  int16_t x = -1;
  uint16_t  FlashStatus;
	uint16_t i;
    
    static uint16_t inited = 0;
    
    if (inited)
    {
        return FLASH_COMPLETE;
    }
    inited = 1;

	if(sizeof(struct VirtEeprom)&(unsigned int)0x1)
	{
		// TODO: by song
		
		while(1);
	}
	if(sizeof(struct VirtEeprom)/2 > NUM_OF_VAR_MAX)
	{
		// TODO : by song

		while(1);
	}
	
	FLASH_Unlock();

  /* Get Page0 status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Check for invalid header states and repair if necessary */
  switch (PageStatus0)
  {
    case ERASED:
      if (PageStatus1 == VALID_PAGE) /* Page0 erased, Page1 valid */
      {
        /* Erase Page0 */
        FlashStatus = FLASH_EraseSector(PAGE0_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else if (PageStatus1 == RECEIVE_DATA) /* Page0 erased, Page1 receive */
      {
        /* Erase Page0 */
        FlashStatus = FLASH_EraseSector(PAGE0_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
						// Add by song: initial content //
				for(i=0;i<sizeof(struct VirtEeprom)/2;i++)
				{
					EE_WriteVariable(*((uint16_t *)pVirtEeprom + i),*((uint16_t *)&eeprom_init_data + i));
				}
      }

      break;

    case RECEIVE_DATA:
      if (PageStatus1 == VALID_PAGE) /* Page0 receive, Page1 valid */
      {
        /* Transfer data from Page1 to Page0 */
        for (VarIdx = 0; VarIdx < sizeof(struct VirtEeprom)/2; VarIdx++)
        {
          if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            /* Read the last variables' updates */
            ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page0 */
              EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
              				FLASH_Lock();
								return EepromStatus;
              }
            }
          }
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
        /* Erase Page1 */
        FlashStatus = FLASH_EraseSector(PAGE1_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 receive, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = FLASH_EraseSector(PAGE1_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      break;

    case VALID_PAGE:
      if (PageStatus1 == VALID_PAGE) /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 valid, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = FLASH_EraseSector(PAGE1_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Transfer data from Page0 to Page1 */
        for (VarIdx = 0; VarIdx < sizeof(struct VirtEeprom)/2; VarIdx++)
        {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            /* Read the last variables' updates */
            ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page1 */
              EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
              				FLASH_Lock();
								return EepromStatus;
              }
            }
          }
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        			FLASH_Lock();
					return FlashStatus;
        }
        /* Erase Page0 */
        FlashStatus = FLASH_EraseSector(PAGE0_ID, VOLTAGE_RANGE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
        		FLASH_Lock();
		  		return FlashStatus;
        }
      }
      break;

    default:  /* Any other state -> format eeprom */
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      FlashStatus = EE_Format();
      /* If erase/program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
      		FLASH_Lock();
				return FlashStatus;
      }
				
      // Add by song:  //
			for(i=0;i<sizeof(struct VirtEeprom)/2;i++)
			{
				EE_WriteVariable(*((uint16_t *)pVirtEeprom + i),*((uint16_t *)&eeprom_init_data + i));
			}
				

      break;
  }

	FLASH_Lock();
  return FLASH_COMPLETE;
}

/*******************************************************************************
* Function Name  : EE_ReadVariable
* Description    : Returns the last stored variable data, if found, which
*                  correspond to the passed virtual address
* Input          : - VirtAddress: Variable virtual address
*                  - Data: Global variable contains the read variable value
* Output         : None
* Return         : - Success or error status:
*                      - 0: if variable was found
*                      - 1: if the variable was not found
*                      - NO_VALID_PAGE: if no valid page was found.
*******************************************************************************/
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data)
{
  uint16_t ValidPage = PAGE0;
  uint16_t AddressValue = 0x5555, ReadStatus = 1;
  uint32_t Address = EEPROM_START_ADDRESS, PageStartAddress = EEPROM_START_ADDRESS;

  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  Address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from end */
  while (Address > (PageStartAddress + 2))
  {
    /* Get the current location content to be compared with virtual address */
    AddressValue = (*(__IO uint16_t*)Address);

    /* Compare the read address with the virtual address */
    if (AddressValue == VirtAddress)
    {
      /* Get content of Address-2 which is variable value */
      *Data = (*(__IO uint16_t*)(Address - 2));

      /* In case variable value is read, reset ReadStatus flag */
      ReadStatus = 0;

      break;
    }
    else
    {
      /* Next address location */
      Address = Address - 4;
    }
  }

  /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
  return ReadStatus;
}

/*******************************************************************************
* Function Name  : EE_WriteVariable
* Description    : Writes/upadtes variable data in EEPROM.
* Input          : - VirtAddress: Variable virtual address
*                  - Data: 16 bit data to be written
* Output         : None
* Return         : - Success or error status:
*                      - FLASH_COMPLETE: on success,
*                      - PAGE_FULL: if valid page is full
*                      - NO_VALID_PAGE: if no valid page was found
*                      - Flash error code: on write Flash error
*******************************************************************************/
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  uint16_t Status = 0;

	FLASH_Unlock();

  /* Write the variable virtual address and value in the EEPROM */
  Status = EE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* In case the EEPROM active page is full */
  if (Status == PAGE_FULL)
  {
    /* Perform Page transfer */
    Status = EE_PageTransfer(VirtAddress, Data);
  }

	/* Return last operation status */
  return Status;
}

/*******************************************************************************
* Function Name  : EE_Format
* Description    : Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
* Input          : None
* Output         : None
* Return         : Status of the last operation (Flash write or erase) done during
*                  EEPROM formating
*******************************************************************************/
static FLASH_Status EE_Format(void)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;

  /* Erase Page0 */
  FlashStatus = FLASH_EraseSector(PAGE0_ID, VOLTAGE_RANGE);

  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
  FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Erase Page1 */
  FlashStatus = FLASH_EraseSector(PAGE1_ID, VOLTAGE_RANGE);

  /* Return Page1 erase operation status */
  return FlashStatus;
}

/*******************************************************************************
* Function Name  : EE_FindValidPage
* Description    : Find valid Page for write or read operation
* Input          : - Operation: operation to achieve on the valid page:
*                      - READ_FROM_VALID_PAGE: read operation from valid page
*                      - WRITE_IN_VALID_PAGE: write operation from valid page
* Output         : None
* Return         : Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
*                  of no valid page was found
*******************************************************************************/
static uint16_t EE_FindValidPage(uint8_t Operation)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;

  /* Get Page0 actual status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

  /* Get Page1 actual status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Write or read operation */
  switch (Operation)
  {
    case WRITE_IN_VALID_PAGE:   /* ---- Write operation ---- */
      if (PageStatus1 == VALID_PAGE)
      {
        /* Page0 receiving data */
        if (PageStatus0 == RECEIVE_DATA)
        {
          return PAGE0;         /* Page0 valid */
        }
        else
        {
          return PAGE1;         /* Page1 valid */
        }
      }
      else if (PageStatus0 == VALID_PAGE)
      {
        /* Page1 receiving data */
        if (PageStatus1 == RECEIVE_DATA)
        {
          return PAGE1;         /* Page1 valid */
        }
        else
        {
          return PAGE0;         /* Page0 valid */
        }
      }
      else
      {
        return NO_VALID_PAGE;   /* No valid Page */
      }

    case READ_FROM_VALID_PAGE:  /* ---- Read operation ---- */
      if (PageStatus0 == VALID_PAGE)
      {
        return PAGE0;           /* Page0 valid */
      }
      else if (PageStatus1 == VALID_PAGE)
      {
        return PAGE1;           /* Page1 valid */
      }
      else
      {
        return NO_VALID_PAGE ;  /* No valid Page */
      }

    default:
      return PAGE0;             /* Page0 valid */
  }
}

/*******************************************************************************
* Function Name  : EE_VerifyPageFullWriteVariable
* Description    : Verify if active page is full and Writes variable in EEPROM.
* Input          : - VirtAddress: 16 bit virtual address of the variable
*                  - Data: 16 bit data to be written as variable value
* Output         : None
* Return         : - Success or error status:
*                      - FLASH_COMPLETE: on success
*                      - PAGE_FULL: if valid page is full
*                      - NO_VALID_PAGE: if no valid page was found
*                      - Flash error code: on write Flash error
*******************************************************************************/
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint16_t ValidPage = PAGE0;
  uint32_t Address = EEPROM_START_ADDRESS, PageEndAddress = EEPROM_START_ADDRESS+PAGE_SIZE;

  /* Get valid Page for write operation */
  ValidPage = EE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from begining */
  while (Address < PageEndAddress)
  {
    /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF)
    {
      /* Set variable data */
      FlashStatus = FLASH_ProgramHalfWord(Address, Data);
      /* If program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
        return FlashStatus;
      }
      /* Set variable virtual address */
      FlashStatus = FLASH_ProgramHalfWord(Address + 2, VirtAddress);
      /* Return program operation status */
      return FlashStatus;
    }
    else
    {
      /* Next address location */
      Address = Address + 4;
    }
  }

  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}

/*******************************************************************************
* Function Name  : EE_PageTransfer
* Description    : Transfers last updated variables data from the full Page to
*                  an empty one.
* Input          : - VirtAddress: 16 bit virtual address of the variable
*                  - Data: 16 bit data to be written as variable value
* Output         : None
* Return         : - Success or error status:
*                      - FLASH_COMPLETE: on success,
*                      - PAGE_FULL: if valid page is full
*                      - NO_VALID_PAGE: if no valid page was found
*                      - Flash error code: on write Flash error
*******************************************************************************/
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint32_t NewPageAddress = EEPROM_START_ADDRESS;
  uint16_t OldPageId=0;
  uint16_t ValidPage = PAGE0, VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;

  /* Get active Page for read operation */
  ValidPage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  if (ValidPage == PAGE1)       /* Page1 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE0_BASE_ADDRESS;

    /* Old page ID where variable will be taken from */
    OldPageId = PAGE1_ID;
  }
  else if (ValidPage == PAGE0)  /* Page0 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE1_BASE_ADDRESS;

    /* Old page ID where variable will be taken from */
    OldPageId = PAGE0_ID;
  }
  else
  {
    return NO_VALID_PAGE;       /* No valid Page */
  }

  /* Set the new Page status to RECEIVE_DATA status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, RECEIVE_DATA);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Write the variable passed as parameter in the new active page */
  EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddress, Data);
  /* If program operation was failed, a Flash error code is returned */
  if (EepromStatus != FLASH_COMPLETE)
  {
    return EepromStatus;
  }

  /* Transfer process: transfer variables from old to the new active page */
  for (VarIdx = 0; VarIdx < sizeof(struct VirtEeprom)/2; VarIdx++)
  {
    if (VirtAddVarTab[VarIdx] != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      /* Read the other last variable updates */
      ReadStatus = EE_ReadVariable(VirtAddVarTab[VarIdx], &DataVar);
      /* In case variable corresponding to the virtual address was found */
      if (ReadStatus != 0x1)
      {
        /* Transfer the variable to the new active page */
        EepromStatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
        /* If program operation was failed, a Flash error code is returned */
        if (EepromStatus != FLASH_COMPLETE)
        {
          return EepromStatus;
        }
      }
    }
  }

  /* Erase the old Page: Set old Page status to ERASED status */
  FlashStatus = FLASH_EraseSector(OldPageId, VOLTAGE_RANGE);
  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Set new Page status to VALID_PAGE status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, VALID_PAGE);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    return FlashStatus;
  }

  /* Return last operation flash status */
  return FlashStatus;
}


//void eeprom_test(void)
//{
//  uint16_t temp_buf[6];
//	uint32_t counter = 0;
//	/* Unlock the Flash Program Erase controller */
//  //FLASH_Unlock();

//  /* EEPROM Init */
//  ////EE_Init();

//		EE_ReadVariable(pVirtEeprom->bd_addr[0][0],&temp_buf[0]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][1],&temp_buf[1]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][2],&temp_buf[2]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][0],&temp_buf[3]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][1],&temp_buf[4]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][2],&temp_buf[5]);
//		
//		if(  (temp_buf[0] != 0x1234)
//			 ||(temp_buf[1] != 0x5678)
//			 ||(temp_buf[2] != 0x90ab)
//			 ||(temp_buf[3] != 0x1234)
//			 ||(temp_buf[4] != 0x5678)
//			 ||(temp_buf[5] != 0x90ab)
//			)
//		{
//			rt_kprintf(" Wrong !!! \n");
//			//while(1);
//		}


//		EE_WriteVariable(pVirtEeprom->bd_addr[0][0],0x1234);
//		EE_WriteVariable(pVirtEeprom->bd_addr[0][1],0x5678);
//		EE_WriteVariable(pVirtEeprom->bd_addr[0][2],0x90ab);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][0],0x1234);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][1],0x5678);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][2],0x90ab);

//		EE_ReadVariable(pVirtEeprom->bd_addr[0][0],&temp_buf[0]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][1],&temp_buf[1]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][2],&temp_buf[2]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][0],&temp_buf[3]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][1],&temp_buf[4]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][2],&temp_buf[5]);
//		
//		if(  (temp_buf[0] != 0x1234)
//			 ||(temp_buf[1] != 0x5678)
//			 ||(temp_buf[2] != 0x90ab)
//			 ||(temp_buf[3] != 0x1234)
//			 ||(temp_buf[4] != 0x5678)
//			 ||(temp_buf[5] != 0x90ab)
//			)
//		{
//			rt_kprintf(" Wrong !!! \n");
//			//while(1);
//		}

//		
//		SYS_SoftReset();
//		while(1);
//	
//	
//	
//	while(1)
//	{	
//		EE_WriteVariable(pVirtEeprom->bd_addr[0][0],0x1234);
//		EE_WriteVariable(pVirtEeprom->bd_addr[0][1],0x5678);
//		EE_WriteVariable(pVirtEeprom->bd_addr[0][2],0x90ab);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][0],0x1234);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][1],0x5678);
//		EE_WriteVariable(pVirtEeprom->bd_addr[1][2],0x90ab);

//		EE_ReadVariable(pVirtEeprom->bd_addr[0][0],&temp_buf[0]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][1],&temp_buf[1]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[0][2],&temp_buf[2]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][0],&temp_buf[3]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][1],&temp_buf[4]);
//		EE_ReadVariable(pVirtEeprom->bd_addr[1][2],&temp_buf[5]);
//		
//		if(  (temp_buf[0] != 0x1234)
//			 ||(temp_buf[1] != 0x5678)
//			 ||(temp_buf[2] != 0x90ab)
//			 ||(temp_buf[3] != 0x1234)
//			 ||(temp_buf[4] != 0x5678)
//			 ||(temp_buf[5] != 0x90ab)
//			)
//		{
//			rt_kprintf(" Wrong !!! \n");
//			while(1);
//		}
//		
//		if(counter >= 10000)
//		{
//			rt_kprintf(" Test complete ! \n");
//			while(1);
//		}
//		
//		counter++;
//	}
//	
//	
//	
//	
//	EE_WriteVariable(pVirtEeprom->bd_addr[0][0],0x1234);
//	EE_WriteVariable(pVirtEeprom->bd_addr[0][1],0x5678);
//	EE_WriteVariable(pVirtEeprom->bd_addr[0][2],0x90ab);
//	EE_WriteVariable(pVirtEeprom->bd_addr[1][0],0x1234);
//	EE_WriteVariable(pVirtEeprom->bd_addr[1][1],0x5678);
//	EE_WriteVariable(pVirtEeprom->bd_addr[1][2],0x90ab);

//	EE_ReadVariable(pVirtEeprom->bd_addr[0][0],&temp_buf[0]);
//	EE_ReadVariable(pVirtEeprom->bd_addr[0][1],&temp_buf[1]);
//	EE_ReadVariable(pVirtEeprom->bd_addr[0][2],&temp_buf[2]);
//	EE_ReadVariable(pVirtEeprom->bd_addr[1][0],&temp_buf[3]);
//	EE_ReadVariable(pVirtEeprom->bd_addr[1][1],&temp_buf[4]);
//	EE_ReadVariable(pVirtEeprom->bd_addr[1][2],&temp_buf[5]);

//	while(1);

////	EE_WriteVariable(VirtAddVarTab[0],0x00a5);
////	EE_WriteVariable(VirtAddVarTab[1],0x005a);
////	EE_WriteVariable(VirtAddVarTab[2],0x00f0);
////	EE_WriteVariable(VirtAddVarTab[3],0x000f);
////	EE_WriteVariable(VirtAddVarTab[4],0xaa55);
////	EE_WriteVariable(VirtAddVarTab[5],0x55aa);
////
////	EE_ReadVariable(VirtAddVarTab[0],&temp_buf[0]);
////	EE_ReadVariable(VirtAddVarTab[1],&temp_buf[1]);
////	EE_ReadVariable(VirtAddVarTab[2],&temp_buf[2]);
////	EE_ReadVariable(VirtAddVarTab[3],&temp_buf[3]);
////	EE_ReadVariable(VirtAddVarTab[4],&temp_buf[4]);
////	EE_ReadVariable(VirtAddVarTab[5],&temp_buf[5]);
////
////
////	EE_WriteVariable(VirtAddVarTab[6],0x1);
////	EE_WriteVariable(VirtAddVarTab[7],0x2);
////	EE_WriteVariable(VirtAddVarTab[8],0x3);
////	EE_WriteVariable(VirtAddVarTab[9],0x4);
////	EE_WriteVariable(VirtAddVarTab[10],0x5);
////	EE_WriteVariable(VirtAddVarTab[11],0x6);
////
////	EE_ReadVariable(VirtAddVarTab[6],&temp_buf[0]);
////	EE_ReadVariable(VirtAddVarTab[7],&temp_buf[1]);
////	EE_ReadVariable(VirtAddVarTab[8],&temp_buf[2]);
////	EE_ReadVariable(VirtAddVarTab[9],&temp_buf[3]);
////	EE_ReadVariable(VirtAddVarTab[10],&temp_buf[4]);
////	EE_ReadVariable(VirtAddVarTab[11],&temp_buf[5]);
////
////  /* --- Store successively many values of the three variables in the EEPROM --- */
////  /* Store 1000 values of Variable1 in EEPROM */
////  for (VarValue = 0; VarValue < 1000; VarValue++)
////  {
////    EE_WriteVariable(VirtAddVarTab[0], VarValue);
////  }
////
////  /* Store 500 values of Variable2 in EEPROM */
////  for (VarValue = 0; VarValue < 500; VarValue++)
////  {
////    EE_WriteVariable(VirtAddVarTab[1], VarValue);
////  }
////
////  /* Store 800 values of Variable3 in EEPROM */
////  for (VarValue = 0; VarValue < 800; VarValue++)
////  {
////    EE_WriteVariable(VirtAddVarTab[2], VarValue);
////  }
////
////  while (1);
////
//}




/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
