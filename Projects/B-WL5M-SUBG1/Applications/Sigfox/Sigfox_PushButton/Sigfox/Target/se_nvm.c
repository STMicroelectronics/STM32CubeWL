/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    se_nvm.c
  * @author  MCD Application Team
  * @brief   manages SE nvm data
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
#include <string.h>
#include "sgfx_eeprom_if.h"
#include "st_sigfox_api.h"
#include "se_nvm.h"
#include "sgfx_credentials.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
sfx_u8 SE_NVM_get(sfx_u8 read_data[SFX_SE_NVMEM_BLOCK_SIZE])
{
  sfx_u8  ret = SFX_ERR_NONE;
  /* USER CODE BEGIN SE_NVM_get_1 */

  /* USER CODE END SE_NVM_get_1 */

  if (E2P_Read_SeNvm(read_data, SFX_SE_NVMEM_BLOCK_SIZE) != E2P_OK)
  {
    ret = SE_ERR_API_SE_NVM;
  }

  /* USER CODE BEGIN SE_NVM_get_2 */

  /* USER CODE END SE_NVM_get_2 */
  return ret;
}

sfx_u8 SE_NVM_set(sfx_u8 data_to_write[SFX_SE_NVMEM_BLOCK_SIZE])
{
  sfx_u8  ret = SFX_ERR_NONE;
  /* USER CODE BEGIN SE_NVM_set_1 */

  /* USER CODE END SE_NVM_set_1 */

  if (E2P_Write_SeNvm(data_to_write, SFX_SE_NVMEM_BLOCK_SIZE) != E2P_OK)
  {
    ret = SE_ERR_API_SE_NVM;
  }
  /* USER CODE BEGIN SE_NVM_set_2 */

  /* USER CODE END SE_NVM_set_2 */
  return ret;
}

sfx_key_type_t SE_NVM_get_key_type(void)
{
  /* USER CODE BEGIN SE_NVM_get_key_type_1 */

  /* USER CODE END SE_NVM_get_key_type_1 */
  return E2P_Read_KeyType();
  /* USER CODE BEGIN SE_NVM_get_key_type_2 */

  /* USER CODE END SE_NVM_get_key_type_2 */
}

void  SE_NVM_set_key_type(sfx_key_type_t keyType)
{
  /* USER CODE BEGIN SE_NVM_set_key_type_1 */

  /* USER CODE END SE_NVM_set_key_type_1 */
  E2P_Write_KeyType(keyType);
  /* USER CODE BEGIN SE_NVM_set_key_type_2 */

  /* USER CODE END SE_NVM_set_key_type_2 */
}

sfx_u8 SE_NVM_get_encrypt_flag(void)
{
  /* USER CODE BEGIN SE_NVM_get_encrypt_flag_1 */

  /* USER CODE END SE_NVM_get_encrypt_flag_1 */
  return E2P_Read_EncryptionFlag();
  /* USER CODE BEGIN SE_NVM_get_encrypt_flag_2 */

  /* USER CODE END SE_NVM_get_encrypt_flag_2 */
}

void  SE_NVM_set_encrypt_flag(sfx_u8 encryption_flag)
{
  /* USER CODE BEGIN SE_NVM_set_encrypt_flag_1 */

  /* USER CODE END SE_NVM_set_encrypt_flag_1 */
  E2P_Write_EncryptionFlag(encryption_flag);
  /* USER CODE BEGIN SE_NVM_set_encrypt_flag_2 */

  /* USER CODE END SE_NVM_set_encrypt_flag_2 */
}
/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
