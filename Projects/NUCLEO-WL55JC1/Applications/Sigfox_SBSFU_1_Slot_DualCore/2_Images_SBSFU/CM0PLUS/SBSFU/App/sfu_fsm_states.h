/**
  ******************************************************************************
  * @file    sfu_fsm_states.h
  * @author  MCD Application Team
  * @brief   This file defines the SB_SFU Finite State Machine states.
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
#ifndef SFU_FSM_STATES_H
#define SFU_FSM_STATES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_sfu.h"

/* Exported types ------------------------------------------------------------*/
/**
  * FSM states: this enum must start from 0.
  */
typedef enum
{
  SFU_STATE_CHECK_STATUS_ON_RESET = 0U, /*!< checks the reset cause and decide how to proceed     */
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) || (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
  SFU_STATE_CHECK_NEW_FW_TO_DOWNLOAD,   /*!< SFU checks if a local download must be handled       */
  SFU_STATE_DOWNLOAD_NEW_USER_FW,       /*!< downloads the encrypted firmware to be installed     */
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) || (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER) */
#ifdef KMS_ENABLED
  SFU_STATE_CHECK_KMS_BLOB_TO_INSTALL, /*!< SFU checks if a kms blob download must be handled    */
  SFU_STATE_INSTALL_KMS_BLOB,          /* !< download the new kms_blob */
#endif /* KMS_ENABLED */
  SFU_STATE_VERIFY_USER_FW_STATUS,      /*!< checks the internal FLASH state to derive a status   */
  SFU_STATE_VERIFY_USER_FW_SIGNATURE,   /*!< checks the validity of the active firmware           */
  SFU_STATE_EXECUTE_USER_FW,            /*!< launch the user application                          */
  SFU_STATE_HANDLE_CRITICAL_FAILURE,    /*!< deal with the critical failures                      */
  SFU_STATE_REBOOT_STATE_MACHINE        /*!< a reboot is forced                                   */
} SFU_BOOT_StateMachineTypeDef; /*!< SFU BOOT State Machine States */

/* External variables --------------------------------------------------------*/
/**
  *  The following strings associated to the SM state are used for debugging purpose.
  *  WARNING: The string array must match perfectly with the SFU_BOOT_StateMachineTypeDef defined above.
  *           And the above enum must be a sequence starting from 0
  */
#ifdef SFU_DEBUG_MODE
#if defined(SFU_VERBOSE_DEBUG_MODE) && defined (SFU_BOOT_C)
char *m_aStateMachineStrings[] = { "Checking Status on Reset.",
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
                                   "Checking if new Fw Image available to download.",
                                   "Downloading new Fw Image.",
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
                                   "Verifying Fw Image status.",
                                   "Verifying Fw Image signature.",
                                   "Executing Fw Image.",
                                   "Handling a critical failure.",
                                   "Rebooting the State Machine"
                                 };
#endif /* SFU_VERBOSE_DEBUG_MODE && SFU_BOOT_C */

#endif /* SFU_DEBUG_MODE */

/* Exported macros -----------------------------------------------------------*/
/************ SFU SM States****************************************************/
#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)
#define IS_SFU_SM_STATE(STATE)  (((STATE) == (uint8_t)SFU_STATE_CHECK_STATUS_ON_RESET)    || \
                                 ((STATE) == (uint8_t)SFU_STATE_CHECK_NEW_FW_TO_DOWNLOAD) || \
                                 ((STATE) == (uint8_t)SFU_STATE_DOWNLOAD_NEW_USER_FW)     || \
                                 ((STATE) == (uint8_t)SFU_STATE_VERIFY_USER_FW_STATUS)    || \
                                 ((STATE) == (uint8_t)SFU_STATE_VERIFY_USER_FW_SIGNATURE) || \
                                 ((STATE) == (uint8_t)SFU_STATE_EXECUTE_USER_FW)          || \
                                 ((STATE) == (uint8_t)SFU_STATE_HANDLE_CRITICAL_FAILURE)  || \
                                 ((STATE) == (uint8_t)SFU_STATE_REBOOT_STATE_MACHINE))   /*!< SFU SM States*/
#else
#define IS_SFU_SM_STATE(STATE)  (((STATE) == (uint8_t)SFU_STATE_CHECK_STATUS_ON_RESET)    || \
                                 ((STATE) == (uint8_t)SFU_STATE_VERIFY_USER_FW_STATUS)    || \
                                 ((STATE) == (uint8_t)SFU_STATE_VERIFY_USER_FW_SIGNATURE) || \
                                 ((STATE) == (uint8_t)SFU_STATE_EXECUTE_USER_FW)          || \
                                 ((STATE) == (uint8_t)SFU_STATE_HANDLE_CRITICAL_FAILURE)  || \
                                 ((STATE) == (uint8_t)SFU_STATE_REBOOT_STATE_MACHINE))   /*!< SFU SM States*/
#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */

#ifdef __cplusplus
}
#endif

#endif /* SFU_FSM_STATES_H */
