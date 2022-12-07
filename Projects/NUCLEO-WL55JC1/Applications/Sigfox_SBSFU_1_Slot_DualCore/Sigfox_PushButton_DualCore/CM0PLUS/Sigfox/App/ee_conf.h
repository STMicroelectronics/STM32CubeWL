/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ee_conf.h
  * @author  MCD Application Team
  * @brief   Header for eeprom configuration
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
#ifndef __EE_CONF_H__
#define __EE_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief Flash address
  */
#define HW_FLASH_ADDRESS                FLASH_BASE

/**
  * @brief Flash page size in bytes
  */
#define HW_FLASH_PAGE_SIZE              FLASH_PAGE_SIZE

/**
  * @brief Flash width in bytes
  */
#define HW_FLASH_WIDTH                  8

/**
  * @brief Flash bank0 size in bytes
  */
#define CFG_EE_BANK0_SIZE               2*HW_FLASH_PAGE_SIZE

/**
  * @brief Maximum number of data that can be stored in bank0
  */
#define CFG_EE_BANK0_MAX_NB             EE_ID_COUNT<<2 /*from uint32 to byte*/

/* Unused Bank1 */
/**
  * @brief Flash bank1 size in bytes
  */
#define CFG_EE_BANK1_SIZE              0

/**
  * @brief Maximum number of data that can be stored in bank1
  */
#define CFG_EE_BANK1_MAX_NB            0

/**
  * @brief EEPROM Flash address
  */
#define EE_BASE_ADRESS                  EE_DATASTORAGE_START

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__EE_CONF_H__ */
