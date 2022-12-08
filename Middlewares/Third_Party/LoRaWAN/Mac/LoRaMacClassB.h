/*!
 * \file      LoRaMacClassB.h
 *
 * \brief     LoRa MAC Class B layer implementation
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
 *              (C)2013 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \defgroup  LORAMACCLASSB LoRa MAC Class B layer implementation
 *            This module specifies the API implementation of the LoRaMAC Class B layer.
 *            This is a placeholder for a detailed description of the LoRaMac
 *            layer and the supported features.
 * \{
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LoRaMacClassB.h
  * @author  MCD Application Team
  * @brief   LoRa MAC Class B layer implementation
  ******************************************************************************
  */
#ifndef __LORAMACCLASSB_H__
#define __LORAMACCLASSB_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacTypes.h"
#include "LoRaMacVersion.h"

/*!
 * States of the class B ping slot mechanism
 */
typedef enum ePingSlotState
{
    /*!
     * Calculation of the ping slot offset
     */
    PINGSLOT_STATE_CALC_PING_OFFSET,
    /*!
     * State to set the timer to open the next ping slot
     */
    PINGSLOT_STATE_SET_TIMER,
    /*!
     * The node is in idle state
     */
    PINGSLOT_STATE_IDLE,
    /*!
     * The node opens up a ping slot window
     */
    PINGSLOT_STATE_RX,
}PingSlotState_t;

/*!
 * Class B ping slot context structure
 */
typedef struct sPingSlotContext
{

    /*!
     * Ping slot length time in ms
     */
    uint32_t PingSlotWindow;
    /*!
     * Ping offset
     */
    uint16_t PingOffset;
    /*!
     * Current symbol timeout. The node enlarges this variable in case of beacon
     * loss.
     */
    uint16_t SymbolTimeout;
    /*!
     * The multicast channel which will be enabled next.
     */
    MulticastCtx_t *NextMulticastChannel;
}PingSlotContext_t;

/*!
 * Class B beacon context structure
 */
typedef struct sBeaconContext
{
    struct sBeaconCtrl
    {
        /*!
         * Set if the node receives beacons
         */
        uint8_t BeaconMode          : 1;
        /*!
         * Set if the node has acquired the beacon
         */
        uint8_t BeaconAcquired      : 1;
        /*!
         * Set if a beacon delay was set for the beacon acquisition
         */
        uint8_t BeaconDelaySet      : 1;
        /*!
         * Set if a beacon channel was set for the beacon acquisition
         */
        uint8_t BeaconChannelSet    : 1;
        /*!
         * Set if beacon acquisition is pending
         */
        uint8_t AcquisitionPending  : 1;
        /*!
         * Set if the beacon state machine will be resumed
         */
        uint8_t ResumeBeaconing      : 1;
    }Ctrl;

    /*!
     * Current temperature
     */
    int16_t Temperature;
    /*!
     * Beacon time received with the beacon frame
     */
    SysTime_t BeaconTime;
    /*!
     * Time when the last beacon was received
     */
    SysTime_t LastBeaconRx;
    /*!
     * Time when the next beacon will be received
     */
    SysTime_t NextBeaconRx;
    /*!
     * This is the time where the RX window will be opened.
     * Its base is NextBeaconRx with temperature compensations
     * and RX window movement.
     */
    TimerTime_t NextBeaconRxAdjusted;
    /*!
     * Current symbol timeout. The node enlarges this variable in case of beacon
     * loss.
     */
    uint16_t SymbolTimeout;
    /*!
     * Specifies how much time the beacon window will be moved.
     */
    TimerTime_t BeaconWindowMovement;
    /*!
     * Beacon timing channel for next beacon
     */
    uint8_t BeaconTimingChannel;
    /*!
     * Delay for next beacon in ms
     */
    TimerTime_t BeaconTimingDelay;
    TimerTime_t TimeStamp;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /*!
     * Beacons transmit time precision determined using
     * param field of beacon frame format.
     */
    SysTime_t BeaconTimePrecision;
#endif /* LORAMAC_VERSION */
}BeaconContext_t;

/*!
 * Data structure which contains the callbacks
 */
typedef struct sLoRaMacClassBCallback
{
    /*!
     * \brief   Measures the temperature level
     *
     * \retval  Temperature level
     */
    int16_t ( *GetTemperatureLevel )( void );
    /*!
     *\brief    Will be called each time a Radio IRQ is handled by the MAC
     *          layer.
     *
     *\warning  Runs in a IRQ context. Should only change variables state.
     */
    void ( *MacProcessNotify )( void );
}LoRaMacClassBCallback_t;

/*!
 * Data structure which pointers to the properties LoRaMAC
 */
typedef struct sLoRaMacClassBParams
{
    /*!
     * Pointer to the MlmeIndication structure
     */
    MlmeIndication_t *MlmeIndication;
    /*!
     * Pointer to the McpsIndication structure
     */
    McpsIndication_t *McpsIndication;
    /*!
     * Pointer to the MlmeConfirm structure
     */
    MlmeConfirm_t *MlmeConfirm;
    /*!
     * Pointer to the LoRaMacFlags structure
     */
    LoRaMacFlags_t *LoRaMacFlags;
    /*!
     * Pointer to the LoRaMac device address
     */
    uint32_t *LoRaMacDevAddr;
    /*!
     * Pointer to the LoRaMac region definition
     */
    LoRaMacRegion_t *LoRaMacRegion;
    /*!
     * Pointer to the LoRaMacParams structure
     */
    LoRaMacParams_t *LoRaMacParams;
    /*!
     * Pointer to the multicast channel list
     */
    MulticastCtx_t *MulticastChannels;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /*!
     * Pointer to the activation type
     */
    ActivationType_t *NetworkActivation;
#endif /* LORAMAC_VERSION */
}LoRaMacClassBParams_t;

/*!
 * Signature of callback function to be called by this module when the
 * non-volatile needs to be saved.
 */
typedef void ( *LoRaMacClassBNvmEvent )( void );

/*!
 * \brief Initialize LoRaWAN Class B
 *
 * \param [in] classBParams Information and feedback parameter
 * \param [in] callbacks Contains the callback which the Class B implementation needs
 * \param [in] nvm Pointer to an external non-volatile memory data structure.
 */
void LoRaMacClassBInit( LoRaMacClassBParams_t *classBParams, LoRaMacClassBCallback_t *callbacks,
                        LoRaMacClassBNvmData_t* nvm );

/*!
 * \brief Set the state of the beacon state machine
 *
 * \param [in] beaconState Beacon state.
 */
void LoRaMacClassBSetBeaconState( BeaconState_t beaconState );

/*!
 * \brief Set the state of the ping slot state machine
 *
 * \param [in] pingSlotState Ping slot state.
 */
void LoRaMacClassBSetPingSlotState( PingSlotState_t pingSlotState );

/*!
 * \brief Set the state of the multicast slot state machine
 *
 * \param [in] multicastSlotState multicast slot state.
 */
void LoRaMacClassBSetMulticastSlotState( PingSlotState_t multicastSlotState );

/*!
 * \brief Verifies if an acquisition procedure is in progress
 *
 * \retval [true, if the acquisition is in progress; false, if not]
 */
bool LoRaMacClassBIsAcquisitionInProgress( void );

/*!
 * \brief Asks to the application cb to schedule the state machine of the Class B for beaconing
 */
void LoRaMacClassBBeaconTimerEvent( void* context );

/*!
 * \brief Asks to the application cb to schedule the state machine of the Class B for ping slots
 */
void LoRaMacClassBPingSlotTimerEvent( void* context );

/*!
 * \brief Asks to the application cb to schedule the state machine of the Class B for multicast slots
 */
void LoRaMacClassBMulticastSlotTimerEvent( void* context );

/*!
 * \brief Receives and decodes the beacon frame
 *
 * \param [in] payload Pointer to the payload
 * \param [in] size Size of the payload
 * \retval [true, if the node has received a beacon; false, if not]
 */
bool LoRaMacClassBRxBeacon( uint8_t *payload, uint16_t size );

/*!
 * \brief The function validates, if the node expects a beacon
 *        at the current time.
 *
 * \retval [true, if the node expects a beacon; false, if not]
 */
bool LoRaMacClassBIsBeaconExpected( void );

/*!
 * \brief The function validates, if the node expects a ping slot
 *        at the current time.
 *
 * \retval [true, if the node expects a ping slot; false, if not]
 */
bool LoRaMacClassBIsPingExpected( void );

/*!
 * \brief The function validates, if the node expects a multicast slot
 *        at the current time.
 *
 * \retval [true, if the node expects a multicast slot; false, if not]
 */
bool LoRaMacClassBIsMulticastExpected( void );

/*!
 * \brief Verifies if the acquisition pending bit is set
 *
 * \retval [true, if the bit is set; false, if not]
 */
bool LoRaMacClassBIsAcquisitionPending( void );

/*!
 * \brief Verifies if the beacon mode active bit is set
 *
 * \retval [true, if the bit is set; false, if not]
 */
bool LoRaMacClassBIsBeaconModeActive( void );

/*!
 * \brief Stops the beacon and ping slot operation
 */
void LoRaMacClassBHaltBeaconing( void );

/*!
 * \brief Resumes the beacon and ping slot operation
 */
void LoRaMacClassBResumeBeaconing( void );

/*!
 * \brief Sets the periodicity of the ping slots
 *
 * \param [in] periodicity Periodicity
 */
void LoRaMacClassBSetPingSlotInfo( uint8_t periodicity );

/*!
 * \brief Switches the device class
 *
 * \param [in] nextClass Device class to switch to
 *
 * \retval LoRaMacStatus_t Status of the operation.
 */
LoRaMacStatus_t LoRaMacClassBSwitchClass( DeviceClass_t nextClass );

/*!
 * \brief   LoRaMAC ClassB MIB-Get
 *
 * \details The mac information base service to get attributes of the LoRaMac
 *          Class B layer.
 *
 * \param   [in] mibGet - MIB-GET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacClassBMibGetRequestConfirm( MibRequestConfirm_t *mibGet );

/*!
 * \brief   LoRaMAC Class B MIB-Set
 *
 * \details The mac information base service to set attributes of the LoRaMac
 *          Class B layer.
 *
 * \param   [in] mibSet - MIB-SET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMibClassBSetRequestConfirm( MibRequestConfirm_t *mibSet );

/*!
 * \brief This function handles the PING_SLOT_FREQ_ANS
 */
void LoRaMacClassBPingSlotInfoAns( void );

/*!
 * \brief This function handles the PING_SLOT_CHANNEL_REQ
 *
 * \param [in] datarate Device class to switch to
 * \param [in] frequency Device class to switch to
 *
 * \retval Status for the MAC answer.
 */
uint8_t LoRaMacClassBPingSlotChannelReq( uint8_t datarate, uint32_t frequency );

/*!
 * \brief This function handles the BEACON_TIMING_ANS
 *
 * \param [in] beaconTimingDelay The beacon timing delay
 * \param [in] beaconTimingChannel The beacon timing channel
 * \param [in] lastRxDone The time of the last frame reception
 */
void LoRaMacClassBBeaconTimingAns( uint16_t beaconTimingDelay, uint8_t beaconTimingChannel, TimerTime_t lastRxDone );

/*!
 * \brief This function handles the ClassB DEVICE_TIME_ANS
 */
void LoRaMacClassBDeviceTimeAns( void );

/*!
 * \brief This function handles the BEACON_FREQ_REQ
 *
 * \param [in] frequency Frequency to set
 *
 * \retval [true, if MAC shall send an answer; false, if not]
 */
bool LoRaMacClassBBeaconFreqReq( uint32_t frequency );

/*!
 * \brief Queries the ping slot window time
 *
 * \param [in] txTimeOnAir TX time on air for the next uplink
 *
 * \retval Returns the time the uplink should be delayed
 */
TimerTime_t LoRaMacClassBIsUplinkCollision( TimerTime_t txTimeOnAir );

/*!
 * \brief Stops the timers for the RX slots. This includes the
 *        timers for ping and multicast slots.
 */
void LoRaMacClassBStopRxSlots( void );

/*!
 * \brief Starts the timers for the RX slots. This includes the
 *        timers for ping and multicast slots.
 */
void LoRaMacClassBStartRxSlots( void );

/*!
 * \brief Starts the timers for the RX slots. This includes the
 *        timers for ping and multicast slots.
 *
 * \param [in] multicastChannel Related multicast channel
 */
void LoRaMacClassBSetMulticastPeriodicity( MulticastCtx_t* multicastChannel );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief Sets the FPending bit status of the related downlink slot
 *
 * \param [in] address Slot address, could be unicast or multicast
 *
 * \param [in] fPendingSet Set to 1, if the fPending bit in the
 *             sequence is set, otherwise 0.
 */
void LoRaMacClassBSetFPendingBit( uint32_t address, uint8_t fPendingSet );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Class B process function.
 */
void LoRaMacClassBProcess( void );

/*! \} defgroup LORAMACCLASSB */

#ifdef __cplusplus
}
#endif

#endif // __LORAMACCLASSB_H__
