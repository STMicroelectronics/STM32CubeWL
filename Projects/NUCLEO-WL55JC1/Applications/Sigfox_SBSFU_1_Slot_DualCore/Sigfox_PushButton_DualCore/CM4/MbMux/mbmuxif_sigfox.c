/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sigfox.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to register and handle SigfoxWAN via MBMUX
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
#include "mbmuxif_sigfox.h"
#include "mbmuxif_sys.h"
#include "sys_app.h"
#include "stm32_mem.h"
#include "stm32_seq.h"
#include "sigfox_mbwrapper.h"
#include "app_version.h"
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
static MBMUX_ComParam_t *SigfoxComObj;

UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSigfoxCmdRespBuff[MAX_PARAM_OF_SIGFOX_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSigfoxNotifAckBuff[MAX_PARAM_OF_SIGFOX_NOTIF_FUNCTIONS];/*shared*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Sigfox response callbacks: set event to release waiting task
  * @param  ComObj pointer to the Sigfox com param buffer
  */
static void MBMUXIF_IsrSigfoxRespRcvCb(void *ComObj);

/**
  * @brief  Sigfox notification callbacks: schedules a task in order to quit the ISR
  * @param  ComObj pointer to the Sigfox com param buffer
  */
static void MBMUXIF_IsrSigfoxNotifRcvCb(void *ComObj);

/**
  * @brief  Sigfox task to process the notification
  */
static void MBMUXIF_TaskSigfoxNotifRcv(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_SigfoxInit(void)
{
  FEAT_INFO_Param_t *p_cm0plus_system_info;
  int32_t cm0_vers = 0;
  int8_t ret = 0;

  /* USER CODE BEGIN MBMUXIF_SigfoxInit_1 */

  /* USER CODE END MBMUXIF_SigfoxInit_1 */

  p_cm0plus_system_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SYSTEM_ID);
  /* abstract CM0 release version from RC (release candidate) and compare */
  cm0_vers = p_cm0plus_system_info->Feat_Info_Feature_Version >> APP_VERSION_SUB2_SHIFT;
  if (cm0_vers < (LAST_COMPATIBLE_CM0_RELEASE >> APP_VERSION_SUB2_SHIFT))
  {
    ret = -4; /* version incompatibility */
  }
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_SIGFOX_ID, MBMUX_CMD_RESP,
                                MBMUXIF_IsrSigfoxRespRcvCb,
                                aSigfoxCmdRespBuff, sizeof(aSigfoxCmdRespBuff));
  }
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_SIGFOX_ID, MBMUX_NOTIF_ACK,
                                MBMUXIF_IsrSigfoxNotifRcvCb,
                                aSigfoxNotifAckBuff, sizeof(aSigfoxNotifAckBuff));
  }
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask(1 << CFG_SEQ_Task_MbSigfoxNotifRcv, UTIL_SEQ_RFU, MBMUXIF_TaskSigfoxNotifRcv);
  }

  if (ret >= 0)
  {
    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_SIGFOX_ID);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  if (ret >= 0)
  {
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_SigfoxInit_Last */

  /* USER CODE END MBMUXIF_SigfoxInit_Last */

  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetSigfoxFeatureCmdComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureCmdComPtr_1 */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureCmdComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_SIGFOX_ID, MBMUX_CMD_RESP);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureCmdComPtr_Last */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureCmdComPtr_Last */
}

void MBMUXIF_SigfoxSendCmd(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendCmd_1 */

  /* USER CODE END MBMUXIF_SigfoxSendCmd_1 */
  if (MBMUX_CommandSnd(FEAT_INFO_SIGFOX_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbSigfoxRespRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendCmd_Last */

  /* USER CODE END MBMUXIF_SigfoxSendCmd_Last */
}

void MBMUXIF_SigfoxSendAck(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendAck_1 */

  /* USER CODE END MBMUXIF_SigfoxSendAck_1 */
  if (MBMUX_AcknowledgeSnd(FEAT_INFO_SIGFOX_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendAck_Last */

  /* USER CODE END MBMUXIF_SigfoxSendAck_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrSigfoxRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxRespRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxRespRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSigfoxRespRcv);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxRespRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxRespRcvCb_Last */
}

static void MBMUXIF_IsrSigfoxNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxNotifRcvCb_1 */
  SigfoxComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_MbSigfoxNotifRcv, CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxNotifRcvCb_Last */
}

static void MBMUXIF_TaskSigfoxNotifRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskSigfoxNotifRcv_1 */

  /* USER CODE END MBMUXIF_TaskSigfoxNotifRcv_1 */
  Process_Sigfox_Notif(SigfoxComObj);
  /* USER CODE BEGIN MBMUXIF_TaskSigfoxNotifRcv_Last */

  /* USER CODE END MBMUXIF_TaskSigfoxNotifRcv_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
