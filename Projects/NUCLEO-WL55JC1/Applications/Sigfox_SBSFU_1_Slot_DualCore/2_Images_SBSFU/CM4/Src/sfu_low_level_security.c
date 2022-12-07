/**
  ******************************************************************************
  * @file    sfu_low_level_security.c
  * @author  MCD Application Team
  * @brief   SB Security Low Level Interface module
  *          This file provides set of firmware functions to manage SB security
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level_security.h"
#include "sfu_boot.h"

#ifndef  SFU_WRP_PROTECT_ENABLE
#warning "SFU_WRP_PROTECT_DISABLED"
#endif /* SFU_WRP_PROTECT_ENABLE */

#ifndef  SFU_MPU_PROTECT_ENABLE
#warning "SFU_MPU_PROTECT_DISABLED"
#endif /* SFU_MPU_PROTECT_ENABLE */

#ifndef  SFU_C2_DDS_PROTECT_ENABLE
#warning "SFU_C2_DDS_PROTECT_DISABLED"
#endif /* SFU_C2_DDS_PROTECT_ENABLE */

#ifndef  SFU_DAP_PROTECT_ENABLE
#warning "SFU_DAP_PROTECT_DISABLED"
#endif /* SFU_DAP_PROTECT_ENABLE */

#ifndef  SFU_DMA_PROTECT_ENABLE
#warning "SFU_DMA_PROTECT_DISABLED"
#endif /* SFU_DMA_PROTECT_ENABLE */


#ifndef  SFU_SECURE_USER_PROTECT_ENABLE
#warning "SFU_SECURE_USER_PROTECT_DISABLED"
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  SFU_FALSE = 0U,
  SFU_TRUE = !SFU_FALSE
} SFU_BoolTypeDef;

typedef struct
{
  uint8_t                Number;            /*!< Specifies the number of the region to protect. This parameter can be a
                                                 value of CORTEX_MPU_Region_Number */
  uint32_t               BaseAddress;       /*!< Specifies the base address of the region to protect. */
  uint8_t                Size;              /*!< Specifies the size of the region to protect. */
  uint8_t                AccessPermission;  /*!< Specifies the region access permission type. This parameter can be a
                                                 value of CORTEX_MPU_Region_Permission_Attributes */
  uint8_t                DisableExec;       /*!< Specifies the instruction access status. This parameter can be a value
                                                 of  CORTEX_MPU_Instruction_Access */
  uint8_t                SubRegionDisable;  /*!< Specifies the sub region field (region is divided in 8 slices) when bit
                                                 is 1 region sub region is disabled */
  uint8_t                Tex;               /*!< Specifies the tex value  */
  uint8_t                Cacheable;         /*!< Specifies the cacheable value  */
  uint8_t                Bufferable;        /*!< Specifies the cacheable value  */
} SFU_MPU_InitTypeDef;

typedef uint32_t      SFU_ProtectionTypeDef;  /*!<   SFU HAL IF Protection Type Def*/

/* Private variables ---------------------------------------------------------*/


#ifdef  SFU_MPU_PROTECT_ENABLE
static SFU_MPU_InitTypeDef MpuAreas[] =
{
  /* Aliases Region and all User Flash & internal RAM */
  {
    SFU_PROTECT_MPU_AREA_USER_RGNV, SFU_PROTECT_MPU_AREA_USER_START, SFU_PROTECT_MPU_AREA_USER_SIZE,
    SFU_PROTECT_MPU_AREA_USER_PERM, SFU_PROTECT_MPU_AREA_USER_EXECV, SFU_PROTECT_MPU_AREA_USER_SREG,
    SFU_PROTECT_MPU_AREA_USER_TEXV, SFU_PROTECT_MPU_AREA_USER_C,     SFU_PROTECT_MPU_AREA_USER_B
  },
  /* SRAM flag access */
  {
    SFU_PROTECT_MPU_SRAM_FLAG_RGNV, SFU_PROTECT_MPU_SRAM_FLAG_START, SFU_PROTECT_MPU_SRAM_FLAG_SIZE,
    SFU_PROTECT_MPU_SRAM_FLAG_PERM, SFU_PROTECT_MPU_SRAM_FLAG_EXECV, SFU_PROTECT_MPU_SRAM_FLAG_SREG,
    SFU_PROTECT_MPU_SRAM_FLAG_TEXV, SFU_PROTECT_MPU_SRAM_FLAG_C, SFU_PROTECT_MPU_SRAM_FLAG_B
  },
  /*  Flash execution */
  {
    SFU_PROTECT_MPU_FLASHEXE_RGNV, SFU_PROTECT_MPU_FLASHEXE_START, SFU_PROTECT_MPU_FLASHEXE_SIZE,
    SFU_PROTECT_MPU_FLASHEXE_PERM, SFU_PROTECT_MPU_FLASHEXE_EXECV, SFU_PROTECT_MPU_FLASHEXE_SREG,
    SFU_PROTECT_MPU_FLASHEXE_TEXV, SFU_PROTECT_MPU_FLASHEXE_C, SFU_PROTECT_MPU_FLASHEXE_B
  },
  /*  App Access */
  {
    SFU_PROTECT_MPU_APP_FLASHEXE_RGNV, SFU_PROTECT_MPU_APP_FLASHEXE_START, SFU_PROTECT_MPU_APP_FLASHEXE_SIZE,
    SFU_PROTECT_MPU_APP_FLASHEXE_PERM, SFU_PROTECT_MPU_APP_FLASHEXE_EXECV, SFU_PROTECT_MPU_APP_FLASHEXE_SREG,
    SFU_PROTECT_MPU_APP_FLASHEXE_TEXV, SFU_PROTECT_MPU_APP_FLASHEXE_C, SFU_PROTECT_MPU_APP_FLASHEXE_B
  },
  /*  App Access */
  {
    SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_RGNV, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_START, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_SIZE,
    SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_PERM, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_EXECV, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_SREG,
    SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_TEXV, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_C, SFU_PROTECT_MPU_APP_FLASHEXE_ADJUST_B
  },
  /*  peripheral  */
  {
    SFU_PROTECT_MPU_PERIPH_1_RGNV, SFU_PROTECT_MPU_PERIPH_1_START, SFU_PROTECT_MPU_PERIPH_1_SIZE,
    SFU_PROTECT_MPU_PERIPH_1_PERM, SFU_PROTECT_MPU_PERIPH_1_EXECV, SFU_PROTECT_MPU_PERIPH_1_SREG,
    SFU_PROTECT_MPU_PERIPH_1_TEXV, SFU_PROTECT_MPU_PERIPH_1_C, SFU_PROTECT_MPU_PERIPH_1_B
  }
};
#endif /* SFU_MPU_PROTECT_ENABLE */

/* Private function prototypes -----------------------------------------------*/
static SFU_ErrorStatus SFU_LL_SECU_CheckC2BootConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetC2BootConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                          SFU_BoolTypeDef *pbIsProtectionToBeApplied);

static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionBootLock(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionBootLock(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                         SFU_BoolTypeDef *pbIsProtectionToBeApplied);


#ifdef SFU_WRP_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionWRP(FLASH_OBProgramInitTypeDef *psSbFlashOptionBytes,
                                                      FLASH_OBProgramInitTypeDef *psSfuFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionWRP(FLASH_OBProgramInitTypeDef *psSbFlashOptionBytes,
                                                    FLASH_OBProgramInitTypeDef *psSfuFlashOptionBytes,
                                                    SFU_BoolTypeDef *pbIsProtectionToBeApplied);
#endif /*SFU_WRP_PROTECT_ENABLE*/

#ifdef SFU_C2_DDS_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                    SFU_BoolTypeDef *pbIsProtectionToBeApplied);

#endif /* SFU_C2_DDS_PROTECT_ENABLE */

#ifdef SFU_DAP_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionDAP(void);
#endif /*SFU_DAP_PROTECT_ENABLE*/

#ifdef SFU_DMA_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionDMA(void);
#endif /*SFU_DMA_PROTECT_ENABLE*/


#ifdef  SFU_SECURE_USER_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                        SFU_BoolTypeDef *pbIsProtectionToBeApplied);
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

/* Functions Definition : helper ---------------------------------------------*/
#ifdef SFU_MPU_PROTECT_ENABLE
/**
  * @brief  Check MPU configuration
  * @param  MPU_InitStruct Configuration to be checked
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus MPU_CheckConfig(MPU_Region_InitTypeDef *pMPUInitStruct)
{
  uint32_t mpu_rasr = 0UL;

  /* Set the Region number */
  MPU->RNR = pMPUInitStruct->Number;

  mpu_rasr |= (((uint32_t)pMPUInitStruct->DisableExec        << MPU_RASR_XN_Pos) & MPU_RASR_XN_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->AccessPermission   << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->TypeExtField       << MPU_RASR_TEX_Pos) & MPU_RASR_TEX_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->IsShareable        << MPU_RASR_S_Pos) & MPU_RASR_S_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->IsCacheable        << MPU_RASR_C_Pos) & MPU_RASR_C_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->IsBufferable       << MPU_RASR_B_Pos) & MPU_RASR_B_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->SubRegionDisable   << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->Size               << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk);
  mpu_rasr |= (((uint32_t)pMPUInitStruct->Enable             << MPU_RASR_ENABLE_Pos) & MPU_RASR_ENABLE_Msk);

  if (((MPU->RBAR & MPU_RBAR_ADDR_Msk) == pMPUInitStruct->BaseAddress) && (MPU->RASR == mpu_rasr))
  {
    return SFU_SUCCESS;
  }
  else
  {
    return SFU_ERROR;
  }
}
#endif /* SFU_MPU_PROTECT_ENABLE */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Check and if not applied apply the Static security  protections to
  *         all the SfuEn Sections in Flash: RDP, WRP. Static security protections
  *         those protections not impacted by a Reset. They are set using the Option Bytes
  *         When the device is locked (RDP Level2), these protections cannot be changed anymore
  * @param  None
  * @note   By default, the best security protections are applied to the different
  *         flash sections in order to maximize the security level for the specific MCU.
  * @retval uint32_t CRC (returned value is the combination of all the applied protections.
  *         If different from SFU_STD_PROTECTION_ALL, 1 or more protections cannot be applied)
  */
SFU_ErrorStatus SFU_LL_SECU_CheckApplyStaticProtections(void)
{
  FLASH_OBProgramInitTypeDef flash_option_bytes;
  FLASH_OBProgramInitTypeDef sfu_flash_option_bytes;
  SFU_BoolTypeDef is_protection_to_be_applied = SFU_FALSE;
  SFU_BoolTypeDef is_current_protection_to_be_applied = SFU_FALSE;
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  /* Get Option Bytes status for WRP AREA  **********/
  flash_option_bytes.WRPArea     = SFU_C1_PROTECT_WRP_AREA;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes);
  sfu_flash_option_bytes.WRPArea = SFU_C2_PROTECT_WRP_AREA;
  HAL_FLASHEx_OBGetConfig(&sfu_flash_option_bytes);

  /* Check/Apply RDP_Level 1. This is the minimum protection allowed */
  /* if RDP_Level 2 is already applied it's not possible to modify the OptionBytes anymore */
  if (flash_option_bytes.RDPLevel == OB_RDP_LEVEL_2)
  {

    /* Sanity check of the (enabled) static protections */
    if (SFU_LL_SECU_CheckC2BootConfiguration(&flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }

    if (SFU_LL_SECU_CheckProtectionBootLock(&flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }

#ifdef SFU_WRP_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionWRP(&flash_option_bytes, &sfu_flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif /* SFU_WRP_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionDDS(&flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif  /* SFU_C2_DDS_PROTECT_ENABLE */

#ifdef SFU_SECURE_USER_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionSecUser(&flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif  /* SFU_SECURE_USER_PROTECT_ENABLE */

    /*RDP level 2 ==> Flow control by-passed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RDP, FLOW_CTRL_RDP);
  }
  else
  {
    /* Check/Set C2 Boot configuration **********************************************/
    if (e_ret_status == SFU_SUCCESS)
    {
      is_current_protection_to_be_applied = SFU_FALSE;
      e_ret_status = SFU_LL_SECU_SetC2BootConfiguration(&flash_option_bytes, &is_current_protection_to_be_applied);
      if ((e_ret_status == SFU_SUCCESS) && (is_current_protection_to_be_applied))
      {
        /* Generate a first system Reset to reload the new option byte values *************/
        /* C2OPT and SBRV cannot be written if C2BOOT_LOCK = 1 */
        /* WARNING: This means that if a protection can't be set, there will be a reset loop! */
        HAL_FLASH_OB_Launch();
      }
    }

    /* Check/Set Boot Lock configuration *******************************************/
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_LL_SECU_SetProtectionBootLock(&flash_option_bytes, &is_protection_to_be_applied);
    }

    /* Check/Apply WRP ********************************************************/
#ifdef SFU_WRP_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_LL_SECU_SetProtectionWRP(&flash_option_bytes, &sfu_flash_option_bytes,
                                                  &is_protection_to_be_applied);
    }
#endif /* SFU_WRP_PROTECT_ENABLE */

    /* Check/Apply C2 DDS *****************************************************/
#ifdef SFU_C2_DDS_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      is_current_protection_to_be_applied = SFU_FALSE;
      e_ret_status = SFU_LL_SECU_SetProtectionDDS(&flash_option_bytes, &is_current_protection_to_be_applied);
      if ((e_ret_status == SFU_SUCCESS) && (is_current_protection_to_be_applied))
      {
        /* Generate a first system Reset to reload the new option byte values *************/
        /* DDS cannot be written if ESE = 1 */
        /* WARNING: This means that if a protection can't be set, there will be a reset loop! */
        HAL_FLASH_OB_Launch();
      }
    }
#endif  /* SFU_C2_DDS_PROTECT_ENABLE */

    /* Check/Apply Secure User Memory *****************************************/
#ifdef SFU_SECURE_USER_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_LL_SECU_SetProtectionSecUser(&flash_option_bytes, &is_protection_to_be_applied);
    }
#endif  /* SFU_SECURE_USER_PROTECT_ENABLE */

    if (e_ret_status == SFU_SUCCESS)
    {
      if (is_protection_to_be_applied)
      {
        /* Generate System Reset to reload the new option byte values *************/
        /* WARNING: This means that if a protection can't be set, there will be a reset loop! */
        HAL_FLASH_OB_Launch();
      }
    }
  }


  /* Lock the Options Bytes ***************************************************/
  HAL_FLASH_OB_Lock();

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* If it was not possible to apply one of the above mandatory protections, the
  Option bytes have not been reloaded. Return the error status in order for the
  caller function to take the right actions */
  return e_ret_status;

}


/**
  * @brief  Apply Runtime security  protections.
  *         Runtime security protections have to be re-configured at each Reset.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckApplyRuntimeProtections(uint8_t uStep)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  SFU_ProtectionTypeDef runtime_protection = SFU_PROTECTIONS_NONE;
  SFU_ProtectionTypeDef expected_runtime_protection = SFU_PROTECTIONS_NONE;

  /* Depending on compilation switches, prevent compilation warning */
  UNUSED(runtime_protection);

#ifdef SFU_MPU_PROTECT_ENABLE
  expected_runtime_protection |= SFU_RUNTIME_PROTECTION_MPU;
  if (SFU_LL_SECU_SetProtectionMPU(uStep) == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_MPU;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
#endif /* SFU_MPU_PROTECT_ENABLE */


  /* Check/Apply disable DMAs  ************************************************/
#ifdef SFU_DMA_PROTECT_ENABLE
  expected_runtime_protection |= SFU_RUNTIME_PROTECTION_DMA;
  if (SFU_LL_SECU_SetProtectionDMA() == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_DMA;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
#endif /* SFU_DMA_PROTECT_ENABLE */

  /* Check/Apply  DAP *********************************************************/
#ifdef SFU_DAP_PROTECT_ENABLE
  expected_runtime_protection |= SFU_RUNTIME_PROTECTION_DAP;
  if (SFU_LL_SECU_SetProtectionDAP() == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_DAP;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
#endif /* SFU_DAP_PROTECT_ENABLE */

  if (runtime_protection != expected_runtime_protection)
  {
    e_ret_status = SFU_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Check C2 boot configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckC2BootConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply C2OPT and SBRV ***********************************************/
  if ((psFlashOptionBytes->C2BootRegion == OB_C2_BOOT_FROM_FLASH)&&
      (psFlashOptionBytes->C2SecureBootVectAddr == SFU_C2_BOOT_BASE_ADDR))
  {
    e_ret_status = SFU_SUCCESS;
  }
  /* Execution stopped if flow control failed */
  if (e_ret_status == SFU_SUCCESS)
  {
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2BOOT, FLOW_CTRL_C2BOOT);
  }
  return e_ret_status;
}


/**
  * @brief  Apply C2 boot configuration.
  * @note   C2OPT and SBRV can only be accessed by software when C2BOOT_LOCK = 0.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval If Protection is not set, RSS procedure is called with a System Reset
  */
SFU_ErrorStatus SFU_LL_SECU_SetC2BootConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                   SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check if Secure User Memory is already configured */
  if (SFU_LL_SECU_CheckC2BootConfiguration(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    /* Apply the proper configuration if we are in development mode */
#if defined(SECBOOT_OB_DEV_MODE)
    /* Set the security bit if needed */
    /* ------------------------------ */
    psFlashOptionBytes->OptionType = OPTIONBYTE_C2_BOOT_VECT;
    psFlashOptionBytes->C2BootRegion            = OB_C2_BOOT_FROM_FLASH;
    psFlashOptionBytes->C2SecureBootVectAddr    = SFU_C2_BOOT_BASE_ADDR;

    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2BOOT, FLOW_CTRL_C2BOOT);
    }
#else
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}

/**
  * @brief  Check Boot Lock configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @retval SFU_ErrorStatus SFU_SUCCESS if Flash configuration is correct, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionBootLock(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply C2BOOT_LOCK, BOOT_LOCK ***************************************/
  if (((psFlashOptionBytes->UserConfig & FLASH_OPTR_BOOT_LOCK) == OB_BOOT_LOCK_ENABLE)&&
      ((psFlashOptionBytes->UserConfig & FLASH_OPTR_C2BOOT_LOCK) == OB_C2BOOT_LOCK_ENABLE))
  {
    e_ret_status = SFU_SUCCESS;
  }
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_UBE, FLOW_CTRL_UBE);
  }
  return e_ret_status;

}

/**
  * @brief  Set Boot Lock configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionBootLock(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                  SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check Boot Lock configuration */
  if (SFU_LL_SECU_CheckProtectionBootLock(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
#if defined(SECBOOT_OB_DEV_MODE)
    psFlashOptionBytes->OptionType      = OPTIONBYTE_USER;
    psFlashOptionBytes->UserConfig      &= ~(OB_BOOT_LOCK_ENABLE | OB_C2BOOT_LOCK_ENABLE);
    psFlashOptionBytes->UserConfig      |= OB_BOOT_LOCK_ENABLE;
    psFlashOptionBytes->UserConfig      |= OB_C2BOOT_LOCK_ENABLE;
    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_UBE, FLOW_CTRL_UBE);
    }
#else
    /* Boot Configuration issue: booting Main flash through PA14-BOOT0 pin functionality cannot be guaranteed */
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}

#ifdef SFU_WRP_PROTECT_ENABLE
/**
  * @brief  Check the WRP protection to the specified Area. It includes the SB Vector Table
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionWRP(FLASH_OBProgramInitTypeDef *psSbFlashOptionBytes,
                                               FLASH_OBProgramInitTypeDef *psSfuFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check WRP ****************************************************************/
  /* The UserApp needs to know this configuration in case it wants to use the same WRP Areas */
  if ((psSbFlashOptionBytes->WRPStartOffset == SFU_C1_PROTECT_WRP_PAGE_START) ||
      (psSbFlashOptionBytes->WRPEndOffset   == SFU_C1_PROTECT_WRP_PAGE_END))
  {
    e_ret_status = SFU_SUCCESS; /*Protection applied */
  }
  if ((psSfuFlashOptionBytes->WRPStartOffset == SFU_C2_PROTECT_WRP_PAGE_START) ||
      (psSfuFlashOptionBytes->WRPEndOffset   == SFU_C2_PROTECT_WRP_PAGE_END))
  {
    e_ret_status = SFU_SUCCESS; /*Protection applied */
  }
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_WRP, FLOW_CTRL_WRP);
  }
  return e_ret_status;
}

/**
  * @brief  Apply the WRP protection to the specified Area. It includes the SB Vector Table
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionWRP(FLASH_OBProgramInitTypeDef *psSbFlashOptionBytes,
                                             FLASH_OBProgramInitTypeDef *psSfuFlashOptionBytes,
                                             SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply WRP **********************************************************/
  /* The UserApp needs to know this configuration in case it wants to use the same WRP Areas */
  if (SFU_LL_SECU_CheckProtectionWRP(psSbFlashOptionBytes, psSfuFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS; /*Protection already applied */
  }
  else
  {
#if defined(SECBOOT_OB_DEV_MODE)
    psSbFlashOptionBytes->OptionType      = OPTIONBYTE_WRP;
    psSbFlashOptionBytes->WRPArea         = SFU_C1_PROTECT_WRP_AREA;
    psSbFlashOptionBytes->WRPStartOffset  = SFU_C1_PROTECT_WRP_PAGE_START;
    psSbFlashOptionBytes->WRPEndOffset    = SFU_C1_PROTECT_WRP_PAGE_END;

    if (HAL_FLASHEx_OBProgram(psSbFlashOptionBytes) == HAL_OK)
    {
      psSfuFlashOptionBytes->OptionType     = OPTIONBYTE_WRP;
      psSfuFlashOptionBytes->WRPArea        = SFU_C2_PROTECT_WRP_AREA;
      psSfuFlashOptionBytes->WRPStartOffset = SFU_C2_PROTECT_WRP_PAGE_START;
      psSfuFlashOptionBytes->WRPEndOffset   = SFU_C2_PROTECT_WRP_PAGE_END;

      if (HAL_FLASHEx_OBProgram(psSfuFlashOptionBytes) == HAL_OK)
      {
        *pbIsProtectionToBeApplied |= 1U;
        e_ret_status = SFU_SUCCESS;
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_WRP, FLOW_CTRL_WRP);
      }
    }
#else
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /*SFU_WRP_PROTECT_ENABLE*/

#ifdef SFU_C2_DDS_PROTECT_ENABLE
/**
  * @brief  Check DDS protection configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check WRP ****************************************************************/
  /* Check if the protection is already applied */
  if (psFlashOptionBytes->C2DebugAccessMode == OB_C2_DEBUG_ACCESS_DISABLE)
  {
    e_ret_status = SFU_SUCCESS;
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2_DDS, FLOW_CTRL_C2_DDS);
  }
  return e_ret_status;
}

/**
  * @brief  Apply DDS protection.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval If Protection is not set, RSS procedure is called with a System Reset
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                             SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply DDS **********************************************************/
  /* Check if the protection is already applied */
  if (SFU_LL_SECU_CheckProtectionDDS(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    /* Apply the proper configuration if we are in development mode */
#if defined(SECBOOT_OB_DEV_MODE)
    /* Set the security bit if needed */
    /* ------------------------------ */
    psFlashOptionBytes->OptionType = OPTIONBYTE_C2_DEBUG_ACCESS;

    psFlashOptionBytes->C2DebugAccessMode = OB_C2_DEBUG_ACCESS_DISABLE;
    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2_DDS, FLOW_CTRL_C2_DDS);
    }
#else
    /* System Security Configuration failed: Sub-GHz radio SPI security access is incorrect */
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /* SFU_C2_DDS_PROTECT_ENABLE */

#ifdef SFU_MPU_PROTECT_ENABLE
/**
  * @brief  Apply MPU protection before executing UserApp
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionMPU_UserApp(void)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  MPU_Region_InitTypeDef MPU_InitStruct;

  /*  modify executable region for executing UserAPP*/
  MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
  MPU_InitStruct.Number               = APP_PROTECT_MPU_FLASHEXE_RGNV;
  MPU_InitStruct.BaseAddress          = APP_PROTECT_MPU_FLASHEXE_START;
  MPU_InitStruct.Size                 = APP_PROTECT_MPU_FLASHEXE_SIZE;
  MPU_InitStruct.SubRegionDisable     = APP_PROTECT_MPU_FLASHEXE_SREG;
  MPU_InitStruct.AccessPermission     = APP_PROTECT_MPU_FLASHEXE_PERM;
  MPU_InitStruct.DisableExec          = APP_PROTECT_MPU_FLASHEXE_EXECV;
  MPU_InitStruct.IsShareable          = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsBufferable         = APP_PROTECT_MPU_FLASHEXE_B ;
  MPU_InitStruct.IsCacheable          = APP_PROTECT_MPU_FLASHEXE_C;
  MPU_InitStruct.TypeExtField         = APP_PROTECT_MPU_FLASHEXE_TEXV;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /*  modify executable region for executing UserAPP */
  MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
  MPU_InitStruct.Number               = APP_PROTECT_MPU_FLASHEXE_ADJUST_RGNV;
  MPU_InitStruct.BaseAddress          = APP_PROTECT_MPU_FLASHEXE_ADJUST_START;
  MPU_InitStruct.Size                 = APP_PROTECT_MPU_FLASHEXE_ADJUST_SIZE;
  MPU_InitStruct.SubRegionDisable     = APP_PROTECT_MPU_FLASHEXE_ADJUST_SREG;
  MPU_InitStruct.AccessPermission     = APP_PROTECT_MPU_FLASHEXE_ADJUST_PERM;
  MPU_InitStruct.DisableExec          = APP_PROTECT_MPU_FLASHEXE_ADJUST_EXECV;
  MPU_InitStruct.IsShareable          = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsBufferable         = APP_PROTECT_MPU_FLASHEXE_ADJUST_B ;
  MPU_InitStruct.IsCacheable          = APP_PROTECT_MPU_FLASHEXE_ADJUST_C;
  MPU_InitStruct.TypeExtField         = APP_PROTECT_MPU_FLASHEXE_ADJUST_TEXV;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  return e_ret_status;
}
#endif /* SFU_MPU_PROTECT_ENABLE */

#ifdef SFU_MPU_PROTECT_ENABLE

/**
  * @brief  Apply MPU protection
  * @param  uStep Configuration step : SFU_INITIAL_CONFIGURATION, SFU_SECOND_CONFIGURATION, SFU_THIRD_CONFIGURATION
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionMPU(uint8_t uStep)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint8_t mpu_region_num = 0U; /* id of the MPU region being configured */
  MPU_Region_InitTypeDef MPU_InitStruct;

  for (mpu_region_num = 0U; mpu_region_num < (sizeof(MpuAreas) / sizeof(SFU_MPU_InitTypeDef)); mpu_region_num++)
  {
    MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
    MPU_InitStruct.Number               = MpuAreas[mpu_region_num].Number;
    MPU_InitStruct.BaseAddress          = MpuAreas[mpu_region_num].BaseAddress;
    MPU_InitStruct.Size                 = MpuAreas[mpu_region_num].Size;
    MPU_InitStruct.SubRegionDisable     = MpuAreas[mpu_region_num].SubRegionDisable;
    MPU_InitStruct.AccessPermission     = MpuAreas[mpu_region_num].AccessPermission;
    MPU_InitStruct.DisableExec          = MpuAreas[mpu_region_num].DisableExec;
    MPU_InitStruct.IsShareable          = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable          = MpuAreas[mpu_region_num].Cacheable;
    MPU_InitStruct.IsBufferable         = MpuAreas[mpu_region_num].Bufferable;
    MPU_InitStruct.TypeExtField         = MpuAreas[mpu_region_num].Tex;
    if (uStep == SFU_INITIAL_CONFIGURATION)
    {
      HAL_MPU_ConfigRegion(&MPU_InitStruct);
    }
    else
    {
      if (MPU_CheckConfig(&MPU_InitStruct) == SFU_ERROR)
      {
        return SFU_ERROR;
      }
    }
  }

  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    HAL_MPU_Enable(MPU_HARDFAULT_NMI);
  }
  else
  {
    if (MPU->CTRL != (MPU_HARDFAULT_NMI | MPU_CTRL_ENABLE_Msk))
    {
      return SFU_ERROR;
    }
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU, FLOW_CTRL_MPU);
  }
  return e_ret_status;
}
#endif /*SFU_MPU_PROTECT_ENABLE*/

#ifdef SFU_DMA_PROTECT_ENABLE
/**
  * @brief  Apply DMA protection
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionDMA(void)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  /*
   * In this function we disable the DMA buses in order to avoid that while the SB/SFU is running
   * some DMA has been already enabled (e.g. through debugger in RDP-1 after reset) in order to access sensitive
   * information in SRAM, FLASH
   */
  /* Disable  DMA1, DMA2 */
  __HAL_RCC_DMA1_CLK_DISABLE();

  __HAL_RCC_DMA2_CLK_DISABLE();

  __HAL_RCC_DMAMUX1_CLK_DISABLE();


  /* Execution stopped if flow control failed */
  FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_DMA, FLOW_CTRL_DMA);
  return e_ret_status;
}
#endif /*SFU_DMA_PROTECT_ENABLE*/

#ifdef SFU_DAP_PROTECT_ENABLE
/**
  * @brief  Set DAP protection status, configuring SWCLK and SWDIO GPIO pins.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionDAP(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  GPIO_InitTypeDef GPIO_InitStruct;

  assert_param(SFU_IS_FUNCTIONAL_STATE(eState));

  /* Enable clock of DBG GPIO port */
  SFU_DBG_CLK_ENABLE();

  /* Enable the DAP protections, so disable the DAP re-configuring SWCLK and SWDIO GPIO pins */
  GPIO_InitStruct.Pin = SFU_DBG_SWDIO_PIN | SFU_DBG_SWCLK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SFU_DBG_PORT, &GPIO_InitStruct);
  e_ret_status = SFU_SUCCESS;
  /* Execution stopped if flow control failed */
  FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_DAP, FLOW_CTRL_DAP);

  return e_ret_status;
}
#endif /*SFU_DAP_PROTECT_ENABLE*/

#ifdef  SFU_SECURE_USER_PROTECT_ENABLE
/**
  * @brief  Check Secure User Memory protection configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Test if Secure Memory size is correctly programmed and unique boot entry activated.
   */
  if (psFlashOptionBytes->SecureMode == SFU_SECURE_MODE)
  {
    /*
     * In this example secure area covers:
     * - the SB_SFU code
     * - the header of the slot #0
     */
    e_ret_status = SFU_SUCCESS;

    if ((psFlashOptionBytes->SecureMode & OB_SECURE_SYSTEM_AND_FLASH_ENABLE) == OB_SECURE_SYSTEM_AND_FLASH_ENABLE)
    {
      if (psFlashOptionBytes->SecureFlashStartAddr != SFU_SECURE_FLASH_START)
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_HIDE_PROTECTION_ENABLE) == OB_SECURE_HIDE_PROTECTION_ENABLE))
    {
      if (psFlashOptionBytes->HideProtectionStartAddr != SFU_HIDE_PROTECTION_START)
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_SRAM1_ENABLE) == OB_SECURE_SRAM1_ENABLE))
    {
      if (psFlashOptionBytes->SecureSRAM1StartAddr != SFU_SECURE_SRAM1_START)
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_SRAM2_ENABLE) == OB_SECURE_SRAM2_ENABLE))
    {
      if (psFlashOptionBytes->SecureSRAM2StartAddr != SFU_SECURE_SRAM2_START)
      {
        e_ret_status = SFU_ERROR;
      }
    }
  }

  /* Execution stopped if flow control failed */
  if (e_ret_status == SFU_SUCCESS)
  {
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SEC_MEM, FLOW_CTRL_SEC_MEM);
  }
  return e_ret_status;
}


/**
  * @brief  Apply Secure User Memory protection.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval If Protection is not set, RSS procedure is called with a System Reset
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                 SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check if Secure User Memory is already configured */
  if (SFU_LL_SECU_CheckProtectionSecUser(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    /* Apply the proper configuration if we are in development mode */
#if defined(SECBOOT_OB_DEV_MODE)
    /* Set the security bit if needed */
    /* ------------------------------ */
    psFlashOptionBytes->OptionType = OPTIONBYTE_SECURE_MODE;

    psFlashOptionBytes->SecureFlashStartAddr    = SFU_SECURE_FLASH_START;
    psFlashOptionBytes->SecureSRAM2StartAddr    = SFU_SECURE_SRAM2_START;
    /* psFlashOptionBytes->SecureSRAM1StartAddr    = SFU_SECURE_SRAM1_START; */
    psFlashOptionBytes->HideProtectionStartAddr = SFU_HIDE_PROTECTION_START;
    psFlashOptionBytes->SecureMode              = SFU_SECURE_MODE;

    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SEC_MEM, FLOW_CTRL_SEC_MEM);
    }
#else
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

