/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_flash.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to register and handle FLASH via MBMUX
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
#include "mbmuxif_flash.h"
#include "mbmuxif_sys.h"
#include "sys_app.h"
#include "msg_id.h"
#include "stm32_mem.h"
#include "flash_if.h"
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
/**
  * @brief flash buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aFlashNotifAckBuff[MAX_PARAM_OF_FLASH_NOTIF_FUNCTIONS];/*shared*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  FLASH notification callbacks: schedules a task in order to quit the ISR
  * @param  ComObj pointer to the FLASH com param buffer
  */
static void MBMUXIF_IsrFlashNotifRcvCb(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_FlashInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_FlashInit_1 */

  /* USER CODE END MBMUXIF_FlashInit_1 */

  ret = MBMUX_RegisterFeature(FEAT_INFO_FLASH_ID, MBMUX_NOTIF_ACK,
                              MBMUXIF_IsrFlashNotifRcvCb,
                              aFlashNotifAckBuff, sizeof(aFlashNotifAckBuff));
  if (ret >= 0)
  {
    /* Note: flash works on ISR, no need to associate a SEQ_TASK */

    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_FLASH_ID);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  /* USER CODE BEGIN MBMUXIF_FlashInit_Last */

  /* USER CODE END MBMUXIF_FlashInit_Last */
  return ret;
}

void MBMUXIF_FlashSendAck(void)
{
  /* USER CODE BEGIN MBMUXIF_FlashSendAck_1 */

  /* USER CODE END MBMUXIF_FlashSendAck_1 */
  if (MBMUX_AcknowledgeSnd(FEAT_INFO_FLASH_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_FlashSendAck_Last */

  /* USER CODE END MBMUXIF_FlashSendAck_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrFlashNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrFlashNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrFlashNotifRcvCb_1 */
  MBMUX_ComParam_t *com_param = (MBMUX_ComParam_t *) ComObj;
  FLASH_IF_StatusTypedef ret;

  switch (com_param->MsgId)
  {
    case FLASHIF_WRITE_ID:
      ret = FLASH_IF_Write((void *)(com_param->ParamBuf[0]),
                           (const void *)(com_param->ParamBuf[1]),
                           com_param->ParamBuf[2]);
      /* prepare ack buffer */
      com_param->ParamCnt = 0;
      com_param->ReturnVal = (uint32_t)ret;
      break;

    case FLASHIF_READ_ID:
      ret = FLASH_IF_Read((void *)(com_param->ParamBuf[0]),
                          (const void *)(com_param->ParamBuf[1]),
                          com_param->ParamBuf[2]);
      /* prepare ack buffer */
      com_param->ParamCnt = 0;
      com_param->ReturnVal = (uint32_t)ret;
      break;

    case FLASHIF_ERASE_ID:
      ret = FLASH_IF_Erase((void *)(com_param->ParamBuf[0]), com_param->ParamBuf[1]);
      /* prepare ack buffer */
      com_param->ParamCnt = 0;
      com_param->ReturnVal = (uint32_t)ret;
      break;

    default:
      break;
  }

  /* Send ack */
  MBMUXIF_FlashSendAck();
  /* USER CODE BEGIN MBMUXIF_IsrFlashNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrFlashNotifRcvCb_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
