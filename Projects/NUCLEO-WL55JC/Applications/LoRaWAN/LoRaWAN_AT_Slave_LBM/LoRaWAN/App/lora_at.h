/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_at.h
  * @author  MCD Application Team
  * @brief   Header for LoRaWAN AT command handlers
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORA_AT_H__
#define __LORA_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "smtc_modem_api.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_NO_NET_JOINED,
} ATEerror_t;

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

/* Exported functions prototypes ---------------------------------------------*/
ATEerror_t AT_LoraGetVersion(void);
ATEerror_t AT_LoraReset(void);
ATEerror_t AT_LoraRestoreFactorySettings(void);
void LoRaWAN_RestoreFactorySettings(void);
ATEerror_t AT_LoraStoreContext(void);
ATEerror_t AT_LoraPrintCommandHelp(const char *command);
ATEerror_t AT_LoraGetVerboseLevel(void);
ATEerror_t AT_LoraGetLocalTime(void);
ATEerror_t AT_LoraSetVerboseLevel(const char *value);
ATEerror_t AT_LoraGetStatus(void);
ATEerror_t AT_LoraGetAdrProfile(void);
ATEerror_t AT_LoraSetAdrProfile(const char *value);
ATEerror_t AT_LoraGetDataRate(void);
ATEerror_t AT_LoraSetDataRate(const char *value);
ATEerror_t AT_LoraGetDutyCycleStatus(void);
ATEerror_t AT_LoraGetBatteryVoltage(void);
ATEerror_t AT_LoraGetPingSlotPeriodicity(void);
ATEerror_t AT_LoraSetPingSlotPeriodicity(const char *value);
ATEerror_t AT_LoraGetDevEui(void);
ATEerror_t AT_LoraSetDevEui(const char *value);
ATEerror_t AT_LoraGetJoinEui(void);
ATEerror_t AT_LoraSetJoinEui(const char *value);
ATEerror_t AT_LoraSetAppKey(const char *value);
ATEerror_t AT_LoraSetNwkKey(const char *value);
ATEerror_t AT_LoraSetNwkSessionKey(const char *value);
ATEerror_t AT_LoraSetAppSessionKey(const char *value);
ATEerror_t AT_LoraGetDevAddr(void);
ATEerror_t AT_LoraSetDevAddr(const char *value);
ATEerror_t AT_LoraGetRegion(void);
ATEerror_t AT_LoraSetRegion(const char *value);
ATEerror_t AT_LoraJoin(const char *value);
ATEerror_t AT_LoraSend(const char *value);
ATEerror_t AT_LoraGetClass(void);
ATEerror_t AT_LoraSetClass(const char *value);

void AT_PrintError(ATEerror_t error);
void AT_PrintHelp(void);
void AT_EventReady(void);
void AT_EventJoined(void);
void AT_EventJoinFailed(void);
void AT_EventTxDone(smtc_modem_event_txdone_status_t status);
void AT_EventDownlink(const uint8_t *payload, uint8_t size, const smtc_modem_dl_metadata_t *metadata);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __LORA_AT_H__ */
