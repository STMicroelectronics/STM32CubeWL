/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash_if.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for FLASH Interface functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H__
#define __FLASH_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief Flash status enumeration
  */
enum
{
  FLASH_PARAM_ERROR = -5,
  FLASH_LOCK_ERROR = -4,
  FLASH_WRITE_ERROR = -3,
  FLASH_ERASE_ERROR  = -2,
  FLASH_ERROR  = -1,
  FLASH_OK     = 0,
  FLASH_BUSY   = 1
};

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#define PAGE(__ADDRESS__) (uint32_t)((((__ADDRESS__) - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE) /*!< Get page index from page address */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  This function writes a user flash area (read/modify/write)
  * @param  address: ptr to user flash area
  * @param  data: ptr to data to be written
  * @param  size: number of 32b.
  * @param  dataTempPage: ptr used to copy already written page in ram
  * @return Flash status.
  */
int32_t FLASH_IF_Write(uint32_t  address, uint8_t *data, uint32_t size, uint8_t *dataTempPage);

/**
  * Writes a 64-bit word in flash at a specific address.
  * @param address (in bytes) must be a multiple of 8.
  * @param data data to write
  * @returns FLASH_OK, FLASH_BUSY, FLASH_ERASE_ERROR
  */
int32_t FLASH_IF_Write64(uint32_t address, uint64_t data);

/**
  * Erases 'n' flash pages from page number 'page' to page number
  * 'page + n - 1'.
  * - If 'interrupt' is set to 0, the erasing is performed in polling mode.
  * - If 'interrupt' is set to 1, the erasing is performed under FLASH
  *   interrupt: the function returns immediately and the user is informed of
  *   the end of erasing procedure by a call to the following function that
  *   must be implemented by the user: void HWCB_FLASH_EndOfCleanup( void );
  *   this call-back function is called under FLASH IRQ handler.
  * @param page memory start page number
  * @param n number of page
  * @param interrupt choice polling/interrupt
  * @returns FLASH_OK, FLASH_BUSY, FLASH_ERASE_ERROR
  */
int32_t FLASH_IF_EraseByPages(uint32_t page, uint16_t n, int32_t interrupt);

/**
  * Callback
  */
void HWCB_FLASH_EndOfCleanup(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
