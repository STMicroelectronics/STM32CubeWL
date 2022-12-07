/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_command.h
  * @author  MCD Application Team
  * @brief   Header for driver command.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEMO_COMMAND_H__
#define __DEMO_COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "demo_at.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Character added when a RX error has been detected */
#define AT_ERROR_RX_CHAR 0x01

/* Exported functions ------------------------------------------------------- */

/**
  * @brief Initializes command module
  *
  * @param [IN] cb to signal appli that character has been received
  * @retval None
  */
void CMD_Init(void (*CmdProcessNotify)(void));

/**
  * @brief Process the command
  *
  * @param [IN] None
  * @retval None
  */
void CMD_Process(void);


#ifdef __cplusplus
}
#endif

#endif /* __DEMO_COMMAND_H__*/
