/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sys.h
  * @author  MCD Application Team
  * @brief   API for CM4 application to handle the SYSTEM MBMUX channel
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MBMUXIF_SYS_CM4_H__
#define __MBMUXIF_SYS_CM4_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mbmux.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* -- FreeRTOS tasks configuration -- */
/*Mailbox Lora*/
#define CFG_MB_LORA_PROCESS_NAME                   "MB_LORA_PROCESS"
#define CFG_MB_LORA_PROCESS_ATTR_BITS              (0)
#define CFG_MB_LORA_PROCESS_CB_MEM                 (0)
#define CFG_MB_LORA_PROCESS_CB_SIZE                (0)
#define CFG_MB_LORA_PROCESS_STACK_MEM              (0)
#define CFG_MB_LORA_PROCESS_PRIORITY               osPriorityNone
#define CFG_MB_LORA_PROCESS_STACK_SIZE             (128 * 10)

/*Mailbox Radio*/
#define CFG_MB_RADIO_PROCESS_NAME                  "MB_RADIO_PROCESS"
#define CFG_MB_RADIO_PROCESS_ATTR_BITS             (0)
#define CFG_MB_RADIO_PROCESS_CB_MEM                (0)
#define CFG_MB_RADIO_PROCESS_CB_SIZE               (0)
#define CFG_MB_RADIO_PROCESS_STACK_MEM             (0)
#define CFG_MB_RADIO_PROCESS_PRIORITY              osPriorityNone
#define CFG_MB_RADIO_PROCESS_STACK_SIZE            (128 * 10)

/*Mailbox Sys*/
#define CFG_MB_SYS_PROCESS_NAME                    "MB_SYS_PROCESS"
#define CFG_MB_SYS_PROCESS_ATTR_BITS               (0)
#define CFG_MB_SYS_PROCESS_CB_MEM                  (0)
#define CFG_MB_SYS_PROCESS_CB_SIZE                 (0)
#define CFG_MB_SYS_PROCESS_STACK_MEM               (0)
#define CFG_MB_SYS_PROCESS_PRIORITY                osPriorityNone
#define CFG_MB_SYS_PROCESS_STACK_SIZE              (128 * 10)

/*Mailbox Kms*/
#define CFG_MB_KMS_PROCESS_NAME                    "MB_KMS_PROCESS"
#define CFG_MB_KMS_PROCESS_ATTR_BITS               (0)
#define CFG_MB_KMS_PROCESS_CB_MEM                  (0)
#define CFG_MB_KMS_PROCESS_CB_SIZE                 (0)
#define CFG_MB_KMS_PROCESS_STACK_MEM               (0)
#define CFG_MB_KMS_PROCESS_PRIORITY                osPriorityNone
#define CFG_MB_KMS_PROCESS_STACK_SIZE              (128 * 10)

/*Send*/
#define CFG_APP_LORA_PROCESS_NAME                  "LORA_SEND_PROCESS"
#define CFG_APP_LORA_PROCESS_ATTR_BITS             (0)
#define CFG_APP_LORA_PROCESS_CB_MEM                (0)
#define CFG_APP_LORA_PROCESS_CB_SIZE               (0)
#define CFG_APP_LORA_PROCESS_STACK_MEM             (0)
#define CFG_APP_LORA_PROCESS_PRIORITY              osPriorityNone
#define CFG_APP_LORA_PROCESS_STACK_SIZE            (128 * 10)

/*Store Context*/
#define CFG_APP_LORA_STORE_CONTEXT_NAME            "LORA_STORE_CONTEXT"
#define CFG_APP_LORA_STORE_CONTEXT_ATTR_BITS       (0)
#define CFG_APP_LORA_STORE_CONTEXT_CB_MEM          (0)
#define CFG_APP_LORA_STORE_CONTEXT_CB_SIZE         (0)
#define CFG_APP_LORA_STORE_CONTEXT_STACK_MEM       (0)
#define CFG_APP_LORA_STORE_CONTEXT_PRIORITY        osPriorityNone
#define CFG_APP_LORA_STORE_CONTEXT_STACK_SIZE      (128 * 10)

/*Stop Join*/
#define CFG_APP_LORA_STOP_JOIN_NAME                "LORA_STOP_JOIN"
#define CFG_APP_LORA_STOP_JOIN_ATTR_BITS           (0)
#define CFG_APP_LORA_STOP_JOIN_CB_MEM              (0)
#define CFG_APP_LORA_STOP_JOIN_CB_SIZE             (0)
#define CFG_APP_LORA_STOP_JOIN_STACK_MEM           (0)
#define CFG_APP_LORA_STOP_JOIN_PRIORITY            osPriorityNone
#define CFG_APP_LORA_STOP_JOIN_STACK_SIZE          (128 * 10)

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief Inits the MBMUX and registers SYS channel to the mailbox and SYS task to the sequencer
  * @retval   0: OK;
             -1: no more ipcc channel available;
             -2: feature not provided by CM0PLUS;
             -3: callback error on CM0PLUS
  */
int8_t MBMUXIF_SystemInit(void);

/**
  * @brief   gives back the pointer to the com buffer associated to System feature Cmd
  * @param   SystemPrioFeat ID of the feature
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Registers to the mailbox a SYSTEM feature channel used on higher PRIORITY
  * @param   SystemPrioFeat ID of the feature
  * @retval  0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS
  */
int8_t MBMUXIF_SystemPrio_Add(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Set synchro flag between the two CPUs
  * @param   flag 0xFFFF: hold Cm0 before it runs its MBMUX_Init,
  *               0x5555: Cm0 allowed to run its MBMUX_Init
  *               0xAAAA: Cm0 has completed initialization
  *               0x9999: RTC PRIO CHANNEL has been registered
  */
void MBMUXIF_SetCpusSynchroFlag(uint16_t flag);

/**
  * @brief gives green light to Cm0 to Initialized MBMUX on its side
  */
void MBMUXIF_WaitCm0MbmuxIsInitialized(void);

/**
  * @brief   To allow SystemCmd to use the sequencer (hence entering in low power)
  * @note    For compatibility with RTOS, the sequencer (scheduler) is not used until the main loop process is reached (after Init). The application can call this API at the end of the Init
  */
void MBMUXIF_SystemAllowSequencerForSysCmd(void);

/**
  * @brief   Sends a System-Cmd via Ipcc and Wait for the response
  * @param   SystemPrioFeat ID of the feature
  */
void MBMUXIF_SystemSendCmd(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Sends a System-Ack  via Ipcc without waiting for the ack
  * @param   SystemPrioFeat ID of the feature
  */
void MBMUXIF_SystemSendAck(FEAT_INFO_IdTypeDef SystemPrioFeat);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Exported services --------------------------------------------------------*/
/**
  * @brief   Gets the pointer to CM0PLUS capabilities info table
  * @retval  pointer to CM0PLUS capabilities info table
  */
FEAT_INFO_List_t *MBMUXIF_SystemSendCm0plusInfoListReq(void);

/**
  * @brief   returns the pointer to the Capability Info table of a given feature
  * @param   e_featID ID of the feature from which retrieve the ptr
  * @retval  LoRaMacInfoTable pointer
  */
FEAT_INFO_Param_t *MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_IdTypeDef e_featID);

/**
  * @brief   Asks CM0PLUS to register its Cmd and Ack callbacks relative to the requested feature
  * @param   e_featID ID of the feature (Lora, Sigfox, etc).
  * @retval  0 ok, else error code
  */
int8_t MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_IdTypeDef e_featID);

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_SYS_CM4_H__ */
