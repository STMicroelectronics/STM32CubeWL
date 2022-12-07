/**
  ******************************************************************************
  * @file    tkms_app_derive_key.c
  * @author  MCD Application Team
  * @brief   tKMS application examples module.
  *          This file provides examples of KMS API usage to derivate a key
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

/* Private defines -----------------------------------------------------------*/
#define EC_POINT_MAX_LEN          (3U + (2U * 32U))   /* EC Point Max Length : X962 header + DER header +
                                                         NIST-P256 *2 - > (0x04|LEN|0x04|X|Y) */
#define DH_SECRET_X_MAX_LEN       (32U)               /* DH Secret X Max Length */

/* Private structures --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_string[1024] = {0};


/**
  * @brief  Key derivation
  *         This example is showing how to use key derivation to create
  *         a specific session key.
  * @note
  *     Key derivation shared information between emitter and receiver
  *       - Secret derivation Key: stored in KMS under
  *                  KMS_KEY_AES128_DERIVABLE_OBJECT_HANDLE object handle
  *       - Session key variation: transmitted or calculated by both emitter & receiver
  * @param  length  Length of the clear text for encryption/decryption tests
  * @param  pClearMessage  Clear text for encryption/decryption tests
  * @retval CK_RV return value.
  */
CK_RV tkms_app_derive_key(uint32_t length, uint8_t *pClearMessage)
{
  uint8_t encrypted_message[256] __attribute__((aligned(8))) = {0};
  uint8_t decrypted_message[256] __attribute__((aligned(8))) = {0};
  /* Generated key template definition */
  CK_ULONG DeriveKey_template_class = CKO_SECRET_KEY;
  CK_ULONG DeriveKey_template_destroyable = CK_TRUE;
  CK_ULONG DeriveKey_template_encrypt = CK_TRUE;
  CK_ULONG DeriveKey_template_decrypt = CK_TRUE;
  CK_ULONG DeriveKey_template_extract = CK_TRUE;
  CK_ATTRIBUTE DeriveKey_template[] =
  {
    {CKA_CLASS, (CK_VOID_PTR) &DeriveKey_template_class,       sizeof(CK_ULONG)},
    {CKA_DESTROYABLE, (CK_VOID_PTR) &DeriveKey_template_destroyable, sizeof(CK_ULONG)},
    {CKA_ENCRYPT, (CK_VOID_PTR) &DeriveKey_template_encrypt,     sizeof(CK_ULONG)},
    {CKA_DECRYPT, (CK_VOID_PTR) &DeriveKey_template_decrypt,     sizeof(CK_ULONG)},
    {CKA_EXTRACTABLE, (CK_VOID_PTR) &DeriveKey_template_extract,     sizeof(CK_ULONG)}
  };
  /* Template to retrieve generate key value */
  CK_ATTRIBUTE GetKey_template[] = {{CKA_VALUE, (CK_VOID_PTR) &DeriveKey_template_class, 32UL}};
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  /* Key derivation */
  uint8_t session_variant[32] = "My session variation 0122004578";
  CK_MECHANISM      mech = {CKM_AES_ECB_ENCRYPT_DATA, session_variant, 32};
  CK_OBJECT_HANDLE  derivedKeyHdle = 0UL;
  uint8_t derivedKey[32] = {0};
  /* AES encryption / decryption */
  uint32_t encrypted_length = 0UL;
  uint32_t decrypted_length = 0UL;
  uint8_t cbc_iv[16] = ">CBC VECTOR    ";
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)cbc_iv, sizeof(cbc_iv) };
  uint8_t tag[16] = {0};
  uint32_t tag_lenth;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Derive key with pass phrase */
  if (rv == CKR_OK)
  {
    rv = C_DeriveKey(session, &(mech), (CK_OBJECT_HANDLE)KMS_KEY_AES128_DERIVABLE_OBJECT_HANDLE,
                     &DeriveKey_template[0], sizeof(DeriveKey_template) / sizeof(CK_ATTRIBUTE), &derivedKeyHdle);
  }

  /* Get derived key to display */
  if (rv == CKR_OK)
  {
    GetKey_template[0].pValue = derivedKey;
    rv = C_GetAttributeValue(session, derivedKeyHdle, &(GetKey_template[0]), sizeof(GetKey_template) /
                             sizeof(CK_ATTRIBUTE));
  }
  if (rv == CKR_OK)
  {
    (void)printf("--- Derivating key --------------------------------------------\r\n");
    (void)printf("--- AES ECB\r\n");
    (void)printf("--- Pass phrase   [%s]\r\n", session_variant);
    tkms_buff2str(derivedKey, a_string, 32);
    (void)printf("--- Derived key   [%s]\r\n", a_string);
  }

  /* Encryption --------------------------------------------------------------*/
  (void)printf("--- Encrypting --------------------------------------------------\r\n");
  (void)printf("--- AES CBC\r\n");
  (void)printf("--- IV      [%s]\r\n", cbc_iv);
  (void)printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  (void)printf("---         [0x%s]\r\n", a_string);
  (void)printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_cbc_mechanism, derivedKeyHdle);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    encrypted_length = sizeof(encrypted_message);
    rv = C_EncryptUpdate(session, pClearMessage, length,
                         encrypted_message, &encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    tag_lenth = sizeof(tag);
    rv = C_EncryptFinal(session, &tag[0], &tag_lenth);
  }

  (void)printf("--- Encrypted ---------------------------------------------------\r\n");
  tkms_buff2str(encrypted_message, a_string, encrypted_length);
  (void)printf("--- Message [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_cbc_mechanism, derivedKeyHdle);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    decrypted_length = sizeof(decrypted_message);
    rv = C_DecryptUpdate(session, encrypted_message, encrypted_length,
                         decrypted_message, &decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    tag_lenth = sizeof(tag);
    rv = C_DecryptFinal(session, &tag[0], &tag_lenth);
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

  /* Remove key once no more needed */
  if (rv == CKR_OK)
  {
    rv = C_DestroyObject(session, derivedKeyHdle);
  }

  /* Verify key is no more usable */
  if (rv == CKR_OK)
  {
    if (C_EncryptInit(session, &aes_cbc_mechanism, derivedKeyHdle) == CKR_OK)
    {
      rv = CKR_FUNCTION_FAILED;
    }
  }

  /* Close sessions */
  (void)C_CloseSession(session);

  return rv;
}
