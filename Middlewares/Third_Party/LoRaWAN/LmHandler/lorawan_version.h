/**
  ******************************************************************************
  * @file    lorawan_version.h
  * @author  MCD Application Team
  * @brief   Identifies the version of LoRaWAN
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

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __LORAWAN_VERSION_H__
#define __LORAWAN_VERSION_H__

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define __LORAWAN_VERSION_MAIN   (0x02U) /*!< [31:24] main version */
#define __LORAWAN_VERSION_SUB1   (0x02U) /*!< [23:16] sub1 version */
#define __LORAWAN_VERSION_SUB2   (0x01U) /*!< [15:8]  sub2 version */
#define __LORAWAN_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */

#define __LORAWAN_MAIN_SHIFT     24
#define __LORAWAN_SUB1_SHIFT     16
#define __LORAWAN_SUB2_SHIFT     8
#define __LORAWAN_RC_SHIFT       0

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define __LORAWAN_VERSION        ((__LORAWAN_VERSION_MAIN  <<__LORAWAN_MAIN_SHIFT)  \
                                  |(__LORAWAN_VERSION_SUB1 << __LORAWAN_SUB1_SHIFT) \
                                  |(__LORAWAN_VERSION_SUB2 << __LORAWAN_SUB2_SHIFT )\
                                  |(__LORAWAN_VERSION_RC   << __LORAWAN_RC_SHIFT))
/* Exported functions ------------------------------------------------------- */ 

#ifdef __cplusplus
}
#endif

#endif /*__LORAWAN_VERSION_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
