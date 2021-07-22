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
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
    count = 1;
    rv = C_FindObjects(session, &object_handle, 1, &count);
  }

  /* Find Objects Final: Finalize the operation */
  if (rv == CKR_OK)
  {
    rv = C_FindObjectsFinal(session);
  }

  if (rv == CKR_OK)
  {
    (void)printf("--- Object Found ------------------------------------------------\r\n");
    (void)printf("--- Object Handle [%d]\r\n", object_handle);
  }
  else
  {
    printf("--- Object Not Found --------------------------------------------\r\n");
  }
  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
