/**
  ******************************************************************************
  * @file    lora_info_template.h
  * @author  MCD Application Team
  * @brief   To give info to the application about LoRaWAN configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#ifndef __LORA_INFO_H__
#define __LORA_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported types ------------------------------------------------------------*/
/*!
 * To give info to the application about LoraWAN capability
 * it can depend how it has been compiled (e.g. compiled regions ...)
 * Params should be better uint32_t foe easier alignment with info_table concept
 */
typedef struct
{
  uint32_t ActivationMode;  /*!< 1: ABP, 2 : OTAA, 3: ABP & OTAA   */
  uint32_t Region;   /*!< Combination of regions compiled on MW  */
  uint32_t ClassB;   /*!< 0: not compiled in Mw, 1 : compiled in MW  */
  uint32_t Kms;      /*!< 0: not compiled in Mw, 1 : compiled in MW  */
} LoraInfo_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief initialises the LoraInfo table
  * @param none
  * @retval  none
  */
void LoraInfo_Init(void);

/**
  * @brief returns the pointer to the LoraInfo capabilities table
  * @param none
  * @retval LoraInfo pointer
  */
LoraInfo_t *LoraInfo_GetPtr(void);

/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /* __LORA_INFO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
