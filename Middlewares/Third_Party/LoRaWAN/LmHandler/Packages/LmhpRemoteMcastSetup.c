/*!
 * \file      LmhpRemoteMcastSetup.c
 *
 * \brief     Implements the LoRa-Alliance remote multicast setup package
 *            Specification: https://lora-alliance.org/sites/default/files/2018-09/remote_multicast_setup_v1.0.0.pdf
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
  * @file    LmhpRemoteMcastSetup.c
  * @author  MCD Application Team
  * @brief   Remote Multicast Package definition
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "LmHandler.h"
#include "LmhpRemoteMcastSetup.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */

/* Private typedef -----------------------------------------------------------*/
typedef enum LmhpRemoteMcastSetupSessionStates_e
{
  REMOTE_MCAST_SETUP_SESSION_STATE_IDLE,
  REMOTE_MCAST_SETUP_SESSION_STATE_START,
  REMOTE_MCAST_SETUP_SESSION_STATE_STOP,
} LmhpRemoteMcastSetupSessionStates_t;

/*!
 * Package current context
 */
typedef struct LmhpRemoteMcastSetupState_s
{
  bool Initialized;
  bool IsRunning;
  LmhpRemoteMcastSetupSessionStates_t SessionState;
  uint8_t DataBufferMaxSize;
  uint8_t *DataBuffer;
} LmhpRemoteMcastSetupState_t;

typedef enum LmhpRemoteMcastSetupMoteCmd_e
{
  REMOTE_MCAST_SETUP_PKG_VERSION_ANS              = 0x00,
  REMOTE_MCAST_SETUP_MC_GROUP_STATUS_ANS          = 0x01,
  REMOTE_MCAST_SETUP_MC_GROUP_SETUP_ANS           = 0x02,
  REMOTE_MCAST_SETUP_MC_GROUP_DELETE_ANS          = 0x03,
  REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_ANS = 0x04,
  REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_ANS = 0x05,
} LmhpRemoteMcastSetupMoteCmd_t;

typedef enum LmhpRemoteMcastSetupSrvCmd_e
{
  REMOTE_MCAST_SETUP_PKG_VERSION_REQ              = 0x00,
  REMOTE_MCAST_SETUP_MC_GROUP_STATUS_REQ          = 0x01,
  REMOTE_MCAST_SETUP_MC_GROUP_SETUP_REQ           = 0x02,
  REMOTE_MCAST_SETUP_MC_GROUP_DELETE_REQ          = 0x03,
  REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_REQ = 0x04,
  REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_REQ = 0x05,
} LmhpRemoteMcastSetupSrvCmd_t;

typedef struct McGroupData_s
{
  uint8_t McGroupEnabled;
  union
  {
    uint8_t Value;
    struct
    {
      uint8_t McGroupId:   2;
      uint8_t RFU:         6;
    } Fields;
  } IdHeader;
  uint32_t McAddr;
  uint8_t McKeyEncrypted[16];
  uint32_t McFCountMin; /*! */
  uint32_t McFCountMax;
} McGroupData_t;

typedef enum eSessionState
{
  SESSION_STOPPED,
  SESSION_STARTED
} SessionState_t;

typedef struct McSessionData_s
{
  McGroupData_t McGroupData;
  SessionState_t SessionState;
  uint32_t SessionTime;
  uint8_t SessionTimeout;
  McRxParams_t RxParams;
} McSessionData_t;

/* Private define ------------------------------------------------------------*/
/*!
 * LoRaWAN Application Layer Remote multicast setup Specification
 */
#define REMOTE_MCAST_SETUP_PORT                     200
#define REMOTE_MCAST_SETUP_ID                       2
#define REMOTE_MCAST_SETUP_VERSION                  1

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*!
 * Initializes the package with provided parameters
 *
 * \param [in] params            Pointer to the package parameters
 * \param [in] dataBuffer        Pointer to main application buffer
 * \param [in] dataBufferMaxSize Main application buffer maximum size
 */
static void LmhpRemoteMcastSetupInit(void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize);

/*!
 * Returns the current package initialization status.
 *
 * \retval status Package initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpRemoteMcastSetupIsInitialized(void);

/*!
 * Returns the package operation status.
 *
 * \retval status Package operation status
 *                [true: Running, false: Not running]
 */
static bool LmhpRemoteMcastSetupIsRunning(void);

/*!
 * Processes the internal package events.
 */
static void LmhpRemoteMcastSetupProcess(void);

/*!
 * Processes the MCPS Indication
 *
 * \param [in] mcpsIndication     MCPS indication primitive data
 */
static void LmhpRemoteMcastSetupOnMcpsIndication(McpsIndication_t *mcpsIndication);

static void OnSessionStartTimer(void *context);

static void OnSessionStopTimer(void *context);

/* Private variables ---------------------------------------------------------*/
static LmhpRemoteMcastSetupState_t LmhpRemoteMcastSetupState =
{
  .Initialized = false,
  .IsRunning =   false,
  .SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE,
};

static McSessionData_t McSessionData[LORAMAC_MAX_MC_CTX];

/*!
 * Session start timer
 */
static TimerEvent_t SessionStartTimer;

/*!
 * Session start timer
 */
static TimerEvent_t SessionStopTimer;

static LmhPackage_t LmhpRemoteMcastSetupPackage =
{
  .Port =                       REMOTE_MCAST_SETUP_PORT,
  .Init =                       LmhpRemoteMcastSetupInit,
  .IsInitialized =              LmhpRemoteMcastSetupIsInitialized,
  .IsRunning =                  LmhpRemoteMcastSetupIsRunning,
  .Process =                    LmhpRemoteMcastSetupProcess,
  .OnMcpsConfirmProcess =       NULL,                              /* Not used in this package */
  .OnMcpsIndicationProcess =    LmhpRemoteMcastSetupOnMcpsIndication,
  .OnMlmeConfirmProcess =       NULL,                              /* Not used in this package */
  .OnJoinRequest =              NULL,                              /* To be initialized by LmHandler */
  .OnSendRequest =              NULL,                              /* To be initialized by LmHandler */
  .OnDeviceTimeRequest =        NULL,                              /* To be initialized by LmHandler */
};

/* Exported functions ---------------------------------------------------------*/
LmhPackage_t *LmhpRemoteMcastSetupPackageFactory(void)
{
  return &LmhpRemoteMcastSetupPackage;
}

/* Private  functions ---------------------------------------------------------*/
static void LmhpRemoteMcastSetupInit(void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize)
{
  if (dataBuffer != NULL)
  {
    LmhpRemoteMcastSetupState.DataBuffer = dataBuffer;
    LmhpRemoteMcastSetupState.DataBufferMaxSize = dataBufferMaxSize;
    LmhpRemoteMcastSetupState.Initialized = true;
    LmhpRemoteMcastSetupState.IsRunning = true;
    TimerInit(&SessionStartTimer, OnSessionStartTimer);
    TimerInit(&SessionStopTimer, OnSessionStopTimer);
  }
  else
  {
    LmhpRemoteMcastSetupState.IsRunning = false;
    LmhpRemoteMcastSetupState.Initialized = false;
  }

  for (uint8_t id = 0; id < LORAMAC_MAX_MC_CTX; id++)
  {
    McSessionData[id].McGroupData.McGroupEnabled = false;
  }
}

static bool LmhpRemoteMcastSetupIsInitialized(void)
{
  return LmhpRemoteMcastSetupState.Initialized;
}

static bool LmhpRemoteMcastSetupIsRunning(void)
{
  if (LmhpRemoteMcastSetupState.Initialized == false)
  {
    return false;
  }

  return LmhpRemoteMcastSetupState.IsRunning;
}

static void LmhpRemoteMcastSetupProcess(void)
{
  LmhpRemoteMcastSetupSessionStates_t state;

  CRITICAL_SECTION_BEGIN();
  state = LmhpRemoteMcastSetupState.SessionState;
  CRITICAL_SECTION_END();

  switch (state)
  {
    case REMOTE_MCAST_SETUP_SESSION_STATE_START:
      /* Switch to Class C */

      if (LmHandlerRequestClass(CLASS_C) == LORAMAC_HANDLER_SUCCESS)
      {
        LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE;

        TimerSetValue(&SessionStopTimer, (1 << McSessionData[0].SessionTimeout) * 1000);
        TimerStart(&SessionStopTimer);
      }
      break;
    case REMOTE_MCAST_SETUP_SESSION_STATE_STOP:
      /* Switch back to Class A */
      if (LmHandlerRequestClass(CLASS_A) == LORAMAC_HANDLER_SUCCESS)
      {
        LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE;
      }
      break;
    case REMOTE_MCAST_SETUP_SESSION_STATE_IDLE:
    /* Intentional fall through */
    default:
      /* Nothing to do. */
      break;
  }
}

static void LmhpRemoteMcastSetupOnMcpsIndication(McpsIndication_t *mcpsIndication)
{
  uint8_t cmdIndex = 0;
  uint8_t dataBufferIndex = 0;
  uint8_t id = 0xFF;

  while (cmdIndex < mcpsIndication->BufferSize)
  {
    switch (mcpsIndication->Buffer[cmdIndex++])
    {
      case REMOTE_MCAST_SETUP_PKG_VERSION_REQ:
      {
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_PKG_VERSION_ANS;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_ID;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_VERSION;
        break;
      }
      case REMOTE_MCAST_SETUP_MC_GROUP_STATUS_REQ:
      {
        uint8_t nbAvailableGroups = 0;
        uint8_t reqGroupMask = mcpsIndication->Buffer[cmdIndex++] & 0x0F;
        uint8_t AnsGroupMask = 0x00;

        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_STATUS_ANS;

        /* move index to the next first optional list */
        dataBufferIndex++;
        for (id = 0; id < LORAMAC_MAX_MC_CTX; id++)
        {
          if (McSessionData[id].McGroupData.McGroupEnabled)
          {
            nbAvailableGroups++;

            /* If multicast group defined in the input bit mask */
            if ((reqGroupMask & (1 << id)) != 0)
            {
              AnsGroupMask |= (1 << id);
              LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = id;
              LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (McSessionData[id].McGroupData.McAddr >> 0) & 0xFF;
              LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (McSessionData[id].McGroupData.McAddr >> 8) & 0xFF;
              LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (McSessionData[id].McGroupData.McAddr >> 16) & 0xFF;
              LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (McSessionData[id].McGroupData.McAddr >> 24) & 0xFF;
            }
          }
        }
        /* set the status bit */
        LmhpRemoteMcastSetupState.DataBuffer[1] = (nbAvailableGroups & 0x07) << 4 | (AnsGroupMask & 0x0F);

        break;
      }
      case REMOTE_MCAST_SETUP_MC_GROUP_SETUP_REQ:
      {
        uint8_t status = id = mcpsIndication->Buffer[cmdIndex++] & 0x03;
        McSessionData[id].McGroupData.IdHeader.Value = id;


        McSessionData[id].McGroupData.McAddr = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        McSessionData[id].McGroupData.McAddr += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        McSessionData[id].McGroupData.McAddr += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        McSessionData[id].McGroupData.McAddr += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        for (int8_t i = 0; i < 16; i++)
        {
          McSessionData[id].McGroupData.McKeyEncrypted[i] = mcpsIndication->Buffer[cmdIndex++];
        }

        McSessionData[id].McGroupData.McFCountMin = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        McSessionData[id].McGroupData.McFCountMin += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        McSessionData[id].McGroupData.McFCountMin += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        McSessionData[id].McGroupData.McFCountMin += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        McSessionData[id].McGroupData.McFCountMax = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        McSessionData[id].McGroupData.McFCountMax += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        McSessionData[id].McGroupData.McFCountMax += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        McSessionData[id].McGroupData.McFCountMax += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        McChannelParams_t channel =
        {
          .IsRemotelySetup = true,
          .Class = CLASS_C, /* Field not used for multicast channel setup. Must be initialized to something */
          .IsEnabled = true,
          .GroupID = (AddressIdentifier_t)McSessionData[id].McGroupData.IdHeader.Fields.McGroupId,
          .Address = McSessionData[id].McGroupData.McAddr,
          .McKeys.McKeyE = McSessionData[id].McGroupData.McKeyEncrypted,
          .FCountMin = McSessionData[id].McGroupData.McFCountMin,
          .FCountMax = McSessionData[id].McGroupData.McFCountMax,
          .RxParams.ClassC = /* Field not used for multicast channel setup. Must be initialized to something */
          {
            .Frequency = 0,
            .Datarate = 0
          }
        };
        if (LoRaMacMcChannelSetup(&channel) != LORAMAC_STATUS_OK)
        {
          status |= 0x04; /* IDerror bit */
        }
        else
        {
          McSessionData[id].McGroupData.McGroupEnabled = true;
        }

        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_SETUP_ANS;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;

        break;
      }
      case REMOTE_MCAST_SETUP_MC_GROUP_DELETE_REQ:
      {
        uint8_t status = id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

        McSessionData[id].McGroupData.IdHeader.Value = 0;
        McSessionData[id].McGroupData.McAddr = 0;
        UTIL_MEM_set_8(McSessionData[id].McGroupData.McKeyEncrypted, 0x00, 16);
        McSessionData[id].McGroupData.McFCountMin = 0;
        McSessionData[id].McGroupData.McFCountMax = 0;

        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_DELETE_ANS;

        if (LoRaMacMcChannelDelete((AddressIdentifier_t)id) != LORAMAC_STATUS_OK)
        {
          status |= 0x04; /* McGroupUndefined bit set */
        }
        else
        {
          McSessionData[id].McGroupData.McGroupEnabled = false;
        }
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;
        break;
      }
      case REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_REQ:
      {
        uint8_t status = 0x00;
        id = mcpsIndication->Buffer[cmdIndex++] & 0x03;
        int32_t timeToSessionStart = 0;

        McSessionData[id].SessionTime = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        McSessionData[id].SessionTime += (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        McSessionData[id].SessionTime += (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        McSessionData[id].SessionTime += (mcpsIndication->Buffer[cmdIndex++] << 24) & 0xFF000000;

        /* Add Unix to Gps epcoh offset. The system time is based on Unix time. */
        McSessionData[id].SessionTime += UNIX_GPS_EPOCH_OFFSET;

        McSessionData[id].SessionTimeout =  mcpsIndication->Buffer[cmdIndex++] & 0x0F;

        McSessionData[id].RxParams.ClassC.Frequency = (mcpsIndication->Buffer[cmdIndex++] << 0) & 0x000000FF;
        McSessionData[id].RxParams.ClassC.Frequency |= (mcpsIndication->Buffer[cmdIndex++] << 8) & 0x0000FF00;
        McSessionData[id].RxParams.ClassC.Frequency |= (mcpsIndication->Buffer[cmdIndex++] << 16) & 0x00FF0000;
        McSessionData[id].RxParams.ClassC.Frequency *= 100;

        McSessionData[id].RxParams.ClassC.Datarate = mcpsIndication->Buffer[cmdIndex++];

        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_ANS;
        if (LoRaMacMcChannelSetupRxParams((AddressIdentifier_t)id, &McSessionData[id].RxParams, &status) == LORAMAC_STATUS_OK)
        {
          SysTime_t curTime = { .Seconds = 0, .SubSeconds = 0 };
          curTime = SysTimeGet();

          if ((McSessionData[id].SessionTime - curTime.Seconds) > 0)
          {
            timeToSessionStart = McSessionData[id].SessionTime - curTime.Seconds;
            /* Start session start timer */
            TimerSetValue(&SessionStartTimer, timeToSessionStart * 1000);
            TimerStart(&SessionStartTimer);

            MW_LOG(TS_OFF, VLEVEL_M, "Time2SessionStart: %d ms\r\n", timeToSessionStart * 1000);
          }
          else
          {
            /* Session start time before current device time */
            status |= 0x20;
          }
        }
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (timeToSessionStart >> 0) & 0xFF;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (timeToSessionStart >> 8) & 0xFF;
        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = (timeToSessionStart >> 16) & 0xFF;
        break;
      }
      case REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_REQ:
      {
        /* implement command prosessing and handling */
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
      .Buffer = LmhpRemoteMcastSetupState.DataBuffer,
      .BufferSize = dataBufferIndex,
      .Port = REMOTE_MCAST_SETUP_PORT
    };

    bool current_dutycycle;
    LmHandlerGetDutyCycleEnable(&current_dutycycle);

    /* force Duty Cycle OFF to this Send */
    LmHandlerSetDutyCycleEnable(false);
    LmhpRemoteMcastSetupPackage.OnSendRequest(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, true);

    /* restore initial Duty Cycle */
    LmHandlerSetDutyCycleEnable(current_dutycycle);

    if (id != 0xFF && id < LORAMAC_MAX_MC_CTX)
    {
      MW_LOG(TS_OFF, VLEVEL_M, "ID          : %d\r\n", McSessionData[id].McGroupData.IdHeader.Fields.McGroupId);
      MW_LOG(TS_OFF, VLEVEL_M, "McAddr      : %08X\r\n", McSessionData[id].McGroupData.McAddr);
      MW_LOG(TS_OFF, VLEVEL_M, "McKey       : %02X", McSessionData[id].McGroupData.McKeyEncrypted[0]);
      for (int i = 1; i < 16; i++)
      {
        MW_LOG(TS_OFF, VLEVEL_M, "-%02X",  McSessionData[id].McGroupData.McKeyEncrypted[i]);
      }
      MW_LOG(TS_OFF, VLEVEL_M, "\r\n");
      MW_LOG(TS_OFF, VLEVEL_M, "McFCountMin : %u\r\n",  McSessionData[id].McGroupData.McFCountMin);
      MW_LOG(TS_OFF, VLEVEL_M, "McFCountMax : %u\r\n",  McSessionData[id].McGroupData.McFCountMax);
      MW_LOG(TS_OFF, VLEVEL_M, "SessionTime : %u\r\n",  McSessionData[id].SessionTime);
      MW_LOG(TS_OFF, VLEVEL_M, "SessionTimeT: %d s\r\n", (1 << McSessionData[id].SessionTimeout));
      MW_LOG(TS_OFF, VLEVEL_M, "Rx Freq     : %u\r\n", McSessionData[id].RxParams.ClassC.Frequency);
      MW_LOG(TS_OFF, VLEVEL_M, "Rx DR       : DR_%d\r\n", McSessionData[id].RxParams.ClassC.Datarate);
    }
  }
}

static void OnSessionStartTimer(void *context)
{
  TimerStop(&SessionStartTimer);
  LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_START;
  LmhpRemoteMcastSetupProcess();
}

static void OnSessionStopTimer(void *context)
{
  TimerStop(&SessionStopTimer);
  LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_STOP;
}
