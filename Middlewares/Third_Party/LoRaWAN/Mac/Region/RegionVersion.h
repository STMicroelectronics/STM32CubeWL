/**
  ******************************************************************************
  * @file    RegionVersion.h
  * @author  MCD Application Team
  * @brief   Identifies the version of Regional Parameters
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
#ifndef __REGION_VERSION_H__
#define __REGION_VERSION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacVersion.h"

#ifndef REGION_VERSION
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * @brief Regional parameters version definition.
 * @note RP001-1.0.3 : https://lora-alliance.org/resource_hub/lorawan-regional-parameters-v1-0-3reva/
 */
#define REGION_VERSION                              0x01010003
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * @brief Regional parameters version definition.
 * @note RP002-1.0.1 : https://lora-alliance.org/resource_hub/rp2-101-lorawan-regional-parameters-2/
 */
#define REGION_VERSION                              0x02010001
#endif /* LORAMAC_VERSION */
#endif /* REGION_VERSION */

#ifdef __cplusplus
}
#endif

#endif // __REGION_VERSION_H__
