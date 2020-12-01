/**
  ******************************************************************************
  * @file    mbmuxif_sys.c
  * @author  MCD Application Team
  * @brief   allows CM0 applic to handle the SYSTEM MBMUX channel
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "mbmuxif_sys.h"
#include "stm32_seq.h"
#include "sys_app.h"
#include "msg_id.h"
#include "mbmux.h"
#include "mbmuxif_trace.h"
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
static void MBMUXIF_IsrSystemAckRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemCmdRcvCb(void *ComObj);
static void MBMUXIF_TaskSystemCmdRcv(void);
static void MBMUXIF_IsrSystemPrioAAckRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioACmdRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioBAckRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioBCmdRcvCb(void *ComObj);

static int8_t MBMUXIF_FeatureCm0plusRegistrationCmd(uint32_t *ParamBuf);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief Inits the MBMUX and registers SYS channel to the mailbox and SYS task to the sequencer
  * @param none
  * @retval   0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS
  */
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
    /* Notify Cm4 that MBMUXIF_System and RTC are initialised on Cm0 side */
    /* as alternative MBMUXIF_SystemSendNotif() can be used as far Cm4 treats it on Irq */
    while (pMb_RefTable->SynchronizeCpusAtBoot != CPUS_BOOT_SYNC_ALLOW_CPU2_TO_START);
    pMb_RefTable->SynchronizeCpusAtBoot = CPUS_BOOT_SYNC_CPU2_INIT_COMPLETED;
  }

  /* USER CODE BEGIN MBMUXIF_SystemInit_Last */

  /* USER CODE END MBMUXIF_SystemInit_Last */
  return ret;
}

/**
  * @brief Get synchro flag between the two CPUs
  * @param flag: 0xFFFF: hold Cm0 before it runs its MBMUX_Init,
  *              0x5555: Cm0 allowed to run its MBMUX_Init
  *              0xAAAA: Cm0 has completed initialisation
  *              0x9999: RTC PRIO CHANNEL has been registered
  * @retval  none
  */
uint16_t MBMUXIF_GetCpusSynchroFlag(void)
{
  /* USER CODE BEGIN MBMUXIF_GetCpusSynchroFlag */

  /* USER CODE END MBMUXIF_GetCpusSynchroFlag */
  return pMb_RefTable->SynchronizeCpusAtBoot;
}

/**
  * @brief gives back the pointer to the com buffer associated to System feature Notif
  * @param none
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureNotifComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetSystemFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(SystemPrioFeat, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    while (1) {} /* ErrorHandler() : feature isn't registered */
  }
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetSystemFeatureNotifComPtr_Last */
  return com_param_ptr;
}

/**
  * @brief Sends a System-Notif via Ipcc and Wait for the ack
  * @param none
  * @retval   none
  */
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
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_Last */

  /* USER CODE END MBMUXIF_SystemSendNotif_Last */
}

/**
  * @brief Sends a Trace-Notif via Ipcc without waiting for the ack
  * @param none
  * @retval   none
  */
void MBMUXIF_SystemSendNotif_NoWait(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_NoWait_1 */

  /* USER CODE END MBMUXIF_SystemSendNotif_NoWait_1 */
  if (MBMUX_NotificationSnd(SystemPrioFeat) != 0)
  {
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendNotif_NoWait_Last */

  /* USER CODE END MBMUXIF_SystemSendNotif_NoWait_Last */
}

/**
  * @brief Sends a System-Resp  via Ipcc without waiting for the response
  * @param none
  * @retval   none
  */
void MBMUXIF_SystemSendResp(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendResp_1 */

  /* USER CODE END EFMBMUXIF_SystemSendResp_1 */
  if (MBMUX_ResponseSnd(SystemPrioFeat) != 0)
  {
    while (1) {} /* ErrorHandler(); */
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

  /* USER CODE BEGIN MBMUXIF_ChipRevId */
  return ret ;
}

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  SYSTEM acknowledge callbacks: set event to release waiting task
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_1 */

  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSystemAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_Last */

  /* USER CODE BEGIN MBMUXIF_IsrSystemAckRcvCb_Last */
}

/**
  * @brief  SYSTEM command callbacks: schedules a task in order to quit the ISR
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemCmdRcvCb(void *ComObj)
{
  SystemComObj = (MBMUX_ComParam_t *) ComObj;
  uint32_t *com_buffer = SystemComObj->ParamBuf;

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_1 */

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_1 */

  /* process Command on Isr or schedule a task*/
  switch (SystemComObj->MsgId)
  {
    case SYS_GET_INFO_LIST_MSG_ID:
      SystemComObj->ReturnVal = (uint32_t) FEAT_INFO_GetListPtr();
      MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
      break;

    case SYS_REGISTER_FEATURE_MSG_ID:
      SystemComObj->ReturnVal = (uint32_t) MBMUXIF_FeatureCm0plusRegistrationCmd(com_buffer);
      MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
      break;

    /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Switch */

    /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Switch */

    default:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSystemCmdRcv), CFG_SEQ_Prio_0);
      break;
  }

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Last */

  /* USER CODE BEGIN MBMUXIF_IsrSystemCmdRcvCb_Last */

}

/**
  * @brief  SYSTEM task to process the command
  * @param  none
  * @retval  none
  */
static void MBMUXIF_TaskSystemCmdRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_1 */

  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_1 */
  Process_Sys_Cmd(SystemComObj);
  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_Last */

  /* USER CODE BEGIN MBMUXIF_TaskSystemCmdRcv_Last */

}

/**
  * @brief  SYSTEM PRIO_A acknowledge callbacks:
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioAAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioAAckRcvCb */

  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioAAckRcvCb */
}

/**
  * @brief  SYSTEM PRIO_A command callbacks:
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioACmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioACmdRcvCb */

  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioACmdRcvCb */
}

/**
  * @brief  SYSTEM PRIO_B acknowledge callbacks:
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioBAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBAckRcvCb */

  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBAckRcvCb */
}

/**
  * @brief  SYSTEM PRIO_B command callbacks:
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioBCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBCmdRcvCb */

  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBCmdRcvCb */
}

/**
  * @brief  Retrieve the feature that needs MbMux registration and call correspondent Init
  * @param  pointer to parameter buffer of the command
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  */
static int8_t MBMUXIF_FeatureCm0plusRegistrationCmd(uint32_t *ParamBuf)
{
  int8_t ret = -2; /* feature not supported */

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_1 */

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_1 */

  switch (ParamBuf[0])
  {
    case FEAT_INFO_SYSTEM_CMD_PRIO_A_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_CMD_PRIO_A_ID, MBMUX_CMD_RESP, MBMUXIF_IsrSystemPrioACmdRcvCb);
      break;

    case FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemPrioAAckRcvCb);
      break;

    case FEAT_INFO_SYSTEM_CMD_PRIO_B_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_CMD_PRIO_B_ID, MBMUX_CMD_RESP, MBMUXIF_IsrSystemPrioBCmdRcvCb);
      break;

    case FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID:
      ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemPrioBAckRcvCb);
      break;

    case FEAT_INFO_KMS_ID:
      /* ret = MBMUXIF_KmsInit(); */
      /* APP_LOG(TS_OFF, VLEVEL_L, " * CM0 : Kms registration done\r\n"); */
      break;

    case FEAT_INFO_TRACE_ID:
      ret = MBMUXIF_TraceInit(VERBOSE_LEVEL);
      break;

    case FEAT_INFO_RADIO_ID:
      ret = MBMUXIF_RadioInit();
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Radio registration done\r\n");
      break;

    case FEAT_INFO_LORAWAN_ID:
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Lora not supported\r\n");
      break;

    case FEAT_INFO_SIGFOX_ID:
      APP_LOG(TS_OFF, VLEVEL_L, "CM0PLUS : Sigfox not supported\r\n");
      break;

    /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Switch */

    /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Switch */

    default:
      break;
  }

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Last */

  /* USER CODE BEGIN MBMUXIF_FeatureCm0plusRegistrationCmd_Last */

  return ret;
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
