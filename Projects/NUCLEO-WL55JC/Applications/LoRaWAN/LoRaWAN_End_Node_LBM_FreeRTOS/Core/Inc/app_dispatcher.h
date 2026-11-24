/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_dispatcher.h
  * @brief   Application-wide dispatcher notification API
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

#ifndef __APP_DISPATCHER_H__
#define __APP_DISPATCHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

#define APP_DISPATCHER_LORA_FLAG               (1U << 0)

extern osThreadId_t LoraTaskHandle;
extern uint32_t WaitTimeoutSleep_time_ms;

#ifdef __cplusplus
}
#endif

#endif /* __APP_DISPATCHER_H__ */
