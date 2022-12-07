/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lorawan_conf.h
  * @author  MCD Application Team
  * @brief   Header for LoRaWAN middleware instances
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
#if defined(__ICCARM__)
#define SOFT_SE_PLACE_IN_NVM_START _Pragma(" default_variable_attributes = @ \".USER_embedded_Keys\"")
#elif defined(__CC_ARM)
#define SOFT_SE_PLACE_IN_NVM_START _Pragma("  arm section rodata = \".USER_embedded_Keys\"")
#elif defined(__GNUC__)
#define SOFT_SE_PLACE_IN_NVM_START __attribute__((section(".USER_embedded_Keys")))
#endif /* __ICCARM__ | __CC_ARM | __GNUC__ */

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#define SOFT_SE_PLACE_IN_NVM_STOP _Pragma("default_variable_attributes =")
#elif defined(__CC_ARM)
#define SOFT_SE_PLACE_IN_NVM_STOP _Pragma("arm section code")
#endif /* __ICCARM__ | __CC_ARM | __GNUC__ */

/*!
 * @brief LoRaWAN version definition
 * @note  possible values: 0x01000300 or 0x01000400
 */
#define LORAMAC_SPECIFICATION_VERSION                   0x01000400

/* Region ------------------------------------*/
/* the region listed here will be linked in the MW code */
/* the application (on sys_conf.h) shall just configure one region at the time */
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
#define CONTEXT_MANAGEMENT_ENABLED                      1

/* Class B ------------------------------------*/
#define LORAMAC_CLASSB_ENABLED                          0

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

/**
  * \brief Disable the ClassA receive windows after Tx (after the Join Accept if OTAA mode defined)
  * \note  Behavior to reduce power consumption but not compliant with LoRa Alliance recommendations.
  *        All device parameters (Spreading Factor, channels selection, Tx Power, ...) should be fixed
  *        and the adaptive datarate should be disabled.
  * /warning This limitation may have consequences for the proper functioning of the device,
             if the LoRaMac ever generates MAC commands that require a response.
  */
#define DISABLE_LORAWAN_RX_WINDOW                       0

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
