/**
  ******************************************************************************
  * @file    sfu_error.h
  * @author  MCD Application Team
  * @brief   This file contains the error handling elements for SB_SFU.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SFU_ERROR_H
#define SFU_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  * This enum must start from 0.
  * When adding a new error in the enum below please also:
  *  1. update IS_SFU_EXCPT
  *  2. update m_aErrorStrings
  */
typedef enum
{
  SFU_EXCPT_NONE = 0UL,
  /* System errors */
  SFU_EXCPT_MEMORY_FAULT,
  SFU_EXCPT_HARD_FAULT,
  SFU_EXCPT_TAMPERING_FAULT,
#ifdef SFU_GTZC_PROTECT_ENABLE
  SFU_EXCPT_GTZC_TZIC_FAULT,
#endif /* SFU_GTZC_PROTECT_ENABLE */
  /* Specific errors */
  SFU_EXCPT_LOCK_SE_SERVICES_ERR,
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
  SFU_EXCPT_FW_TOO_BIG,            /* The binary is too big to be installed */
  SFU_EXCPT_COM_ERR,               /* COM error during the local download of the binary */
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
  SFU_EXCPT_HEADER_AUTH_ERR,       /* header authentication failure */
  SFU_EXCPT_DECRYPT_ERR,           /* Decrypt failure */
  SFU_EXCPT_SIGNATURE_ERR,         /* Signature check failure */
  SFU_EXCPT_FLASH_ERR,             /* Problem with FLASH */
  SFU_EXCPT_FLASH_CFG_ERR,         /* Flash configuration issue (external flash) */
  SFU_EXCPT_HEADER_ERASING_ERR,    /* Header erasing failure */
  SFU_EXCPT_HEADER_VALIDATION_ERR, /* Header validation failure */
  SFU_EXCPT_ADDITIONAL_CODE_ERR,   /* Additional code detected beyond the firmware image (malicious SW) */
  SFU_EXCPT_VERSION_ERR,           /* Anti-rollback check rejected the version to be installed */
  /* Unknown error */
  SFU_EXCPT_UNKNOWN                /* To be kept as last error */
} SFU_EXCPT_IdTypeDef;   /*!< Errors, Exception, Faults Id */

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void SFU_EXCPT_SetError(SFU_EXCPT_IdTypeDef eExceptionId);
void SFU_EXCPT_IrqExceptionHandler(SFU_EXCPT_IdTypeDef eExceptionId);
void SFU_EXCPT_Security_Error(void);

SFU_ErrorStatus SFU_EXCPT_Init(void);
SFU_ErrorStatus SFU_EXCPT_DeInit(void);

void SFU_EXCPT_ResetExceptionHandler(SFU_EXCPT_IdTypeDef eExceptionId);

#ifdef __cplusplus
}
#endif

#endif /* SFU_ERROR_H */
