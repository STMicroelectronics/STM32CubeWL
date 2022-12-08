/**
  ******************************************************************************
  * @file    tkms_app_find.c
  * @author  MCD Application Team
  * @brief   KMS application examples module.
  *          This file provides examples of KMS API usage to find
  *          objects.
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
#define TEST_KEY_AES128 "USERCRYP128"
#define TEST_KEY_AES256 "USERCRYP256"
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Object search
  *         This example is showing how to find an object.
  * @param  None
  * @retval CK_RV return value.
  */
CK_RV tkms_app_find(void)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_ATTRIBUTE object_template;
  CK_OBJECT_HANDLE object_handle;
  CK_ULONG count;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Fill in the template with the attribute to retrieve the specific key
  (CKA_LABEL is used to refer to a specific object) */
  object_template.type = CKA_LABEL;
  object_template.pValue = TEST_KEY_AES128;
  object_template.ulValueLen = sizeof(TEST_KEY_AES128);

  (void)printf("--- Find Object -----------------------------------------------------\r\n");
  (void)printf("--- Label [%s]\r\n", (uint8_t *)object_template.pValue);

  /* Find Objects Init : template of the object to retrieve is passed to KMS */
  if (rv == CKR_OK)
  {
    rv = C_FindObjectsInit(session, (CK_ATTRIBUTE_PTR)&object_template, 1);
  }

  /* Find Objects : Find object corresponding to the template specified in FindObjectsInit */
  if (rv == CKR_OK)
  {
    rv = C_FindObjects(session, &object_handle, 1, &count);
  }

  /* Find Objects Final: Finalize the operation */
  if (rv == CKR_OK)
  {
    rv = C_FindObjectsFinal(session);
  }

  if ((rv == CKR_OK) && (count == 0UL))
  {
    printf("--- Object Not Found --------------------------------------------\r\n");
  }
  else if (rv == CKR_OK)
  {
    (void)printf("--- Object Found ------------------------------------------------\r\n");
    (void)printf("--- Object Handle [%d]\r\n", object_handle);
  }
  else
  {
    printf("--- Unexpected error --------------------------------------------\r\n");
  }
  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}
