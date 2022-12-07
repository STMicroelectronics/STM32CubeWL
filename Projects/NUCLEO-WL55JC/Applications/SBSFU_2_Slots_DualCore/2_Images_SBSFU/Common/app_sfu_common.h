/**
  ******************************************************************************
  * @file    app_sfu_common.h
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
#ifndef APP_SFU_COMMON_H
#define APP_SFU_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/**
  * Optional Features Software Configuration
  */
#define CM4_SBFU_BOOT_FLAG_ADDRESS M4_M0PLUS_FLAG_RAM_START /*!< address of the M4/M0+ synchronization flag (unused RAM) */
#define SBSFU_NOT_BOOTED           0xFFFF               /*!< M0+ SBSFU hasn't finished its boot */
#define SBSFU_BOOTED               0xAAAA               /*!< M0+ SBSFU boot is over: the M4 UserApp can be started */
#define SBSFU_FW_DOWNLOAD          0x5555               /*!< M0+ SBSFU boot is over: a firmware download is required */

#define SECBOOT_USE_LOCAL_LOADER              (1U) /*!< local loader feature integrated into SBSFU (YMODEM over UART) */
#define SECBOOT_USE_STANDALONE_LOADER         (2U) /*!< standalone loader : see specific loader project  */
#define SECBOOT_USE_NO_LOADER                 (3U) /*!< no loader capability at SBSFU stage */
#define SECBOOT_DUAL_CORE_LOADER              (4U) /*!< dual core loader */

/* Uncomment the define below if you want to use minicom with Linux */
/* #define MINICOM_YMODEM */                       /*!< YMODEM protocol handled by MINICOM (Linux): 128 bytes packets */

#if defined(MINICOM_YMODEM)
/* Minicom does not accept the debug prints during the YMODEM session */
#undef SFU_VERBOSE_DEBUG_MODE
#endif /* MINICOM_YMODEM */

/* Multi-images configuration :
   - Max : 3 Active images and 3 Download areas
   - Not necessary same configuration between SFU_NB_MAX_ACTIVE_IMAGE and SFU_NB_MAX_DWL_AREA
   - Active slot identified with SFU magic (1,2,3) information from header
   - Do not forget to add keys for each image in SE_Corebin/Binary folder
   - Master slot : image started in priority if valid
   - FW image validate all features authorized from master slot
*/
#define SFU_NB_MAX_ACTIVE_IMAGE 2U                        /*!< 2 active images managed */
#define SFU_NB_MAX_DWL_AREA     1U                        /*!< 1 dwl area managed */

/* The define below allows disabling all security IPs at once.
 *
 * Enabled: all security IPs (WRP, watchdog...) are disabled.
 * Disabled: the security IPs can be used (if their specific compiler switches are enabled too).
 *
 */

/*#define SECBOOT_DISABLE_SECURITY_IPS*/  /*!< Disable all security IPs at once when activated */

#if !defined(SECBOOT_DISABLE_SECURITY_IPS)

/* Uncomment the following defines when in Release mode.
   In debug mode it can be better to disable some of the following protection
   for a better Debug experience (WRP, RDP, IWDG, DAP, etc.) */

#define SFU_WRP_PROTECT_ENABLE
#define SFU_DAP_PROTECT_ENABLE         /*!< WARNING: Be Careful if enabling this protection. Debugger will be disconnected.
                                            It might be difficult to reconnect the Debugger.*/
#define SFU_DMA_PROTECT_ENABLE
#define SFU_IWDG_PROTECT_ENABLE    /*!< WARNING:
                                        1. Be Careful if enabling this protection. IWDG will be active also after
                                           switching to UserApp: a refresh is needed.
                                        2. The IWDG reload in the SB_SFU code will have to be tuned depending on your
                                           platform (flash size...)*/
#define SFU_C2_DDS_PROTECT_ENABLE      /*!< Static disabling of the CPU2 debug */
#define SFU_SECURE_USER_PROTECT_ENABLE /*!< Only accessible in Secure access mode,
                                          the Secure user software is stored in the secure user memory, a configurable
                                          protected area which is part of the user main memory. */

/*#define SFU_FINAL_SECURE_LOCK_ENABLE */   /*!< WARNING: Should be enabled at the end of product development and test
                                                 steps.
                                                 When enabling this lock, Static protections cannot be modified any more
                                                 and Debug is finally disabled. */
#endif /* !SECBOOT_DISABLE_SECURITY_IPS */

/**
  * The define below (SECBOOT_OB_DEV_MODE) determines if the OPTION BYTES should be handled in Development mode or not.
  * This define is taken into account only if RDP level 2 is not set.
  * If RDP level 2 is set no modification can be done anyway.
  *
  * Enabled: Option Bytes Development Mode enabled.
  *          SB_SFU uses a "check and apply" strategy when checking the Option Bytes configuration.
  *          If an OB is not set though it should be then this setting is automatically corrected.
  *          This applies only as long as RDP level 2 is not set.
  *
  * Disabled: Option Bytes Development Mode disabled.
  *           In this mode the Option Bytes are supposed to be already configured properly when the software starts for
              the first time.
  *           SB_SFU checks the Option Bytes configuration but does not correct it.
  *           If a problem is detected an error message is reported and the execution stops.
  */
#define SECBOOT_OB_DEV_MODE


#define SFU_IWDG_TIMEOUT  ((uint32_t)15) /*!< IWDG timeout in seconds (the max. value that can be set here depends on
                                              the prescaler settings: IWDG_PRESCALER_XXX. ) */

#if defined(SFU_SECURE_USER_PROTECT_ENABLE)
#define SFU_SECURE_FLASH_START    SLOT_ACTIVE_1_START              /*!< Secure Flash start address: SFSA[6:0] */
#define SFU_SECURE_SRAM1_START    0x1F                             /*!< Secure non-Backup SRAM1 start address:
                                                                        SNBRSA[4:0]. 0x1F configures an entirely
                                                                        non-secure SRAM1. */
#define SFU_SECURE_SRAM2_START    SRAM2_BASE                       /*!< Secure Backup SRAM2 start address: SBRSA[4:0] */
#define SFU_HIDE_PROTECTION_START SE_KEY_REGION_ROM_START          /*!< Hide Protection area start address:
                                                                        HDPSA[6:0] */
#define SFU_HIDE_PROTECTION_CFG OB_SECURE_HIDE_PROTECTION_ENABLE   /*!< Hide Protection area start address:
                                                                        HDPSA[6:0] */

/*!< Secure mode activated or deactivated */
#define SFU_SECURE_MODE           (OB_SECURE_SYSTEM_AND_FLASH_ENABLE | \
                                   SFU_HIDE_PROTECTION_CFG           | \
                                   OB_SECURE_SRAM1_DISABLE           | \
                                   OB_SECURE_SRAM2_ENABLE)
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */



#ifdef __cplusplus
}
#endif

#endif /* APP_SFU_COMMON_H */
