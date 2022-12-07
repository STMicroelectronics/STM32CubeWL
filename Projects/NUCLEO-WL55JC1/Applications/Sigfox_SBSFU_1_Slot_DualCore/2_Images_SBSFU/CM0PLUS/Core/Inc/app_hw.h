/**
  ******************************************************************************
  * @file    app_hw.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update hardware
  *          interface.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_HW_H
#define APP_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros -----------------------------------------------------------*/
/* At power-on, the capacitor related to SW1 button needs to charge.
 * The charging time must be waited to be sure that a low level is a real one. */
#define BUTTON_INIT() \
  BSP_PB_Init(BUTTON_SW1,BUTTON_MODE_GPIO);                               \
  if ((uint32_t) BSP_PB_GetState(BUTTON_SW1) == (uint32_t)GPIO_PIN_RESET) \
  {                                                                       \
     HAL_Delay(6U);                                                       \
  }

#define BUTTON_PUSHED()      ((uint32_t) BSP_PB_GetState(BUTTON_SW1) == (uint32_t)GPIO_PIN_RESET)


#ifdef __cplusplus
}
#endif

#endif /* APP_HW_H */
