/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_at.c
  * @author  MCD Application Team
  * @brief   at command API
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "demo_concentrator.h"
#include "demo_utils.h"
#include "demo_at.h"
#include "usart.h"
#include "stm32_tiny_sscanf.h"
#include "version.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static DEMO_coding_lora_t RegLoraParam; /**< Registers to store modulation before change*/
static DEMO_coding_fsk_t RegFskParam;   /**< Registers to store modulation before change*/
static uint32_t RegRegion;              /**< Register to store region before start*/
static uint32_t RegSubregion;           /**< Register to store subregion before start*/

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief Set modulation to LoRa or FSK.
  * @param lora true if LoRa, false if FSK
  * @param param parameters of AT command
  * @param test_only if true, only respond and do not use the modulation
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_helper(bool lora, const char *param, bool test_only);

/* Exported functions ------------------------------------------------------- */

/**
  * @brief Set default values to registers.
  */
void at_init(void)
{
  DEMO_ValidateCodingLora(&RegLoraParam);
  DEMO_ValidateCodingFsk(&RegFskParam);
  RegLoraParam.cr = DEMO_DEFAULT_CR;
  RegLoraParam.de = DEMO_DEFAULT_DE;

  RegFskParam.br = DEMO_DEFAULT_BR;
  RegFskParam.bt = DEMO_DEFAULT_BT;
  RegFskParam.fdev = DEMO_DEFAULT_FDEV;
  RegFskParam.rise = DEMO_DEFAULT_RISE;

  RegRegion = 0;
  RegSubregion = 0;
}

ATEerror_t at_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t at_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t at_reset(const char *param)
{
  NVIC_SystemReset();
}

ATEerror_t at_version_get(const char *param)
{
  AT_PRINTF(VERSION_STR "\r\n");
  return AT_OK;
}

ATEerror_t at_verbose_set(const char *param)
{
  const char *buf = param;
  int32_t lvl_nb;

  /* read and set the verbose level */
  if (1 != tiny_sscanf(buf, "%u", &lvl_nb))
  {
    AT_PRINTF("AT+VL: verbose level is not well set");
    return AT_PARAM_ERROR;
  }
  if ((lvl_nb > VLEVEL_H) || (lvl_nb < VLEVEL_OFF))
  {
    AT_PRINTF("AT+VL: verbose level out of range => 0(VLEVEL_OFF) to 3(VLEVEL_H)");
    return AT_PARAM_ERROR;
  }

  UTIL_ADV_TRACE_SetVerboseLevel(lvl_nb);

  return AT_OK;
}

ATEerror_t at_verbose_get(const char *param)
{
  AT_PRINTF("%u", UTIL_ADV_TRACE_GetVerboseLevel());
  return AT_OK;
}

/**
  * @brief List all available regions.
  * @param param ignored
  * @return AT_OK
  */
ATEerror_t at_list_regions(const char *param)
{
  AT_PRINTF("Regions and subregions %s:", VERSION_STR);
  for (int r = 0; r < DEMO_Regions_n; r++)
  {
    AT_PRINTF("\r\n\t%i - %3u.%03u MHz - %s\r\n", r,
              DEMO_Regions[r].beacon_freq / 1000000, (DEMO_Regions[r].beacon_freq / 1000) % 1000,
              DEMO_Regions[r].name);

    for (int s = 0; s < DEMO_Regions[r].subregions_n; s++)
    {
      AT_PRINTF("\t%i.%i - %s\r\n", r, s, DEMO_Regions[r].subregions[s].name);
    }
  }

  return AT_OK;
}

/**
  * @brief Macro to get many register getters and setters.
  */
#define AT_PROVIDE_GETSET(name, var, min, max)                   \
  \
  /**                                                              \
    * @brief Set variable.                                          \
    * @param param parameters of AT command                         \
    * @return one of ATEerror_t                                     \
    */                                                              \
  ATEerror_t at_##name##_set(const char *param)                    \
  {                                                                \
    unsigned long ul;                                              \
    int ret;                                                       \
    \
    ret = tiny_sscanf(param, "%ul", &ul);                          \
    if((ret != 1)                                                  \
       || ((ul - (min)) > ((max) - (min)))) /*Check limits*/  \
    {                                                              \
      AT_PRINTF(#name);                                            \
      AT_PRINTF(" needs to be between %u and %u!", min, max);      \
      return AT_PARAM_ERROR;                                       \
    }                                                              \
    \
    var = ul;                                                      \
    return AT_OK;                                                  \
  }                                                                \
  \
  /**                                                              \
    * @brief Get variable.                                          \
    * @param param parameters of AT command                         \
    * @return AT_OK                                                 \
    */                                                              \
  ATEerror_t at_##name##_get(const char *param)                    \
  {                                                                \
    AT_PRINTF("%u", (unsigned int)(var));                        \
    return AT_OK;                                                \
  }

AT_PROVIDE_GETSET(DE,        RegLoraParam.de,      0,                           1)
AT_PROVIDE_GETSET(CR,        RegLoraParam.cr,      DEMO_LORA_PARAM_CR_MIN,      DEMO_LORA_PARAM_CR_MAX)
AT_PROVIDE_GETSET(SF,        RegLoraParam.sf,      DEMO_LORA_PARAM_SF_MIN,      DEMO_LORA_PARAM_SF_MAX)
AT_PROVIDE_GETSET(BW,        RegLoraParam.bw,      DEMO_LORA_PARAM_BW_MIN,      DEMO_LORA_PARAM_BW_MAX)

AT_PROVIDE_GETSET(RISE,      RegFskParam.rise,     DEMO_FSK_PARAM_RISE_MIN,     DEMO_FSK_PARAM_RISE_MAX)
AT_PROVIDE_GETSET(BR,        RegFskParam.br,       DEMO_FSK_PARAM_BR_MIN,       DEMO_FSK_PARAM_BR_MAX)
AT_PROVIDE_GETSET(FDEV,      RegFskParam.fdev,     DEMO_FSK_PARAM_FDEV_MIN,     DEMO_FSK_PARAM_FDEV_MAX)
AT_PROVIDE_GETSET(BT,        RegFskParam.bt,       DEMO_FSK_PARAM_BT_MIN,       DEMO_FSK_PARAM_BT_MAX)

AT_PROVIDE_GETSET(REGION,    RegRegion,            0,                           255)
AT_PROVIDE_GETSET(SUBREGION, RegSubregion,         0,                           255)

/**
  * @brief Start or stop sending beacons.
  * @param param parameters of AT command
  * @return one of ATEerror_t
  */
ATEerror_t at_beacon_get(const char *param)
{
  if (CONC_IsEnabled() == true)
  {
    AT_PRINTF("1\r\n");
  }
  else
  {
    AT_PRINTF("0\r\n");
  }
  return AT_OK;
}

/**
  * @brief Start or stop sending beacons.
  * @param param
  * @return one of ATEerror_t
  */
ATEerror_t at_beacon_set(const char *param)
{
  int ret;

  if (param[0] == '\0')
  {
    return AT_PARAM_ERROR;
  }
  else if (param[0] == '0')
  {
    CONC_StopBeacon();
    return AT_OK;
  }
  else if (param[0] == '1')
  {
    ret = CONC_StartBeacon(RegRegion, RegSubregion);  /*Enable transmitting beacons*/
    switch (ret)
    {
      case 0:
        return AT_OK;
      case 1:
        AT_PRINTF("Beacon is already enabled!");
        return AT_ERROR;
      case 2:
        AT_PRINTF("Wrong region number!");
        return AT_PARAM_ERROR;
      case 3:
        AT_PRINTF("Wrong subregion number for region given!");
        return AT_PARAM_ERROR;
      default:
        return AT_ERROR;
    }
  }
  else
  {
    AT_PRINTF("Value can be only 0 or 1!");
    return AT_PARAM_ERROR;
  }
}

/**
  * @brief Start or stop sending beacons.
  * @param param
  * @return one of ATEerror_t
  */
ATEerror_t at_beacon_run(const char *param)
{
  return at_beacon_set("1");
}

/**
  * @brief Set modulation to LoRa or FSK.
  * @param lora true if LoRa, false if FSK
  * @param param parameters of AT command
  * @param test_only if true, only respond and do not use the modulation
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_helper(bool lora, const char *param, bool test_only)
{
  uint32_t eui;
  CONC_SetModReturn_t ret2;
  int ret1;


  if (CONC_IsEnabled() == false)
  {
    AT_PRINTF("Beacon is not on!");
    return AT_PARAM_ERROR;
  }

  if (test_only == false)
  {
    ret1 = tiny_sscanf(param, "0x%x", &eui);
    if (ret1 != 1)
    {
      AT_PRINTF("EUI was not understood! (use AT+MOD_XXX=0xabcd1234)");
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    eui = 0;
  }

  if (lora)
  {
    ret2 = CONC_SetModLora(eui, &RegLoraParam, test_only);     /*Change or test the mode*/
  }
  else
  {
    ret2 = CONC_SetModFSK(eui, &RegFskParam, test_only);     /*Change or test the mode*/
  }

  switch (ret2)
  {
    case CONC_SETMOD_Ok:
      return AT_OK;
    case CONC_SETMOD_EuiWrong:
      AT_PRINTF("This EUI is not connected!");
      return AT_EUI_NOT_CONNECTED;
    case CONC_SETMOD_ModNotAllowed:
      AT_PRINTF("This modulation is not allowed in this region!");
      return AT_MOD_NOT_ALLOWED;
    case CONC_SETMOD_ModWrong:
      AT_PRINTF("This modulation doesn't work!");
      return AT_MOD_NOT_ALLOWED;
    default:
      return AT_ERROR;
  }
}

/**
  * @brief Set modulation to LoRa.
  * @param param parameters of AT command
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_lora(const char *param)
{
  return at_mod_helper(1, param, 0);
}

/**
  * @brief Set modulation to FSK.
  * @param param parameters of AT command
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_fsk(const char *param)
{
  return at_mod_helper(0, param, 0);
}

/**
  * @brief Test LoRa modulation.
  * @param param parameters of AT command
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_test_lora(const char *param)
{
  return at_mod_helper(1, param, 1);
}

/**
  * @brief Test FSK modulation.
  * @param param parameters of AT command
  * @return one of ATEerror_t
  */
ATEerror_t at_mod_test_fsk(const char *param)
{
  return at_mod_helper(0, param, 1);
}

/* Private functions ---------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
