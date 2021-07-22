/**
  ******************************************************************************
  * @file    sfu_error.h
  * @author  MCD Application Team
  * @brief   This file contains the error handling elements for SB_SFU.
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
  SFU_EXCPT_FWIMG_SWAP_ERR,        /* Problem while swapping the images */
  SFU_EXCPT_FWIMG_ROLLBACK_ERR,    /* Problem during rollback process */
  SFU_EXCPT_FWIMG_ROLLBACK_PREV_ID_ERR,/* Backed-up firmware not identified */
  SFU_EXCPT_BACKUP_COPY_FAILURE_ERR, /* Copy in the backup slot failure */
  SFU_EXCPT_DWL_SLOT_ERASING_ERR,  /* Dwl slot erasing failure */
  SFU_EXCPT_TRAILER_ERASING_ERR,   /* Trailer update failure */
  SFU_EXCPT_TRAILER_UPDATE_ERR,    /* Trailer update failure */
  SFU_EXCPT_FWIMG_MAGIC_ERR,       /* Problem with a FWIMG pattern */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

