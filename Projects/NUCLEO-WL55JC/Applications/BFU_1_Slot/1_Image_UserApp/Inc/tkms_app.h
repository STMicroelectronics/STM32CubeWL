/**
  ******************************************************************************
  * @file    tkms_app.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for tkms application examples.
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

#ifdef __cplusplus
}
#endif

#endif /* KMS_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
