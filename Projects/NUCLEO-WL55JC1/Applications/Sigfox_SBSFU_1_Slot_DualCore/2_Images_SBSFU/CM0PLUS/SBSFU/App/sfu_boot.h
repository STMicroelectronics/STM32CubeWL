/**
  ******************************************************************************
  * @file    sfu_boot.h
  * @author  MCD Application Team
  * @brief   Header the Secure Boot module, part of the SFU-En project (SB/SFU).
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

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  SFU_BOOT Init Error Type Definition
  */
typedef enum
{
  SFU_BOOT_SECENG_INIT_FAIL,   /*!< failure at secure engine initialization stage */
  SFU_BOOT_SECIPS_CFG_FAIL,    /*!< failure when configuring the security IPs  */
  SFU_BOOT_INIT_FAIL,          /*!< failure when initializing the secure boot service  */
  SFU_BOOT_INIT_ERROR          /*!< Service cannot start: unspecified error */
} SFU_BOOT_InitErrorTypeDef;

/* Exported constants --------------------------------------------------------*/
/**
  * @brief  SFU_BOOT Flow Control : Initial value
  */
#define FLOW_CTRL_INIT_VALUE 0x00005776U         /*!< Init value definition */

/**
  * @brief  SFU_BOOT Flow Control : Steps definition
  */
#define FLOW_STEP_UBE 0x00006787U                /*!< Step UBE value */
#define FLOW_STEP_AUTHENTICATE 0x00007999U       /*!< Step AUTHENTICATE value */
#define FLOW_STEP_INTEGRITY 0x0000aab5U          /*!< Step INTEGRITY value */
#define FLOW_STEP_LOCK_SERVICE 0x0000b4abU       /*!< Step LOCK SERVICE value */

#ifdef  SFU_WRP_PROTECT_ENABLE
#define FLOW_STEP_WRP 0x0000cccdU                /*!< Step WRP value */
#else
#define FLOW_STEP_WRP 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_WRP_PROTECT_ENABLE */

#ifdef  SFU_PCROP_PROTECT_ENABLE
#define FLOW_STEP_PCROP 0x0000d2d3U              /*!< Step PCROP value */
#else
#define FLOW_STEP_PCROP 0x00000000U              /*!< No effect on control flow */
#endif /* SFU_PCROP_PROTECT_ENABLE */

#ifdef  SFU_RDP_PROTECT_ENABLE
#define FLOW_STEP_RDP 0x0000e3dcU                /*!< Step RDP value */
#else
#define FLOW_STEP_RDP 0x00000000U                /*!< No effect on control flow */
#endif /* SFU_RDP_PROTECT_ENABLE */

#ifdef  SFU_SUBGHZSPI_PROTECT_ENABLE
#define FLOW_STEP_SUBGHZSPI 0x0003befU           /*!< Step SUBGHZSPI value */
#else
#define FLOW_STEP_SUBGHZSPI 0x00000000U          /*!< No effect on control flow */
#endif /* SFU_SUBGHZSPI_PROTECT_ENABLE */

#ifdef SFU_C2_DDS_PROTECT_ENABLE
#define FLOW_STEP_C2_DDS 0x000096bdU             /*!< Step DDS value */
#else
#define FLOW_STEP_C2_DDS 0x00000000U                /*!< No effect on control flow */
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

#ifdef SFU_C2SWDBG_PROTECT_ENABLE
#define FLOW_STEP_C2SWDBG 0x00024e87U            /*!< Step C2SWDBG value */
#else
#define FLOW_STEP_C2SWDBG 0x00000000U            /*!< No effect on control flow */
#endif /* SFU_C2SWDBG_PROTECT_ENABLE */

#ifdef  SFU_GTZC_PROTECT_ENABLE
#define FLOW_STEP_GTZC 0x0000dbaeU               /*!< Step GTZC value */
#else
#define FLOW_STEP_GTZC 0x00000000U               /*!< No effect on control flow */
#endif /* SFU_GTZC_PROTECT_ENABLE */

/**
  * @brief  SFU_BOOT Flow Control : Control values static protections
  */
#define FLOW_CTRL_UBE (FLOW_CTRL_INIT_VALUE ^ FLOW_STEP_UBE)
#define FLOW_CTRL_WRP (FLOW_CTRL_UBE ^ FLOW_STEP_WRP)
#define FLOW_CTRL_PCROP (FLOW_CTRL_WRP ^ FLOW_STEP_PCROP)
#define FLOW_CTRL_SEC_MEM (FLOW_CTRL_PCROP ^ FLOW_STEP_SEC_MEM)
#define FLOW_CTRL_SUBGHZSPI (FLOW_CTRL_SEC_MEM ^ FLOW_STEP_SUBGHZSPI)
#define FLOW_CTRL_C2_DDS (FLOW_CTRL_SUBGHZSPI ^ FLOW_STEP_C2_DDS)
#define FLOW_CTRL_RDP (FLOW_CTRL_C2_DDS ^ FLOW_STEP_RDP)
#define FLOW_CTRL_STATIC_PROTECT FLOW_CTRL_RDP

/**
  * @brief  SFU_BOOT Flow Control : Control values runtime protections
  */
#define FLOW_CTRL_MPU (FLOW_CTRL_STATIC_PROTECT ^ FLOW_STEP_MPU)
#define FLOW_CTRL_DMA (FLOW_CTRL_MPU ^ FLOW_STEP_DMA)
#define FLOW_CTRL_IWDG (FLOW_CTRL_DMA ^ FLOW_STEP_IWDG)
#define FLOW_CTRL_DAP (FLOW_CTRL_IWDG ^ FLOW_STEP_DAP)
#define FLOW_CTRL_TAMPER (FLOW_CTRL_DAP ^ FLOW_STEP_TAMPER)
#define FLOW_CTRL_C2SWDBG (FLOW_CTRL_TAMPER ^ FLOW_STEP_C2SWDBG)
#define FLOW_CTRL_GTZC (FLOW_CTRL_C2SWDBG ^ FLOW_STEP_GTZC)
#define FLOW_CTRL_RUNTIME_PROTECT FLOW_CTRL_GTZC

/**
  * @brief  SFU_BOOT Flow Control : Control values crypto operations
  */
#define FLOW_CTRL_AUTHENTICATE (FLOW_CTRL_INIT_VALUE ^ FLOW_STEP_AUTHENTICATE)
#define FLOW_CTRL_INTEGRITY (FLOW_CTRL_AUTHENTICATE ^ FLOW_STEP_INTEGRITY)
#define FLOW_CTRL_LOCK_SERVICE (FLOW_CTRL_INTEGRITY ^ FLOW_STEP_LOCK_SERVICE)

/* External variables --------------------------------------------------------*/
/**
  *  Flow control initial values.
  *   - Security protections flow (static + dynamic)
  *   - Crypto operations flow (authentication, integrity)
  */
extern uint32_t uFlowProtectValue;
extern uint32_t uFlowCryptoValue;

/* Exported macros -----------------------------------------------------------*/
/** Control with STEP operation :
  * (uFlowValue XOR STEP_VALUE) should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#define FLOW_CONTROL_STEP(C,B,A) \
  do{ \
    (C) ^= (B);\
    if ((C) != (A))\
    { \
      SFU_EXCPT_Security_Error();\
    } \
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

/** STEP update only :
  * (uFlowValue XOR STEP_VALUE)
  */
#define FLOW_STEP(B,A) \
  do{ \
    (B) ^= (A);\
  }while(0)


/* Exported functions ------------------------------------------------------- */
SFU_BOOT_InitErrorTypeDef SFU_BOOT_RunSecureBootService(void);
void SFU_BOOT_ForceReboot(void);

#ifdef __cplusplus
}
#endif

#endif /* SFU_BOOT_H */
