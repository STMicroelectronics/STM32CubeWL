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
#include "radio.h"
#include "sys_conf.h"
#include "sgfx_eeprom_if.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "sgfx_command.h"
#include "sigfox_mbwrapper.h"
#include "sigfox_info.h"
#include "mbmuxif_sys.h"

/* USER CODE BEGIN Includes */

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
static SigfoxCallback_t SigfoxCallbacks = { SYS_GetBatteryLevel,
                                            GetTemperatureLevel
                                          };

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
  * @brief  It schedules the Virtual Com task
  * @param  None
  * @retval None
  */
static void CmdProcessNotify(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void Sigfox_Init(void)
{
  sfx_rc_enum_t sfx_rc = SFX_RC1;
  sfx_error_t error = 0;
  SigfoxInfo_t *SigfoxRegionInfo;

  /* USER CODE BEGIN Sigfox_Init_0 */
  FEAT_INFO_Param_t *p_cm0plus_specific_features_info;
  uint32_t feature_version = 0UL;
  /* USER CODE END Sigfox_Init_0 */
  CMD_Init(CmdProcessNotify);

  /* USER CODE BEGIN Sigfox_Init_1 */

  /* Get CM4 Sigfox APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "M4_APP_VERSION:      V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get CM0 Sigfox APP version*/
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SYSTEM_ID);
  feature_version = p_cm0plus_specific_features_info->Feat_Info_Feature_Version;
  APP_LOG(TS_OFF, VLEVEL_M, "M0PLUS_APP_VERSION:  V%X.%X.%X\r\n",
          (uint8_t)(feature_version >> 24),
          (uint8_t)(feature_version >> 16),
          (uint8_t)(feature_version >> 8));

  /* Get MW Sigfox info */
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SIGFOX_ID);
  feature_version = p_cm0plus_specific_features_info->Feat_Info_Feature_Version;
  APP_LOG(TS_OFF, VLEVEL_M, "MW_SIGFOX_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(feature_version >> 24),
          (uint8_t)(feature_version >> 16),
          (uint8_t)(feature_version >> 8));

  /* Get MW SubGhz_Phy info */
  p_cm0plus_specific_features_info = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_RADIO_ID);
  feature_version = p_cm0plus_specific_features_info->Feat_Info_Feature_Version;
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(feature_version >> 24),
          (uint8_t)(feature_version >> 16),
          (uint8_t)(feature_version >> 8));

  APP_PPRINTF("ATtention command interface\n\r");

  /* USER CODE END Sigfox_Init_1 */

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
  (void)error;

  /* Register Virtual-Com task then radio goes in Sleep/Stop mode waiting an AT cmd */
  UTIL_SEQ_RegTask(1 << CFG_SEQ_Task_Vcom, UTIL_SEQ_RFU, CMD_Process);

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

static void CmdProcessNotify(void)
{
  /* USER CODE BEGIN CmdProcessNotify_1 */

  /* USER CODE END CmdProcessNotify_1 */
  UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_Vcom, CFG_SEQ_Prio_0);
  /* USER CODE BEGIN CmdProcessNotify_2 */

  /* USER CODE END CmdProcessNotify_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
