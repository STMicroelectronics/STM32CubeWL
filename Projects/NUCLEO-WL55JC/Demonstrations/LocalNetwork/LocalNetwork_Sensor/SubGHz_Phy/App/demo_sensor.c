/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_sensor.c
  * @author  MCD Application Team
  * @brief   Concentrator behavior module for STM32WL Concentrator Demo.
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
#include <string.h>

#include "demo_utils.h"
#include "demo_radio_utils.h"
#include "demo_regions.h"
#include "demo_sensor.h"
#include "version.h"

#include "radio.h"
#include "stm32_timer.h"
#include "stm32_lpm.h"
#include "sys_app.h" /*APP_LOG*/
#include "platform.h"

#if SENS_SCAN_TESTING != 0
typedef struct
{
  UTIL_TIMER_Time_t ScanStart;
  UTIL_TIMER_Time_t CycleStart;
  UTIL_TIMER_Time_t CycleLength;
  uint32_t CntSuccess;
  uint32_t CntFail;
} SENS_Debug_t; /*Structure with temporary debugging variables*/
#endif /*SENS_SCAN_TESTING*/

typedef struct
{
#if SENS_SCAN_TESTING != 0
  SENS_Debug_t Debug;
#endif /*SENS_SCAN_TESTING*/
  UTIL_TIMER_Object_t TransmitTimer;    /**<Timer when it is time to transmit*/
  UTIL_TIMER_Object_t BeaconTimer;      /**<Timer when next beacon is expected*/
  UTIL_TIMER_Object_t SyncTimer;        /**<Timer when sync is expected*/
  UTIL_TIMER_Object_t ReceiveTimer;     /**<Timer for RX timeout when receiving Beacon or Sync*/
  UTIL_TIMER_Object_t CadTimer;         /**<Timer for CAD timeout when scanning*/
  const DEMO_Region_t *Region;  /**<Pointer to tuned region*/
  uint32_t RegionNr;            /**<Number of tuned region*/
  const DEMO_Subregion_t *Subregion;  /**<Pointer to tuned subregion*/
  void (*GetDataCallback)(void); /**<Callback called before transmission*/
  void (*ActivityCallback)(void); /**<Callback called on every activity to blink LEDs or something*/
  uint32_t SubregionNr;         /**<Number of tuned subregion*/
  uint32_t TxDutyCounter;       /**<Counts not-transmitting periods when duty-cycle is limited*/
  UTIL_TIMER_Time_t BeaconTime; /**<Time when last received beacon's period started*/
  uint16_t Occupied;            /**<Mask of occupied channels obtained in Sync*/
  DEMO_codings_t Codings;       /**<Communication parameters*/
  uint32_t Missed;              /**<Number of missed beacons*/
  uint8_t Seed;                 /**<Seed obtained in Beacon*/
  bool ReceivingBeacon;         /**<Receiver is on, waiting for the Beacon*/
  bool ReceivingSync;           /**<Receiver is on, waiting for the Sync*/
  SENS_State_t State;           /**<Sensor communication state*/
  union
  {
    uint8_t Packet[DEMO_SENSOR_MAX_PAYLOAD];    /**<Array to transmit*/
    struct
    {
      DEMO_packet_sensor_header_t Header;       /**<Packet header*/
      uint8_t Data[];   /**<Sensor measurements*/
    };
  };    /**<Packet to transmit*/
} SENS_Global_t;
static SENS_Global_t SENS; /*Global variables of this module*/

static void ReceiveBeacon(void *context);
static void ReceiveSync(void *context);
static void Transmit(void *context);
static void ReceiveTimeout(void *context);
static void CadTimeout(void *context);
static void StartScanning(void);
static void OneScan(void);
static void ChangeState(SENS_State_t State);
static bool ValidReceivedBeacon(DEMO_packet_beacon_t *Beacon);
static bool ValidReceivedSync(DEMO_packet_sync_t *Sync, uint32_t Size, uint32_t RadioRandom);
static void ProcessCodingChanges(DEMO_packet_sync_t *Sync, uint32_t Size);

static void TxDone(void);
static void TxTimeout(void);
static void RxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
static void RxTimeout(void);
static void RxError(void);
static void FhssChangeChannel(uint8_t currentChannel);
static void CadDone(bool channelActivityDetected);

/**
  * @brief Structure with handlers for radio events.
  */
static RadioEvents_t SENS_RadioEvents;


/* Function definitions ------------------------------------------------------*/


/**
  * @brief Initialize Sensor and start scanning for Beacon.
  * @param GetDataCallback called before transmission, quickly add data to transmit, do block in the callback
  * @param ActivityCallback called on every activity to blink LED or something
  */
int SENS_Init(void (*GetDataCallback)(void), void (*ActivityCallback)(void))
{
  UTILS_ENTER_CRITICAL_SECTION();

  /*Init Radio and its event callbacks (Not using linker initialized data)*/
  SENS_RadioEvents.TxDone = TxDone;
  SENS_RadioEvents.TxTimeout = TxTimeout;
  SENS_RadioEvents.RxDone = RxDone;
  SENS_RadioEvents.RxTimeout = RxTimeout;
  SENS_RadioEvents.RxError = RxError;
  SENS_RadioEvents.FhssChangeChannel = FhssChangeChannel;
  SENS_RadioEvents.CadDone = CadDone;
  Radio.Init(&SENS_RadioEvents);
  Radio.SetModem(MODEM_LORA);
  Radio.SetPublicNetwork(false);        /*Set to private network*/
  Radio.Standby();

  /*Get Eui and set packet header*/
#if SENS_EUI_SOURCE == 0
  SENS.Header.eui = LL_FLASH_GetUDN();  /*Get EUI from HW*/
#elif SENS_EUI_SOURCE == 1
  SENS.Header.eui = HAL_GetUIDw0() ^ HAL_GetUIDw1() ^ HAL_GetUIDw2();
#else   /*SENS_EUI_SOURCE*/
  SENS.Header.eui = Radio.Random();
#endif  /*SENS_EUI_SOURCE*/
  APP_LOG(TS_OFF, VLEVEL_L, "eui=0x%08x\r\n", SENS.Header.eui);        /*Print eui to console*/
  SENS.Header.packet_cnt = 0;
  SENS.Header.version_major = VERSION_MAJOR;
  SENS.Header.version_minor = VERSION_MINOR;

  /*Store callback and init variables*/
  SENS.GetDataCallback = GetDataCallback;
  SENS.ActivityCallback = ActivityCallback;
  SENS.BeaconTime = 0;
  SENS.ReceivingBeacon = false;
  SENS.ReceivingSync = false;

  /*Init radio timers*/
  UTIL_TIMER_Create(&SENS.BeaconTimer, 0, UTIL_TIMER_ONESHOT, ReceiveBeacon, NULL);
  UTIL_TIMER_Create(&SENS.SyncTimer, 0, UTIL_TIMER_ONESHOT, ReceiveSync, NULL);
  UTIL_TIMER_Create(&SENS.TransmitTimer, 0, UTIL_TIMER_ONESHOT, Transmit, NULL);
  UTIL_TIMER_Create(&SENS.ReceiveTimer, 0, UTIL_TIMER_ONESHOT, ReceiveTimeout, NULL);
  UTIL_TIMER_Create(&SENS.CadTimer, SENS_CAD_TIMEOUT, UTIL_TIMER_ONESHOT, CadTimeout, NULL);

  /*Start scanning*/
#if SENS_SCAN_TESTING != 0
  SENS.Debug.CntSuccess = 0;
  SENS.Debug.CntFail = 0;
#endif /*SENS_SCAN_TESTING*/
  StartScanning();

  UTILS_EXIT_CRITICAL_SECTION();
  return 0;
}

/**
  * @brief Store data to send in next packet.
  * @param Data structure
  */
void SENS_WriteSensorData(const DEMO_data_1_0_t *Data)
{
  APP_LOG(TS_ON, VLEVEL_M, "Sensor data written\r\n");

  /*In critical, because main could be half way copying when TX happens*/
  UTILS_ENTER_CRITICAL_SECTION();
  memcpy(SENS.Data, Data, sizeof(DEMO_data_1_0_t));  /*Store data*/
  UTILS_EXIT_CRITICAL_SECTION();
}

/**
  * @brief Get state of the sensor connection.
  * @return one of SENS_State_t
  */
SENS_State_t SENS_GetState(void)
{
  return SENS.State;
}

/**
  * @brief Start receiving for Beacon.
  * @param context not used
  */
static void ReceiveBeacon(void *context)
{
  if (SENS.State != SENS_STATE_Scan)
  {
    Radio.Standby();      /*Prepare for RX*/
    Radio.SetChannel(SENS.Region->beacon_freq); /*Frequency*/

    if (SENS.RegionNr == 1)
    {
      /*For FCC, force beacon at 500kHZ/SF12*/
      Radio.SetRxConfig(MODEM_LORA,
                        DEMO_BW_TO_SETTXCONFIG(BEACON_FCC_BW_500),     /*Bandwidth*/
                        BEACON_FCC_SF_12,     /*Spreading Factor*/
                        DEMO_DEFAULT_CR,  /*Code Rate*/
                        0,  /*Not used for LoRa*/
                        38, /*38 symbols of preamble*/
                        0,  /*Not used when continuous receive mode*/
                        1,  /*Fixed length*/
                        4,  /*Payload length*/
                        0,  /*Include CRC*/
                        0, 0, /*No hopping*/
                        0,  /*IQ not inverted*/
                        1); /*Continuous receive mode*/
    }
    else
    {
      Radio.SetRxConfig(MODEM_LORA,
                        DEMO_BW_TO_SETTXCONFIG(SENS.Region->beacon_bw),     /*Bandwidth*/
                        SENS.Region->beacon_sf,     /*Spreading Factor*/
                        DEMO_DEFAULT_CR,  /*Code Rate*/
                        0,  /*Not used for LoRa*/
                        38, /*38 symbols of preamble*/
                        0,  /*Not used when continuous receive mode*/
                        1,  /*Fixed length*/
                        4,  /*Payload length*/
                        0,  /*Include CRC*/
                        0, 0, /*No hopping*/
                        0,  /*IQ not inverted*/
                        1); /*Continuous receive mode*/
    }
  }
  Radio.Rx(0);

  SENS.ReceivingSync = false;
  SENS.ReceivingBeacon = true;

  if ((SENS.State == SENS_STATE_Connect)
      || (SENS.State == SENS_STATE_Sync))
  {
    /*Set to the latest time a Beacon can come*/
    UTIL_TIMER_Stop(&SENS.ReceiveTimer);
    UTIL_TIMER_SetPeriod(&SENS.ReceiveTimer, DEMO_MAX_END_OF_PACKET_RX + 2 * SENS.Missed * SENS_TIMING_TOLERANCE
                         + SENS_TIMING_PRERECEIVE + Radio.GetWakeupTime());    /*Add time of initialization to timeout*/
    APP_LOG(TS_ON, VLEVEL_M, "Receive for beacon\r\n");
  }
  else
  {
    /*Receive for whole period*/
    UTIL_TIMER_Stop(&SENS.ReceiveTimer);
    UTIL_TIMER_SetPeriod(&SENS.ReceiveTimer, SENS_LOST_DUTY_RX
                         + SENS_TIMING_PRERECEIVE + Radio.GetWakeupTime());    /*Add time of initialization to timeout*/
    APP_LOG(TS_ON, VLEVEL_L, "Receive for the whole period\r\n");
  }
  UTIL_TIMER_Start(&SENS.ReceiveTimer);

  /*Notify*/
  (*SENS.ActivityCallback)();
}

/**
  * @brief Start receiving for Sync.
  * @param context not used
  */
static void ReceiveSync(void *context)
{
  const DEMO_Channel_t *channel;

  Radio.Standby();      /*Prepare for RX*/

  if (SENS.Subregion->hopping == true)
  {
    /*Select one of available channels, depending only on seed*/
    channel = &(SENS.Subregion->channels[SENS.Seed % SENS.Subregion->channels_n]);
  }
  else
  {
    channel = &(SENS.Subregion->channels[0]);   /*Nonhopping subregions use the first channel for Sync*/
  }

  /*Set radio parameters*/
  Radio.SetChannel(channel->freq); /*Frequency*/
  Radio.SetRxConfig(MODEM_LORA,
                    DEMO_BW_TO_SETTXCONFIG(SENS.Region->beacon_bw),     /*Bandwidth*/
                    SENS.Region->beacon_sf,     /*Spreading Factor*/
                    DEMO_DEFAULT_CR,  /*Code Rate*/
                    0,  /*Not used for LoRa*/
                    8,  /*8 symbols of preamble*/
                    0,  /*Not used when continuous receive mode*/
                    0,  /*Variable length*/
                    0xff,  /*Payload length not used*/
                    1,  /*Include CRC*/
                    0, 0, /*No hopping*/
                    0,  /*IQ not inverted*/
                    1); /*Continuous receive mode*/
  Radio.Rx(0);
  SENS.ReceivingBeacon = false;
  SENS.ReceivingSync = true;
  /*Set to the latest time a packet can come*/
  UTIL_TIMER_Stop(&SENS.ReceiveTimer);
  UTIL_TIMER_SetPeriod(&SENS.ReceiveTimer, DEMO_MAX_END_OF_PACKET_RX
                       + SENS_TIMING_PRERECEIVE + Radio.GetWakeupTime());    /*Add time of initialization to timeout*/
  UTIL_TIMER_Start(&SENS.ReceiveTimer);

  /*Notify*/
  (*SENS.ActivityCallback)();
  APP_LOG(TS_ON, VLEVEL_M, "Receive for sync\r\n");
}

/**
  * @brief Start transmitting in sensor slot.
  * @param context not used
  * @note This function doesn't use Radio.SetTxConfig() because it allows only LoRaWAN modulation
  *    and it doesn't use Radio.Send() because it highly depends on SetTxConfig.
  */
static void Transmit(void *context)
{
  const DEMO_Channel_t *channel;      /*Pointer to channel used to transmit*/
  uint8_t ant_switch;
  PacketParams_t pck_params;
  ModulationParams_t mod_params;

  /*Application Callback*/
  if (SENS.GetDataCallback != NULL)
  {
    SENS.GetDataCallback();     /*Get new data to transmit*/
  }

  /*Select channel*/
  if (SENS.Subregion->hopping == true)
  {
    /*Get number of used channels*/
    uint32_t channel_count = SENS.Subregion->channels_n;
    /*Get the right number in pseudorandom sequence*/
    uint32_t pseudorandom = SENS.Seed;
    for (int i = 1; i < SENS.Codings.hdr.slot; i++)
    {
      pseudorandom = DEMO_PSEUDORANDOM_NEXT(pseudorandom);
    }

    /*Select channel for transmit slot*/
    channel = &(SENS.Subregion->channels[pseudorandom % channel_count]);
  }
  else
  {
    channel = &(SENS.Subregion->channels[SENS.Codings.hdr.chan_nr]);  /*Channel set by Concentrator*/
  }

  /*Prepare radio in standby*/
  Radio.Standby();
  Radio.SetChannel(channel->freq); /*Set the right frequency*/

  /*Listen Before Talk is required*/
  if (SENS.Subregion->lbt)
  {
    uint32_t packet_length;

    /*Calculate time on air*/
    if (SENS.Codings.hdr.mode == true) /*LoRa*/
    {
      packet_length = DEMO_TimeOnAirLora(&SENS.Codings.lora, SENS.Codings.hdr.data_lim);
    }
    else
    {
      packet_length = DEMO_TimeOnAirFsk(&SENS.Codings.fsk, SENS.Codings.hdr.data_lim);
    }

    if (DEMO_WaitForFreeChannel((SENS.Codings.hdr.mode == true) ? MODEM_LORA : MODEM_FSK, SENS.Subregion->lbt_rssi,
                                SENS.Subregion->lbt_time,
                                DEMO_MAX_END_OF_PACKET_TX - packet_length) == false)
    {
      Radio.Sleep();
      APP_LOG(TS_ON, VLEVEL_L, "Failed to transmit due to LBT\r\n");
      return; /*Sending failed*/
    }
  }

  /*Configure*/
  if (SENS.Codings.hdr.mode == true)  /*LoRa*/
  {
    mod_params.PacketType = PACKET_TYPE_LORA;
    mod_params.Params.LoRa.SpreadingFactor = DEMO_GetSpreadingFactor(&(SENS.Codings.lora));
    mod_params.Params.LoRa.Bandwidth = DEMO_GetBandwidth(&(SENS.Codings.lora));
    mod_params.Params.LoRa.CodingRate = DEMO_GetCoderate(&(SENS.Codings.lora));
    mod_params.Params.LoRa.LowDatarateOptimize = SENS.Codings.lora.de;

    pck_params.PacketType = PACKET_TYPE_LORA;
    if ((mod_params.Params.LoRa.SpreadingFactor == LORA_SF5) || (mod_params.Params.LoRa.SpreadingFactor == LORA_SF6))
    {
      pck_params.Params.LoRa.PreambleLength = 12;
    }
    else
    {
      pck_params.Params.LoRa.PreambleLength = 8;
    }
    pck_params.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;
    pck_params.Params.LoRa.PayloadLength = SENS.Codings.hdr.data_lim;
    pck_params.Params.LoRa.CrcMode = LORA_CRC_ON;
    pck_params.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;

    SUBGRF_SetPacketType(PACKET_TYPE_LORA);
    SUBGRF_SetModulationParams(&mod_params);
    SUBGRF_SetPacketParams(&pck_params);

    /*Set syncword*/
    uint8_t reg = (DEMO_SYNCWORD_LORA_SENS >> 8) & 0xFF;
    SUBGRF_WriteRegisters(REG_LR_SYNCWORD, &reg, 1);
    reg = DEMO_SYNCWORD_LORA_SENS & 0xFF;
    SUBGRF_WriteRegisters(REG_LR_SYNCWORD + 1, &reg, 1);

    ant_switch = SUBGRF_SetRfTxPower(channel->power);
  }
  else /*FSK*/
  {
    mod_params.PacketType = PACKET_TYPE_GFSK;
    mod_params.Params.Gfsk.BitRate = SENS.Codings.fsk.br;
    mod_params.Params.Gfsk.ModulationShaping = DEMO_GetShaping(&(SENS.Codings.fsk));
    mod_params.Params.Gfsk.Bandwidth = 0x1F;    /*Not used in TX*/
    mod_params.Params.Gfsk.Fdev = SENS.Codings.fsk.fdev;

    pck_params.PacketType = PACKET_TYPE_GFSK;
    pck_params.Params.Gfsk.PreambleLength = (4 << 3); /*Convert byte into bit*/
    pck_params.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
    pck_params.Params.Gfsk.SyncWordLength = (4 << 3); /*Convert byte into bit*/
    pck_params.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
    pck_params.Params.Gfsk.HeaderType = RADIO_PACKET_VARIABLE_LENGTH;
    pck_params.Params.Gfsk.PayloadLength = SENS.Codings.hdr.data_lim;
    pck_params.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
    pck_params.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

    SUBGRF_SetPacketType(PACKET_TYPE_GFSK);
    SUBGRF_SetModulationParams(&mod_params);
    SUBGRF_SetPacketParams(&pck_params);

    /*Set syncword*/
    uint8_t syncword[8] = DEMO_SYNCWORD_FSK_SENS;
    SUBGRF_SetSyncWord(syncword);
    SUBGRF_SetWhiteningSeed(0x01FF);

    uint8_t paSelect = RFO_LP;
    int32_t TxConfig = RBI_GetTxConfig();
    if (((TxConfig == RBI_CONF_RFO_LP_HP) && (channel->power > 15)) || (TxConfig == RBI_CONF_RFO_HP))
    {
      paSelect = RFO_HP;
    }
    SUBGRF_SetTxParams(paSelect, channel->power, DEMO_GetRampUp(&(SENS.Codings.fsk)));
    ant_switch = paSelect;
  }

  /*Set needed interrupts*/
  SUBGRF_SetDioIrqParams(IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                         IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                         IRQ_RADIO_NONE,
                         IRQ_RADIO_NONE);

  /*Set DBG pin PB13*/
#ifdef LET_SUBGHZ_MW_USING_PROBE_LINE2_PIN
  PROBE_GPIO_SET_LINE(PROBE_LINE2_PORT, PROBE_LINE2_PIN);
#endif /* LET_SUBGHZ_MW_USING_PROBE_LINE2_PIN */

  /*Send*/
  SUBGRF_SetSwitch(ant_switch, RFSWITCH_TX);  /*Set RF switch*/
  SUBGRF_SendPayload(SENS.Packet, SENS.Codings.hdr.data_lim, 0);
  SENS.Header.packet_cnt++;   /*Count number of sent packets*/

  APP_LOG(TS_ON, VLEVEL_M, "Tx at %d\r\n", channel->freq);

  /*Notification*/
  (*SENS.ActivityCallback)();
}

/**
  * @brief Timeout when planned receive was unsuccessful.
  * @param context not used
  */
static void ReceiveTimeout(void *context)
{
  if (SENS.State == SENS_STATE_Scan)
  {
    SENS.ReceivingBeacon = false;
    Radio.Standby();
    OneScan();     /*When scanning, scan again immediately*/
    return;
  }

  Radio.Sleep();


  if (SENS.ReceivingBeacon == true) /*Beacon was missed*/
  {
    SENS.ReceivingBeacon = false;

    if (SENS.State == SENS_STATE_Lost)  /*Beacon is lost, and wasn't found now*/
    {
      /*Try to find beacon again in X minutes*/
      UTIL_TIMER_Stop(&SENS.BeaconTimer);
      UTIL_TIMER_SetPeriod(&SENS.BeaconTimer, SENS_LOST_DUTY_SLEEP);
      UTIL_TIMER_Start(&SENS.BeaconTimer);
      APP_LOG(TS_ON, VLEVEL_L, "Sleep to save power\r\n");
    }
    else
    {
      SENS.Missed++;       /*Count missed beacons*/

      APP_LOG(TS_ON, VLEVEL_L, "Missed Beacon %u\r\n", SENS.Missed);

      if (SENS.Missed >= SENS_MAX_MISSED_TO_LOST)
      {
        SENS.Codings.hdr.slot = 0;   /*Reset slot*/
        SENS.TxDutyCounter = 0; /*Reset duty cycle*/
        ChangeState(SENS_STATE_Lost);     /*Too many misses, lost*/
        ReceiveBeacon(NULL);    /*Try to receive for one whole period*/
      }
      else
      {
        /*Start timer for next Beacon*/
        UTIL_TIMER_Stop(&SENS.BeaconTimer);
        UTIL_TIMER_SetPeriod(&SENS.BeaconTimer, (SENS.Missed + 1) * (DEMO_SLOT_NUMBER * 1000)
                             - SENS_TIMING_PRERECEIVE - Radio.GetWakeupTime()  /*Start the radio this earlier*/
                             - SENS.Missed * SENS_TIMING_TOLERANCE /*Subtract tolerance*/
                             + SENS.BeaconTime - UTIL_TIMER_GetCurrentTime()); /*Make the timer absolute to the last known period start*/
        UTIL_TIMER_Start(&SENS.BeaconTimer);
      }
    }
  }
  else if (SENS.ReceivingSync == true)  /*Sync was missed*/
  {
    SENS.ReceivingSync = false;
    APP_LOG(TS_ON, VLEVEL_L, "Missed Sync\r\n");
    /*Wait for next beacon, timer was already set*/
  }

  /*Notify*/
  (*SENS.ActivityCallback)();
}

/**
  * @brief CAD didn't finish in time.
  * @param context not used
  */
static void CadTimeout(void *context)
{
  /*Reinit radio*/
  Radio.Init(&SENS_RadioEvents);
  Radio.SetModem(MODEM_LORA);
  Radio.SetPublicNetwork(false);
  Radio.Standby();

  /*Scan again*/
  OneScan();
}

/**
  * @brief Start the scanning process.
  */
static void StartScanning(void)
{
  /*Init or reinit radio, without reinit CadTimeout() happens often*/
  Radio.Init(&SENS_RadioEvents);
  Radio.SetModem(MODEM_LORA);
  Radio.SetPublicNetwork(false);
  Radio.Standby();

  ChangeState(SENS_STATE_Scan);

  /*Disable STOP while scanning*/
  UTIL_LPM_SetStopMode(1 << CFG_LPM_SENS_Id, UTIL_LPM_DISABLE);

  /*Set Region to overflow, so the scan starts with Region 0*/
  SENS.RegionNr = DEMO_Regions_n;

#if SENS_SCAN_TESTING != 0
  SENS.Debug.ScanStart = UTIL_TIMER_GetCurrentTime();
#endif /*SENS_SCAN_TESTING*/

  /*Start CAD scanning*/
  OneScan();
}


/**
  * @brief Start scanning for a Beacon.
  */
static void OneScan(void)
{
  if (SENS.State != SENS_STATE_Scan)
  {
    return;
  }

  /*Check valid RegionNR or continue iterating*/
  SENS.RegionNr++;
  if (SENS.RegionNr >= DEMO_Regions_n)
  {
    SENS.RegionNr = 0;
#if SENS_SCAN_TESTING != 0
    UTIL_TIMER_Time_t now = UTIL_TIMER_GetCurrentTime();
    SENS.Debug.CycleLength = now - SENS.Debug.CycleStart;
    SENS.Debug.CycleStart = now;
#endif /*SENS_SCAN_TESTING*/
  }
  SENS.Region = &(DEMO_Regions[SENS.RegionNr]);
  SENS.SubregionNr = 0;
  SENS.Subregion = &(SENS.Region->subregions[SENS.SubregionNr]);

  /*Tune radio*/
  Radio.SetChannel(SENS.Region->beacon_freq); /*Frequency*/
  if (SENS.RegionNr == 1) /*FCC*/
  {
    /*For FCC, force beacon at 500kHZ/SF12*/
    Radio.SetRxConfig(MODEM_LORA,
                      DEMO_BW_TO_SETTXCONFIG(BEACON_FCC_BW_500),     /*Bandwidth*/
                      BEACON_FCC_SF_12,     /*Spreading Factor*/
                      DEMO_DEFAULT_CR,  /*Code Rate*/
                      0,  /*Not used for LoRa*/
                      38, /*38 symbols of preamble*/
                      0,  /*Not used when continuous receive mode*/
                      1,  /*Fixed length*/
                      4,  /*Payload length*/
                      0,  /*Include CRC*/
                      0, 0, /*No hopping*/
                      0,  /*IQ not inverted*/
                      1); /*Continuous receive mode*/
  }
  else
  {
    Radio.SetRxConfig(MODEM_LORA,
                      DEMO_BW_TO_SETTXCONFIG(SENS.Region->beacon_bw),     /*Bandwidth*/
                      SENS.Region->beacon_sf,     /*Spreading Factor*/
                      DEMO_DEFAULT_CR,  /*Code Rate*/
                      0,  /*Not used for LoRa*/
                      38, /*38 symbols of preamble*/
                      0,  /*Not used when continuous receive mode*/
                      1,  /*Fixed length*/
                      4,  /*Payload length*/
                      0,  /*Include CRC*/
                      0, 0, /*No hopping*/
                      0,  /*IQ not inverted*/
                      1); /*Continuous receive mode*/
  }
  /*Configure CAD*/
  SUBGRF_SetCadParams(LORA_CAD_02_SYMBOL,       /*Detect n symbols*/
                      SENS_SCAN_CadDetPeak,     /*Sensitivity settings*/
                      SENS_SCAN_CadDetMin,      /*Sensitivity settings*/
                      LORA_CAD_RX,      /*Receive after CAD was successful*/
                      100 * 64);  /*(n * 15.625) us timeout*/

  UTIL_TIMER_Start(&(SENS.CadTimer)); /*Start timeout*/
  Radio.StartCad();     /*Start Scanning*/
}

/**
  * @brief Change sensor communication state.
  * @param State new state
  */
static void ChangeState(SENS_State_t State)
{
  if (SENS.State != State)
  {
    SENS.State = State;
    APP_LOG(TS_ON, VLEVEL_L, "   ==== State: ");
    switch (State)
    {
      case SENS_STATE_Connect:
        APP_LOG(TS_OFF, VLEVEL_L, "Connected");
        break;
      case SENS_STATE_Lost:
        APP_LOG(TS_OFF, VLEVEL_L, "Lost");
        break;
      case SENS_STATE_Scan:
        APP_LOG(TS_OFF, VLEVEL_L, "Scan");
        break;
      case SENS_STATE_Sync:
        APP_LOG(TS_OFF, VLEVEL_L, "Sync");
        break;
      default:
        APP_LOG(TS_OFF, VLEVEL_L, "Dead");
        break;
    }
    APP_LOG(TS_OFF, VLEVEL_L, " ====\r\n");
  }
}

/**
  * @brief Process valid beacon packet.
  * @param Beacon 4 B beacon packet
  */
static bool ValidReceivedBeacon(DEMO_packet_beacon_t *Beacon)
{
  if ((Beacon->version_major != VERSION_MAJOR) /*Concentrator with the correct version*/
      || ((SENS.State == SENS_STATE_Connect) && (SENS.SubregionNr != Beacon->subregion)))      /*Check subregion*/
  {
    return false;       /*Reject this*/
  }

  /*Store time of the period start*/
  SENS.BeaconTime = UTIL_TIMER_GetCurrentTime() - Beacon->offset;

  /*Start timer for Sync*/
  UTIL_TIMER_Stop(&SENS.SyncTimer);
  UTIL_TIMER_SetPeriod(&SENS.SyncTimer, 1000 - Beacon->offset
                       - SENS_TIMING_PRERECEIVE - Radio.GetWakeupTime());
  UTIL_TIMER_Start(&SENS.SyncTimer);

  /*Start timer for next Beacon*/
  UTIL_TIMER_Stop(&SENS.BeaconTimer);
  UTIL_TIMER_SetPeriod(&SENS.BeaconTimer, DEMO_SLOT_NUMBER * 1000 - Beacon->offset
                       - SENS_TIMING_PRERECEIVE - Radio.GetWakeupTime());
  UTIL_TIMER_Start(&SENS.BeaconTimer);

  /*Print info*/
  APP_LOG(TS_ON, VLEVEL_M, "Beacon received, seed = 0x%02x, offset = %hu ms\r\n", Beacon->seed,
          Beacon->offset);     /*Print info*/

  /*Process Beacon and change state*/
  if ((SENS.State == SENS_STATE_Scan)
      || (SENS.State == SENS_STATE_Lost))
  {
    SENS.Codings.hdr.slot = 0;     /*Reset slot*/
    SENS.TxDutyCounter = 0; /*Reset duty cycle*/
    UTIL_LPM_SetStopMode(1 << CFG_LPM_SENS_Id, UTIL_LPM_ENABLE); /*Re-enable STOP mode*/
    ChangeState(SENS_STATE_Sync);     /*Change lower states to sync*/
  }
  if (SENS.State != SENS_STATE_Connect)
  {
    SENS.SubregionNr = Beacon->subregion; /*Store subregion*/
    SENS.Subregion = &(SENS.Region->subregions[Beacon->subregion]);

    /*Print subregion if it was changed*/
    APP_LOG(TS_ON, VLEVEL_M, "Subregion set to %i.%i - %s\r\n", SENS.RegionNr, SENS.SubregionNr, SENS.Subregion->name);
  }
  SENS.Seed = Beacon->seed;
  SENS.ReceivingBeacon = false;
  SENS.Missed = 0; /*Clear counter of missed beacons*/

  return true;
}

/**
  * @brief Process successfully received Sync
  * @param Sync pointer to Sync structure
  * @param Size number of bytes received
  * @param RadioRandom one random value obtained from the radio before it was put to sleep
  */
static bool ValidReceivedSync(DEMO_packet_sync_t *Sync, uint32_t Size, uint32_t RadioRandom)
{

  PROBE_GPIO_WRITE(PROBE_LINE3_PORT, PROBE_LINE3_PIN, GPIO_PIN_SET);
  PROBE_GPIO_WRITE(PROBE_LINE3_PORT, PROBE_LINE3_PIN, GPIO_PIN_RESET);

  if ((Sync->subregion != SENS.SubregionNr) /*Subregion must fit*/
      || (Sync->seed != SENS.Seed)    /*Seed must fit*/
      || (Sync->version_major != VERSION_MAJOR)) /*Only concentrator with the correct version*/
  {
    return false;
  }

  /*Store mask of occupied channels*/
  SENS.Occupied = Sync->occupied;

  /*Print info*/
  APP_LOG(TS_ON, VLEVEL_M, "Sync received, occupied = bs");
  char oc_str[17] = "..............\r\n";
  uint16_t oc_shift = Sync->occupied >> 2;
  for (int i = 0; i < (DEMO_SLOT_NUMBER - 2); oc_shift >>= 1, i++)
  {
    if (oc_shift & 0x1)
    {
      oc_str[i] = '0';
    }
  }
  APP_LOG(TS_OFF, VLEVEL_M, oc_str);

  /*Unconnect*/
  if ((SENS.State == SENS_STATE_Connect)
      && ((Sync->occupied & (0x1 << SENS.Codings.hdr.slot)) == 0))
  {
    SENS.TxDutyCounter = 0; /*Reset duty cycle*/
    ChangeState(SENS_STATE_Sync); /*Last used slot is now unoccupied, un-connected*/
  }
  else
  {
    ProcessCodingChanges(Sync, Size);       /*Parse coding changes or connect accept*/
  }


  /*Pick a random slot if not connected*/
  if (SENS.State != SENS_STATE_Connect)
  {
    /*Reset coding with the randomly selected slot (slot will be 0 if no free slot is available)*/
    DEMO_DefaultCoding(&(SENS.Codings), DEMO_ChooseRandomSlot(Sync->occupied, RadioRandom), SENS.Region);
    SENS.Missed = 0;
  }

  /*Start timer for transmit*/
  if (SENS.Codings.hdr.slot > 0) /*do not transmit if no free slot is available*/
  {
    if (SENS.TxDutyCounter == 0) /*Limit duty cycle*/
    {
      SENS.TxDutyCounter = SENS.Codings.hdr.period;

      /*Make the timer absolute to the period start*/
      UTIL_TIMER_Stop(&SENS.TransmitTimer);
      UTIL_TIMER_SetPeriod(&SENS.TransmitTimer,
                           SENS.Codings.hdr.slot * 1000 + SENS.BeaconTime - UTIL_TIMER_GetCurrentTime() - Radio.GetWakeupTime());
      UTIL_TIMER_Start(&SENS.TransmitTimer); /*Start timer for the transmission slot*/
    }
    else
    {
      SENS.TxDutyCounter--;
    }
  }

  return true;
}

/**
  * @brief Process coding changes and apply the first one with correct slot.
  * @param Sync received sync packet
  * @param Size number of bytes received
  */
static void ProcessCodingChanges(DEMO_packet_sync_t *Sync, uint32_t Size)
{
  DEMO_coding_combined_t combined;
  uint32_t Len = Size - 4;       /*Get number of bytes in codings*/

  for (uint32_t offset = 0; (offset + sizeof(DEMO_coding_combined_t)) <= Len;)  /*While there is enough data for header*/
  {
    memcpy(&combined, &(Sync->codings[offset]), sizeof(DEMO_coding_combined_t)); /*Copy header to parseable structure*/

    if (combined.connect.mark == DEMO_HDR_CONNECT_MARK)  /*Connect accept and slot control*/
    {
      offset += sizeof(DEMO_coding_connect_t);

      if (offset + sizeof(uint32_t) > Len)    /*No space left for eui*/
      {
        return;
      }

      uint32_t eui;
      memcpy(&eui, &(Sync->codings[offset]), sizeof(uint32_t));  /*Get eui*/

      if ((eui == SENS.Header.eui)
          && (combined.connect.slot > 1)
          && ((Sync->occupied & (1 << combined.connect.slot)) != 0))
      {
        SENS.Codings.hdr.slot = combined.connect.slot;
        APP_LOG(TS_ON, VLEVEL_M, "Connected to slot = %u\r\n", SENS.Codings.hdr.slot);
        ChangeState(SENS_STATE_Connect); /*Connected*/
      }

      offset += sizeof(uint32_t);
    }
    else        /*Coding change*/
    {
      offset += sizeof(DEMO_coding_hdr_t);

      if (combined.hdr.mode == true)     /*LoRa*/
      {
        if (offset + sizeof(DEMO_coding_lora_t) > Len)    /*No space left for the rest of the coding*/
        {
          return;
        }
        if ((combined.hdr.slot == SENS.Codings.hdr.slot)     /*Slot fits*/
            && (SENS.State == SENS_STATE_Connect))
        {
          /*Copy and check LoRa parameters*/
          SENS.Codings.hdr = combined.hdr;
          memcpy(&(SENS.Codings.lora), &(Sync->codings[offset]), sizeof(DEMO_coding_lora_t));
          DEMO_ValidateCodingHdr(&(SENS.Codings.hdr), SENS.Subregion);
          DEMO_ValidateCodingLora(&(SENS.Codings.lora));
          SENS.TxDutyCounter = 0;
          APP_LOG(TS_ON, VLEVEL_M, "Changed to LoRa f = %u Hz, ", SENS.Subregion->channels[SENS.Codings.hdr.chan_nr].freq);
          APP_LOG(TS_OFF, VLEVEL_M, "period = %u, data_lim = %u B\r\n", SENS.Codings.hdr.period, SENS.Codings.hdr.data_lim);
        }
        offset += sizeof(DEMO_coding_lora_t);
      }
      else    /*FSK*/
      {
        if (offset + sizeof(DEMO_coding_fsk_t) > Len)    /*No space left for the rest of the coding*/
        {
          return;
        }
        if ((combined.hdr.slot == SENS.Codings.hdr.slot)     /*Slot fits*/
            && (SENS.State == SENS_STATE_Connect))
        {
          /*Copy and check FSK parameters*/
          SENS.Codings.hdr = combined.hdr;
          memcpy(&(SENS.Codings.fsk), &(Sync->codings[offset]), sizeof(DEMO_coding_fsk_t));
          DEMO_ValidateCodingHdr(&(SENS.Codings.hdr), SENS.Subregion);
          DEMO_ValidateCodingFsk(&(SENS.Codings.fsk));
          SENS.TxDutyCounter = 0;
          APP_LOG(TS_ON, VLEVEL_M, "Changed to FSK f = %u Hz, ", SENS.Subregion->channels[SENS.Codings.hdr.chan_nr].freq);
          APP_LOG(TS_OFF, VLEVEL_M, "period = %u, data_lim = %u B\r\n", SENS.Codings.hdr.period, SENS.Codings.hdr.data_lim);
        }
        offset += sizeof(DEMO_coding_fsk_t);
      }
    }
  }
}

/*!
  * \brief  Tx Done callback.
  */
static void TxDone(void)
{
  /*Put radio to sleep*/
  Radio.Sleep();

  /*Notify*/
  (*SENS.ActivityCallback)();
  APP_LOG(TS_ON, VLEVEL_M, "Transmit done\r\n");
}

/*!
  * \brief  Tx Timeout callback.
  */
static void TxTimeout(void)
{
  /*Put radio to sleep*/
  Radio.Sleep();

  APP_LOG(TS_ON, VLEVEL_L, "Transmit timeout\r\n");
}

/*!
  * \brief Rx Done callback.
  *
  * \param [IN] payload Received buffer pointer
  * \param [IN] size    Received buffer size
  * \param [IN] rssi    RSSI value computed while receiving the frame [dBm]
  * \param [IN] snr     Raw SNR value given by the radio hardware
  *                     FSK : N/A ( set to 0 )
  *                     LoRa: SNR value in dB
  */
static void RxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  if (SENS.ReceivingBeacon == true) /*Receiving beacon*/
  {
    if (size != 4) /*Beacon must be 4 bytes*/
    {
      if (SENS.State == SENS_STATE_Scan)
      {
        OneScan(); /*Continue scanning*/
      }
      else
      {
        Radio.Rx(0); /*Continue receiving*/
      }
      return;
    }

    DEMO_packet_beacon_t beacon;
    memcpy(&beacon, payload, size);
    uint8_t bytesum = payload[0] + payload[1] + payload[2] + payload[3];

    if ((beacon.pattern != DEMO_BEACON_PATTERN) /*Pattern must fit*/
        || (((bytesum + (bytesum >> 4)) & 0xf) != 0)) /*Checksum doesn't fit*/
    {
      if (SENS.State == SENS_STATE_Scan)
      {
        OneScan(); /*Continue scanning*/
      }
      else
      {
        Radio.Rx(0); /*Continue receiving*/
      }
      return;
    }

    Radio.Sleep(); /*Turn off the radio*/

    /*Process Beacon*/
    if (ValidReceivedBeacon(&beacon) == true)
    {
#if SENS_SCAN_TESTING != 0
      SENS.Debug.CntSuccess++;
      UTIL_TIMER_Time_t now = UTIL_TIMER_GetCurrentTime();
      SENS.Debug.CntFail += ((now - SENS.Debug.ScanStart - 1000) / 16000);
      UTIL_TIMER_Stop(&(SENS.BeaconTimer));
      UTIL_TIMER_Stop(&(SENS.SyncTimer));
      APP_PRINTF("   Beacon OK, Success=%u, Fail=%u, Cycle=%u ms\r\n",
                 SENS.Debug.CntSuccess, SENS.Debug.CntFail, SENS.Debug.CycleLength);
      StartScanning();
#endif /*SENS_SCAN_TESTING*/
    }
    else        /*Wrong beacon*/
    {
      if (SENS.State == SENS_STATE_Scan)
      {
        OneScan(); /*Continue scanning*/
      }
      else
      {
        Radio.Rx(0); /*Continue receiving*/
      }
      return;
    }
  }
  else if ((SENS.ReceivingSync == true) /*Receiving Sync*/
           && ((SENS.State == SENS_STATE_Connect) || (SENS.State == SENS_STATE_Sync))) /*Only in sync*/
  {
    if (size < 4) /*Sync must be at least 4 bytes*/
    {
      Radio.Rx(0); /*Continue receiving*/
      return;
    }

    uint32_t radio_random = Radio.Random();     /*Get random number before going to sleep*/

    Radio.Sleep(); /*Turn off the radio*/

    DEMO_packet_sync_t sync;
    memcpy(&sync, payload, size);

    /*Process Sync*/
    if (ValidReceivedSync(&sync, size, radio_random) == true)
    {
      /*Sync was OK*/
    }
    else        /*Wrong Sync*/
    {
      Radio.Rx(0); /*Continue receiving*/
      return;
    }
  }

  UTIL_TIMER_Stop(&SENS.ReceiveTimer);      /*Stop receive timeout*/
  SENS.ReceivingBeacon = false;
  SENS.ReceivingSync = false;

  /*Notify*/
  (*SENS.ActivityCallback)();
}

/*!
  * \brief  Rx Timeout callback.
  */
static void RxTimeout(void)
{
  if (SENS.State == SENS_STATE_Scan)
  {
    Radio.Standby();
    OneScan();
  }
  else
  {
    APP_LOG(TS_ON, VLEVEL_L, "Receive timeout\r\n");
    Radio.Rx(0);
  }
}

/*!
  * \brief Rx Error callback.
  */
static void RxError(void)
{
  APP_LOG(TS_ON, VLEVEL_L, "Receive error\r\n");
}

/*!
  * \brief  FHSS Change Channel callback.
  *
  * \param [IN] currentChannel   Index number of the current channel
  */
static void FhssChangeChannel(uint8_t currentChannel)
{
  UNUSED(currentChannel);
}

/*!
  * \brief CAD Done callback.
  *
  * \param [IN] channelDetected    Channel Activity detected during the CAD
  */
static void CadDone(bool channelActivityDetected)
{
  UTIL_TIMER_Stop(&(SENS.CadTimer)); /*Stop timeout*/

  if (channelActivityDetected)
  {
    /*Receive Beacon*/
    SENS.ReceivingBeacon = true;

    Radio.Standby();
    Radio.Rx(0);

    /*Receive maximally for length of the beacon*/
    UTIL_TIMER_Stop(&SENS.ReceiveTimer);
    UTIL_TIMER_SetPeriod(&SENS.ReceiveTimer, SENS.Region->beacon_length + Radio.GetWakeupTime());
    UTIL_TIMER_Start(&SENS.ReceiveTimer);
    APP_LOG(TS_ON, VLEVEL_L, "CAD OK\r\n");
  }
  else
  {
    OneScan(); /*Scan again*/
  }

  /*Notify*/
  (*SENS.ActivityCallback)();
}

