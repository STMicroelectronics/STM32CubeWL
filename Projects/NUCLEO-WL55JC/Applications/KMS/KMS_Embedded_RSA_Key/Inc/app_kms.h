/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_kms.h
  * Description        : This file provides code for the configuration
  *                      of the kms instances.
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
#ifndef __app_kms_H
#define __app_kms_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN 0 */
#include <stdint.h>
#include <stdio.h>
#include "tkms.h"
/* USER CODE END 0 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* KMS init function */
void MX_KMS_Init(void);

/* USER CODE BEGIN 2 */
CK_RV app_kms_sign_verify_rsa(uint32_t length, uint8_t *pClearMessage);
/* USER CODE END 2 */

#ifdef __cplusplus
}
#endif
#endif /*__kms_H */

/**
  * @}
  */

/**
  * @}
  */
