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
#define KMS_ZERO_KEY_OBJECT_HANDLE                    ((CK_OBJECT_HANDLE)1)
#define KMS_APP_KEY_OBJECT_HANDLE                     ((CK_OBJECT_HANDLE)2)
#define KMS_NWK_KEY_OBJECT_HANDLE                     ((CK_OBJECT_HANDLE)3)
#define KMS_DEVJOINEUIADDR_KEY_OBJECT_HANDLE          ((CK_OBJECT_HANDLE)4)
#define KMS_NWK_S_KEY_OBJECT_HANDLE                   ((CK_OBJECT_HANDLE)5)
#define KMS_APP_S_KEY_OBJECT_HANDLE                   ((CK_OBJECT_HANDLE)6)
#define LAST_KMS_KEY_OBJECT_HANDLE                    ((CK_OBJECT_HANDLE)7)
/* USER CODE END KMS_PLATF_OBJECTS_INTERFACE_H */

#ifdef __cplusplus
}
#endif

#endif /* KMS_PLATF_OBJECTS_INTERFACE_H */
