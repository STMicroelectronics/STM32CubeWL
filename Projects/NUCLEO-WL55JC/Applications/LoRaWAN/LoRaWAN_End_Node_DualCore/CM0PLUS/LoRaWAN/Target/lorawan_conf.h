/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lorawan_conf.h
  * @author  MCD Application Team
  * @brief   Header for LoRaWAN middleware instances
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORAWAN_CONF_H__
#define __LORAWAN_CONF_H__

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
/* To enable the KMS Middleware with LoRaWAN, you must update these files from the DualCore example project:
   - CM0PLUS/Core/Inc/kms_platf_objects_config.h : Add all LoRaWAN keys as kms_object_keyhead_32_t structures.
   - CM0PLUS/Core/Inc/kms_platf_objects_interface.h : Add all LoRaWAN key indexes
   - CM0PLUS/Core/Inc/nvms_low_level.h : Enable the NVMS (Non Volatile Memory) to store the session keys
   - CM0PLUS/Core/Src/nvms_low_level.c : Implement the Flash read/write functions to manage the NVMS items
   And finally, change the LORAWAN_KMS define to 1
*/
/* USER CODE BEGIN LORAWAN_KMS */
#define LORAWAN_KMS 1
/* USER CODE END LORAWAN_KMS */

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#define OVER_THE_AIR_ACTIVATION 1
#define ACTIVATION_BY_PERSONALIZATION 1
#endif /* LORAWAN_KMS */

/* Region ------------------------------------*/
/* the region listed here will be linked in the CM0 MW code */
/* the application (on CM4/sys_conf.h) shall just configure one region at the time */
/*#define REGION_AS923*/
/*#define REGION_AU915*/
/*#define REGION_CN470*/
/*#define REGION_CN779*/
/*#define REGION_EU433*/
#define REGION_EU868
/*#define REGION_KR920*/
/*#define REGION_IN865*/
#define REGION_US915
/*#define REGION_RU864*/

/**
  * \brief Limits the number usable channels by default for AU915, CN470 and US915 regions
  * \note the default channel mask with this option activates the first 8 channels. \
  *       this default mask can be modified in the RegionXXXXXInitDefaults function associated with the active region.
  */
#define HYBRID_ENABLED                                  0

/**
  * \brief Define the read access of the keys in memory
  * \note this value should be disabled after the development process
  */
#define KEY_EXTRACTABLE                                 1

/*!
 * Enables/Disables the context storage management storage.
 * Must be enabled for LoRaWAN 1.0.4 or later.
 */
#define CONTEXT_MANAGEMENT_ENABLED                      0

/* Class B ------------------------------------*/
#define LORAMAC_CLASSB_ENABLED  0

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/* CLASS B LSE crystal calibration*/
/**
  * \brief Temperature coefficient of the clock source
  */
#define RTC_TEMP_COEFFICIENT                            ( -0.035 )

/**
  * \brief Temperature coefficient deviation of the clock source
  */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035 )

/**
  * \brief Turnover temperature of the clock source
  */
#define RTC_TEMP_TURNOVER                               ( 25.0 )

/**
  * \brief Turnover temperature deviation of the clock source
  */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0 )
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#ifndef CRITICAL_SECTION_BEGIN
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_BEGIN */
#ifndef CRITICAL_SECTION_END
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_END */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __LORAWAN_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
