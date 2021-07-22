/**
  ******************************************************************************
  * @file    LmhpFirmwareManagement.c
  * @author  MCD Application Team
  * @brief   Implements the LoRa-Alliance Firmware Management package
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "LmHandler.h"
#include "LmhpFirmwareManagement.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */

/* Private typedef -----------------------------------------------------------*/
/*!
 * Package current context
 */
typedef struct LmhpFirmwareManagementState_s
{
  bool Initialized;
  bool IsRunning;
  uint8_t DataBufferMaxSize;
  uint8_t *DataBuffer;
} LmhpFirmwareManagementState_t;

typedef enum LmhpFirmwareManagementMoteCmd_e
{
  FW_MANAGEMENT_PKG_VERSION_ANS              = 0x00,
  FW_MANAGEMENT_DEV_VERSION_ANS              = 0x01,
  FW_MANAGEMENT_DEV_REBOOT_TIME_ANS          = 0x02,
  FW_MANAGEMENT_DEV_REBOOT_COUNTDOWN_ANS     = 0x03,
  FW_MANAGEMENT_DEV_UPGRADE_IMAGE_ANS        = 0x04,
  FW_MANAGEMENT_DEV_DELETE_IMAGE_ANS         = 0x05,
} LmhpFirmwareManagementMoteCmd_t;

typedef enum LmhpFirmwareManagementSrvCmd_e
{
  FW_MANAGEMENT_PKG_VERSION_REQ              = 0x00,
  FW_MANAGEMENT_DEV_VERSION_REQ              = 0x01,
  FW_MANAGEMENT_DEV_REBOOT_TIME_REQ          = 0x02,
  FW_MANAGEMENT_DEV_REBOOT_COUNTDOWN_REQ     = 0x03,
  FW_MANAGEMENT_DEV_UPGRADE_IMAGE_REQ        = 0x04,
  FW_MANAGEMENT_DEV_DELETE_IMAGE_REQ         = 0x05,
} LmhpFirmwareManagementSrvCmd_t;

typedef enum LmhpFirmwareManagementUpImageStatus_e
{
  FW_MANAGEMENT_NO_PRESENT_IMAGE             = 0x00,
  FW_MANAGEMENT_CORRUPTED_IMAGE              = 0x01,
  FW_MANAGEMENT_INCOMPATIBLE_IMAGE           = 0x02,
  FW_MANAGEMENT_VALID_IMAGE                  = 0x03,
} LmhpFirmwareManagementUpImageStatus_t;
/* Private define ------------------------------------------------------------*/
/*!
 * LoRaWAN Application Layer Remote multicast setup Specification
 */
#define FW_MANAGEMENT_PORT                          203
#define FW_MANAGEMENT_ID                            4
#define FW_MANAGEMENT_VERSION                       1
#define FW_VERSION                                  0x00000000 /* Not yet managed */
#define HW_VERSION                                  0x00000000 /* Not yet managed */

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*!
 * Initializes the package with provided parameters
 *
 * \param [in] params            Pointer to the package parameters
 * \param [in] dataBuffer        Pointer to main application buffer
 * \param [in] dataBufferMaxSize Main application buffer maximum size
 */
static void LmhpFirmwareManagementInit(void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize);

/*!
 * Returns the current package initialization status.
 *
 * \retval status Package initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpFirmwareManagementIsInitialized(void);

/*!
 * Returns the package operation status.
 *
 * \retval status Package operation status
 *                [true: Running, false: Not running]
 */
static bool LmhpFirmwareManagementIsRunning(void);

/*!
 * Processes the internal package events.
 */
static void LmhpFirmwareManagementProcess(void);

/*!
 * Processes the MCPS Indication
 *
 * \param [in] mcpsIndication     MCPS indication primitive data
 */
static void LmhpFirmwareManagementOnMcpsIndication(McpsIndication_t *mcpsIndication);

static void OnRebootTimer(void *context);

/* Private variables ---------------------------------------------------------*/
static LmhpFirmwareManagementState_t LmhpFirmwareManagementState =
{
  .Initialized = false,
  .IsRunning =   false,
};

static LmhPackage_t LmhpFirmwareManagementPackage =
{
  .Port =                       FW_MANAGEMENT_PORT,
  .Init =                       LmhpFirmwareManagementInit,
  .IsInitialized =              LmhpFirmwareManagementIsInitialized,
  .IsRunning =                  LmhpFirmwareManagementIsRunning,
  .Process =                    LmhpFirmwareManagementProcess,
  .OnMcpsConfirmProcess =       NULL,                              /* Not used in this package */
  .OnMcpsIndicationProcess =    LmhpFirmwareManagementOnMcpsIndication,
  .OnMlmeConfirmProcess =       NULL,                              /* Not used in this package */
  .OnJoinRequest =              NULL,                              /* To be initialized by LmHandler */
  .OnSendRequest =              NULL,                              /* To be initialized by LmHandler */
  .OnDeviceTimeRequest =        NULL,                              /* To be initialized by LmHandler */
  .OnPackageProcessEvent =      NULL,                              /* To be initialized by LmHandler */
};

/*!
 * Reboot timer
 */
static TimerEvent_t RebootTimer;

/* Exported functions ---------------------------------------------------------*/
LmhPackage_t *LmhpFirmwareManagementPackageFactory(void)
{
  return &LmhpFirmwareManagementPackage;
}

/* Private  functions ---------------------------------------------------------*/
static void LmhpFirmwareManagementInit(void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize)
{
  if (dataBuffer != NULL)
  {
    LmhpFirmwareManagementState.DataBuffer = dataBuffer;
    LmhpFirmwareManagementState.DataBufferMaxSize = dataBufferMaxSize;
    LmhpFirmwareManagementState.Initialized = true;
    LmhpFirmwareManagementState.IsRunning = true;
    TimerInit(&RebootTimer, OnRebootTimer);
  }
  else
  {
    LmhpFirmwareManagementState.IsRunning = false;
    LmhpFirmwareManagementState.Initialized = false;
  }
}

static bool LmhpFirmwareManagementIsInitialized(void)
{
  return LmhpFirmwareManagementState.Initialized;
}

static bool LmhpFirmwareManagementIsRunning(void)
{
  if (LmhpFirmwareManagementState.Initialized == false)
  {
    return false;
  }

  return LmhpFirmwareManagementState.IsRunning;
}

static void LmhpFirmwareManagementProcess(void)
{
  /* Not yet implemented */
}

static void LmhpFirmwareManagementOnMcpsIndication(McpsIndication_t *mcpsIndication)
{
  uint8_t cmdIndex = 0;
  uint8_t dataBufferIndex = 0;

  while (cmdIndex < mcpsIndication->BufferSize)
  {
    switch (mcpsIndication->Buffer[cmdIndex++])
    {
      case FW_MANAGEMENT_PKG_VERSION_REQ:
      {
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_PKG_VERSION_ANS;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_ID;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_VERSION;
        break;
      }
      case FW_MANAGEMENT_DEV_VERSION_REQ:
      {
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_DEV_VERSION_ANS;
        /* FW Version */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (FW_VERSION >> 0) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (FW_VERSION >> 8) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (FW_VERSION >> 16) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (FW_VERSION >> 24) & 0xFF;
        /* HW Version */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (HW_VERSION >> 0) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (HW_VERSION >> 8) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (HW_VERSION >> 16) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (HW_VERSION >> 24) & 0xFF;
        break;
      }
      case FW_MANAGEMENT_DEV_REBOOT_TIME_REQ:
      {
        uint32_t rebootTimeReq = 0;
        uint32_t rebootTimeAns = 0;
        rebootTimeReq  = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        rebootTimeReq += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        rebootTimeReq += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        rebootTimeReq += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        if (rebootTimeReq == 0)
        {
          NVIC_SystemReset();
        }
        else if (rebootTimeReq == 0xFFFFFFFF)
        {
          rebootTimeAns = rebootTimeReq;
          TimerStop(&RebootTimer);
        }
        else
        {
          SysTime_t curTime = { .Seconds = 0, .SubSeconds = 0 };
          curTime = SysTimeGet();

          rebootTimeAns = rebootTimeReq - curTime.Seconds;
          if (rebootTimeAns > 0)
          {
            /* Start session start timer */
            TimerSetValue(&RebootTimer, rebootTimeAns * 1000);
            TimerStart(&RebootTimer);
          }
        }

        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_DEV_REBOOT_TIME_ANS;
        /* FW Version */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootTimeAns >> 0) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootTimeAns >> 8) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootTimeAns >> 16) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootTimeAns >> 24) & 0xFF;

        break;
      }
      case FW_MANAGEMENT_DEV_REBOOT_COUNTDOWN_REQ:
      {
        uint32_t rebootCountdown = 0;
        rebootCountdown  = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        rebootCountdown += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        rebootCountdown += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;

        if (rebootCountdown == 0)
        {
          NVIC_SystemReset();
        }
        else if (rebootCountdown == 0xFFFFFF)
        {
          TimerStop(&RebootTimer);
        }
        else
        {
          if (rebootCountdown > 0)
          {
            /* Start session start timer */
            TimerSetValue(&RebootTimer, rebootCountdown * 1000);
            TimerStart(&RebootTimer);
          }
        }
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_DEV_REBOOT_COUNTDOWN_ANS;
        /* FW Version */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootCountdown >> 0) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootCountdown >> 8) & 0xFF;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (rebootCountdown >> 16) & 0xFF;
        break;
      }
      case FW_MANAGEMENT_DEV_UPGRADE_IMAGE_REQ:
      {
        uint32_t imageVersion = 0;
        uint8_t imageStatus = FW_MANAGEMENT_NO_PRESENT_IMAGE;
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_DEV_UPGRADE_IMAGE_ANS;
        /* No FW present */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = imageStatus & 0x03;

        if (imageStatus == FW_MANAGEMENT_VALID_IMAGE)
        {
          /* Next FW version (opt) */
          LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (imageVersion >> 0) & 0xFF;
          LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (imageVersion >> 8) & 0xFF;
          LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (imageVersion >> 16) & 0xFF;
          LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = (imageVersion >> 24) & 0xFF;
        }
        break;
        break;
      }
      case FW_MANAGEMENT_DEV_DELETE_IMAGE_REQ:
      {
        uint32_t firmwareVersion = 0;
        firmwareVersion  = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        firmwareVersion += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        firmwareVersion += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        firmwareVersion += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = FW_MANAGEMENT_DEV_DELETE_IMAGE_ANS;
        /* No valid image present */
        LmhpFirmwareManagementState.DataBuffer[dataBufferIndex++] = 0x01;
        break;
      }
      default:
      {
        break;
      }
    }
  }

  if (dataBufferIndex != 0)
  {
    /* Answer commands */
    LmHandlerAppData_t appData =
    {
      .Buffer = LmhpFirmwareManagementState.DataBuffer,
      .BufferSize = dataBufferIndex,
      .Port = FW_MANAGEMENT_PORT
    };
    bool current_dutycycle;
    LmHandlerGetDutyCycleEnable(&current_dutycycle);

    /* force Duty Cycle OFF to this Send */
    LmHandlerSetDutyCycleEnable(false);
    LmhpFirmwareManagementPackage.OnSendRequest(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, true);

    /* restore initial Duty Cycle */
    LmHandlerSetDutyCycleEnable(current_dutycycle);
  }
}

static void OnRebootTimer(void *context)
{
  NVIC_SystemReset();
}
