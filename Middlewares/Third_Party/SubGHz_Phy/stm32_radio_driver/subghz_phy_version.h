/******************************************************************************
  * @file    subghz_phy_version.h
  * @author  MCD Application Team
  * @brief   defines the radio driver version
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

#ifndef __SUBGHZ_PHY_VERSION_H__
#define __SUBGHZ_PHY_VERSION_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* __SUBGHZ_PHY_TYPE: 0x01  STM32WL
                      0x61  SX126X
                      0x72  SX1272
                      0x76  SX1276 */

#define SUBGHZ_PHY_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define SUBGHZ_PHY_VERSION_SUB1   (0x03U) /*!< [23:16] sub1 version */
#define SUBGHZ_PHY_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define SUBGHZ_PHY_TYPE           (0x01U) /*!< [7:0] type version */
#define SUBGHZ_PHY_VERSION        ((SUBGHZ_PHY_VERSION_MAIN  << 24) \
                                   |(SUBGHZ_PHY_VERSION_SUB1 << 16) \
                                   |(SUBGHZ_PHY_VERSION_SUB2 << 8)  \
                                   |(SUBGHZ_PHY_TYPE))

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /*__SUBGHZ_PHY_VERSION_H__*/
