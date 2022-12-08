/**
  ******************************************************************************
  * @file    b_wl5m_subg_env_sensors_ex.h
  * @author  MEMS Software Solutions Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the b_wl5m_subg_env_sensors_ex.c driver.
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
#ifndef B_WL5M_SUBG_ENV_SENSOR_EX_H
#define B_WL5M_SUBG_ENV_SENSOR_EX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "b_wl5m_subg_env_sensors.h"

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG B-WL5M-SUBG
  * @{
  */

/** @addtogroup B_WL5M_SUBG_ENV_SENSOR_EX B-WL5M-SUBG ENV SENSOR EX
  * @{
  */

/** @addtogroup B_WL5M_SUBG_ENV_SENSOR_EX_Exported_Functions B-WL5M-SUBG ENV SENSOR EX Exported Functions
  * @{
  */

int32_t BSP_ENV_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);
int32_t BSP_ENV_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t BSP_ENV_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);
int32_t BSP_ENV_SENSOR_FIFO_Get_Data(uint32_t Instance, float *Press, float *Temp);
int32_t BSP_ENV_SENSOR_FIFO_Get_Fth_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint8_t *NumSamples);
int32_t BSP_ENV_SENSOR_FIFO_Get_Ovr_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_FIFO_Reset_Interrupt(uint32_t Instance, uint8_t Interrupt);
int32_t BSP_ENV_SENSOR_FIFO_Set_Interrupt(uint32_t Instance, uint8_t Interrupt);
int32_t BSP_ENV_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode);
int32_t BSP_ENV_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint8_t Watermark);
int32_t BSP_ENV_SENSOR_FIFO_Stop_On_Watermark(uint32_t Instance, uint8_t Stop);
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance);
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status);

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

#endif /* B_WL5M_SUBG_ENV_SENSOR_EX_H */
