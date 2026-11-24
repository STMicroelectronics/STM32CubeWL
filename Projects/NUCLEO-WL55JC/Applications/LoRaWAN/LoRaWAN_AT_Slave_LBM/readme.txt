/**
  @page LoRaWAN_AT_Slave_LBM Readme file

  @verbatim
  ******************************************************************************
  * @file    Applications/LoRaWAN/LoRaWAN_AT_Slave_LBM/readme.txt
  * @author  MCD Application Team
  * @brief   This application exposes a LoRa Basics Modem through an AT command
  *          interface over UART by an external host.
  ******************************************************************************
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Description

This example exposes LoRa Basics Modem through an AT command interface on USART2. The host controls all LoRaWAN operations: the application does not join a network or transmit automatically.

@par Keywords

Applications, SubGHz_Phy, LoRaWAN, AT_Slave, SingleCore

@par Connection

Connect the NUCLEO-WL55JC board through the ST-LINK USB connector. Configure the serial terminal for 9600 baud, 8 data bits, 1 stop bit, no parity, and no flow control. Terminate commands with CR, LF, or CRLF. The modem returns OK on success or ERROR when an operation cannot be completed.

@par Command syntax

- AT performs a basic interface check.
- AT? displays this help.
- Commands ending in =? get a value; commands ending in =<value> set a value or start an operation.

@par Commands

AT             Basic interface check.
AT?            Display this help.
ATZ            Trigger an MCU reset.
AT+VL=?        Get the verbose level.
AT+VL=<Level>  Set the verbose level.
AT+LTIME=?     Get the local time in UTC format.
AT+RFS         Restore factory settings.
AT+CS          Store the current context in non-volatile memory.
AT+APPEUI=?    Get the JoinEUI.
AT+APPEUI=<EUI> Set the JoinEUI.
AT+NWKKEY=<KEY> Set the network key.
AT+APPKEY=<KEY> Set the AppKey.
AT+APPSKEY=<KEY> Set the application session key.
AT+NWKSKEY=<KEY> Set the network session key.
AT+DADDR=?     Get the device address.
AT+DADDR=<ADDR> Set the device address.
AT+DEUI=?      Get the DevEUI.
AT+DEUI=<EUI>  Set the DevEUI.
AT+JOIN[=1]    Join the network using OTAA.
AT+SEND=<PORT>:<ACK>:<HEX> Send a payload.
AT+VER=?       Get application and middleware versions.
AT+ADR=?       Get the ADR profile.
AT+ADR=<0|1|2> Set the ADR profile: 0=network controlled, 1=mobile long range, 2=mobile low power.
AT+DR=?        Get the data rate for the next uplink.
AT+DR=<0..7>   Set a fixed data rate.
AT+STATUS=?    Get modem and network status.
AT+BAND=?      Get the region ID.
AT+BAND=<ID>   Set the region ID.
AT+CLASS=?     Get the device class.
AT+CLASS=<A|B|C> Set the device class.
AT+DCS=?       Get the duty-cycle status in milliseconds; a negative value is the remaining wait time.
AT+PGSLOT=?    Get the Class B ping slot periodicity.
AT+PGSLOT=<0..7> Set the Class B ping slot periodicity: 0=1 s, 1=2 s, 2=4 s, 3=8 s, 4=16 s, 5=32 s, 6=64 s, 7=128 s.
AT+BAT         Get the battery voltage in millivolts.

The network key, AppKey, application session key, and network session key are write-only and cannot be read back.

Setting a fixed data rate switches the ADR profile to custom. The selected data rate must be supported by the active region and enabled channels.

The ping slot periodicity applies to Class B. When the device is joined, the modem sends a PingSlotInfoReq; confirmation is reported asynchronously by the modem event.

@par Hardware and Software environment

The example targets STM32WLxx Nucleo boards, including NUCLEO-WL55JC1 and NUCLEO-WL55JC2.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */