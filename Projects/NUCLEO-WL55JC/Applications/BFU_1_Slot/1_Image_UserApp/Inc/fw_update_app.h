/**
  ******************************************************************************
  * @file    fw_update_app.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Firmware Update module
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FW_UPDATE_APP_H
#define FW_UPDATE_APP_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

void FW_UPDATE_Run(void);
void FW_UPDATE_MULTIPLE_RunMenu(void);
void FW_VALIDATE_RunMenu(void);

#ifdef __cplusplus
}
#endif

#endif /* FW_UPDATE_APP_H */
