/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : app_kms.c
  * Description        : This file provides code for the configuration
  *                      of the kms instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_kms.h"
#include "tkms.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
static void buff2str(uint8_t *pbuff, uint8_t *pstr, uint32_t length);
/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */
static uint8_t a_string[1024] = {0};
static uint8_t encrypted_message[256] __attribute__((aligned(8)));
static uint8_t decrypted_message[256] __attribute__((aligned(8)));
/* USER CODE END 2 */

/* KMS init function */
void MX_KMS_Init(void)
{
/***************************************/
   /**
  */

  C_Initialize(NULL);

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  AES CBC 256 Bits
  *         This example is showing how to use AES CBC encryption/decryption to
  *         protect a message.
  * @note
  *     CBC shared information between emitter and receiver
  *       - Secret encryption/decryption Key: stored in KMS under
  *                             KMS_KEY_AES256_OBJECT_HANDLE object handle
  *       - Encrypted message: transmitted from emitter to sender
  *       - Initialization Vector: transmitted from emitter to sender
  * @param  length  Length of the clear text for encryption/decryption tests
  * @param  pClearMessage  Clear text for encryption/decryption tests
  * @retval CK_RV return value.
  */
CK_RV app_kms_encrypt_decrypt_aes_cbc(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t encrypted_length;
  uint32_t decrypted_length;
  uint8_t tag[16] = {0};
  uint32_t tag_lenth;

  /* AES CBC Configuration variables */
  char cbc_iv[16] = ">CBC VECTOR    ";
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)cbc_iv, sizeof(cbc_iv) };

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Encryption --------------------------------------------------------------*/
  printf("--- Encrypting --------------------------------------------------\r\n");
  printf("--- AES CBC\r\n");
  printf("--- IV      [%s]\r\n", cbc_iv);
  printf("--- Message [%s]\r\n", pClearMessage);
  buff2str(pClearMessage, a_string, length);
  printf("---         [0x%s]\r\n", a_string);
  printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    encrypted_length = sizeof(encrypted_message);
    rv = C_EncryptUpdate(session, (CK_BYTE_PTR)pClearMessage, length,
                         encrypted_message, (CK_ULONG_PTR)&encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    tag_lenth = sizeof(tag);
    rv = C_EncryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }

  printf("--- Encrypted ---------------------------------------------------\r\n");
  buff2str(encrypted_message, a_string, encrypted_length);
  printf("--- Message [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    decrypted_length = sizeof(decrypted_message);
    rv = C_DecryptUpdate(session, (CK_BYTE_PTR)encrypted_message, encrypted_length,
                         decrypted_message, (CK_ULONG_PTR)&decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    tag_lenth = sizeof(tag);
    rv = C_DecryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }
  printf("--- Decrypted ---------------------------------------------------\r\n");
  printf("--- Message [%s]\r\n", decrypted_message);
  buff2str(decrypted_message, a_string, decrypted_length);
  printf("---         [0x%s]\r\n", a_string);

  /* Compare decrypted message with clear one */
  if (rv == CKR_OK)
  {
    if (memcmp(pClearMessage, decrypted_message, length))
    {
      printf("XXX Decrypted message differs\r\n");
      rv = CKR_FUNCTION_FAILED;
    }
    else
    {
      printf(">>> Decrypted message is same\r\n");
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/**
  * @brief  Buffer to string translation for printing
  * @param  pbuff  Buffer to translate
  * @param  pstr   String results of the translation
  * @param  length Buffer length in bytes
  * @retval None
  */
static void buff2str(uint8_t *pbuff, uint8_t *pstr, uint32_t length)
{
  uint8_t ascii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  for (uint32_t i = 0; i < length; i += 1)
  {
    *pstr = ascii[*pbuff >> 4];
    pstr++;
    *pstr = ascii[*pbuff & 0x0FUL];
    pstr++;
    pbuff++;
  }
  *pstr = 0;
}
/* USER CODE END 4 */

/**
  * @}
  */

/**
  * @}
  */

