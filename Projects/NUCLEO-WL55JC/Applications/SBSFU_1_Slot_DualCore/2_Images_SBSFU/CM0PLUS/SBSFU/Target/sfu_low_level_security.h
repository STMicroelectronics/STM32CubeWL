/**
  ******************************************************************************
  * @file    sfu_low_level_security.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update security
  *          low level interface.
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
#ifndef SFU_LOW_LEVEL_SECURITY_H
#define SFU_LOW_LEVEL_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sfu_fwimg_regions.h"
#include "sfu_def.h"

/* Exported constants --------------------------------------------------------*/
/* Secure User Memory end : from FLASH_BASE to end of active slot header */
#define SFU_SEC_MEM_AREA_ADDR_END SFU_ROM_ADDR_END


/*!< Bank 1, Area A  used to protect Vector Table */
#define SFU_PROTECT_WRP_AREA_1          (OB_WRPAREA_BANK1_AREAA)

/*!< First page including the Vector Table: 0 based */
#define SFU_PROTECT_WRP_PAGE_START_1    ((uint32_t)((SFU_ROM_ADDR_START - FLASH_BASE) / FLASH_PAGE_SIZE))

/*!< Last page: (code_size-1)/page_size because the page
     indexes start from 0 */
#define SFU_PROTECT_WRP_PAGE_END_1      ((uint32_t)((SFU_ROM_ADDR_END - FLASH_BASE) / FLASH_PAGE_SIZE))



#define SFU_PROTECT_GTZC_EXEC_SE_START INTVECT_START /*!< GTZC Configuration */

/**
  * @brief The regions can overlap, and can be nested. The region 7 has the highest priority
  *    and the region 0 has the lowest one and this governs how overlapping the regions behave.
  *    The priorities are fixed, and cannot be changed.
  */




#define SFU_PROTECT_MPU_MAX_NB_SUBREG           (8U)

/**
  * @brief Region 0 - The Aliases Region and all User Flash & internal RAM. When executing inside SB/SFU by default all
  *                   the Aliases and all the User Flash (used for UserApp1 and UserApp2) area must not be executable,
  *                   but used only to read and write the downloaded code RAM is also not executable.
  *                   From this full Area we'll enable the Execution permission only on the SB/SFU Flash Area.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_AREA_USER_RGNV  MPU_REGION_NUMBER0
#define SFU_PROTECT_MPU_AREA_USER_START ((uint32_t)0x00000000U)
#define SFU_PROTECT_MPU_AREA_USER_SIZE  MPU_REGION_SIZE_1GB  /*!< up to 0x3FFFFFFF */
#define SFU_PROTECT_MPU_AREA_USER_SREG  0x00U      /*!< All subregions activated */
#define SFU_PROTECT_MPU_AREA_USER_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_AREA_USER_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_AREA_USER_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_AREA_USER_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_AREA_USER_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 1 - Enable the read/write operations for full peripheral area in unprivileged mode.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_PERIPH_1_RGNV  MPU_REGION_NUMBER1
#define SFU_PROTECT_MPU_PERIPH_1_START PERIPH_BASE         /*!< Peripheral memory area */
#define SFU_PROTECT_MPU_PERIPH_1_SIZE  MPU_REGION_SIZE_512MB
#define SFU_PROTECT_MPU_PERIPH_1_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_PERIPH_1_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_PERIPH_1_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_PERIPH_1_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_PERIPH_1_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_PERIPH_1_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 2 - Enable the execution for SB/SFU Full area (SBSFU + SE + Keys) in unprivileged mode.
  *                   Read only capability configured
  *                   Inner region inside the Region 0
  *                   From 0x0802C000 ==> 0x0803FFFF
  */
#define SFU_PROTECT_MPU_FLASHEXE_RGNV  MPU_REGION_NUMBER2
#define SFU_PROTECT_MPU_FLASHEXE_START 0x08020000UL
#define SFU_PROTECT_MPU_FLASHEXE_SIZE  MPU_REGION_SIZE_128KB
#define SFU_PROTECT_MPU_FLASHEXE_SREG  0x07U               /*!< 128 Kbytes / 8 * 5 ==> 80 Kbytes */
#define SFU_PROTECT_MPU_FLASHEXE_PERM  MPU_REGION_PRIV_RO_URO
#define SFU_PROTECT_MPU_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define SFU_PROTECT_MPU_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 3 - Enable the read/write operation in privileged mode for Header of active slots
  *                   Execution capability disabled
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_HEADER_RGNV  MPU_REGION_NUMBER3
#define SFU_PROTECT_MPU_HEADER_START SLOT_ACTIVE_2_HEADER
#define SFU_PROTECT_MPU_HEADER_SREG  0x00U                 /*!< All subregions activated */
#define SFU_PROTECT_MPU_HEADER_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_HEADER_PERM  MPU_REGION_PRIV_RW
#define SFU_PROTECT_MPU_HEADER_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_HEADER_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_HEADER_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_HEADER_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 4 - Enable the read/write operation in privileged mode for KMS_DataStorage
  *                   Execution capability disabled
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_KMS_1_RGNV  MPU_REGION_NUMBER4
#define SFU_PROTECT_MPU_KMS_1_START 0x0802A000UL
#define SFU_PROTECT_MPU_KMS_1_SREG  0x03U               /*!< 8 Kbytes / 8 * 6 ==> 6 Kbytes */
#define SFU_PROTECT_MPU_KMS_1_SIZE  MPU_REGION_SIZE_8KB
#define SFU_PROTECT_MPU_KMS_1_PERM  MPU_REGION_PRIV_RW
#define SFU_PROTECT_MPU_KMS_1_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_KMS_1_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_KMS_1_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_KMS_1_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 5 - Enable the read/write operation in privileged mode for KMS_DataStorage
  *                   Execution capability disabled
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_KMS_2_RGNV  MPU_REGION_NUMBER5
#define SFU_PROTECT_MPU_KMS_2_START 0x0802C000UL
#define SFU_PROTECT_MPU_KMS_2_SREG  0xFCU               /*!< 8 Kbytes / 8 * 2 ==> 2 Kbytes */
#define SFU_PROTECT_MPU_KMS_2_SIZE  MPU_REGION_SIZE_8KB
#define SFU_PROTECT_MPU_KMS_2_PERM  MPU_REGION_PRIV_RW
#define SFU_PROTECT_MPU_KMS_2_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_KMS_2_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_KMS_2_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_KMS_2_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 7 - Forbid all access to the DMA IPs
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_DMA_RGNV  MPU_REGION_NUMBER7
#define SFU_PROTECT_MPU_DMA_START 0x40020000UL
#define SFU_PROTECT_MPU_DMA_SREG  0xC0U               /*!< 4 Kbytes / 8 * 6 ==> 3 Kbytes */
#define SFU_PROTECT_MPU_DMA_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_DMA_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_DMA_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_DMA_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_DMA_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_DMA_C     MPU_ACCESS_NOT_CACHEABLE


/**
  */

/**
  * MPU configuration for UserApp execution
  * =======================================
  * Reconfiguration of existing regions (useless during UserApp execution)
  * MPU constraint = Region base address should be aligned on Region size
  */

/**
  * @brief Region 2 - Enable the execution for the SBSFU and active slots in unprivileged mode.
  *                   Read only capability configured
  *                   Execution capability enabled
  *                   Execution capability restricted by other regions (secure)
  *                   From 0x08000000 ==> 0x0803FFFF (256 kbytes)
  */
#define APP_PROTECT_MPU_FLASHEXE_RGNV  MPU_REGION_NUMBER2
#define APP_PROTECT_MPU_FLASHEXE_START FLASH_BASE
#define APP_PROTECT_MPU_FLASHEXE_SIZE  MPU_REGION_SIZE_256KB
#define APP_PROTECT_MPU_FLASHEXE_SREG  0x00U               /*!< All subregions activated */
#define APP_PROTECT_MPU_FLASHEXE_PERM  MPU_REGION_PRIV_RO_URO
#define APP_PROTECT_MPU_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define APP_PROTECT_MPU_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define APP_PROTECT_MPU_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define APP_PROTECT_MPU_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 3 - Enable the read/write operations for the blob download slots in unprivileged mode.
  *                   Read/write capability configured
  *                   Execution capability disabled
  *                   From 0x08002800U ==>  0x08002FFF (2 kbytes)
  */
#define APP_PROTECT_MPU_BLOB_DWL_RGNV  MPU_REGION_NUMBER3
#define APP_PROTECT_MPU_BLOB_DWL_START SLOT_BLOB_DWL_1_START
#define APP_PROTECT_MPU_BLOB_DWL_SIZE  MPU_REGION_SIZE_2KB
#define APP_PROTECT_MPU_BLOB_DWL_SREG  0x00U               /*!< All subregions activated */
#define APP_PROTECT_MPU_BLOB_DWL_PERM  MPU_REGION_FULL_ACCESS
#define APP_PROTECT_MPU_BLOB_DWL_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define APP_PROTECT_MPU_BLOB_DWL_TEXV  MPU_TEX_LEVEL0
#define APP_PROTECT_MPU_BLOB_DWL_B     MPU_ACCESS_BUFFERABLE
#define APP_PROTECT_MPU_BLOB_DWL_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 6 - Enable the read operation for SBSFU synchronization flag.
  *                   Execution capability disabled
  */
#define APP_PROTECT_MPU_SRAM_FLAG_RGNV  MPU_REGION_NUMBER6
#define APP_PROTECT_MPU_SRAM_FLAG_START 0x20000C00UL
#define APP_PROTECT_MPU_SRAM_FLAG_SIZE  MPU_REGION_SIZE_256B
#define APP_PROTECT_MPU_SRAM_FLAG_SREG  0x7FU                /*!< 256 bytes / 8 * 1 ==> 32 bytes */
#define APP_PROTECT_MPU_SRAM_FLAG_PERM  MPU_REGION_PRIV_RO_URO
#define APP_PROTECT_MPU_SRAM_FLAG_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define APP_PROTECT_MPU_SRAM_FLAG_TEXV  MPU_TEX_LEVEL0
#define APP_PROTECT_MPU_SRAM_FLAG_B     MPU_ACCESS_NOT_BUFFERABLE
#define APP_PROTECT_MPU_SRAM_FLAG_C     MPU_ACCESS_CACHEABLE



/**
  * @}
  */

/** @defgroup SFU_CONFIG_TAMPER Tamper Configuration
  * @{
  */
#define TAMPER_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define RTC_TAMPER_ID                      RTC_TAMPER_1
#define RTC_TAMPER_ID_INTERRUPT            RTC_IT_TAMP_1

/**
  * @}
  */

/** @defgroup SFU_CONFIG_DBG Debug Port Configuration
  * @{
  */
#define SFU_DBG_PORT            GPIOA
#define SFU_DBG_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define SFU_DBG_SWDIO_PIN       GPIO_PIN_13
#define SFU_DBG_SWCLK_PIN       GPIO_PIN_14


/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup SFU_SECURITY_Exported_Constants_Wakeup FU WAKEUP ID Type definition
  * @{
  */
typedef enum
{
  SFU_RESET_UNKNOWN = 0x00U,
  SFU_RESET_WDG_RESET,
  SFU_RESET_LOW_POWER,
  SFU_RESET_HW_RESET,
  SFU_RESET_BOR_RESET,
  SFU_RESET_SW_RESET,
  SFU_RESET_OB_LOADER,
} SFU_RESET_IdTypeDef;

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Constants_Protections FU SECURITY Protections_Constants
  * @{
  */
#define SFU_PROTECTIONS_NONE                 ((uint32_t)0x00000000U)   /*!< Protection configuration unchanged */
#define SFU_STATIC_PROTECTION_RDP            ((uint32_t)0x00000001U)   /*!< RDP protection level 1 is applied */
#define SFU_STATIC_PROTECTION_WRP            ((uint32_t)0x00000002U)   /*!< Constants section in Flash. Needed as
                                                                            separate section to support PCRoP */
#define SFU_STATIC_PROTECTION_PCROP          ((uint32_t)0x00000004U)   /*!< SFU App section in Flash */
#define SFU_STATIC_PROTECTION_LOCKED         ((uint32_t)0x00000008U)   /*!< RDP Level2 is applied. The device is Locked!
                                                                            Std Protections cannot be
                                                                            added/removed/modified */
#define SFU_STATIC_PROTECTION_BFB2           ((uint32_t)0x00000010U)   /*!< BFB2 is disabled. The device shall always
                                                                            boot in bank1! */

#define SFU_RUNTIME_PROTECTION_MPU           ((uint32_t)0x00000100U)   /*!< Shared Info section in Flash */
#define SFU_RUNTIME_PROTECTION_IWDG          ((uint32_t)0x00000400U)   /*!< Independent Watchdog */
#define SFU_RUNTIME_PROTECTION_DAP           ((uint32_t)0x00000800U)   /*!< Debug Access Port control */
#define SFU_RUNTIME_PROTECTION_DMA           ((uint32_t)0x00001000U)   /*!< DMA protection, disable DMAs */
#define SFU_RUNTIME_PROTECTION_ANTI_TAMPER   ((uint32_t)0x00002000U)   /*!< Anti-Tampering protections */
#define SFU_RUNTIME_PROTECTION_CLOCK_MONITOR ((uint32_t)0x00004000U)   /*!< Activate a clock monitoring */
#define SFU_RUNTIME_PROTECTION_TEMP_MONITOR  ((uint32_t)0x00008000U)   /*!< Activate a Temperature monitoring */
#define SFU_RUNTIME_PROTECTION_GTZC          ((uint32_t)0x00010000U)   /*!< GTZC protection */
#define SFU_RUNTIME_PROTECTION_C2SWDBG       ((uint32_t)0x00020000U)   /*!< CPU2 Software Debug */

#define SFU_STATIC_PROTECTION_ALL           (SFU_STATIC_PROTECTION_RDP   | SFU_STATIC_PROTECTION_WRP   | \
                                             SFU_STATIC_PROTECTION_PCROP | SFU_STATIC_PROTECTION_LOCKED)
/*!< All the static protections */

#define SFU_RUNTIME_PROTECTION_ALL          (SFU_RUNTIME_PROTECTION_MPU  | SFU_RUNTIME_PROTECTION_FWALL | \
                                             SFU_RUNTIME_PROTECTION_IWDG | SFU_RUNTIME_PROTECTION_DAP   | \
                                             SFU_RUNTIME_PROTECTION_DMA  | SFU_RUNTIME_PROTECTION_ANTI_TAMPER  | \
                                             SFU_RUNTIME_PROTECTION_CLOCK_MONITOR | \
                                             SFU_RUNTIME_PROTECTION_TEMP_MONITOR  | \
                                             SFU_RUNTIME_PROTECTION_GTZC          | \
                                             SFU_RUNTIME_PROTECTION_C2SWDBG)
/*!< All the run-time protections */

#define SFU_INITIAL_CONFIGURATION           (0x00U)     /*!< Initial configuration */
#define SFU_SECOND_CONFIGURATION            (0x01U)     /*!< Second configuration */
#define SFU_THIRD_CONFIGURATION             (0x02U)     /*!< Third configuration */

/* Exported functions ------------------------------------------------------- */
#define SFU_CALLBACK_ANTITAMPER HAL_RTCEx_Tamper1EventCallback
/*!<SFU Redirect of RTC Tamper Event Callback*/
#define SFU_CALLBACK_MEMORYFAULT(void) MemManage_Handler(void)  /*!<SFU Redirect of Mem Manage Callback*/
#ifdef SFU_GTZC_PROTECT_ENABLE
#define SFU_CALLBACK_GTZC_TZIC(void) HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
#endif /* SFU_GTZC_PROTECT_ENABLE */

SFU_ErrorStatus    SFU_LL_SECU_IWDG_Refresh(void);
SFU_ErrorStatus    SFU_LL_SECU_CheckApplyStaticProtections(void);
SFU_ErrorStatus    SFU_LL_SECU_CheckApplyRuntimeProtections(uint8_t uStep);
void               SFU_LL_SECU_GetResetSources(SFU_RESET_IdTypeDef *peResetpSourceId);
void               SFU_LL_SECU_ClearResetSources(void);
#ifdef SFU_MPU_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionMPU(uint8_t uStep);
#endif /*SFU_MPU_PROTECT_ENABLE*/
#ifdef SFU_DMA_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionDMA(void);
#endif /*SFU_DMA_PROTECT_ENABLE*/
#ifdef SFU_DAP_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionDAP(void);
#endif /*SFU_DAP_PROTECT_ENABLE*/
#ifdef SFU_TAMPER_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionANTI_TAMPER(void);
#endif /*SFU_DAP_PROTECT_ENABLE*/
#if defined(SFU_MPU_PROTECT_ENABLE)
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionMPU_UserApp(void);
#endif /* SFU_MPU_PROTECT_ENABLE */
#ifdef SFU_GTZC_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_CheckProtectionGTZC(void);
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_SECURITY_H */
