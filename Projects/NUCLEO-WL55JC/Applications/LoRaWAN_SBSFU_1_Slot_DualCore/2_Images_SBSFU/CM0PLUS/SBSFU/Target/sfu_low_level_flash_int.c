/**
  ******************************************************************************
  * @file    sfu_low_level_flash_int.c
  * @author  MCD Application Team
  * @brief   SFU Flash Low Level Interface module
  *          This file provides set of firmware functions to manage SFU internal
  *          flash low level interface.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "se_interface_bootloader.h"
#include "string.h"
#include "sfu_fsm_states.h"
#include "sfu_error.h"

/* Private defines -----------------------------------------------------------*/
#define NB_PAGE_SECTOR_PER_ERASE  2U    /*!< Nb page erased per erase */

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t DoubleECC_Error_Counter = 0U;


static SFU_ErrorStatus SFU_LL_FLASH_INT_Clear_Error(void);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  This function initialize the internal flash interface if required
  * @param  none
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Init(void)
{
  return SFU_SUCCESS;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in user flash area
  * @param  pFlashStatus: SFU_FLASH Status pointer
  * @param  pStart: flash address to be erased
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Erase_Size(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t *pStart, uint32_t Length)
{
  uint32_t page_error = 0U;
  uint32_t start = (uint32_t)pStart;
  FLASH_EraseInitTypeDef p_erase_init;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t first_page;
  uint32_t nb_pages;
  uint32_t chunk_nb_pages;
  uint32_t length = Length;

  /* Check the pointers allocation */
  if (pFlashStatus == NULL)
  {
    return SFU_ERROR;
  }

  *pFlashStatus = SFU_FLASH_SUCCESS;

  /* Clear error flags raised during previous operation */
  e_ret_status = SFU_LL_FLASH_INT_Clear_Error();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      first_page = SFU_LL_FLASH_INT_GetPage(start);
      /* Get the number of pages to erase from 1st page */
      nb_pages = SFU_LL_FLASH_INT_GetPage(start + length - 1U) - first_page + 1U;
      p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
      /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
      do
      {
        chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
        p_erase_init.Page = first_page;
        p_erase_init.NbPages = chunk_nb_pages;
        first_page += chunk_nb_pages;
        nb_pages -= chunk_nb_pages;
        if (HAL_FLASHEx_Erase(&p_erase_init, &page_error) != HAL_OK)
        {
          e_ret_status = SFU_ERROR;
          *pFlashStatus = SFU_FLASH_ERR_ERASE;
        }
        SFU_LL_SECU_IWDG_Refresh(); /* calling this function which checks the compiler switch */
      } while (nb_pages > 0U);
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      if (HAL_FLASH_Lock() != HAL_OK)
      {
        e_ret_status = SFU_ERROR;
        *pFlashStatus = SFU_FLASH_ERR_HAL;
      }
    }
    else
    {
      *pFlashStatus = SFU_FLASH_ERR_HAL;
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pFlashStatus: FLASH_StatusTypeDef
  * @param  pDestination: Start address for target location. It has to be 8 bytes aligned.
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: Length of data buffer in bytes. It has to be 8 bytes aligned.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Write(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t  *pDestination,
                                       const uint8_t *pSource, uint32_t Length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t i;
  uint32_t inside_header = 0U;
  uint32_t destination = (uint32_t)pDestination;
  uint32_t source = (uint32_t)pSource;

  /* Check the pointers allocation */
  if ((pFlashStatus == NULL) || (pSource == NULL))
  {
    return SFU_ERROR;
  }

  /* Is destination area inside 1 of the firmware image headers ? */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
    if ((destination >= SlotHeaderAdd[SLOT_ACTIVE_1 + i]) &&
        ((destination + Length) <= (SlotHeaderAdd[SLOT_ACTIVE_1 + i] + SFU_IMG_IMAGE_OFFSET)))
    {
      inside_header = 1U;
    }
  }

  /* Destination area part of 1 of the firmware image headers :
     writing operation should be executed inside secure environment */
  if ((inside_header == 1U) && (Length != 0U))
  {
    /* SE Access */
    SE_StatusTypeDef se_status;
    SE_ErrorStatus se_ret_status = SE_SFU_IMG_Write(&se_status, (uint8_t *)destination, pSource, Length);
    if (se_ret_status == SE_SUCCESS)
    {
      e_ret_status = SFU_SUCCESS;
      *pFlashStatus = SFU_FLASH_SUCCESS;
    }
    else
    {
      e_ret_status = SFU_ERROR;
      *pFlashStatus = SFU_FLASH_ERROR;
    }
  }
  /* Writing operation executed by SBSFU */
  else
  {
    *pFlashStatus = SFU_FLASH_ERROR;

    /* Clear error flags raised during previous operation */
    e_ret_status = SFU_LL_FLASH_INT_Clear_Error();

    if (e_ret_status == SFU_SUCCESS)
    {
      /* Unlock the Flash to enable the flash control register access *************/
      if (HAL_FLASH_Unlock() != HAL_OK)
      {
        *pFlashStatus = SFU_FLASH_ERR_HAL;

      }
      else
      {
        /* DataLength must be a multiple of 64 bit */
        for (i = 0U; (i < Length) && (e_ret_status == SFU_SUCCESS);  i += sizeof(SFU_LL_FLASH_write_t))
        {
          *pFlashStatus = SFU_FLASH_ERROR;

          /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
          be done by word */
          if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, *((uint64_t *)(source + i)))
              == HAL_OK)
          {
            /* Check the written value */
            if (*(uint64_t *)destination != *(uint64_t *)(source + i))
            {
              /* Flash content doesn't match SRAM content */
              *pFlashStatus = SFU_FLASH_ERR_WRITINGCTRL;
              e_ret_status = SFU_ERROR;
            }
            else
            {
              /* Increment FLASH Destination address */
              destination = destination + sizeof(SFU_LL_FLASH_write_t);
              e_ret_status = SFU_SUCCESS;
              *pFlashStatus = SFU_FLASH_SUCCESS;
            }
          }
          else
          {
            /* Error occurred while writing data in Flash memory */
            *pFlashStatus = SFU_FLASH_ERR_WRITING;
            e_ret_status = SFU_ERROR;
          }
        }
        /* Lock the Flash to disable the flash control register access (recommended
        to protect the FLASH memory against possible unwanted operation) */
        if (HAL_FLASH_Lock() != HAL_OK)
        {
          e_ret_status = SFU_ERROR;
          *pFlashStatus = SFU_FLASH_ERR_HAL;
        }
      }
    }
  }
  return e_ret_status;
}

/**
  * @brief  This function reads flash
  * @param  pDestination: Start address for target location
  * @param  pSource: flash address to read
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_ErrorStatus se_ret_status;
  SE_StatusTypeDef se_status;
  uint32_t i;
  uint32_t inside_header = 0U;
  uint32_t source = (uint32_t)pSource;

  /* Is destination area inside 1 of the firmware image headers ? */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
    if ((source >= SlotHeaderAdd[SLOT_ACTIVE_1 + i]) &&
        ((source + Length) <= (SlotHeaderAdd[SLOT_ACTIVE_1 + i] + SFU_IMG_IMAGE_OFFSET)))
    {
      inside_header = 1U;
    }
  }

  /* Destination area part of 1 of the firmware image headers :
     reading operation should be executed inside secure environment */
  if (inside_header == 1U)
  {
    /* SE Access */
    DoubleECC_Error_Counter = 0U;
    se_ret_status = SE_SFU_IMG_Read(&se_status, pDestination, (uint8_t *)source, Length);
    if ((se_ret_status == SE_SUCCESS) && (DoubleECC_Error_Counter == 0U))
    {
      e_ret_status = SFU_SUCCESS;
    }
    DoubleECC_Error_Counter = 0U;
  }
  /* Reading operation executed by SBSFU */
  else
  {
    DoubleECC_Error_Counter = 0U;
    /* Do not use memcpy from lib : ECC error should be checked at each loop */
    for (i = 0; (i < Length) && (DoubleECC_Error_Counter == 0U); i++, pDestination++, pSource++)
    {
      *pDestination = *pSource;
    }
    if (DoubleECC_Error_Counter == 0U)
    {
      e_ret_status = SFU_SUCCESS;
    }
    DoubleECC_Error_Counter = 0U;
  }
  return e_ret_status;
}

/**
  * @brief  This function compare a buffer with a flash area
  * @note   The flash area should not be located inside the secure area
  * @param  pFlash: address of the flash area
  * @param  Pattern1: first 32 bits pattern to be compared
  * @param  Pattern2: second 32 bits pattern to be compared
  * @param  Length: number of bytes to be compared
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length)
{
  uint32_t flash = (uint32_t) pFlash;
  uint32_t i;

  /* Comparison executed by SBSFU ==> flash area could not be located inside secured environment */
  for ( i = 0U; i < Length; i += 4U)
  {
    if ((*(uint32_t *)(flash + i) != Pattern1) &&  (*(uint32_t *)(flash + i) != Pattern2))
    {
      return SFU_ERROR;
    }
  }
  return SFU_SUCCESS;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: flash address
  * @retval The page of a given address
  */
uint32_t SFU_LL_FLASH_INT_GetPage(uint32_t Addr)
{
  uint32_t page;

  page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;

  return page;
}




/**
  * @brief  Clear error flags raised during previous operation
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus SFU_LL_FLASH_INT_Clear_Error(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {

    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      e_ret_status = SFU_SUCCESS;
    }
  }

  return e_ret_status;

}


/**
  * @brief  Get Link Register value (LR)
  * @param  None.
  * @retval LR Register value
  */
__attribute__((always_inline)) __STATIC_INLINE uint32_t get_LR(void)
{
  register uint32_t result;

#if defined ( __ICCARM__ ) || ( __GNUC__ )
  __ASM volatile("MOV %0, LR" : "=r"(result));
#elif defined ( __CC_ARM )
  __ASM volatile("MOV result, __return_address()");
#endif /* ( __ICCARM__ ) || ( __GNUC__ ) */

  return result;
}

/*
   As this handler code relies on stack pointer position to manipulate the PC return value, it is important
   not to use extra registers (on top of scratch registers), because it would change the stack pointer
   position. Then compiler optimizations are customized to ensure that.
*/
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#elif defined(__ARMCC_VERSION)
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("O1")))
#endif /* __ICCARM__ */
/**
  * @brief  NMI Handler present for handling Double ECC NMI interrupt
  * @param  None.
  * @retvat void
  */
void NMI_Handler(void)
{
  uint32_t *p_sp;
  uint32_t lr;
  uint16_t opcode_msb;

  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_ECCD))
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ECCD);

    /* Memorize error to ignore the read value */
    DoubleECC_Error_Counter++;

    lr = get_LR();

    /* Check EXC_RETURN value in LR to know which SP was used prior entering exception */
    if (((lr) & (0xFU)) == 0xDU)
    {
      /* interrupted code was using Process Stack Pointer */
      p_sp = (uint32_t *)__get_PSP();
    }
    else
    {
      /* interrupted code was using Main Stack Pointer */
      p_sp = (uint32_t *)__get_MSP();
    }

    /* Test caller mode T bit from CPSR in stack */
    if ((*(p_sp + 7U) & (1U << xPSR_T_Pos)) != 0U)
    {
      /* Thumb  mode.
         Test PC in stack.
         If bits [15:11] of the halfword being decoded take any of the following values,
         the halfword is the first halfword of a 32-bit instruction: 0b11101, 0b11110, 0b11111.
         Otherwise, the halfword is a 16-bit instruction.
      */
      opcode_msb = (*(uint16_t *)(*(p_sp + 6) & 0xFFFFFFFEU) & 0xF800U);
      if ((opcode_msb == 0xE800U) || (opcode_msb == 0xF000U) || (opcode_msb == 0xF800U))
      {
        /* execute next instruction PC +4  */
        *(p_sp + 6U) += 4U;
      }
      else
      {
        /* execute next instruction PC +2  */
        *(p_sp + 6U) += 2U;
      }
    }
    else
    {
      /* ARM mode execute next instruction PC +4 */
      *(p_sp + 6U) += 4U;
    }
  }
  else
  {
    while (1 == 1)
    {
      SFU_EXCPT_IrqExceptionHandler(SFU_EXCPT_UNKNOWN);
    }
  }
}


/**
  * @brief  HardFault Handler
  * @param  None.
  * @retval None.
  */
void HardFault_Handler(void)
{
  SFU_EXCPT_IrqExceptionHandler(SFU_EXCPT_HARD_FAULT);
}