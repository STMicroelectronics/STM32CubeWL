/**
  ******************************************************************************
  * @file    kms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module Low Level access to Flash, CRC...
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

#ifndef KMS_LOW_LEVEL_H
#define KMS_LOW_LEVEL_H

/* Includes ------------------------------------------------------------------*/
#include "sfu_fwimg_regions.h"
#include "stm32wlxx_hal.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @addtogroup KMS_LL_Exported_Constants Exported Constants
  * @{
  */

#define KMS_LL_ERROR_MEM_ALLOC_FAILURE    (0x00000001UL)    /*!< Memory allocation failure error */
#define KMS_LL_ERROR_MEM_FREE_CANARY      (0x00000002UL)    /*!< Memory free detected altered canaries error */
#define KMS_LL_ERROR_MEM_FREE_NULL_PTR    (0x00000003UL)    /*!< Memory free detected a NULL pointer error */
#define KMS_LL_ERROR_MEM_FREE_UNKNOWN     (0x00000004UL)    /*!< Memory free detected an unknown pointer error */

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_LL_Exported_Functions Exported Functions
  * @{
  */
CK_RV KMS_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length);
uint32_t KMS_LL_FLASH_GetBlobDownloadAddress(void);

#ifdef KMS_SE_CHECK_PARAMS
void KMS_LL_IsBufferInSecureEnclave(void *pBuffer, uint32_t ulSize);
#endif /* KMS_SE_CHECK_PARAMS */

void KMS_LL_ReportError(uint32_t error);

void KMS_LL_ReportMemInit(void);
void KMS_LL_ReportMemAlloc(uint32_t size, void *pmem);
void KMS_LL_ReportMemFree(void *pmem);

/**
  * @brief  Get Link Register value (LR)
  * @param  None.
  * @retval LR Register value
  */
__attribute__((always_inline)) __STATIC_INLINE uint32_t KMS_LL_GetLR(void)
{
  register uint32_t result;

#if defined ( __ICCARM__ ) || ( __GNUC__ )
  __ASM volatile("MOV %0, LR" : "=r"(result));
#elif defined ( __CC_ARM )
  __ASM volatile("MOV result, __return_address()");
#endif /* ( __ICCARM__ ) || ( __GNUC__ ) */

  return result;
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

#endif /* KMS_LOW_LEVEL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
