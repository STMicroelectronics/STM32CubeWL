/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_lorawan.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
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
#include "app_lorawan.h"
#include "lora_app.h"
#include "sys_app.h"
#include "stm32_seq.h"

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

void MX_LoRaWAN_Init(void)
{
  /* USER CODE BEGIN MX_LoRaWAN_Init_1 */

  /* USER CODE END MX_LoRaWAN_Init_1 */
  SystemApp_Init();
  /* USER CODE BEGIN MX_LoRaWAN_Init_2 */

  /* USER CODE END MX_LoRaWAN_Init_2 */
  LoRaWAN_Init();
  /* USER CODE BEGIN MX_LoRaWAN_Init_3 */

  /* USER CODE END MX_LoRaWAN_Init_3 */
}

void MX_LoRaWAN_Process(void)
{
  /* USER CODE BEGIN MX_LoRaWAN_Process_1 */

  /* USER CODE END MX_LoRaWAN_Process_1 */
  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
  /* USER CODE BEGIN MX_LoRaWAN_Process_2 */

  /* USER CODE END MX_LoRaWAN_Process_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
