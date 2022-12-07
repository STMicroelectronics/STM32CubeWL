/**
  @page 1_Image_UserApp / Firmware Update - User Application Demo

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows a User Application
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application demonstrates firmware download capabilities and provides a set of functions to test the active
protections offered by Secure Boot and Secure Engine.
It also demonstrates key management services through basic examples and specific boot
mechanism (firmware update decryption, verification...)
A terminal connected with the board via VCOM is needed to communicate with the board and to select which feature
to demonstrate.

For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube" available from
the STMicroelectronics microcontroller website www.st.com.

@par Keywords

Security, KMS, SBSFU

@par Directory contents

   - 1_Image_UserApp/Src/com.c                    Communication module file
   - 1_Image_UserApp/Src/common.c                 Common module file
   - 1_Image_UserApp/Src/flash_if.c               Flash interface file
   - 1_Image_UserApp/Src/fw_update_app.c          Firmware update application
   - 1_Image_UserApp/Src/kms_object_update_app.c  KMS Blob update application
   - 1_Image_UserApp/Src/main.c                   Main program
   - 1_Image_UserApp/Src/se_user_code.c           Call user defined services running in Secure Engine
   - 1_Image_UserApp/Src/stm32wlxx_it.c           Interrupt handlers
   - 1_Image_UserApp/Src/system_stm32wlxx.c       STM32 system file
   - 1_Image_UserApp/Src/test_protections.c       Protection test
   - 1_Image_UserApp/Src/tkms_app.c               tKMS tests
   - 1_Image_UserApp/Src/tkms_app_derive_key.c    tKMS key derivation tests
   - 1_Image_UserApp/Src/tkms_app_encrypt_decrypt.c tKMS encryption/decryption tests
   - 1_Image_UserApp/Src/tkms_app_sign_verify.c   tKMS signature/verification tests
   - 1_Image_UserApp/Src/ymodem.c                 Ymodem communication module
   - 1_Image_UserApp/Inc/com.h                    Header for com.c file
   - 1_Image_UserApp/Inc/common.h                 Header for common.c file
   - 1_Image_UserApp/Inc/flash_if.h               Header for flash_if.c file
   - 1_Image_UserApp/Inc/fw_update_app.h          Header for fw_update_app.c file
   - 1_Image_UserApp/Inc/main.h                   Header for main.c file
   - 1_Image_UserApp/Inc/se_user_code.h           Header file for se_user_code.c
   - 1_Image_UserApp/Inc/sfu_app_new_image.h      Definition for the new blob storage and installation
   - 1_Image_UserApp/Inc/stm32wlxx_hal_conf.h     HAL configuration file
   - 1_Image_UserApp/Inc/stm32wlxx_it.h           Header for stm32wlxx_it.c file
   - 1_Image_UserApp/Inc/stm32wlxx_nucleo_conf.h  BSP configuration file
   - 1_Image_UserApp/Inc/test_protections.h       Header for test_protections.c file
   - 1_Image_UserApp/Inc/tkms_app.h               Header for tkms_app.c file
   - 1_Image_UserApp/Inc/ymodem.h                 Header for ymodem.c file

@par Hardware and Software environment

  - This example runs on STM32WL55xx devices.
  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.
  - An up-to-date version of ST-LINK firmware is required. Upgrading ST-LINK firmware
    is a feature provided by STM32Cube programmer available on www.st.com.
  - This example is based on se_interface_application.o module exported by BFU project.
  - This example needs a terminal emulator.
  - Microsoft Windows has a limitation whereby paths to files and directories cannot
    be longer than 256 characters. Paths to files exceeding that limits cause tools (e.g. compilers,
    shell scripts) to fail reading from or writing to such files.
    As a workaround, it is advised to use the subst.exe command from within a command prompt to set
    up a local drive out of an existing directory on the hard drive, such as:
    C:\> subst X: <PATH_TO_CUBEFW>\Firmware

@par IDE postbuild script

In order to ease the development process, a postbuild script ("postbuild.bat") is integrated in each IDE project.
This postbuild script:
 - is generated when compiling the Secure Engine Core project,
 - prepares the firmware image of the user application to be installed in the device.

A known limitation of this integration occurs when you update the firmware version (parameter of postbuild.bat script).
The IDE does not track this update so you need to force the rebuild of the project manually.

@par How to use it ?

Refer to BFU readme and follow steps by steps instructions.

Once executed, this user application gives access to a menu which allows:
  1 - to download a new firmware : feature not supported in 1 firmware image example
  2 - to test protections (WRP, IWDG, TAMPER)
  3 - to demonstrate how to call user defined services running in Secure Engine
  4 - to provide access to multiple images feature (not supported)
  5 - to validate a firmware image at first start-up (not supported)
  a - to demonstrate usage of key management services

1. Pressing 1 allows to download a new firmware.
Feature not supported in 1 firmware image example

2. Pressing 2 allows to test protections.

 - CORRUPT IMAGE test (#1): causes a signature verification failure at next boot
 - WRP test (#2): causes an error trying to erase write protected code
 - IWDG test (#3): causes a reset simulating a deadlock by not refreshing the watchdog
 - TAMPER test (#4): causes a reset if a tamper event is detected. In order to generate a tamper event,
   user has to connect PC13 (CN7.23) to GND (It may be enough to put your finger close to PC13 (CN7.23)).

3. Pressing 3 allows to call user defined services running in Secure Engine.
As an example, after selecting the firmware image, SE_APP_GetActiveFwInfo service is called in order
to display the information located in the protected area such as version and size.

4. This menu is dedicated to multiple images feature.
Feature not available as there is no firmware download area.

5. This menu is dedicated to image validation.
Feature available under ENABLE_IMAGE_STATE_HANDLING compilation switch, not available in this example.

a. Pressing a allows to enter specific key management services test menu.
With this menu, you should be able to try encryption/decryption, signature/verification or key derivation mechanisms.


Note1 : There is only 1 active slot configured in this example.
Note2 : for Linux users Minicom can be used but to do so you need to compile the UserApp project with the MINICOM_YMODEM
        switch enabled (ymodem.h)


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
