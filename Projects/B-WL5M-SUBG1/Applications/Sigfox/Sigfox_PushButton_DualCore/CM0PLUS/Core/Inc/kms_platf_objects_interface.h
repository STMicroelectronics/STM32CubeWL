/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    kms_platf_objects_interface.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management configuration interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
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
