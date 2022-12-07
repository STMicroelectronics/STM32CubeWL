/**
  ******************************************************************************
  * @file    sfu_boot.h
  * @author  MCD Application Team
  * @brief   Header the Secure Boot module.
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
#ifndef SFU_BOOT_H
#define SFU_BOOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/**
  * @brief  SB Flow Control : Initial value
  */
#define FLOW_CTRL_INIT_VALUE 0x00005776U         /*!< Init value definition */

/**
  * @brief  SB Flow Control : Steps definition
  */

#define FLOW_STEP_C2BOOT 0x00006787U             /*!< Step C2OPT and SBRV value */

#define FLOW_STEP_UBE 0x00006787U                /*!< Step UBE value */

#ifdef  SFU_WRP_PROTECT_ENABLE
#define FLOW_STEP_WRP 0x0002cccdU                /*!< Step WRP value */
#else
#define FLOW_STEP_WRP 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_WRP_PROTECT_ENABLE */

#ifdef  SFU_RDP_PROTECT_ENABLE
#define FLOW_STEP_RDP 0x0000e3dcU                /*!< Step RDP value */
#else
#define FLOW_STEP_RDP 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_RDP_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
#define FLOW_STEP_C2_DDS 0x00003befU             /*!< Step DDS value */
#else
#define FLOW_STEP_C2_DDS 0x00000000U             /*!< No effect on control flow */
#endif /* SFU_C2_DDS_PROTECT_ENABLE */

#ifdef  SFU_SECURE_USER_PROTECT_ENABLE
#define FLOW_STEP_SEC_MEM 0x0000fdc2U            /*!< Step SECURE MEMORY value */
#else
#define FLOW_STEP_SEC_MEM 0x00000000U            /*!< No effect on control flow */
#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

#ifdef  SFU_TAMPER_PROTECT_ENABLE
#define FLOW_STEP_TAMPER 0x00012cd3U             /*!< Step TAMPER value */
#else
#define FLOW_STEP_TAMPER 0x00000000U             /*!< No effect on control flow */
#endif /* SFU_TAMPER_PROTECT_ENABLE */

#ifdef  SFU_MPU_PROTECT_ENABLE
#define FLOW_STEP_MPU 0x000132cdU                /*!< Step MPU value */
#else
#define FLOW_STEP_MPU 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_MPU_PROTECT_ENABLE */

#ifdef  SFU_FWALL_PROTECT_ENABLE
#define FLOW_STEP_FWALL 0x00014aabU              /*!< Step FWALL value */
#else
#define FLOW_STEP_FWALL 0x00000000U              /*!< No effect on control flow */
#endif /* SFU_FWALL_PROTECT_ENABLE */

#ifdef  SFU_DMA_PROTECT_ENABLE
#define FLOW_STEP_DMA 0x000154b5U                /*!< Step DMA value */
#else
#define FLOW_STEP_DMA 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_DMA_PROTECT_ENABLE */

#ifdef  SFU_IWDG_PROTECT_ENABLE
#define FLOW_STEP_IWDG 0x000165baU               /*!< Step IWDG value */
#else
#define FLOW_STEP_IWDG 0x00000000U               /*!< No effect on control flow */
#endif /* SFU_IWDG_PROTECT_ENABLE */

#ifdef  SFU_DAP_PROTECT_ENABLE
#define FLOW_STEP_DAP 0x00017ba4U                /*!< Step DAP value */
#else
#define FLOW_STEP_DAP 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_DAP_PROTECT_ENABLE */

/**
  * @brief  SB Flow Control : Control values static protections
  */
#define FLOW_CTRL_C2BOOT (FLOW_CTRL_INIT_VALUE ^ FLOW_STEP_C2BOOT)
#define FLOW_CTRL_UBE (FLOW_CTRL_C2BOOT ^ FLOW_STEP_UBE)
#define FLOW_CTRL_WRP (FLOW_CTRL_UBE ^ FLOW_STEP_WRP)
#define FLOW_CTRL_C2_DDS (FLOW_CTRL_WRP ^ FLOW_STEP_C2_DDS)
#define FLOW_CTRL_SEC_MEM (FLOW_CTRL_C2_DDS ^ FLOW_STEP_SEC_MEM)
#define FLOW_CTRL_RDP (FLOW_CTRL_SEC_MEM ^ FLOW_STEP_RDP)
#define FLOW_CTRL_STATIC_PROTECT FLOW_CTRL_RDP

/**
  * @brief  SB Flow Control : Control values runtime protections
  */
#define FLOW_CTRL_MPU (FLOW_CTRL_STATIC_PROTECT ^ FLOW_STEP_MPU)
#define FLOW_CTRL_FWALL (FLOW_CTRL_MPU ^ FLOW_STEP_FWALL)
#define FLOW_CTRL_DMA (FLOW_CTRL_FWALL ^ FLOW_STEP_DMA)
#define FLOW_CTRL_IWDG (FLOW_CTRL_DMA ^ FLOW_STEP_IWDG)
#define FLOW_CTRL_DAP (FLOW_CTRL_IWDG ^ FLOW_STEP_DAP)
#define FLOW_CTRL_TAMPER (FLOW_CTRL_DAP ^ FLOW_STEP_TAMPER)
#define FLOW_CTRL_RUNTIME_PROTECT FLOW_CTRL_TAMPER

/**
  * @brief  SB Flow Control : Exported macro
  */
#if defined(MAIN_C)
uint32_t uFlowProtectValue = FLOW_CTRL_INIT_VALUE;
uint32_t uFlowCryptoValue = FLOW_CTRL_INIT_VALUE;
#else
extern uint32_t uFlowProtectValue;
extern uint32_t uFlowCryptoValue;
#endif /* MAIN_C */

/* Control with STEP operation :
 * (uFlowValue XOR STEP_VALUE) should be equal to CTRL_VALUE ==> execution stopped if failed !
 */
#define FLOW_CONTROL_STEP(C,B,A) \
  do{ \
    (C) ^= (B);\
    if ((C) != (A))\
      SFU_EXCPT_Security_Error();\
  }while(0)

/** Control without STEP operation :
  * uFlowValue should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#define FLOW_CONTROL_CHECK(B,A) \
  do{ \
    if ((B) != (A))\
    { \
      SFU_EXCPT_Security_Error();\
    } \
  }while(0)

/** Control flow initialization
  */
#define FLOW_CONTROL_INIT(B,A) \
  do{ \
    (B) = (A);\
  }while(0)

#ifdef __cplusplus
}
#endif

#endif /* SFU_BOOT_H */
