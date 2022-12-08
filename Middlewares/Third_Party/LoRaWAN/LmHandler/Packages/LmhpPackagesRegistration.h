/**
  ******************************************************************************
  * @file    LmhpPackagesRegistration.h
  * @author  MCD Application Team
  * @brief   Function prototypes for LoRaMac Packages Registration agent
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020(-2021) STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __LMHP_PACKAGES_REGISTRATION_H__
#define __LMHP_PACKAGES_REGISTRATION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LmhPackage.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
LmHandlerErrorStatus_t LmhpPackagesRegistrationInit( Version_t *fwVersion );

LmHandlerErrorStatus_t LmhpPackagesRegister( uint8_t id, LmhPackage_t **package );

#ifdef __cplusplus
}
#endif

#endif /* __LMHP_PACKAGES_REGISTRATION_H__ */
