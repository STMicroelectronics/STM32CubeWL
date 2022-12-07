/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_credentials.h
  * @author  MCD Application Team
  * @brief   interface to key manager and encryption algorithm
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
#ifndef __SGFX_CREDENTIALS_H__
#define __SGFX_CREDENTIALS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "sigfox_types.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define AES_KEY_LEN 16 /* bytes */

/* encrypted_sigfox_data defines size */
#define MANUF_DEVICE_ID_LENGTH      4
#define MANUF_SIGNATURE_LENGTH     16
#define MANUF_VER_LENGTH            1
#define MANUF_SPARE_1               3
#define MANUF_DEVICE_KEY_LENGTH    16
#define MANUF_PAC_LENGTH            8
#define MANUF_SPARE_2              14
#define MANUF_CRC_LENGTH            2

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
  * @brief      get the cmac library version
  * @return     string containing the library version
  */
const char *CREDENTIALS_get_version(void);

/**
  * @brief      get the loaded dev_id
  * @param[out] dev_id return the dev_id in the pointer
  */
void CREDENTIALS_get_dev_id(uint8_t *dev_id);

/**
  * @brief      get the loaded pac
  * @param[out] pac return the pac in the pointer
  */
void CREDENTIALS_get_initial_pac(uint8_t *pac);

/**
  * @brief      get the payload_encryption_flag
  * @retval     the payload_encryption_flag
  */
sfx_bool CREDENTIALS_get_payload_encryption_flag(void);

/**
  * @brief      set the payload_encryption_flag
  * @param[in]  enable flag. 0: disable, 1: enable
  */
void CREDENTIALS_set_payload_encryption_flag(uint8_t enable);

/**
  * @brief      encrypts the data_to_encrypt with aes secrete Key
  * @param[out] encrypted_data the encrypted data
  * @param[in]  data_to_encrypt
  * @param[in]  block_len the number of aes blocks
  * @return     sfx_error_t status
  */
sfx_error_t CREDENTIALS_aes_128_cbc_encrypt(uint8_t *encrypted_data, uint8_t *data_to_encrypt, uint8_t block_len);

/**
  * @brief      encrypts the data_to_encrypt with aes session Key
  * @param[out] encrypted_data the encrypted data
  * @param[in]  data_to_encrypt
  * @param[in]  block_len the number of aes blocks
  * @return     sfx_error_t status
  */
sfx_error_t CREDENTIALS_aes_128_cbc_encrypt_with_session_key(uint8_t *encrypted_data, uint8_t *data_to_encrypt,
                                                             uint8_t block_len);

/**
  * @brief      wraps the session Key
  * @param[in]  data the arguments used to generate the session Key
  * @param[in]  blocks the number of aes blocks
  * @return     sfx_error_t status
  */
sfx_error_t CREDENTIALS_wrap_session_key(uint8_t *data, uint8_t blocks);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SGFX_CREDENTIALS_H__ */
