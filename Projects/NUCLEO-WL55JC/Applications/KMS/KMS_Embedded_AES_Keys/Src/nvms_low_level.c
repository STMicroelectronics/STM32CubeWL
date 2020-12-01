/**
  ******************************************************************************
  * @file    nvms_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module NVM Low Level access to physical storage (Flash...)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nvms_low_level.h"
/* USER CODE BEGIN NVMS_LOW_LEVEL_Includes */
/* USER CODE END NVMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_NVMLL NVM Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Types */
/* USER CODE END NVMS_LOW_LEVEL_Private_Types */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Defines */
/* USER CODE END NVMS_LOW_LEVEL_Private_Defines */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Macros */
/* USER CODE END NVMS_LOW_LEVEL_Private_Macros */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Variables */
/* USER CODE END NVMS_LOW_LEVEL_Private_Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Function_Prototypes */
/* USER CODE END NVMS_LOW_LEVEL_Private_Function_Prototypes */

/* Private function ----------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Functions */
/* USER CODE END NVMS_LOW_LEVEL_Private_Functions */

/* Exported functions --------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Private_Exported_Functions */
/* USER CODE END NVMS_LOW_LEVEL_Private_Exported_Functions */

/** @addtogroup KMS_NVMLL_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief   Flash low level driver initialization.
  */
void NVMS_LL_Init(void)
{
  /* USER CODE BEGIN NVMS_LL_Init */
  /* USER CODE END NVMS_LL_Init */
}

/**
  * @brief   Determines if a flash block is in erased state.
  *
  * @param   block         the block identifier
  * @return                  The block state.
  * @retval false            if the block is not in erased state.
  * @retval true             if the block is in erased state.
  */

/* Template version of the function */
bool NVMS_LL_IsBlockErased(nvms_block_t block)
{
  /* USER CODE BEGIN NVMS_LL_IsBlockErased */

  return false;
  /* USER CODE END NVMS_LL_IsBlockErased */
}

/**
  * @brief   Erases a block.
  * @note    The erase operation is verified internally.
  *
  * @param   block         the block identifier
  * @return                  The operation status.
  * @retval false            if the operation is successful.
  * @retval true             if the erase operation failed.
  */
/* Template version of the function */
bool NVMS_LL_BlockErase(nvms_block_t block)
{
  /* USER CODE BEGIN NVMS_LL_BlockErase */

  return true;
  /* USER CODE END NVMS_LL_BlockErase */
}

/**
  * @brief   Writes data.
  * @note    The write operation is verified internally.
  * @note    If the write operation partially writes flash pages then the
  *          unwritten bytes are left to the filler value.
  *
  * @param   source        pointer to the data to be written
  * @param   destination   pointer to the flash position
  * @param   size          size of data
  * @return                  The operation status.
  * @retval false            if the operation is successful.
  * @retval true             if the write operation failed.
  */
/* Template version of the function */
bool NVMS_LL_Write(const uint8_t *source, uint8_t *destination, size_t size)
{
  /* USER CODE BEGIN NVMS_LL_Write */
  return true;
  /* USER CODE END NVMS_LL_Write */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

