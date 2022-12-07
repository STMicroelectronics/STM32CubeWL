/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sys.h
  * @author  MCD Application Team
  * @brief   API for CM0PLUS application to handle the SYSTEM MBMUX channel
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
#ifndef __MBMUXIF_SYS_CM0PLUS_H__
#define __MBMUXIF_SYS_CM0PLUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mbmux.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief Inits the MBMUX and registers SYS channel to the mailbox and SYS task to the sequencer
  * @retval   0: OK;
             -1: no more ipcc channel available;
             -2: feature not provided by CM0PLUS;
             -3: callback error on CM0PLUS
  */
int8_t MBMUXIF_SystemInit(void);

/**
  * @brief   Get synchro flag between the two CPUs
  * @return  flag: 0xFFFF: hold Cm0 before it runs its MBMUX_Init,
  *                0x5555: Cm0 allowed to run its MBMUX_Init
  *                0xAAAA: Cm0 has completed initialization
  *                0x9999: RTC PRIO CHANNEL has been registered
  */
uint16_t MBMUXIF_GetCpusSynchroFlag(void);

/**
  * @brief   gives back the pointer to the com buffer associated to System feature Notif
  * @param   SystemPrioFeat ID of the feature
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureNotifComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Sends a System-Notif via Ipcc and Wait for the ack
  * @param   SystemPrioFeat ID of the feature
  */
void MBMUXIF_SystemSendNotif(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Sends a Trace-Notif via Ipcc without waiting for the ack
  * @param   SystemPrioFeat ID of the feature
  */
void MBMUXIF_SystemSendNotif_NoWait(FEAT_INFO_IdTypeDef SystemPrioFeat);

/**
  * @brief   Sends a System-Resp  via Ipcc without waiting for the response
  * @param   SystemPrioFeat ID of the feature
  */
void MBMUXIF_SystemSendResp(FEAT_INFO_IdTypeDef SystemPrioFeat);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Exported services --------------------------------------------------------*/
/**
  * @brief   Get the chip revision ID from Mbmux table
  * @retval  Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
int16_t MBMUXIF_ChipRevId(void);

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_SYS_CM0PLUS_H__ */
