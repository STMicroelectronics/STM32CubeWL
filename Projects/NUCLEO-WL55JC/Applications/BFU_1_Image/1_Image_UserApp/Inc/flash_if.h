/**
  ******************************************************************************
  * @file    flash_if.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for FLASH Interface functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_IF_H
#define FLASH_IF_H

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define FLASH_IF_MIN_WRITE_LEN (8U)  /* Flash programming by 64 bits */
/* Exported functions ------------------------------------------------------- */

HAL_StatusTypeDef FLASH_If_Init(void);
HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_If_Read(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_If_Erase_Size(void *pStart, uint32_t uLength);
HAL_StatusTypeDef FLASH_INT_Init(void);
HAL_StatusTypeDef FLASH_INT_If_Write(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_INT_If_Read(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_INT_If_Erase_Size(void *pStart, uint32_t uLength);
HAL_StatusTypeDef FLASH_EXT_Init(void);
HAL_StatusTypeDef FLASH_EXT_If_Write(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_EXT_If_Read(void *pDestination, const void *pSource, uint32_t uLength);
HAL_StatusTypeDef FLASH_EXT_If_Erase_Size(void *pStart, uint32_t uLength);


/**
  * @}
  */

/**
  * @}
  */

#endif  /* FLASH_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
