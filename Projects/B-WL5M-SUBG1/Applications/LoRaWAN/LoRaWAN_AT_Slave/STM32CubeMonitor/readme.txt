
STM32WLxx Wireless Long Range RF test Readme file

******************************************************************************
 Copyright (c) 2020 STMicroelectronics.
 All rights reserved.
 This software component is licensed by ST under license SLA0048, the "License";
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License at: www.st.com/SLA0048

 The sub-flow "ui-table handler" is licensed under under Apache License, Version 2.0
 You may obtain a copy of the License at:opensource.org/licenses/Apache-2.0

******************************************************************************


The STM32 Wireless Long Range RF test flow is used to test the wireless part of STM32WL. The test flow can be used to :
- Display the Received Signal Strength Indicator (RSSI)
- Send a Continuous Wave test tone
- Perform Packet Error Rate measurement with 2 devices (Lora and FSK modulation)
- Send AT commands to the device
- Calculate AirTime for Lora packets


This directory contains the flow to be loaded in STM32CubeMonitor. (WLR_RF_test.json)

The information to install and use the dashboard are available in wiki at
https://wiki.st.com/stm32mcu/wiki/STM32CubeMonitor:Wireless_Long_Range_RF_Test

This application is targeting boards embedding the STM32WLxx with AT_Slave firmware.

******************************************************************************

