/**
  ******************************************************************************
  * @file    motion_mc.h
  * @author  MEMS Application Team
  * @version V2.5.1
  * @date    10-August-2021
  * @brief   Header for motion_mc module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016-2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MOTION_MC_H_
#define _MOTION_MC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup MIDDLEWARES
  * @{
  */

/** @defgroup MOTION_MC MOTION_MC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MMC_CALQSTATUSUNKNOWN = 0,
  MMC_CALQSTATUSPOOR,
  MMC_CALQSTATUSOK,
  MMC_CALQSTATUSGOOD
} MMC_CalQuality_t;

typedef struct {
  float Mag[3];  /* Magnetometer sensor output [uT] */
  int TimeStamp; /* Timestamp [ms] */
} MMC_Input_t;

typedef struct {
  float HI_Bias[3];            /* Hard iron offset array [uT] */
  float SF_Matrix[3][3];       /* Scale factor correction matrix [-] */
  MMC_CalQuality_t CalQuality; /* Calibration quality factor */
} MMC_Output_t;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @defgroup MOTION_MC_Exported_Functions MOTION_MC_Exported_Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialize the MotionMC engine
  * @param  sampletime  period in milliseconds [ms] between the update function call
  * @param  enable  Enable (1) or disable (0) library
  * @retval none
  */
void MotionMC_Initialize(int sampletime, unsigned short int enable);

/**
  * @brief  Run Magnetic Calibration algorithm
  * @param  data_in  Structure containing input data
  * @retval none
  */
void MotionMC_Update(MMC_Input_t *data_in);

/**
  * @brief  Get the magnetic compensation for hard/soft iron
  * @param  data_out  Structure containing output data
  * @retval none
  */
void MotionMC_GetCalParams(MMC_Output_t *data_out);

/**
  * @brief  Get the library version
  * @param  version  pointer to an array of 35 char
  * @retval Number of characters in the version string
  */
uint8_t MotionMC_GetLibVersion(char *version);

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

#endif /* _MOTION_MC_H_ */
