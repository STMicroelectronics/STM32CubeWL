/**
  @page 2_Images_SBSFU / Secure Boot and Secure Firmware Update Demo Application (Dual Core)

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows Secure Boot and Secure Firmware Update example.
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

The Secure Boot (SB) and Secure Firmware Update (SFU) solution allows the update of the STM32 microcontroller built-in
program with new firmware versions, adding new features and correcting potential issues. The update process is performed
in a secure way to prevent unauthorized updates and access to confidential on-device data such as secret code and
firmware encryption key.
In addition, Secure Boot (Root of Trust services) checks and activates the STM32 security mechanisms, and checks the
authenticity and integrity of user application code before every execution to ensure that invalid or malicious code
cannot be run. The Secure Firmware Update application receives the encrypted firmware image, decrypts it, then checks
the authenticity and integrity of the code before installing it.

The secure key management services provide cryptographic services to the user application through the pkcs11 APIs
(KEY ID based APIs) that are executed inside a protected and isolated environment. User application keys are stored in
the protected and isolated environment. They can be updated in a secure way (authenticity check, data decryption and
data integrity check).

This example supports multiple images feature configured with 2 active images for
execution from internal flash and a 1-slot configuration allowing to
maximize the user application size.
As a consequence, some features are not available:
* download a new firmware image from the application
* resume firmware update procedure in case of power off during installation
For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube" available from the
STMicroelectronics microcontroller website www.st.com.

@par Keywords

Security, SBSFU, GTZC, Dual core, MPU

@par Directory contents

   - 2_Images_SBSFU/CM0PLUS/Core/Src/main.c                           Main M0+ application file
   - 2_Images_SBSFU/CM0PLUS/Core/Src/stm32wlxx_hal_msp.c              M0+ HAL MSP module
   - 2_Images_SBSFU/CM0PLUS/Core/Src/stm32wlxx_it.c                   STM32 M0+ interrupt handlers
   - 2_Images_SBSFU/CM0PLUS/Core/Inc/app_hw.h                         Hardware definition for application
   - 2_Images_SBSFU/CM0PLUS/Core/Inc/main.h                           Header file for M0+ main.c
   - 2_Images_SBSFU/CM0PLUS/Core/Inc/stm32wlxx_nucleo_conf.h          M0+ BSP configuration file
   - 2_Images_SBSFU/CM0PLUS/Core/Inc/stm32wlxx_hal_conf.h             M0+ HAL configuration file
   - 2_Images_SBSFU/CM0PLUS/Core/Inc/stm32wlxx_it.h                   Header file for M0+ stm32wlxx_it.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_boot.c                      Secure Boot (SB): entry/exit points and state machine
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/Src/sfu_com_loader.c            SBSFU communication module: local loader part
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/Src/sfu_com_trace.c             SBSFU communication module: trace part
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_error.c                     SBSFU errors management
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_fwimg_common.c              SBSFU image handling: common functionalities/services
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_interface_crypto_scheme.c   Crypto scheme implementation based on KMS API
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_kms.c                       Key management service blob importation handling
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_loader.c                    SBSFU Local Loader
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_mpu_isolation.c             Secure Engine isolation with MPU (when no Firewall is available)
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_new_image.c                 SBSFU image handling: new image storage and installation request
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_test.c                      SBSFU security protection automatic test
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/app_sfu.h                       Software configuration of M0+ SBSFU application
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_boot.h                      Header file for sfu_boot.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_com_loader.h                Header file for sfu_com_loader.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_com_trace.h                 Header file for sfu_com_trace.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_error.h                     Header file for sfu_error.c file
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_fsm_states.h                SBSFU FSM states definitions
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_fwimg_internal.h            Internal definitions for firmware image handling
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_fwimg_services.h            Header file for sfu_fwimg_services.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_interface_crypto_scheme.h   Header file for sfu_interface_crypto_scheme.h
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_kms.h                       Header file for sfu_kms.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_loader.h                    Header file for sfu_loader.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_mpu_isolation.h             Header file for sfu_mpu_isolation.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_new_image.h                 Header file for sfu_new_image.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_test.h                      Header file for sfu_test.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/App/sfu_trace.h                     Header file for sfu_trace.c
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level.c              SBSFU general low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash.c        SBSFU flash low level interface (wrapper)
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash_ext.c    SBSFU external flash low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash_int.c    SBSFU internal flash low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_security.c     SBSFU security low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level.h              Header file for general low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash.h        Header file for flash low level interface (wrapper)
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash_ext.h    Header external file for flash low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_flash_int.h    Header internal file for flash low level interface
   - 2_Images_SBSFU/CM0PLUS/SBSFU/Target/sfu_low_level_security.h     Header file for security low level interface
   - 2_Images_SBSFU/CM4/Src/main.c                                    Main M4 application file
   - 2_Images_SBSFU/CM4/Src/sfu_low_level_security.c                  M4 SB security low level interface
   - 2_Images_SBSFU/CM4/Src/stm32wlxx_hal_msp.c                       M4 HAL MSP module
   - 2_Images_SBSFU/CM4/Src/stm32wlxx_it.c                            STM32 M4 interrupt handlers
   - 2_Images_SBSFU/CM4/Inc/app_sfu.h                                 Software configuration of M4 SB application
   - 2_Images_SBSFU/CM4/Inc/main.h                                    Header file for M4 main.c
   - 2_Images_SBSFU/CM4/Inc/sfu_boot.h                                Definitions for Secure Boot (SB) flow control
   - 2_Images_SBSFU/CM4/Inc/sfu_low_level_security.h                  Header file for security low level interface
   - 2_Images_SBSFU/CM4/Inc/stm32wlxx_hal_conf.h                      M4 HAL configuration file
   - 2_Images_SBSFU/CM4/Inc/stm32wlxx_it.h                            Header file for M4 stm32wlxx_it.c
   - 2_Images_SBSFU/CM4/Inc/stm32wlxx_nucleo_conf.h                   M4 BSP configuration file
   - 2_Images_SBSFU/Common/app_sfu_common.h                           Common software configuration of M4 SB and M0+ SBSFU applications
   - 2_Images_SBSFU/Common/sfu_def.h                                  General definition for SBSFU application
   - 2_Images_SBSFU/Common/sfu_fwimg_regions.h                        FLASH regions definitions for image handling

@par Hardware and Software environment

   - This example runs on STM32WL55xx devices.
   - This example has been tested with NUCLEO-WL55JC RevC board and can be easily tailored to any other supported device and
     development board.
   - An up-to-date version of ST-LINK firmware is required. Upgrading ST-LINK firmware is a feature provided by
     STM32Cube programmer available on www.st.com.
   - This example is linked with SE_Core binary generated by Secure Engine Core binary generation project.
   - This example needs a terminal emulator.
   - By default, SBSFU is configured in development mode. To switch in production mode, refer to AN5056 "Integration
     guide - software expansion for STM32Cube" available from the STMicroelectronics microcontroller website
     www.st.com.
   - Microsoft Windows has a limitation whereby paths to files and directories cannot be longer than 256 characters.
     Paths to files exceeding that limits cause tools (e.g. compilers, shell scripts) to fail reading from or writing
     to such files.
     As a workaround, it is advised to use the subst.exe command from within a command prompt to set up a local drive
     out of an existing directory on the hard drive, such as:
     C:\> subst X: <PATH_TO_CUBEFW>\Firmware

@par How to use it ?

Several steps to run SBSFU application :

1. Compile projects in the following order. This is mandatory as each project requests some objects generated by the
   compilation of the previous one:
   - 2_Images_SECoreBin (see also SECoreBin/readme.txt)
   - 2_Images_SBSFU (using both CM4 and CM0+ workspaces)
   - 2_Images_UserApp_M0Plus (see also UserApp_M0Plus/readme.txt)
   - 2_Images_UserApp_M4 (see also UserApp_M4/readme.txt)

2. Before loading SBSFU image into the target, please ensure with STM32CubeProgrammer available on www.st.com that the
   following are valid for the device:
   - RDP Level 0
   - Write Protection disabled on all FLASH pages
   - Chip has been erased

3. Use a terminal emulator (Tera Term for example, open source free software terminal emulator that can be downloaded
   from https://osdn.net/projects/ttssh2/) for UART connection with the board.
   Support of YMODEM protocol is required. Serial port configuration should be :
   - Baud rate = 115200
   - Data = 8 bits
   - Parity = none
   - Stop = 1 bit
   - Flow control = none

4. Load successively CM4 SB image and CM0+ SBSFU image into target memory with your preferred toolchain or STM32CubeProgrammer:
   - STM32CubeProgrammer with IAR SBSFU files:
       - EWARM\CM0PLUS\Exe\Project.out
       - EWARM\CM4\Exe\Project.out
   - STM32CubeProgrammer with STM32CubeIDE files:
       - STM32CubeIDE\CM0PLUS\Debug\SBSFU.elf
       - STM32CubeIDE\CM4\Debug\SB.elf
   - STM32CubeProgrammer with Keil files:
       - MDK-ARM\.._CM0PLUS\Exe\SBSFU.axf
       - MDK-ARM\.._CM4\Exe\SB.axf
   - IAR with .out files : From CM4 project (SB or M4 UserApp):
       - Project > Download > Download file... > CM0+ Project.out
       - Project > Download > Download file... > CM4 Project.out 
	   refer to AN5556 Getting started with STM32WL5x MCUs using IAR Embedded Workbench and MDK-ARM
	- STM32CubeIDE : CM4 debug configuration is responsible for loading both SB and SBSFU projects
	   refer to AN5564 Getting started with projects based on dual-core STM32WL microcontrollers in STM32CubeIDE

5. Once the SB_SFU software is downloaded, power cycle the board (unplug/plug USB cable) : the SBSFU application starts
   and configures the security mechanisms.

6. Power cycle the board a second time (unplug/plug the USB cable): the SBSFU application starts with the configured
   securities turned on and the Tera Term connection is possible.
   Caution: Make sure to use an up-to-date version of ST-LINK firmware else SBSFU may not start.

7. At startup (Power On or Reset button pushed) welcome information are displayed on terminal emulator.
   Red LED blinks every 500 ms when waiting for the CM0+ SBSFU to boot.
   Green LED blinks every 100 ms when CM0+ and/or CM4 firmware image(s) need(s) to be downloaded.
   Red LED blinks every 3 seconds when a local download is waited.
   A reboot occurs in case of error in option bytes configuration.

8. Send the first user encrypted firmware file (\1_Slot\2_Images_UserApp_M0Plus\Binary\UserApp.sfb or
   \1_Slot\2_Images_UserApp_M4\Binary\UserApp.sfb) with Tera Term by using menu
   "File > Transfer > YMODEM > Send..."

9. Send the second user encrypted firmware file (\1_Slot\2_Images_UserApp_M0Plus\Binary\UserApp.sfb or
   \1_Slot\2_Images_UserApp_M4\Binary\UserApp.sfb) with Tera Term by using menu
   "File > Transfer > YMODEM > Send..."

Note1 : Press User push-button (B1) at reset to force a local download if an application is already installed.
Note2 : As TAMPER detection can be very sensitive, the protection is disabled by default.
        Uncomment "SFU_TAMPER_PROTECT_ENABLE" in app_sfu.h to enable it.
Note3 : for Linux users Minicom can be used but to do so you need to compile the SBSFU project with the MINICOM_YMODEM
        switch enabled (app_sfu.h)

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
