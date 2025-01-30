/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    system_mbwrapper.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to handle the SYSTEM MBMUX channel.
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
#include "system_mbwrapper.h"
#include "platform.h"
#include "mbmux.h"
#include "msg_id.h"
#include "mbmuxif_sys.h"
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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
uint32_t SYS_EE_ReadBuffer_mbwrapper(e_EE_ID EEsgfxID, uint32_t *data)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret = 0;

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_EE_READ_ID;
    com_buffer = com_obj->ParamBuf;
    /* Caution: copy all PARAMS from struct E2P_wrapper_params_t */
    com_buffer[i++] = (uint32_t) EEsgfxID;
    com_buffer[i++] = (uint32_t) data;
    com_obj->ParamCnt = i;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for event */
    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }
  return (uint32_t) ret;
}

uint32_t SYS_EE_WriteBuffer_mbwrapper(e_EE_ID EEsgfxID, uint32_t data)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret = 0;

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_EE_WRITE_ID;
    com_buffer = com_obj->ParamBuf;
    /* Caution: copy all PARAMS from struct E2P_wrapper_params_t */
    com_buffer[i++] = (uint32_t) EEsgfxID;
    com_buffer[i++] = (uint32_t) data;
    com_obj->ParamCnt = i;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for event */
    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }
  return (uint32_t) ret;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */
