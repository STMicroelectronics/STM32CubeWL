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
#if defined(SFU_SECURE_USER_PROTECT_ENABLE)
#define SFU_SECURE_FLASH_START    SLOT_ACTIVE_1_START              /*!< Secure Flash start address: SFSA[6:0] */
#define SFU_SECURE_SRAM1_START    0x1F                             /*!< Secure non-Backup SRAM1 start address:
                                                                        SNBRSA[4:0]. 0x1F configures an entirely
                                                                        non-secure SRAM1. */
#define SFU_SECURE_SRAM2_START    SB_REGION_RAM_START              /*!< Secure Backup SRAM2 start address: SBRSA[4:0] */
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