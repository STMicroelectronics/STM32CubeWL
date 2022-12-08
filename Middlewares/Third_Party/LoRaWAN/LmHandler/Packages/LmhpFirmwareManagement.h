/**
  ******************************************************************************
  * @file    LmhpFirmwareManagement.h
  * @author  MCD Application Team
  * @brief   Implements the LoRa-Alliance Firmware Management package
  *          Specification: https://resources.lora-alliance.org/technical-specifications/ts006-1-0-0-firmware-management-protocol
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

#ifndef __LMHP_FIRMWARE_MANAGEMENT_H__
#define __LMHP_FIRMWARE_MANAGEMENT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LmhPackage.h"

/* Exported defines ----------------------------------------------------------*/
/*!
 * Remote multicast setup package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_FIRMWARE_MANAGEMENT               4

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
LmhPackage_t *LmhpFirmwareManagementPackageFactory( void );

#ifdef __cplusplus
}
#endif

#endif /* __LMHP_FIRMWARE_MANAGEMENT_H__ */
