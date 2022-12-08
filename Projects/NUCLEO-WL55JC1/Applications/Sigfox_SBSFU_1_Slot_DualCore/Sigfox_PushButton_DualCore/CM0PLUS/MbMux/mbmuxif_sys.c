/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sys.c
  * @author  MCD Application Team
  * @brief   allows CM0 application to handle the SYSTEM MBMUX channel
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "mbmuxif_sys.h"
#include "stm32_seq.h"
#include "sys_app.h"
#include "msg_id.h"
#include "mbmux.h"
#include "mbmuxif_trace.h"
#include "mbmuxif_sigfox.h"
#include "mbmuxif_radio.h"
#include "features_info.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static MBMUX_ComTable_t *pMb_RefTable;  /* SRAM1_BASE + IPCCDBA<<4*/
static MBMUX_ComParam_t *SystemComObj;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   SYSTEM acknowledge callbacks: set event to release waiting task
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemAckRcvCb(void *ComObj);

/**
  * @brief   SYSTEM command callbacks: schedules a task in order to quit the ISR
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemCmdRcvCb(void *ComObj);

/**
  * @brief   SYSTEM task to process the command
  */
static void MBMUXIF_TaskSystemCmdRcv(void);

/**
  * @brief   SYSTEM PRIO_A acknowledge callbacks:
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioAAckRcvCb(void *ComObj);

/**
  * @brief   SYSTEM PRIO_A command callbacks:
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioACmdRcvCb(void *ComObj);

/**
  * @brief   SYSTEM PRIO_B acknowledge callbacks:
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioBAckRcvCb(void *ComObj);

/**
  * @brief   SYSTEM PRIO_B command callbacks:
  * @param   ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioBCmdRcvCb(void *ComObj);

/**
  * @brief   Retrieve the feature that needs MbMux registration and call correspondent Init
  * @param   pBuf pointer to parameter buffer of the command
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  */
static int8_t MBMUXIF_FeatureCm0plusRegistrationCmd(uint32_t *pBuf);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_SystemInit(void)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_SystemInit_1 */

  /* USER CODE END MBMUXIF_SystemInit_1 */

  /*Read FLASH IPCC data buffer base address based on (IPCCDBA << 4) + SRAM1_BASE*/
  FLASH_OBProgramInitTypeDef OptionsBytesStruct;

  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
  pMb_RefTable = (MBMUX_ComTable_t *) OptionsBytesStruct.IPCCdataBufAddr;

  /* prevents Cm0 to go further, during boot after PWR_CR4_C2BOOT is set by Cm4*/
  /* it allows Cm0 debugging step by step the MBMUXIF_SystemInit */
  while (pMb_RefTable->SynchronizeCpusAtBoot == CPUS_BOOT_SYNC_PREVENT_CPU2_TO_START);

  MBMUX_Init(pMb_RefTable);

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemAckRcvCb);
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_ID, MBMUX_CMD_RESP, MBMUXIF_IsrSystemCmdRcvCb);
  }
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbSystemCmdRcv), UTIL_SEQ_RFU, MBMUXIF_TaskSystemCmdRcv);
    ret = 0;
  }

  if (ret >= 0)
  {
    /* Notify Cm4 that MBMUXIF_System and RTC are initialized on Cm0 side */
    /* as alternative MBMUXIF_SystemSendNotif() can be used as far Cm4 treats it on Irq */
    while (pMb_RefTable->SynchronizeCpusAtBoot != CPUS_BOOT_SYNC_ALLOW_CPU2_TO_START);
    pMb_RefTable->SynchronizeCpusAtBoot = CPUS_BOOT_SYNC_CPU2_INIT_COMPLETED;
  }

  /* USER CODE BEGIN MBMUXIF_SystemInit_Last */

  /* USER CODE END MBMUXIF_SystemInit_Last */
  return ret;
}

uint16_t MBMUXIF_GetCpusSynchroFlag(void)
{
  /* USER CODE BEGIN MBMUXIF_GetCpusSynchroFlag */

  /* USER CODE END MBMUXIF_GetCpusSynchroFlag */
  return pMb_RefTable->SynchronizeCpusAtBoot;
}

MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureNotifComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetSystemFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(SystemPrioFeat, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetSystemFeatureNotifComPtr_Last */
  return com_param_ptr;
}

void MBMUXIF_SystemSendNotif(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_1 */

  /* USER CODE END MBMUXIF_SystemSendNotif_1 */
  if (MBMUX_NotificationSnd(SystemPrioFeat) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbSystemAckRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_Last */

  /* USER CODE END MBMUXIF_SystemSendNotif_Last */
}

void MBMUXIF_SystemSendNotif_NoWait(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_NoWait_1 */

  /* USER CODE END MBMUXIF_SystemSendNotif_NoWait_1 */
  if (MBMUX_NotificationSnd(SystemPrioFeat) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_NoWait_Last */

  /* USER CODE END MBMUXIF_SystemSendNotif_NoWait_Last */
}

void MBMUXIF_SystemSendResp(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendResp_1 */

  /* USER CODE END MBMUXIF_SystemSendResp_1 */
  if (MBMUX_ResponseSnd(SystemPrioFeat) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendResp_Last */

  /* USER CODE END MBMUXIF_SystemSendResp_Last */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Exported services --------------------------------------------------------*/
int16_t MBMUXIF_ChipRevId(void)
{
  int16_t ret = pMb_RefTable->ChipRevId;
  /* USER CODE BEGIN MBMUXIF_ChipRevId */

  /* USER CODE END MBMUXIF_ChipRevId */
  return ret;
}

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrSystemAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSystemAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemAckRcvCb_Last */
}

static void MBMUXIF_IsrSystemCmdRcvCb(void *ComObj)
{
  SystemComObj = (MBMUX_ComParam_t *) ComObj;
  uint32_t *com_buffer = SystemComObj->ParamBuf;
  uint32_t status;

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemCmdRcvCb_1 */

  /* process Command on Isr or schedule a task*/
  switch (SystemComObj->MsgId)
  {
    case SYS_GET_INFO_LIST_MSG_ID:
      SystemComObj->ReturnVal = (uint32_t) FEAT_INFO_GetListPtr();
      MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
      break;

    case SYS_EE_READ_ID:
      status = (uint32_t) EE_ReadValue((e_EE_ID) com_buffer[0], (uint32_t *) com_buffer[1]);
      /* prepare response buffer */
      SystemComObj->ParamCnt = 1; /* reset ParamCnt */
      SystemComObj->ReturnVal = status;
      MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
      break;

    case SYS_EE_WRITE_ID:
      status = (uint32_t)EE_WriteValue((e_EE_ID) com_buffer[0], (uint32_t) com_buffer[1]);
      /* prepare response buffer */
      SystemComObj->ParamCnt = 1; /* reset ParamCnt */
      SystemComObj->ReturnVal = status;
      MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
      break;

    /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Switch */

    /* USER CODE END MBMUXIF_IsrSystemCmdRcvCb_Switch */

    default:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSystemCmdRcv), CFG_SEQ_Prio_0);
      break;
  }

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemCmdRcvCb_Last */

}

static void MBMUXIF_TaskSystemCmdRcv(void)
{
  uint32_t *com_buffer = MBMUX_SEC_VerifySramBufferPtr(SystemComObj->ParamBuf, SystemComObj->BufSize);
  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_1 */

  /* USER CODE END MBMUXIF_TaskSystemCmdRcv_1 */
  if (SystemComObj->MsgId == SYS_REGISTER_FEATURE_MSG_ID)
  {
    SystemComObj->ReturnVal = (uint32_t) MBMUXIF_FeatureCm0plusRegistrationCmd(com_buffer);
    MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
  }
  else
  {
    Process_Sys_Cmd(SystemComObj);
  }
  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_Last */

  /* USER CODE END MBMUXIF_TaskSystemCmdRcv_Last */

}

static void MBMUXIF_IsrSystemPrioAAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioAAckRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioAAckRcvCb */
}

static void MBMUXIF_IsrSystemPrioACmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioACmdRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioACmdRcvCb */
}

static void MBMUXIF_IsrSystemPrioBAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBAckRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBAckRcvCb */
}

static void MBMUXIF_IsrSystemPrioBCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBCmdRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBCmdRcvCb */
}

static int8_t MBMUXIF_FeatureCm0plusRegistrationCmd(uint32_t *pBuf)
{
  int8_t ret = -2; /* feature not supported */

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_1 */

  /* USER CODE END MBMUXIF_FeatureCm0plusRegistrationCmd_1 */
  APP_LOG(TS_OFF, VLEVEL_L, "\r\n");

  switch (pBuf[0])
  {
    case FEAT_INFO_SYSTEM_CMD_PRIO_A_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_CMD_PRIO_A_ID, MBMUX_CMD_RESP,
                                          MBMUXIF_IsrSystemPrioACmdRcvCb);
      break;

    case FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID, MBMUX_NOTIF_ACK,
                                          MBMUXIF_IsrSystemPrioAAckRcvCb);
      break;

    case FEAT_INFO_SYSTEM_CMD_PRIO_B_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_CMD_PRIO_B_ID, MBMUX_CMD_RESP,
                                          MBMUXIF_IsrSystemPrioBCmdRcvCb);
      break;

    case FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID, MBMUX_NOTIF_ACK,
                                          MBMUXIF_IsrSystemPrioBAckRcvCb);
      break;

    case FEAT_INFO_KMS_ID:
      /* ret = MBMUXIF_KmsInit(); */
      /* APP_LOG(TS_OFF, VLEVEL_L, " * CM0 : Kms registration done\r\n"); */
      break;

    case FEAT_INFO_TRACE_ID:
      /* USER CODE BEGIN FEAT_INFO_TRACE_ID */

      /* USER CODE END FEAT_INFO_TRACE_ID */
      ret = MBMUXIF_TraceInit(VERBOSE_LEVEL);
      break;

    case FEAT_INFO_RADIO_ID:
      ret = MBMUXIF_RadioInit();
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Radio registration done\r\n");
      break;

    case FEAT_INFO_LORAWAN_ID:
      /* USER CODE BEGIN FEAT_INFO_LORAWAN_ID */

      /* USER CODE END FEAT_INFO_LORAWAN_ID */
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Lora not supported\r\n");
      break;

    case FEAT_INFO_SIGFOX_ID:
      /* USER CODE BEGIN FEAT_INFO_SIGFOX_ID */

      /* USER CODE END FEAT_INFO_SIGFOX_ID */
      ret = MBMUXIF_SigfoxInit();
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Sigfox registration done\r\n");
      break;

    /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Switch */

    /* USER CODE END MBMUXIF_FeatureCm0plusRegistrationCmd_Switch */

    default:
      break;
  }

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Last */

  /* USER CODE END MBMUXIF_FeatureCm0plusRegistrationCmd_Last */

  return ret;
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
