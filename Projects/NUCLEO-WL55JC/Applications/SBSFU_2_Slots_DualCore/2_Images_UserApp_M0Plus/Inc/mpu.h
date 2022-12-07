/**
  ******************************************************************************
  * @file    mpu.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for MPU module functionalities.
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
#ifndef MPU_H
#define MPU_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* applicative sys call */
#define SFU_ISOLATE_SE_WITH_MPU


/* Exported functions ------------------------------------------------------- */
void MPU_EnterUnprivilegedMode(void);
void SVC_NVIC_SystemReset(void);
void SVC_MPU_Disable(void);
void MPU_SVC_Handler(uint32_t *args);
void HAL_APP_SVC_Handler(uint32_t *args);

#ifdef __cplusplus
}
#endif

#endif /* MPU_H */
