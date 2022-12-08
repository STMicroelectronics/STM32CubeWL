/*!
 * \file      LoRaMac.h
 *
 * \brief     LoRa MAC layer implementation
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
 *              (C)2013-2017 Semtech
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
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * \defgroup  LORAMAC LoRa MAC layer implementation
 *            This module specifies the API implementation of the LoRaMAC layer.
 *            This is a placeholder for a detailed description of the LoRaMac
 *            layer and the supported features.
 * \{
 *
 * \example   periodic-uplink-lpp/B-L072Z-LRWAN1/main.c
 *            LoRaWAN class A/B/C application example for the B-L072Z-LRWAN1.
 *
 * \example   periodic-uplink-lpp/NAMote72/main.c
 *            LoRaWAN class A/B/C application example for the NAMote72.
 *
 * \example   periodic-uplink-lpp/NucleoL073/main.c
 *            LoRaWAN class A/B/C application example for the NucleoL073.
 *
 * \example   periodic-uplink-lpp/NucleoL152/main.c
 *            LoRaWAN class A/B/C application example for the NucleoL152.
 *
 * \example   periodic-uplink-lpp/NucleoL476/main.c
 *            LoRaWAN class A/B/C application example for the NucleoL476.
 *
 * \example   periodic-uplink-lpp/SAMR34/main.c
 *            LoRaWAN class A/B/C application example for the SAMR34.
 *
 * \example   periodic-uplink-lpp/SKiM880B/main.c
 *            LoRaWAN class A/B/C application example for the SKiM880B.
 *
 * \example   periodic-uplink-lpp/SKiM881AXL/main.c
 *            LoRaWAN class A/B/C application example for the SKiM881AXL.
 *
 * \example   periodic-uplink-lpp/SKiM980A/main.c
 *            LoRaWAN class A/B/C application example for the SKiM980A.
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LoRaMac.h
  * @author  MCD Application Team
  * @brief   Header for LoRa MAC Layer
  ******************************************************************************
  */
#ifndef __LORAMAC_H__
#define __LORAMAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacInterfaces.h"

/*!
 * Maximum number of times the MAC layer tries to get an acknowledge.
 */
#define MAX_ACK_RETRIES                             8

/*!
 * Frame direction definition for up-link communications
 */
#define UP_LINK                                     0

/*!
 * Frame direction definition for down-link communications
 */
#define DOWN_LINK                                   1

/*!
 * LoRaMac MLME-Confirm queue length
 */
#define LORA_MAC_MLME_CONFIRM_QUEUE_LEN             5

/*!
 * Maximum MAC commands buffer size
 */
#define LORA_MAC_COMMAND_MAX_LENGTH                 128

/*!
 * Bitmap value
 */
#define LORAMAC_NVM_NOTIFY_FLAG_NONE                0x00

/*!
 * Bitmap value for the NVM group crypto.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_CRYPTO              0x01

/*!
 * Bitmap value for the NVM group MAC 1.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP1          0x02

/*!
 * Bitmap value for the NVM group MAC 2.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP2          0x04

/*!
 * Bitmap value for the NVM group secure element.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_SECURE_ELEMENT      0x08

/*!
 * Bitmap value for the NVM group 1 region.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP1       0x10

/*!
 * Bitmap value for the NVM group 2 region.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP2       0x20

/*!
 * Bitmap value for the NVM group class b.
 */
#define LORAMAC_NVM_NOTIFY_FLAG_CLASS_B             0x40

/*!
 * LoRaWAN compliance certification protocol port number.
 *
 * LoRaWAN Specification V1.x.x, chapter 4.3.2
 */
#define LORAMAC_CERT_FPORT 224

/*!
 * \brief   LoRaMAC layer initialization
 *
 * \details In addition to the initialization of the LoRaMAC layer, this
 *          function initializes the callback primitives of the MCPS and
 *          MLME services. Every data field of \ref LoRaMacPrimitives_t must be
 *          set to a valid callback function.
 *
 * \param   [in] primitives - Pointer to a structure defining the LoRaMAC
 *                            event functions. Refer to \ref LoRaMacPrimitives_t.
 *
 * \param   [in] callbacks  - Pointer to a structure defining the LoRaMAC
 *                            callback functions. Refer to \ref LoRaMacCallback_t.
 *
 * \param   [in] region     - The region to start.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_REGION_NOT_SUPPORTED.
 */
LoRaMacStatus_t LoRaMacInitialization( LoRaMacPrimitives_t* primitives, LoRaMacCallback_t* callbacks, LoRaMacRegion_t region );

/*!
 * \brief   Starts LoRaMAC layer
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 */
LoRaMacStatus_t LoRaMacStart( void );

/*!
 * \brief   Stops LoRaMAC layer
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 */
LoRaMacStatus_t LoRaMacStop( void );

LoRaMacStatus_t LoRaMacHalt( void );

/*!
 * \brief Returns a value indicating if the MAC layer is busy or not.
 *
 * \retval isBusy Mac layer is busy.
 */
bool LoRaMacIsBusy( void );

/*!
 * \brief Returns a value indicating if the MAC layer is stopped
 *
 * \retval isStopped Mac layer is stopped.
 */
bool LoRaMacIsStopped( void );

/*!
 * Processes the LoRaMac events.
 *
 * \remark This function must be called in the main loop.
 */
void LoRaMacProcess( void );

/*!
 * \brief   Queries the LoRaMAC if it is possible to send the next frame with
 *          a given application data payload size. The LoRaMAC takes scheduled
 *          MAC commands into account and reports, when the frame can be send or not.
 *
 * \param   [in] size - Size of application data payload to be send next
 *
 * \param   [out] txInfo - The structure \ref LoRaMacTxInfo_t contains
 *                         information about the actual maximum payload possible
 *                         ( according to the configured datarate or the next
 *                         datarate according to ADR ), and the maximum frame
 *                         size, taking the scheduled MAC commands into account.
 *
 * \retval  LoRaMacStatus_t Status of the operation. When the parameters are
 *          not valid, the function returns \ref LORAMAC_STATUS_PARAMETER_INVALID.
 *          In case of a length error caused by the application data payload in combination
 *          with the MAC commands, the function returns \ref LORAMAC_STATUS_LENGTH_ERROR.
 *          In this case its recommended to send a frame without application data to flush
 *          the MAC commands. Otherwise the LoRaMAC will prioritize the MAC commands and
 *          if needed it will skip the application data. Please note that if MAC commands do
 *          not fit at all into the payload size on the related datarate, the LoRaMAC will
 *          automatically clip the MAC commands.
 *          In case the query is valid, and the LoRaMAC is able to send the frame,
 *          the function returns \ref LORAMAC_STATUS_OK.
 */
LoRaMacStatus_t LoRaMacQueryTxPossible( uint8_t size, LoRaMacTxInfo_t* txInfo );

/*!
 * \brief   LoRaMAC channel add service
 *
 * \details Adds a new channel to the channel list and activates the id in
 *          the channel mask. Please note that this functionality is not available
 *          on all regions. Information about allowed ranges are available at the LoRaWAN Regional Parameters V1.0.2rB
 *
 * \param   [in] id - Id of the channel.
 *
 * \param   [in] params - Channel parameters to set.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacChannelAdd( uint8_t id, ChannelParams_t params );

/*!
 * \brief   LoRaMAC channel remove service
 *
 * \details Deactivates the id in the channel mask.
 *
 * \param   [in] id - Id of the channel.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacChannelRemove( uint8_t id );

/*!
 * \brief   LoRaMAC multicast channel setup service
 *
 * \details Sets up a multicast channel.
 *
 * \param   [in] channel - Multicast channel to set.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelSetup( McChannelParams_t *channel );

/*!
 * \brief   LoRaMAC multicast channel removal service
 *
 * \details Removes/Disables a multicast channel.
 *
 * \param   [in] groupID - Multicast channel ID to be removed/disabled
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelDelete( AddressIdentifier_t groupID );

/*!
 * \brief   LoRaMAC multicast channel get groupId from MC address.
 *
 * \param   [in]  mcAddress - Multicast address to be checked
 *
 * \retval  groupID           Multicast channel ID associated to the address.
 *                            Returns 0xFF if the address isn't found.
 */
uint8_t LoRaMacMcChannelGetGroupId( uint32_t mcAddress );

/*!
 * \brief   LoRaMAC multicast channel Rx parameters setup service
 *
 * \details Sets up a multicast channel reception parameters.
 *
 * \param   [in]  groupID  - Multicast channel ID
 * \param   [in]  rxParams - Reception parameters
 * \param   [out] status   - Status mask [UNDEF_ID | FREQ_ERR | DR_ERR | GROUP_ID]
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelSetupRxParams( AddressIdentifier_t groupID, McRxParams_t *rxParams, uint8_t *status );

/*!
 * \brief   LoRaMAC MIB-Get
 *
 * \details The mac information base service to get attributes of the LoRaMac
 *          layer.
 *
 *          The following code-snippet shows how to use the API to get the
 *          parameter AdrEnable, defined by the enumeration type
 *          \ref MIB_ADR.
 * \code
 * MibRequestConfirm_t mibReq;
 * mibReq.Type = MIB_ADR;
 *
 * if( LoRaMacMibGetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
 * {
 *   // LoRaMAC updated the parameter mibParam.AdrEnable
 * }
 * \endcode
 *
 * \param   [in] mibGet - MIB-GET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMibGetRequestConfirm( MibRequestConfirm_t* mibGet );

/*!
 * \brief   LoRaMAC MIB-Set
 *
 * \details The mac information base service to set attributes of the LoRaMac
 *          layer.
 *
 *          The following code-snippet shows how to use the API to set the
 *          parameter AdrEnable, defined by the enumeration type
 *          \ref MIB_ADR.
 *
 * \code
 * MibRequestConfirm_t mibReq;
 * mibReq.Type = MIB_ADR;
 * mibReq.Param.AdrEnable = true;
 *
 * if( LoRaMacMibGetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
 * {
 *   // LoRaMAC updated the parameter
 * }
 * \endcode
 *
 * \param   [in] mibSet - MIB-SET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMibSetRequestConfirm( MibRequestConfirm_t* mibSet );

/*!
 * \brief   LoRaMAC MLME-Request
 *
 * \details The Mac layer management entity handles management services. The
 *          following code-snippet shows how to use the API to perform a
 *          network join request. Please note that for a join request, the
 *          DevEUI and the JoinEUI must be set previously via the MIB. Please
 *          also refer to the sample implementations.
 *
 * \code
 *
 * MlmeReq_t mlmeReq;
 * mlmeReq.Type = MLME_JOIN;
 * mlmeReq.Req.Join.Datarate = LORAWAN_DEFAULT_DATARATE;
 *
 * if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
 * {
 *   // Service started successfully. Waiting for the Mlme-Confirm event
 * }
 * \endcode
 *
 * \param   [in] mlmeRequest - MLME-Request to perform. Refer to \ref MlmeReq_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_NO_NETWORK_JOINED,
 *          \ref LORAMAC_STATUS_LENGTH_ERROR,
 */
LoRaMacStatus_t LoRaMacMlmeRequest( MlmeReq_t* mlmeRequest );

/*!
 * \brief   LoRaMAC MCPS-Request
 *
 * \details The Mac Common Part Sublayer handles data services. The following
 *          code-snippet shows how to use the API to send an unconfirmed
 *          LoRaMAC frame.
 *
 * \code
 * uint8_t myBuffer[] = { 1, 2, 3 };
 *
 * McpsReq_t mcpsReq;
 * mcpsReq.Type = MCPS_UNCONFIRMED;
 * mcpsReq.Req.Unconfirmed.fPort = 1;
 * mcpsReq.Req.Unconfirmed.fBuffer = myBuffer;
 * mcpsReq.Req.Unconfirmed.fBufferSize = sizeof( myBuffer );
 *
 * if( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
 * {
 *   // Service started successfully. Waiting for the MCPS-Confirm event
 * }
 * \endcode
 *
 * \param   [in] mcpsRequest - MCPS-Request to perform. Refer to \ref McpsReq_t.
 * \param   [in] allowDelayedTx - When set to true, the frame will be delayed
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_NO_NETWORK_JOINED,
 *          \ref LORAMAC_STATUS_LENGTH_ERROR,
 */
LoRaMacStatus_t LoRaMacMcpsRequest( McpsReq_t* mcpsRequest, bool allowDelayedTx );

/*!
 * \brief   LoRaMAC deinitialization
 *
 * \details This function stops the timers, re-initializes MAC & regional parameters to default
 *          and sets radio into sleep state.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY
 */
LoRaMacStatus_t LoRaMacDeInitialization( void );

LoRaMacStatus_t LoRaMacProcessMicForDatablock( uint8_t *buffer, uint32_t size, uint16_t sessionCnt, uint8_t fragIndex, uint32_t descriptor, uint32_t *mic );

/*! \} defgroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_H__
