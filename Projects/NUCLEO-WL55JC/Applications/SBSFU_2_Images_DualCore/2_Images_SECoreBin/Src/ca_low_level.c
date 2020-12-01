/**
  ******************************************************************************
  * @file    ca_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains the low level implementations of the Cryptographic API (CA) module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CA_LOW_LEVEL_C
#define CA_LOW_LEVEL_C

/* Includes ------------------------------------------------------------------*/
#include "CryptoApi/ca.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Exported functions implementation -----------------------------------------*/

#if defined(CA_HAL_CRYPTOLIB_SUPP)

#if ((CA_ROUTE_AES_CBC & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
 || ((CA_ROUTE_AES_CCM & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
 || ((CA_ROUTE_AES_CMAC & CA_ROUTE_MASK) == CA_ROUTE_HAL) \
 || ((CA_ROUTE_AES_ECB & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
     || ((CA_ROUTE_AES_GCM & CA_ROUTE_MASK) == CA_ROUTE_HAL)

/**
  * @brief CRYP MSP Initialization
  * This function configures the hardware resources
  * @param hcryp: CRYP handle pointer
  * @retval None
  */
void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp)
{
  if (hcryp->Instance == CA_AES_INSTANCE)
  {
    /* Release AES/CRYP from reset state */
    __HAL_RCC_AES_RELEASE_RESET();
    /* Peripheral clock enable */
    __HAL_RCC_AES_CLK_ENABLE();
  }
}

/**
  * @brief CRYP MSP De-Initialization
  * This function freeze the hardware resources
  * @param hcryp: CRYP handle pointer
  * @retval None
  */
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp)
{
  if (hcryp->Instance == CA_AES_INSTANCE)
  {
    /* Peripheral clock disable */
    __HAL_RCC_AES_CLK_DISABLE();
    /* Force AES/CRYP into reset state */
    __HAL_RCC_AES_FORCE_RESET();
  }
}
#endif /* CA_ROUTE_AES_CBC || CA_ROUTE_AES_CCM || CA_ROUTE_AES_CMAC \
       || CA_ROUTE_AES_ECB || CA_ROUTE_AES_GCM => CA_ROUTE_HAL */


#if ((CA_ROUTE_ECC_ECDSA & CA_ROUTE_MASK) == CA_ROUTE_HAL) || ((CA_ROUTE_RSA & CA_ROUTE_MASK) == CA_ROUTE_HAL)
/**
  * @brief      PKA MSP Initialization
  *             This function configures the hardware resources used in this example
  * @param[in] hpka: PKA handle pointer
  * @retval None
  */
void HAL_PKA_MspInit(PKA_HandleTypeDef *hpka)
{
  if (hpka->Instance == CA_PKA_INSTANCE)
  {
    /* Release PKA from reset state */
    __HAL_RCC_PKA_RELEASE_RESET();
    /* Peripheral clock enable */
    __HAL_RCC_PKA_CLK_ENABLE();
  }
}

/**
  * @brief      PKA MSP De-Initialization
  *             This function freeze the hardware resources used in this example
  * @param[in] hpka: PKA handle pointer
  * @retval None
  */
void HAL_PKA_MspDeInit(PKA_HandleTypeDef *hpka)
{
  if (hpka->Instance == CA_PKA_INSTANCE)
  {
    /* Peripheral clock disable */
    __HAL_RCC_PKA_CLK_DISABLE();
    /* Force PKA into reset state */
    __HAL_RCC_PKA_FORCE_RESET();
  }
}
#endif /* CA_ROUTE_ECC_ECDSA || CA_ROUTE_RSA => CA_ROUTE_HAL */

#endif /* CA_HAL_CRYPTOLIB_SUPP */


#if defined(CA_ST_CRYPTOLIB_SUPP)
/**
  * @brief      CRC initialization
  *             This function release reset and clock CRC IP peripheral
  * @retval None
  */
void CA_LL_CRC_Init(void)
{
  /* Force CRC into reset state */
  __HAL_RCC_CRC_FORCE_RESET();
  /* Release CRC from reset state */
  __HAL_RCC_CRC_RELEASE_RESET();
  /* Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();
}

#endif /* CA_ST_CRYPTOLIB_SUPP */

#endif /* CA_LOW_LEVEL_C */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
