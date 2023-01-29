/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup FLASH_EraseProgram
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_ROW_SIZE          32

/* !!! Be careful the user area should be in another bank than the code !!! */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_511 + FLASH_PAGE_SIZE - 1   /* End @ of user Flash area */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t BankNumber = 0;
uint32_t Address = 0, PAGEError = 0;
extern uint32_t data_index2 ,data_index1;
uint32_t data_index=0;
extern uint64_t emk[100][3];
uint64_t data[300];

__IO uint32_t MemoryProgramStatus = 0;
__IO uint64_t data64 = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

/* Private function prototypes -----------------------------------------------*/

static uint32_t GetBank(uint32_t Address);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int	flash_write(uint32_t Address, int length, uint64_t *data, uint32_t OFFSET )
{
		/* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
	Address = Address +(OFFSET*8);
	data_index = 0;
  HAL_FLASH_Unlock();
  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	while(data_index<length){
		if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, data[data_index]) == HAL_OK ){
			Address = Address + 8;
			data_index++;
		}
		else{
			return 1;
		}
	}
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
	return 0;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;
  
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
  	/* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
  	/* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
  
  return bank;
}

int flash_erase(uint32_t Address, uint32_t NoOfPages)
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 

  /* Get the bank */
  BankNumber = GetBank(Address);

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page     = Address;
	EraseInitStruct.NbPages  = NoOfPages;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    /*
      Error occurred while mass erase.
      User can add here some code to deal with this error.
      To know the code error, user can call function 'HAL_FLASH_GetError()'
    */
		return 1;
//    /* Infinite loop */
//    while (1)
//    {
//      /* Make LED2 blink (100ms on, 2s off) to indicate error in Erase operation */
//      BSP_LED_On(LED2);
//      HAL_Delay(100);
//      BSP_LED_Off(LED2);
//      HAL_Delay(2000);
//    }
  }
	
	  HAL_FLASH_Lock();

	return 0;
}

int flash_read( uint32_t Address, int length, uint64_t *data)
{
  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
	data_index=0;
  while (data_index<length)
  {
		data[data_index]=*(__IO uint32_t *)Address;
		data_index++;
    Address = Address + 8;
  }
	return flash_read_ok;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
