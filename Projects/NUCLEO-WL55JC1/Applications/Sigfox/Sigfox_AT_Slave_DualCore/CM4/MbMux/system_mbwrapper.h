/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    system_mbwrapper.h
  * @author  MCD Application Team
  * @brief   This file implements the CM4 side wrapper of the Radio interface
  *          shared between M0 and M4.
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
#ifndef __SYSTEM_MBWRAPPER_CM4_H__
#define __SYSTEM_MBWRAPPER_CM4_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sgfx_eeprom_if.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief   Returns the last stored variable data, if found, which corresponds to
  *          the passed virtual address
  * @param   EEsgfxID ID of EEPROM Sigfox variable
  * @param   data to be read
  *
  * @retval  error status
  */
uint32_t SYS_EE_ReadBuffer_mbwrapper(e_EE_ID EEsgfxID, uint32_t *data);

/**
  * \brief   Writes/updates variable data in EEPROM emulator.
  * @param   EEsgfxID ID of EEPROM Sigfox variable
  * @param   data to be written
  *
  * @retval error status
  */
uint32_t SYS_EE_WriteBuffer_mbwrapper(e_EE_ID EEsgfxID, uint32_t data);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_MBWRAPPER_CM4_H__*/
