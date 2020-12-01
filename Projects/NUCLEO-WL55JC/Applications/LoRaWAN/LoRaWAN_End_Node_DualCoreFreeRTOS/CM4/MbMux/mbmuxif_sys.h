/**
  ******************************************************************************
  * @file    mbmuxif_sys.h
  * @author  MCD Application Team
  * @brief   API for CM4 applic to handle the SYSTEM MBMUX channel
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
#define CFG_MB_LORA_PROCESS_NAME                  "MB_LORA_PROCESS"
#define CFG_MB_LORA_PROCESS_ATTR_BITS             (0)
#define CFG_MB_LORA_PROCESS_CB_MEM                (0)
#define CFG_MB_LORA_PROCESS_CB_SIZE               (0)
#define CFG_MB_LORA_PROCESS_STACK_MEM             (0)
#define CFG_MB_LORA_PROCESS_PRIORITY              osPriorityNone
#define CFG_MB_LORA_PROCESS_STACk_SIZE            (128 * 10)

/*Mailbox Radio*/
#define CFG_MB_RADIO_PROCESS_NAME                  "MB_RADIO_PROCESS"
#define CFG_MB_RADIO_PROCESS_ATTR_BITS             (0)
#define CFG_MB_RADIO_PROCESS_CB_MEM                (0)
#define CFG_MB_RADIO_PROCESS_CB_SIZE               (0)
#define CFG_MB_RADIO_PROCESS_STACK_MEM             (0)
#define CFG_MB_RADIO_PROCESS_PRIORITY              osPriorityNone
#define CFG_MB_RADIO_PROCESS_STACk_SIZE            (128 * 10)

/*Mailbox Sys*/
#define CFG_MB_SYS_PROCESS_NAME                  "MB_SYS_PROCESS"
#define CFG_MB_SYS_PROCESS_ATTR_BITS             (0)
#define CFG_MB_SYS_PROCESS_CB_MEM                (0)
#define CFG_MB_SYS_PROCESS_CB_SIZE               (0)
#define CFG_MB_SYS_PROCESS_STACK_MEM             (0)
#define CFG_MB_SYS_PROCESS_PRIORITY              osPriorityNone
#define CFG_MB_SYS_PROCESS_STACk_SIZE            (128 * 10)

/*Mailbox Kms*/
#define CFG_MB_KMS_PROCESS_NAME                  "MB_KMS_PROCESS"
#define CFG_MB_KMS_PROCESS_ATTR_BITS             (0)
#define CFG_MB_KMS_PROCESS_CB_MEM                (0)
#define CFG_MB_KMS_PROCESS_CB_SIZE               (0)
#define CFG_MB_KMS_PROCESS_STACK_MEM             (0)
#define CFG_MB_KMS_PROCESS_PRIORITY              osPriorityNone
#define CFG_MB_KMS_PROCESS_STACk_SIZE            (128 * 10)

/*Send*/
#define CFG_APP_LORA_PROCESS_NAME                  "LORA_SEND_PROCESS"
#define CFG_APP_LORA_PROCESS_ATTR_BITS             (0)
#define CFG_APP_LORA_PROCESS_CB_MEM                (0)
#define CFG_APP_LORA_PROCESS_CB_SIZE               (0)
#define CFG_APP_LORA_PROCESS_STACK_MEM             (0)
#define CFG_APP_LORA_PROCESS_PRIORITY              osPriorityNone
#define CFG_APP_LORA_PROCESS_STACk_SIZE            (128 * 10)

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
int8_t MBMUXIF_SystemInit(void);
MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat);
int8_t MBMUXIF_SystemPrio_Add(FEAT_INFO_IdTypeDef SystemPrioFeat);
void MBMUXIF_SetCpusSynchroFlag(uint16_t flag);
void MBMUXIF_WaitCm0MbmuxIsInitialised(void);
void MBMUXIF_SystemAllowSequencerForSysCmd(void);
void MBMUXIF_SystemSendCmd(FEAT_INFO_IdTypeDef SystemPrioFeat);
void MBMUXIF_SystemSendAck(FEAT_INFO_IdTypeDef SystemPrioFeat);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Exported services --------------------------------------------------------*/
FEAT_INFO_List_t *MBMUXIF_SystemSendCm0plusInfoListReq(void);
FEAT_INFO_Param_t *MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_IdTypeDef e_featID);
int8_t MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_IdTypeDef e_featID);

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_SYS_CM4_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
