/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_app.c
  * @author  MCD Application Team
  * @brief   provides code for the application of the sigfox Middleware
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "st_sigfox_api.h"
#include "sgfx_app.h"
#include "sgfx_app_version.h"
#include "sigfox_version.h"
#include "subghz_phy_version.h"
#include "stm32_seq.h"
#include "radio.h"
#include "sys_conf.h"
#include "sgfx_eeprom_if.h"
#include "sys_app.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "sigfox_mbwrapper.h"
#include "sigfox_info.h"
#include "mbmuxif_sys.h"

/* USER CODE BEGIN Includes */
#include "sys_sensors.h"

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
extern RadioEvents_t RfApiRadioEvents;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SigfoxCallback_t SigfoxCallbacks = { SYS_GetBatteryLevel,
                                            SYS_GetTemperatureLevel
                                          };

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static sfx_error_t st_sigfox_open(sfx_rc_enum_t sfx_rc);

/* USER CODE BEGIN PFP */
static void SendSigfox(void);

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void Sigfox_Init(void)
{
  sfx_rc_enum_t sfx_rc = SFX_RC1;
  sfx_error_t error = 0;
  SigfoxInfo_t *SigfoxRegionInfo;
  FEAT_INFO_Param_t *p_cm0plus_specific_features_info;
  uint32_t sgfx_cm0plus_app;

  /* USER CODE BEGIN Sigfox_Init_1 */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  /* Initialize button here*/
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);

  /* Get CM4 Sigfox APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "M4 APP_VERSION:     V%X.%X.%X\r\n",
          (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get CM0 Sigfox APP version*/
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SYSTEM_ID);
  sgfx_cm0plus_app = p_cm0plus_specific_features_info->Feat_Info_Feature_Version ;
  APP_LOG(TS_OFF, VLEVEL_M, "M0PLUS_APP_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW Sigfox info */
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SIGFOX_ID);
  sgfx_cm0plus_app = p_cm0plus_specific_features_info->Feat_Info_Feature_Version ;
  APP_LOG(TS_OFF, VLEVEL_M, "MW_SIGFOX_VERSION:  V%X.%X.%X\r\n",
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW SubGhz_Phy info */
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_RADIO_ID);
  sgfx_cm0plus_app = p_cm0plus_specific_features_info->Feat_Info_Feature_Version ;
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(sgfx_cm0plus_app >> __APP_VERSION_SUB2_SHIFT));

  /* USER CODE END Sigfox_Init_1 */

  E2P_Write_Rc(DEFAULT_RC);
  sfx_rc = E2P_Read_Rc();

  /*Open Sigfox library */
  error = st_sigfox_open(sfx_rc);

  /* Init SigfoxInfo capabilities table (redundant: Cm0 is supposed to have done it already) */
  SigfoxInfo_Init();

  SigfoxRegionInfo = SigfoxInfo_GetPtr();
  if (SigfoxRegionInfo->Region == 0)
  {
    APP_PRINTF("error: At least one region shall be defined : RC1 to RC7 \n\r");
    while (1) {} /* At least one region shall be defined */
  }

  /* Register GetBatteryLevel and GetTemperatureLevel functions */
  Sigfox_Register(&SigfoxCallbacks);
  /* USER CODE BEGIN Sigfox_Init_ErrorCheck */
  if (1U == E2P_Read_AtEcho())
  {
    if (error == SFX_ERR_NONE)
    {
      APP_PPRINTF("\r\n\n\rSIGFOX APPLICATION READY\n\r\n\r");
    }
    else
    {
      APP_PPRINTF("\r\n\n\rSIGFOX APPLICATION ERROR: 0x%04X\n\r\n\r", error);
    }
  }
  /* USER CODE END Sigfox_Init_ErrorCheck */
  /* USER CODE BEGIN Sigfox_Init_2 */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Pb), UTIL_SEQ_RFU, SendSigfox);

  /* USER CODE END Sigfox_Init_2 */
}
/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static sfx_error_t st_sigfox_open(sfx_rc_enum_t sfx_rc)
{
  /* USER CODE BEGIN st_sigfox_open_1 */

  /* USER CODE END st_sigfox_open_1 */
  sfx_u32 config_words[3] = {0};

  E2P_Read_ConfigWords(sfx_rc, config_words);

  sfx_error_t error = SFX_ERR_NONE;

  /*record RCZ*/
  switch (sfx_rc)
  {
    case SFX_RC1:
    {
      sfx_rc_t SgfxRc = RC1;
      error = SIGFOX_API_open(&SgfxRc);

      break;
    }
    case SFX_RC2:
    {
      sfx_rc_t SgfxRc = RC2;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, RC2_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    case SFX_RC3A:
    {
      sfx_rc_t SgfxRc = RC3A;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }
      break;
    }
    case SFX_RC3C:
    {
      sfx_rc_t SgfxRc = RC3C;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }
      break;
    }
    case SFX_RC4:
    {
      sfx_rc_t SgfxRc = RC4;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, RC4_SET_STD_TIMER_ENABLE);
      }
      break;
    }
    case SFX_RC5:
    {
      sfx_rc_t SgfxRc = RC5;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }
      break;
    }
    case SFX_RC6:
    {
      sfx_rc_t SgfxRc = RC6;
      error = SIGFOX_API_open(&SgfxRc);
      break;
    }
    case SFX_RC7:
    {
      sfx_rc_t SgfxRc = RC7;
      error = SIGFOX_API_open(&SgfxRc);
      break;
    }
    /* USER CODE BEGIN st_sigfox_open_case */

    /* USER CODE END st_sigfox_open_case */
    default:
    {
      error = SFX_ERR_API_OPEN;
      break;
    }
  }

  return error;
  /* USER CODE BEGIN st_sigfox_open_2 */

  /* USER CODE END st_sigfox_open_2 */
}

/* USER CODE BEGIN PrFD */
/* Note: Current the stm32wlxx_it.c generated by STM32CubeMX does not support BSP for PB in EXTI mode. */
/* In order to get a push button IRS by code automatically generated */
/* HAL_GPIO_EXTI_Callback is today the only available possibility. */
/* Using HAL_GPIO_EXTI_Callback() shortcuts the BSP. */
/* If users wants to really go through the BSP, stm32wlxx_it.c should be updated  */
/* in the USER CODE SESSION of the correspondent EXTIn_IRQHandler() */
/* to call the BSP_PB_IRQHandler() or the HAL_EXTI_IRQHandler(&H_EXTI_n);. */
/* Then the below HAL_GPIO_EXTI_Callback() can be replaced by BSP callback */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUTTON_SW1_PIN:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Pb), CFG_SEQ_Prio_0);
      break;
    case  BUTTON_SW2_PIN:
      break;
    case  BUTTON_SW3_PIN:
      break;
    default:
      break;
  }
}

static void SendSigfox(void)
{
  uint8_t ul_msg[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11};
  uint8_t dl_msg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t  ul_size = 0;
  int16_t temperature = SYS_GetTemperatureLevel() >> 8;
  uint16_t batteryLevel = SYS_GetBatteryLevel();
  uint32_t nbTxRepeatFlag = 1;
  sensor_t sensor_data;
  uint16_t pressure = 0;
  uint16_t humidity = 0;

  EnvSensors_Read(&sensor_data);
  pressure    = (uint16_t)(sensor_data.pressure * 100 / 10);      /* in hPa / 10 */
  humidity    = (uint16_t) sensor_data.humidity;

  APP_LOG(TS_ON, VLEVEL_L, "sending temperature=%d degC,  battery=%d mV", temperature, batteryLevel);

  ul_msg[ul_size++] = (uint8_t)((batteryLevel * 100) / 3300);
  ul_msg[ul_size++] = (pressure >> 8) & 0xFF;
  ul_msg[ul_size++] = pressure & 0xFF;
  ul_msg[ul_size++] = (temperature >> 8) & 0xFF;
  ul_msg[ul_size++] = temperature & 0xFF;
  ul_msg[ul_size++] = (humidity >> 8) & 0xFF;
  ul_msg[ul_size++] = humidity & 0xFF;

  BSP_LED_On(LED_BLUE);

  SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, nbTxRepeatFlag, SFX_FALSE);

  BSP_LED_Off(LED_BLUE);

  APP_LOG(TS_OFF, VLEVEL_L, " done\n\r");
}

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
