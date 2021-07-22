/**
  ******************************************************************************
  * @file    demo_command.h
  * @author  MCD Application Team
  * @brief   Header for driver command.c module
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
