/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_kms.h
  * Description        : This file provides code for the configuration
  *                      of the kms instances.
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
CK_RV app_kms_encrypt_decrypt_aes_gcm(uint32_t length, uint8_t *pClearMessage);
CK_RV app_kms_encrypt_decrypt_aes_cbc(uint32_t length, uint8_t *pClearMessage);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
