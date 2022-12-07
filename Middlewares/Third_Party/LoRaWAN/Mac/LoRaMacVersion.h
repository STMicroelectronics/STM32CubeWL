/**
  ******************************************************************************
  * @file    LoRaMacVersion.h
  * @author  MCD Application Team
  * @brief   Identifies the version of LoRaMAC
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
#ifndef __LORAMAC_VERSION_H__
#define __LORAMAC_VERSION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "lorawan_conf.h"

#ifndef LORAMAC_VERSION
/*!
 * @brief LoRaWAN version definition.
 * @note TS001-1.0.4 : https://lora-alliance.org/resource_hub/lorawan-104-specification-package/
 */
#define LORAMAC_VERSION                             LORAMAC_SPECIFICATION_VERSION
#endif

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_VERSION_H__
