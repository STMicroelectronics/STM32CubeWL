/**
  ******************************************************************************
  * @file    motion_ec.h
  * @author  MEMS Application Team
  * @version V1.4.1
  * @date    10-August-2021
  * @brief   Header for motion_ec module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MOTION_EC_H_
#define _MOTION_EC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup MIDDLEWARES
  * @{
  */

/** @defgroup MOTION_EC MOTION_EC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MEC_MCU_STM32 = 0,
  MEC_MCU_BLUE_NRG1,
  MEC_MCU_BLUE_NRG2,
  MEC_MCU_BLUE_NRG_LP,
} MEC_mcu_type_t;

typedef struct
{
  float acc[3];        /* Accelerometer data [g] */
  float mag[3];        /* Magnetometer data [uT / 50] */
  float deltatime_s;   /* Time between 2 library calls in [s] */
} MEC_input_t;

typedef struct
{
  float quaternion[4]; /* Quaternion [x, y, z, w] */
  float euler[3];      /* Yaw, Pitch, Roll [deg] */
  float i_gyro[3];     /* Virtual gyroscope [dps] */
  float gravity[3];    /* Gravity [g] */
  float linear[3];     /* Linear acceleration [g] */
} MEC_output_t;

typedef enum
{
  MEC_DISABLE = 0,
  MEC_ENABLE  = 1
} MEC_state_t;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @defgroup MOTION_EC_Exported_Functions MOTION_EC_Exported_Functions
  * @{
  */

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialize and reset the MotionEC engine
  * @param  mcu_type MCU type
  * @param  freq  Sensors sampling frequency [Hz]
  * @retval None
  */
void MotionEC_Initialize(MEC_mcu_type_t mcu_type, float *freq);


/**
  * @brief  Run E-Compass algorithm (accelerometer and magnetometer data fusion)
  * @param  data_in  Structure containing input data
  * @param  data_out  Structure containing output data
  * @retval None
  */
void MotionEC_Run(MEC_input_t *data_in, MEC_output_t *data_out);


/**
  * @brief  Get enable/disable state of the Euler angles calculation
  * @param  state  Current enable/disable state
  * @retval None
  */
void MotionEC_GetOrientationEnable(MEC_state_t *state);


/**
  * @brief  Set enable/disable state of the Euler angles calculation
  * @param  state  New enable/disable state to be set
  * @retval None
  */
void MotionEC_SetOrientationEnable(MEC_state_t state);


/**
  * @brief  Get enable/disable state of the virtual gyroscope calculation
  * @param  state  Current enable/disable state
  * @retval None
  */
void MotionEC_GetVirtualGyroEnable(MEC_state_t *state);


/**
  * @brief  Set enable/disable state of the virtual gyroscope calculation
  * @param  state  New enable/disable state to be set
  * @retval None
  */
void MotionEC_SetVirtualGyroEnable(MEC_state_t state);


/**
  * @brief  Get enable/disable state of the gravity vector calculation
  * @param  state  Current enable/disable state
  * @retval None
  */
void MotionEC_GetGravityEnable(MEC_state_t *state);


/**
  * @brief  Set enable/disable state of the gravity vector calculation
  * @param  state  New enable/disable state to be set
  * @retval None
  */
void MotionEC_SetGravityEnable(MEC_state_t state);


/**
  * @brief  Get enable/disable state of the linear acceleration calculation
  * @param  state  Current enable/disable state
  * @retval None
  */
void MotionEC_GetLinearAccEnable(MEC_state_t *state);


/**
  * @brief  Set enable/disable state of the linear acceleration calculation
  * @param  state  New enable/disable state to be set
  * @retval None
  */
void MotionEC_SetLinearAccEnable(MEC_state_t state);


/**
  * @brief  Set sampling frequency (modify filtering parameters)
  * @param  freq  New sensors sampling frequency [Hz]
  * @retval None
  */
void MotionEC_SetFrequency(float freq);


/**
  * @brief  Get the library version
  * @param  version  pointer to an array of 35 char
  * @retval Number of characters in the version string
  */
uint8_t MotionEC_GetLibVersion(char *version);

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
#endif

#endif /* _MOTION_EC_H_ */
