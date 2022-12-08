/*!
 * \file      LmhPackage.h
 *
 * \brief     Defines the packages API
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
  * @file    LmhPackage.h
  * @author  MCD Application Team
  * @brief   Header for Package definition API
  ******************************************************************************
  */
#ifndef __LMH_PACKAGE_H__
#define __LMH_PACKAGE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LmHandlerTypes.h"
#include "LoRaMacVersion.h"

/*!
 * Maximum number of packages
 */
#define PKG_MAX_NUMBER                              5

typedef struct LmhPackage_s
{
    uint8_t Port;
    /*
     *=========================================================================
     * Below callbacks must be initialized in package variable declaration
     *=========================================================================
     */

    /*!
     * Initializes the package with provided parameters
     *
     * \param [in] params            Pointer to the package parameters
     * \param [in] dataBuffer        Pointer to main application buffer
     * \param [in] dataBufferMaxSize Main application buffer maximum size
     */
    void ( *Init )( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );
    /*!
     * Returns the current package initialization status.
     *
     * \retval status Package initialization status
     *                [true: Initialized, false: Not initialized]
     */
    bool ( *IsInitialized )( void );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    /*!
     * Returns the package operation status.
     *
     * \retval status Package operation status
     *                [true: Running, false: Not running]
     */
    bool ( *IsRunning )( void );
#endif /* LORAMAC_VERSION */
    /*!
     * Returns if a package transmission is pending or not.
     *
     * \retval status Package transmission status
     *                [true: pending, false: Not pending]
     */
    bool ( *IsTxPending )( void );
    /*!
     * Processes the internal package events.
     */
    void ( *Process )( void );
    /*!
     * Notify the LmHandler process through an event
     */
    void ( *OnPackageProcessEvent )( void );
    /*!
     * Processes the MCSP Confirm
     *
     * \param [in] mcpsConfirm MCPS confirmation primitive data
     */
    void ( *OnMcpsConfirmProcess )( McpsConfirm_t *mcpsConfirm );
    /*!
     * Processes the MCPS Indication
     *
     * \param [in] mcpsIndication     MCPS indication primitive data
     */
    void ( *OnMcpsIndicationProcess )( McpsIndication_t *mcpsIndication );
    /*!
     * Processes the MLME Confirm
     *
     * \param [in] mlmeConfirm MLME confirmation primitive data
     */
    void ( *OnMlmeConfirmProcess )( MlmeConfirm_t *mlmeConfirm );
    /*!
     * Processes the MLME Indication
     *
     * \param [in] mlmeIndication     MLME indication primitive data
     */
    void ( *OnMlmeIndicationProcess )( MlmeIndication_t *mlmeIndication );

    /*
     *=========================================================================
     * Below callbacks must be initialized in LmHandler initialization with
     * provided LmHandlerSend and OnMacRequest functions
     *=========================================================================
     */

    /*!
     * Join a LoRa Network in classA
     *
     * \note if the device is ABP, this is a pass through function
     *
     * \param [in] mode Indicates which activation mode must be used
     * \param [in] forceRejoin Flag to force the rejoin even if LoRaWAN context can be restored
     */
    void ( *OnJoinRequest )( ActivationType_t mode, bool forceRejoin );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    /*!
     * Instructs the MAC layer to send a ClassA uplink
     *
     * \param [IN] appData Data to be sent
     * \param [IN] isTxConfirmed Indicates if the uplink requires an acknowledgement
     *
     * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
     *                processed else \ref LORAMAC_HANDLER_ERROR
     */
    LmHandlerErrorStatus_t ( *OnSendRequest )( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed, bool allowDelayedTx );
#endif /* LORAMAC_VERSION */
    /*!
     * Requests network server time update
     *
     * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
     */
    LmHandlerErrorStatus_t ( *OnDeviceTimeRequest )( void );
    /*!
     * Notifies the upper layer that the system time has been updated.
     */
    void ( *OnSysTimeUpdate )( void );
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /*!
     * Request an NVIC Reset
     */
    void ( *OnSystemReset )( void );
#endif /* LORAMAC_VERSION */
} LmhPackage_t;

#ifdef __cplusplus
}
#endif

#endif /* __LMH_PACKAGE_H__ */
