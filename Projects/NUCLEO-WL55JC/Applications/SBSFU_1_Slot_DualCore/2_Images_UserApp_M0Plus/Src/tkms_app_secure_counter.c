/**
  ******************************************************************************
  * @file    tkms_app_secure_counter.c
  * @author  MCD Application Team
  * @brief   KMS application examples module.
  *          This file provides examples of KMS API usage to use
  *          secure counters.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private defines -----------------------------------------------------------*/
#define STATIC_COUNTER_LABEL "SECURECOUNTER-1"
#define LABEL_SIZE 16
#define KMS_OBJECT_MAX_NUMBER 25
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static CK_RV tkms_app_test_secure_counter(CK_OBJECT_HANDLE counter_handle);
static CK_RV tkms_app_find_counter(CK_VOID_PTR plabel,
                                   CK_ULONG label_length,
                                   CK_OBJECT_HANDLE *phandle,
                                   CK_ULONG *pcount);
static CK_RV tkms_app_get_attribute_value(CK_SESSION_HANDLE session,
                                          CK_OBJECT_HANDLE counter_handle,
                                          CK_ATTRIBUTE_TYPE attribute_type,
                                          CK_ULONG *pvalue);
CK_ULONG tkms_app_get_expected_value(CK_ULONG current_value,
                                     CK_ULONG increment_value,
                                     CK_ULONG reset_value,
                                     CK_ULONG max_value,
                                     CK_ULONG saturation);

/**
  * @brief  Static secure counter
  *         This example is showing how to use a static secure counter.
  * @param  None
  * @retval CK_RV return value.
  */
CK_RV tkms_app_static_secure_counter(void)
{
  CK_RV rv;
  CK_OBJECT_HANDLE counter_handle;
  CK_ULONG count;

  printf("\r\n-------------------- Static Secure Counter ---------------------\r\n\n");

  rv = tkms_app_find_counter(STATIC_COUNTER_LABEL, sizeof(STATIC_COUNTER_LABEL), &counter_handle, &count);
  if ((rv == CKR_OK) && (count == 0UL))
  {
    printf("--- No static secure counter was found\r\n");
    return CKR_GENERAL_ERROR;
  }
  else if (rv == CKR_OK)
  {
    /* Test of the static secure counter */
    rv = tkms_app_test_secure_counter(counter_handle);
  }
  else
  {
    printf("--- Unexpected error\r\n");
  }

  return rv;
}

/**
  * @brief  Dynamic secure counters
  *         This example is showing how to use dynamic secure counters.
  * @param  None
  * @retval CK_RV return value.
  */
CK_RV tkms_app_dynamic_secure_counters(void)
{
  CK_RV rv;
  CK_ULONG count;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_OBJECT_HANDLE dynamic_secure_counter_1;
  CK_OBJECT_HANDLE dynamic_secure_counter_2;
  /* Generated secure counter template definition */
  CK_ULONG Counter_template_class             = CKO_STM_SECURE_COUNTER;
  CK_ULONG Counter_template_label[LABEL_SIZE] = {0x55434553U, 0x4F434552U, 0x45544E55U, 0x00322D52U};
  CK_ULONG Counter_template_destroyable       = CK_FALSE;
  CK_ULONG Counter_template_saturated         = CK_TRUE;
  CK_ULONG Counter_template_max               = 0xFFFFFFFFU;
  CK_ULONG Counter_template_reset             = 0x00000001U;
  CK_ULONG Counter_template_value_1           = 0x00000001U;
  CK_ULONG Counter_template_value_2           = 0x000000A0U;
  CK_ULONG Counter_template_increment         = 0x00000005U;
  CK_ATTRIBUTE SecureCounter_template_1[] =
  {
    {CKA_CLASS,                   (CK_VOID_PTR) &Counter_template_class,       sizeof(CK_OBJECT_CLASS)},
    {CKA_LABEL,                   (CK_VOID_PTR) &Counter_template_label,       LABEL_SIZE},
    {CKA_DESTROYABLE,             (CK_VOID_PTR) &Counter_template_destroyable, sizeof(CK_ULONG)},
    {CKA_STM_COUNTER_SATURATED,   (CK_VOID_PTR) &Counter_template_saturated,   sizeof(CK_ULONG)},
    {CKA_STM_COUNTER_MAX_VALUE,   (CK_VOID_PTR) &Counter_template_max,         sizeof(CK_ULONG)},
    {CKA_STM_COUNTER_RESET_VALUE, (CK_VOID_PTR) &Counter_template_reset,       sizeof(CK_ULONG)},
    {CKA_STM_COUNTER_VALUE,       (CK_VOID_PTR) &Counter_template_value_1,     sizeof(CK_ULONG)},
    {CKA_STM_COUNTER_INCREMENT,   (CK_VOID_PTR) &Counter_template_increment,   sizeof(CK_ULONG)},
  };
  CK_ULONG SecureCounter_1_nb_arg         = 8;
  CK_ATTRIBUTE SecureCounter_template_2[] =
  {
    {CKA_CLASS,                   (CK_VOID_PTR) &Counter_template_class,       sizeof(CK_OBJECT_CLASS)},
    {CKA_STM_COUNTER_VALUE,       (CK_VOID_PTR) &Counter_template_value_2,     sizeof(CK_ULONG)},
  };
  CK_ULONG SecureCounter_2_nb_arg         = 2;

  printf("\r\n------------------ Dynamic Secure Counter 1 ------------------\r\n\n");

  /* Check whether the undestroyable dynamic object already exists */
  rv = tkms_app_find_counter(Counter_template_label, LABEL_SIZE, &dynamic_secure_counter_1, &count);

  if ((rv == CKR_OK) && (count != 0UL))
  {
    (void)printf("--- The undestroyable counter already exists with the handle [%d]\r\n", dynamic_secure_counter_1);
  }
  else if (rv == CKR_OK)
  {
    /* Open session with KMS */
    rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
    if (rv != CKR_OK)
    {
      printf("--- C_OpenSession: error [0x%x]\r\n", rv);
      return rv;
    }

    /* Create the first secure counter */
    if (rv == CKR_OK)
    {
      rv = C_CreateObject(session, SecureCounter_template_1, SecureCounter_1_nb_arg, &dynamic_secure_counter_1);
    }

    /* Close session with KMS */
    (void)C_CloseSession(session);

    if (rv == CKR_OK)
    {
      (void)printf("--- The undestroyable counter was created with the handle [%d]\r\n", dynamic_secure_counter_1);
    }
    else
    {
      printf("--- C_CreateObject: error [0x%x]\r\n", rv);
    }
  }

  if (rv == CKR_OK)
  {
    /* Test of the first dynamic secure counter */
    rv = tkms_app_test_secure_counter(dynamic_secure_counter_1);
  }

  if (rv != CKR_OK)
  {
    return rv;
  }

  printf("\r\n------------------ Dynamic Secure Counter 2 ------------------\r\n\n");

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    printf("--- C_OpenSession: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Create the second secure counter */
  if (rv == CKR_OK)
  {
    rv = C_CreateObject(session, SecureCounter_template_2, SecureCounter_2_nb_arg, &dynamic_secure_counter_2);
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);
  if (rv != CKR_OK)
  {
    printf("--- C_CreateObject: error [0x%x]\r\n", rv);
  }

  /* Test of the second dynamic secure counter */
  rv = tkms_app_test_secure_counter(dynamic_secure_counter_2);

  printf("\r\n------------------ Remove dynamic objects ------------------\r\n\n");

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    printf("--- C_OpenSession: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Remove the first secure counter */
  if (rv == CKR_OK)
  {
    rv = C_DestroyObject(session, dynamic_secure_counter_1);
  }

  if (rv == CKR_ACTION_PROHIBITED)
  {
    printf("--- C_DestroyObject: action forbidden as expected [0x%x]\r\n", rv);
    rv = CKR_OK;
  }
  else if (rv == CKR_OK)
  {
    printf("--- C_DestroyObject: it should have been forbidden [0x%x]\r\n", rv);
    rv = CKR_GENERAL_ERROR;
  }
  else
  {
    printf("--- C_DestroyObject: error [0x%x]\r\n", rv);
  }

  /* Try to remove the second secure counter */
  if (rv == CKR_OK)
  {
    rv = C_DestroyObject(session, dynamic_secure_counter_2);
  }

  if (rv == CKR_OK)
  {
    printf("--- C_DestroyObject: action authorized as expected [0x%x]\r\n", rv);
    rv = CKR_OK;
  }
  else
  {
    printf("--- C_DestroyObject: error [0x%x]\r\n", rv);
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/**
  * @brief  Test of a Secure Counter
  * @param  counter_handle: handle of the secure counter object.
  * @retval CK_RV return value.
  */
static CK_RV tkms_app_test_secure_counter(CK_OBJECT_HANDLE counter_handle)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_ULONG current_counter = 0UL;
  CK_ULONG updated_counter = 0UL;
  CK_ULONG expected_counter = 0UL;
  CK_ULONG increment_value = 0UL;
  CK_ULONG reset_value = 0UL;
  CK_ULONG max_value = 0UL;
  CK_ULONG saturation = 0UL;
  CK_ULONG counter;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    printf("--- C_OpenSession: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Read the increment value */
  rv = tkms_app_get_attribute_value(session, counter_handle, CKA_STM_COUNTER_INCREMENT, &increment_value);
  if (rv != CKR_OK)
  {
    printf("--- C_GetAttributeValue[CKA_STM_COUNTER_INCREMENT]: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Read the reset value */
  rv = tkms_app_get_attribute_value(session, counter_handle, CKA_STM_COUNTER_RESET_VALUE, &reset_value);
  if (rv != CKR_OK)
  {
    printf("--- C_GetAttributeValue[CKA_STM_COUNTER_RESET_VALUE]: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Read the max value */
  rv = tkms_app_get_attribute_value(session, counter_handle, CKA_STM_COUNTER_MAX_VALUE, &max_value);
  if (rv != CKR_OK)
  {
    printf("--- C_GetAttributeValue[CKA_STM_COUNTER_MAX_VALUE]: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Read the saturation configuration */
  rv = tkms_app_get_attribute_value(session, counter_handle, CKA_STM_COUNTER_SATURATED, &saturation);
  if (rv != CKR_OK)
  {
    printf("--- C_GetAttributeValue[CKA_STM_COUNTER_SATURATED]: error [0x%x]\r\n", rv);
    return rv;
  }

  /* Read the current value of the secure counter */
  rv = C_STM_CounterGetValue(session, counter_handle, &current_counter);
  if (rv != CKR_OK)
  {
    printf("--- C_STM_CounterGetValue: error [0x%x]\r\n", rv);
    return rv;
  }
  else
  {
    printf("--- Initial value   [0x%x]\r\n", current_counter);
  }
  /* Updates of the counter */
  for (counter = 0; counter < 5; counter++)
  {
    /* Compute the value expected for the counter */
    expected_counter = tkms_app_get_expected_value(current_counter,
                                                   increment_value,
                                                   reset_value,
                                                   max_value,
                                                   saturation);
    /* Test C_STM_CounterIncrement with and without result pointer */
    if (counter % 2 == 0)
    {
      rv = C_STM_CounterIncrement(session, counter_handle, NULL);
      updated_counter = expected_counter;
    }
    else
    {
      rv = C_STM_CounterIncrement(session, counter_handle, &updated_counter);
    }
    /* Check the output of C_STM_CounterIncrement with and without result pointer */
    if ((rv == CKR_OK) && (expected_counter != updated_counter))
    {
      printf("--- The counter value returned by C_STM_CounterIncrement is wrong\r\n");
      return CKR_GENERAL_ERROR;
    }
    else if (rv != CKR_OK)
    {
      printf("--- C_STM_CounterIncrement: error [0x%x]\r\n", rv);
      return rv;
    }
    /* Test C_STM_CounterGetValue */
    rv = C_STM_CounterGetValue(session, counter_handle, &current_counter);
    if (rv != CKR_OK)
    {
      printf("--- C_STM_CounterGetValue: error [0x%x]\r\n", rv);
      return rv;
    }
    /* Check the output of C_STM_CounterGetValue */
    printf("--- Updated value %d [0x%x]\r\n", counter, current_counter);
    if(expected_counter != current_counter)
    {
      printf("--- The counter wasn't updated\r\n");
      return CKR_GENERAL_ERROR;
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/**
  * @brief  Search the secure counter matching the provided label
  * @retval phandle Handle of the static secure counter
  * @retval CK_RV   Return value.
  */
static CK_RV tkms_app_find_counter(CK_VOID_PTR plabel,
                                   CK_ULONG label_length,
                                   CK_OBJECT_HANDLE *phandle,
                                   CK_ULONG *pcount)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_ATTRIBUTE object_template;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Fill in the template with the attribute to retrieve the specific key
  (CKA_LABEL is used to refer to a specific object) */
  object_template.type = CKA_LABEL;
  object_template.pValue = plabel;
  object_template.ulValueLen = label_length;

  /* Find Objects Init : template of the object to retrieve is passed to KMS */
  if (rv == CKR_OK)
  {
    rv = C_FindObjectsInit(session, (CK_ATTRIBUTE_PTR)&object_template, 1);
  }

  /* Find Objects : Find object corresponding to the template specified in FindObjectsInit */
  if (rv == CKR_OK)
  {
    rv = C_FindObjects(session, phandle, 1, pcount);
  }

  /* Find Objects Final: Finalize the operation */
  if (rv == CKR_OK)
  {
    rv = C_FindObjectsFinal(session);
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/**
  * @brief  Get the increment value
  * @param  counter_handle: handle of the secure counter object.
  * @retval pincrement Increment value
  * @retval CK_RV   Return value.
  */
static CK_RV tkms_app_get_attribute_value(CK_SESSION_HANDLE session,
                                          CK_OBJECT_HANDLE counter_handle,
                                          CK_ATTRIBUTE_TYPE attribute_type,
                                          CK_ULONG *pvalue)
{
  CK_RV rv;
  CK_ATTRIBUTE object_template;

  /* Fill in the template with the attribute to retrieve from the counter */
  object_template.type = attribute_type;
  object_template.pValue = pvalue;
  object_template.ulValueLen = sizeof(CK_ULONG);

  rv =  C_GetAttributeValue(session, counter_handle, (CK_ATTRIBUTE_PTR)&object_template, 1);

  if (rv == CKR_ATTRIBUTE_TYPE_INVALID)
  {
    if (attribute_type == CKA_STM_COUNTER_INCREMENT)
    {
      *pvalue = 1UL;
      rv = CKR_OK;
    }
    else if (attribute_type == CKA_STM_COUNTER_RESET_VALUE)
    {
      *pvalue = 0UL;
      rv = CKR_OK;
    }
    else if (attribute_type == CKA_STM_COUNTER_MAX_VALUE)
    {
      *pvalue = 0xFFFFFFFFUL;
      rv = CKR_OK;
    }
    else if (attribute_type == CKA_STM_COUNTER_SATURATED)
    {
      *pvalue = 0UL;
      rv = CKR_OK;
    }
  }

  return rv;
}

/**
  * @brief  Compute the expected counter value
  * @param  current_value   Current value of the counter
  * @param  increment_value Increment value
  * @param  reset_value     Reset value (saturation disabled)
  * @param  saturation      Max value
  * @retval Expected counter value
  */
CK_ULONG tkms_app_get_expected_value(CK_ULONG current_value,
                                     CK_ULONG increment_value,
                                     CK_ULONG reset_value,
                                     CK_ULONG max_value,
                                     CK_ULONG saturation)
{
  CK_ULONG expected_value;

  if (current_value <= max_value - increment_value)
  {
    expected_value = current_value + increment_value;
  }
  else if (saturation == 0U)
  {
    expected_value = reset_value;
  }
  else
  {
    expected_value = max_value;
  }
  return expected_value;
}

