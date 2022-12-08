/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
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
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"
#include "subghz_phy_version.h"
#include "subg_command.h"
#include "subg_at.h"

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
/**
  * @brief  call back when LoRaWan Stack needs update
  */
static void CmdProcessNotify(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  CMD_Init(CmdProcessNotify);

  /* USER CODE BEGIN SubghzApp_Init_1 */

  /* Get SubGHY_Phy APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  /* USER CODE END SubghzApp_Init_1 */

  /* Register Virtual-Com task then radio goes in Sleep/Stop mode waiting an AT cmd */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Vcom), UTIL_SEQ_RFU, CMD_Process);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  APP_PPRINTF("ATtention command interface (only for TST_RF)\r\n");
  APP_PPRINTF("AT? to list all available functions\r\n");

  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void CmdProcessNotify(void)
{
  /* USER CODE BEGIN CmdProcessNotify_1 */

  /* USER CODE END CmdProcessNotify_1 */
  UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_Vcom, CFG_SEQ_Prio_0);
  /* USER CODE BEGIN CmdProcessNotify_2 */

  /* USER CODE END CmdProcessNotify_2 */
}
/* USER CODE BEGIN PrFD */
/* USER CODE END PrFD */
