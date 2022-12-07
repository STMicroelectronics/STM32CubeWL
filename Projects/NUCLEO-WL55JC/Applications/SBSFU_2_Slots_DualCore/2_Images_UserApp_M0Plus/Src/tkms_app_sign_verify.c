/**
  ******************************************************************************
  * @file    tkms_app_sign_verify.c
  * @author  MCD Application Team
  * @brief   KMS application examples module.
  *          This file provides examples of KMS API usage to sign & verify
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

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_string[1024] = {0};
static uint8_t signature[2048 / 8] __attribute__((aligned(8))) = {0};


/**
  * @brief  RSA 1024 Bits
  *         This example is showing how to use RSA signature to
  *         authenticate a message.
  * @note
  *     RSA shared information between emitter and receiver
  *       - Secret signature/verification Key: stored in KMS under
  *         KMS_KEY_RSA2048_OBJECT_HANDLE object handle
  *       - Message to authenticate: transmitted from emitter to sender
  *       - Signature: transmitted from emitter to sender
  *             (calculated by emitter, verified by receiver)
  * @param  length  Length of the clear text for signature/verification tests
  * @param  pMessage  Clear text for signature/verification tests
  * @retval CK_RV return value.
  */
CK_RV tkms_app_sign_verify_rsa(uint32_t length, uint8_t *pMessage)
{

  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t signature_length = 0UL;

  /* Prepare RSA mechanism */
  CK_MECHANISM rsa_mechanism = { CKM_SHA256_RSA_PKCS, (CK_VOID_PTR *) NULL, 0};

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Signature --------------------------------------------------------------*/
  (void)printf("--- Signing -----------------------------------------------------\r\n");
  (void)printf("--- RSA 1024 bits\r\n");
  (void)printf("--- Message [%s]\r\n", pMessage);
  tkms_buff2str(pMessage, a_string, length);
  (void)printf("---         [0x%s]\r\n", a_string);
  (void)printf("--- Length  [%d]\r\n", length);

  /* Configure session to sign message using RSA with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_SignInit(session, &rsa_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_RSA2048_OBJECT_HANDLE);
  }

  /* Sign message */
  if (rv == CKR_OK)
  {
    signature_length = sizeof(signature);
    rv = C_Sign(session, pMessage, length,
                signature, &signature_length);
  }

  (void)printf("--- Signed ------------------------------------------------------\r\n");
  tkms_buff2str(signature, a_string, signature_length);
  (void)printf("--- Signature [0x%s]\r\n", a_string);
  (void)printf("--- Length    [%d]\r\n", signature_length);

  /* Verification ------------------------------------------------------------*/

  /* Configure session to verify message using RSA with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_VerifyInit(session, &rsa_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_RSA2048_OBJECT_HANDLE);
  }

  /* Verify message */
  if (rv == CKR_OK)
  {
    rv = C_Verify(session, pMessage, length,
                  signature, signature_length);
  }

  if (rv == CKR_OK)
  {
    (void)printf(">>> Message authenticated\r\n");
  }
  else
  {
    (void)printf("XXX Message not authenticated\r\n");
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

