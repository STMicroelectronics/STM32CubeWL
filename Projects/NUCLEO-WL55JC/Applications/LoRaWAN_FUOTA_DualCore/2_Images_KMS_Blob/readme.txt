/**
  @page 2_Images_KMS_Blob / Key Management Services / Blob generation project.

  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    readme.txt
  * @brief   Key Management Services Blob generation.
  *          Allows to generate Blob to be injected to a device supporting
  *          KMS ImportBlob().
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
  @endverbatim

@par Application Description 

This application is used to generate Key Management Services Blob binary file to be downloaded
with KMS through the ImportBlob() API. 

You can edit your own keys to generate your own Blob.

For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube"
available from the STMicroelectronics microcontroller website www.st.com.

@par Keywords

Cryptography, KMS, Provisioning

@par Directory contents

   - 2_Images_KMS_Blob/Src/kms_blob_example_keys.c             KMS blob objects definition
   - 2_Images_KMS_Blob/Binary/ECCKEY.txt                       Private ECCDSA key for signature verification
   - 2_Images_KMS_Blob/Binary/iv.bin                           Encryption/decryption initialization vector
   - 2_Images_KMS_Blob/Binary/OEM_KEY_COMPANY1_key_AES_CBC.bin Public key for AES CBC encryption

@par Hardware and Software environment

  - This example runs on STM32WL55xx devices
  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.
  - Python installation required. In addition, install the appropriate python modules as described in
    Middlewares\ST\STM32_Secure_Engine\Utilities\KeysAndImages\readme.txt
    
    
@par How to use it ? 

In order to generate Secure Key Storage Blob file, you must do the following :
 - Edit the .c source file, where examples of keys are provided. Update with your own keys.
 - Open your preferred toolchain 
 - Rebuild the project

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
