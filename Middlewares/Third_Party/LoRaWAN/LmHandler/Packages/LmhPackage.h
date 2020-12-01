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
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LmHandlerTypes.h"

/* Exported defines ----------------------------------------------------------*/
/*!
 * Maximum number of packages
 */
#define PKG_MAX_NUMBER                              5

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/*!
 * LoRaMac handler package parameters
 */
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
  void (*Init)(void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize);
  /*!
   * Returns the current package initialization status.
   *
   * \retval status Package initialization status
   *                [true: Initialized, false: Not initialized]
   */
  bool (*IsInitialized)(void);
  /*!
   * Returns the package operation status.
   *
   * \retval status Package operation status
   *                [true: Running, false: Not running]
   */
  bool (*IsRunning)(void);
  /*!
   * Processes the internal package events.
   */
  void (*Process)(void);
  /*!
   * Processes the MCSP Confirm
   *
   * \param [in] mcpsConfirm MCPS confirmation primitive data
   */
  void (*OnMcpsConfirmProcess)(McpsConfirm_t *mcpsConfirm);
  /*!
   * Processes the MCPS Indication
   *
   * \param [in] mcpsIndication     MCPS indication primitive data
   */
  void (*OnMcpsIndicationProcess)(McpsIndication_t *mcpsIndication);
  /*!
   * Processes the MLME Confirm
   *
   * \param [in] mlmeConfirm MLME confirmation primitive data
   */
  void (*OnMlmeConfirmProcess)(MlmeConfirm_t *mlmeConfirm);

  /*
   *=========================================================================
   * Below callbacks must be initialized in LmHandler initialization with
   * provideded LmHandlerSend and OnMacRequest functions
   *=========================================================================
   */

  /*!
  * \brief Join a LoRa Network in classA
  *
  * \note if the device is ABP, this is a pass through function
  *
  * \param [in] isOtaa Indicates which activation mode must be used
  */
  void (*OnJoinRequest)(ActivationType_t mode);
  /*!
   * \brief Instructs the MAC layer to send a ClassA uplink
   *
   * \param [in] appData Data to be sent
   * \param [in] isTxConfirmed Indicates if the uplink requires an acknowledgement
   *
   * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
   *                processed else \ref LORAMAC_HANDLER_ERROR
   */
  LmHandlerErrorStatus_t (*OnSendRequest)(LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed, TimerTime_t *nextTxIn, bool allowDelayedTx);
  /*!
  * \brief Requests network server time update
  *
  * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
  */
  LmHandlerErrorStatus_t (*OnDeviceTimeRequest)(void);
} LmhPackage_t;

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif /* __LMH_PACKAGE_H__ */
