/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_regions.c
  * @author  MCD Application Team
  * @brief   Region specific definitions for STM32WL Sensor Concentrator Demo.
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
#include "demo_regions.h"

/**
  * @brief European channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsEU[] =
{
  /*EU 10% 869.525*/
  {
    .power = 21,
    .max_bw = 250000,
    .freq = 869525000,
    .duty_cycle = 10,
  },

  /*EU 1% 868.3*/
  {
    .power = 16,
    .max_bw = 600000,
    .freq = 868300000,
    .duty_cycle = 100,
  },
};

/**
  * @brief European subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsEU[] =
{
  /*Subregion 0.0, Europe*/
  {
    .name = "European Union",
    .beacon_power = 22,
    .max_t = 900,
    .hopping = false,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsEU) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsEU,
  },
};

/**
  * @brief FCC channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsFCC[] =
{
  /*FCC 924.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 924500000,
    .duty_cycle = 1,
  },
  /*FCC 924.0 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 924000000,
    .duty_cycle = 1,
  },

  /*FCC 923.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 923500000,
    .duty_cycle = 1,
  },

  /*FCC 923 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 923000000,
    .duty_cycle = 1,
  },

  /*FCC 922.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 922500000,
    .duty_cycle = 1,
  },

  /*FCC 922 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 922000000,
    .duty_cycle = 1,
  },

  /*FCC 921.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 921500000,
    .duty_cycle = 1,
  },

  /*FCC 921 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 921000000,
    .duty_cycle = 1,
  },

  /*FCC 920.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 920500000,
    .duty_cycle = 1,
  },

  /*FCC 920 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 920000000,
    .duty_cycle = 1,
  },

  /*FCC 919.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 919500000,
    .duty_cycle = 1,
  },

  /*FCC 919 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 919000000,
    .duty_cycle = 1,
  },

  /*FCC 918.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 918500000,
    .duty_cycle = 1,
  },

  /*FCC 918 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 918000000,
    .duty_cycle = 1,
  },

  /*FCC 917.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 917500000,
    .duty_cycle = 1,
  },

  /*FCC 917 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 917000000,
    .duty_cycle = 1,
  },
  /*FCC 916.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 916500000,
    .duty_cycle = 1,
  },

  /*FCC 916 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 916000000,
    .duty_cycle = 1,
  },

  /*FCC 915.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 915500000,
    .duty_cycle = 1,
  },

  /*FCC 915 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 915000000,
    .duty_cycle = 1,
  },

  /*FCC 914.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 914500000,
    .duty_cycle = 1,
  },

  /*FCC 914 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 914000000,
    .duty_cycle = 1,
  },

  /*FCC 913.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 913500000,
    .duty_cycle = 1,
  },

  /*FCC 913 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 913000000,
    .duty_cycle = 1,
  },

  /*FCC 912.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 912500000,
    .duty_cycle = 1,
  },

  /*FCC 912 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 912000000,
    .duty_cycle = 1,
  },

  /*FCC 911.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 911500000,
    .duty_cycle = 1,
  },

  /*FCC 911 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 911000000,
    .duty_cycle = 1,
  },

  /*FCC 910.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 910500000,
    .duty_cycle = 1,
  },

  /*FCC 910 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 910000000,
    .duty_cycle = 1,
  },
  /*FCC 910.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 909500000,
    .duty_cycle = 1,
  },

  /*FCC 909 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 909000000,
    .duty_cycle = 1,
  },
  /*FCC 908.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 908500000,
    .duty_cycle = 1,
  },

  /*FCC 908 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 908000000,
    .duty_cycle = 1,
  },
  /*FCC 907.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 907500000,
    .duty_cycle = 1,
  },

  /*FCC 907 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 907000000,
    .duty_cycle = 1,
  },
  /*FCC 906.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 906500000,
    .duty_cycle = 1,
  },

  /*FCC 906 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 906000000,
    .duty_cycle = 1,
  },
  /*FCC 905.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 905500000,
    .duty_cycle = 1,
  },

  /*FCC 905 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 905000000,
    .duty_cycle = 1,
  },
  /*FCC 904.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 904500000,
    .duty_cycle = 1,
  },

  /*FCC 904 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 904000000,
    .duty_cycle = 1,
  },
  /*FCC 903.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 903500000,
    .duty_cycle = 1,
  },

  /*FCC 903 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 903000000,
    .duty_cycle = 1,
  },
  /*FCC 902.5 MHz*/
  {
    .power = 21,
    .max_bw = 500000,
    .freq = 902500000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Australian channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsAU[] =
{
  /*Australia 925 MHz*/
  {
    .power = 21,
    .max_bw = 1000000,
    .freq = 925000000,
    .duty_cycle = 1,
  },
};

/**
  * @brief US subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsUS_AU[] =
{
  /*Subregion 1.0, United States 902-928MHz*/
  {
    .name = "United States",
    .beacon_power = 22,
    .max_t = 400,
    .hopping = true,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsFCC) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsFCC,
  },

  /*Subregion 1.1, Australia 915-928MHz*/
  {
    .name = "Australia",
    .beacon_power = 22,
    .max_t = 1000,
    .hopping = true,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = 19, /* 924.5 down to 915.5*/
    .channels = DEMO_ChannelsFCC,
  },
};

/**
  * @brief Korean channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsKR[] =
{
  /*Korea 923.4 MHz*/
  {
    .power = 12,
    .max_bw = 200000,
    .freq = 923400000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Japanese channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsJP[] =
{
  /*Japan 923.4 MHz, 200 kHz*/
  {
    .power = 15,
    .max_bw = 200000,
    .freq = 923400000,
    .duty_cycle = 1,
  },

  /*Japan 923.3 MHz, 400 kHz*/
  {
    .power = 15,
    .max_bw = 400000,
    .freq = 923300000,
    .duty_cycle = 1,
  },

  /*Japan 923.2 MHz, 600 kHz*/
  {
    .power = 15,
    .max_bw = 600000,
    .freq = 923200000,
    .duty_cycle = 1,
  },

  /*Japan 923.1 MHz, 800 kHz*/
  {
    .power = 15,
    .max_bw = 800000,
    .freq = 923100000,
    .duty_cycle = 1,
  },

  /*Japan 923.0 MHz, 1000 kHz*/
  {
    .power = 15,
    .max_bw = 1000000,
    .freq = 923000000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Generic Asian channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsAS[] =
{
  /*Generic Asia 923.4 MHz*/
  {
    .power = 16,
    .max_bw = 200000,
    .freq = 923400000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Asian subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsAS[] =
{
  /*Subregion 2.0, Korea*/
  {
    .name = "Korea",
    .beacon_power = 12,
    .max_t = 1000,
    .hopping = false,
    .lbt = true,
    .lbt_time = 5,
    .lbt_rssi = -80,
    .channels_n = sizeof(DEMO_ChannelsKR) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsKR,
  },

  /*Subregion 2.1, Japan*/
  {
    .name = "Japan",
    .beacon_power = 15,
    .max_t = 1000,
    .hopping = false,
    .lbt = true,
    .lbt_time = 5,
    .lbt_rssi = -80,
    .channels_n = sizeof(DEMO_ChannelsJP) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsJP,
  },

  /*Subregion 2.2, Generic Asia*/
  {
    .name = "Generic Asia",
    .beacon_power = 16,
    .max_t = 1000,
    .hopping = false,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsAS) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsAS,
  },
};

/**
  * @brief Chinese channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsCN[] =
{
  /*China 470.3 MHz*/
  {
    .power = 19,
    .max_bw = 200000,
    .freq = 470300000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Chinese subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsCN[] =
{
  /*Subregion 3.0, China*/
  {
    .name = "China",
    .beacon_power = 19,
    .max_t = 1000,
    .hopping = false,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsCN) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsCN,
  },
};

/**
  * @brief Indian channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsIN[] =
{
  /*India 865.1 MHz*/
  {
    .power = 21,
    .max_bw = 200000,
    .freq = 865100000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Indian subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsIN[] =
{
  /*Subregion 4.0, India*/
  {
    .name = "India",
    .beacon_power = 22,
    .max_t = 1000,
    .hopping = false,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsIN) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsIN,
  },
};

/**
  * @brief Russian channels definitions.
  */
const DEMO_Channel_t DEMO_ChannelsRU[] =
{
  /*Russia 868.95 MHz*/
  {
    .power = 16,
    .max_bw = 500000,
    .freq = 868950000,
    .duty_cycle = 1,
  },
};

/**
  * @brief Russian subregions definitions.
  */
const DEMO_Subregion_t DEMO_SubregionsRU[] =
{
  /*Subregion 5.0, Russia*/
  {
    .name = "Russia",
    .beacon_power = 19,
    .max_t = 1000,
    .hopping = false,
    .lbt = false,
    .lbt_time = 0,
    .lbt_rssi = 0,
    .channels_n = sizeof(DEMO_ChannelsRU) / sizeof(DEMO_Channel_t),
    .channels = DEMO_ChannelsRU,
  },
};

/**
  * @brief All region definitions.
  */
const DEMO_Region_t DEMO_Regions[] =
{
  /*Region 0, Europe*/
  {
    .name = "European Union",
    .beacon_freq = 869525000,
    .beacon_bw = 7,         /*125 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 791,
    .subregions_n = sizeof(DEMO_SubregionsEU) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsEU,
  },

  /*Region 1, United States/Australia*/
  {
    .name = "United States/Australia",
    .beacon_freq = 925000000,
    .beacon_bw = 8,         /*250 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 395,
    .subregions_n = sizeof(DEMO_SubregionsUS_AU) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsUS_AU,
  },

  /*Region 2, Asia*/
  {
    .name = "Asia",
    .beacon_freq = 923400000,
    .beacon_bw = 7,         /*125 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 791,
    .subregions_n = sizeof(DEMO_SubregionsAS) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsAS,
  },

  /*Region 3, China*/
  {
    .name = "China",
    .beacon_freq = 470300000,
    .beacon_bw = 7,         /*125 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 791,
    .subregions_n = sizeof(DEMO_SubregionsCN) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsCN,
  },

  /*Region 4, India*/
  {
    .name = "India",
    .beacon_freq = 865100000,
    .beacon_bw = 7,         /*125 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 791,
    .subregions_n = sizeof(DEMO_SubregionsIN) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsIN,
  },

  /*Region 5, Russia*/
  {
    .name = "Russia",
    .beacon_freq = 868950000,
    .beacon_bw = 7,         /*125 KHz*/
    .beacon_sf = 11,        /*SF11*/
    .beacon_length = 791,
    .subregions_n = sizeof(DEMO_SubregionsRU) / sizeof(DEMO_Subregion_t),
    .subregions = DEMO_SubregionsRU,
  },
};

/**
  * How many regions are there.
  */
const int DEMO_Regions_n = sizeof(DEMO_Regions) / sizeof(DEMO_Region_t);

