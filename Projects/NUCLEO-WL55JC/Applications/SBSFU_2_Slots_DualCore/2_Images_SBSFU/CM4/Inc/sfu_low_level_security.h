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
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_fwimg_regions.h"
#include "se_def_core.h"


/*!< Bank 1, Area A  used to protect Vector Table */
#define SFU_C2_PROTECT_WRP_AREA            (OB_WRPAREA_BANK1_AREAA)

/*!< First page including the Vector Table: 0 based */
#define SFU_C2_PROTECT_WRP_PAGE_START      ((uint32_t)((SFU_ROM_ADDR_START - FLASH_BASE) / FLASH_PAGE_SIZE))

/*!< Last page: (code_size-1)/page_size because the page
     indexes start from 0 */
#define SFU_C2_PROTECT_WRP_PAGE_END        ((uint32_t)((SFU_ROM_ADDR_END - FLASH_BASE) / FLASH_PAGE_SIZE))

/*!< Bank 1, Area B  used to protect Vector Table */
#define SFU_C1_PROTECT_WRP_AREA            (OB_WRPAREA_BANK1_AREAB)

/*!< First page including the Vector Table: 0 based */
#define SFU_C1_PROTECT_WRP_PAGE_START      ((uint32_t)((SFU_C1_BOOT_BASE_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE))

/*!< Last page: (code_size-1)/page_size because the page
     indexes start from 0 */
#define SFU_C1_PROTECT_WRP_PAGE_END        ((uint32_t)((SFU_C1_AREA_ADDR_END - FLASH_BASE) / FLASH_PAGE_SIZE))

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
  * @brief Region 1 - Enable the read/write operations for full peripheral area (without bit banding region) in
  *                   unprivileged mode.
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
  * @brief Region 2 - Enable the execution for SB Full area in unprivileged mode.
  *                   Read only capability configured
  *                   from 0x08000000 ==> 0x08004FFF (20 Kbytes)
  */
#define SFU_PROTECT_MPU_FLASHEXE_RGNV  MPU_REGION_NUMBER2
#define SFU_PROTECT_MPU_FLASHEXE_START M4_SB_REGION_ROM_START
#define SFU_PROTECT_MPU_FLASHEXE_SIZE  MPU_REGION_SIZE_32KB
#define SFU_PROTECT_MPU_FLASHEXE_SREG  0xE0U               /*!< 32 Kbytes / 8 * 5 ==> 20 Kbytes */
#define SFU_PROTECT_MPU_FLASHEXE_PERM  MPU_REGION_PRIV_RO
#define SFU_PROTECT_MPU_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define SFU_PROTECT_MPU_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 3 - Enable the execution for SB Full area in unprivileged mode.
  *                   Read only capability configured
  *                   In addition to region 2, from 0x08005000 ==> 0x080057FF (2 Kbytes)
  */
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_RGNV  MPU_REGION_NUMBER3
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_START 0x08005000UL
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_SIZE  MPU_REGION_SIZE_2KB
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_PERM  MPU_REGION_PRIV_RO
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_FLASHEXE_ADJUST_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 4 - Forbid all access to the active slot.
  *                   From 0x08018000 ==> 0x0801B7FF (14 kbytes)
  */
#define SFU_PROTECT_MPU_APP_FLASHEXE_RGNV  MPU_REGION_NUMBER4
#define SFU_PROTECT_MPU_APP_FLASHEXE_START 0x08018000UL
#define SFU_PROTECT_MPU_APP_FLASHEXE_SIZE  MPU_REGION_SIZE_16KB
#define SFU_PROTECT_MPU_APP_FLASHEXE_SREG  0x80U           /*!< 16 Kbytes / 8 * 7 ==> 14 Kbytes */
#define SFU_PROTECT_MPU_APP_FLASHEXE_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_APP_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_APP_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_APP_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_APP_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 5 - Forbid all access to the active slot.
  *                   In addition to region 4, from 0x08014800 ==> 0x08017FFF (14 kbytes)
  */
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_RGNV  MPU_REGION_NUMBER5
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_START 0x08014000UL
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_SIZE  MPU_REGION_SIZE_16KB
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_SREG  0x01U    /*!< 16 Kbytes / 8 * 7 ==> 14 Kbytes */
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 6 - Enable the read operation for SBSFU synchronization flag.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_SRAM_FLAG_RGNV  MPU_REGION_NUMBER6
#define SFU_PROTECT_MPU_SRAM_FLAG_START M4_M0PLUS_FLAG_RAM_START /*!< SBSFU synchronization flag */
#define SFU_PROTECT_MPU_SRAM_FLAG_SIZE  MPU_REGION_SIZE_32B
#define SFU_PROTECT_MPU_SRAM_FLAG_SREG  0x00U                /*!< All subregions activated */
#define SFU_PROTECT_MPU_SRAM_FLAG_PERM  MPU_REGION_PRIV_RO_URO
#define SFU_PROTECT_MPU_SRAM_FLAG_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_SRAM_FLAG_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_SRAM_FLAG_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_SRAM_FLAG_C     MPU_ACCESS_CACHEABLE

/**
  * MPU configuration for UserApp execution
  * =======================================
  * Reconfiguration of existing regions (useless during UserApp execution)
  * MPU constraint = Region base address should be aligned on Region size
  */

/**
  * @brief Region 4 - Enable the execution for the active slot in unprivileged mode.
  *                   Read only capability configured
  *                   From 0x08018000 ==> 0x0801B7FF (14 kbytes)
  */
#define APP_PROTECT_MPU_FLASHEXE_RGNV  MPU_REGION_NUMBER4
#define APP_PROTECT_MPU_FLASHEXE_START 0x08018000UL
#define APP_PROTECT_MPU_FLASHEXE_SIZE  MPU_REGION_SIZE_16KB
#define APP_PROTECT_MPU_FLASHEXE_SREG  0x80U           /*!< 16 Kbytes / 8 * 7 ==> 14 Kbytes */
#define APP_PROTECT_MPU_FLASHEXE_PERM  MPU_REGION_PRIV_RO_URO
#define APP_PROTECT_MPU_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define APP_PROTECT_MPU_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define APP_PROTECT_MPU_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define APP_PROTECT_MPU_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 5 - Enable the execution for active slot in unprivileged mode.
  *                   Read only capability configured
  *                   In addition to region 4, from 0x08014800 ==> 0x08017FFF (14 kbytes)
  */
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_RGNV  MPU_REGION_NUMBER5
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_START 0x08014000UL
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_SIZE  MPU_REGION_SIZE_16KB
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_SREG  0x01U    /*!< 16 Kbytes / 8 * 7 ==> 14 Kbytes */
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_PERM  MPU_REGION_PRIV_RO_URO
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_TEXV  MPU_TEX_LEVEL0
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_B     MPU_ACCESS_BUFFERABLE
#define APP_PROTECT_MPU_FLASHEXE_ADJUST_C     MPU_ACCESS_NOT_CACHEABLE


#define SFU_DBG_PORT            GPIOA
#define SFU_DBG_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define SFU_DBG_SWDIO_PIN       GPIO_PIN_13
#define SFU_DBG_SWCLK_PIN       GPIO_PIN_14

#define SFU_PROTECTIONS_NONE                 ((uint32_t)0x00000000U)   /*!< Protection configuration unchanged */
#define SFU_STATIC_PROTECTION_WRP            ((uint32_t)0x00000002U)   /*!< Constants section in Flash. */
#define SFU_RUNTIME_PROTECTION_MPU           ((uint32_t)0x00000100U)   /*!< Shared Info section in Flash */
#define SFU_RUNTIME_PROTECTION_IWDG          ((uint32_t)0x00000400U)   /*!< Independent Watchdog */
#define SFU_RUNTIME_PROTECTION_DAP           ((uint32_t)0x00000800U)   /*!< Debug Access Port control */
#define SFU_RUNTIME_PROTECTION_DMA           ((uint32_t)0x00001000U)   /*!< DMA protection, disable DMAs */

#define SFU_STATIC_PROTECTION_ALL           (SFU_STATIC_PROTECTION_WRP)
/*!< All the static protections */

#define SFU_RUNTIME_PROTECTION_ALL          (SFU_RUNTIME_PROTECTION_MPU  | \
                                             SFU_RUNTIME_PROTECTION_IWDG | SFU_RUNTIME_PROTECTION_DAP  | \
                                             SFU_RUNTIME_PROTECTION_DMA )
/*!< All the run-time protections */

#define SFU_INITIAL_CONFIGURATION           (0x00)     /*!< Initial configuration */
#define SFU_SECOND_CONFIGURATION            (0x01)     /*!< Second configuration */
#define SFU_THIRD_CONFIGURATION             (0x02)     /*!< Third configuration */

SFU_ErrorStatus    SFU_LL_SECU_CheckApplyStaticProtections(void);
SFU_ErrorStatus    SFU_LL_SECU_CheckApplyRuntimeProtections(uint8_t uStep);
#ifdef SFU_MPU_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionMPU(uint8_t uStep);
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionMPU_UserApp(void);
#endif /*SFU_MPU_PROTECT_ENABLE*/
SFU_ErrorStatus SFU_LL_SECU_IWDG_Refresh(void);
SE_ErrorStatus SE_SFU_IMG_Write(SE_StatusTypeDef *pSE_Status, uint8_t *pDestination, const uint8_t *pSource, uint32_t Length);
SE_ErrorStatus SE_SFU_IMG_Read(SE_StatusTypeDef *pSE_Status, uint8_t *pDestination, const uint8_t *pSource, uint32_t Length);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_SECURITY_H */
