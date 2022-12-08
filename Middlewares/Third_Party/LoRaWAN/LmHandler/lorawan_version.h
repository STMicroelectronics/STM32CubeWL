/**
  ******************************************************************************
  * @file    lorawan_version.h
  * @author  MCD Application Team
  * @brief   Identifies the version of LoRaWAN
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020(-2021) STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __LORAWAN_VERSION_H__
#define __LORAWAN_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define LORAWAN_VERSION_MAIN   (0x02U) /*!< [31:24] main version */
#define LORAWAN_VERSION_SUB1   (0x05U) /*!< [23:16] sub1 version */
#define LORAWAN_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define LORAWAN_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define LORAWAN_MAIN_SHIFT     24
#define LORAWAN_SUB1_SHIFT     16
#define LORAWAN_SUB2_SHIFT     8
#define LORAWAN_RC_SHIFT       0

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define LORAWAN_VERSION        ((LORAWAN_VERSION_MAIN  << LORAWAN_MAIN_SHIFT)\
                                |(LORAWAN_VERSION_SUB1 << LORAWAN_SUB1_SHIFT)\
                                |(LORAWAN_VERSION_SUB2 << LORAWAN_SUB2_SHIFT)\
                                |(LORAWAN_VERSION_RC   << LORAWAN_RC_SHIFT))
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /*__LORAWAN_VERSION_H__*/
