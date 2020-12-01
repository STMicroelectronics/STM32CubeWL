/**
  ******************************************************************************
  * @file    kms_platf_objects_interface.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management configuration interface
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
#ifndef KMS_PLATF_OBJECTS_INTERFACE_H
#define KMS_PLATF_OBJECTS_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN KMS_PLATF_OBJECTS_INTERFACE_H */
#define KMS_SGFX_DATA_KEY_OBJECT_HANDLE            ((CK_OBJECT_HANDLE)1)
#define KMS_SGFX_PAC_OBJECT_HANDLE                 ((CK_OBJECT_HANDLE)2)
#define KMS_SGFX_ID_OBJECT_HANDLE                  ((CK_OBJECT_HANDLE)3)
#define KMS_SGFX_DATA_KEY_PUBLIC_OBJECT_HANDLE     ((CK_OBJECT_HANDLE)4)
/* USER CODE END KMS_PLATF_OBJECTS_INTERFACE_H */

#ifdef __cplusplus
}
#endif

#endif /* KMS_PLATF_OBJECTS_INTERFACE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

