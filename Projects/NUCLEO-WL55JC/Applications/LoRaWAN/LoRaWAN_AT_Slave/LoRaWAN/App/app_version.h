/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_version.h
  * @author  MCD Application Team
  * @brief   Definition the version of the application
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
#ifndef __APP_VERSION_H__
#define __APP_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define APP_VERSION_SUB1   (0x03U) /*!< [23:16] sub1 version */
#define APP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define APP_VERSION_MAIN_SHIFT 24  /*!< main byte shift */
#define APP_VERSION_SUB1_SHIFT 16  /*!< sub1 byte shift */
#define APP_VERSION_SUB2_SHIFT 8   /*!< sub2 byte shift */
#define APP_VERSION_RC_SHIFT   0   /*!< release candidate byte shift */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/**
  * @brief Application version
  */
#define APP_VERSION         ((APP_VERSION_MAIN  << APP_VERSION_MAIN_SHIFT)\
                             |(APP_VERSION_SUB1 << APP_VERSION_SUB1_SHIFT)\
                             |(APP_VERSION_SUB2 << APP_VERSION_SUB2_SHIFT)\
                             |(APP_VERSION_RC   << APP_VERSION_RC_SHIFT))

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__APP_VERSION_H__*/
