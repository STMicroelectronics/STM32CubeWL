/**
  ******************************************************************************
  * @file    mpu.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for MPU module functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
