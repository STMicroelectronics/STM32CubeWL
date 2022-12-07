/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ipcc_if.c
  * @author  MCD Application Team
  * @brief   Interface to IPCC: handles IRQs and abstract application from Ipcc handler and channel direction
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

/*ipcc interface for M4*/
#include "assert.h"
#include "stddef.h"
#include "ipcc_if.h"

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
static void (*IpccResponseRcv)(uint32_t channelIdx);

static void (*IpccNotificationRcv)(uint32_t channelIdx);

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief Interrupt service routine executed when Ipcc receive IRQ confirmation of a received message by remote CPU
  */
static void IpccIfIsrTxCb(IPCC_HandleTypeDef *hIpcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);

/**
  * @brief Interrupt service routine executed when Ipcc receive IRQ notification request by remote CPU
  */
static void IpccIfIsrRxCb(IPCC_HandleTypeDef *hIpcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void IPCC_IF_Init(void (*IPCC_IF_ResponseRcv_cb)(uint32_t channelIdx),
                  void (*IPCC_IF_NotificationRcv_cb)(uint32_t channelIdx))
{
  /* USER CODE BEGIN IPCC_IF_Init_1 */

  /* USER CODE END IPCC_IF_Init_1 */
  /*Initialize the ipcc, initialize MSP*/
  MX_IPCC_Init();

  /* IPCC EXTI (AIEC) line for CPU1  */
  LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_36);
  LL_EXTI_EnableRisingTrig_32_63(LL_EXTI_LINE_36);

  /* Initialize IPCC callback of all channels*/
  for (int32_t i = 0; i < IPCC_CHANNEL_NUMBER; i++)
  {
    HAL_IPCC_ActivateNotification(&hipcc, i, IPCC_CHANNEL_DIR_TX, IpccIfIsrTxCb);
    HAL_IPCC_ActivateNotification(&hipcc, i, IPCC_CHANNEL_DIR_RX, IpccIfIsrRxCb);
  }

  /* Registers upper level (application) callback */
  IpccResponseRcv = IPCC_IF_ResponseRcv_cb;
  IpccNotificationRcv = IPCC_IF_NotificationRcv_cb;
  return;
  /* USER CODE BEGIN IPCC_IF_Init_2 */

  /* USER CODE END IPCC_IF_Init_2 */
}

uint32_t IPCC_IF_CmdRespStatus(uint32_t channelIdx)
{
  /* USER CODE BEGIN IPCC_IF_CmdRespStatus_1 */

  /* USER CODE END IPCC_IF_CmdRespStatus_1 */
  return HAL_IPCC_GetChannelStatus(&hipcc, channelIdx, IPCC_CHANNEL_DIR_TX);
  /* USER CODE BEGIN IPCC_IF_CmdRespStatus_2 */

  /* USER CODE END IPCC_IF_CmdRespStatus_2 */
}

uint32_t IPCC_IF_NotifAckStatus(uint32_t channelIdx)
{
  /* USER CODE BEGIN IPCC_IF_NotifAckStatus_1 */

  /* USER CODE END IPCC_IF_NotifAckStatus_1 */
  return HAL_IPCC_GetChannelStatus(&hipcc, channelIdx, IPCC_CHANNEL_DIR_RX);
  /* USER CODE BEGIN IPCC_IF_NotifAckStatus_2 */

  /* USER CODE END IPCC_IF_NotifAckStatus_2 */
}

int32_t IPCC_IF_CommandSnd(uint32_t channelIdx)
{
  /* USER CODE BEGIN IPCC_IF_CommandSnd_1 */

  /* USER CODE END IPCC_IF_CommandSnd_1 */
  if (HAL_OK != HAL_IPCC_NotifyCPU(&hipcc, channelIdx, IPCC_CHANNEL_DIR_TX))
  {
    return -1;
  }
  else
  {
    return 0;
  }
  /* USER CODE BEGIN IPCC_IF_CommandSnd_2 */

  /* USER CODE END IPCC_IF_CommandSnd_2 */
}

int32_t IPCC_IF_AcknowledgeSnd(uint32_t channelIdx)
{
  /* USER CODE BEGIN IPCC_IF_AcknowledgeSnd_1 */

  /* USER CODE END IPCC_IF_AcknowledgeSnd_1 */
  if (HAL_OK != HAL_IPCC_NotifyCPU(&hipcc, channelIdx, IPCC_CHANNEL_DIR_RX))
  {
    return -1;
  }
  else
  {
    return 0;
  }
  /* USER CODE BEGIN IPCC_IF_AcknowledgeSnd_2 */

  /* USER CODE END IPCC_IF_AcknowledgeSnd_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static void IpccIfIsrTxCb(IPCC_HandleTypeDef *hIpcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN IpccIfIsrTxCb_1 */

  /* USER CODE END IpccIfIsrTxCb_1 */
  UNUSED(hIpcc);
  UNUSED(ChannelDir);

  IpccResponseRcv(ChannelIndex);
  return;
  /* USER CODE BEGIN IpccIfIsrTxCb_2 */

  /* USER CODE END IpccIfIsrTxCb_2 */
}

static void IpccIfIsrRxCb(IPCC_HandleTypeDef *hIpcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN IpccIfIsrRxCb_1 */

  /* USER CODE END IpccIfIsrRxCb_1 */
  UNUSED(hIpcc);
  UNUSED(ChannelDir);

  IpccNotificationRcv(ChannelIndex);
  return;
  /* USER CODE BEGIN IpccIfIsrRxCb_2 */

  /* USER CODE END IpccIfIsrRxCb_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
