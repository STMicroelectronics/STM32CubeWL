/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_command.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdint.h>
#include <string.h>
#include "lora_command.h"
#include "lora_at.h"
#include "stm32_adv_trace.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define CMD_LINE_SIZE 540U
#define CMD_RX_BUFFER_SIZE 640U

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static volatile uint8_t RxBuffer[CMD_RX_BUFFER_SIZE];
static volatile uint16_t RxWriteIndex;
static volatile uint16_t RxReadIndex;
static volatile bool RxOverflow;
static char CommandLine[CMD_LINE_SIZE];
static uint16_t CommandLength;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static void CMD_GetChar(uint8_t *data, uint16_t size, uint8_t error);
static void CMD_Parse(char *command);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void CMD_Init(void)
{
  RxWriteIndex = 0U;
  RxReadIndex = 0U;
  RxOverflow = false;
  CommandLength = 0U;
  (void)UTIL_ADV_TRACE_StartRxProcess(CMD_GetChar);
}

bool CMD_HasPendingData(void)
{
  uint16_t readIndex = RxReadIndex;
  uint16_t writeIndex = RxWriteIndex;

  return (readIndex != writeIndex) || (CommandLength != 0U);
}

void CMD_Process(void)
{
  uint16_t writeIndex = RxWriteIndex;

  while (RxReadIndex != writeIndex)
  {
    char character = (char)RxBuffer[RxReadIndex];
    RxReadIndex = (uint16_t)((RxReadIndex + 1U) % CMD_RX_BUFFER_SIZE);

    if ((character == '\r') || (character == '\n'))
    {
      if (CommandLength != 0U)
      {
        CommandLine[CommandLength] = '\0';
        CMD_Parse(CommandLine);
        CommandLength = 0U;
      }
    }
    else if ((character == '\b') || (character == 0x7FU))
    {
      if (CommandLength != 0U)
      {
        CommandLength--;
      }
    }
    else if (CommandLength < (CMD_LINE_SIZE - 1U))
    {
      CommandLine[CommandLength++] = character;
    }
    else
    {
      CommandLength = 0U;
      AT_PrintError(AT_PARAM_ERROR);
    }

    writeIndex = RxWriteIndex;
  }

  if (RxOverflow)
  {
    RxOverflow = false;
    CommandLength = 0U;
    AT_PrintError(AT_BUSY_ERROR);
  }
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static void CMD_GetChar(uint8_t *data, uint16_t size, uint8_t error)
{
  uint16_t nextIndex;

  if ((data == NULL) || (size == 0U) || (error != 0U))
  {
    return;
  }

  nextIndex = (uint16_t)((RxWriteIndex + 1U) % CMD_RX_BUFFER_SIZE);
  if (nextIndex == RxReadIndex)
  {
    RxOverflow = true;
    return;
  }

  RxBuffer[RxWriteIndex] = data[0];
  RxWriteIndex = nextIndex;
}

static void CMD_Parse(char *command)
{
  ATEerror_t result = AT_ERROR;
  char *parameter;

  if (strcmp(command, "AT") == 0)
  {
    result = AT_OK;
  }
  else if (strcmp(command, "AT?") == 0)
  {
    AT_PrintHelp();
    result = AT_OK;
  }
  else if (strcmp(command, "ATZ?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "ATZ") == 0)
  {
    result = AT_LoraReset();
  }
  else if (strcmp(command, "AT+RFS?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+RFS") == 0)
  {
    result = AT_LoraRestoreFactorySettings();
  }
  else if (strcmp(command, "AT+CS?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+CS") == 0)
  {
    result = AT_LoraStoreContext();
  }
  else if (strcmp(command, "AT+VER?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+VER=?") == 0)
  {
    result = AT_LoraGetVersion();
  }
  else if (strcmp(command, "AT+VL?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+VL=?") == 0)
  {
    result = AT_LoraGetVerboseLevel();
  }
  else if (strcmp(command, "AT+LTIME?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+LTIME=?") == 0)
  {
    result = AT_LoraGetLocalTime();
  }
  else if (strncmp(command, "AT+VL=", 6U) == 0)
  {
    result = AT_LoraSetVerboseLevel(&command[6]);
  }
  else if (strcmp(command, "AT+STATUS?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+STATUS=?") == 0)
  {
    result = AT_LoraGetStatus();
  }
  else if (strcmp(command, "AT+ADR?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+ADR=?") == 0)
  {
    result = AT_LoraGetAdrProfile();
  }
  else if (strncmp(command, "AT+ADR=", 7U) == 0)
  {
    result = AT_LoraSetAdrProfile(&command[7]);
  }
  else if (strcmp(command, "AT+DR?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+DR=?") == 0)
  {
    result = AT_LoraGetDataRate();
  }
  else if (strncmp(command, "AT+DR=", 6U) == 0)
  {
    result = AT_LoraSetDataRate(&command[6]);
  }
  else if (strcmp(command, "AT+DCS?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+DCS=?") == 0)
  {
    result = AT_LoraGetDutyCycleStatus();
  }
  else if (strcmp(command, "AT+BAT?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+BAT") == 0)
  {
    result = AT_LoraGetBatteryVoltage();
  }
  else if (strcmp(command, "AT+PGSLOT?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+PGSLOT=?") == 0)
  {
    result = AT_LoraGetPingSlotPeriodicity();
  }
  else if (strncmp(command, "AT+PGSLOT=", 10U) == 0)
  {
    result = AT_LoraSetPingSlotPeriodicity(&command[10]);
  }
  else if (strcmp(command, "AT+DEUI?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+DEUI=?") == 0)
  {
    result = AT_LoraGetDevEui();
  }
  else if (strncmp(command, "AT+DEUI=", 8U) == 0)
  {
    result = AT_LoraSetDevEui(&command[8]);
  }
  else if (strcmp(command, "AT+APPEUI?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+APPEUI=?") == 0)
  {
    result = AT_LoraGetJoinEui();
  }
  else if (strncmp(command, "AT+APPEUI=", 10U) == 0)
  {
    result = AT_LoraSetJoinEui(&command[10]);
  }
  else if (strncmp(command, "AT+NWKKEY=", 10U) == 0)
  {
    result = AT_LoraSetNwkKey(&command[10]);
  }
  else if (strcmp(command, "AT+NWKKEY?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strncmp(command, "AT+APPKEY=", 10U) == 0)
  {
    result = AT_LoraSetAppKey(&command[10]);
  }
  else if (strcmp(command, "AT+APPKEY?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+NWKSKEY?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strncmp(command, "AT+NWKSKEY=", 11U) == 0)
  {
    result = AT_LoraSetNwkSessionKey(&command[11]);
  }
  else if (strcmp(command, "AT+APPSKEY?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strncmp(command, "AT+APPSKEY=", 11U) == 0)
  {
    result = AT_LoraSetAppSessionKey(&command[11]);
  }
  else if (strcmp(command, "AT+DADDR?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+DADDR=?") == 0)
  {
    result = AT_LoraGetDevAddr();
  }
  else if (strncmp(command, "AT+DADDR=", 9U) == 0)
  {
    result = AT_LoraSetDevAddr(&command[9]);
  }
  else if (strcmp(command, "AT+BAND?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+BAND=?") == 0)
  {
    result = AT_LoraGetRegion();
  }
  else if (strncmp(command, "AT+BAND=", 8U) == 0)
  {
    result = AT_LoraSetRegion(&command[8]);
  }
  else if ((strcmp(command, "AT+JOIN") == 0) || (strncmp(command, "AT+JOIN=", 8U) == 0))
  {
    parameter = (command[7] == '=') ? &command[8] : NULL;
    result = AT_LoraJoin(parameter);
  }
  else if (strcmp(command, "AT+JOIN?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strncmp(command, "AT+SEND=", 8U) == 0)
  {
    result = AT_LoraSend(&command[8]);
  }
  else if (strcmp(command, "AT+SEND?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+CLASS?") == 0)
  {
    result = AT_LoraPrintCommandHelp(command);
  }
  else if (strcmp(command, "AT+CLASS=?") == 0)
  {
    result = AT_LoraGetClass();
  }
  else if (strncmp(command, "AT+CLASS=", 9U) == 0)
  {
    result = AT_LoraSetClass(&command[9]);
  }

  AT_PrintError(result);
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
