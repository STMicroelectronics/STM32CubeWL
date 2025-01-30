/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    se_nvm.h
  * @author  MCD Application Team
  * @brief   header to emulated SE nvm data
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
#ifndef __SE_NVM_H__
#define __SE_NVM_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  CREDENTIALS_KEY_PRIVATE = 0,   /* private key */
  CREDENTIALS_KEY_PUBLIC  = 1,   /* public key  <i> 0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF</i>.*/
} sfx_key_type_t;

typedef enum
{
  SE_NVMEM_SEQNUM         = 0,                 /*!< Index of nv memory for PN */
  SE_NVMEM_RCSYNC_PERIOD  = 2,                 /*!< Index of nv memory for dedicated FH information */
  SE_NVMEM_ROLLOVER       = 4,
} se_nvmem_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define SFX_SE_NVMEM_BLOCK_SIZE  5
#define SE_ERR_API_SE_NVM                 (sfx_u8)(0x4A)

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief     This function reads data pointed by read_data to non volatile memory
  *
  * @param[in] read_data Pointer to the data block to write with the data stored in memory
  * @retval    SFX_ERR_NONE:                      No error
  * @retval    SE_ERR_API_SE_NVM:                 SE nvmem error
  */
sfx_u8 SE_NVM_get(sfx_u8 read_data[SFX_SE_NVMEM_BLOCK_SIZE]);

/**
  * @brief     This function write data pointed by data_to_write to non volatile memory
  *
  * @param[in] data_to_write Pointer to the data block to write with the data stored in memory
  * @retval    SFX_ERR_NONE:                      No error
  * @retval    SE_ERR_API_SE_NVM:                 SE nvmem error
  */
sfx_u8 SE_NVM_set(sfx_u8 data_to_write[SFX_SE_NVMEM_BLOCK_SIZE]);

/**
  * @brief     Get the active encryption payload flag
  * @retval    encryption_flag 0:Off, 1:On
  */
sfx_u8 SE_NVM_get_encrypt_flag(void);

/**
  * @brief     Set the active encryption payload flag
  * @param[in] encryption_flag 0:Off, 1:On
  */
void  SE_NVM_set_encrypt_flag(sfx_u8 encryption_flag);

/**
  * @brief     Get the active encryption key
  * @return    key public or private
  */
sfx_key_type_t SE_NVM_get_key_type(void);

/**
  * @brief     Set the active encryption key
  * @param[in] keyType key public or private
  */
void SE_NVM_set_key_type(sfx_key_type_t keyType);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
#ifdef __cplusplus
}
#endif

#endif /* __SE_NVM_H__ */
