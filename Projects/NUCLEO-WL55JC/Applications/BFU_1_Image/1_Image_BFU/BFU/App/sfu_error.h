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
  SFU_EXCPT_WATCHDOG_RESET,
  SFU_EXCPT_MEMORY_FAULT,
  SFU_EXCPT_HARD_FAULT,
  SFU_EXCPT_TAMPERING_FAULT,
  SFU_EXCPT_CHECK_PROTECTIONS_ERR,
  /* Generic errors derived from the FSM state */
  SFU_EXCPT_CHECK_STATUS_ON_RESET_ERR,
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
  SFU_EXCPT_CHECK_NEW_FW_TO_DOWNLOAD_ERR,
  SFU_EXCPT_DOWNLOAD_NEW_USER_FW_ERR,
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
  SFU_EXCPT_VERIFY_USER_FW_STATUS_ERR,
  SFU_EXCPT_DECRYPT_NEW_USER_FW_ERR,
  SFU_EXCPT_VERIFY_USER_FW_SIGNATURE_ERR,
  SFU_EXCPT_EXECUTE_USER_FW_ERR,
  /* Specific errors */
  SFU_EXCPT_LOCK_SE_SERVICES_ERR,
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
  SFU_EXCPT_FW_TOO_BIG,            /* The binary is too big to be installed */
  SFU_EXCPT_COM_ERROR,             /* COM error during the local download of the binary */
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
  SFU_EXCPT_HEADER_AUTH_FAILED,    /* header authentication failure */
  SFU_EXCPT_DECRYPT_FAILURE,       /* Decrypt failure */
  SFU_EXCPT_SIGNATURE_FAILURE,     /* Signature check failure */
  SFU_EXCPT_FLASH_ERROR,           /* Problem with FLASH */
  SFU_EXCPT_FWIMG_MAGIC,           /* Problem with a FWIMG pattern */
  SFU_EXCPT_INCORRECT_VERSION,     /* Anti-rollback check rejected the version to be installed */
  /* Unknown error */
  SFU_EXCPT_UNKNOWN
} SFU_EXCPT_IdTypeDef;   /*!< Errors, Exception, Faults Id */

/* External variables --------------------------------------------------------*/
#ifdef SFU_DEBUG_MODE
#if defined (SFU_BOOT_C)

/**
  *  The following strings associated to the exceptions/errors are used for debugging purpose.
  *  WARNING: The string array must match perfectly with the @ref SFU_EXCPT_IdTypeDef.
  *  And the @ref SFU_EXCPT_IdTypeDef enum must be a sequence starting from 0
  */
char *m_aErrorStrings[] =       {  "No error. Success.",
                                   "Watchdog error.",
                                   "Memory fault.",
                                   "Hard fault.",
                                   "Tampering fault.",
                                   "Check protections error.",
                                   "Check status on reset error.",
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
                                   "Check new user fw to download error.",
                                   "Download new user fw error.",
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
                                   "Verify user fw status error.",
                                   "Decrypt user fw error.",
                                   "Install user fw error.",
                                   "Verify user fw signature.",
                                   "Resume install new user fw error.",
                                   "Execute user fw error.",
                                   "Max. Consecutive Errors reached.",
                                   "SE lock cannot be set.",
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
                                   "Inconsistent FW size.",
                                   "FW too big.",
                                   "YMODEM com failure.",
                                   "File not correctly received.",
                                   "Header authentication failed.",
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
                                   "Decrypt failure.",
                                   "Signature check failure.",
                                   "Incorrect binary format (not encrypted).",
                                   "Flash error.",
                                   "FWIMG pattern issue.",
                                   "Error while swapping the images in slot #0 and slot #1.",
                                   "Firmware version rejected by anti-rollback check.",
                                   "Unknown error."
                                };

#endif /* SFU_BOOT_C */
#endif /* SFU_DEBUG_MODE */

/* Exported macros -----------------------------------------------------------*/
#define IS_SFU_EXCPT(EXCPT)  (SFU_EXCPT_Check_Code((EXCPT)) == SFU_SUCCESS) /*!< Check validity of exception Id */

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_BOOT_SetLastExecError(uint32_t uLastExecError);
void SFU_BOOT_StateExceptionHandler(SFU_BOOT_StateMachineTypeDef eStateMachineState);
void SFU_BOOT_IrqExceptionHandler(SFU_EXCPT_IdTypeDef eExceptionId);
SFU_ErrorStatus SFU_EXCPT_Check_Code(SFU_EXCPT_IdTypeDef eExceptionId);
void SFU_EXCPT_Security_Error(void);

SFU_ErrorStatus SFU_EXCPT_Init(void);
SFU_ErrorStatus SFU_EXCPT_DeInit(void);

void SFU_EXCPT_ResetExceptionHandler(SFU_EXCPT_IdTypeDef eExceptionId);

#ifdef __cplusplus
}
#endif

#endif /* SFU_ERROR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

