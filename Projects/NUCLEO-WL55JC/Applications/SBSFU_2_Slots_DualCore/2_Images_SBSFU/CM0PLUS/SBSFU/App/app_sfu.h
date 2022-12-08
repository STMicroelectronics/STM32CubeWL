/**
  ******************************************************************************
  * @file    app_sfu.h
  * @author  MCD Application Team
  * @brief   This file contains the configuration of SBSFU application.
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
#include "se_crypto_config.h"
#include "app_sfu_common.h"

/* Exported constants --------------------------------------------------------*/
/**
  * Use this define to choose the type of Firmware Image Programming you want to use.
  * This version supports only 2 modes:
  *
  * SFU_ENCRYPTED_IMAGE: Encrypted Firmware Image
  *       The image is received in encrypted format.
  *       The image must be decrypted to be installed:
  *         this is done according to the selected crypto scheme,
  *         see @ref SECBOOT_CRYPTO_SCHEME in the SE_CoreBin part.
  *
  * SFU_CLEAR_IMAGE: Clear Firmware Image
  *       The image is received in clear format.
  *       No decrypt operation is needed before installing the image:
  *         the selected crypto scheme must be compatible with this choice,
  *         see @ref SECBOOT_CRYPTO_SCHEME in the SE_CoreBin part.
  *
  *
  */
#if  SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
#define SFU_IMAGE_PROGRAMMING_TYPE SFU_CLEAR_IMAGE
#else
#define SFU_IMAGE_PROGRAMMING_TYPE SFU_ENCRYPTED_IMAGE
#endif /* SECBOOT_CRYPTO_SCHEME */

#define SFU_ENCRYPTED_IMAGE (0U) /*!< The Firmware Image to be installed is downloaded in ENCRYPTED format */
#define SFU_CLEAR_IMAGE     (1U) /*!< The Firmware Image to be installed is downloaded in CLEAR format */

#define SFU_DEBUG_MODE               /*!< Comment this define to optimize memory footprint (debug mode removed)
                                          No more print on terminal during SBSFU execution */

/*#define SFU_VERBOSE_DEBUG_MODE*/   /*!< Uncomment this define when in verbose Debug mode.
                                          this switch activates more debug prints in the console (FSM state info...) */


/*#define SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE*/  /*!< You may uncomment this define when running development tests.
                                                          When this switch is activated, the FWIMG part of SB_SFU will
                                                          block when an abnormal error is encountered */

#if defined(SFU_VERBOSE_DEBUG_MODE) && !defined(SFU_DEBUG_MODE)
#error You cannot activate SFU_VERBOSE_DEBUG_MODE without activating SFU_DEBUG_MODE too.
#endif /* SFU_VERBOSE_DEBUG_MODE && !SFU_DEBUG_MODE */

#if defined(SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE) && !defined(SFU_DEBUG_MODE)
#error SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE is meant to be used in DEBUG mode
#endif /* SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE && !SFU_DEBUG_MODE */

/*#define SFU_TEST_PROTECTION*/       /*!< Auto-test of protections : WRP, PCROP, MPU, FWALL.
                                           Automatically executed @startup */
#if defined(SFU_TEST_PROTECTION)
#undef SFU_DEBUG_MODE                 /*!< Remove definition to optimize memory footprint (debug mode removed) */
#endif /* SFU_TEST_PROTECTION */
/**
  * SB_SFU status LED.
  * The constants below define the LED to be used and the LED blinking frequency to identify some situations.
  * This is useful when no log is enabled.
  *
  * \li The LED blinks every see @ref SFU_COM_YMODEM_DOWNLOAD_TIMEOUT seconds when a local download is waited.
  * \li For the other situations, please check the other defines below.
  */
#define SFU_STATUS_LED (LED_GREEN)            /*!< LED to be used to provide the SB_SFU status to the end-user */
#define SFU_STOP_NO_FW_BLINK_DELAY     (100U) /*!< Blinks every 100ms when no valid firmware is available and the local
                                                   loader feature is disabled - see @ref SECBOOT_USE_LOCAL_LOADER */
#define SFU_INCORRECT_OB_BLINK_DELAY   (250U) /*!< Blinks every 250ms when an Option Bytes issue is detected */

/**
  * Optional Features Software Configuration
  */
#if !defined(SFU_TEST_PROTECTION)
#define SECBOOT_LOADER SECBOOT_DUAL_CORE_LOADER    /*!< Loader selection inside SBSFU : local/standalone/none */
#else
#define SECBOOT_LOADER SECBOOT_USE_NO_LOADER       /*!< No loader usage forced when SFU_TEST_PROTECTION is set */
#endif /* SFU_TEST_PROTECTION */

/* Uncomment the define below if you want to remove the swap area
   ==> partial update is not supported in this configuration
   ==> image validation is not supported in this configuration */
/*#define SFU_NO_SWAP*/                              /*!< FW upgrade installation process without swap area */

/* Multi-images configuration :
   - Max : 3 Active images and 3 Download areas
   - Not necessary same configuration between SFU_NB_MAX_ACTIVE_IMAGE and SFU_NB_MAX_DWL_AREA
   - Active slot identified with SFU magic (1,2,3) information from header
   - Do not forget to add keys for each image in SE_Corebin/Binary folder
   - Master slot : image started in priority if valid
   - FW image validate all features authorized from master slot
*/
#define MASTER_SLOT             SLOT_ACTIVE_1             /*!< SLOT_ACTIVE_1 identified as master slot */


#if !defined(SECBOOT_DISABLE_SECURITY_IPS)

/* Uncomment the following defines when in Release mode.
   In debug mode it can be better to disable some of the following protection
   for a better Debug experience (WRP, RDP, IWDG, DAP, etc.) */

#define SFU_RDP_PROTECT_ENABLE
/*#define SFU_TAMPER_PROTECT_ENABLE */  /*!< WARNING : Tamper protection deactivated. As the tamper tamper pin is
                                             neither connected to GND nor to 5V (floating level), there are too many
                                             spurious tamper event detected */
#define SFU_MPU_PROTECT_ENABLE     /*!< MPU protection:
                                        Enables/Disables the MPU protection.
                                        If Secure Engine isolation is ensured by MPU (see SFU_ISOLATE_SE_WITH_MPU in
                                        SE_CoreBin\Inc\se_low_level.h), then this switch also enables/disables it, in
                                        addition to the overall MPU protection. */
#define SFU_MPU_USERAPP_ACTIVATION /*!< MPU protection during UserApp execution : Only active slot(s) considered as an
                                        executable area */

#if defined(SFU_SECURE_USER_PROTECT_ENABLE)
#define SFU_GTZC_PROTECT_ENABLE    /*!< GTZC protection (dependent on SFU_SECURE_USER_PROTECT_ENABLE):
                                        Enables/Disables the GTZC protection. */
#define SFU_C2SWDBG_PROTECT_ENABLE /*!< Dynamic disabling of the CPU2 debug:
                                        not writable if ESE=0, no meaning if DDS=1. */
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

#if defined(SFU_FINAL_SECURE_LOCK_ENABLE)
#define SFU_PROTECT_RDP_LEVEL  (OB_RDP_LEVEL_2)  /*!< RDP level2 for product on the field. Final OB lock, Debug
                                                      completely disabled, OB update no more possible  */


#else
#define SFU_PROTECT_RDP_LEVEL  (OB_RDP_LEVEL_1)  /*!< RDP level is set as 1 for debugging purposes. A product on the
                                                      field should set it as Level2 */
#endif /* SFU_FINAL_SECURE_LOCK_ENABLE */

#endif /* !SECBOOT_DISABLE_SECURITY_IPS */


/**
  * Application Configuration
  *
  */
#define SFU_FW_VERSION_START_NUM (1U)    /*!< The very first version number a Firmware can have
                                              You can also define an upper bound here if you plan to use it */

#define SFU_FW_VERSION_INIT_NUM (1U)     /*!< The version number accepted when the header is not valid (either because 
                                              no FW installed or due to an attack attempt). Could be different from
                                              SFU_FW_VERSION_START_NUM */

/**
  * Features compatibility control
  */
#if defined(SFU_NO_SWAP) && defined(ENABLE_IMAGE_STATE_HANDLING)
#warning "ENABLE_IMAGE_STATE_HANDLING not compatible with SFU_NO_SWAP process"
#endif
#if defined(SFU_SECURE_USER_PROTECT_ENABLE) && defined(ENABLE_IMAGE_STATE_HANDLING)
#warning "ENABLE_IMAGE_STATE_HANDLING not compatible with SFU_SECURE_USER_PROTECT_ENABLE process"
#endif


#ifdef __cplusplus
}
#endif

#endif /* APP_SFU_H */
