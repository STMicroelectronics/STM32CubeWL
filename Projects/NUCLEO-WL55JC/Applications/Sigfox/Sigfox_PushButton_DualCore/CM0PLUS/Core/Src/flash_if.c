/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  MCD Application Team
  * @brief   This file contains the FLASH driver
  *****************************************************************************
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
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "stm32wlxx.h"
#include "flash_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * @brief Flash status enumeration
  */
enum
{
  FLASH_ERROR  = -1,
  FLASH_OK     = 0,
  FLASH_BUSY   = 1
};
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t HW_FLASH_Write(uint32_t address,
                       uint64_t data)
{
  /* USER CODE BEGIN HW_FLASH_Write_1 */

  /* USER CODE END HW_FLASH_Write_1 */
  HAL_StatusTypeDef hal_status = HAL_ERROR;

  while (*(uint64_t *)address != data)
  {
    while (LL_FLASH_IsActiveFlag_OperationSuspended());

    hal_status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
  }

  return ((hal_status == HAL_OK) ? FLASH_OK :
          ((hal_status == HAL_BUSY) ? FLASH_BUSY : FLASH_ERROR));
  /* USER CODE BEGIN HW_FLASH_Write_2 */

  /* USER CODE END HW_FLASH_Write_2 */
}

int32_t HW_FLASH_Erase(uint32_t page, uint16_t n, int32_t interrupt)
{
  /* USER CODE BEGIN HW_FLASH_Erase_1 */

  /* USER CODE END HW_FLASH_Erase_1 */
  HAL_StatusTypeDef hal_status;
  FLASH_EraseInitTypeDef erase_str;
  uint32_t page_error;

  erase_str.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_str.Page      = page;
  erase_str.NbPages   = n;

  /* Erase the Page */
  if (interrupt)
  {
    hal_status = HAL_FLASHEx_Erase_IT(&erase_str);
  }
  else
  {
    hal_status = HAL_FLASHEx_Erase(&erase_str, &page_error);
  }

  return ((hal_status == HAL_OK) ? FLASH_OK :
          ((hal_status == HAL_BUSY) ? FLASH_BUSY : FLASH_ERROR));
  /* USER CODE BEGIN HW_FLASH_Erase_2 */

  /* USER CODE END HW_FLASH_Erase_2 */
}

uint32_t HW_FLASH_OB_GetIPCCBufferAddr(void)
{
  /* USER CODE BEGIN HW_FLASH_OB_GetIPCCBufferAddr_1 */

  /* USER CODE END HW_FLASH_OB_GetIPCCBufferAddr_1 */
  return READ_BIT(FLASH->IPCCBR, FLASH_IPCCBR_IPCCDBA);
  /* USER CODE BEGIN HW_FLASH_OB_GetIPCCBufferAddr_2 */

  /* USER CODE END HW_FLASH_OB_GetIPCCBufferAddr_2 */
}

uint32_t HW_FLASH_OB_GetSFSA(void)
{
  /* USER CODE BEGIN HW_FLASH_OB_GetSFSA_1 */

  /* USER CODE END HW_FLASH_OB_GetSFSA_1 */
  return (READ_BIT(FLASH->SFR, FLASH_SFR_SFSA) >> FLASH_SFR_SFSA_Pos);
  /* USER CODE BEGIN HW_FLASH_OB_GetSFSA_2 */

  /* USER CODE END HW_FLASH_OB_GetSFSA_2 */
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_1 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_1 */
  /* Call CleanUp callback when all requested pages have been erased */
  if (ReturnValue == 0xFFFFFFFFUL)
  {
    HWCB_FLASH_EndOfCleanup();
  }
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_2 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_2 */
}

__WEAK void HWCB_FLASH_EndOfCleanup(void)
{
  /* USER CODE BEGIN HWCB_FLASH_EndOfCleanup_1 */

  /* USER CODE END HWCB_FLASH_EndOfCleanup_1 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE***/
