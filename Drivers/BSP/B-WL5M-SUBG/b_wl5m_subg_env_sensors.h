/**
  ******************************************************************************
  * @file    b_wl5m_subg_env_sensors.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the b_wl5m_subg_env_sensors.c driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_WL5M_SUBG_ENV_SENSORS_H
#define B_WL5M_SUBG_ENV_SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "b_wl5m_subg_conf.h"
#include "b_wl5m_subg_errno.h"
#include "env_sensor.h"

#ifndef USE_ENV_SENSOR_STTS22H_0
#define USE_ENV_SENSOR_STTS22H_0          1
#endif /* USE_ENV_SENSOR_STTS22H_0 */
  
#ifndef USE_ENV_SENSOR_ILPS22QS_0
#define USE_ENV_SENSOR_ILPS22QS_0         1
#endif /* USE_ENV_SENSOR_ILPS22QS_0 */

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#include "stts22h.h"
#endif /* (USE_ENV_SENSOR_STTS22H_0 == 1) */

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
#include "ilps22qs.h"
#endif /* (USE_ENV_SENSOR_ILPS22QS_0 == 1) */

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG
  * @{
  */

/** @addtogroup B_WL5M_SUBG_ENV_SENSORS
  * @{
  */

/** @defgroup B_WL5M_SUBG_ENV_SENSORS_Exported_Types B-WL5M-SUBG ENV SENSORS Exported Types
  * @{
  */

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;

/**
  * @}
  */

/** @defgroup B_WL5M_SUBG_ENV_SENSORS_Exported_Constants B-WL5M-SUBG ENV SENSORS Exported Constants
  * @{
  */
#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#define ENV_SENSOR_STTS22H_0 0
#endif /* (USE_ENV_SENSOR_STTS22H_0 == 1) */

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
#define ENV_SENSOR_ILPS22QS_0 1
#endif /* (USE_ENV_SENSOR_ILPS22QS_0 == 1) */

#ifndef ENV_TEMPERATURE
#define ENV_TEMPERATURE      1U
#endif /* ENV_TEMPERATURE */

#ifndef ENV_PRESSURE
#define ENV_PRESSURE         2U
#endif /* ENV_PRESSURE */

#ifndef ENV_HUMIDITY
#define ENV_HUMIDITY         4U
#endif /* ENV_HUMIDITY */

#define ENV_SENSOR_FUNCTIONS_NBR    2U

#define ENV_SENSOR_INSTANCES_NBR    ((USE_ENV_SENSOR_STTS22H_0) \
                                     + (USE_ENV_SENSOR_ILPS22QS_0))


#if (ENV_SENSOR_INSTANCES_NBR == 0)
#warning "No environmental sensor instance has been selected"
#endif /* (ENV_SENSOR_INSTANCES_NBR == 0) */

/**
  * @}
  */

/** @addtogroup B_WL5M_SUBG_ENV_SENSORS_Exported_Functions
  * @{
  */

int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* B_WL5M_SUBG_ENV_SENSORS_H */
