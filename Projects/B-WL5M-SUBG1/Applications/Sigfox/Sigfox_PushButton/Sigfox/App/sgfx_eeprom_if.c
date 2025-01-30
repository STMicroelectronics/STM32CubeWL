/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_eeprom_if.c
  * @author  MCD Application Team
  * @brief   eeprom interface to the sigfox component
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
#include <stdint.h>
#include "sgfx_eeprom_if.h"
#include "ee.h"
#include "ee_conf.h"
#include "st_sigfox_api.h"
#include "se_nvm.h"
#include "sys_conf.h"
#include "utilities_conf.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
enum
{
  EE_BANK_0 = 0,
  EE_BANK_1 = 1
};

enum
{
  NO_FORMAT =         0,
  FORMAT =            1
};

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
  * @brief        Read Sigfox Configuration from eeprom
  * @param addr   variable virtual address
  * @param data   pointer to a 32-bit word (allocated by the caller) containing the
  *               variable value in case of success.
  */
static void E2P_Read(e_EE_ID addr, uint32_t *data);

/**
  * @brief        Write Sigfox Configuration to eeprom
  * @param addr   variable virtual address
  * @param data   32-bit data word to be written
  */
static void E2P_Write(e_EE_ID addr, uint32_t data);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void E2P_Init(void)
{
  /* USER CODE BEGIN E2P_Init_1 */

  /* USER CODE END E2P_Init_1 */
  if (EE_Init(NO_FORMAT, EE_BASE_ADRESS) != EE_OK)
  {
    if (EE_Init(FORMAT, EE_BASE_ADRESS) == EE_OK)
    {
      E2P_RestoreFs();
    }
    else
    {
      Error_Handler();
    }
  }
  /* USER CODE BEGIN E2P_Init_2 */

  /* USER CODE END E2P_Init_2 */
}

void E2P_RestoreFs(void)
{
  /* USER CODE BEGIN E2P_RestoreFs_1 */

  /* USER CODE END E2P_RestoreFs_1 */
  uint32_t rc3a_config[] = RC3A_CONFIG;
  uint32_t rc3c_config[] = RC3C_CONFIG;
  uint32_t rc5_config[] = RC5_CONFIG;

  E2P_Write(EE_RSSI_CAL_ID, 0);
  E2P_Write(EE_AT_ECHO_ID, 1);   /* AtEcho  =  Set */
  E2P_Write(EE_TX_POWER_RC1_ID, 13);
  E2P_Write(EE_TX_POWER_RC2_ID, 22);
  E2P_Write(EE_TX_POWER_RC3A_ID, 13);
  E2P_Write(EE_TX_POWER_RC3C_ID, 13);
  E2P_Write(EE_TX_POWER_RC4_ID, 22);
  E2P_Write(EE_TX_POWER_RC5_ID, 12);
  E2P_Write(EE_TX_POWER_RC6_ID, 13);
  E2P_Write(EE_TX_POWER_RC7_ID, 13);
  E2P_Write(EE_SGFX_RC_ID, SFX_RC1);    /* Default RC at device birth*/
  E2P_Write(EE_SGFX_KEYTYPE_ID, CREDENTIALS_KEY_PRIVATE);
  E2P_Write(EE_MACROCH_CONFIG_WORD0_RC2_ID, RC2_SET_STD_CONFIG_SM_WORD_0);
  E2P_Write(EE_MACROCH_CONFIG_WORD1_RC2_ID, RC2_SET_STD_CONFIG_SM_WORD_1);
  E2P_Write(EE_MACROCH_CONFIG_WORD2_RC2_ID, RC2_SET_STD_CONFIG_SM_WORD_2);
  E2P_Write(EE_MACROCH_CONFIG_WORD0_RC3A_ID, rc3a_config[0]);
  E2P_Write(EE_MACROCH_CONFIG_WORD1_RC3A_ID, rc3a_config[1]);
  E2P_Write(EE_MACROCH_CONFIG_WORD2_RC3A_ID, rc3a_config[2]);
  E2P_Write(EE_MACROCH_CONFIG_WORD0_RC3C_ID, rc3c_config[0]);
  E2P_Write(EE_MACROCH_CONFIG_WORD1_RC3C_ID, rc3c_config[1]);
  E2P_Write(EE_MACROCH_CONFIG_WORD2_RC3C_ID, rc3c_config[2]);
  E2P_Write(EE_MACROCH_CONFIG_WORD0_RC4_ID, RC4_SET_STD_CONFIG_SM_WORD_0);
  E2P_Write(EE_MACROCH_CONFIG_WORD1_RC4_ID, RC4_SET_STD_CONFIG_SM_WORD_1);
  E2P_Write(EE_MACROCH_CONFIG_WORD2_RC4_ID, RC4_SET_STD_CONFIG_SM_WORD_2);
  E2P_Write(EE_MACROCH_CONFIG_WORD0_RC5_ID, rc5_config[0]);
  E2P_Write(EE_MACROCH_CONFIG_WORD1_RC5_ID, rc5_config[1]);
  E2P_Write(EE_MACROCH_CONFIG_WORD2_RC5_ID, rc5_config[2]);
  E2P_Write(EE_SE_NVM_0_ID, 0xFF);
  E2P_Write(EE_SE_NVM_1_ID, 0x0F);
  E2P_Write(EE_SE_NVM_2_ID, 0);
  E2P_Write(EE_SE_NVM_3_ID, 0);
  E2P_Write(EE_SE_NVM_4_ID, 0xFF);
  E2P_Write(EE_MCU_NVM_0_ID, 0);
  E2P_Write(EE_MCU_NVM_1_ID, 0);
  E2P_Write(EE_MCU_NVM_2_ID, 0);
  E2P_Write(EE_MCU_NVM_3_ID, 0);
  E2P_Write(EE_SGFX_ENCRYPTIONFLAG_ID, 0);
  E2P_Write(EE_SGFX_VERBOSELEVEL_ID, VERBOSE_LEVEL);
  /* USER CODE BEGIN E2P_RestoreFs_2 */

  /* USER CODE END E2P_RestoreFs_2 */
}

int8_t E2P_Read_Power(sfx_rc_enum_t SgfxRc)
{
  /* USER CODE BEGIN E2P_Read_Power_1 */

  /* USER CODE END E2P_Read_Power_1 */
  uint32_t power;
  switch (SgfxRc)
  {
    case SFX_RC1:
      E2P_Read(EE_TX_POWER_RC1_ID, &power);
      break;
    case SFX_RC2:
      E2P_Read(EE_TX_POWER_RC2_ID, &power);
      break;
    case SFX_RC3A:
      E2P_Read(EE_TX_POWER_RC3A_ID, &power);
      break;
    case SFX_RC3C:
      E2P_Read(EE_TX_POWER_RC3C_ID, &power);
      break;
    case SFX_RC4:
      E2P_Read(EE_TX_POWER_RC4_ID, &power);
      break;
    case SFX_RC5:
      E2P_Read(EE_TX_POWER_RC5_ID, &power);
      break;
    case SFX_RC6:
      E2P_Read(EE_TX_POWER_RC6_ID, &power);
      break;
    case SFX_RC7:
      E2P_Read(EE_TX_POWER_RC7_ID, &power);
      break;
    default:
      break;
  }
  return (int8_t) power;
  /* USER CODE BEGIN E2P_Read_Power_2 */

  /* USER CODE END E2P_Read_Power_2 */
}

void E2P_Write_Power(sfx_rc_enum_t SgfxRc, int8_t power)
{
  /* USER CODE BEGIN E2P_Write_Power_1 */

  /* USER CODE END E2P_Write_Power_1 */
  switch (SgfxRc)
  {
    case SFX_RC1:
      E2P_Write(EE_TX_POWER_RC1_ID, (uint32_t) power);
      break;
    case SFX_RC2:
      E2P_Write(EE_TX_POWER_RC2_ID, (uint32_t) power);
      break;
    case SFX_RC3A:
      E2P_Write(EE_TX_POWER_RC3A_ID, (uint32_t) power);
      break;
    case SFX_RC3C:
      E2P_Write(EE_TX_POWER_RC3C_ID, (uint32_t) power);
      break;
    case SFX_RC4:
      E2P_Write(EE_TX_POWER_RC4_ID, (uint32_t) power);
      break;
    case SFX_RC5:
      E2P_Write(EE_TX_POWER_RC5_ID, (uint32_t) power);
      break;
    case SFX_RC6:
      E2P_Write(EE_TX_POWER_RC6_ID, (uint32_t) power);
      break;
    case SFX_RC7:
      E2P_Write(EE_TX_POWER_RC7_ID, (uint32_t) power);
      break;
    default:
      break;
  }
  /* USER CODE BEGIN E2P_Write_Power_2 */

  /* USER CODE END E2P_Write_Power_2 */
}

sfx_rc_enum_t E2P_Read_Rc(void)
{
  /* USER CODE BEGIN E2P_Read_Rc_1 */

  /* USER CODE END E2P_Read_Rc_1 */
  uint32_t SgfxRc;
  E2P_Read(EE_SGFX_RC_ID, &SgfxRc);
  return (sfx_rc_enum_t) SgfxRc;
  /* USER CODE BEGIN E2P_Read_Rc_2 */

  /* USER CODE END E2P_Read_Rc_2 */
}

void E2P_Write_Rc(sfx_rc_enum_t SgfxRc)
{
  /* USER CODE BEGIN E2P_Write_Rc_1 */

  /* USER CODE END E2P_Write_Rc_1 */
  E2P_Write(EE_SGFX_RC_ID, (uint32_t) SgfxRc);
  /* USER CODE BEGIN E2P_Write_Rc_2 */

  /* USER CODE END E2P_Write_Rc_2 */
}

int16_t E2P_Read_RssiCal(void)
{
  /* USER CODE BEGIN E2P_Read_RssiCal_1 */

  /* USER CODE END E2P_Read_RssiCal_1 */
  uint32_t rssi_cal;
  E2P_Read(EE_RSSI_CAL_ID, &rssi_cal);
  return (int16_t) rssi_cal;
  /* USER CODE BEGIN E2P_Read_RssiCal_2 */

  /* USER CODE END E2P_Read_RssiCal_2 */
}

void E2P_Write_RssiCal(int16_t rssi_cal)
{
  /* USER CODE BEGIN E2P_Write_RssiCal_1 */

  /* USER CODE END E2P_Write_RssiCal_1 */
  E2P_Write(EE_RSSI_CAL_ID, (uint32_t) rssi_cal);
  /* USER CODE BEGIN E2P_Write_RssiCal_2 */

  /* USER CODE END E2P_Write_RssiCal_2 */
}

uint32_t E2P_Read_AtEcho(void)
{
  /* USER CODE BEGIN E2P_Read_AtEcho_1 */

  /* USER CODE END E2P_Read_AtEcho_1 */
  uint32_t at_echo;
  E2P_Read(EE_AT_ECHO_ID, &at_echo);
  return (E2P_flagStatus_t) at_echo;
  /* USER CODE BEGIN E2P_Read_AtEcho_2 */

  /* USER CODE END E2P_Read_AtEcho_2 */
}

void E2P_Write_AtEcho(uint32_t at_echo)
{
  /* USER CODE BEGIN E2P_Write_AtEcho_1 */

  /* USER CODE END E2P_Write_AtEcho_1 */
  E2P_Write(EE_AT_ECHO_ID, (uint32_t) at_echo);
  /* USER CODE BEGIN E2P_Write_AtEcho_2 */

  /* USER CODE END E2P_Write_AtEcho_2 */
}

sfx_key_type_t E2P_Read_KeyType(void)
{
  /* USER CODE BEGIN E2P_Read_KeyType_1 */

  /* USER CODE END E2P_Read_KeyType_1 */
  uint32_t data_from_eeprom;
  E2P_Read(EE_SGFX_KEYTYPE_ID, &data_from_eeprom);
  return (sfx_key_type_t) data_from_eeprom;
  /* USER CODE BEGIN E2P_Read_KeyType_2 */

  /* USER CODE END E2P_Read_KeyType_2 */
}

void E2P_Write_KeyType(sfx_key_type_t key_type)
{
  /* USER CODE BEGIN E2P_Write_KeyType_1 */

  /* USER CODE END E2P_Write_KeyType_1 */
  E2P_Write(EE_SGFX_KEYTYPE_ID, (uint32_t) key_type);
  /* USER CODE BEGIN E2P_Write_KeyType_2 */

  /* USER CODE END E2P_Write_KeyType_2 */
}

uint8_t E2P_Read_EncryptionFlag(void)
{
  /* USER CODE BEGIN E2P_Read_EncryptionFlag_1 */

  /* USER CODE END E2P_Read_EncryptionFlag_1 */
  uint32_t data_from_eeprom;
  E2P_Read(EE_SGFX_ENCRYPTIONFLAG_ID, &data_from_eeprom);
  return (uint8_t) data_from_eeprom;
  /* USER CODE BEGIN E2P_Read_EncryptionFlag_2 */

  /* USER CODE END E2P_Read_EncryptionFlag_2 */
}

void E2P_Write_EncryptionFlag(sfx_u8 encryption_flag)
{
  /* USER CODE BEGIN E2P_Write_EncryptionFlag_1 */

  /* USER CODE END E2P_Write_EncryptionFlag_1 */
  E2P_Write(EE_SGFX_ENCRYPTIONFLAG_ID, (uint32_t) encryption_flag);
  /* USER CODE BEGIN E2P_Write_EncryptionFlag_2 */

  /* USER CODE END E2P_Write_EncryptionFlag_2 */
}

uint8_t E2P_Read_VerboseLevel(void)
{
  /* USER CODE BEGIN E2P_Read_VerboseLevel_1 */

  /* USER CODE END E2P_Read_VerboseLevel_1 */
  uint32_t data_from_eeprom;
  E2P_Read(EE_SGFX_VERBOSELEVEL_ID, &data_from_eeprom);
  return (uint8_t) data_from_eeprom;
  /* USER CODE BEGIN E2P_Read_VerboseLevel_2 */

  /* USER CODE END E2P_Read_VerboseLevel_2 */
}

void E2P_Write_VerboseLevel(uint8_t verboselevel)
{
  /* USER CODE BEGIN E2P_Write_VerboseLevel_1 */

  /* USER CODE END E2P_Write_VerboseLevel_1 */
  E2P_Write(EE_SGFX_VERBOSELEVEL_ID, (uint32_t) verboselevel);
  /* USER CODE BEGIN E2P_Write_VerboseLevel_2 */

  /* USER CODE END E2P_Write_VerboseLevel_2 */
}

void E2P_Read_ConfigWords(sfx_rc_enum_t sfx_rc, sfx_u32 config_words[3])
{
  /* USER CODE BEGIN E2P_Read_ConfigWords_1 */

  /* USER CODE END E2P_Read_ConfigWords_1 */
  switch (sfx_rc)
  {
    case SFX_RC1:
    {
      break;
    }
    case SFX_RC2:
    {
      E2P_Read(EE_MACROCH_CONFIG_WORD0_RC2_ID, (uint32_t *) &config_words[0]);
      E2P_Read(EE_MACROCH_CONFIG_WORD1_RC2_ID, (uint32_t *) &config_words[1]);
      E2P_Read(EE_MACROCH_CONFIG_WORD2_RC2_ID, (uint32_t *) &config_words[2]);
      break;
    }
    case SFX_RC3A:
    {
      E2P_Read(EE_MACROCH_CONFIG_WORD0_RC3A_ID, (uint32_t *) &config_words[0]);
      E2P_Read(EE_MACROCH_CONFIG_WORD1_RC3A_ID, (uint32_t *) &config_words[1]);
      E2P_Read(EE_MACROCH_CONFIG_WORD2_RC3A_ID, (uint32_t *) &config_words[2]);

      break;
    }
    case SFX_RC3C:
    {

      E2P_Read(EE_MACROCH_CONFIG_WORD0_RC3C_ID, (uint32_t *) &config_words[0]);
      E2P_Read(EE_MACROCH_CONFIG_WORD1_RC3C_ID, (uint32_t *) &config_words[1]);
      E2P_Read(EE_MACROCH_CONFIG_WORD2_RC3C_ID, (uint32_t *) &config_words[2]);

      break;
    }
    case SFX_RC4:
    {
      E2P_Read(EE_MACROCH_CONFIG_WORD0_RC4_ID, (uint32_t *) &config_words[0]);
      E2P_Read(EE_MACROCH_CONFIG_WORD1_RC4_ID, (uint32_t *) &config_words[1]);
      E2P_Read(EE_MACROCH_CONFIG_WORD2_RC4_ID, (uint32_t *) &config_words[2]);
      break;
    }
    case SFX_RC5:
    {
      E2P_Read(EE_MACROCH_CONFIG_WORD0_RC5_ID, (uint32_t *) &config_words[0]);
      E2P_Read(EE_MACROCH_CONFIG_WORD1_RC5_ID, (uint32_t *) &config_words[1]);
      E2P_Read(EE_MACROCH_CONFIG_WORD2_RC5_ID, (uint32_t *) &config_words[2]);

      break;
    }
    case SFX_RC6:
    {
      break;
    }
    case SFX_RC7:
    {
      break;
    }
    default:
      break;
  }
  /* USER CODE BEGIN E2P_Read_ConfigWords_2 */

  /* USER CODE END E2P_Read_ConfigWords_2 */
}

void E2P_Write_ConfigWords(sfx_rc_enum_t sfx_rc, sfx_u32 config_words[3])
{
  /* USER CODE BEGIN E2P_Write_ConfigWords_1 */

  /* USER CODE END E2P_Write_ConfigWords_1 */
  switch (sfx_rc)
  {
    case SFX_RC1:
    {
      break;
    }
    case SFX_RC2:
    {
      E2P_Write(EE_MACROCH_CONFIG_WORD0_RC2_ID, config_words[0]);
      E2P_Write(EE_MACROCH_CONFIG_WORD1_RC2_ID, config_words[1]);
      E2P_Write(EE_MACROCH_CONFIG_WORD2_RC2_ID, config_words[2]);
      break;
    }
    case SFX_RC3A:
    {
      E2P_Write(EE_MACROCH_CONFIG_WORD0_RC3A_ID, config_words[0]);
      E2P_Write(EE_MACROCH_CONFIG_WORD1_RC3A_ID, config_words[1]);
      E2P_Write(EE_MACROCH_CONFIG_WORD2_RC3A_ID, config_words[2]);

      break;
    }
    case SFX_RC3C:
    {

      E2P_Write(EE_MACROCH_CONFIG_WORD0_RC3C_ID, config_words[0]);
      E2P_Write(EE_MACROCH_CONFIG_WORD1_RC3C_ID, config_words[1]);
      E2P_Write(EE_MACROCH_CONFIG_WORD2_RC3C_ID, config_words[2]);

      break;
    }
    case SFX_RC4:
    {
      E2P_Write(EE_MACROCH_CONFIG_WORD0_RC4_ID, config_words[0]);
      E2P_Write(EE_MACROCH_CONFIG_WORD1_RC4_ID, config_words[1]);
      E2P_Write(EE_MACROCH_CONFIG_WORD2_RC4_ID, config_words[2]);
      break;
    }
    case SFX_RC5:
    {
      E2P_Write(EE_MACROCH_CONFIG_WORD0_RC5_ID, config_words[0]);
      E2P_Write(EE_MACROCH_CONFIG_WORD1_RC5_ID, config_words[1]);
      E2P_Write(EE_MACROCH_CONFIG_WORD2_RC5_ID, config_words[2]);
      break;
    }
    case SFX_RC6:
    {
      break;
    }
    case SFX_RC7:
    {
      break;
    }
    default:
      break;
  }
  /* USER CODE BEGIN E2P_Write_ConfigWords_2 */

  /* USER CODE END E2P_Write_ConfigWords_2 */
}

E2P_ErrorStatus_t E2P_Read_SeNvm(sfx_u8 *read_data, uint32_t len)
{
  /* USER CODE BEGIN E2P_Read_SeNvm_1 */

  /* USER CODE END E2P_Read_SeNvm_1 */
  E2P_ErrorStatus_t error = E2P_OK;
  int32_t i = 0;
  uint32_t data_from_eeprom;

  E2P_Read(EE_SE_NVM_0_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_SE_NVM_1_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_SE_NVM_2_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_SE_NVM_3_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_SE_NVM_4_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;

  if (i != len)
  {
    error =  E2P_KO;
  }
  return error;
  /* USER CODE BEGIN E2P_Read_SeNvm_2 */

  /* USER CODE END E2P_Read_SeNvm_2 */
}

E2P_ErrorStatus_t E2P_Write_SeNvm(sfx_u8 *data_to_write,  uint32_t len)
{
  /* USER CODE BEGIN E2P_Write_SeNvm_1 */

  /* USER CODE END E2P_Write_SeNvm_1 */
  E2P_ErrorStatus_t error = E2P_OK;
  int32_t i = 0;

  E2P_Write(EE_SE_NVM_0_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_SE_NVM_1_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_SE_NVM_2_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_SE_NVM_3_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_SE_NVM_4_ID, (uint32_t) data_to_write[i++]);

  if (i != len)
  {
    error =  E2P_KO;
  }
  return error;
  /* USER CODE BEGIN E2P_Write_SeNvm_2 */

  /* USER CODE END E2P_Write_SeNvm_2 */
}

E2P_ErrorStatus_t E2P_Read_McuNvm(sfx_u8 *read_data, uint32_t len)
{
  /* USER CODE BEGIN E2P_Read_McuNvm_1 */

  /* USER CODE END E2P_Read_McuNvm_1 */
  E2P_ErrorStatus_t error = E2P_OK;
  int32_t i = 0;
  uint32_t data_from_eeprom;
  E2P_Read(EE_MCU_NVM_0_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_MCU_NVM_1_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_MCU_NVM_2_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  E2P_Read(EE_MCU_NVM_3_ID, &data_from_eeprom);
  read_data[i++] = (sfx_u8) data_from_eeprom;
  if (i != len)
  {
    error =  E2P_KO;
  }
  return error;
  /* USER CODE BEGIN E2P_Read_McuNvm_2 */

  /* USER CODE END E2P_Read_McuNvm_2 */
}

E2P_ErrorStatus_t E2P_Write_McuNvm(sfx_u8 *data_to_write, uint32_t len)
{
  /* USER CODE BEGIN E2P_Write_McuNvm_1 */

  /* USER CODE END E2P_Write_McuNvm_1 */
  E2P_ErrorStatus_t error = E2P_OK;
  int32_t i = 0;

  E2P_Write(EE_MCU_NVM_0_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_MCU_NVM_1_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_MCU_NVM_2_ID, (uint32_t) data_to_write[i++]);
  E2P_Write(EE_MCU_NVM_3_ID, (uint32_t) data_to_write[i++]);

  if (i != len)
  {
    error =  E2P_KO;
  }
  return error;
  /* USER CODE BEGIN E2P_Write_McuNvm_2 */

  /* USER CODE END E2P_Write_McuNvm_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void E2P_Write(e_EE_ID addr, uint32_t data)
{
  /* USER CODE BEGIN E2P_Write_1 */

  /* USER CODE END E2P_Write_1 */
  if (EE_Write(EE_BANK_0, (uint16_t) addr, data) == EE_CLEAN_NEEDED)
  {
    EE_Clean(EE_BANK_0);
  }
  /* USER CODE BEGIN E2P_Write_2 */

  /* USER CODE END E2P_Write_2 */
}

static void E2P_Read(e_EE_ID addr, uint32_t *data)
{
  /* USER CODE BEGIN E2P_Read_1 */

  /* USER CODE END E2P_Read_1 */
  EE_Read(EE_BANK_0, (uint16_t) addr, data);
  /* USER CODE BEGIN E2P_Read_2 */

  /* USER CODE END E2P_Read_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
