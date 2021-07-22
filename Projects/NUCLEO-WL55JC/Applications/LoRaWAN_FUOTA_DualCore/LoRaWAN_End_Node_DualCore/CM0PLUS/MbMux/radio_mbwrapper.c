/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM0 side wrapper of the Radio interface
  *          shared between M0 and M4.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "radio_mbwrapper.h"
#include "main.h"
#include "stm32_mem.h"
#include "mbmux_table.h"
#include "msg_id.h"
#include "mbmux.h"
#include "mbmuxif_radio.h"
#include "sys_app.h"

#include "radio.h"
#include "radio_conf.h"

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
/**
  * @brief radio buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM2") uint8_t aRadioMbWrapRxBuffer[RADIO_RX_BUF_SIZE];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void Process_Radio_Cmd(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Radio_Cmd_1 */

  /* USER CODE END Process_Radio_Cmd_1 */

  APP_LOG(TS_ON, VLEVEL_H, ">CM0PLUS(Radio)\r\n");

  /* process Command */
  switch (ComObj->MsgId)
  {
    default:
      break;
  }

  /* send Response */
  APP_LOG(TS_ON, VLEVEL_H, "<CM0PLUS(Radio)\r\n");
  MBMUX_ResponseSnd(FEAT_INFO_RADIO_ID);
  /* USER CODE BEGIN Process_Radio_Cmd_2 */

  /* USER CODE END Process_Radio_Cmd_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
