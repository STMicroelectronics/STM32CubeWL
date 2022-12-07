/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_report.c
  * @author  MCD Application Team
  * @brief   Report various things to printf console.
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
#include <stdbool.h>
#include "sys_app.h" /*For LOG*/
#include "version.h"
#include "demo_report.h"

/**Make value positive*/
#define CONC_ABS_INT(a)                 (((a) >= 0) ? (a) : (-1 * (a)))

static bool CheckAndUpdateLength(uint32_t *DataLen, uint32_t Size, bool ErrorAnyway, char *ErrorStr);
static void Report_1_0_RCV(const DEMO_data_1_0_t *Data, uint32_t DataLen);
static void Report_1_1_RCV(const DEMO_data_1_1_t *Data, uint32_t DataLen);

/**
  * @brief Report lost sensor to console.
  * @param Eui sensor's address
  */
void CONC_Report_LOST(uint32_t Eui)
{
  APP_PPRINTF("AT+LOST=0x%08x\r\n", Eui);
}

/**
  * @brief Report successful modulation change to console.
  * @param Eui sensor's address
  */
void CONC_Report_MOD_OK(uint32_t Eui)
{
  APP_PPRINTF("AT+MOD_OK=0x%08x\r\n", Eui);
}

/**
  * @brief Check whether DataLen can fit element of size and subtract.
  * @param DataLen remaining bytes in buffer
  * @param Size size of the element that should be in buffer
  * @param ErrorAnyway If set, print the error string anyway
  * @param ErrorStr string printed when element doesn't fit or if ErrorAnyway
  * @return true if OK, false if element couldn't fit
  */
static bool CheckAndUpdateLength(uint32_t *DataLen, uint32_t Size, bool ErrorAnyway, char *ErrorStr)
{
  if (*DataLen < Size) /*Not enough data*/
  {
    *DataLen = 0; /*Reset length and read nothing more*/
    APP_PPRINTF(ErrorStr); /*Print error*/
    return false;
  }
  else
  {
    *DataLen -= Size; /*Subtract element size*/

    if (ErrorAnyway) /*Force error*/
    {
      APP_PPRINTF(ErrorStr); /*Print error*/
      return false;
    }
    else
    {
      return true;
    }
  }
}

/**
  * @brief Report received packet to console.
  * @param Header sensor packet header
  * @param Rssi RSSI value computed while receiving the frame [dBm]
  * @param Snr raw SNR value given by the radio hardware [dB]
  * @param Data sensor's measured sensor data
  * @param DataLen length of data [byte]
  */
void CONC_Report_RCV(const DEMO_packet_sensor_header_t *Header,
                     int16_t Rssi, int8_t Snr,
                     const uint8_t *Data, uint32_t DataLen)
{
  if (Header->version_major != VERSION_MAJOR)
  {
    return;     /*Unknown major version*/
  }

  APP_PPRINTF("AT+RCV=0x%08x,0x%02x,%u:%u,%hi,%i,",
              Header->eui, Header->packet_cnt, Header->version_major, Header->version_minor, Rssi, Snr);

  switch (Header->version_minor)
  {
#if ((VERSION_MAJOR != 1) || (VERSION_MINOR != 0))     /*Reserved for future*/
#error "Version changed, check data format!"
    /**@note Keep older versions intact and add code to decipher and print the new version*/
    /**@note Everything is to be cleared on change of major version, such sensors cannot connect*/

    case VERSION_MINOR:     /*Version X:X*/
    {
      DEMO_data_X_X_t data;
      memcpy((uint8_t *)data, Data, DataLen);
      Report_X_X_RCV(&data, DataLen);
    }
#endif  /*Reserved for future*/
    case 0: /*v1:0*/
    {
      DEMO_data_1_0_t data;
      memcpy((uint8_t *)(&data), Data, DataLen); /*Copy the payload into parseable structure*/
      Report_1_0_RCV(&data, DataLen);   /*Print*/
      break;
    }
    case 1: /*v1:1*/
    {
      DEMO_data_1_1_t data;
      memcpy((uint8_t *)(&data), Data, DataLen); /*Copy the payload into parseable structure*/
      Report_1_1_RCV(&data, DataLen);   /*Print*/
      break;
    }
    default:    /*Unknown version*/
    {
      APP_PPRINTF("E(v%u:%u)\r\n", Header->version_major, Header->version_minor);
      break;
    }
  }
}

/**
  * @brief Report received packet with version 1:0 to console.
  * @param Data sensor's measured sensor data of version 1:0
  * @param DataLen length of data [byte]
  */
static void Report_1_0_RCV(const DEMO_data_1_0_t *Data, uint32_t DataLen)
{
  bool voltage_present = true;

  /*Temperature*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t),
                           Data->temperature == DEMO_DATA_1_0_ERROR_TEMPERATURE, "E,") == true)
  {
    APP_PPRINTF("%+hi.%02hu,", Data->temperature / 100, CONC_ABS_INT(Data->temperature % 100));
  }

  /*Voltage*/
  if (voltage_present == true)
  {
    APP_PPRINTF("%u.%02u\r\n", Data->voltage / 20, (Data->voltage % 20) * 5);
  }
  else
  {
    APP_PPRINTF("E\r\n");
  }
}

/**
  * @brief Report received packet with version 1:0 to console.
  * @param Data sensor's measured sensor data of version 1:0
  * @param DataLen length of data [byte]
  */
static void Report_1_1_RCV(const DEMO_data_1_1_t *Data, uint32_t DataLen)
{
  bool voltage_present = true;

  /*Temperature*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t),
                           Data->temperature == DEMO_DATA_1_1_ERROR_TEMPERATURE, "E,") == true)
  {
    APP_PPRINTF("%+hi.%02hu,", Data->temperature / 100, CONC_ABS_INT(Data->temperature % 100));
  }
  /*sensors data board*/
  /*Pressure*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t),
                           Data->pressure == DEMO_DATA_1_1_ERROR_PRESSURE, "E,") == true)
  {
    APP_PPRINTF("%hu.%01hu,", Data->pressure / 10, Data->pressure % 10);
  }

  /*Yaw Pitch Roll*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint32_t),
                           ((Data->yaw == DEMO_DATA_1_1_ERROR_YAW)
                            || (Data->pitch == DEMO_DATA_1_1_ERROR_PITCH)
                            || (Data->roll == DEMO_DATA_1_1_ERROR_ROLL)), "E:E:E,") == true)
  {
    APP_PPRINTF("%+hi:%+hi:%+hi,", Data->yaw, Data->pitch, Data->roll);
  }

  /*Humidity*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint8_t),
                           Data->humidity == DEMO_DATA_1_1_ERROR_HUMIDITY, "E,") == true)
  {
    APP_PPRINTF("%u.%01u,", Data->humidity / 2, 5 * (Data->humidity % 2));
  }

  /*Voltage, do not print now, keep it for the end*/
  if (DataLen < sizeof(uint8_t)) /*Not enough data*/
  {
    DataLen = 0; /*Reset length and read nothing more*/
    voltage_present = false;
  }
  else
  {
    DataLen -= sizeof(uint8_t); /*Subtract element size*/
    voltage_present = (Data->voltage != DEMO_DATA_1_1_ERROR_VOLTAGE);
  }

  /*Distance and Activity are in the same word*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t), false, "E,E,") == true)
  {
    if (Data->activity == DEMO_DATA_1_1_ERROR_ACTIVITY)
    {
      APP_PPRINTF("E,");
    }
    else
    {
      APP_PPRINTF("%hu,", Data->activity);
    }
    if (Data->distance == DEMO_DATA_1_1_ERROR_DISTANCE)
    {
      APP_PPRINTF("E,");
    }
    else
    {
      APP_PPRINTF("%hu,", Data->distance);
    }
  }

  /*Step count*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t), false, "E,") == true)
  {
    APP_PPRINTF("%hu,", Data->step_count);
  }

  /*Swipe count*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t), false, "E,") == true)
  {
    APP_PPRINTF("%hu,", Data->swipe_count);
  }

  /*Average acceleration*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t),
                           Data->acceleration_avg == DEMO_DATA_1_1_ERROR_ACCELERATION, "E,") == true)
  {
    APP_PPRINTF("%hu.%03hu,", Data->acceleration_avg / 1000, Data->acceleration_avg % 1000);
  }

  /*Maximum acceleration*/
  if (CheckAndUpdateLength(&DataLen, sizeof(uint16_t),
                           Data->acceleration_max == DEMO_DATA_1_1_ERROR_ACCELERATION, "E,") == true)
  {
    APP_PPRINTF("%hu.%03hu,", Data->acceleration_max / 1000, Data->acceleration_max % 1000);
  }

  /*Voltage*/
  if (voltage_present == true)
  {
    APP_PPRINTF("%u.%02u\r\n", Data->voltage / 20, (Data->voltage % 20) * 5);
  }
  else
  {
    APP_PPRINTF("E\r\n");
  }
}

