/**
  ******************************************************************************
  * @file    LmhpPackagesRegistration.h
  * @author  MCD Application Team
  * @brief   Function prototypes for LoRaMac Packages Registration agent
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
LmHandlerErrorStatus_t LmhpPackagesRegistrationInit(void);

LmHandlerErrorStatus_t LmhpPackagesRegister(uint8_t id, LmhPackage_t **package);

#ifdef __cplusplus
}
#endif

#endif /* __LMHP_PACKAGES_REGISTRATION_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
