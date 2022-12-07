/**
  ******************************************************************************
  * @file    sfu_interface_crypto_scheme.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE Interface
  *          functionalities. These services are used by the bootloader.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"           /* se_interface_bootloader.c is compiled in SBSFU project using main.h from this project */
#include "se_low_level.h"   /* This file is part of SE_CoreBin and adapts the Secure Engine (and its interface) to the 
                               STM32 board specificities */
#include "tkms.h"
#include "sfu_interface_crypto_scheme.h"
#include <string.h>
#include "kms_platf_objects_interface.h"     /* To get keys handle definitions */

/* Private defines -----------------------------------------------------------*/
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
#define GCM_TAGBITS_SIZE          128
#define GCM_IVLEN_BITS            16
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

/* Private variables ---------------------------------------------------------*/
CK_SESSION_HANDLE EncryptSession;
CK_SESSION_HANDLE DecryptSession;
CK_SESSION_HANDLE AuthenticateSession;
CK_SESSION_HANDLE VerifySession;
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
static uint8_t *gcm_tag_header;
#endif /* SECBOOT_CRYPTO_SCHEME */

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Get AES key handle.
  * @param  pxSE_Metadata Metadata that will be used to identify the keys.
  * @param  phAESKey pointer on AES key object handle
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
static SE_ErrorStatus SFU_GetAESKeyObject(SE_FwRawHeaderTypeDef *pxSE_Metadata, CK_OBJECT_HANDLE *phAESKey)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  /* Each magic corresponds to a different key:
   * SFUM_1 = SBSFU_KEY_1
   * SFUM_2 = SBSFU_KEY_2
   * SFUM_3 = SBSFU_KEY_3
   */
  if (memcmp(pxSE_Metadata->SFUMagic, SFUM_1, strlen(SFUM_1)) == 0)
  {
    *phAESKey = KMS_SBSFU_KEY_1_AES128_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#if (SFU_NB_MAX_ACTIVE_IMAGE > 1U)
  else if (memcmp(pxSE_Metadata->SFUMagic, SFUM_2, strlen(SFUM_2)) == 0)
  {
    *phAESKey = KMS_SBSFU_KEY_2_AES128_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#endif  /* (NB_FW_IMAGES > 1) */
#if (SFU_NB_MAX_ACTIVE_IMAGE > 2U)
  else if (memcmp(pxSE_Metadata->SFUMagic, SFUM_3, strlen(SFUM_3)) == 0)
  {
    *phAESKey = KMS_SBSFU_KEY_3_AES128_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#endif  /* (NB_FW_IMAGES > 2) */
  else
  {
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}
#endif /* ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)) */

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
/**
  * @brief  Get ECDSA key handle.
  * @param  pxSE_Metadata Metadata that will be used to identify the keys.
  * @param  phAESKey pointer on AES key object handle
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
static SE_ErrorStatus SFU_GetECDSAKeyObject(SE_FwRawHeaderTypeDef *pxSE_Metadata, CK_OBJECT_HANDLE *phECDSAKey)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  /* Each magic corresponds to a different key:
   * SFUM_1 = SBSFU_KEY_1
   * SFUM_2 = SBSFU_KEY_2
   * SFUM_3 = SBSFU_KEY_3
   */
  if (memcmp(pxSE_Metadata->SFUMagic, SFUM_1, strlen(SFUM_1)) == 0)
  {
    *phECDSAKey = KMS_SBSFU_KEY_1_ECDSA_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#if (SFU_NB_MAX_ACTIVE_IMAGE > 1U)
  else if (memcmp(pxSE_Metadata->SFUMagic, SFUM_2, strlen(SFUM_2)) == 0)
  {
    *phECDSAKey = KMS_SBSFU_KEY_2_ECDSA_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#endif  /* (NB_FW_IMAGES > 1) */
#if (SFU_NB_MAX_ACTIVE_IMAGE > 2U)
  else if (memcmp(pxSE_Metadata->SFUMagic, SFUM_3, strlen(SFUM_3)) == 0)
  {
    *phECDSAKey = KMS_SBSFU_KEY_3_ECDSA_OBJECT_HANDLE;
    e_ret_status = SE_SUCCESS;
  }
#endif  /* (NB_FW_IMAGES > 2) */
  else
  {
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}
#endif /*((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)) */


/**
  * @brief Secure Engine Decrypt Init function.
  *        It is a wrapper of Decrypt_Init function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pxSE_Metadata Metadata that will be used to fill the Crypto Init structure.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata, uint32_t SE_FwType)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  CK_RV rv = CKR_OK;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint8_t *gcm_iv = pxSE_Metadata->Nonce;
  CK_OBJECT_HANDLE hAESKey = 0UL;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    NULL,
    0,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

  gcm_tag_header = pxSE_Metadata->FwTag;

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  CK_RV rv = CKR_OK;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint8_t *iv_cbc = pxSE_Metadata->InitVector;
  CK_OBJECT_HANDLE hAESKey = 0UL;

  CK_MECHANISM cktest_aes_cbc_mechanism =
  {
    CKM_AES_CBC,
    iv_cbc,
    SE_IV_LEN
  };
#elif SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
  /*
   * In this crypto scheme the Firmware is not encrypted.
   * The Decrypt operation is called anyhow before installing the firmware.
   * Indeed, it allows moving the Firmware image blocks in FLASH.
   * These moves are mandatory to create the appropriate mapping in FLASH
   * allowing the swap procedure to run without using the swap area at each and every move.
   *
   * See in SB_SFU project: @ref SFU_IMG_PrepareCandidateImageForInstall.
   */
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointer allocation */
  if (pxSE_Metadata == NULL)
  {
    return SE_ERROR;
  }
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &DecryptSession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* Identify the keys after FW header analysis (magic tag) */
  if (SFU_GetAESKeyObject(pxSE_Metadata, &hAESKey) != SE_SUCCESS)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#endif /* (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) */

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  if (se_status == SE_SUCCESS)
  {
    /* Initialize the decryption session */
    rv = C_DecryptInit(DecryptSession, &cktest_aes_gcm_mechanism, hAESKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }
  }

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  /* Initialize the decryption session */
  if (se_status == SE_SUCCESS)
  {
    rv = C_DecryptInit(DecryptSession, &cktest_aes_cbc_mechanism, hAESKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }
  }

#elif SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
  /* Nothing to do as we won't decrypt anything */
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Decrypt Append function.
  *        It is a wrapper of Decrypt_Append function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pInputBuffer pointer to Input Buffer.
  * @param uInputSize Input Size (bytes).
  * @param pOutputBuffe pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                  uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)\
     || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  CK_RV rv = CKR_OK;
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  /* DecryptImageInDwlSlot() always starts by calling the Decrypt service with a 0 byte buffer */
  if (0 == InputSize)
  {
    /* Nothing to do but we must return a success for the decrypt operation to continue */
    return (SE_SUCCESS);
  }

  /* Check the pointers allocation */
  if ((pInputBuffer == NULL) || (pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)\
     || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  /* Buffer decryption */
  rv = C_DecryptUpdate(DecryptSession, (CK_BYTE *)pInputBuffer, InputSize, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
  /*
   * The firmware is not encrypted.
   * The only thing we need to do is to recopy the input buffer in the output buffer
   */
  (void)memcpy(pOutputBuffer, pInputBuffer, (uint32_t)InputSize);
  *pOutputSize = InputSize;
  se_status = SE_SUCCESS;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  return se_status;
}

/**
  * @brief Secure Engine Decrypt Finish function.
  *        It is a wrapper of Decrypt_Finish function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  *peSE_Status = SE_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  CK_RV rv = CKR_OK;
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  /* Check the pointers allocation */
  if ((pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)\
     || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  memcpy(pOutputBuffer, gcm_tag_header, 16);

  /* End-up decryption session */
  rv = C_DecryptFinal(DecryptSession, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
  /* Nothing to do */
  se_status = SE_SUCCESS;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  /* Close the session */
  rv = C_CloseSession(DecryptSession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;
#endif /* (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) */

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Init function.
  *        It is a wrapper of AuthenticateFW_Init function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pxSE_Metadata Metadata that will be used to fill the Crypto Init structure.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata,
                                       uint32_t SE_FwType)
{
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  uint8_t *gcm_iv =  pxSE_Metadata->Nonce;
  CK_OBJECT_HANDLE hAESKey;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    NULL,
    0,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256) )
  CK_MECHANISM cktest_sha_256_mechanism =
  {
    CKM_SHA256,
    NULL,
    0
  };
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointer allocation */
  if (pxSE_Metadata == NULL)
  {
    return SE_ERROR;
  }

  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &AuthenticateSession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Identify the keys after FW header analysis (magic tag) */
  if (SFU_GetAESKeyObject(pxSE_Metadata, &hAESKey) != SE_SUCCESS)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  if (se_status == SE_SUCCESS)
  {
    /* Initialize encryption session  */
    rv = C_EncryptInit(AuthenticateSession, &cktest_aes_gcm_mechanism, hAESKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256) )
  /* Digest initialization */
  rv = C_DigestInit(AuthenticateSession, &cktest_sha_256_mechanism);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Append function.
  *        It is a wrapper of AuthenticateFW_Append function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pInputBuffer pointer to Input Buffer.
  * @param uInputSize Input Size (bytes).
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                         uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pInputBuffer == NULL) || (pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Buffer encryption */
  rv = C_EncryptUpdate(AuthenticateSession, (CK_BYTE *)pInputBuffer, InputSize, pOutputBuffer,
                       (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256) )
  /* Digest calculation */
  rv = C_DigestUpdate(AuthenticateSession, (CK_BYTE_PTR)pInputBuffer, InputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Finish function.
  *        It is a wrapper of AuthenticateFW_Finish function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* End-up encryption session */
  rv = C_EncryptFinal(AuthenticateSession, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256) )
  /* End-up Digest calculation */
  rv = C_DigestFinal(AuthenticateSession, (CK_BYTE_PTR)pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Close the session */
  rv = C_CloseSession(AuthenticateSession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;
}

/**
  * @brief Secure Engine Verify Raw Fw Header Tag.
  *        It verifies the signature of a raw header file.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pFwRawHeader pointer to RawHeader Buffer.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_VerifyHeaderSignature(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxFwRawHeader)
{
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  int32_t fw_raw_header_output_length = SE_TAG_LEN;
  uint8_t *gcm_iv = pxFwRawHeader->Nonce;
  SE_FwRawHeaderTypeDef *gcm_header = pxFwRawHeader;
  CK_OBJECT_HANDLE hAESKey = 0UL;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    (CK_BYTE_PTR)gcm_header,
    ((int32_t)SE_FW_AUTH_LEN),
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)\
       || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))

  const uint8_t *pPayload;                        /* Metadata payload */
  int32_t payloadSize;                            /* Metadata length to be considered for hash */
  uint8_t *pSignature = pxFwRawHeader->HeaderSignature; /* Signature ECDSA */
  CK_OBJECT_HANDLE hECDSAKey = 0UL;
  CK_MECHANISM cktest_ecdsa_256_mechanism =
  {
    CKM_ECDSA_SHA256,
    NULL,
    0
  };
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointers allocation */
  if (NULL == pxFwRawHeader)
  {
    *peSE_Status = SE_KO;
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  gcm_tag_header = pxFwRawHeader->HeaderSignature;
#endif /* SECBOOT_CRYPTO_SCHEME */


  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &VerifySession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Identify the keys after FW header analysis (magic tag) */
  if (SFU_GetAESKeyObject(pxFwRawHeader, &hAESKey) != SE_SUCCESS)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  if (se_status == SE_SUCCESS)
  {
    /* Initialize the decryption session */
    rv = C_DecryptInit(VerifySession, &cktest_aes_gcm_mechanism, hAESKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }
  }

  /* End-up decryption session  : control if the tag is correct */
  rv = C_DecryptFinal(VerifySession, gcm_tag_header, (CK_ULONG_PTR)&fw_raw_header_output_length);

  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)\
       || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))

  pPayload = (const uint8_t *)pxFwRawHeader;
  payloadSize = SE_FW_AUTH_LEN;          /* Authenticated part of the header */

  /* Identify the keys after FW header analysis (magic tag) */
  if (SFU_GetECDSAKeyObject(pxFwRawHeader, &hECDSAKey) != SE_SUCCESS)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  if (se_status == SE_SUCCESS)
  {
    /* Initialize the verification process */
    rv = C_VerifyInit(VerifySession, &cktest_ecdsa_256_mechanism, hECDSAKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }
  }

  /* End-up verification process : control if the signature is correct */
  rv = C_Verify(VerifySession, (CK_BYTE_PTR)pPayload, payloadSize, (CK_BYTE_PTR)pSignature, SE_HEADER_SIGN_LEN);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* Double Check to avoid basic fault injection */
  if (se_status == SE_SUCCESS)
  {
    /* Initialize the verification process */
    rv = C_VerifyInit(VerifySession, &cktest_ecdsa_256_mechanism, hECDSAKey);
    if (rv != CKR_OK)
    {
      se_status = SE_ERROR;
      *peSE_Status = SE_KO;
    }

    if (se_status == SE_SUCCESS)
    {
      /* End-up verification process : control if the signature is correct */
      rv = C_Verify(VerifySession, (CK_BYTE_PTR)pPayload, payloadSize, (CK_BYTE_PTR)pSignature, SE_HEADER_SIGN_LEN);
      if (rv != CKR_OK)
      {
        se_status = SE_ERROR;
        *peSE_Status = SE_KO;
      }
    }
  }

#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_CRYPTO_SCHEME */

  /* Close the session */
  rv = C_CloseSession(VerifySession);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;

}