/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_flash.c
  * @author  MCD Application Team
  * @brief   Interface layer CM0PLUS Flash to MBMUX (Mailbox Multiplexer)
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
#include "mbmux.h"
#include "sys_app.h"
#include "msg_id.h"
#include "stm32_seq.h"
#include "utilities_def.h"   /* for CFG_SEQ_Evt_MbFlashAckRcv */
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
#define FLASH_MBWRAP_SHBUF_SIZE      512
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Flash_if buffer to exchange data between from CM0+ to CM4
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM3") uint8_t aFlashMbWrapSharedBuffer[FLASH_MBWRAP_SHBUF_SIZE];
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   FLASH acknowledge callbacks: set event to release waiting task
  * @param   ComObj pointer to the FLASH com param buffer
  */
static void MBMUXIF_IsrFlashAckRcvCb(void *ComObj);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_FlashInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_FlashInit_1 */

  /* USER CODE END MBMUXIF_FlashInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_FLASH_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrFlashAckRcvCb);
  if (ret >= 0)
  {
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_FlashInit_Last */

  /* USER CODE END MBMUXIF_FlashInit_Last */
  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetFlashFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetFlashFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetFlashFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_FLASH_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetFlashFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetFlashFeatureNotifComPtr_Last */
}

void MBMUXIF_FlashSendNotif(void)
{
  /* USER CODE BEGIN MBMUXIF_FlashSendNotif_1 */

  /* USER CODE END MBMUXIF_FlashSendNotif_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_FLASH_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbFlashAckRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_FlashSendNotif_Last */

  /* USER CODE END MBMUXIF_FlashSendNotif_Last */
}

FLASH_IF_StatusTypedef FLASH_IF_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  /* USER CODE BEGIN FLASH_IF_Write_1 */

  /* USER CODE END FLASH_IF_Write_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if ((pDestination == NULL) || (pSource == NULL) || (uLength > FLASH_MBWRAP_SHBUF_SIZE))
  {
    return FLASH_IF_ERROR;
  }

  /* copy data from CM0PLUS stack memory to shared memory */
  if (uLength > 0)
  {
    UTIL_MEM_cpy_8(aFlashMbWrapSharedBuffer, (const void *)pSource, uLength);
  }

  com_obj = MBMUXIF_GetFlashFeatureNotifComPtr();
  com_obj->MsgId = FLASHIF_WRITE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) pDestination;
  com_buffer[i++] = (uint32_t) aFlashMbWrapSharedBuffer;
  com_buffer[i++] = (uint32_t) uLength;
  com_obj->ParamCnt = i;

  MBMUXIF_FlashSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (FLASH_IF_StatusTypedef) ret;
  /* USER CODE BEGIN FLASH_IF_Write_2 */

  /* USER CODE END FLASH_IF_Write_2 */
}

FLASH_IF_StatusTypedef FLASH_IF_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  /* USER CODE BEGIN FLASH_IF_Read_1 */

  /* USER CODE END FLASH_IF_Read_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if ((pDestination == NULL) || (pSource == NULL) || (uLength > FLASH_MBWRAP_SHBUF_SIZE))
  {
    return FLASH_IF_ERROR;
  }

  com_obj = MBMUXIF_GetFlashFeatureNotifComPtr();
  com_obj->MsgId = FLASHIF_READ_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aFlashMbWrapSharedBuffer;
  com_buffer[i++] = (uint32_t) pSource;
  com_buffer[i++] = (uint32_t) uLength;
  com_obj->ParamCnt = i;

  MBMUXIF_FlashSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  UTIL_MEM_cpy_8(pDestination, (const void *)aFlashMbWrapSharedBuffer, uLength);

  ret = com_obj->ReturnVal;
  return (FLASH_IF_StatusTypedef) ret;
  /* USER CODE BEGIN FLASH_IF_Read_2 */

  /* USER CODE END FLASH_IF_Read_2 */
}

FLASH_IF_StatusTypedef FLASH_IF_Erase(void *pStart, uint32_t uLength)
{
  /* USER CODE BEGIN FLASH_IF_Erase_1 */

  /* USER CODE END FLASH_IF_Erase_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (pStart == NULL)
  {
    return FLASH_IF_ERROR;
  }

  com_obj = MBMUXIF_GetFlashFeatureNotifComPtr();
  com_obj->MsgId = FLASHIF_ERASE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) pStart;
  com_buffer[i++] = (uint32_t) uLength;
  com_obj->ParamCnt = i;

  MBMUXIF_FlashSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (FLASH_IF_StatusTypedef) ret;
  /* USER CODE BEGIN FLASH_IF_Erase_2 */

  /* USER CODE END FLASH_IF_Erase_2 */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrFlashAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrFlashAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrFlashAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbFlashAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrFlashAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrFlashAckRcvCb_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
