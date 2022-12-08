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
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_PLATF_OBJECTS_INTERFACE_H
#define KMS_PLATF_OBJECTS_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* KMS exported Object handles for Secure Boot */

/* SBSFU Root CA Certificate */
#define KMS_SBSFU_ROOT_CA_CRT_LABEL           "SBSFURootCACertificate"  /*!< SBSFU Root CA Certificate Label */
#define KMS_SBSFU_ROOT_CA_CRT_OBJECT_HANDLE   ((CK_OBJECT_HANDLE)9)  /*!< SBSFU Root CA Certificate object handle */

/* OEM Interm CA Certificate */
#define KMS_SBSFU_OEM_INTM_CA_CRT_1_LABEL           "OEMIntermCACertificate_1"  /*!< OEM 1 Interm CA Certificate */
#define KMS_SBSFU_OEM_INTM_CA_CRT_2_LABEL           "OEMIntermCACertificate_2"  /*!< OEM 2 Interm CA Certificate */
#define KMS_SBSFU_OEM_INTM_CA_CRT_3_LABEL           "OEMIntermCACertificate_3"  /*!< OEM 3 Interm CA Certificate */
#define KMS_SBSFU_OEM_INTM_CA_CRT_1_OBJECT_HANDLE   ((CK_OBJECT_HANDLE)10)  /*!< OEM 1 Interm CA Certificate object handle */
#define KMS_SBSFU_OEM_INTM_CA_CRT_2_OBJECT_HANDLE   ((CK_OBJECT_HANDLE)11)  /*!< OEM 2 Interm CA Certificate object handle */
#define KMS_SBSFU_OEM_INTM_CA_CRT_3_OBJECT_HANDLE   ((CK_OBJECT_HANDLE)12)  /*!< OEM 3 Interm CA Certificate object handle */


#define KMS_SBSFU_KEY_1_AES128_OBJECT_HANDLE            ((CK_OBJECT_HANDLE)3)
#define KMS_SBSFU_KEY_2_AES128_OBJECT_HANDLE            ((CK_OBJECT_HANDLE)4)
#define KMS_SBSFU_KEY_3_AES128_OBJECT_HANDLE            ((CK_OBJECT_HANDLE)5)
#define KMS_SBSFU_KEY_1_ECDSA_OBJECT_HANDLE             ((CK_OBJECT_HANDLE)6)
#define KMS_SBSFU_KEY_2_ECDSA_OBJECT_HANDLE             ((CK_OBJECT_HANDLE)7)
#define KMS_SBSFU_KEY_3_ECDSA_OBJECT_HANDLE             ((CK_OBJECT_HANDLE)8)

/* KMS exported Object handles for tKMS test application */
#define KMS_KEY_RSA2048_OBJECT_HANDLE                 ((CK_OBJECT_HANDLE)21)
#define KMS_KEY_AES128_DERIVABLE_OBJECT_HANDLE        ((CK_OBJECT_HANDLE)22)
#define KMS_COUNTER_OBJECT_HANDLE                     ((CK_OBJECT_HANDLE)23)

/*  Object handles for user application */
#define KMS_SGFX_DATA_KEY_OBJECT_HANDLE            ((CK_OBJECT_HANDLE)13)
#define KMS_SGFX_PAC_OBJECT_HANDLE                 ((CK_OBJECT_HANDLE)14)
#define KMS_SGFX_ID_OBJECT_HANDLE                  ((CK_OBJECT_HANDLE)15)
#define KMS_SGFX_DATA_KEY_PUBLIC_OBJECT_HANDLE     ((CK_OBJECT_HANDLE)16)

#ifdef __cplusplus
}
#endif

#endif /* KMS_PLATF_OBJECTS_INTERFACE_H */
