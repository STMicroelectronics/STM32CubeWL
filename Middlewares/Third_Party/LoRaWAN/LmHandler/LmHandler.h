/*!
 * \file      LmHandler.h
 *
 * \brief     Implements the LoRaMac layer handling.
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LmHandler.h
  * @author  MCD Application Team
  * @brief   Header for LoRaMAC Layer handling module
  ******************************************************************************
  */
#ifndef __LORAMAC_HANDLER_H__
#define __LORAMAC_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LmHandlerTypes.h"
#include "LoRaMacClassB.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/*!
 * \brief Join notification parameters
 */
  typedef struct LmHandlerJoinParams_s
{
  int8_t Datarate;
  LmHandlerErrorStatus_t Status;
  ActivationType_t Mode;
} LmHandlerJoinParams_t;

/*!
 * \brief Tx notification parameters
 */
typedef struct LmHandlerTxParams_s
{
  uint8_t IsMcpsConfirm;
  LoRaMacEventInfoStatus_t Status;
  LmHandlerMsgTypes_t MsgType;
  uint8_t AckReceived;
  int8_t Datarate;
  uint32_t UplinkCounter;
  LmHandlerAppData_t AppData;
  int8_t TxPower;
  uint8_t Channel;
} LmHandlerTxParams_t;

/*!
 * \brief Rx notification parameters
 */
typedef struct LmHandlerRxParams_s
{
  uint8_t IsMcpsIndication;
  LoRaMacEventInfoStatus_t Status;
  int8_t Datarate;
  int8_t Rssi;
  int8_t Snr;
  uint32_t DownlinkCounter;
  int8_t RxSlot;
} LmHandlerRxParams_t;

/*!
 * \brief Beacon notification parameters
 */
typedef struct LmHandlerBeaconParams_s
{
  LoRaMacEventInfoStatus_t Status;
  LmHandlerBeaconState_t State;
  BeaconInfo_t Info;
} LmHandlerBeaconParams_t;

/*!
 * \brief LoRaMac handler parameters
 */
typedef struct LmHandlerParams_s
{
  /*!
   * Active Region
   */
  LoRaMacRegion_t ActiveRegion;
  /*!
   * Default Class
   */
  DeviceClass_t DefaultClass;
  /*!
   * Holds the ADR state
   */
  bool AdrEnable;
  /*!
   * Uplink datarate, when \ref AdrEnable is OFF
   */
  int8_t TxDatarate;
  /*!
   * LoRaWAN ETSI duty cycle control enable/disable
   *
   * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
   */
  bool DutyCycleEnabled;
  /*!
   * Periodicity of the ping slots
   */
  uint8_t PingPeriodicity;
} LmHandlerParams_t;

/*!
 * \brief LoRaMac handler callbacks
 */
typedef struct LmHandlerCallbacks_s
{
  /*!
   * \brief Get the current battery level
   *
   * \retval value  Battery level ( 0: very low, 254: fully charged )
   */
  uint8_t (*GetBatteryLevel)(void);
  /*!
   * \brief Get the current temperature
   *
   * \retval value  Temperature in degree Celsius
   */
  /*float*/ uint16_t (*GetTemperature)(void);
  /*!
   * \brief    Will be called each time a Radio IRQ is handled by the MAC
   *          layer.
   *
   * \warning  Runs in a IRQ context. Should only change variables state.
   */
  void (*OnMacProcess)(void);
  /*!
   * \brief Notifies the upper layer that a network has been joined
   *
   * \param [in] params notification parameters
   */
  void (*OnJoinRequest)(LmHandlerJoinParams_t *params);
  /*!
   * \brief Notifies upper layer that a frame has been transmitted
   *
   * \param [in] params notification parameters
   */
  void (*OnTxData)(LmHandlerTxParams_t *params);
  /*!
   * \brief Notifies the upper layer that an applicative frame has been received
   *
   * \param [in] appData Received applicative data
   * \param [in] params notification parameters
   */
  void (*OnRxData)(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
} LmHandlerCallbacks_t;

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*!
 * \brief LoRaMac handler initialisation
 *
 * \param [in] handlerCallbacks LoRaMac handler callbacks
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if package have been
 *                initialized else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerInit(LmHandlerCallbacks_t *handlerCallbacks);

/*!
 * \brief Indicates if the LoRaMacHandler is busy
 *
 * \retval status [true] Busy, [false] free
 */
bool LmHandlerIsBusy(void);

/*!
 * \brief Processes the LoRaMac and Radio events. When no pendig operation asks to go in low power mode.
 *
 * \remark This function must be called in the main loop.
 */
void LmHandlerProcess(void);

/*!
 * \brief Instructs the MAC layer to send a ClassA uplink
 *
 * \param [in] appData Data to be sent
 * \param [in] isTxConfirmed Indicates if the uplink requires an acknowledgement
 * \param [out] nextTxIn Time before next uplink window available
 * \param [in] allowDelayedTx when set to true, the frame will be delayed
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerSend(LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed,
                                     TimerTime_t *nextTxIn, bool allowDelayedTx);

/*!
 * \brief   Check whether the Device is joined to the network
 *
 * \retval  status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
LmHandlerFlagStatus_t LmHandlerJoinStatus(void);

/*!
 * \brief Join a LoRa Network in classA
 *
 * \param [in] mode Activation mode (OTAA or ABP)
 */
void LmHandlerJoin(ActivationType_t mode);

/*!
 * \brief Stop a LoRa Network connection
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if LoRaMAC has been stopped successfully
 *                else \ref LORAMAC_HANDLER_BUSY_ERROR
 */
LmHandlerErrorStatus_t LmHandlerStop(void);

/*!
 * \brief Request the MAC layer to change LoRaWAN class
 *
 * \note Callback \ref LmHandlerConfirmClass informs upper layer that the change has occurred
 * \note Only switch from class A to class B/C OR from class B/C to class A is allowed
 *
 * \param [in] newClass New class to be requested
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else if device not yet joined a network \ref LORAMAC_HANDLER_NO_NETWORK_JOINED
                  else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerRequestClass(DeviceClass_t newClass);

/*!
 * \brief LoRaMac handler configuration
 *
 * \param [in] handlerParams LoRaMac handler parameters
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if configuration is
 *                done else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerConfigure(LmHandlerParams_t *handlerParams);

/*!
 * \brief LoRaMac handler package initialization
 *
 * \param [in] id package identifier
 * \param [in] params package parameters
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if package initialization is
 *                completed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerPackageRegister(uint8_t id, void *params);

/*!
 * \brief Gets the current LoRaWAN class
 *
 * \param [out] deviceClass current class
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetCurrentClass(DeviceClass_t *deviceClass);

/*!
 * \brief Gets the LoRaWAN Device EUI (if OTAA)
 *
 * \param [out] devEUI LoRaWAN DevEUI
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetDevEUI(uint8_t *devEUI);

/*!
 * \brief Sets the LoRaWAN Device EUI (if OTAA)
 *
 * \param [in] devEUI LoRaWAN DevEUI
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetDevEUI(uint8_t *devEUI);

/*!
 * \brief Gets the LoRaWAN AppEUI (from the Commissionning)
 *
 * \param [out] appEUI LoRaWAN AppEUI
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetAppEUI(uint8_t *appEUI);

/*!
 * \brief Sets the LoRaWAN App EUI (if OTAA)
 *
 * \param [in] appEUI LoRaWAN AppEUI
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetAppEUI(uint8_t *appEUI);

/*!
 * \brief Gets the LoRaWAN Network ID (from the Commissionning if ABP or after the Join if OTAA)
 *
 * \param [out] networkId current network ID
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetNetworkID(uint32_t *networkId);

/*!
 * \brief Sets the LoRaWAN Network ID
 *
 * \param [in] networkId new network ID
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetNetworkID(uint32_t networkId);

/*!
 * \brief Gets the LoRaWAN Device Address (from the Commissionning if ABP or after the Join if OTAA)
 *
 * \param [out] devAddr current device address
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetDevAddr(uint32_t *devAddr);

/*!
 * \brief Sets the LoRaWAN Device Address (if ABP)
 *
 * \param [in] devAddr device address
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetDevAddr(uint32_t devAddr);

/*!
 * \brief Gets the current active region
 *
 * \param [out] region Current active region
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetActiveRegion(LoRaMacRegion_t *region);

/*!
 * \brief Sets the current active region
 *
 * \param [in] region New active region requested
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetActiveRegion(LoRaMacRegion_t region);

/*!
 * \brief Gets the Adaptive data rate (1 = the Network manages the DR, 0 = the device manages the DR)
 *
 * \param [out] adrEnable Adaptive data rate flag
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetAdrEnable(bool *adrEnable);

/*!
 * \brief Sets the Adaptive data rate (1 = the Network manages the DR, 0 = the device manages the DR)
 *
 * \param [in] adrEnable Adaptive data rate flag
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetAdrEnable(bool adrEnable);

/*!
 * \brief Gets the current datarate
 *
 * \param [out] txDatarate Current TX datarate
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetTxDatarate(int8_t *txDatarate);

/*!
 * \brief Sets the current datarate
 *
 * \param [in] txDatarate new TX datarate
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetTxDatarate(int8_t txDatarate);

/*!
 * \brief Gets the duty cycle flag
 *
 * \param [out] dutyCycleEnable duty cycle flag
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetDutyCycleEnable(bool *dutyCycleEnable);

/*!
 * \brief Sets the current datarate
 *
 * \param [in] dutyCycleEnable duty cycle flag
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetDutyCycleEnable(bool dutyCycleEnable);

/*!
 * \brief Gets the current RX_2 datarate and frequency
 *
 * \param [out] rxParams rx2 parameters
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetRX2Params(RxChannelParams_t *rxParams);

/*!
 * \brief Gets the current TX power
 *
 * \param [out] txPower TX power
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetTxPower(int8_t *txPower);

/*!
 * \brief Gets the current RX1 delay (after the TX done)
 *
 * \param [out] rxDelay RX1 delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetRx1Delay(uint32_t *rxDelay);

/*!
 * \brief Gets the current RX2 delay (after the TX done)
 *
 * \param [out] rxDelay RX2 delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetRx2Delay(uint32_t *rxDelay);

/*!
 * \brief Gets the current RX1 Join delay (after the TX done)
 *
 * \param [out] rxDelay RX1 Join delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetJoinRx1Delay(uint32_t *rxDelay);

/*!
 * \brief Gets the current RX2 Join delay (after the TX done)
 *
 * \param [out] rxDelay RX2 Join delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetJoinRx2Delay(uint32_t *rxDelay);

/*!
 * \brief Sets the RX_2 datarate and frequency
 *
 * \param [in] rxParams rx2 parameters
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetRX2Params(RxChannelParams_t *rxParams);

/*!
 * \brief Sets the TX power
 *
 * \param [in] txPower TX power
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetTxPower(int8_t txPower);

/*!
 * \brief Sets the RX1 delay (after the TX done)
 *
 * \param [in] rxDelay RX1 delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetRx1Delay(uint32_t rxDelay);

/*!
 * \brief Sets the RX2 delay (after the TX done)
 *
 * \param [in] rxDelay RX2 delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetRx2Delay(uint32_t rxDelay);

/*!
 * \brief Sets the RX1 delay (after the TX done)
 *
 * \param [in] rxDelay RX1 Join delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetJoinRx1Delay(uint32_t rxDelay);

/*!
 * \brief Sets the RX2 delay (after the TX done)
 *
 * \param [in] rxDelay RX2 Join delay
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetJoinRx2Delay(uint32_t rxDelay);

/*!
 * \brief Gets the current ClassB Ping periodicity
 *
 * \param [out] pingPeriodicity ping periodicity
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetPingPeriodicity(uint8_t *pingPeriodicity);

/*!
 * \brief Sets the ClassB Ping periodicity
 *
 * \param [in] pingPeriodicity ping periodicity
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerSetPingPeriodicity(uint8_t pingPeriodicity);

/*!
 * \brief Gets the current ClassB Beacon status
 *
 * \param [out] beaconState current ClassB Beacon status
 *
 * \retval -1 LORAMAC_HANDLER_ERROR
 *          0 LORAMAC_HANDLER_SUCCESS
 */
int32_t LmHandlerGetBeaconState(BeaconState_t *beaconState);

#ifdef __cplusplus
}
#endif

#endif /* __LORAMAC_HANDLER_H__ */
