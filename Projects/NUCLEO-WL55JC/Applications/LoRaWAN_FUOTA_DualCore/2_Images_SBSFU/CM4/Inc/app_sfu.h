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

/*#define SFU_DEBUG_MODE*/               /*!< Comment this define to optimize memory footprint (debug mode removed)
                                          No more print on terminal during SBSFU execution */

/**
  * SB_SFU status LED.
  * The constants below define the LED to be used and the LED blinking frequency to identify some situations.
  * This is useful when no log is enabled.
  *
  * \li The LED blinks every see @ref SFU_COM_YMODEM_DOWNLOAD_TIMEOUT seconds when a local download is waited.
  * \li For the other situations, please check the other defines below.
  */
#define SFU_STATUS_LED (LED_RED)              /*!< LED to be used to provide the SB_SFU status to the end-user */
#define SFU_STOP_NO_FW_BLINK_DELAY     (100U) /*!< Blinks every 100ms when no valid firmware is available and the local
                                                   loader feature is disabled - see @ref SECBOOT_USE_LOCAL_LOADER */
#define SFU_INCORRECT_OB_BLINK_DELAY   (250U) /*!< Blinks every 250ms when an Option Bytes issue is detected */


/**
  * Optional Features Software Configuration
  */
#define SECBOOT_LOADER SECBOOT_USE_NO_LOADER    /*!< Loader selection inside SBSFU : local/standalone/none */

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
