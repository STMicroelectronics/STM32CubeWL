/**
  ******************************************************************************
  * @file    tkms_app.c
  * @author  MCD Application Team
  * @brief   tKMS application examples module.
  *          This file provides examples of KMS API usages.
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


/* Private variables ---------------------------------------------------------*/
extern void KMS_UPDATE_StartLocalUpdate(void);  /* Function to perform Ymodem download of blob image to import */

static void tkms_app_print_menu(void)
{
  (void)printf("\r\n======================= tKMS Examples Menu ===========================\r\n\n");
  (void)printf("  (*) Requires execution of 'Import Blob' test (3) prior to execute this one\r\n\n");
  (void)printf("  TKMS - Test All (*)                               --------------------- 0\r\n\n");
  (void)printf("  TKMS - Tests AES-GCM Embedded key                 --------------------- 1\r\n\n");
  (void)printf("  TKMS - Tests AES-CBC Embedded key                 --------------------- 2\r\n\n");
  (void)printf("  TKMS - Import blob                                --------------------- 3\r\n\n");
  (void)printf("  TKMS - Tests RSA Static key (*)                   --------------------- 4\r\n\n");
  (void)printf("  TKMS - Tests Derive Static key (*)                --------------------- 5\r\n\n");
  (void)printf("  TKMS - Tests key finding                          --------------------- 6\r\n\n");
  (void)printf("  TKMS - Tests static secure counter                --------------------- 7\r\n\n");
  (void)printf("  TKMS - Tests dynamic secure counters              --------------------- 8\r\n\n");
  (void)printf("  Exit tKMS Examples Menu                           --------------------- x\r\n\n");
}

void tkms_app_menu(void)
{
  uint8_t key;
  uint8_t exit = 0U;
  CK_RV ret_status;
  uint8_t tests_executed; /* Tests executed count */
  uint8_t tests_success;  /* Tests success count */
  uint8_t clear_buffer[128] = "STM32 Key Management Services - Example buffer"; /* Clear text test buffer */

  /* Initialize tKMS for subsequent usage */
  ret_status = C_Initialize(NULL);
  if (ret_status != CKR_OK)
  {
    (void)printf("tKMS initialization failed\r\n");
    exit = 1;
  }
  else
  {
    tkms_app_print_menu();
  }

  while (exit == 0U)  /* Till user request to exit this menu, loop on it */
  {
    key = 0U;

    INVOKE_SCHEDULE_NEEDS();

    /* Clean the user input path */
    (void)COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)  /* User pressed key */
      {
        case '0' :
          /*
           * Execute the whole supported tests in sequence
           * For each test:
           * - call INVOKE_SCHEDULE_NEEDS to rewarm WDG
           * - check results and report test status
           * At the end, global result is provided
           */
          tests_executed = 0U;
          tests_success = 0U;
          ret_status = tkms_app_encrypt_decrypt_aes_gcm(sizeof(clear_buffer), clear_buffer);
          (void)printf("AES GCM test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_encrypt_decrypt_aes_cbc(sizeof(clear_buffer), clear_buffer);
          (void)printf("AES CBC test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_sign_verify_rsa(sizeof(clear_buffer), clear_buffer);
          (void)printf("RSA test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_derive_key(sizeof(clear_buffer), clear_buffer);
          (void)printf("Derive key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_find();
          (void)printf("Find key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_static_secure_counter();
          (void)printf("Test static secure counter %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_dynamic_secure_counters();
          (void)printf("Test dynamic secure counters %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1U : 0U;
          INVOKE_SCHEDULE_NEEDS();
          /* Report final status */
          if (tests_success == tests_executed)
          {
            (void)printf("CUMULATIVE RESULT: ALL TESTS PASSED\r\n");
          }
          else
          {
            (void)printf("CUMULATIVE RESULT: %d/%d success\r\n", tests_success, tests_executed);
          }
          tkms_app_print_menu();
          break;

        /* 1 = TKMS - Tests AES-GCM Embedded key */
        case '1' :
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_encrypt_decrypt_aes_gcm(sizeof(clear_buffer), clear_buffer);
          (void)printf("AES GCM test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 2 = Tests AES-CBC Embedded key */
        case '2' :
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_encrypt_decrypt_aes_cbc(sizeof(clear_buffer), clear_buffer);
          (void)printf("AES CBC test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 3 = Import Blob */
        case '3':
          INVOKE_SCHEDULE_NEEDS();
          KMS_UPDATE_StartLocalUpdate();
          tkms_app_print_menu();
          break;
        /* 4 = Tests RSA Static key (requires blob import) */
        case '4' :
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_sign_verify_rsa(sizeof(clear_buffer), clear_buffer);
          (void)printf("RSA test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 5 = Tests Key derivation (requires blob import) */
        case '5':
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_derive_key(sizeof(clear_buffer), clear_buffer);
          (void)printf("Derive key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 6 = Tests Key finding */
        case '6':
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_find();
          (void)printf("Find key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 7 = Tests static secure counter */
        case '7':
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_static_secure_counter();
          (void)printf("Test static secure counter %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 8 = Tests dynamic secure counters */
        case '8':
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_dynamic_secure_counters();
          (void)printf("Test dynamic secure counters %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFUL" : "FAILED");
          tkms_app_print_menu();
          break;
        case 'x':
          exit = 1;
          break;

        default:
          (void)printf("Invalid Number !\r");
          tkms_app_print_menu();
          break;
      }
    }
  }

  /* Finalize tKMS since no more in use */
  ret_status = C_Finalize(NULL);
  if (ret_status != CKR_OK)
  {
    (void)printf("tKMS finalization failed\r\n");
  }
}

/*
 * Utilities to format buffer in string for display in traces
 */
void tkms_buff2str(uint8_t *pbuff, uint8_t *pstr, uint32_t length)
{
  uint8_t ascii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  uint8_t *ptmp = pstr;
  uint8_t *ptmp2 = pbuff;
  for (uint32_t i = 0UL; i < length; i += 1UL)
  {
    *ptmp = ascii[*ptmp2 >> 4];
    ptmp++;
    *ptmp = ascii[*ptmp2 & 0x0FUL];
    ptmp++;
    ptmp2++;
  }
  *ptmp = 0U;
}
