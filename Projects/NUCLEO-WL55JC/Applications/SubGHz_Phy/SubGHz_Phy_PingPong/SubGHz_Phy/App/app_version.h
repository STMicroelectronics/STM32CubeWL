/**
  ******************************************************************************
  * @file    app_version.h
  * @author  MCD Application Team
  * @brief   Definition the version of the application
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
#define __APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define __APP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define __APP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define __APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define __APP_VERSION_MAIN_SHIFT 24  /*!< main byte shift */
#define __APP_VERSION_SUB1_SHIFT 16  /*!< sub1 byte shift */
#define __APP_VERSION_SUB2_SHIFT 8   /*!< sub2 byte shift */
#define __APP_VERSION_RC_SHIFT   0   /*!< release candidate byte shift */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/**
  * @brief Application version
  */
#define __APP_VERSION         ((__APP_VERSION_MAIN  << __APP_VERSION_MAIN_SHIFT)\
                               |(__APP_VERSION_SUB1 << __APP_VERSION_SUB1_SHIFT)\
                               |(__APP_VERSION_SUB2 << __APP_VERSION_SUB2_SHIFT)\
                               |(__APP_VERSION_RC   << __APP_VERSION_RC_SHIFT))

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__APP_VERSION_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
