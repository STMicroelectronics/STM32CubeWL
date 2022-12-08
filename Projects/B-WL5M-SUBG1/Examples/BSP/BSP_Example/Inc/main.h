/**
  ******************************************************************************
  * @file    BSP/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Defines -------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "b_wl5m_subg.h"
#include "b_wl5m_subg_eeprom.h"
#include "b_wl5m_subg_env_sensors.h"
#include "b_wl5m_subg_env_sensors_ex.h"
#include "b_wl5m_subg_motion_sensors.h"
#include "b_wl5m_subg_flash.h"
#include "b_wl5m_subg_radio.h"
#include "b_wl5m_subg_errno.h"
#include "b_wl5m_subg_conf.h"
#include "b_wl5m_subg_bus.h"
#include "stm32wlxx.h"
#include "stm32wlxx_it.h"

#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[50];
  uint32_t DemoIndex;
} BSP_DemoTypeDef;

/* Exported variables --------------------------------------------------------*/
extern __IO uint32_t ButtonState;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypeDef))

/* Exported functions ------------------------------------------------------- */
void Temperature_demo(void);
void Pressure_demo(void);
void Gyro_demo(void);
void Accelero_demo(void);
void Magneto_demo(void);
uint8_t CheckForUserInput(void);
void Led_demo(void);
void Flash_demo(void);
void EEPROM_demo(void);
void Radio_demo (void);

void Error_Handler(void);
uint32_t Serial_Scanf(uint32_t value);

#endif /* MAIN_H */
