/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_info.h
  * @author  MCD Application Team
  * @brief   To give info to the application about LoRaWAN configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

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
  uint32_t ContextManagement;  /*!< 0: not compiled in Mw, 1 : compiled in MW  */
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
  * @brief initialize the LoraInfo table
  */
void LoraInfo_Init(void);

/**
  * @brief returns the pointer to the LoraInfo capabilities table
  * @retval LoraInfo pointer
  */
LoraInfo_t *LoraInfo_GetPtr(void);

/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /* __LORA_INFO_H__ */
