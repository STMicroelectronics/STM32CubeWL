/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_concentrator.c
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
#include "demo_concentrator.h"
#include "demo_radio_utils.h"
#include "demo_report.h"
#include "version.h"
#include "sys_app.h" /*For LOG*/
#include "string.h"
#include "stm32_timer.h"
#include "stm32wlxx_ll_lptim.h"
#include "stm32wlxx_ll_rcc.h"
#include "stm32wlxx_ll_bus.h"

/* Private macros ------------------------------------------------------------*/

/**Convert LPTIM value to ms*/
#define CONC_LPTIM_TO_MS(lptim_cnt)     ((125*((uint32_t)(lptim_cnt))) >> 12)        /*time = (1000 * lptim / 2^15)*/

/**Trim the offset given in beacon*/
#define CONC_BEACON_OFFSET_TRIM         ((int32_t)0)    /*[ms]*/

/**
  *
  */
#define PRBS9_INIT ((uint16_t) 0x0055);

/**
  * @brief Max size of the data that can be received
  */
#define MAX_RECEIVED_DATA 255

/* Private global variables --------------------------------------------------*/

uint16_t prbs9_val;

typedef struct
{
  uint32_t Eui;                 /**<Device EUI (the lower 4 bytes)*/
  DEMO_codings_t Coding;        /**<Coding parameters*/
  bool CodingApplied;           /**<True when sensor responded with the new modulation*/
  bool ConnectVerified;         /**<True when sensor connect process was verified*/
  uint32_t Missed;              /**<Number of periods missed (not correcting for mod_hdr.period)*/
} CONC_Sensor_t;

typedef struct
{
  DEMO_packet_beacon_t Beacon;  /**<Beacon packet buffer*/
  DEMO_packet_sync_t Sync;      /**<Sync packet buffer*/
  UTIL_TIMER_Object_t LedTimer;        /**<Timer which turns LEDs off*/
  UTIL_TIMER_Object_t LedEffect;        /**<Timer which blink leds after other*/
  UTIL_TIMER_Object_t StartTimer;      /**<Timer which starts LPTIM at a random offset*/
  CONC_Sensor_t Sensors[DEMO_SLOT_NUMBER - 2]; /**<Storage for all connected sensors*/
  CONC_Sensor_t ShadowSensors[DEMO_SLOT_NUMBER - 2]; /**<Storage for planned coding changes*/
  uint32_t SlotCounter;         /**<Count slots, starting with 0=beacon and 1=sync*/
  uint32_t PeriodCounter;       /**<Counts periods from BEACON_ON*/
  uint32_t PseudorandomState;   /**<If hopping is used, it holds state of the generator*/
  uint32_t OccupiedNrLatch;     /**<Number of occupied slots sent in last Sync (not modified for predictable hopping)*/
  uint32_t BeaconLength;        /**<Length of the beacon packet [ms]*/
  const DEMO_Region_t *Region;          /**<Region number used*/
  const DEMO_Subregion_t *Subregion;    /**<Subregion number used*/
  bool Enable;                  /**<Set when beacon sending is enabled*/
  bool BeaconOk;                /**<Set if beacon was sent (not set if it failed due to LBT)*/
  bool SyncOk;                  /**<Set if sync was sent (not set if it failed due to LBT)*/
} CONC_Global_t;
static CONC_Global_t CONC; /*Global variables of this module*/

/* Private function declarations ---------------------------------------------*/

static void StartLPTIM(void *context);
static void LedsOff(void *context);
static void LedsEffect(void *context);
static void LedBlink(void);
static void SendBeacon(void);
static void SendSync(void);
static void Receive(void);
static void EvaluateLastPeriod(void);
static uint32_t FindEui(uint32_t Eui);
static uint32_t AddCodingChanges(uint8_t *Buf, uint32_t Len);
void LPTIM1_IRQHandler(void);
static void TxDone(void);
static void TxTimeout(void);
static void RxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
static void RxTimeout(void);
static void RxError(void);
static void FhssChangeChannel(uint8_t currentChannel);
static void CadDone(bool channelActivityDetected);
static void Prbs9Init(void);
static uint8_t Prbs9Next(void);

/**
  * @brief Structure with handlers for radio events.
  */
static RadioEvents_t CONC_RadioEvents;

/* Function definitions ------------------------------------------------------*/

/**
  * @brief Initialize Concentrator but don't send anything yet.
  * @return zero on success, 1 - LSE fault, 2 - ARR set fault
  */
int CONC_Init(void)
{
  int timeout;
  uint32_t random;

  UTILS_ENTER_CRITICAL_SECTION();

  /*Init Radio and its event callbacks (Not using linker initialized data)*/
  CONC_RadioEvents.TxDone = TxDone;
  CONC_RadioEvents.TxTimeout = TxTimeout;
  CONC_RadioEvents.RxDone = RxDone;
  CONC_RadioEvents.RxTimeout = RxTimeout;
  CONC_RadioEvents.RxError = RxError;
  CONC_RadioEvents.FhssChangeChannel = FhssChangeChannel;
  CONC_RadioEvents.CadDone = CadDone;
  Radio.Init(&CONC_RadioEvents);
  Radio.SetModem(MODEM_LORA);
  Radio.SetPublicNetwork(false);        /*Set to private network*/
  random = Radio.Random();
  Radio.Sleep();

  /*Reset variables*/
  CONC.BeaconOk = false;
  CONC.Enable = false;
  CONC.Region = NULL;
  CONC.Subregion = NULL;
  CONC.SlotCounter = random & 0xf; /*Select random slot to start with*/

  /*Init LED timer*/
  UTIL_TIMER_Create(&CONC.LedTimer, 125, UTIL_TIMER_ONESHOT, LedsOff, NULL);

  UTIL_TIMER_Create(&CONC.LedEffect, 125, UTIL_TIMER_ONESHOT, LedsEffect, NULL);
  /*Start Led Effect*/
  UTIL_TIMER_Start(&CONC.LedEffect);

  /*Enable LSE and LPTIM clock*/
  if (!LL_RCC_LSE_IsReady())
  {
    LL_RCC_LSE_Enable();
    timeout = 1000;
    while (!LL_RCC_LSE_IsReady())
    {
      if (timeout == 0)
      {
        return 1; /*LSE won't work*/
      }
    }
  }
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSE);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1);

  /*Reset LPTIM*/
  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPTIM1);
  HAL_Delay(1);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPTIM1);
  HAL_Delay(1);

  /*Init Beacon Timer to 1 s overflows*/
  LL_LPTIM_SetCounterMode(LPTIM1, LL_LPTIM_COUNTER_MODE_INTERNAL);
  LL_LPTIM_SetClockSource(LPTIM1, LL_LPTIM_CLK_SOURCE_INTERNAL);
  LL_LPTIM_SetPrescaler(LPTIM1, LL_LPTIM_PRESCALER_DIV1);
  LL_LPTIM_Enable(LPTIM1);
  LL_LPTIM_ClearFlag_ARROK(LPTIM1);
  LL_LPTIM_SetAutoReload(LPTIM1, 0x7fff); /*Overflow each second*/
  timeout = 1000;
  while (!LL_LPTIM_IsActiveFlag_ARROK(LPTIM1))
  {
    if (timeout == 0)
    {
      return 2; /*ARR register couldn't be set*/
    }
  }

  Prbs9Init();

  /*Enable timer interrupt*/
  LL_LPTIM_EnableIT_ARRM(LPTIM1);
  NVIC_SetPriority(LPTIM1_IRQn, 0);     /*Needs to be the same priority as radio to prevent race conditions*/
  NVIC_EnableIRQ(LPTIM1_IRQn);

  /*Init and start the start timer*/
  UTIL_TIMER_Create(&CONC.StartTimer, random % 1000, UTIL_TIMER_ONESHOT, StartLPTIM, NULL);
  UTIL_TIMER_Start(&CONC.StartTimer);

  UTILS_EXIT_CRITICAL_SECTION();

  return 0;
}

/**
  * @brief Start LPTIM at a random time.
  */
static void StartLPTIM(void *context)
{
  UNUSED(context);

  /*Start LPTIM*/
  LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
  /*Beacons are not sent yet, but the slot counting runs*/
}

/**
  * @brief Start sending beacons.
  * @param Region region number as index to CONC_Regions
  * @param Subregion subregion number as index to CONC_Regions[].subregions
  * @return zero on success
  */
int CONC_StartBeacon(uint32_t Region, uint32_t Subregion)
{
  if (CONC.Enable == true)
  {
    return 1; /*Already enabled*/
  }

  if (Region >= DEMO_Regions_n)
  {
    return 2; /*Region number is too high*/
  }

  if (Subregion >= DEMO_Regions[Region].subregions_n)
  {
    return 3; /*Subregion number is too high*/
  }
  /*stop Led effect*/
  UTIL_TIMER_Stop(&CONC.LedEffect);
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */

  /*Store Parameters*/
  CONC.Region = &(DEMO_Regions[Region]);
  CONC.Subregion = &(DEMO_Regions[Region].subregions[Subregion]);

  /*Prepare constant elements of Beacon and Sync*/
  CONC.Beacon.version_major = VERSION_MAJOR;
  CONC.Beacon.subregion = Subregion;
  CONC.Beacon.pattern = DEMO_BEACON_PATTERN;
  CONC.Sync.version_major = VERSION_MAJOR;
  CONC.Sync.subregion = Subregion;

  /*Reset all modulations and sensors*/
  CONC.PeriodCounter = 0;
  for (int i = 0; i < (DEMO_SLOT_NUMBER - 2); i++)
  {
    DEMO_DefaultCoding(&(CONC.Sensors[i].Coding), i + 2, CONC.Region);
    CONC.Sensors[i].CodingApplied = true;       /*Default coding is already applied*/
    CONC.Sensors[i].Eui = 0;
    CONC.Sensors[i].ConnectVerified = false;
    CONC.Sensors[i].Missed = 0;
    DEMO_DefaultCoding(&(CONC.ShadowSensors[i].Coding), i + 2, CONC.Region);
    CONC.ShadowSensors[i].CodingApplied = true;       /*Default coding is already applied*/
    CONC.ShadowSensors[i].Eui = 0;
    CONC.ShadowSensors[i].ConnectVerified = false;
    CONC.ShadowSensors[i].Missed = 0;
  }
  CONC.Sync.occupied = 0x2;     /*Sync is always occupied*/

  /*Start sending beacons*/
  CONC.Enable = true;
  return 0;
}

/**
  * @brief Stop sending beacons and forget all sensors.
  */
void CONC_StopBeacon(void)
{
  CONC.Enable = false; /*Stop sending beacons*/

  UTILS_ENTER_CRITICAL_SECTION();
  /*Set radio to sleep*/
  Radio.Sleep();
  UTILS_EXIT_CRITICAL_SECTION();
}

/**
  * @brief Get whether beacon sending is enabled.
  * @return true when beacons are sent
  */
bool CONC_IsEnabled(void)
{
  return CONC.Enable;
}

/**
  * @brief Change modulation for one sensor to LoRa.
  * @param eui device whose modulation is to change
  * @param param LoRa modulation parameters
  * @param test_only if true, only respond and do not use the modulation
  * @return CONC_SETMOD_Ok when successfully added the modulation, the modulation will change after a while
  */
CONC_SetModReturn_t CONC_SetModLora(uint32_t eui, const DEMO_coding_lora_t *param, bool test_only)
{
  uint32_t slot_nr;             /*Slot number of the eui given*/
  CONC_Sensor_t *shadow_sensor = NULL;       /*Sensor for eui given*/
  uint32_t channel_nr;          /*Selected channel number*/
  const DEMO_Channel_t *channel;      /*Iterator over available channels*/

  if (CONC.Enable == false)
  {
    return CONC_SETMOD_Fail;
  }

  if (test_only == false)
  {
    /*Find the sensor*/
    slot_nr = FindEui(eui);
    if (slot_nr == 0)
    {
      return CONC_SETMOD_EuiWrong;   /*Eui not found*/
    }
    shadow_sensor = &(CONC.ShadowSensors[slot_nr - 2]);        /*Get sensor structure*/
  }

  /*Check if the modulation is valid*/
  for (channel_nr = 0; (channel_nr < CONC.Subregion->channels_n) && (channel_nr <= DEMO_HDR_CHAN_NR_MAX); channel_nr++)
  {
    channel = &(CONC.Subregion->channels[channel_nr]);

    if (DEMO_BandwidthLora(param) <= channel->max_bw) /*Bandwidth fits*/
    {
      /*Calculate what is the maximal payload and how long is the packet*/
      uint32_t packet_time;
      uint32_t max_payload = DEMO_MaxPayloadLora(param, CONC.Subregion, &packet_time);
      if (max_payload >= sizeof(DEMO_packet_sensor_header_t))  /*Modulation allows to send at least header*/
      {
        /*Calculate how many periods must be skipped to comply with duty cycle*/
        uint32_t period = (packet_time * channel->duty_cycle) / (DEMO_SLOT_NUMBER * 1000);
        if (period <= DEMO_HDR_PARAM_PERIOD_MAX) /*Check that period is achievable*/
        {
          UTILS_ENTER_CRITICAL_SECTION();
          if (test_only == false)
          {
            /*Store the config*/
            shadow_sensor->Coding.hdr.data_lim = max_payload;
            shadow_sensor->Coding.hdr.mode = true;    /*LoRa*/
            shadow_sensor->Coding.hdr.period = period;
            shadow_sensor->Coding.hdr.chan_nr = channel_nr;
            shadow_sensor->Coding.lora = *param;      /*Copy LoRa parameters*/

            /*Validate config*/
            DEMO_ValidateCodingHdr(&(shadow_sensor->Coding.hdr), CONC.Subregion);
            DEMO_ValidateCodingLora(&(shadow_sensor->Coding.lora));

            /*Put the coding to be broadcast in Sync*/
            shadow_sensor->CodingApplied = false;
          }

          /*Print limitations, print probably needs to be in critical section*/
          if ((test_only == true) || (period > 0) || (max_payload < DEMO_SENSOR_MAX_PAYLOAD))
          {
            APP_PRINTF("AT+MOD_LIM=%u,%u\r\n", period, max_payload);
          }
          UTILS_EXIT_CRITICAL_SECTION();
          return CONC_SETMOD_Ok;
        }
      }
    }
  }

  return CONC_SETMOD_ModNotAllowed;     /*Didn't find viable channel*/
}

/**
  * @brief Change modulation for one sensor to FSK.
  * @param eui device whose modulation is to change
  * @param param FSK modulation parameters
  * @param test_only if true, only respond and do not use the modulation
  * @return CONC_SETMOD_Ok when successfully added the modulation, the modulation will change after a while
  */
CONC_SetModReturn_t CONC_SetModFSK(uint32_t eui, const DEMO_coding_fsk_t *param, bool test_only)
{
  uint32_t slot_nr;             /*Slot number of the eui given*/
  CONC_Sensor_t *shadow_sensor = NULL;       /*Sensor for eui given*/
  uint32_t channel_nr;          /*Selected channel number*/
  const DEMO_Channel_t *channel;      /*Pointer to iterate over channels*/

  if (test_only == false)
  {
    /*Find the sensor*/
    slot_nr = FindEui(eui);
    if (slot_nr == 0)
    {
      return CONC_SETMOD_EuiWrong;   /*Eui not found*/
    }
    shadow_sensor = &(CONC.ShadowSensors[slot_nr - 2]);        /*Get sensor structure*/
  }

  /*Check MSK limit*/
  if ((param->fdev * 4) < param->br)
  {
    return CONC_SETMOD_ModWrong;
  }

  /*Check if the modulation is valid*/
  for (channel_nr = 0; (channel_nr < CONC.Subregion->channels_n) && (channel_nr <= DEMO_HDR_CHAN_NR_MAX); channel_nr++)
  {
    channel = &(CONC.Subregion->channels[channel_nr]);  /*Iterate over channels*/

    if (DEMO_BandwidthFsk(param) <= channel->max_bw) /*Bandwidth fits*/
    {
      /*Calculate what is the maximal payload and how long is the packet*/
      uint32_t packet_time;
      uint32_t max_payload = DEMO_MaxPayloadFsk(param, CONC.Subregion, &packet_time);
      if (max_payload >= sizeof(DEMO_packet_sensor_header_t))     /*Modulation allows to send at least header*/
      {
        /*Calculate how many periods must be skipped to comply with duty cycle*/
        uint32_t period = (packet_time * channel->duty_cycle) / (DEMO_SLOT_NUMBER * 1000);
        if (period <= DEMO_HDR_PARAM_PERIOD_MAX) /*Check that period is achievable*/
        {
          UTILS_ENTER_CRITICAL_SECTION();
          if (test_only == false)
          {
            /*Store the config*/
            shadow_sensor->Coding.hdr.data_lim = max_payload;
            shadow_sensor->Coding.hdr.mode = false;    /*FSK*/
            shadow_sensor->Coding.hdr.period = period;
            shadow_sensor->Coding.hdr.chan_nr = channel_nr;
            shadow_sensor->Coding.fsk = *param;       /*Copy FSK parameters*/

            /*Validate config*/
            DEMO_ValidateCodingHdr(&(shadow_sensor->Coding.hdr), CONC.Subregion);
            DEMO_ValidateCodingFsk(&(shadow_sensor->Coding.fsk));

            /*Put the coding to be broadcast in Sync*/
            shadow_sensor->CodingApplied = false;
          }

          /*Print limitations, print probably needs to be in critical section*/
          if ((test_only == true) || (period > 0) || (max_payload < DEMO_SENSOR_MAX_PAYLOAD))
          {
            APP_PRINTF("AT+MOD_LIM=%u,%u\r\n", period, max_payload);
          }
          UTILS_EXIT_CRITICAL_SECTION();
          return CONC_SETMOD_Ok;
        }
      }
    }
  }

  return CONC_SETMOD_ModNotAllowed;     /*Didn't find viable channel*/
}

/**
  * @brief Turn all LEDs off.
  * Used as a timer callback.
  */
static void LedsOff(void *context)
{
  UNUSED(context);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
}

static void LedsEffect(void *context)
{
  UNUSED(context);
  if (1U == HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin)) /* LED_BLUE */
  {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); /* LED_GREEN */
  }
  else if (1U == HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin)) /* LED_GREEN */
  {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); /* LED_RED */
  }
  else if (1U == HAL_GPIO_ReadPin(LED3_GPIO_Port, LED3_Pin)) /* LED_RED */
  {
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
  }
  else
  {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
  }
  UTIL_TIMER_Start(&CONC.LedEffect);
}
/**
  * @brief Do a short blink with a LED.
  */
static void LedBlink(void)
{
  UTIL_TIMER_Start(&CONC.LedTimer);
}

/**
  * @brief Send beacon.
  */
static void SendBeacon(void)
{
  if (CONC.Subregion->hopping == true)
  {
    /*Generate seed,*/
    CONC.Beacon.seed = Prbs9Next();
    CONC.PseudorandomState = CONC.Beacon.seed;    /*And init pseudorandom state*/
  }
  else  /*Not used*/
  {
    CONC.Beacon.seed = 0;
    CONC.PseudorandomState = 0;
  }

  /*Set radio parameters*/
  Radio.SetChannel(CONC.Region->beacon_freq); /*Frequency*/
  Radio.SetModem(MODEM_LORA);
  if (CONC.Subregion->hopping == true)
  {
    /*beacon at 500kHz*/
    Radio.SetTxConfig(MODEM_LORA, /*Beacon is always LoRa*/
                      CONC.Subregion->beacon_power, /*Power*/
                      0, /*FSK parameter*/
                      DEMO_BW_TO_SETTXCONFIG(BEACON_FCC_BW_500), /*Bandwidth*/
                      BEACON_FCC_SF_12, /*Spreading factor*/
                      DEMO_DEFAULT_CR, /*Coderate 4/5*/
                      36, /*Preamble length*/
                      1, /*Fixed length*/
                      0, /*CRC not used*/
                      0, 0, /*No hopping*/
                      0, /*IQ not inverted*/
                      1000); /*Tx timeout is not needed*/
  }
  else
  {
    Radio.SetTxConfig(MODEM_LORA, /*Beacon is always LoRa*/
                      CONC.Subregion->beacon_power, /*Power*/
                      0, /*FSK parameter*/
                      DEMO_BW_TO_SETTXCONFIG(CONC.Region->beacon_bw), /*Bandwidth*/
                      CONC.Region->beacon_sf, /*Spreading factor*/
                      DEMO_DEFAULT_CR, /*Coderate 4/5*/
                      36, /*Preamble length*/
                      1, /*Fixed length*/
                      0, /*CRC not used*/
                      0, 0, /*No hopping*/
                      0, /*IQ not inverted*/
                      1000); /*Tx timeout is not needed*/
  }
  /*Listen Before Talk is required*/
  if (CONC.Subregion->lbt)
  {
    if (DEMO_WaitForFreeChannel(MODEM_LORA, CONC.Subregion->lbt_rssi, CONC.Subregion->lbt_time,
                                DEMO_MAX_END_OF_PACKET_TX - CONC.Region->beacon_length) == false)
    {
      Radio.Sleep();
      APP_LOG(TS_ON, VLEVEL_L, "Beacon transmission failed\n\r");
      return; /*Sending of Beacon failed*/
    }
  }

  /*Send*/
  CONC.Beacon.offset = (int32_t)CONC.Region->beacon_length      /*Mark end of the packet*/
                       + (int32_t)CONC_LPTIM_TO_MS(LL_LPTIM_GetCounter(LPTIM1))  /*Delay caused by config and LBT*/
                       + CONC_BEACON_OFFSET_TRIM;                                /*Configurable trim*/
  CONC.Beacon.sum = 0;  /*Set to 0 before summing bytes*/
  uint8_t bytesum = ((uint8_t *)&CONC.Beacon)[0] + ((uint8_t *)&CONC.Beacon)[1]
                    + ((uint8_t *)&CONC.Beacon)[2] + ((uint8_t *)&CONC.Beacon)[3];
  CONC.Beacon.sum = - ((bytesum + (bytesum >> 4)) & 0xf);   /* Add negative value of primitive checksum */

  Radio.Send((uint8_t *)&CONC.Beacon, 4); /*Send beacon*/

  EvaluateLastPeriod(); /*Check all sensors and update occupied mask before Sync*/

  APP_LOG(TS_ON, VLEVEL_H, "Beacon Tx at %d\n\r", (CONC.Region->beacon_freq));
  /*Notify*/
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); /* LED_RED */
}

/**
  * @brief Send sync.
  */
static void SendSync(void)
{
  const DEMO_Channel_t *channel;

  if (CONC.Subregion->hopping == true)
  {
    /*Select one of available channels, depending only on seed*/
    channel = &(CONC.Subregion->channels[CONC.Beacon.seed % CONC.Subregion->channels_n]);
  }
  else
  {
    channel = &(CONC.Subregion->channels[0]);   /*Nonhopping subregions use the first channel for Sync*/
  }


  /*Add coding changes*/
  uint32_t codings_len = AddCodingChanges(CONC.Sync.codings, DEMO_SYNC_CODINGS_LEN);

  /*Set radio parameters*/
  Radio.SetChannel(channel->freq); /*Frequency*/
  Radio.SetModem(MODEM_LORA);
  Radio.SetTxConfig(MODEM_LORA, /*Sync is always LoRa*/
                    channel->power, /*Power*/
                    0, /*FSK parameter*/
                    DEMO_BW_TO_SETTXCONFIG(CONC.Region->beacon_bw), /*Bandwidth*/
                    CONC.Region->beacon_sf, /*Spreading factor*/
                    DEMO_DEFAULT_CR, /*Coderate 4/5*/
                    8, /*Preamble length*/
                    0, /*Variable length*/
                    1, /*CRC is used*/
                    0, 0, /*No hopping*/
                    0, /*IQ not inverted*/
                    1000); /*Tx timeout is not needed*/

  /*Listen Before Talk is required*/
  if (CONC.Subregion->lbt)
  {
    /*Calculate time on air*/
    DEMO_coding_lora_t sync_coding = {.de = 0, .cr = 1, .bw = CONC.Region->beacon_bw, .sf = CONC.Region->beacon_sf};
    uint32_t packet_length = DEMO_TimeOnAirLora(&sync_coding, 4 + codings_len);

    if (DEMO_WaitForFreeChannel(MODEM_LORA, CONC.Subregion->lbt_rssi, CONC.Subregion->lbt_time,
                                DEMO_MAX_END_OF_PACKET_TX - packet_length) == false)
    {
      Radio.Sleep();
      APP_LOG(TS_ON, VLEVEL_L, "Sync transmission failed\n\r");
      return; /*Sending of Sync failed*/
    }
  }

  /*Send*/
  CONC.Sync.seed = CONC.Beacon.seed;    /*Repeat the seed*/
  Radio.Send((uint8_t *)&CONC.Sync, 4 + codings_len);

  /*Notify*/
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); /* LED_RED */

  APP_LOG(TS_ON, VLEVEL_H, "Sync Tx at %d\n\r", (channel->freq));
}

/**
  * @brief Set receiver and listen for sensor.
  * @note This function doesn't use Radio.SetRxConfig() and Radio.SetTxConfig() because it allows only LoRaWAN
  *    modulation and it doesn't use Radio.Rx() because it highly depends on SetRxConfig.
  */
static void Receive(void)
{
  const DEMO_Channel_t *channel;
  PacketParams_t pck_params;
  ModulationParams_t mod_params;
  CONC_Sensor_t *sensor = &(CONC.Sensors[CONC.SlotCounter - 2]);        /*Make the below code less confusing*/

  /*Select channel*/
  if (CONC.Subregion->hopping == true)
  {
    /*Get number of used channels*/
    uint32_t channel_count = CONC.Subregion->channels_n;       /*Limited by number of existing channels*/

    /*Generate next pseudorandom number*/
    CONC.PseudorandomState = DEMO_PSEUDORANDOM_NEXT(CONC.PseudorandomState);

    /*Select one of available channels*/
    channel = &(CONC.Subregion->channels[CONC.PseudorandomState % channel_count]);
  }
  else
  {
    channel = &(CONC.Subregion->channels[sensor->Coding.hdr.chan_nr]);  /*Use sensors channel*/
  }

  /*Prepare radio in standby*/
  Radio.Standby();
  Radio.SetChannel(channel->freq); /*Set the right frequency*/

  /*Configure*/
  if (sensor->Coding.hdr.mode == true)  /*LoRa*/
  {
    mod_params.PacketType = PACKET_TYPE_LORA;
    mod_params.Params.LoRa.SpreadingFactor = DEMO_GetSpreadingFactor(&(sensor->Coding.lora));
    mod_params.Params.LoRa.Bandwidth = DEMO_GetBandwidth(&(sensor->Coding.lora));
    mod_params.Params.LoRa.CodingRate = DEMO_GetCoderate(&(sensor->Coding.lora));
    mod_params.Params.LoRa.LowDatarateOptimize = sensor->Coding.lora.de;

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
    pck_params.Params.LoRa.PayloadLength = MAX_RECEIVED_DATA;
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
  }
  else /*FSK*/
  {
    mod_params.PacketType = PACKET_TYPE_GFSK;
    mod_params.Params.Gfsk.BitRate = sensor->Coding.fsk.br;
    mod_params.Params.Gfsk.ModulationShaping = DEMO_GetShaping(&(sensor->Coding.fsk));
    mod_params.Params.Gfsk.Bandwidth = DEMO_ConvertFSKBandwidth(DEMO_BandwidthFsk(&(sensor->Coding.fsk)));
    mod_params.Params.Gfsk.Fdev = sensor->Coding.fsk.fdev;

    pck_params.PacketType = PACKET_TYPE_GFSK;
    pck_params.Params.Gfsk.PreambleLength = (4 << 3); /*Convert byte into bit*/
    pck_params.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
    pck_params.Params.Gfsk.SyncWordLength = (4 << 3); /*Convert byte into bit*/
    pck_params.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
    pck_params.Params.Gfsk.HeaderType = RADIO_PACKET_VARIABLE_LENGTH;
    pck_params.Params.Gfsk.PayloadLength = MAX_RECEIVED_DATA;
    pck_params.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
    pck_params.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

    SUBGRF_SetPacketType(PACKET_TYPE_GFSK);
    SUBGRF_SetModulationParams(&mod_params);
    SUBGRF_SetPacketParams(&pck_params);

    /*Set syncword*/
    uint8_t syncword[8] = DEMO_SYNCWORD_FSK_SENS;
    SUBGRF_SetSyncWord(syncword);
    SUBGRF_SetWhiteningSeed(0x01FF);
  }

  /*Set needed interrupts*/
  SUBGRF_SetDioIrqParams(IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                         IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                         IRQ_RADIO_NONE,
                         IRQ_RADIO_NONE);

  /*Set DBG pin PB12*/
#ifdef LET_SUBGHZ_MW_USING_PROBE_LINE1_PIN
  PROBE_GPIO_SET_LINE(PROBE_LINE1_PORT, PROBE_LINE1_PIN);
#endif /* LET_SUBGHZ_MW_USING_PROBE_LINE1_PIN */

  /*Receive*/
  SUBGRF_SetSwitch(0, RFSWITCH_RX); /*Set RF switch*/
  SUBGRF_SetRx(0xFFFFFF);       /*Receive without timeout*/
}

/**
  * @brief Evaluate last period and update occupied mask.
  * Check all sensors and loose those who do not communicate.
  * Count occupied slots.
  * Apply coding changes for the Sync which will come just now.
  */
static void EvaluateLastPeriod(void)
{
  uint32_t slot_nr;             /*Slot number*/
  CONC_Sensor_t *sensor;        /*Iterate over connected sensors*/
  CONC_Sensor_t *shadow_sensor;        /*Pointer to a temporary storage of coding changes*/

  CONC.OccupiedNrLatch = 0;
  for (slot_nr = 2; slot_nr < DEMO_SLOT_NUMBER; slot_nr++)
  {
    if (CONC.Sync.occupied & (1 << slot_nr))    /*Slot is occupied*/
    {
      sensor = &(CONC.Sensors[slot_nr - 2]);    /*Iterate over connected sensors*/
      shadow_sensor = &(CONC.ShadowSensors[slot_nr - 2]);        /*Get respective shadow sensor*/

      if (sensor->Missed > (DEMO_SENSOR_MISS_LIMIT * (sensor->Coding.hdr.period + 1)))     /*Missed too many beacons*/
      {
        CONC.Sync.occupied &= ~(1 << slot_nr);  /*Kick the sensor out*/
        CONC_Report_LOST(sensor->Eui);  /*Report lost sensor*/
        DEMO_DefaultCoding(&(sensor->Coding), slot_nr, CONC.Region);       /*Clear sensor's structure*/
        sensor->CodingApplied = true;       /*Default coding is already applied*/
        sensor->Eui = 0;
        sensor->ConnectVerified = false;
        sensor->Missed = 0;
        DEMO_DefaultCoding(&(shadow_sensor->Coding), slot_nr, CONC.Region);       /*Clear shadow sensor's structure*/
        shadow_sensor->CodingApplied = true;       /*Default coding is already applied*/
        shadow_sensor->Eui = 0;
        shadow_sensor->ConnectVerified = false;
        shadow_sensor->Missed = 0;
      }
      else
      {
        CONC.OccupiedNrLatch++;      /*Count number of occupied slots*/
        sensor->Missed++; /*Increase missed counter now, it is cleared on each reception*/

        if (shadow_sensor->CodingApplied == false)       /*There is a modulation change to apply*/
        {
          /*Apply the new coding, to be sent soon in Sync*/
          sensor->Coding.hdr = shadow_sensor->Coding.hdr;
          sensor->Coding.fsk = shadow_sensor->Coding.fsk;
          sensor->Coding.lora = shadow_sensor->Coding.lora;
          sensor->CodingApplied = false;
          shadow_sensor->CodingApplied = true;
        }
      }
    }
  }

  /*Print periodic info*/
  APP_LOG(TS_ON, VLEVEL_M, "Period %u, occupied = bs", CONC.PeriodCounter++);
  char oc_str[17] = "..............\r\n";
  uint16_t oc_shift = CONC.Sync.occupied >> 2;
  for (int i = 0; i < (DEMO_SLOT_NUMBER - 2); oc_shift >>= 1, i++)
  {
    if (oc_shift & 0x1)
    {
      oc_str[i] = '0';
    }
  }
  APP_LOG(TS_OFF, VLEVEL_M, oc_str);
}

/**
  * @brief Find eui in list of connected sensors.
  * @param Eui sensor's address
  * @return slot on which the sensor is connected or 0 if it is unknown
  */
static uint32_t FindEui(uint32_t Eui)
{
  int i;

  for (i = 0; i < (DEMO_SLOT_NUMBER - 2); i++)
  {
    if ((CONC.Sync.occupied & (1 << (i + 2)))   /*This slot is occupied*/
        && (CONC.Sensors[i].Eui == Eui))        /*Eui fits*/
    {
      return i + 2;     /*Slot number is sensor location + 2*/
    }
  }

  return 0;
}

/**
  * @brief Add coding changes to a buffer.
  * @param buf where to write the changes
  * @param len available length
  * @return number of bytes used
  */
static uint32_t AddCodingChanges(uint8_t *Buf, uint32_t Len)
{
  uint32_t offset; /*Current offset in Buf*/
  uint32_t slot_nr; /*Slot index*/
  CONC_Sensor_t *sensor; /*Sensor iterator*/

  for (slot_nr = 2, offset = 0; slot_nr < DEMO_SLOT_NUMBER; slot_nr++)
  {
    if ((CONC.Sync.occupied & (1 << slot_nr)) == 0)
    {
      continue; /*Skip unused slots*/
    }

    sensor = &(CONC.Sensors[slot_nr - 2]); /*Iterate over sensors*/

    if (sensor->ConnectVerified == false)  /*Sensor needs to connect proper slot*/
    {
      if ((offset + sizeof(DEMO_coding_connect_t) + sizeof(uint32_t)) > Len)
      {
        break; /*No more codings will fit in*/
      }

      DEMO_coding_connect_t connect =
      {
        .mark = DEMO_HDR_CONNECT_MARK,
        .slot = sensor->Coding.hdr.slot,
      };
      memcpy(&(Buf[offset]), &connect, sizeof(DEMO_coding_connect_t));
      offset += sizeof(DEMO_coding_connect_t);
      memcpy(&(Buf[offset]), &(sensor->Eui), sizeof(uint32_t));
      offset += sizeof(uint32_t);
    }

    if (sensor->CodingApplied == false) /*Sensor needs coding change*/
    {
      if (sensor->Coding.hdr.mode == true) /*LoRa mode*/
      {
        if ((offset + sizeof(DEMO_coding_hdr_t) + sizeof(DEMO_coding_lora_t)) > Len)
        {
          break; /*No more codings will fit in*/
        }

        memcpy(&(Buf[offset]), &(sensor->Coding.hdr), sizeof(DEMO_coding_hdr_t));
        offset += sizeof(DEMO_coding_hdr_t);
        memcpy(&(Buf[offset]), &(sensor->Coding.lora), sizeof(DEMO_coding_lora_t));
        offset += sizeof(DEMO_coding_lora_t);
      }
      else /*FSK mode*/
      {
        if ((offset + sizeof(DEMO_coding_hdr_t) + sizeof(DEMO_coding_fsk_t)) > Len)
        {
          break; /*No more codings will fit in*/
        }

        memcpy(&(Buf[offset]), &(sensor->Coding.hdr), sizeof(DEMO_coding_hdr_t));
        offset += sizeof(DEMO_coding_hdr_t);
        memcpy(&(Buf[offset]), &(sensor->Coding.fsk), sizeof(DEMO_coding_fsk_t));
        offset += sizeof(DEMO_coding_fsk_t);
      }
    }
  }

  return offset;
}

/**
  * @brief LPTIM autoreload interrupt handler.
  * This is launched each second to mark start of a slot.
  */
void LPTIM1_IRQHandler(void)
{
  LL_LPTIM_ClearFLAG_ARRM(LPTIM1);

  /*Increment slot number*/
  if (CONC.SlotCounter == 15)
  {
    CONC.SlotCounter = 0;
    /*Start of period, clear OKs*/
    CONC.BeaconOk = false;
    CONC.SyncOk = false;

    PROBE_GPIO_WRITE(PROBE_LINE3_PORT, PROBE_LINE3_PIN, GPIO_PIN_SET);
    PROBE_GPIO_WRITE(PROBE_LINE3_PORT, PROBE_LINE3_PIN, GPIO_PIN_RESET);

  }
  else
  {
    CONC.SlotCounter++;
  }

  /*Do something when slot starts*/
  if (CONC.Enable)
  {
    if (CONC.SlotCounter == 0) /*Time to send Beacon*/
    {
      SendBeacon();
    }
    else if (CONC.SlotCounter == 1) /*Time to send Sync*/
    {
      if (CONC.BeaconOk == true)  /*No point sending Sync if Beacon wasn't sent*/
      {
        SendSync();
      }
    }
    else /*Receive for sensors*/
    {
      if (CONC.SyncOk == true)     /*No point receiving if Sync wasn't sent properly*/
      {
        Receive();
      }
      else
      {
        Radio.Sleep();
      }
    }
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); /* LED_GREEN */
    LedBlink();       /*Indicate slot start*/
  }
}

/*!
  * \brief  Tx Done callback prototype.
  */
static void TxDone(void)
{
  if (CONC.Enable == true)
  {
    /*Set radio to idle state and wait for next slot*/
    Radio.Standby();

    if (CONC.SlotCounter == 0)
    {
      CONC.BeaconOk = true; /*Beacon was sent successfully*/
    }
    else if (CONC.SlotCounter == 1)
    {
      CONC.SyncOk = true; /*Sync was sent successfully*/
    }
  }
  else
  {
    Radio.Sleep();
  }
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
}

/*!
  * \brief  Tx Timeout callback prototype.
  */
static void TxTimeout(void)
{
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
}

/*!
  * \brief Rx Done callback prototype.
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
  if (CONC.Enable == true)
  {
    /*Set radio to idle state and wait for next slot*/
    Radio.Standby();

    if (size < sizeof(DEMO_packet_sensor_header_t)) /*The packet is too short to contain anything*/
    {
      return;
    }
    if (size > DEMO_SENSOR_MAX_PAYLOAD)
    {
      size = DEMO_SENSOR_MAX_PAYLOAD;   /*Limit data to maximal size*/
    }

    DEMO_packet_sensor_header_t header;
    memcpy(&header, payload, sizeof(DEMO_packet_sensor_header_t)); /*Copy payload header to sensor's packet*/

    CONC_Sensor_t *sensor = &(CONC.Sensors[CONC.SlotCounter - 2]); /*Look up this slot's sensor*/

    if (CONC.Sync.occupied & (1 << CONC.SlotCounter)) /*Slot is already occupied*/
    {
      if (header.eui != sensor->Eui) /*Sensor address must fit*/
      {
        return;
      }

      if (sensor->ConnectVerified == false)
      {
        sensor->ConnectVerified = true;
      }
      if (sensor->CodingApplied == false)
      {
        sensor->CodingApplied = true; /*Modulation was successfully changed*/
        CONC_Report_MOD_OK(header.eui); /*Report to console*/
      }
      sensor->Missed = 0;
    }
    else /*New Connect*/
    {
      uint32_t already_in = FindEui(header.eui); /*Find if the same eui is already connected*/
      if (already_in >= 2) /*This sensor is already connected in different slot*/
      {
        CONC.Sensors[already_in - 2].ConnectVerified = false;      /*Switch the eui to that slot*/
        if (DEMO_IsCodingDefault(&(CONC.Sensors[already_in - 2].Coding), CONC.Region) != true)   /*Coding was changed*/
        {
          CONC.Sensors[already_in - 2].CodingApplied = false;   /*Reapply coding*/
        }
      }
      else      /*New sensor*/
      {
        CONC.Sync.occupied |= (1 << CONC.SlotCounter); /*Add to new position*/
        sensor->Eui = header.eui; /*Store eui*/
        sensor->ConnectVerified = false;   /*Connect the sensor*/
        sensor->CodingApplied = true;   /*Default coding is already applied*/
        sensor->Missed = 0;
      }
    }

    /*Report to console*/
    CONC_Report_RCV(&header, rssi, snr,
                    ((uint8_t *)payload) + sizeof(DEMO_packet_sensor_header_t),
                    size - sizeof(DEMO_packet_sensor_header_t));

    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
    LedBlink();
  }
  else
  {
    Radio.Sleep();
  }
}

/*!
  * \brief  Rx Timeout callback prototype.
  */
static void RxTimeout(void) {}

/*!
  * \brief Rx Error callback prototype.
  */
static void RxError(void) {}

/*!
  * \brief  FHSS Change Channel callback prototype.
  *
  * \param [IN] currentChannel   Index number of the current channel
  */
static void FhssChangeChannel(uint8_t currentChannel)
{
  UNUSED(currentChannel);
}

/*!
  * \brief CAD Done callback prototype.
  *
  * \param [IN] channelDetected    Channel Activity detected during the CAD
  */
static void CadDone(bool channelActivityDetected)
{
  UNUSED(channelActivityDetected);
}

/**
  * \brief Initialise the PRBS9 generator
  */
static void Prbs9Init(void)
{
  prbs9_val = PRBS9_INIT;
}

/**
  * \brief Returns the next number from PRBS9 generator
  *
  * \return new PRBS number
  */
static uint8_t Prbs9Next(void)
{
  int newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
  prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
  return (uint8_t)(prbs9_val & 0xFF);
}
