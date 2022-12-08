/**
  ******************************************************************************
  * @file    kms_mem_pool_def.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          memory pool definition
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

/* Recursive inclusion authorized ---------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * POOL DESCRIPTION
 */

#if  defined(KMS_ENCRYPT_DECRYPT_BLOB)
/*
 * Memory pools definition:
 * 2 pools of 256
 * 1 pool of 512
 * 3 pools of 1K
 * 1 pool of 5K
 */

KMS_MEM_DECLARE_POOL_START()
KMS_MEM_DECLARE_POOL_ENTRY(1, 256)
KMS_MEM_DECLARE_POOL_ENTRY(2, 256)
KMS_MEM_DECLARE_POOL_ENTRY(3, 512)
KMS_MEM_DECLARE_POOL_ENTRY(4, 1024)
KMS_MEM_DECLARE_POOL_ENTRY(5, 1024)
KMS_MEM_DECLARE_POOL_ENTRY(6, 1024)
KMS_MEM_DECLARE_POOL_ENTRY(7, 5 * 1024)
KMS_MEM_DECLARE_POOL_END()
#else /* KMS_ENCRYPT_DECRYPT_BLOB */
/*
 * Memory pools definition:
 * 2 pools of 256
 * 1 pool of 512
 * 2 pools of 1K
 * 1 pool of 5K
 */

KMS_MEM_DECLARE_POOL_START()
KMS_MEM_DECLARE_POOL_ENTRY(1, 256)
KMS_MEM_DECLARE_POOL_ENTRY(2, 256)
KMS_MEM_DECLARE_POOL_ENTRY(3, 512)
KMS_MEM_DECLARE_POOL_ENTRY(4, 1024)
KMS_MEM_DECLARE_POOL_ENTRY(5, 1024)
KMS_MEM_DECLARE_POOL_ENTRY(6, 5 * 1024)
KMS_MEM_DECLARE_POOL_END()
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */


#ifdef __cplusplus
}
#endif
