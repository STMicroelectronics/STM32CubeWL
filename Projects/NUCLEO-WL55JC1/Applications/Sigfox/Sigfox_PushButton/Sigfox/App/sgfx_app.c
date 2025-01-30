/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_app.c
  * @author  MCD Application Team
  * @brief   provides code for the application of the sigfox Middleware
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

#include "st_sigfox_api.h"
#include "sgfx_app.h"
#include "app_version.h"
#include "sigfox_version.h"
#include "subghz_phy_version.h"
#include "radio.h"
#include "sys_conf.h"
#include "sgfx_eeprom_if.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */
#include "sys_sensors.h"

/* USER CODE END Includes */

/* Exported variables ---------------------------------------------------------*/

/* USER CODE BEGIN ExpV */

/* USER CODE END ExpV */

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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  It calls SIGFOX_API_open()
  * @param  Config
  * @retval Status
  */
static sfx_error_t st_sigfox_open(sfx_rc_enum_t sfx_rc);

/**
  * @brief  The example sends T, P and H data. To be changed by the user.
  * @param  None
  * @retval None
  */
static void SendSigfox(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void Sigfox_Init(void)
{
  sfx_rc_enum_t sfx_rc = SFX_RC1;
  sfx_error_t error = 0;
  /* USER CODE BEGIN Sigfox_Init_1 */

  /* Get Sigfox APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW Sigfox info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_SIGFOX_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(SIGFOX_VERSION_MAIN),
          (uint8_t)(SIGFOX_VERSION_SUB1),
          (uint8_t)(SIGFOX_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  /* USER CODE END Sigfox_Init_1 */

  E2P_Write_Rc(DEFAULT_RC);
  sfx_rc = E2P_Read_Rc();

  /*Open Sigfox library */
  error = st_sigfox_open(sfx_rc);

  Radio.Init(&RfApiRadioEvents);
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
  (void)error;

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Pb), UTIL_SEQ_RFU, SendSigfox);

  /* USER CODE BEGIN Sigfox_Init_2 */

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

/* USER CODE BEGIN PB_Callbacks */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUT1_Pin:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Pb), CFG_SEQ_Prio_0);
      break;
    case  BUT2_Pin:
      break;
    default:
      break;
  }
}
/* USER CODE END PB_Callbacks */

static void SendSigfox(void)
{
  /* USER CODE BEGIN SendSigfox */

  /* Following code is an example that user can replace according with his needs */

  uint8_t ul_msg[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11};
  uint8_t dl_msg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t  ul_size = 0;
  int16_t temperature = 0;
  uint16_t batteryLevel = SYS_GetBatteryLevel();
  uint32_t nbTxRepeatFlag = 1;
  sensor_t sensor_data;
  uint16_t pressure = 0;
  uint16_t humidity = 0;

  EnvSensors_Read(&sensor_data);

  temperature = (uint16_t)(sensor_data.temperature);
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

  SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, nbTxRepeatFlag, SFX_FALSE);

  APP_LOG(TS_OFF, VLEVEL_L, " done\n\r");
  /* USER CODE END SendSigfox */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
