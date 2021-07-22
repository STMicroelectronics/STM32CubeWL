/**
  ******************************************************************************
  * @file    ca_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains the low level definitions of the Cryptographic API (CA) module.
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
#ifndef CA_LOW_LEVEL_H
#define CA_LOW_LEVEL_H

/* Includes ------------------------------------------------------------------*/
/*
 * STM32 target HAL driver include
 * ex: stm32l4xx_hal.h
 */
#include "stm32wlxx_hal.h"

/* Exported constants --------------------------------------------------------*/
#if defined(CA_HAL_CRYPTOLIB_SUPP)
#if ((CA_ROUTE_AES_CBC & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
 || ((CA_ROUTE_AES_CCM & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
 || ((CA_ROUTE_AES_CMAC & CA_ROUTE_MASK) == CA_ROUTE_HAL) \
 || ((CA_ROUTE_AES_ECB & CA_ROUTE_MASK) == CA_ROUTE_HAL)  \
     || ((CA_ROUTE_AES_GCM & CA_ROUTE_MASK) == CA_ROUTE_HAL)
/*
 * AES/CRYP instance to use
 * ex: AES1
 */
#define CA_AES_INSTANCE     (AES)

#endif /* CA_ROUTE_AES_CBC || CA_ROUTE_AES_CCM || CA_ROUTE_AES_CMAC \
       || CA_ROUTE_AES_ECB || CA_ROUTE_AES_GCM => CA_ROUTE_HAL */


#if ((CA_ROUTE_ECC_ECDSA & CA_ROUTE_MASK) == CA_ROUTE_HAL) || ((CA_ROUTE_RSA & CA_ROUTE_MASK) == CA_ROUTE_HAL)
/*
 * PKA instance to use
 * ex: PKA
 */
#define CA_PKA_INSTANCE     (PKA)

#endif /* CA_ROUTE_ECC_ECDSA || CA_ROUTE_RSA => CA_ROUTE_HAL */

#endif /* CA_HAL_CRYPTOLIB_SUPP */


/* Exported function ---------------------------------------------------------*/
#if defined(CA_ST_CRYPTOLIB_SUPP)
void CA_LL_CRC_Init(void);

#endif /* CA_ST_CRYPTOLIB_SUPP */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CA_LOW_LEVEL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

