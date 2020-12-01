/**
  ******************************************************************************
  * @file    LmhpFirmwareManagement.h
  * @author  MCD Application Team
  * @brief   Implements the LoRa-Alliance Firmware Management package
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
LmhPackage_t *LmhpFirmwareManagementPackageFactory(void);


#ifdef __cplusplus
}
#endif

#endif /* __LMHP_FIRMWARE_MANAGEMENT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
