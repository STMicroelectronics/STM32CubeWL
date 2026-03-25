/**
  ******************************************************************************
  * @file    RegionVersion.h
  * @author  MCD Application Team
  * @brief   Identifies the version of Regional Parameters
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

//  #include "LoRaMacVersion.h"

#ifndef REGION_VERSION
/*!
 * Regional parameters version definition.
 * RP002-1.0.3 - RP002-1.0.4
 */
#define REGION_VERSION                              0x02010004
#endif /* REGION_VERSION */

#ifdef __cplusplus
}
#endif

#endif // __REGION_VERSION_H__
