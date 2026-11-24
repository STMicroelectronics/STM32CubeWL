/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_at.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "lora_at.h"
#include "app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "adc_if.h"
#include "flash_if.h"
#include "lorawan_api.h"
#include "lora_app.h"
#include "stm32_adv_trace.h"
#include "stm32_systime.h"
#include "smtc_secure_element.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define AT_STACK_ID 0U
#define LORAWAN_NVM_BASE_ADDRESS (0x0803F000UL)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
#define AT_PRINTF(...) do { (void)UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__); } while (0)

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static ATEerror_t AT_FromModemStatus(smtc_modem_return_code_t status);
static bool AT_ParseHex(const char *text, uint8_t *output, uint8_t expectedSize);
static int8_t AT_HexNibble(char character);
static void AT_PrintHex(const char *prefix, const uint8_t *data, uint8_t size);
static void AT_PrintHelpLine(const char *line);
static bool AT_MapRegion(uint32_t legacyRegion, smtc_modem_region_t *region);
static bool AT_UnmapRegion(smtc_modem_region_t region, uint8_t *legacyRegion);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
ATEerror_t AT_LoraGetVersion(void)
{
  lr1mac_version_t lorawanVersion = lorawan_api_get_spec_version(AT_STACK_ID);
  lr1mac_version_t regionalParametersVersion = lorawan_api_get_regional_parameters_version(AT_STACK_ID);

  AT_PRINTF("+VER:APPLICATION_VERSION=V%X.%X.%X\r\n", APP_VERSION_MAIN, APP_VERSION_SUB1, APP_VERSION_SUB2);
  AT_PRINTF("+VER:MW_LORAWAN_VERSION=V%X.%X.%X\r\n",
            LORAWAN_VERSION_MAIN, LORAWAN_VERSION_SUB1, LORAWAN_VERSION_SUB2);
  AT_PRINTF("+VER:MW_RADIO_VERSION=V%X.%X.%X\r\n",
            SUBGHZ_PHY_VERSION_MAIN, SUBGHZ_PHY_VERSION_SUB1, SUBGHZ_PHY_VERSION_SUB2);
  AT_PRINTF("+VER:L2_SPEC_VERSION=V%X.%X.%X\r\n",
            lorawanVersion.major, lorawanVersion.minor, lorawanVersion.patch);
  AT_PRINTF("+VER:RP_SPEC_VERSION=V%X-%X.%X.%X\r\n",
            regionalParametersVersion.revision, regionalParametersVersion.major,
            regionalParametersVersion.minor, regionalParametersVersion.patch);

  return AT_OK;
}

ATEerror_t AT_LoraReset(void)
{
  __disable_irq();
  HAL_NVIC_SystemReset();
  return AT_OK;
}

void LoRaWAN_RestoreFactorySettings(void)
{
  if (FLASH_IF_Erase((void *)LORAWAN_NVM_BASE_ADDRESS, FLASH_PAGE_SIZE) == FLASH_IF_OK)
  {
    __disable_irq();
    HAL_NVIC_SystemReset();
  }
}

ATEerror_t AT_LoraRestoreFactorySettings(void)
{
  LoRaWAN_RestoreFactorySettings();
  return AT_OK;
}

ATEerror_t AT_LoraStoreContext(void)
{
  AT_PRINTF("NVM DATA UP TO DATE\r\n");
  return AT_OK;
}

ATEerror_t AT_LoraPrintCommandHelp(const char *command)
{
  if (strcmp(command, "AT+VER?") == 0)
  {
    AT_PRINTF("AT+VER=?       : Get application and middleware versions\r\n");
  }
  else if (strcmp(command, "AT+VL?") == 0)
  {
    AT_PRINTF("AT+VL=<Level><CR>. Set the Verbose Level=[0:Off .. 3:High]\r\n");
    AT_PRINTF("AT+VL=?       : Get the current verbose level\r\n");
  }
  else if (strcmp(command, "AT+LTIME?") == 0)
  {
    AT_PRINTF("AT+LTIME=?    : Get the local time in UTC format\r\n");
  }
  else if (strcmp(command, "ATZ?") == 0)
  {
    AT_PRINTF("ATZ           : Trigger an MCU reset\r\n");
  }
  else if (strcmp(command, "AT+RFS?") == 0)
  {
    AT_PRINTF("AT+RFS        : Restore factory settings\r\n");
  }
  else if (strcmp(command, "AT+CS?") == 0)
  {
    AT_PRINTF("AT+CS         : Store current context to NVM\r\n");
  }
  else if (strcmp(command, "AT+STATUS?") == 0)
  {
    AT_PRINTF("AT+STATUS=?    : Get modem and network status\r\n");
  }
  else if (strcmp(command, "AT+ADR?") == 0)
  {
    AT_PRINTF("AT+ADR=<0|1|2> : Set ADR profile\r\n");
    AT_PRINTF("AT+ADR=?       : Get ADR profile\r\n");
  }
  else if (strcmp(command, "AT+DR?") == 0)
  {
    AT_PRINTF("AT+DR=<0..7>   : Set fixed data rate\r\n");
    AT_PRINTF("AT+DR=?        : Get next data rate\r\n");
  }
  else if (strcmp(command, "AT+DCS?") == 0)
  {
    AT_PRINTF("AT+DCS=?       : Get duty cycle status in ms\r\n");
  }
  else if (strcmp(command, "AT+BAT?") == 0)
  {
    AT_PRINTF("AT+BAT          : Get battery voltage in mV\r\n");
  }
  else if (strcmp(command, "AT+PGSLOT?") == 0)
  {
    AT_PRINTF("AT+PGSLOT=<0..7> : Set Class B ping slot periodicity\r\n");
    AT_PRINTF("AT+PGSLOT=?      : Get Class B ping slot periodicity\r\n");
  }
  else if (strcmp(command, "AT+DEUI?") == 0)
  {
    AT_PRINTF("AT+DEUI=<EUI>  : Set the device EUI\r\n");
    AT_PRINTF("AT+DEUI=?      : Get the device EUI\r\n");
  }
  else if (strcmp(command, "AT+APPEUI?") == 0)
  {
    AT_PRINTF("AT+APPEUI=<EUI>: Set the Join EUI\r\n");
    AT_PRINTF("AT+APPEUI=?    : Get the Join EUI\r\n");
  }
  else if (strcmp(command, "AT+NWKKEY?") == 0)
  {
    AT_PRINTF("AT+NWKKEY=<KEY>: Set the network key\r\n");
  }
  else if (strcmp(command, "AT+APPKEY?") == 0)
  {
    AT_PRINTF("AT+APPKEY=<KEY>: Set the application key\r\n");
  }
  else if (strcmp(command, "AT+NWKSKEY?") == 0)
  {
    AT_PRINTF("AT+NWKSKEY=<KEY>: Set the network session key\r\n");
  }
  else if (strcmp(command, "AT+APPSKEY?") == 0)
  {
    AT_PRINTF("AT+APPSKEY=<KEY>: Set the application session key\r\n");
  }
  else if (strcmp(command, "AT+DADDR?") == 0)
  {
    AT_PRINTF("AT+DADDR=?    : Get the device address\r\n");
    AT_PRINTF("AT+DADDR=<ADDR>: Set the device address\r\n");
  }
  else if (strcmp(command, "AT+BAND?") == 0)
  {
    AT_PRINTF("AT+BAND=<ID>   : Set the region ID\r\n");
    AT_PRINTF("AT+BAND=?      : Get the region ID\r\n");
  }
  else if (strcmp(command, "AT+JOIN?") == 0)
  {
    AT_PRINTF("AT+JOIN[=1]    : Join the network using OTAA\r\n");
  }
  else if (strcmp(command, "AT+SEND?") == 0)
  {
    AT_PRINTF("AT+SEND=<PORT>:<ACK>:<HEX> : Send a payload\r\n");
  }
  else if (strcmp(command, "AT+CLASS?") == 0)
  {
    AT_PRINTF("AT+CLASS=<A|B|C>: Set the device class\r\n");
    AT_PRINTF("AT+CLASS=?      : Get the device class\r\n");
  }
  else
  {
    return AT_ERROR;
  }
  return AT_OK;
}

ATEerror_t AT_LoraGetVerboseLevel(void)
{
  AT_PRINTF("+VL:%u\r\n", UTIL_ADV_TRACE_GetVerboseLevel());
  return AT_OK;
}

ATEerror_t AT_LoraGetLocalTime(void)
{
  SysTime_t systemTime = SysTimeGet();
  struct tm utcTime;

  SysTimeLocalTime(systemTime.Seconds, &utcTime);
  AT_PRINTF("+LTIME:%04d-%02d-%02dT%02d:%02d:%02dZ\r\n",
            utcTime.tm_year, utcTime.tm_mon + 1, utcTime.tm_mday,
            utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec);
  return AT_OK;
}

ATEerror_t AT_LoraSetVerboseLevel(const char *value)
{
  char *end;
  unsigned long level = strtoul(value, &end, 10);

  if ((value == end) || (*end != '\0') || (level > 3U))
  {
    return AT_PARAM_ERROR;
  }

  UTIL_ADV_TRACE_SetVerboseLevel((uint8_t)level);
  return AT_OK;
}

ATEerror_t AT_LoraGetStatus(void)
{
  smtc_modem_status_mask_t modemStatus = 0U;
  smtc_modem_return_code_t status = smtc_modem_get_status(AT_STACK_ID, &modemStatus);

  if (status == SMTC_MODEM_RC_OK)
  {
    const char *networkState = "READY";
    if ((modemStatus & SMTC_MODEM_STATUS_JOINED) != 0U)
    {
      networkState = "JOINED";
    }
    else if ((modemStatus & SMTC_MODEM_STATUS_JOINING) != 0U)
    {
      networkState = "JOINING";
    }
    AT_PRINTF("+STATUS:%s,0x%08X\r\n", networkState, (unsigned int)modemStatus);
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraGetAdrProfile(void)
{
  smtc_modem_adr_profile_t profile;
  smtc_modem_return_code_t status = smtc_modem_adr_get_profile(AT_STACK_ID, &profile);

  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PRINTF("+ADR:%u\r\n", (unsigned int)profile);
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetAdrProfile(const char *value)
{
  char *end;
  unsigned long profile = strtoul(value, &end, 10);

  if ((value == end) || (*end != '\0') || (profile > (unsigned long)SMTC_MODEM_ADR_PROFILE_MOBILE_LOW_POWER))
  {
    return AT_PARAM_ERROR;
  }

  return AT_FromModemStatus(smtc_modem_adr_set_profile(AT_STACK_ID, (smtc_modem_adr_profile_t)profile, NULL));
}

ATEerror_t AT_LoraGetDataRate(void)
{
  AT_PRINTF("+DR:%u\r\n", lorawan_api_next_dr_get(AT_STACK_ID));
  return AT_OK;
}

ATEerror_t AT_LoraSetDataRate(const char *value)
{
  uint8_t dataRateDistribution[SMTC_MODEM_CUSTOM_ADR_DATA_LENGTH];
  char *end;
  unsigned long dataRate = strtoul(value, &end, 10);

  if ((value == end) || (*end != '\0') || (dataRate > 7U))
  {
    return AT_PARAM_ERROR;
  }

  (void)memset(dataRateDistribution, (int)dataRate, sizeof(dataRateDistribution));
  return AT_FromModemStatus(smtc_modem_adr_set_profile(AT_STACK_ID, SMTC_MODEM_ADR_PROFILE_CUSTOM,
                                                        dataRateDistribution));
}

ATEerror_t AT_LoraGetDutyCycleStatus(void)
{
  int32_t statusMilliseconds;
  smtc_modem_return_code_t status = smtc_modem_get_duty_cycle_status(AT_STACK_ID, &statusMilliseconds);

  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PRINTF("+DCS:%d\r\n", (int)statusMilliseconds);
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraGetBatteryVoltage(void)
{
  AT_PRINTF("+BAT:%u\r\n", (unsigned int)SYS_GetBatteryLevel());
  return AT_OK;
}

ATEerror_t AT_LoraGetPingSlotPeriodicity(void)
{
  smtc_modem_class_b_ping_slot_periodicity_t periodicity;
  smtc_modem_return_code_t status = smtc_modem_class_b_get_ping_slot_periodicity(AT_STACK_ID, &periodicity);

  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PRINTF("+PGSLOT:%u\r\n", (unsigned int)periodicity);
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetPingSlotPeriodicity(const char *value)
{
  char *end;
  unsigned long periodicity = strtoul(value, &end, 10);

  if ((value == end) || (*end != '\0') || (periodicity > (unsigned long)SMTC_MODEM_CLASS_B_PINGSLOT_128_S))
  {
    return AT_PARAM_ERROR;
  }

  return AT_FromModemStatus(smtc_modem_class_b_set_ping_slot_periodicity(
      AT_STACK_ID, (smtc_modem_class_b_ping_slot_periodicity_t)periodicity));
}

ATEerror_t AT_LoraGetDevEui(void)
{
  uint8_t eui[SMTC_MODEM_EUI_LENGTH];
  smtc_modem_return_code_t status = smtc_modem_get_deveui(AT_STACK_ID, eui);
  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PrintHex("+DEUI:", eui, sizeof(eui));
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetDevEui(const char *value)
{
  uint8_t eui[SMTC_MODEM_EUI_LENGTH];
  if (!AT_ParseHex(value, eui, sizeof(eui)))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_set_deveui(AT_STACK_ID, eui));
}

ATEerror_t AT_LoraGetJoinEui(void)
{
  uint8_t eui[SMTC_MODEM_EUI_LENGTH];
  smtc_modem_return_code_t status = smtc_modem_get_joineui(AT_STACK_ID, eui);
  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PrintHex("+APPEUI:", eui, sizeof(eui));
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetJoinEui(const char *value)
{
  uint8_t eui[SMTC_MODEM_EUI_LENGTH];
  if (!AT_ParseHex(value, eui, sizeof(eui)))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_set_joineui(AT_STACK_ID, eui));
}

ATEerror_t AT_LoraSetAppKey(const char *value)
{
  uint8_t key[SMTC_MODEM_KEY_LENGTH];
  if (!AT_ParseHex(value, key, sizeof(key)))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_set_appkey(AT_STACK_ID, key));
}

ATEerror_t AT_LoraSetNwkKey(const char *value)
{
  uint8_t key[SMTC_MODEM_KEY_LENGTH];
  if (!AT_ParseHex(value, key, sizeof(key)))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_set_nwkkey(AT_STACK_ID, key));
}

ATEerror_t AT_LoraSetNwkSessionKey(const char *value)
{
  uint8_t key[SMTC_MODEM_KEY_LENGTH];
  if (!AT_ParseHex(value, key, sizeof(key)))
  {
    return AT_PARAM_ERROR;
  }
  if (smtc_secure_element_set_key(SMTC_SE_NWK_S_ENC_KEY, key, AT_STACK_ID) != SMTC_SE_RC_SUCCESS)
  {
    return AT_ERROR;
  }
  return AT_OK;
}

ATEerror_t AT_LoraSetAppSessionKey(const char *value)
{
  uint8_t key[SMTC_MODEM_KEY_LENGTH];
  if (!AT_ParseHex(value, key, sizeof(key)))
  {
    return AT_PARAM_ERROR;
  }
  if (smtc_secure_element_set_key(SMTC_SE_APP_S_KEY, key, AT_STACK_ID) != SMTC_SE_RC_SUCCESS)
  {
    return AT_ERROR;
  }
  return AT_OK;
}

ATEerror_t AT_LoraGetDevAddr(void)
{
  AT_PRINTF("+DADDR:%08X\r\n", (unsigned int)lorawan_api_devaddr_get(AT_STACK_ID));
  return AT_OK;
}

ATEerror_t AT_LoraSetDevAddr(const char *value)
{
  uint8_t addressBytes[sizeof(uint32_t)];
  uint32_t address;

  if (!AT_ParseHex(value, addressBytes, sizeof(addressBytes)))
  {
    return AT_PARAM_ERROR;
  }
  address = ((uint32_t)addressBytes[0] << 24) |
            ((uint32_t)addressBytes[1] << 16) |
            ((uint32_t)addressBytes[2] << 8) |
            (uint32_t)addressBytes[3];
  return (lorawan_api_devaddr_set(address, AT_STACK_ID) == OKLORAWAN) ? AT_OK : AT_ERROR;
}

ATEerror_t AT_LoraGetRegion(void)
{
  smtc_modem_region_t region;
  uint8_t legacyRegion;
  smtc_modem_return_code_t status = smtc_modem_get_region(AT_STACK_ID, &region);

  if ((status == SMTC_MODEM_RC_OK) && AT_UnmapRegion(region, &legacyRegion))
  {
    AT_PRINTF("+BAND:%u\r\n", legacyRegion);
    return AT_OK;
  }
  return AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetRegion(const char *value)
{
  char *end;
  smtc_modem_region_t region;
  uint32_t legacyRegion = strtoul(value, &end, 10);

  if ((value == end) || (*end != '\0') || !AT_MapRegion(legacyRegion, &region))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_set_region(AT_STACK_ID, region));
}

ATEerror_t AT_LoraJoin(const char *value)
{
  if ((value != NULL) && (strcmp(value, "1") != 0))
  {
    return AT_PARAM_ERROR;
  }
  return AT_FromModemStatus(smtc_modem_join_network(AT_STACK_ID));
}

ATEerror_t AT_LoraSend(const char *value)
{
  uint8_t payload[SMTC_MODEM_MAX_LORAWAN_PAYLOAD_LENGTH];
  char *end;
  uint32_t port = strtoul(value, &end, 10);
  uint32_t confirmed;
  const char *payloadText;
  size_t payloadTextLength;
  uint8_t payloadSize;

  if ((value == end) || (*end != ':') || (port < 1U) || (port > 223U))
  {
    return AT_PARAM_ERROR;
  }
  confirmed = strtoul(end + 1, &end, 10);
  if ((*end != ':') || (confirmed > 1U))
  {
    return AT_PARAM_ERROR;
  }

  payloadText = end + 1;
  payloadTextLength = strlen(payloadText);
  if ((payloadTextLength == 0U) || ((payloadTextLength & 1U) != 0U) ||
      ((payloadTextLength / 2U) > sizeof(payload)))
  {
    return AT_PARAM_ERROR;
  }
  payloadSize = (uint8_t)(payloadTextLength / 2U);
  if (!AT_ParseHex(payloadText, payload, payloadSize))
  {
    return AT_PARAM_ERROR;
  }

  return AT_FromModemStatus(smtc_modem_request_uplink(AT_STACK_ID, (uint8_t)port, confirmed != 0U,
                                                       payload, payloadSize));
}

ATEerror_t AT_LoraGetClass(void)
{
  smtc_modem_class_t deviceClass;
  smtc_modem_return_code_t status = smtc_modem_get_class(AT_STACK_ID, &deviceClass);
  if (status == SMTC_MODEM_RC_OK)
  {
    AT_PRINTF("+CLASS:%c\r\n", (char)('A' + (uint8_t)deviceClass));
  }
  return (status == SMTC_MODEM_RC_FAIL) ? AT_NO_NET_JOINED : AT_FromModemStatus(status);
}

ATEerror_t AT_LoraSetClass(const char *value)
{
  smtc_modem_class_t deviceClass;
  if ((value == NULL) || (value[1] != '\0') || (value[0] < 'A') || (value[0] > 'C'))
  {
    return AT_PARAM_ERROR;
  }
  deviceClass = (smtc_modem_class_t)(value[0] - 'A');
  return AT_FromModemStatus(smtc_modem_set_class(AT_STACK_ID, deviceClass));
}

void AT_PrintError(ATEerror_t error)
{
  static const char *const descriptions[] =
  {
    "OK", "AT_ERROR", "AT_PARAM_ERROR", "AT_BUSY_ERROR", "AT_NO_NETWORK_JOINED"
  };
  if ((uint32_t)error >= (sizeof(descriptions) / sizeof(descriptions[0])))
  {
    error = AT_ERROR;
  }
  AT_PRINTF("\r\n%s\r\n", descriptions[error]);
}

static void AT_PrintHelpLine(const char *line)
{
  AT_PRINTF("%s\r\n", line);
  while (UTIL_ADV_TRACE_IsBufferEmpty() == 0U)
  {
  }
}

void AT_PrintHelp(void)
{
  AT_PrintHelpLine("Commands:");
  AT_PrintHelpLine("  AT                                     : basic check");
  AT_PrintHelpLine("  AT?                                    : this help");
  AT_PrintHelpLine("  ATZ                                    : Trigger an MCU reset");
  AT_PrintHelpLine("  AT+VL=?                                : Get verbose level");
  AT_PrintHelpLine("  AT+VL=<Level>                          : Set verbose level");
  AT_PrintHelpLine("  AT+LTIME=?                             : Get the local time in UTC format");
  AT_PrintHelpLine("  AT+RFS                                 : Restore factory settings");
  AT_PrintHelpLine("  AT+CS                                  : Store current context to NVM");
  AT_PrintHelpLine("  AT+APPEUI=?                            : Get JoinEUI");
  AT_PrintHelpLine("  AT+APPEUI=<EUI>                        : Set JoinEUI");
  AT_PrintHelpLine("  AT+NWKKEY=<KEY>                        : Set NwkKey");
  AT_PrintHelpLine("  AT+APPKEY=<KEY>                        : Set AppKey");
  AT_PrintHelpLine("  AT+APPSKEY=<KEY>                       : Set Application Session Key");
  AT_PrintHelpLine("  AT+NWKSKEY=<KEY>                       : Set Network Session Key");
  AT_PrintHelpLine("  AT+DADDR=?                             : Get device address");
  AT_PrintHelpLine("  AT+DADDR=<ADDR>                        : Set device address");
  AT_PrintHelpLine("  AT+DEUI=?                              : Get DevEUI");
  AT_PrintHelpLine("  AT+DEUI=<EUI>                          : Set DevEUI");
  AT_PrintHelpLine("  AT+JOIN[=1]                            : Join the network using OTAA");
  AT_PrintHelpLine("  AT+SEND=<PORT>:<ACK>:<HEX>             : Send payload");
  AT_PrintHelpLine("  AT+VER=?                               : Get versions");
  AT_PrintHelpLine("  AT+ADR=?                               : Get ADR profile");
  AT_PrintHelpLine("  AT+ADR=<0|1|2>                         : Set ADR profile");
  AT_PrintHelpLine("  AT+DR=?                                : Get next data rate");
  AT_PrintHelpLine("  AT+DR=<0..7>                           : Set fixed data rate");
  AT_PrintHelpLine("  AT+STATUS=?                            : Get modem and network status");
  AT_PrintHelpLine("  AT+BAND=?                              : Get region ID");
  AT_PrintHelpLine("  AT+BAND=<ID>                           : Set region ID");
  AT_PrintHelpLine("  AT+CLASS=?                             : Get device class");
  AT_PrintHelpLine("  AT+CLASS=<A|B|C>                       : Set device class");
  AT_PrintHelpLine("  AT+DCS=?                               : Get duty cycle status in ms");
  AT_PrintHelpLine("  AT+PGSLOT=?                            : Get Class B ping slot periodicity");
  AT_PrintHelpLine("  AT+PGSLOT=<0..7>                       : Set Class B ping slot periodicity");
  AT_PrintHelpLine("  AT+BAT                                 : Get battery voltage in mV");
}

void AT_EventReady(void)
{
  AT_PRINTF("+EVENT:READY\r\n");
}

void AT_EventJoined(void)
{
  AT_PRINTF("+EVENT:JOINED\r\n");
}

void AT_EventJoinFailed(void)
{
  AT_PRINTF("+EVENT:JOINFAIL\r\n");
}

void AT_EventTxDone(smtc_modem_event_txdone_status_t status)
{
  AT_PRINTF("+EVENT:TXDONE,%u\r\n", (uint8_t)status);
}

void AT_EventDownlink(const uint8_t *payload, uint8_t size, const smtc_modem_dl_metadata_t *metadata)
{
  static const char hex[] = "0123456789ABCDEF";
  char encodedPayload[(SMTC_MODEM_MAX_LORAWAN_PAYLOAD_LENGTH * 2U) + 1U];
  uint16_t index;

  if ((payload == NULL) || (metadata == NULL))
  {
    return;
  }
  for (index = 0U; index < size; index++)
  {
    encodedPayload[index * 2U] = hex[payload[index] >> 4];
    encodedPayload[(index * 2U) + 1U] = hex[payload[index] & 0x0FU];
  }
  encodedPayload[size * 2U] = '\0';
  AT_PRINTF("+EVENT:RXDATA,%u,%d,%d,%s\r\n", metadata->fport, metadata->rssi, metadata->snr,
            encodedPayload);
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static ATEerror_t AT_FromModemStatus(smtc_modem_return_code_t status)
{
  switch (status)
  {
    case SMTC_MODEM_RC_OK:
      return AT_OK;
    case SMTC_MODEM_RC_INVALID:
      return AT_PARAM_ERROR;
    case SMTC_MODEM_RC_BUSY:
      return AT_BUSY_ERROR;
    case SMTC_MODEM_RC_FAIL:
      return AT_NO_NET_JOINED;
    default:
      return AT_ERROR;
  }
}

static bool AT_ParseHex(const char *text, uint8_t *output, uint8_t expectedSize)
{
  uint8_t outputIndex = 0U;
  int8_t highNibble = -1;

  while (*text != '\0')
  {
    int8_t nibble;
    if ((*text == ':') || (*text == '-') || (*text == ' '))
    {
      text++;
      continue;
    }
    nibble = AT_HexNibble(*text++);
    if (nibble < 0)
    {
      return false;
    }
    if (highNibble < 0)
    {
      highNibble = nibble;
    }
    else
    {
      if (outputIndex >= expectedSize)
      {
        return false;
      }
      output[outputIndex++] = (uint8_t)(((uint8_t)highNibble << 4) | (uint8_t)nibble);
      highNibble = -1;
    }
  }
  return (highNibble < 0) && (outputIndex == expectedSize);
}

static int8_t AT_HexNibble(char character)
{
  if ((character >= '0') && (character <= '9'))
  {
    return (int8_t)(character - '0');
  }
  if ((character >= 'A') && (character <= 'F'))
  {
    return (int8_t)(character - 'A' + 10);
  }
  if ((character >= 'a') && (character <= 'f'))
  {
    return (int8_t)(character - 'a' + 10);
  }
  return -1;
}

static void AT_PrintHex(const char *prefix, const uint8_t *data, uint8_t size)
{
  static const char hex[] = "0123456789ABCDEF";
  char encoded[(SMTC_MODEM_KEY_LENGTH * 2U) + 1U];
  uint8_t index;

  for (index = 0U; index < size; index++)
  {
    encoded[index * 2U] = hex[data[index] >> 4];
    encoded[(index * 2U) + 1U] = hex[data[index] & 0x0FU];
  }
  encoded[size * 2U] = '\0';
  AT_PRINTF("%s%s\r\n", prefix, encoded);
}

static bool AT_MapRegion(uint32_t legacyRegion, smtc_modem_region_t *region)
{
  switch (legacyRegion)
  {
    case 0U: *region = LORAMAC_REGION_AS923_GRP1; return true;
    case 1U: *region = LORAMAC_REGION_AU915; return true;
    case 2U: *region = LORAMAC_REGION_CN470; return true;
    case 5U: *region = LORAMAC_REGION_EU868; return true;
    case 6U: *region = LORAMAC_REGION_KR920; return true;
    case 7U: *region = LORAMAC_REGION_IN865; return true;
    case 8U: *region = LORAMAC_REGION_US915; return true;
    case 9U: *region = LORAMAC_REGION_RU864; return true;
    default: return false;
  }
}

static bool AT_UnmapRegion(smtc_modem_region_t region, uint8_t *legacyRegion)
{
  switch (region)
  {
    case LORAMAC_REGION_AS923_GRP1: *legacyRegion = 0U; return true;
    case LORAMAC_REGION_AU915: *legacyRegion = 1U; return true;
    case LORAMAC_REGION_CN470: *legacyRegion = 2U; return true;
    case LORAMAC_REGION_EU868: *legacyRegion = 5U; return true;
    case LORAMAC_REGION_KR920: *legacyRegion = 6U; return true;
    case LORAMAC_REGION_IN865: *legacyRegion = 7U; return true;
    case LORAMAC_REGION_US915: *legacyRegion = 8U; return true;
    case LORAMAC_REGION_RU864: *legacyRegion = 9U; return true;
    default: return false;
  }
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
