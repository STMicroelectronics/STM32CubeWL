/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ipcc.c
  * @brief   This file provides code for the configuration
  *          of the IPCC instances.
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
#include "ipcc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

IPCC_HandleTypeDef hipcc;

/* IPCC init function */
void MX_IPCC_Init(void)
{

  /* USER CODE BEGIN IPCC_Init 0 */

  /* USER CODE END IPCC_Init 0 */

  /* USER CODE BEGIN IPCC_Init 1 */

  /* USER CODE END IPCC_Init 1 */
  hipcc.Instance = IPCC;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IPCC_Init 2 */

  /* USER CODE END IPCC_Init 2 */

}

void HAL_IPCC_MspInit(IPCC_HandleTypeDef* ipccHandle)
{

  if(ipccHandle->Instance==IPCC)
  {
  /* USER CODE BEGIN IPCC_MspInit 0 */

  /* USER CODE END IPCC_MspInit 0 */
    /* IPCC clock enable */
    __HAL_RCC_IPCC_CLK_ENABLE();

    /* IPCC interrupt Init */
    HAL_NVIC_SetPriority(IPCC_C2_RX_C2_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(IPCC_C2_RX_C2_TX_IRQn);
  /* USER CODE BEGIN IPCC_MspInit 1 */

  /* USER CODE END IPCC_MspInit 1 */
  }
}

void HAL_IPCC_MspDeInit(IPCC_HandleTypeDef* ipccHandle)
{

  if(ipccHandle->Instance==IPCC)
  {
  /* USER CODE BEGIN IPCC_MspDeInit 0 */

  /* USER CODE END IPCC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_IPCC_CLK_DISABLE();

    /* IPCC interrupt Deinit */
    HAL_NVIC_DisableIRQ(IPCC_C2_RX_C2_TX_IRQn);
  /* USER CODE BEGIN IPCC_MspDeInit 1 */

  /* USER CODE END IPCC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
