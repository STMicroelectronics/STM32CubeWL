/**
  @page KMS_Embedded_AES_Keys KMS application

  @verbatim
  ******************************************************************************
  * @file    Applications/KMS/KMS_Embedded_AES_Keys/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the KMS_Embedded_AES_Keys application.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description 

How to use the KMS middleware to encrypt and decrypt message with AES key.

Application configuration:
KMS is configured with embedded keys:
- one AES 128 bits key
- one AES 256 bits key

Application execution:
The application encrypts a clear text message, then decrypts the encrypted message, to finally compares
the original clear text to the decrypted one.
This is done twice, using AES CBC and AES GCM algorithms.

For debug: A terminal connected with the board via VCOM is needed to see the execution log and test status.

Connection needed:
Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.
A virtual COM port will then appear in the HyperTerminal.
  - Hyperterminal configuration:
    - Data Length = 8 bits
    - One Stop Bit
    - Parity: none
    - BaudRate = 115200 baud
    - Flow control: None

NUCLEO-WL55JC RevC board LED3 is also used to report test status:
  - LED3 is turned on at the end of the execution when all completed successfully
  - LED3 is blinking twice per second when an error occurred

@par Keywords

Cryptography, KMS, cipher, decipher, AES, AESCBC, AESGCM

@par Directory contents 

  - KMS/KMS_Embedded_AES_Keys/Inc/stm32wlxx_nucleo_conf.h       BSP configuration file
  - KMS/KMS_Embedded_AES_Keys/Inc/stm32wlxx_hal_conf.h          HAL configuration file
  - KMS/KMS_Embedded_AES_Keys/Inc/stm32wlxx_it.h                Interrupt handlers header file
  - KMS/KMS_Embedded_AES_Keys/Inc/app_kms.h                     Header for app_kms.c module
  - KMS/KMS_Embedded_AES_Keys/Inc/ca_conf.h                     Cryptographic configuration file
  - KMS/KMS_Embedded_AES_Keys/Inc/ca_low_level.h                Header for ca_low_level.c module
  - KMS/KMS_Embedded_AES_Keys/Inc/com.h                         Header for com.c module
  - KMS/KMS_Embedded_AES_Keys/Inc/kms_config.h                  KMS configuration file
  - KMS/KMS_Embedded_AES_Keys/Inc/kms_low_level.h               Header for kms_low_level.c module
  - KMS/KMS_Embedded_AES_Keys/Inc/kms_mem_pool_def.h            KMS memory pool definition file
  - KMS/KMS_Embedded_AES_Keys/Inc/kms_platf_objects_config.h    KMS embedded objects definition files
  - KMS/KMS_Embedded_AES_Keys/Inc/kms_platf_objects_interface.h KMS embedded objects interface files
  - KMS/KMS_Embedded_AES_Keys/Inc/main.h                        Header for main.c module
  - KMS/KMS_Embedded_AES_Keys/Inc/mbedtls_config.h              MBED crypto configuration file
  - KMS/KMS_Embedded_AES_Keys/Src/stm32wlxx_it.c                Interrupt handlers
  - KMS/KMS_Embedded_AES_Keys/Src/stm32wlxx_hal_msp.c           HAL MSP module
  - KMS/KMS_Embedded_AES_Keys/Src/app_kms.c                     KMS application
  - KMS/KMS_Embedded_AES_Keys/Src/ca_low_level.c                Cryptographic low level source file
  - KMS/KMS_Embedded_AES_Keys/Src/com.c                         Communication module file
  - KMS/KMS_Embedded_AES_Keys/Src/kms_low_level.c               KMS low level source file
  - KMS/KMS_Embedded_AES_Keys/Src/main.c                        Main program
  - KMS/KMS_Embedded_AES_Keys/Src/system_stm32wlxx.c            STM32WLxx system source file


@par Hardware and Software environment

  - This application runs on STM32WL55xx devices.
    
  - This application has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.
  - This application needs a terminal emulator.



@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
