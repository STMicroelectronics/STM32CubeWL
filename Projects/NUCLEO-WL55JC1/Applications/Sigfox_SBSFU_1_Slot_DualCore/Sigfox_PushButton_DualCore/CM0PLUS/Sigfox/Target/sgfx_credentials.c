/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_credentials.c
  * @author  MCD Application Team
  * @brief   manages keys and encryption algorithm
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
#include "st_sigfox_api.h"
#include "sgfx_credentials.h"
#include "se_nvm.h"
#include "app_sigfox.h"

#if (!defined (SIGFOX_KMS))
#warning SIGFOX_KMS not defined, forced to 0
#define SIGFOX_KMS 0
#endif /* !SIGFOX_KMS*/

#if (SIGFOX_KMS == 0)
#include "sigfox_aes.h"
#else /* SIGFOX_KMS == 1 */
#include "se_api.h"
#include "kms_if.h"
#endif /* SIGFOX_KMS */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
typedef struct manuf_device_info_s
{
  /* 16bits block 1 */
  sfx_u8 dev_id[MANUF_DEVICE_ID_LENGTH];
  sfx_u8 pac[MANUF_PAC_LENGTH];
  sfx_u8 ver[MANUF_VER_LENGTH];
  sfx_u8 spare1[MANUF_SPARE_1];
  /* 16bits block 2 */
  sfx_u8 dev_key[MANUF_DEVICE_KEY_LENGTH];
  /* 16bits block 3 */
  sfx_u8 spare2[MANUF_SPARE_2];
  sfx_u8 crc[MANUF_CRC_LENGTH];
} manuf_device_info_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/*Sigfox defines*/
#define KEY_128_LEN               16 /*bytes*/
#define CREDENTIALS_VERSION       11
#define PUBLIC_KEY                {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
#define N_BLOCK_128               16

/*CREDENTIAL_KEY may be used to encrypt sigfox_data
  CREDENTIAL_KEY must be aligned with the sigfox tool generating and encrypting the sigfox_data*/
/*
#define CREDENTIAL_KEY {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}
*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/*PREPROCESSOR CONVERSION*/
#define DECIMAL2STRING_DEF(s) #s
#define DECIMAL2STRING(s) DECIMAL2STRING_DEF(s)

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const char sgfxSeeLibVersion[] = "." DECIMAL2STRING(CREDENTIALS_VERSION);

#if (SIGFOX_KMS == 0)
extern sfx_u8 encrypted_sigfox_data[ sizeof(manuf_device_info_t) ];

static uint8_t device_public_key[] = PUBLIC_KEY;
static sigfox_aes_context AesContext;
static uint8_t session_key[KEY_128_LEN] = {0};
#else  /* SIGFOX_KMS */

static CK_ULONG DeriveKey_template_class = CKO_SECRET_KEY;
static CK_ULONG DeriveKey_template_destroyable = CK_TRUE;
static CK_ULONG DeriveKey_template_encrypt = CK_TRUE;
static CK_ULONG DeriveKey_template_decrypt = CK_TRUE;
static CK_ULONG DeriveKey_template_extract = CK_FALSE;

static CK_ATTRIBUTE DeriveKey_template[] =
{
  {CKA_CLASS, (CK_VOID_PTR) &DeriveKey_template_class,       sizeof(CK_ULONG)},
  {CKA_DESTROYABLE, (CK_VOID_PTR) &DeriveKey_template_destroyable, sizeof(CK_ULONG)},
  {CKA_ENCRYPT, (CK_VOID_PTR) &DeriveKey_template_encrypt,     sizeof(CK_ULONG)},
  {CKA_DECRYPT, (CK_VOID_PTR) &DeriveKey_template_decrypt,     sizeof(CK_ULONG)},
  {CKA_EXTRACTABLE, (CK_VOID_PTR) &DeriveKey_template_extract,     sizeof(CK_ULONG)}
};
static CK_OBJECT_HANDLE kms_sgfx_session_key_handle;
#endif  /* SIGFOX_KMS */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#if (SIGFOX_KMS == 0)
static sfx_error_t CREDENTIALS_get_cra(sfx_u8 *decrypted_data, sfx_u8 *data_to_decrypt, sfx_u8 data_len);
static void CREDENTIALS_get_key(uint8_t *key, sfx_key_type_t KeyType);
#else
static sfx_error_t Sgfx_KMS_AES_CBC_encrypt(sfx_u8 *data_to_encrypt, sfx_u8 *encrypted_data,
                                            CK_OBJECT_HANDLE key_object_handle, sfx_u8 length);
static sfx_error_t Sgfx_KMS_DeriveKey(uint8_t *input, CK_OBJECT_HANDLE derivedkeyObj);
static sfx_error_t Sgfx_KMS_GetAttribute(CK_OBJECT_HANDLE input, uint8_t *pAttrVal);
static void Sgfx_KMS_DeleteDerivedKeys(CK_OBJECT_HANDLE input);
#endif /* SIGFOX_KMS */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
sfx_error_t CREDENTIALS_aes_128_cbc_encrypt(uint8_t *encrypted_data, uint8_t *data_to_encrypt, uint8_t blocks)
{
  sfx_error_t retval = SFX_ERR_NONE;
  /* USER CODE BEGIN CREDENTIALS_aes_128_cbc_encrypt_1 */

  /* USER CODE END CREDENTIALS_aes_128_cbc_encrypt_1 */
#if (SIGFOX_KMS == 0)
  uint8_t iv[N_BLOCK_128] = {0x00};

  uint8_t key[AES_KEY_LEN];

  sfx_key_type_t KeyType = SE_NVM_get_key_type();

  CREDENTIALS_get_key(key, KeyType);

  sigfox_aes_set_key(key, AES_KEY_LEN,  &AesContext);
  /* Clear Key */
  memset(key, 0, AES_KEY_LEN);

  sigfox_aes_cbc_encrypt(data_to_encrypt,
                         encrypted_data,
                         blocks,
                         iv,
                         &AesContext);
#else
  sfx_key_type_t KeyType = SE_NVM_get_key_type();
  switch (KeyType)
  {
    case CREDENTIALS_KEY_PRIVATE:
      retval = Sgfx_KMS_AES_CBC_encrypt(data_to_encrypt, encrypted_data, (CK_OBJECT_HANDLE)KMS_SGFX_DATA_KEY_OBJECT_HANDLE, blocks);
      break;
    case CREDENTIALS_KEY_PUBLIC:
      retval = Sgfx_KMS_AES_CBC_encrypt(data_to_encrypt, encrypted_data, (CK_OBJECT_HANDLE)KMS_SGFX_DATA_KEY_PUBLIC_OBJECT_HANDLE, blocks);
      break;
    default:
      break;
  }
#endif /* SIGFOX_KMS */
  /* USER CODE BEGIN CREDENTIALS_aes_128_cbc_encrypt_2 */

  /* USER CODE END CREDENTIALS_aes_128_cbc_encrypt_2 */
  return retval;
}

sfx_error_t CREDENTIALS_aes_128_cbc_encrypt_with_session_key(uint8_t *encrypted_data, uint8_t *data_to_encrypt,
                                                             uint8_t blocks)
{
  sfx_error_t retval = SFX_ERR_NONE;
  /* USER CODE BEGIN CREDENTIALS_aes_128_cbc_encrypt_with_session_key_1 */

  /* USER CODE END CREDENTIALS_aes_128_cbc_encrypt_with_session_key_1 */
#if (SIGFOX_KMS == 0)
  uint8_t iv[N_BLOCK_128] = {0x00};

  sigfox_aes_set_key(session_key, AES_KEY_LEN,  &AesContext);

  sigfox_aes_cbc_encrypt(data_to_encrypt,
                         encrypted_data,
                         blocks,
                         iv,
                         &AesContext);
#else
  /* encrypt with session key */
  retval = Sgfx_KMS_AES_CBC_encrypt(data_to_encrypt, encrypted_data, (CK_OBJECT_HANDLE)kms_sgfx_session_key_handle, blocks);
#endif /* SIGFOX_KMS */
  /* USER CODE BEGIN CREDENTIALS_aes_128_cbc_encrypt_with_session_key_2 */

  /* USER CODE END CREDENTIALS_aes_128_cbc_encrypt_with_session_key_2 */
  return retval;
}

sfx_error_t CREDENTIALS_wrap_session_key(uint8_t *data, uint8_t blocks)
{
  sfx_error_t retval = SFX_ERR_NONE;
  /* USER CODE BEGIN CREDENTIALS_wrap_session_key_1 */

  /* USER CODE END CREDENTIALS_wrap_session_key_1 */
#if (SIGFOX_KMS == 0)
  uint8_t iv[N_BLOCK_128] = {0x00};

  uint8_t key[AES_KEY_LEN];

  CREDENTIALS_get_key(key, CREDENTIALS_KEY_PRIVATE);

  sigfox_aes_set_key(key, AES_KEY_LEN,  &AesContext);

  memset(key, 0, AES_KEY_LEN);

  sigfox_aes_cbc_encrypt(data,
                         session_key,
                         blocks,
                         iv,
                         &AesContext);
#else
  /* DestroyKey: to be done before new deriveKey */
  Sgfx_KMS_DeleteDerivedKeys(kms_sgfx_session_key_handle);

  retval = Sgfx_KMS_DeriveKey(data, (CK_OBJECT_HANDLE)kms_sgfx_session_key_handle);
#endif /* SIGFOX_KMS */
  /* USER CODE BEGIN CREDENTIALS_wrap_session_key_2 */

  /* USER CODE END CREDENTIALS_wrap_session_key_2 */
  return retval;
}

const char *CREDENTIALS_get_version(void)
{
  /* USER CODE BEGIN CREDENTIALS_get_version_1 */

  /* USER CODE END CREDENTIALS_get_version_1 */
  return sgfxSeeLibVersion;
  /* USER CODE BEGIN CREDENTIALS_get_version_2 */

  /* USER CODE END CREDENTIALS_get_version_2 */
}

void CREDENTIALS_get_dev_id(uint8_t *dev_id)
{
  /* USER CODE BEGIN CREDENTIALS_get_dev_id_1 */

  /* USER CODE END CREDENTIALS_get_dev_id_1 */
#if (SIGFOX_KMS == 0)
  manuf_device_info_t DeviceInfo;

  CREDENTIALS_get_cra((uint8_t *) &DeviceInfo, encrypted_sigfox_data, sizeof(manuf_device_info_t));

  memcpy(dev_id, DeviceInfo.dev_id, MANUF_DEVICE_ID_LENGTH);

  /*clear key*/
  memset(DeviceInfo.dev_key, 0, AES_KEY_LEN);
#else
  Sgfx_KMS_GetAttribute(KMS_SGFX_ID_OBJECT_HANDLE, dev_id);
#endif /* SIGFOX_KMS */
  /* USER CODE BEGIN CREDENTIALS_get_dev_id_2 */

  /* USER CODE END CREDENTIALS_get_dev_id_2 */
}

void CREDENTIALS_get_initial_pac(uint8_t *pac)
{
  /* USER CODE BEGIN CREDENTIALS_get_initial_pac_1 */

  /* USER CODE END CREDENTIALS_get_initial_pac_1 */
#if (SIGFOX_KMS == 0)
  manuf_device_info_t DeviceInfo;

  CREDENTIALS_get_cra((uint8_t *) &DeviceInfo, encrypted_sigfox_data, sizeof(manuf_device_info_t));
  /*clear key*/
  memcpy(pac, DeviceInfo.pac, MANUF_PAC_LENGTH);
  /*clear key*/
  memset(DeviceInfo.dev_key, 0, AES_KEY_LEN);
#else
  Sgfx_KMS_GetAttribute(KMS_SGFX_PAC_OBJECT_HANDLE, pac);
#endif /* SIGFOX_KMS */
  /* USER CODE BEGIN CREDENTIALS_get_initial_pac_2 */

  /* USER CODE END CREDENTIALS_get_initial_pac_2 */
}

sfx_bool CREDENTIALS_get_payload_encryption_flag(void)
{
  sfx_bool ret = SFX_FALSE;

  ret = (sfx_bool) SE_NVM_get_encrypt_flag();
  /* USER CODE BEGIN CREDENTIALS_get_payload_encryption_flag_1 */

  /* USER CODE END CREDENTIALS_get_payload_encryption_flag_1 */
  return ret;
}

void CREDENTIALS_set_payload_encryption_flag(uint8_t enable)
{
  /* USER CODE BEGIN CREDENTIALS_set_payload_encryption_flag_1 */

  /* USER CODE END CREDENTIALS_set_payload_encryption_flag_1 */
  if (enable == 0)
  {
    SE_NVM_set_encrypt_flag(0);
  }
  else
  {
    SE_NVM_set_encrypt_flag(1);
  }
  /* USER CODE BEGIN CREDENTIALS_set_payload_encryption_flag_2 */

  /* USER CODE END CREDENTIALS_set_payload_encryption_flag_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
#if (SIGFOX_KMS == 0)
static void CREDENTIALS_get_key(uint8_t *key, sfx_key_type_t KeyType)
{
  /* USER CODE BEGIN CREDENTIALS_get_key_1 */

  /* USER CODE END CREDENTIALS_get_key_1 */
  switch (KeyType)
  {
    case CREDENTIALS_KEY_PUBLIC:
    {
      memcpy(key, device_public_key, AES_KEY_LEN);

      break;
    }
    case CREDENTIALS_KEY_PRIVATE:
    {
      manuf_device_info_t DeviceInfo;

      CREDENTIALS_get_cra((uint8_t *) &DeviceInfo, encrypted_sigfox_data, sizeof(manuf_device_info_t));

      memcpy(key, DeviceInfo.dev_key, AES_KEY_LEN);

      memset(DeviceInfo.dev_key, 0, AES_KEY_LEN);

      break;
    }
    default:
      break;
  }
  /* USER CODE BEGIN CREDENTIALS_get_key_2 */

  /* USER CODE END CREDENTIALS_get_key_2 */
}

static sfx_error_t CREDENTIALS_get_cra(sfx_u8 *decrypted_data, sfx_u8 *data_to_decrypt, sfx_u8 data_len)
{
  /* USER CODE BEGIN CREDENTIALS_get_cra_1 */

  /* USER CODE END CREDENTIALS_get_cra_1 */
#ifdef CREDENTIAL_KEY
  uint8_t iv[N_BLOCK_128] = {0x00};

  uint8_t CredentialKey[AES_KEY_LEN] = CREDENTIAL_KEY;

  /*device is provisioned with sigfox_data.h   */
  /*encrypted with CREDENTIAL_KEY in Sigfox Tool*/
  sigfox_aes_set_key(CredentialKey, AES_KEY_LEN,  &AesContext);

  memset(CredentialKey, 0, AES_KEY_LEN);

  sigfox_aes_cbc_decrypt(data_to_decrypt,
                         decrypted_data,
                         sizeof(manuf_device_info_t) / AES_KEY_LEN,
                         iv,
                         &AesContext);
#else
  /* default sigfox_data.h provided, sigfox_data.h is not encrypted*/
  memcpy((uint8_t *) decrypted_data, (uint8_t *) data_to_decrypt, sizeof(manuf_device_info_t));
#endif /* CREDENTIAL_KEY */
  /* USER CODE BEGIN CREDENTIALS_get_cra_2 */

  /* USER CODE END CREDENTIALS_get_cra_2 */
  return SFX_ERR_NONE;
}

#else /* SIGFOX_KMS == 1 */

static sfx_error_t Sgfx_KMS_AES_CBC_encrypt(sfx_u8 *data_to_encrypt, sfx_u8 *encrypted_data,
                                            CK_OBJECT_HANDLE key_object_handle, sfx_u8 length)
{
  /* USER CODE BEGIN Sgfx_KMS_AES_CBC_encrypt_1 */

  /* USER CODE END Sgfx_KMS_AES_CBC_encrypt_1 */
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t length16Bytes = 0;
  uint32_t encrypted_length = 0;
  uint8_t tag[16] = {0};
  uint32_t tag_length = 0;
  sfx_error_t retval = SFX_ERR_NONE;

  /* AES CBC Configuration variables */
  uint8_t cbc_iv[N_BLOCK_128] = {0x00};
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)cbc_iv, sizeof(cbc_iv) };

  /* convert blocks is 16bytes (128b) input KMS is in byte */
  length16Bytes = length << 4;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* -------------------------------------------------------------------------*/
  /* Encryption --------------------------------------------------------------*/
  /* -------------------------------------------------------------------------*/

  /* Configure session to Authentication message in AES CMAC with settings included into the mechanism */
  if (rv == CKR_OK)
  {

    /* encrypt with Sigfox key */
    rv = C_EncryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)key_object_handle);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    encrypted_length = length16Bytes;
    rv = C_EncryptUpdate(session, (CK_BYTE_PTR)data_to_encrypt, length16Bytes,
                         encrypted_data, (CK_ULONG_PTR)&encrypted_length);
  }

  /* Finalize message Authentication */
  if (rv == CKR_OK)
  {
    rv = C_EncryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_length);
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);
  if (rv != CKR_OK)
  {
    retval = SE_ERR_API_SECURE_UP_MSG;
  }

  /* USER CODE BEGIN Sgfx_KMS_AES_CBC_encrypt_2 */

  /* USER CODE END Sgfx_KMS_AES_CBC_encrypt_2 */
  return retval;
}

static sfx_error_t Sgfx_KMS_DeriveKey(uint8_t *input, CK_OBJECT_HANDLE derivedkeyObj)
{
  /* USER CODE BEGIN Sgfx_KMS_DeriveKey_1 */

  /* USER CODE END Sgfx_KMS_DeriveKey_1 */
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  /* Key derivation */
  CK_MECHANISM      mech = {CKM_AES_ECB_ENCRYPT_DATA, input, AES_KEY_LEN};
  sfx_error_t retval = SFX_ERR_NONE;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Derive key with pass phrase */
  if (rv == CKR_OK)
  {
    rv = C_DeriveKey(session, &(mech), (CK_OBJECT_HANDLE)KMS_SGFX_DATA_KEY_OBJECT_HANDLE,
                     &DeriveKey_template[0], sizeof(DeriveKey_template) / sizeof(CK_ATTRIBUTE), &derivedkeyObj);
  }
  if (rv != CKR_OK)
  {
    retval = SE_ERR_API_SECURE_UP_MSG;
  }
  /* store generated derivedkeyObj */
  kms_sgfx_session_key_handle = derivedkeyObj;

  /* Close session with KMS */
  (void)C_CloseSession(session);

  /* USER CODE BEGIN Sgfx_KMS_DeriveKey_2 */

  /* USER CODE END Sgfx_KMS_DeriveKey_2 */
  return retval;
}

static sfx_error_t Sgfx_KMS_GetAttribute(CK_OBJECT_HANDLE input, uint8_t *pAttrVal)
{
  /* USER CODE BEGIN Sgfx_KMS_GetAttribute_1 */

  /* USER CODE END Sgfx_KMS_GetAttribute_1 */
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  sfx_error_t retval = SFX_ERR_NONE;
  uint8_t attrlength;

  /* Determine size of AttrVal tab*/
  if (input == KMS_SGFX_ID_OBJECT_HANDLE)
  {
    attrlength = MANUF_DEVICE_ID_LENGTH;
  }
  else if (input == KMS_SGFX_PAC_OBJECT_HANDLE)
  {
    attrlength = MANUF_PAC_LENGTH;
  }
  else
  {
    return -1;
  }

  CK_ATTRIBUTE pAttr[] = {{CKA_VALUE, (CK_VOID_PTR) &DeriveKey_template_class, attrlength}};

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Get derived key to display */
  /* possible only because DeriveKey_template DeriveKey_template_extract = TRUE */
  /* after debug phase DeriveKey_template_extract shall be FALSE */
  if (rv == CKR_OK)
  {
    pAttr[0].pValue = pAttrVal;
    rv = C_GetAttributeValue(session, (CK_OBJECT_HANDLE) input, &(pAttr[0]), sizeof(pAttr) /
                             sizeof(CK_ATTRIBUTE));
  }

  if (rv != CKR_OK)
  {
    if (input == KMS_SGFX_ID_OBJECT_HANDLE)
    {
      retval = SE_ERR_API_GET_ID;
    }
    else
    {
      retval = SE_ERR_API_GET_PAC;
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  /* USER CODE BEGIN Sgfx_KMS_GetAttribute_2 */

  /* USER CODE END Sgfx_KMS_GetAttribute_2 */
  return retval;
}

/*!
 * \brief Deletion of Derived Key
 * \param [IN] input handle to be destroyed
 * \param [OUT] NONE
 * NOTE: C_DestroyObject() KMS function output isn't transmitted to higher layers
 * because the 1st call, as no handle is created, returns an error
 */
static void Sgfx_KMS_DeleteDerivedKeys(CK_OBJECT_HANDLE input)
{
  /* USER CODE BEGIN Sgfx_KMS_DeleteDerivedKeys_1 */

  /* USER CODE END Sgfx_KMS_DeleteDerivedKeys_1 */
#if (!defined (SIGFOX_KMS) || (SIGFOX_KMS == 0))
#else /* SIGFOX_KMS == 1 */
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  if (rv == CKR_OK)
  {
    C_DestroyObject(session, input);
  }

  /* Close sessions */
  if (session > 0)
  {
    (void)C_CloseSession(session);
  }
#endif /* SIGFOX_KMS == 1 */
  /* USER CODE BEGIN Sgfx_KMS_DeleteDerivedKeys_2 */

  /* USER CODE END Sgfx_KMS_DeleteDerivedKeys_2 */
}

#endif /* SIGFOX_KMS */

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
