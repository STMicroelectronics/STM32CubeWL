/**
  ******************************************************************************
  * @file    sfu_low_level_security.c
  * @author  MCD Application Team
  * @brief   SFU Security Low Level Interface module
  *          This file provides set of firmware functions to manage SFU security
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
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level.h"
#include "sfu_trace.h"
#include "sfu_boot.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "stm32wlxx_it.h"
#include "sfu_mpu_isolation.h"        /* Secure Engine isolated thanks to MPU */

#ifndef  SFU_WRP_PROTECT_ENABLE
#warning "SFU_WRP_PROTECT_DISABLED"
#endif /* SFU_WRP_PROTECT_ENABLE */

#ifndef  SFU_RDP_PROTECT_ENABLE
#warning "SFU_RDP_PROTECT_DISABLED"
#endif /* SFU_RDP_PROTECT_ENABLE */


#ifndef  SFU_MPU_PROTECT_ENABLE
#warning "SFU_MPU_PROTECT_DISABLED"
#endif /* SFU_MPU_PROTECT_ENABLE */

#ifndef  SFU_GTZC_PROTECT_ENABLE
#warning "SFU_GTZC_PROTECT_DISABLED"
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifndef  SFU_C2_DDS_PROTECT_ENABLE
#warning "SFU_C2_DDS_PROTECT_DISABLED"
#endif /* SFU_C2_DDS_PROTECT_ENABLE */

#ifndef  SFU_C2SWDBG_PROTECT_ENABLE
#warning "SFU_C2SWDBG_DISABLED"
#endif /* SFU_C2SWDBG_PROTECT_ENABLE */



#ifndef  SFU_TAMPER_PROTECT_ENABLE
/* #warning "SFU_TAMPER_PROTECT_DISABLED" */
#endif /* SFU_TAMPER_PROTECT_ENABLE */

#ifndef  SFU_DAP_PROTECT_ENABLE
#warning "SFU_DAP_PROTECT_DISABLED"
#endif /* SFU_DAP_PROTECT_ENABLE */

#ifndef  SFU_DMA_PROTECT_ENABLE
#warning "SFU_DMA_PROTECT_DISABLED"
#endif /* SFU_DMA_PROTECT_ENABLE */

#ifndef  SFU_IWDG_PROTECT_ENABLE
#warning "SFU_IWDG_PROTECT_DISABLED"
#endif /* SFU_IWDG_PROTECT_ENABLE */

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
#ifdef  SFU_IWDG_PROTECT_ENABLE
static IWDG_HandleTypeDef   IwdgHandle;

#endif /* SFU_IWDG_PROTECT_ENABLE */
#ifdef  SFU_MPU_PROTECT_ENABLE
static SFU_MPU_InitTypeDef MpuAreas[] =

{
  /* Aliases Region and all User Flash & internal RAM */
  {
    SFU_PROTECT_MPU_AREA_USER_RGNV, SFU_PROTECT_MPU_AREA_USER_START, SFU_PROTECT_MPU_AREA_USER_SIZE,
    SFU_PROTECT_MPU_AREA_USER_PERM, SFU_PROTECT_MPU_AREA_USER_EXECV, SFU_PROTECT_MPU_AREA_USER_SREG,
    SFU_PROTECT_MPU_AREA_USER_TEXV, SFU_PROTECT_MPU_AREA_USER_C,     SFU_PROTECT_MPU_AREA_USER_B
  },

  /* install header : read /write only for privileged */
  {
    SFU_PROTECT_MPU_HEADER_RGNV, SFU_PROTECT_MPU_HEADER_START, SFU_PROTECT_MPU_HEADER_SIZE,
    SFU_PROTECT_MPU_HEADER_PERM, SFU_PROTECT_MPU_HEADER_EXECV, SFU_PROTECT_MPU_HEADER_SREG,
    SFU_PROTECT_MPU_HEADER_TEXV, SFU_PROTECT_MPU_HEADER_C, SFU_PROTECT_MPU_HEADER_B
  },

  /*  Flash execution */
  {
    SFU_PROTECT_MPU_FLASHEXE_RGNV, SFU_PROTECT_MPU_FLASHEXE_START, SFU_PROTECT_MPU_FLASHEXE_SIZE,
    SFU_PROTECT_MPU_FLASHEXE_PERM, SFU_PROTECT_MPU_FLASHEXE_EXECV, SFU_PROTECT_MPU_FLASHEXE_SREG,
    SFU_PROTECT_MPU_FLASHEXE_TEXV, SFU_PROTECT_MPU_FLASHEXE_C, SFU_PROTECT_MPU_FLASHEXE_B
  },

  /* KMS_DataStorage: NVM storage */
  {
    SFU_PROTECT_MPU_KMS_1_RGNV, SFU_PROTECT_MPU_KMS_1_START, SFU_PROTECT_MPU_KMS_1_SIZE,
    SFU_PROTECT_MPU_KMS_1_PERM, SFU_PROTECT_MPU_KMS_1_EXECV, SFU_PROTECT_MPU_KMS_1_SREG,
    SFU_PROTECT_MPU_KMS_1_TEXV, SFU_PROTECT_MPU_KMS_1_C, SFU_PROTECT_MPU_KMS_1_B
  },
  {
    SFU_PROTECT_MPU_KMS_2_RGNV, SFU_PROTECT_MPU_KMS_2_START, SFU_PROTECT_MPU_KMS_2_SIZE,
    SFU_PROTECT_MPU_KMS_2_PERM, SFU_PROTECT_MPU_KMS_2_EXECV, SFU_PROTECT_MPU_KMS_2_SREG,
    SFU_PROTECT_MPU_KMS_2_TEXV, SFU_PROTECT_MPU_KMS_2_C, SFU_PROTECT_MPU_KMS_2_B
  },

#ifdef SFU_DMA_PROTECT_ENABLE
  /* DMA */
  {
    SFU_PROTECT_MPU_DMA_RGNV, SFU_PROTECT_MPU_DMA_START, SFU_PROTECT_MPU_DMA_SIZE,
    SFU_PROTECT_MPU_DMA_PERM, SFU_PROTECT_MPU_DMA_EXECV, SFU_PROTECT_MPU_DMA_SREG,
    SFU_PROTECT_MPU_DMA_TEXV, SFU_PROTECT_MPU_DMA_C, SFU_PROTECT_MPU_DMA_B
  },
#endif /* SFU_DMA_PROTECT_ENABLE */
  /*  peripheral  */
  {
    SFU_PROTECT_MPU_PERIPH_1_RGNV, SFU_PROTECT_MPU_PERIPH_1_START, SFU_PROTECT_MPU_PERIPH_1_SIZE,
    SFU_PROTECT_MPU_PERIPH_1_PERM, SFU_PROTECT_MPU_PERIPH_1_EXECV, SFU_PROTECT_MPU_PERIPH_1_SREG,
    SFU_PROTECT_MPU_PERIPH_1_TEXV, SFU_PROTECT_MPU_PERIPH_1_C, SFU_PROTECT_MPU_PERIPH_1_B
  }
};
#endif /* SFU_MPU_PROTECT_ENABLE */

/* Private function prototypes -----------------------------------------------*/
static SFU_ErrorStatus SFU_LL_SECU_CheckFlashConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetFlashConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                         SFU_BoolTypeDef *pbIsProtectionToBeApplied);

#ifdef SFU_RDP_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionRDP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                    SFU_BoolTypeDef *pbIsProtectionToBeApplied);
#endif /*SFU_RDP_PROTECT_ENABLE*/

#ifdef SFU_WRP_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionWRP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionWRP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                    SFU_BoolTypeDef *pbIsProtectionToBeApplied);
#endif /*SFU_WRP_PROTECT_ENABLE*/




#ifdef SFU_IWDG_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionIWDG(void);
#endif /*SFU_IWDG_PROTECT_ENABLE*/

#ifdef SFU_GTZC_PROTECT_ENABLE
static SFU_ErrorStatus    SFU_LL_SECU_SetProtectionGTZC(void);
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifdef SFU_C2SWDBG_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionC2SWDBG(void);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionC2SWDBG(void);
#endif /* SFU_C2SWDBG_PROTECT_ENABLE */

#ifdef SFU_CLCK_MNTR_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionCLOCK_MONITOR(void);
#endif /*SFU_CLCK_MNTR_PROTECT_ENABLE*/

#ifdef SFU_TEMP_MNTR_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionTEMP_MONITOR(void);
#endif /*SFU_TEMP_MNTR_PROTECT_ENABLE*/

#ifdef  SFU_SECURE_USER_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                        SFU_BoolTypeDef *pbIsProtectionToBeApplied);
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */
#ifdef SFU_GTZC_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionSUBGHZSPI(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionSUBGHZSPI(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                          SFU_BoolTypeDef *pbIsProtectionToBeApplied);

#endif /* SFU_GTZC_PROTECT_ENABLE */
#ifdef SFU_C2_DDS_PROTECT_ENABLE
static SFU_ErrorStatus SFU_LL_SECU_CheckProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes);
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                    SFU_BoolTypeDef *pbIsProtectionToBeApplied);

#endif /* SFU_C2_DDS_PROTECT_ENABLE */


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

#ifdef SFU_GTZC_PROTECT_ENABLE
/**
  * @brief  Check that a GTZC interrupt is enabled
  * @param  none
  * @retval SFU_ErrorStatus SFU_SUCCESS if enabled, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus GTZC_CheckITEnabled(uint32_t PeriphId)
{
  uint32_t register_address;
  uint32_t bit_pos;

  register_address = (uint32_t)&(GTZC_TZIC->IER1) + (4U * GTZC_GET_REG_INDEX(PeriphId));
  bit_pos = (uint32_t)(1UL << GTZC_GET_PERIPH_POS(PeriphId));
  if (READ_BIT(*(uint32_t *)register_address, bit_pos) == bit_pos)
  {
    return SFU_SUCCESS;
  }
  return SFU_ERROR;
}
#endif /* SFU_GTZC_PROTECT_ENABLE */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Check and if not applied apply the Static security  protections to
  *         all the SfuEn Sections in Flash: RDP, WRP, PCRoP. Static security protections
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
  SFU_BoolTypeDef is_protection_to_be_applied = SFU_FALSE;
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint32_t i_protection_set = 0U;
  uint32_t i_ctrl = 0U;

  /* Unlock the Flash to enable the flash control register access *************/
  (void) HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  /* Unlock the Options Bytes *************************************************/
  (void) HAL_FLASH_OB_Unlock();

  /* Get Option Bytes status for WRP AREA_A  **********/
  flash_option_bytes.WRPArea     = SFU_PROTECT_WRP_AREA_1;
  (void) HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

  /* Check/Apply RDP_Level 1. This is the minimum protection allowed */
  /* if RDP_Level 2 is already applied it's not possible to modify the OptionBytes anymore */
  if (flash_option_bytes.RDPLevel == OB_RDP_LEVEL_2)
  {
    /* Sanity check of the (enabled) static protections */
    if (SFU_LL_SECU_CheckFlashConfiguration(&flash_option_bytes) != SFU_SUCCESS)
    {
      TRACE("\r\n= [SBOOT] Flash configuration failed! Product blocked.");
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }

#ifdef SFU_WRP_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionWRP(&flash_option_bytes) != SFU_SUCCESS)
    {
      TRACE("\r\n= [SBOOT] System Security Configuration failed! Product blocked.");
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif /* SFU_WRP_PROTECT_ENABLE */


#ifdef SFU_SECURE_USER_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionSecUser(&flash_option_bytes) != SFU_SUCCESS)
    {
      TRACE("\r\n= [SBOOT] System Security Configuration failed! Product blocked.");
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif  /* SFU_SECURE_USER_PROTECT_ENABLE */
#ifdef SFU_GTZC_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionSUBGHZSPI(&flash_option_bytes) != SFU_SUCCESS)
    {
      TRACE("\r\n= [SBOOT] System Security Configuration failed! Product blocked.");
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
    if (SFU_LL_SECU_CheckProtectionDDS(&flash_option_bytes) != SFU_SUCCESS)
    {
      /* Security issue : execution stopped ! */
      SFU_EXCPT_Security_Error();
    }
#endif  /* SFU_C2_DDS_PROTECT_ENABLE */

    /*RDP level 2 ==> Flow control by-passed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RDP, FLOW_CTRL_RDP);
  }
  else
  {
    /* Check/Set Flash configuration *******************************************/
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_LL_SECU_SetFlashConfiguration(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;

    /* Check/Apply WRP ********************************************************/
#ifdef SFU_WRP_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      e_ret_status = SFU_LL_SECU_SetProtectionWRP(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;
#endif /* SFU_WRP_PROTECT_ENABLE */

    /* Check/Apply PCRoP ******************************************************/

    /* Check/Apply Secure User Memory *****************************************/
#ifdef SFU_SECURE_USER_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      e_ret_status = SFU_LL_SECU_SetProtectionSecUser(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;
#endif  /* SFU_SECURE_USER_PROTECT_ENABLE */

#ifdef SFU_GTZC_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      e_ret_status = SFU_LL_SECU_SetProtectionSUBGHZSPI(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      e_ret_status = SFU_LL_SECU_SetProtectionDDS(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;
#endif  /* SFU_C2_DDS_PROTECT_ENABLE */

    /* Check/Apply RDP : RDP-L2 should be done as last option bytes configuration */
#ifdef SFU_RDP_PROTECT_ENABLE
    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      e_ret_status = SFU_LL_SECU_SetProtectionRDP(&flash_option_bytes, &is_protection_to_be_applied);
    }
    i_ctrl++;
#endif  /* SFU_RDP_PROTECT_ENABLE */

    if (e_ret_status == SFU_SUCCESS)
    {
      i_protection_set++;
      if (is_protection_to_be_applied)
      {
        /* Generate System Reset to reload the new option byte values *************/
        /* WARNING: This means that if a protection can't be set, there will be a reset loop! */
        (void) HAL_FLASH_OB_Launch();
      }
    }
  }

  if (i_protection_set != i_ctrl)
  {
    e_ret_status = SFU_ERROR;
  }

  /* Lock the Options Bytes ***************************************************/
  (void) HAL_FLASH_OB_Lock();

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  (void) HAL_FLASH_Lock();

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
  uint32_t i_protection_set = 0U;
  uint32_t i_ctrl = 0U;

  /* Depending on compilation switches, prevent compilation warning */
  UNUSED(runtime_protection);


#ifdef SFU_MPU_PROTECT_ENABLE
  /* Check/Apply MPU ************************************************/
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionMPU(uStep) == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_MPU;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    /* Privileged mode required for MPU second-configuration
       Errors caught by FLOW_CONTROL */
    SFU_MPU_SysCall((uint32_t)SB_SYSCALL_MPU_CONFIG);
    i_protection_set++;
  }
  i_ctrl++;
#endif /* SFU_MPU_PROTECT_ENABLE */



  /* Check/Apply disable DMAs  ************************************************/
#ifdef SFU_DMA_PROTECT_ENABLE
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionDMA() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_DMA;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    /* Privileged mode required for DMA second-configuration in order to activate the clock through RCC
       Errors caught by FLOW_CONTROL */
    SFU_MPU_SysCall((uint32_t)SB_SYSCALL_DMA_CONFIG);
    i_protection_set++;
  }
  i_ctrl++;
#endif /* SFU_DMA_PROTECT_ENABLE */

  /* Check/Apply  IWDG **************************************************/
#ifdef SFU_IWDG_PROTECT_ENABLE
  if (SFU_LL_SECU_SetProtectionIWDG() == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_IWDG;
    i_protection_set++;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
  i_ctrl++;
#endif /* SFU_IWDG_PROTECT_ENABLE */

  /* Check/Apply  DAP *********************************************************/
#ifdef SFU_DAP_PROTECT_ENABLE
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionDAP() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_DAP;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    /* Privileged mode required for DMA second-configuration in order to activate the clock through RCC
       Errors caught by FLOW_CONTROL */
    SFU_MPU_SysCall((uint32_t)SB_SYSCALL_DAP_CONFIG);
    i_protection_set++;
  }
  i_ctrl++;
#else
#endif /* SFU_DAP_PROTECT_ENABLE */

  /* Check/Apply  ANTI_TAMPER *************************************************/
#ifdef SFU_TAMPER_PROTECT_ENABLE
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionANTI_TAMPER() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_ANTI_TAMPER;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    /* Privileged mode required for TAMPER second-configuration in order to activate the clock through RCC
       Errors caught by FLOW_CONTROL */
    SFU_MPU_SysCall((uint32_t)SB_SYSCALL_TAMPER_CONFIG);
    i_protection_set++;
  }
  i_ctrl++;
#else
#ifdef SFU_TEST_PROTECTION
  if (SFU_LL_RTC_Init() != SFU_SUCCESS)
  {
    e_ret_status = SFU_ERROR;
  }
#endif /* SFU_TEST_PROTECTION */
#endif /* SFU_TAMPER_PROTECT_ENABLE */

  /* Check/Apply  C2SWDBG *****************************************************/
#ifdef SFU_C2SWDBG_PROTECT_ENABLE
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionC2SWDBG() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_C2SWDBG;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    if (SFU_LL_SECU_CheckProtectionC2SWDBG() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_C2SWDBG;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  i_ctrl++;
#endif /* SFU_C2SWDBG_PROTECT_ENABLE */

  /* Check/Apply  GTZC ********************************************************/
#ifdef SFU_GTZC_PROTECT_ENABLE
  if (uStep == SFU_INITIAL_CONFIGURATION)
  {
    if (SFU_LL_SECU_SetProtectionGTZC() == SFU_SUCCESS)
    {
      runtime_protection |= SFU_RUNTIME_PROTECTION_GTZC;
      i_protection_set++;
    }
    else
    {
      /* When a protection cannot be set then SFU_ERROR is returned */
      e_ret_status = SFU_ERROR;
    }
  }
  else
  {
    /* Privileged mode required for GTZC checks
       Errors caught by FLOW_CONTROL */
    SFU_MPU_SysCall((uint32_t)SB_SYSCALL_GTZC_CONFIG);
    i_protection_set++;
  }
  i_ctrl++;
#endif /* SFU_GTZC_PROTECT_ENABLE */

  /* Check/Apply  CLOCK_MONITOR **********************************************/
#ifdef SFU_CLCK_MNTR_PROTECT_ENABLE
  if (SFU_LL_SECU_SetProtectionCLOCK_MONITOR() == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_CLOCK_MONITOR;
    i_protection_set++;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
  i_ctrl++;
#endif /* SFU_CLCK_MNTR_PROTECT_ENABLE */

  /* Check/Apply  TEMP_MONITOR **********************************************/
#ifdef SFU_TEMP_MNTR_PROTECT_ENABLE
  if (SFU_LL_SECU_SetProtectionTEMP_MONITOR() == SFU_SUCCESS)
  {
    runtime_protection |= SFU_RUNTIME_PROTECTION_TEMP_MONITOR;
    i_protection_set++;
  }
  else
  {
    /* When a protection cannot be set then SFU_ERROR is returned */
    e_ret_status = SFU_ERROR;
  }
  i_ctrl++;
#endif /* SFU_TEMP_MNTR_PROTECT_ENABLE */

#if defined(SFU_VERBOSE_DEBUG_MODE)
  TRACE("\r\n= [SBOOT] RuntimeProtections: %x", runtime_protection);
#endif /* SFU_VERBOSE_DEBUG_MODE */

  if (i_protection_set != i_ctrl)
  {
    e_ret_status = SFU_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Return the reset source  detected after a reboot. The related flag is reset
  *         at the end of this function.
  * @param  peResetpSourceId: to be filled with the detected source of reset
  * @note   In case of multiple reset sources this function return only one of them.
  *         It can be improved returning and managing a combination of them.
  * @retval SFU_SUCCESS if successful, SFU_ERROR otherwise
  */
void SFU_LL_SECU_GetResetSources(SFU_RESET_IdTypeDef *peResetpSourceId)
{
  /* Check if the last reset has been generated from a Watchdog exception */
  if ((__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) ||
      (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET))
  {
    *peResetpSourceId = SFU_RESET_WDG_RESET;

  }


  /* Check if the last reset has been generated from a Low Power reset */
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)
  {
    *peResetpSourceId = SFU_RESET_LOW_POWER;

  }

  /* Check if the last reset has been generated from a Software reset  */
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
  {
    *peResetpSourceId = SFU_RESET_SW_RESET;

  }
  /* Check if the last reset has been generated from an Option Byte Loader reset  */
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)
  {
    *peResetpSourceId = SFU_RESET_OB_LOADER;
  }
  /* Check if the last reset has been generated from a Hw pin reset  */
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
  {
    *peResetpSourceId = SFU_RESET_HW_RESET;

  }
  /* Unknown */
  else
  {
    *peResetpSourceId = SFU_RESET_UNKNOWN;
  }
}

/**
  * @brief  Clear the reset sources. This function should be called after the actions
  *         on the reset sources has been already taken.
  * @param  none
  * @note   none
  * @retval none
  */
void SFU_LL_SECU_ClearResetSources()
{
  /* Clear reset flags  */
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

/**
  * @brief  Refresh Watchdog : reload counter
  *         This function must be called just before jumping to the UserFirmware
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_IWDG_Refresh(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

#ifdef SFU_IWDG_PROTECT_ENABLE
  /* Refresh IWDG: reload counter */
  if (HAL_IWDG_Refresh(&IwdgHandle) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }
#else
  e_ret_status = SFU_SUCCESS;
#endif /*SFU_IWDG_PROTECT_ENABLE*/

  return e_ret_status;
}

/**
  * @brief  Check Flash configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @retval SFU_ErrorStatus SFU_SUCCESS if Flash configuration is correct, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckFlashConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
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
  * @brief  Set Flash configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetFlashConfiguration(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                  SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check Flash configuration */
  if (SFU_LL_SECU_CheckFlashConfiguration(psFlashOptionBytes) == SFU_SUCCESS)
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
    TRACE("\r\n= [SBOOT] Boot Configuration issue: booting Main flash through PA14-BOOT0 pin functionality cannot be guaranteed");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}

#ifdef SFU_RDP_PROTECT_ENABLE
/**
  * @brief  Apply the RDP protection
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionRDP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                             SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply RDP **********************************************************/
  /* Please consider that the suggested and most secure approach is to set the RDP_LEVEL_2 */
  if (psFlashOptionBytes->RDPLevel == SFU_PROTECT_RDP_LEVEL)
  {
    e_ret_status = SFU_SUCCESS; /*Protection already applied */
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RDP, FLOW_CTRL_RDP);
  }
  else
  {
#if defined(SECBOOT_OB_DEV_MODE)
#if defined(SFU_FINAL_SECURE_LOCK_ENABLE)
    TRACE("\r\n\t  Applying RDP-2 Level. Product locked! You might need to unplug/plug the USB cable!");
#else
    TRACE("\r\n\t  Applying RDP-1 Level. You might need to unplug/plug the USB cable!");
#endif /* SFU_FINAL_SECURE_LOCK_ENABLE */
    psFlashOptionBytes->OptionType      = OPTIONBYTE_RDP;
    psFlashOptionBytes->RDPLevel        = SFU_PROTECT_RDP_LEVEL;
    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RDP, FLOW_CTRL_RDP);
    }
#else
    TRACE("\r\n= [SBOOT] System Security Configuration failed: RDP is incorrect. STOP!");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }
  return e_ret_status;
}
#endif /*SFU_RDP_PROTECT_ENABLE*/

#ifdef SFU_WRP_PROTECT_ENABLE
/**
  * @brief  Check the WRP protection to the specified Area. It includes the SFU Vector Table
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionWRP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check WRP ****************************************************************/
  if ((psFlashOptionBytes->WRPStartOffset == SFU_PROTECT_WRP_PAGE_START_1) &&
      (psFlashOptionBytes->WRPEndOffset   == SFU_PROTECT_WRP_PAGE_END_1))
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
  * @brief  Apply the WRP protection to the specified Area. It includes the SFU Vector Table
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionWRP(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                             SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply WRP **********************************************************/
  if (SFU_LL_SECU_CheckProtectionWRP(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS; /*Protection already applied */
  }
  else
  {
#if defined(SECBOOT_OB_DEV_MODE)
    psFlashOptionBytes->OptionType     = OPTIONBYTE_WRP;
    psFlashOptionBytes->WRPArea        = SFU_PROTECT_WRP_AREA_1;
    psFlashOptionBytes->WRPStartOffset = SFU_PROTECT_WRP_PAGE_START_1;
    psFlashOptionBytes->WRPEndOffset   = SFU_PROTECT_WRP_PAGE_END_1;

    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_WRP, FLOW_CTRL_WRP);
    }
#else
    TRACE("\r\n= [SBOOT] System Security Configuration failed: WRP is incorrect. STOP!");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /*SFU_WRP_PROTECT_ENABLE*/

#ifdef SFU_MPU_PROTECT_ENABLE
/**
  * @brief  Apply MPU protection before executing UserApp
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionMPU_UserApp(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* configure a temporary area for the currently executing code (SBSFU + active slots) */
  /* This area of higher rank will keep the execution rights when reconfiguring the current area */
  MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
  MPU_InitStruct.Number               = APP_PROTECT_MPU_BLOB_DWL_RGNV; /* temporary area */
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

  /* configure a new area for the currently executing code (SBSFU + active slots) */
  MPU_InitStruct.Number               = APP_PROTECT_MPU_FLASHEXE_RGNV;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* remap the current area (SBSFU + active slots) */
  MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
  MPU_InitStruct.Number               = APP_PROTECT_MPU_BLOB_DWL_RGNV;
  MPU_InitStruct.BaseAddress          = APP_PROTECT_MPU_BLOB_DWL_START;
  MPU_InitStruct.Size                 = APP_PROTECT_MPU_BLOB_DWL_SIZE;
  MPU_InitStruct.SubRegionDisable     = APP_PROTECT_MPU_BLOB_DWL_SREG;
  MPU_InitStruct.AccessPermission     = APP_PROTECT_MPU_BLOB_DWL_PERM;
  MPU_InitStruct.DisableExec          = APP_PROTECT_MPU_BLOB_DWL_EXECV;
  MPU_InitStruct.IsShareable          = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsBufferable         = APP_PROTECT_MPU_BLOB_DWL_B ;
  MPU_InitStruct.IsCacheable          = APP_PROTECT_MPU_BLOB_DWL_C;
  MPU_InitStruct.TypeExtField         = APP_PROTECT_MPU_BLOB_DWL_TEXV;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /*  Remove the write rights for the SRAM synchronization flag */
  MPU_InitStruct.Enable               = MPU_REGION_ENABLE;
  MPU_InitStruct.Number               = APP_PROTECT_MPU_SRAM_FLAG_RGNV;
  MPU_InitStruct.BaseAddress          = APP_PROTECT_MPU_SRAM_FLAG_START;
  MPU_InitStruct.Size                 = APP_PROTECT_MPU_SRAM_FLAG_SIZE;
  MPU_InitStruct.SubRegionDisable     = APP_PROTECT_MPU_SRAM_FLAG_SREG;
  MPU_InitStruct.AccessPermission     = APP_PROTECT_MPU_SRAM_FLAG_PERM;
  MPU_InitStruct.DisableExec          = APP_PROTECT_MPU_SRAM_FLAG_EXECV;
  MPU_InitStruct.IsShareable          = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsBufferable         = APP_PROTECT_MPU_SRAM_FLAG_B ;
  MPU_InitStruct.IsCacheable          = APP_PROTECT_MPU_SRAM_FLAG_C;
  MPU_InitStruct.TypeExtField         = APP_PROTECT_MPU_SRAM_FLAG_TEXV;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);


  return SFU_SUCCESS;
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
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint8_t mpu_region_num; /* id of the MPU region being configured */
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
    MPU_InitStruct.IsBufferable         = MpuAreas[mpu_region_num].Bufferable;
    MPU_InitStruct.IsCacheable          = MpuAreas[mpu_region_num].Cacheable;
    MPU_InitStruct.TypeExtField         = MpuAreas[mpu_region_num].Tex;
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE(" @%d:%x size:%x sub:%x perm:%x exec:%x\r\n",
          MPU_InitStruct.Number, MPU_InitStruct.BaseAddress, MPU_InitStruct.Size, MPU_InitStruct.SubRegionDisable,
          MPU_InitStruct.AccessPermission, MPU_InitStruct.DisableExec);
#endif /* SFU_VERBOSE_DEBUG_MODE */

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
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_HARDFAULT_NMI);

#if defined(SCB_SHCSR_MEMFAULTENA_Msk)
    /* Enables memory fault exception */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif /* SCB_SHCSR_MEMFAULTENA_Msk */
  }
  else
  {
    if (MPU->CTRL != (MPU_HARDFAULT_NMI | MPU_CTRL_ENABLE_Msk))
    {
      return SFU_ERROR;
    }
#if defined(SCB_SHCSR_MEMFAULTENA_Msk)
    if ((SCB->SHCSR & SCB_SHCSR_MEMFAULTENA_Msk) != SCB_SHCSR_MEMFAULTENA_Msk)
    {
      return SFU_ERROR;
    }
#endif /* SCB_SHCSR_MEMFAULTENA_Msk */
  }

  e_ret_status = SFU_SUCCESS;


  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU, FLOW_CTRL_MPU);
  }
  return e_ret_status;
}
#endif /*SFU_MPU_PROTECT_ENABLE*/

#ifdef SFU_GTZC_PROTECT_ENABLE
/**
  * @brief  Check if GTZC protection is enabled.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionGTZC(void)
{
  MPCWM_ConfigTypeDef mpcwm_desc;
  uint32_t periph_attributes;
  uint32_t i_protection_set = 0U;
  uint32_t i_ctrl = 0U;

  /* Check FLASH memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  if (HAL_GTZC_TZSC_MPCWM_GetConfigMemAttributes(FLASH_BASE, &mpcwm_desc) == HAL_OK)
  {
    if (mpcwm_desc.Length == SFU_PROTECT_GTZC_EXEC_SE_START - FLASH_BASE)
    {
      i_protection_set++;
    }
  }
  i_ctrl++;
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV_WRITABLE;
  if (HAL_GTZC_TZSC_MPCWM_GetConfigMemAttributes(FLASH_BASE, &mpcwm_desc) == HAL_OK)
  {
    if (mpcwm_desc.Length == SFU_PROTECT_GTZC_EXEC_SE_START - FLASH_BASE)
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  /* Check SRAM1 memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  if (HAL_GTZC_TZSC_MPCWM_GetConfigMemAttributes(SRAM1_BASE, &mpcwm_desc) == HAL_OK)
  {
    if (mpcwm_desc.Length == SRAM1_SIZE)
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  /* Check SRAM2 memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  if (HAL_GTZC_TZSC_MPCWM_GetConfigMemAttributes(SRAM2_BASE, &mpcwm_desc) == HAL_OK)
  {
    if (mpcwm_desc.Length == SE_REGION_RAM_START - SRAM2_BASE)
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  /* Check TZSC peripherals ***********************************************/
  if (HAL_GTZC_TZSC_GetConfigPeriphAttributes(GTZC_PERIPH_AES,
                                              &periph_attributes) == HAL_OK)
  {
    if (periph_attributes == (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV))
    {
      i_protection_set++;
    }
  }
  i_ctrl++;
  if (HAL_GTZC_TZSC_GetConfigPeriphAttributes(GTZC_PERIPH_RNG,
                                              &periph_attributes) == HAL_OK)
  {
    if (periph_attributes == (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV))
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  if (HAL_GTZC_TZSC_GetConfigPeriphAttributes(GTZC_PERIPH_SUBGHZSPI,
                                              &periph_attributes) == HAL_OK)
  {
    if (periph_attributes == (GTZC_TZSC_ATTRIBUTE_NPRIV))
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  if (HAL_GTZC_TZSC_GetConfigPeriphAttributes(GTZC_PERIPH_PKA,
                                              &periph_attributes) == HAL_OK)
  {
    if (periph_attributes == (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV))
    {
      i_protection_set++;
    }
  }
  i_ctrl++;

  /* Check TZIC peripherals ***********************************************/

  /* Check FLASH_ACR2 */
  if (READ_BIT(FLASH->ACR2, FLASH_ACR2_PRIVMODE) == FLASH_ACR2_PRIVMODE)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Check the IT for the concerned peripherals */
  if (GTZC_CheckITEnabled(GTZC_PERIPH_TZIC) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_TZSC) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_AES) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_RNG) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_SUBGHZSPI) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_PWR) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_FLASHIF) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_DMA1) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_DMA2) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_DMAMUX) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_FLASH) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_SRAM1) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_SRAM2) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (GTZC_CheckITEnabled(GTZC_PERIPH_PKA) == SFU_SUCCESS)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Enable IRQ */
  if (NVIC_GetEnableIRQ(TZIC_ILA_IRQn))
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Check the GTZC TZC lock *************************************************/
  if (HAL_GTZC_TZSC_GetLock(GTZC_TZSC) == GTZC_TZSC_LOCK_ON)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Double Check to avoid basic fault injection : no configuration skipped */
  if (i_protection_set == i_ctrl)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC, FLOW_CTRL_GTZC);

    return SFU_SUCCESS;
  }

  return SFU_ERROR;
}

/**
  * @brief  Apply GTZC protection: FLASH, RAM, PKA, RNG, AES.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionGTZC(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  MPCWM_ConfigTypeDef mpcwm_desc;
  uint32_t i_protection_set = 0U;
  uint32_t i_ctrl = 0U;

  /* Handle the GTZC TZC lock *************************************************/

  /* Configure FLASH memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  mpcwm_desc.Length = SFU_PROTECT_GTZC_EXEC_SE_START - FLASH_BASE;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &mpcwm_desc) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV_WRITABLE;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &mpcwm_desc) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Configure SRAM1 memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  mpcwm_desc.Length = SRAM1_SIZE;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM1_BASE, &mpcwm_desc) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Configure SRAM2 memory ***************************************************/
  mpcwm_desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  mpcwm_desc.Length = SE_REGION_RAM_START - SRAM2_BASE;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &mpcwm_desc) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Configure TZSC peripherals ***********************************************/
  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_AES,
                                           (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV)) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_RNG,
                                           (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV)) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_SUBGHZSPI,
                                           (GTZC_TZSC_ATTRIBUTE_NPRIV)) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_PKA,
                                           (GTZC_TZSC_ATTRIBUTE_SEC  | GTZC_TZSC_ATTRIBUTE_PRIV)) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Configure TZIC peripherals ***********************************************/

  /* Configure FLASH_ACR2 */
  MODIFY_REG(FLASH->ACR2, FLASH_ACR2_PRIVMODE, FLASH_ACR2_PRIVMODE);
  if (READ_BIT(FLASH->ACR2, FLASH_ACR2_PRIVMODE) == FLASH_ACR2_PRIVMODE)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Enable the IT for the concerned peripherals */
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_TZIC) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_TZSC) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_AES) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_RNG) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SUBGHZSPI) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_PWR) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_FLASHIF) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_DMA1) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_DMA2) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_DMAMUX) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_FLASH) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM1) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM2) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_PKA) == HAL_OK)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Enable IRQ */
  NVIC_EnableIRQ(TZIC_ILA_IRQn);
  if (NVIC_GetEnableIRQ(TZIC_ILA_IRQn))
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Handle the GTZC TZC lock *************************************************/
  HAL_GTZC_TZSC_Lock(GTZC_TZSC);
  if (HAL_GTZC_TZSC_GetLock(GTZC_TZSC) == GTZC_TZSC_LOCK_ON)
  {
    i_protection_set++;
  }
  i_ctrl++;

  /* Double Check to avoid basic fault injection : no configuration skipped */
  if (i_protection_set == i_ctrl)
  {
    e_ret_status = SFU_SUCCESS;

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC, FLOW_CTRL_GTZC);
  }

  return e_ret_status;
}
#endif /* SFU_GTZC_PROTECT_ENABLE */



#ifdef SFU_DMA_PROTECT_ENABLE
/**
  * @brief  Apply DMA protection
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionDMA(void)
{
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
  return SFU_SUCCESS;
}
#endif /*SFU_DMA_PROTECT_ENABLE*/

#ifdef SFU_IWDG_PROTECT_ENABLE
/**
  *  @brief Apply IWDG protection
  *         The IWDG timeout is set to 4 second.
  *         Then, the IWDG reload counter is configured as below to obtain 4 second according
  *         to the measured LSI frequency after setting the prescaler value:
  *         IWDG counter clock Frequency = LSI Frequency / Prescaler value
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionIWDG(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* TIMER could be used to get the LSI frequency in order to have a more precise IWDG.
  This is not used in this implementation because not necessary and in order
  to optimize code-size. If you are interested, please have a look at the IWDG Cube example. */

  /* Configure & Start the IWDG peripheral */
  /* Set counter reload value to obtain 6 sec. IWDG TimeOut.
  IWDG counter clock Frequency = uwLsiFreq
  Set Prescaler to 64 (IWDG_PRESCALER_64)
  Timeout Period = (Reload Counter Value * 64) / uwLsiFreq
  So Set Reload Counter Value = (6 * uwLsiFreq) / 64 */
  IwdgHandle.Instance = IWDG;
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;
  IwdgHandle.Init.Reload = (SFU_IWDG_TIMEOUT * LSI_VALUE / 64U);
  IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

  if (HAL_IWDG_Init(&IwdgHandle) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_IWDG, FLOW_CTRL_IWDG);
  }

  return e_ret_status;
}
#endif /*SFU_IWDG_PROTECT_ENABLE*/

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
#else
#endif /*SFU_DAP_PROTECT_ENABLE*/

#ifdef SFU_TAMPER_PROTECT_ENABLE
/**
  * @brief  Apply ANTI TAMPER protection
  * @param None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionANTI_TAMPER(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  RTC_TamperTypeDef  stamperstructure;

  /* RTC_TAMPER_1 (PC13) selected. PA0 connected to RTC_TAMPER_2 is also connected to the B1 button */
  TAMPER_GPIO_CLK_ENABLE();

  /* Configure Tamper Pin */
  /* tamper is an additional function */
  /* not an alternate Function : config not needed */
  /* Configure the RTC peripheral */
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
  - Hour Format    = Format 24
  - Asynch Prediv  = Value according to source clock
  - Synch Prediv   = Value according to source clock
  - OutPut         = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType     = Open Drain */
  RtcHandle.Instance            = RTC;
  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

  if (HAL_RTC_Init(&RtcHandle) == HAL_OK)
  {
    /* Configure RTC Tamper */
    stamperstructure.Tamper                       = RTC_TAMPER_ID;
    stamperstructure.Trigger                      = RTC_TAMPERTRIGGER_FALLINGEDGE;
    stamperstructure.Filter                       = RTC_TAMPERFILTER_DISABLE;
    stamperstructure.SamplingFrequency            = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768;
    stamperstructure.PrechargeDuration            = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
    stamperstructure.TamperPullUp                 = RTC_TAMPER_PULLUP_ENABLE;
    stamperstructure.TimeStampOnTamperDetection   = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE;
    stamperstructure.NoErase                      = RTC_TAMPER_ERASE_BACKUP_ENABLE;
    stamperstructure.MaskFlag                     = RTC_TAMPERMASK_FLAG_DISABLE;

    if (HAL_RTCEx_SetTamper_IT(&RtcHandle, &stamperstructure) == HAL_OK)
    {

      /* Clear the Tamper interrupt pending bit */
      __HAL_RTC_TAMPER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_TAMP_1);
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMPER, FLOW_CTRL_TAMPER);
    }
  }

  return e_ret_status;
}
#endif /*SFU_TAMPER_PROTECT_ENABLE*/

#ifdef SFU_C2SWDBG_PROTECT_ENABLE
/**
  * @brief  Check if C2SWDBG protection is enabled.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionC2SWDBG(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  if (READ_BIT(FLASH->ACR2, FLASH_ACR2_C2SWDBGEN) == 0U)
  {
    /* C2SWDBG disabled */
    e_ret_status = SFU_SUCCESS;
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2SWDBG, FLOW_CTRL_C2SWDBG);
  }
  return e_ret_status;
}

/**
  * @brief  Apply C2SWDBG protection
  * @param None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus SFU_LL_SECU_SetProtectionC2SWDBG(void)
{
  HAL_FLASHEx_DisableC2Debug();
  /* Execution stopped if flow control failed */
  FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_C2SWDBG, FLOW_CTRL_C2SWDBG);
  return SFU_SUCCESS;
}
#endif /* SFU_C2SWDBG_PROTECT_ENABLE */

#ifdef SFU_CLCK_MNTR_PROTECT_ENABLE
/**
  * @brief  Apply CLOCK MONITOR protection
  * @note   This function has been added just as template to be used/customized
  *         if a clock monitor is requested.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionCLOCK_MONITOR(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Apply the Clock Monitoring */
  /* Add your code here for customization
     e.g. if HSE or LSE is used enable the CSS!
   ...
   ...
  */

  e_ret_status = SFU_SUCCESS;

  return e_ret_status;
}
#endif /*SFU_CLCK_MNTR_PROTECT_ENABLE*/

#ifdef SFU_TEMP_MNTR_PROTECT_ENABLE
/**
  * @brief  Apply TEMP MONITOR protection
  * @note   This function has been added just as template to be used/customized
  *         if a temperature monitor is requested.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionTEMP_MONITOR(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Apply the Temperature Monitoring */
  /* Add your code here for customization
     ...
     ...
  */

  e_ret_status = SFU_SUCCESS;

  return e_ret_status;
}
#endif /*SFU_TEMP_MNTR_PROTECT_ENABLE*/


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
     * - the header of the active slots
     */
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [SBOOT] Secure Mode: [%x]", psFlashOptionBytes->SecureMode);
#endif /* SFU_VERBOSE_DEBUG_MODE */
    e_ret_status = SFU_SUCCESS;

    if ((psFlashOptionBytes->SecureMode & OB_SECURE_SYSTEM_AND_FLASH_ENABLE) == OB_SECURE_SYSTEM_AND_FLASH_ENABLE)
    {
      if (psFlashOptionBytes->SecureFlashStartAddr == SFU_SECURE_FLASH_START)
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n= [SBOOT] Secure Flash Start Address: [%x]", psFlashOptionBytes->SecureFlashStartAddr);
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
      else
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_HIDE_PROTECTION_ENABLE) == OB_SECURE_HIDE_PROTECTION_ENABLE))
    {
      if (psFlashOptionBytes->HideProtectionStartAddr == SFU_HIDE_PROTECTION_START)
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n= [SBOOT] Hide Protection Start Address: [%x]", psFlashOptionBytes->HideProtectionStartAddr);
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
      else
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_SRAM1_ENABLE) == OB_SECURE_SRAM1_ENABLE))
    {
      if (psFlashOptionBytes->SecureSRAM1StartAddr == SFU_SECURE_SRAM1_START)
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n= [SBOOT] Secure SRAM1 Start Address: [%x]", psFlashOptionBytes->SecureSRAM1StartAddr);
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
      else
      {
        e_ret_status = SFU_ERROR;
      }
    }

    if ((e_ret_status == SFU_SUCCESS)
        && ((psFlashOptionBytes->SecureMode & OB_SECURE_SRAM2_ENABLE) == OB_SECURE_SRAM2_ENABLE))
    {
      if (psFlashOptionBytes->SecureSRAM2StartAddr == SFU_SECURE_SRAM2_START)
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n= [SBOOT] Secure SRAM2 Start Address: [%x]", psFlashOptionBytes->SecureSRAM2StartAddr);
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
      else
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
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
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
    psFlashOptionBytes->SecureSRAM1StartAddr    = SFU_SECURE_SRAM1_START;
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
    TRACE("\r\n= [SBOOT] System Security Configuration failed: Secure User Memory is incorrect. STOP!");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}

#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

/**
  * @brief  Activate Secure User Memory protection and jump into user application.
  * @param  Address of the code to jump into
  * @retval none.
  */


#ifdef SFU_GTZC_PROTECT_ENABLE
/**
  * @brief  Check Sub-GHz radio SPI protection configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionSUBGHZSPI(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply Sub-GHz radio SPI ********************************************/
  /* Check if the protection is already applied */
  if (psFlashOptionBytes->SUBGHZSPISecureAccess == OB_SUBGHZSPI_SECURE_ACCESS_ENABLE)
  {
    e_ret_status = SFU_SUCCESS;
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SUBGHZSPI, FLOW_CTRL_SUBGHZSPI);
  }
  return e_ret_status;
}

/**
  * @brief  Apply Sub-GHz radio SPI protection.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval If Protection is not set, RSS procedure is called with a System Reset
  */
SFU_ErrorStatus SFU_LL_SECU_SetProtectionSUBGHZSPI(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                   SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check if the protection is already applied */
  if (SFU_LL_SECU_CheckProtectionSUBGHZSPI(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    /* Apply the proper configuration if we are in development mode */
#if defined(SECBOOT_OB_DEV_MODE)
    /* Set the security bit if needed */
    /* ------------------------------ */
    psFlashOptionBytes->OptionType = OPTIONBYTE_SUBGHZSPI_SECURE_ACCESS;

    psFlashOptionBytes->SUBGHZSPISecureAccess = OB_SUBGHZSPI_SECURE_ACCESS_ENABLE;
    if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
    {
      *pbIsProtectionToBeApplied |= 1U;
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SUBGHZSPI, FLOW_CTRL_SUBGHZSPI);
    }
#else
    TRACE("\r\n= [SBOOT] System Security Configuration failed: Sub-GHz radio SPI security access is incorrect. STOP!");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /* SFU_GTZC_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
/**
  * @brief  Check DDS protection configuration.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_SECU_CheckProtectionDDS(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check/Apply DDS ********************************************/
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
    TRACE("\r\n= [SBOOT] System Security Configuration failed: Sub-GHz radio SPI security access is incorrect. STOP!");
    /* Security issue : execution stopped ! */
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
  }

  return e_ret_status;
}
#endif /* SFU_C2_DDS_PROTECT_ENABLE */
