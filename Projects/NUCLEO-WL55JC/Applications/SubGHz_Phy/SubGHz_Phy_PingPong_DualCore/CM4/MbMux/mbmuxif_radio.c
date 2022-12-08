/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_radio.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to register and handle RADIO driver via MBMUX
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
#include "mbmuxif_radio.h"
#include "mbmuxif_sys.h"
#include "sys_app.h"
#include "stm32_mem.h"
#include "stm32_seq.h"
#include "radio_mbwrapper.h"
#include "utilities_def.h"
#include "app_version.h"

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
static MBMUX_ComParam_t *RadioComObj;

/**
  * @brief radio cmd buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aRadioCmdRespBuff[MAX_PARAM_OF_RADIO_CMD_FUNCTIONS];/*shared*/

/**
  * @brief radio notif buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aRadioNotifAckBuff[MAX_PARAM_OF_RADIO_NOTIF_FUNCTIONS];/*shared*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   RADIO response callbacks: set event to release waiting task
  * @param   ComObj pointer to the RADIO com param buffer
  */
static void MBMUXIF_IsrRadioRespRcvCb(void *ComObj);

/**
  * @brief   RADIO notification callbacks: schedules a task in order to quit the ISR
  * @param   ComObj pointer to the RADIO com param buffer
  */
static void MBMUXIF_IsrRadioNotifRcvCb(void *ComObj);

/**
  * @brief  RADIO task to process the notification
  */
static void MBMUXIF_TaskRadioNotifRcv(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_RadioInit(void)
{
  FEAT_INFO_Param_t *p_cm0plus_system_info;
  int32_t cm0_vers = 0;
  int8_t ret = 0;

  /* USER CODE BEGIN MBMUXIF_RadioInit_1 */

  /* USER CODE END MBMUXIF_RadioInit_1 */

  p_cm0plus_system_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SYSTEM_ID);
  /* abstract CM0 release version from RC (release candidate) and compare */
  cm0_vers = p_cm0plus_system_info->Feat_Info_Feature_Version >> APP_VERSION_SUB2_SHIFT;
  if (cm0_vers < (LAST_COMPATIBLE_CM0_RELEASE >> APP_VERSION_SUB2_SHIFT))
  {
    ret = -4; /* version incompatibility */
  }
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_RADIO_ID, MBMUX_CMD_RESP,
                                MBMUXIF_IsrRadioRespRcvCb,
                                aRadioCmdRespBuff, sizeof(aRadioCmdRespBuff));
  }
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_RADIO_ID, MBMUX_NOTIF_ACK,
                                MBMUXIF_IsrRadioNotifRcvCb,
                                aRadioNotifAckBuff, sizeof(aRadioNotifAckBuff));
  }

  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbRadioNotifRcv), UTIL_SEQ_RFU, MBMUXIF_TaskRadioNotifRcv);
  }

  if (ret >= 0)
  {
    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_RADIO_ID);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  if (ret >= 0)
  {
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_RadioInit_Last */

  /* USER CODE END MBMUXIF_RadioInit_Last */
  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetRadioFeatureCmdComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetRadioFeatureCmdComPtr_1 */

  /* USER CODE END MBMUXIF_GetRadioFeatureCmdComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_RADIO_ID, MBMUX_CMD_RESP);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetRadioFeatureCmdComPtr_Last */

  /* USER CODE END MBMUXIF_GetRadioFeatureCmdComPtr_Last */
}

void MBMUXIF_RadioSendCmd(void)
{
  /* USER CODE BEGIN MBMUXIF_RadioSendCmd_1 */

  /* USER CODE END MBMUXIF_RadioSendCmd_1 */
  if (MBMUX_CommandSnd(FEAT_INFO_RADIO_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbRadioRespRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_RadioSendCmd_Last */

  /* USER CODE END MBMUXIF_RadioSendCmd_Last */
}

void MBMUXIF_RadioSendAck(void)
{
  /* USER CODE BEGIN MBMUXIF_RadioSendAck_1 */

  /* USER CODE END MBMUXIF_RadioSendAck_1 */
  if (MBMUX_AcknowledgeSnd(FEAT_INFO_RADIO_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_RadioSendAck_Last */

  /* USER CODE END MBMUXIF_RadioSendAck_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrRadioRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrRadioRespRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrRadioRespRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbRadioRespRcv);
  /* USER CODE BEGIN MBMUXIF_IsrRadioRespRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrRadioRespRcvCb_Last */
}

static void MBMUXIF_IsrRadioNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrRadioNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrRadioNotifRcvCb_1 */
  RadioComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbRadioNotifRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrRadioNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrRadioNotifRcvCb_Last */
}

static void MBMUXIF_TaskRadioNotifRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskRadioNotifRcv_1 */

  /* USER CODE END MBMUXIF_TaskRadioNotifRcv_1 */
  Process_Radio_Notif(RadioComObj);
  /* USER CODE BEGIN MBMUXIF_TaskRadioNotifRcv_Last */

  /* USER CODE END MBMUXIF_TaskRadioNotifRcv_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
