/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ca_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains the low level implementations of the Cryptographic API (CA) module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CA_LOW_LEVEL_C
#define CA_LOW_LEVEL_C

/* Includes ------------------------------------------------------------------*/
#include "ca.h"
/* USER CODE BEGIN KMS_CA_LOW_LEVEL_Includes */
/* USER CODE END KMS_CA_LOW_LEVEL_Includes */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN KMS_CA_LOW_LEVEL_Private_Defines */
/* USER CODE END KMS_CA_LOW_LEVEL_Private_Defines */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN KMS_CA_LOW_LEVEL_Private_Typedef */
/* USER CODE END KMS_CA_LOW_LEVEL_Private_Typedef */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN KMS_CA_LOW_LEVEL_Private_Function_Prototypes */
/* USER CODE END KMS_CA_LOW_LEVEL_Private_Function_Prototypes */

/* Exported functions implementation -----------------------------------------*/
/* USER CODE BEGIN KMS_CA_LOW_LEVEL_Exported_Function_Implementation */
/* USER CODE END KMS_CA_LOW_LEVEL_Exported_Function_Implementation */

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
  /* USER CODE BEGIN HAL_CRYP_MspInit */
  if (hcryp->Instance == CA_AES_INSTANCE)
  {
    /* Release AES/CRYP from reset state */
    __HAL_RCC_AES_RELEASE_RESET();
    /* Peripheral clock enable */
    __HAL_RCC_AES_CLK_ENABLE();
  }
  /* USER CODE END HAL_CRYP_MspInit */
}

/**
  * @brief CRYP MSP De-Initialization
  * This function freeze the hardware resources
  * @param hcryp: CRYP handle pointer
  * @retval None
  */
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp)
{
  /* USER CODE BEGIN HAL_CRYP_MspDeInit */
  if (hcryp->Instance == CA_AES_INSTANCE)
  {
    /* Peripheral clock disable */
    __HAL_RCC_AES_CLK_DISABLE();
    /* Force AES/CRYP into reset state */
    __HAL_RCC_AES_FORCE_RESET();
  }
  /* USER CODE END HAL_CRYP_MspDeInit */
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
  /* USER CODE BEGIN HAL_PKA_MspInit */
  if (hpka->Instance == CA_PKA_INSTANCE)
  {
    /* Release PKA from reset state */
    __HAL_RCC_PKA_RELEASE_RESET();
    /* Peripheral clock enable */
    __HAL_RCC_PKA_CLK_ENABLE();
  }
  /* USER CODE END HAL_PKA_MspInit */
}

/**
  * @brief      PKA MSP De-Initialization
  *             This function freeze the hardware resources used in this example
  * @param[in] hpka: PKA handle pointer
  * @retval None
  */
void HAL_PKA_MspDeInit(PKA_HandleTypeDef *hpka)
{
  /* USER CODE BEGIN HAL_PKA_MspDeInit */
  if (hpka->Instance == CA_PKA_INSTANCE)
  {
    /* Peripheral clock disable */
    __HAL_RCC_PKA_CLK_DISABLE();
    /* Force PKA into reset state */
    __HAL_RCC_PKA_FORCE_RESET();
  }
  /* USER CODE END HAL_PKA_MspDeInit */
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
  /* USER CODE BEGIN CA_LL_CRC_Init */
  /* Release CRC from reset state */
  __HAL_RCC_CRC_RELEASE_RESET();
  /* Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();
  /* USER CODE END CA_LL_CRC_Init */
}

#endif /* CA_ST_CRYPTOLIB_SUPP */

#endif /* CA_LOW_LEVEL_C */
