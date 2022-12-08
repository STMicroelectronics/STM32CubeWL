/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM4 side wrapper of the Radio interface
  *          shared between M0 and M4.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "radio_mbwrapper.h"
#include "radio.h"
#include "mbmux.h"
#include "msg_id.h"
#include "mbmuxif_radio.h"
#include "sys_app.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* **********************************************************************
 * Interrupts functions prototypes
 ************************************************************************/
/*!
 * @brief Process radio irq
 */
static void RadioIrqProcess(void);

/* **********************************************************************
 * Middleware Interface functions prototypes
 ************************************************************************/
/*!
 * \brief Initializes the radio
 *
 * \param[in] events Structure containing the driver callback functions
 */
static void RadioInit(RadioEvents_t *events);

/*!
 * Return current radio status
 *
 * \retval status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
static RadioState_t RadioGetStatus(void);

/*!
 * \brief Configures the radio with the given modem
 *
 * \param[in] modem Modem to be used [0: FSK, 1: LoRa]
 */
static void RadioSetModem(RadioModems_t modem);

/*!
 * \brief Sets the channel frequency
 *
 * \param[in] freq Channel RF frequency
 */
static void RadioSetChannel(uint32_t freq);

/*!
  * \brief Checks if the channel is free for the given time
  *
  * \param[in] freq                Channel RF frequency in Hertz
  * \param[in] rxBandwidth         Rx bandwidth in Hertz
  * \param[in] rssiThresh          RSSI threshold in dBm
  * \param[in] maxCarrierSenseTime Max time in milliseconds while the RSSI is measured
  *
  * \retval isFree         [true: Channel is free, false: Channel is not free]
  */
static bool RadioIsChannelFree(uint32_t freq, uint32_t rxBandwidth,
                               int16_t rssiThresh, uint32_t maxCarrierSenseTime);

/*!
 * \brief Generates a 32 bits random value based on the RSSI readings
 *
 * \remark This function sets the radio in LoRa modem mode and disables
 *         all interrupts.
 *         After calling this function either Radio.SetRxConfig or
 *         Radio.SetTxConfig functions must be called.
 *
 * \retval randomValue    32 bits random value
 */
static uint32_t RadioRandom(void);

/*!
 * \brief Sets the reception parameters
 *
 * \param[in] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param[in] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param[in] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] bandwidthAfc Sets the AFC Bandwidth (FSK only)
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: N/A ( set to 0 )
 * \param[in] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \param[in] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param[in] payloadLen   Sets payload length when fixed length is used
 * \param[in] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \param[in] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param[in] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param[in] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param[in] rxContinuous Sets the reception in continuous mode
 *                          [false: single mode, true: continuous mode]
 */
static void RadioSetRxConfig(RadioModems_t modem, uint32_t bandwidth,
                             uint32_t datarate, uint8_t coderate,
                             uint32_t bandwidthAfc, uint16_t preambleLen,
                             uint16_t symbTimeout, bool fixLen,
                             uint8_t payloadLen,
                             bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                             bool iqInverted, bool rxContinuous);

/*!
 * \brief Sets the transmission parameters
 *
 * \param[in] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param[in] power        Sets the output power [dBm]
 * \param[in] fdev         Sets the frequency deviation (FSK only)
 *                          FSK : [Hz]
 *                          LoRa: 0
 * \param[in] bandwidth    Sets the bandwidth (LoRa only)
 *                          FSK : 0
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param[in] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] preambleLen  Sets the preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param[in] crcOn        Enables disables the CRC [0: OFF, 1: ON]
 * \param[in] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param[in] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param[in] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param[in] timeout      Transmission timeout [ms]
 */
static void RadioSetTxConfig(RadioModems_t modem, int8_t power, uint32_t fdev,
                             uint32_t bandwidth, uint32_t datarate,
                             uint8_t coderate, uint16_t preambleLen,
                             bool fixLen, bool crcOn, bool FreqHopOn,
                             uint8_t HopPeriod, bool iqInverted, uint32_t timeout);

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param[in] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
static bool RadioCheckRfFrequency(uint32_t frequency);

/*!
 * \brief Computes the packet time on air in ms for the given payload
 *
 * \remark Can only be called once SetRxConfig or SetTxConfig have been called
 *
 * \param[in] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param[in] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param[in] spreadingFactor  Sets the Spreading Factor
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param[in] payloadLen   Sets payload length when fixed length is used
 * \param[in] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 *
 * \retval airTime        Computed airTime (ms) for the given packet payload length
 */
static uint32_t RadioGetTimeOnAir(RadioModems_t modem, uint32_t bandwidth,
                                  uint32_t spreadingFactor, uint8_t coderate,
                                  uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                  bool crcOn);

/*!
 * \brief Sends the buffer of size. Prepares the packet to be sent and sets
 *        the radio in transmission
 *
 * \param[in] buffer     Buffer pointer
 * \param[in] size       Buffer size
 */
static radio_status_t RadioSend(uint8_t *buffer, uint8_t size);

/*!
 * \brief Sets the radio in sleep mode
 */
static void RadioSleep(void);

/*!
 * \brief Sets the radio in standby mode
 */
static void RadioStandby(void);

/*!
 * \brief Sets the radio in reception mode for the given time
 * \param[in] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
static void RadioRx(uint32_t timeout);

/*!
 * \brief Start a Channel Activity Detection
 */
static void RadioStartCad(void);

/*!
 * \brief Sets the radio in continuous wave transmission mode
 *
 * \param[in] freq       Channel RF frequency
 * \param[in] power      Sets the output power [dBm]
 * \param[in] time       Transmission mode timeout [s]
 */
static void RadioSetTxContinuousWave(uint32_t freq, int8_t power, uint16_t time);

/*!
 * \brief Reads the current RSSI value
 *
 * \param[in] modem      Radio modem to be used [0: FSK, 1: LoRa]
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
static int16_t RadioRssi(RadioModems_t modem);

/*!
 * \brief Writes the radio register at the specified address
 *
 * \param[in] addr Register address
 * \param[in] data New register value
 */
static void RadioWrite(uint16_t addr, uint8_t data);

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param[in] addr Register address
 * \retval data Register value
 */
static uint8_t RadioRead(uint16_t addr);

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param[in] addr   First Radio register address
 * \param[in] buffer Buffer containing the new register's values
 * \param[in] size   Number of registers to be written
 */
static void RadioWriteRegisters(uint16_t addr, uint8_t *buffer, uint8_t size);

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param[in] addr First Radio register address
 * \param[out] buffer Buffer where to copy the registers data
 * \param[in] size Number of registers to be read
 */
static void RadioReadRegisters(uint16_t addr, uint8_t *buffer, uint8_t size);

/*!
 * \brief Sets the maximum payload length.
 *
 * \param[in] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param[in] max        Maximum payload length in bytes
 */
static void RadioSetMaxPayloadLength(RadioModems_t modem, uint8_t max);

/*!
 * \brief Sets the network to public or private. Updates the sync byte.
 *
 * \remark Applies to LoRa modem only
 *
 * \param[in] enable if true, it enables a public network
 */
static void RadioSetPublicNetwork(bool enable);

/*!
 * \brief Gets the time required for the board plus radio to get out of sleep.[ms]
 *
 * \retval time Radio plus board wakeup time in ms.
 */
static uint32_t RadioGetWakeUpTime(void);

/*!
 * \brief Sets the radio in reception mode with Max LNA gain for the given time
 * \param[in] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
static void RadioRxBoosted(uint32_t timeout);

/*!
 * \brief Sets the Rx duty cycle management parameters
 *
 * \param[in]  rxTime        Structure describing reception timeout value
 * \param[in]  sleepTime     Structure describing sleep timeout value
 */
static void RadioSetRxDutyCycle(uint32_t rxTime, uint32_t sleepTime);

/*!
 * \brief Set Tx PRBS modulated wave
 */
static void RadioTxPrbs(void);

/*!
 * \brief Set Tx continuous wave
 * \param[in]  power  Tx power level [0..15]
 */
static void RadioTxCw(int8_t power);

/*!
 * \brief Sets the reception parameters
 *
 * \param[in] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK]
 * \param[in] config       configuration of receiver
 *                          fsk field to be used if modem =GENERIC_FSK
*                           lora field to be used if modem =GENERIC_LORA
 * \param[in] rxContinuous Sets the reception in continuous mode
 *                          [0: single mode, otherwise continuous mode]
 * \param[in] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \return 0 when no parameters error, -1 otherwise
 */
static int32_t RadioSetRxGenericConfig(GenericModems_t modem, RxConfigGeneric_t *config,
                                       uint32_t rxContinuous, uint32_t symbTimeout);

/*!
 * \brief Sets the transmission parameters
 *
 * \param[in] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK or GENERIC_BPSK]
 * \param[in] config       configuration of receiver
 *                          fsk field to be used if modem =GENERIC_FSK
*                           lora field to be used if modem =GENERIC_LORA
                            bpsk field to be used if modem =GENERIC_BPSK
 * \param[in] power        Sets the output power [dBm]
 * \param[in] timeout      Transmission timeout [ms]
 * \return 0 when no parameters error, -1 otherwise
 */
static int32_t RadioSetTxGenericConfig(GenericModems_t modem, TxConfigGeneric_t *config,
                                       int8_t power, uint32_t timeout);

/*
* Private global constants
*/

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
  RadioInit,
  RadioGetStatus,
  RadioSetModem,
  RadioSetChannel,
  RadioIsChannelFree,
  RadioRandom,
  RadioSetRxConfig,
  RadioSetTxConfig,
  RadioCheckRfFrequency,
  RadioGetTimeOnAir,
  RadioSend,
  RadioSleep,
  RadioStandby,
  RadioRx,
  RadioStartCad,
  RadioSetTxContinuousWave,
  RadioRssi,
  RadioWrite,
  RadioRead,
  RadioWriteRegisters,
  RadioReadRegisters,
  RadioSetMaxPayloadLength,
  RadioSetPublicNetwork,
  RadioGetWakeUpTime,
  RadioIrqProcess,
  RadioRxBoosted,
  RadioSetRxDutyCycle,
  RadioTxPrbs,
  RadioTxCw,
  RadioSetRxGenericConfig,
  RadioSetTxGenericConfig,
};

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void Process_Radio_Notif(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Radio_Notif_1 */

  /* USER CODE END Process_Radio_Notif_1 */
  /* Radio Notif received */
  APP_LOG(TS_ON, VLEVEL_H, "CM4<(Radio)\r\n");

  /* process Command */
  switch (ComObj->MsgId)
  {
    default:
      break;
  }

  /* Radio sending ack */
  APP_LOG(TS_ON, VLEVEL_H, "CM4>(Radio)\r\n");
  MBMUX_AcknowledgeSnd(FEAT_INFO_RADIO_ID);
  /* USER CODE BEGIN Process_Radio_Notif_2 */

  /* USER CODE END Process_Radio_Notif_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void RadioInit(RadioEvents_t *events)
{
  /* USER CODE BEGIN RadioInit_1 */

  /* USER CODE END RadioInit_1 */
  return;
  /* USER CODE BEGIN RadioInit_2 */

  /* USER CODE END RadioInit_2 */
}

/*!
  * Return current radio status
  *
  * \retval status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
  */
static RadioState_t RadioGetStatus(void)
{
  /* USER CODE BEGIN RadioGetStatus_1 */

  /* USER CODE END RadioGetStatus_1 */
  return (RadioState_t) 0UL;
  /* USER CODE BEGIN RadioGetStatus_2 */

  /* USER CODE END RadioGetStatus_2 */
}

static void RadioSetModem(RadioModems_t modem)
{
  /* USER CODE BEGIN RadioSetModem_1 */

  /* USER CODE END RadioSetModem_1 */
  return;
  /* USER CODE BEGIN RadioSetModem_2 */

  /* USER CODE END RadioSetModem_2 */
}

static void RadioSetChannel(uint32_t freq)
{
  /* USER CODE BEGIN RadioSetChannel_1 */

  /* USER CODE END RadioSetChannel_1 */
  return;
  /* USER CODE BEGIN RadioSetChannel_2 */

  /* USER CODE END RadioSetChannel_2 */
}

static bool RadioIsChannelFree(uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime)
{
  /* USER CODE BEGIN RadioIsChannelFree_1 */

  /* USER CODE END RadioIsChannelFree_1 */
  return (bool) 0UL;
  /* USER CODE BEGIN RadioIsChannelFree_2 */

  /* USER CODE END RadioIsChannelFree_2 */
}

static uint32_t RadioRandom(void)
{
  /* USER CODE BEGIN RadioRandom_1 */

  /* USER CODE END RadioRandom_1 */
  return 0UL;
  /* USER CODE BEGIN RadioRandom_2 */

  /* USER CODE END RadioRandom_2 */
}

static void RadioSetRxConfig(RadioModems_t modem, uint32_t bandwidth,
                             uint32_t datarate, uint8_t coderate,
                             uint32_t bandwidthAfc, uint16_t preambleLen,
                             uint16_t symbTimeout, bool fixLen,
                             uint8_t payloadLen,
                             bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                             bool iqInverted, bool rxContinuous)
{
  /* USER CODE BEGIN RadioSetRxConfig_1 */

  /* USER CODE END RadioSetRxConfig_1 */
  return;
  /* USER CODE BEGIN RadioSetRxConfig_2 */

  /* USER CODE END RadioSetRxConfig_2 */
}

static void RadioSetTxConfig(RadioModems_t modem, int8_t power, uint32_t fdev,
                             uint32_t bandwidth, uint32_t datarate,
                             uint8_t coderate, uint16_t preambleLen,
                             bool fixLen, bool crcOn, bool freqHopOn,
                             uint8_t hopPeriod, bool iqInverted, uint32_t timeout)
{
  /* USER CODE BEGIN RadioSetTxConfig_1 */

  /* USER CODE END RadioSetTxConfig_1 */
  return;
  /* USER CODE BEGIN RadioSetTxConfig_2 */

  /* USER CODE END RadioSetTxConfig_2 */
}

static bool RadioCheckRfFrequency(uint32_t frequency)
{
  /* USER CODE BEGIN RadioCheckRfFrequency_1 */

  /* USER CODE END RadioCheckRfFrequency_1 */
  return (bool) 0UL;
  /* USER CODE BEGIN RadioCheckRfFrequency_2 */

  /* USER CODE END RadioCheckRfFrequency_2 */
}

static uint32_t RadioGetTimeOnAir(RadioModems_t modem, uint32_t bandwidth,
                                  uint32_t spreadingFactor, uint8_t coderate,
                                  uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                  bool crcOn)
{
  /* USER CODE BEGIN RadioGetTimeOnAir_1 */

  /* USER CODE END RadioGetTimeOnAir_1 */
  return 0UL;
  /* USER CODE BEGIN RadioGetTimeOnAir_2 */

  /* USER CODE END RadioGetTimeOnAir_2 */
}

static radio_status_t RadioSend(uint8_t *buffer, uint8_t size)
{
  /* USER CODE BEGIN RadioSend_1 */

  /* USER CODE END RadioSend_1 */
  uint32_t ret = RADIO_STATUS_OK;
  return (radio_status_t) ret;
  /* USER CODE BEGIN RadioSend_2 */

  /* USER CODE END RadioSend_2 */
}

static void RadioSleep(void)
{
  /* USER CODE BEGIN RadioSleep_1 */

  /* USER CODE END RadioSleep_1 */
  return;
  /* USER CODE BEGIN RadioSleep_2 */

  /* USER CODE END RadioSleep_2 */
}

static void RadioStandby(void)
{
  /* USER CODE BEGIN RadioStandby_1 */

  /* USER CODE END RadioStandby_1 */
  return;
  /* USER CODE BEGIN RadioStandby_2 */

  /* USER CODE END RadioStandby_2 */
}

static void RadioRx(uint32_t timeout)
{
  /* USER CODE BEGIN RadioRx_1 */

  /* USER CODE END RadioRx_1 */
  return;
  /* USER CODE BEGIN RadioRx_2 */

  /* USER CODE END RadioRx_2 */
}

static void RadioStartCad(void)
{
  /* USER CODE BEGIN RadioStartCad_1 */

  /* USER CODE END RadioStartCad_1 */
  return;
  /* USER CODE BEGIN RadioStartCad_2 */

  /* USER CODE END RadioStartCad_2 */
}

static void RadioSetTxContinuousWave(uint32_t freq, int8_t power, uint16_t time)
{
  /* USER CODE BEGIN RadioSetTxContinuousWave_1 */

  /* USER CODE END RadioSetTxContinuousWave_1 */
  return;
  /* USER CODE BEGIN RadioSetTxContinuousWave_2 */

  /* USER CODE END RadioSetTxContinuousWave_2 */
}

static int16_t RadioRssi(RadioModems_t modem)
{
  /* USER CODE BEGIN RadioRssi_1 */

  /* USER CODE END RadioRssi_1 */
  return (int16_t) 0UL;
  /* USER CODE BEGIN RadioRssi_2 */

  /* USER CODE END RadioRssi_2 */
}

static void RadioWrite(uint16_t addr, uint8_t data)
{
  /* USER CODE BEGIN RadioWrite_1 */

  /* USER CODE END RadioWrite_1 */
  return;
  /* USER CODE BEGIN RadioWrite_2 */

  /* USER CODE END RadioWrite_2 */
}

static uint8_t RadioRead(uint16_t addr)
{
  /* USER CODE BEGIN RadioRead_1 */

  /* USER CODE END RadioRead_1 */
  return (uint8_t) 0UL;
  /* USER CODE BEGIN RadioRead_2 */

  /* USER CODE END RadioRead_2 */
}

static void RadioWriteRegisters(uint16_t addr, uint8_t *buffer, uint8_t size)
{
  /* USER CODE BEGIN RadioWriteRegisters_1 */

  /* USER CODE END RadioWriteRegisters_1 */
  return;
  /* USER CODE BEGIN RadioWriteRegisters_2 */

  /* USER CODE END RadioWriteRegisters_2 */
}

static void RadioReadRegisters(uint16_t addr, uint8_t *buffer, uint8_t size)
{
  /* USER CODE BEGIN RadioReadRegisters_1 */

  /* USER CODE END RadioReadRegisters_1 */
  return;
  /* USER CODE BEGIN RadioReadRegisters_2 */

  /* USER CODE END RadioReadRegisters_2 */
}

static void RadioSetMaxPayloadLength(RadioModems_t modem, uint8_t max)
{
  /* USER CODE BEGIN RadioSetMaxPayloadLength_1 */

  /* USER CODE END RadioSetMaxPayloadLength_1 */
  return;
  /* USER CODE BEGIN RadioSetMaxPayloadLength_2 */

  /* USER CODE END RadioSetMaxPayloadLength_2 */
}

static void RadioSetPublicNetwork(bool enable)
{
  /* USER CODE BEGIN RadioSetPublicNetwork_1 */

  /* USER CODE END RadioSetPublicNetwork_1 */
  return;
  /* USER CODE BEGIN RadioSetPublicNetwork_2 */

  /* USER CODE END RadioSetPublicNetwork_2 */
}

static uint32_t RadioGetWakeUpTime(void)
{
  /* USER CODE BEGIN RadioGetWakeUpTime_1 */

  /* USER CODE END RadioGetWakeUpTime_1 */
  return 0UL;
  /* USER CODE BEGIN RadioGetWakeUpTime_2 */

  /* USER CODE END RadioGetWakeUpTime_2 */
}

static void RadioIrqProcess(void)
{
  /* USER CODE BEGIN RadioIrqProcess_1 */

  /* USER CODE END RadioIrqProcess_1 */
  return;
  /* USER CODE BEGIN RadioIrqProcess_2 */

  /* USER CODE END RadioIrqProcess_2 */
}

static void RadioRxBoosted(uint32_t timeout)
{
  /* USER CODE BEGIN RadioRxBoosted_1 */

  /* USER CODE END RadioRxBoosted_1 */
  return;
  /* USER CODE BEGIN RadioRxBoosted_2 */

  /* USER CODE END RadioRxBoosted_2 */
}

static void RadioSetRxDutyCycle(uint32_t rxTime, uint32_t sleepTime)
{
  /* USER CODE BEGIN RadioSetRxDutyCycle_1 */

  /* USER CODE END RadioSetRxDutyCycle_1 */
  return;
  /* USER CODE BEGIN RadioSetRxDutyCycle_2 */

  /* USER CODE END RadioSetRxDutyCycle_2 */
}

static void RadioTxPrbs(void)
{
  /* USER CODE BEGIN RadioTxPrbs_1 */

  /* USER CODE END RadioTxPrbs_1 */
  return;
  /* USER CODE BEGIN RadioTxPrbs_2 */

  /* USER CODE END RadioTxPrbs_2 */
}

static void RadioTxCw(int8_t power)
{
  /* USER CODE BEGIN RadioTxCw_1 */

  /* USER CODE END RadioTxCw_1 */
  return;
  /* USER CODE BEGIN RadioTxCw_2 */

  /* USER CODE END RadioTxCw_2 */
}

static int32_t RadioSetRxGenericConfig(GenericModems_t modem, RxConfigGeneric_t *config, uint32_t rxContinuous,
                                       uint32_t symbTimeout)
{
  /* USER CODE BEGIN RadioSetRxGenericConfig_1 */

  /* USER CODE END RadioSetRxGenericConfig_1 */
  return 0UL;
  /* USER CODE BEGIN RadioSetRxGenericConfig_2 */

  /* USER CODE END RadioSetRxGenericConfig_2 */
}

static int32_t RadioSetTxGenericConfig(GenericModems_t modem, TxConfigGeneric_t *config, int8_t power, uint32_t timeout)
{
  /* USER CODE BEGIN RadioSetTxGenericConfig_1 */

  /* USER CODE END RadioSetTxGenericConfig_1 */
  return 0UL;
  /* USER CODE BEGIN RadioSetTxGenericConfig_2 */

  /* USER CODE END RadioSetTxGenericConfig_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
