/**
  ******************************************************************************
  * @file    app_sfu.h
  * @author  MCD Application Team
  * @brief   This file contains the configuration of SB application.
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
#ifndef APP_SFU_H
#define APP_SFU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_sfu_common.h"

/**
  * Optional Features Software Configuration
  */
#define SECBOOT_LOADER SECBOOT_USE_LOCAL_LOADER    /*!< Loader selection inside SBSFU : local/standalone/none */

#if !defined(SECBOOT_DISABLE_SECURITY_IPS)

/* Uncomment the following defines when in Release mode.
   In debug mode it can be better to disable some of the following protection
   for a better Debug experience (WRP, RDP, IWDG, DAP, etc.) */

#define SFU_MPU_PROTECT_ENABLE         /*!< MPU protection:
                                            Enables/Disables the MPU protection. */
#define SFU_MPU_USERAPP_ACTIVATION     /*!< MPU protection during UserApp execution : Only Active Slot 2 considered as
                                            an executable area */

#endif /* !SECBOOT_DISABLE_SECURITY_IPS */


#ifdef __cplusplus
}
#endif

#endif /* APP_SFU_H */