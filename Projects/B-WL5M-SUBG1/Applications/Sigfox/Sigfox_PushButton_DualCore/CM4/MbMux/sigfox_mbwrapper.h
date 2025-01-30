/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sigfox_mbwrapper.h
  * @author  MCD Application Team
  * @brief   This file implements the CM0 side wrapper of the SigfoxMac interface
  *          shared between M0 and M4.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIGFOX_MBWRAPPER_CM4_H__
#define __SIGFOX_MBWRAPPER_CM4_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mbmux_table.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/*!
  * Sigfox callback structure
*/
typedef struct sSigfoxCallback
{
  /*!
  * \brief   Measures the battery level
  *
  * \retval  Battery level [0: node is connected to an external
  *          power source, 1..254: battery level, where 1 is the minimum
  *          and 254 is the maximum value, 255: the node was not able
  *          to measure the battery level]
  */
  uint16_t (*GetBatteryLevel)(void);
  /*!
  * \brief   Measures the temperature level
  *
  * \retval  Temperature level
  */
  int16_t (*GetTemperatureLevel)(void);
} SigfoxCallback_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief This function processes the callbacks and primitives from Cm0
  * @param ComObj exchange buffer parameter
  */
void Process_Sigfox_Notif(MBMUX_ComParam_t *ComObj);

/**
  * @brief Register the callbacks structure for the process notification
  * @param SigfoxCallback struct of callbacks
  */
void Sigfox_Register(SigfoxCallback_t *SigfoxCallback);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__SIGFOX_MBWRAPPER_CM4_H__ */
