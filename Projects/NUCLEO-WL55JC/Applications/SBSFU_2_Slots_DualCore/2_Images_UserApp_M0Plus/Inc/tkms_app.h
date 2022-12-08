/**
  ******************************************************************************
  * @file    tkms_app.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for tkms application examples.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_APP_H
#define KMS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_sfu.h"

/* Exported macros -----------------------------------------------------------*/
#if  defined(SFU_IWDG_PROTECT_ENABLE)
/* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload register */
#define INVOKE_SCHEDULE_NEEDS()            \
  do {                                     \
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);  \
  } while(0);
#else
#define INVOKE_SCHEDULE_NEEDS()            \
  do {                                     \
    ;                                      \
  } while(0);
#endif /* SFU_IWDG_PROTECT_ENABLE */



/* Exported functions ------------------------------------------------------- */
void tkms_app_menu(void);
void tkms_buff2str(uint8_t *pbuff, uint8_t *pstr, uint32_t length);

CK_RV tkms_app_encrypt_decrypt_aes_gcm(uint32_t length, uint8_t *pClearMessage);
CK_RV tkms_app_encrypt_decrypt_aes_cbc(uint32_t length, uint8_t *pClearMessage);
CK_RV tkms_app_sign_verify_rsa(uint32_t length, uint8_t *pMessage);
CK_RV tkms_app_derive_key(uint32_t length, uint8_t *pMessage);
CK_RV tkms_app_find(void);
CK_RV tkms_app_static_secure_counter(void);
CK_RV tkms_app_dynamic_secure_counters(void);

#ifdef __cplusplus
}
#endif

#endif /* KMS_APP_H */
