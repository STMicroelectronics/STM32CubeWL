/**
  ******************************************************************************
  * @file    tkms_app_encrypt_decrypt.c
  * @author  MCD Application Team
  * @brief   KMS application examples module.
  *          This file provides examples of KMS API usage to encrypt & decrypt
  *          messages.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tkms.h"
#include "tkms_app.h"
#include "com.h"
#include "common.h"
#include "kms_platf_objects_interface.h"

/* Private defines ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_string[1024] = {0};
static uint8_t encrypted_message[256] __attribute__((aligned(8))) = {0};
static uint8_t decrypted_message[256] __attribute__((aligned(8))) = {0};

/**
  * @brief  AES GCM 128 Bits
  *         This example is showing how to use AES GCM encryption/decryption to
  *         authenticate and protect a message.
  * @note
  *     Shared information between emitter and receiver
  *       - Secret encryption/decryption Key: stored in KMS under
  *                        KMS_KEY_AES128_OBJECT_HANDLE object handle
  *       - Encrypted message: transmitted from emitter to sender
  *       - Initialization Vector: transmitted from emitter to sender
  *       - Message Header: transmitted from emitter to sender
  *       - Authentication Tag: transmitted from emitter to sender
  *                      (calculated during encryption, verified during decryption)
  * @param  length  Length of the clear text for encryption/decryption tests
  * @param  pClearMessage  Clear text for encryption/decryption tests
  * @retval CK_RV return value.
  */
CK_RV tkms_app_encrypt_decrypt_aes_gcm(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t encrypted_length = 0UL;
  uint32_t decrypted_length = 0UL;
  uint8_t tag[16] = {0};  /* 128 bits tag size */
  uint32_t tag_lenth = 0UL;

  /* AES GCM Configuration variables */
  uint8_t gcm_iv[12] = ">GCM VECTOR";
  uint8_t gcm_header[16] = ">AES GCM Header";
  /* Prepare GCM mechanism */
  CK_GCM_PARAMS gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    sizeof(gcm_iv),
    (CK_BYTE_PTR)gcm_header,
    sizeof(gcm_header),
    128   /* Tag length in Bits */
  };
  CK_MECHANISM aes_gcm_mechanism = { CKM_AES_GCM, (CK_VOID_PTR *)(uint32_t) &gcm_params, sizeof(gcm_params) };

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Encryption --------------------------------------------------------------*/
  (void)printf("--- Encrypting --------------------------------------------------\r\n");
  (void)printf("--- AES GCM\r\n");
  (void)printf("--- IV      [%s]\r\n", gcm_iv);
  (void)printf("--- Header  [%s]\r\n", gcm_header);
  (void)printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  (void)printf("---         [0x%s]\r\n", a_string);
  (void)printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_gcm_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES128_OBJECT_HANDLE);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    encrypted_length = sizeof(encrypted_message);
    rv = C_EncryptUpdate(session, pClearMessage, length,
                         encrypted_message, &encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    tag_lenth = sizeof(tag);
    rv = C_EncryptFinal(session, &tag[0], &tag_lenth);
  }

  (void)printf("--- Encrypted ---------------------------------------------------\r\n");
  tkms_buff2str(encrypted_message, a_string, encrypted_length);
  (void)printf("--- Message [0x%s]\r\n", a_string);
  tkms_buff2str(tag, a_string, tag_lenth);
  (void)printf("--- Tag     [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_gcm_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES128_OBJECT_HANDLE);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    decrypted_length = sizeof(decrypted_message);
    rv = C_DecryptUpdate(session, encrypted_message, encrypted_length,
                         decrypted_message, &decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    tag_lenth = sizeof(tag);
    rv = C_DecryptFinal(session, &tag[0], &tag_lenth);
  }
  (void)printf("--- Decrypted ---------------------------------------------------\r\n");
  (void)printf("--- Message [%s]\r\n", decrypted_message);
  tkms_buff2str(decrypted_message, a_string, decrypted_length);
  (void)printf("---         [0x%s]\r\n", a_string);

  if (rv == CKR_OK)
  {
    (void)printf(">>> Decrypted message authenticated\r\n");
  }
  else
  {
    (void)printf("XXX Decrypted message not authenticated\r\n");
  }

  /* Compare decrypted message with clear one */
  if (rv == CKR_OK)     /* Tag verification done into C_DecryptFinal, thus tag check reflected into rv */
  {
    if (memcmp(pClearMessage, decrypted_message, length) != 0)
    {
      (void)printf("XXX Decrypted message differs\r\n");
      rv = CKR_FUNCTION_FAILED;
    }
    else
    {
      (void)printf(">>> Decrypted message is same\r\n");
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/**
  * @brief  AES CBC 256 Bits
  *         This example is showing how to use AES CBC encryption/decryption to
  *         protect a message.
  * @note
  *     CBC shared information between emitter and receiver
  *       - Secret encryption/decryption Key: stored in KMS under
  *                             KMS_KEY_AESC256_OBJECT_HANDLE object handle
  *       - Encrypted message: transmitted from emitter to sender
  *       - Initialization Vector: transmitted from emitter to sender
  * @param  length  Length of the clear text for encryption/decryption tests
  * @param  pClearMessage  Clear text for encryption/decryption tests
  * @retval CK_RV return value.
  */
CK_RV tkms_app_encrypt_decrypt_aes_cbc(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t encrypted_length = 0UL;
  uint32_t decrypted_length = 0UL;
  uint8_t tag[16] = {0};
  uint32_t tag_lenth;

  /* AES CBC Configuration variables */
  uint8_t cbc_iv[16] = ">CBC VECTOR    ";
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)(uint32_t)cbc_iv, sizeof(cbc_iv) };

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Encryption --------------------------------------------------------------*/
  (void)printf("--- Encrypting --------------------------------------------------\r\n");
  (void)printf("--- AES CBC\r\n");
  (void)printf("--- IV      [%s]\r\n", cbc_iv);
  (void)printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  (void)printf("---         [0x%s]\r\n", a_string);
  (void)printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    encrypted_length = sizeof(encrypted_message);
    rv = C_EncryptUpdate(session, pClearMessage, length,
                         encrypted_message, &encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    tag_lenth = sizeof(tag);
    rv = C_EncryptFinal(session, &tag[0], &tag_lenth);
  }

  (void)printf("--- Encrypted ---------------------------------------------------\r\n");
  tkms_buff2str(encrypted_message, a_string, encrypted_length);
  (void)printf("--- Message [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    decrypted_length = sizeof(decrypted_message);
    rv = C_DecryptUpdate(session, encrypted_message, encrypted_length,
                         decrypted_message, &decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    /* Set given buffer length */
    tag_lenth = sizeof(tag);
    rv = C_DecryptFinal(session, &tag[0], &tag_lenth); /* In case of AES CBC, no tag comparison is done */
  }
  (void)printf("--- Decrypted ---------------------------------------------------\r\n");
  (void)printf("--- Message [%s]\r\n", decrypted_message);
  tkms_buff2str(decrypted_message, a_string, decrypted_length);
  (void)printf("---         [0x%s]\r\n", a_string);

  /* Compare decrypted message with clear one */
  if (rv == CKR_OK)
  {
    if (memcmp(pClearMessage, decrypted_message, length) != 0)
    {
      (void)printf("XXX Decrypted message differs\r\n");
      rv = CKR_FUNCTION_FAILED;
    }
    else
    {
      (void)printf(">>> Decrypted message is same\r\n");
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}
