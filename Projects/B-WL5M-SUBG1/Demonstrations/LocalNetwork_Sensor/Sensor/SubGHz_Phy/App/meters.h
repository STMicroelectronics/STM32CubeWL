/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    meters.h
  * @author  MCD Application Team
  * @brief   Header for metering module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef METERS_H_
#define METERS_H_

#include <stdint.h>
#include <stdbool.h>
#include <meters_conf.h>
#include <demo_data_format.h>
#include <stm32wlxx_hal.h>

/**
  * @brief Initialize sensors.
  * @return nonzero on error
  */
uint32_t Meters_Init(void);

/**
  * @brief De-initialize sensors.
  */
void Meters_DeInit(void);

/**
  * @brief Do last minute calculations and get measured data ready to be sent.
  * @param Data pointer to data to be written
  * @param NextGet after how long will the next call of this functions be [ms]
  */
void Meters_GetData(DEMO_data_1_1_t *Data, uint32_t NextGet);


#endif /* METERS_H_ */
