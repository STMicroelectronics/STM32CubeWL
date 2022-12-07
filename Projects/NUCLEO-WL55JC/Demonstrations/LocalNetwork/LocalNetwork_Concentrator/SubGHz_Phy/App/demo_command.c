/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_command.c
  * @author  MCD Application Team
  * @brief   Main command driver dedicated to command AT
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
#include "platform.h"
#include "demo_command.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */
/* comment the following to have help message */
/* #define NO_HELP */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  Structure defining an AT Command
  */
struct ATCommand_s
{
  const char *string;                       /*< command string, after the "AT" */
  const int size_string;                    /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param);     /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
#if !defined(NO_HELP)
  const char *help_string;                  /*< to be printed when ? after the string */
#endif /* !NO_HELP */
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define CMD_SIZE                        540
#define CIRC_BUFF_SIZE                  8

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Array corresponding to the description of each possible AT Error
  */
static const char *const ATError_description[] =
{
  "\r\nOK\r\n",                     /* AT_OK */
  "\r\nAT_ERROR\r\n",               /* AT_ERROR */
  "\r\nAT_PARAM_ERROR\r\n",         /* AT_PARAM_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n", /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_RX_ERROR\r\n",            /* AT_RX_ERROR */
  "\r\nAT_MOD_NOT_ALLOWED\r\n",     /* AT_MOD_NOT_ALLOWED */
  "\r\nAT_EUI_NOT_CONNECTED\r\n",   /* AT_EUI_NOT_CONNECTED */
  "\r\nerror unknown\r\n",          /* AT_MAX */
};

/**
  * @brief  Array of all supported AT Commands
  */
static const struct ATCommand_s ATCommand[] =
{
  {
    .string = AT_RESET,
    .size_string = sizeof(AT_RESET) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RESET ": Trig a reset of the MCU\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_return_error,
    .run = at_reset,
  },

  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VER ": Get the version of the FW\r\n",
#endif /* NO_HELP */
    .get = at_version_get,
    .set = at_return_error,
    .run = at_return_error,
  },

  {
    .string = AT_VL,
    .size_string = sizeof(AT_VL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VL ": Set the Verbose Level with integer from 0(VLEVEL_OFF) to 3(VLEVEL_H)\r\n",
#endif /* NO_HELP */
    .get = at_verbose_get,
    .set = at_verbose_set,
    .run = at_return_error,
  },

  {
    .string = AT_LIST_REGIONS,
    .size_string = sizeof(AT_LIST_REGIONS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_LIST_REGIONS ": List all available regions and subregions\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_return_error,
    .run = at_list_regions,
  },

#ifndef NO_HELP
#define COMMAND_PROVIDE_GETSET(name, info)                                               \
  {                                                                                      \
    .string = AT_##name,                                                                 \
    .size_string = sizeof(AT_##name) - 1,                                                \
    .help_string = "AT"AT_##name ": Get/Set the " #name " register\r\n\t" info "\r\n",   \
    .get = at_##name##_get,                                                              \
    .set = at_##name##_set,                                                              \
    .run = at_return_error,                                                              \
  }
#else  /*NO_HELP*/
#define COMMAND_PROVIDE_GETSET(name)                                                     \
  {                                                                                      \
    .string = AT_##name,                                                                 \
    .size_string = sizeof(AT_##name) - 1,                                                \
    .get = at_##name##_get,                                                              \
    .set = at_##name##_set,                                                              \
    .run = at_return_error,                                                              \
  }
#endif /*NO_HELP*/

  COMMAND_PROVIDE_GETSET(DE, "LoRa LowDataRateOptimize [0, 1]"),
  COMMAND_PROVIDE_GETSET(CR, "LoRa Code Rate [1 .. 4] = [4/5 .. 4/8]"),
  COMMAND_PROVIDE_GETSET(SF, "LoRa Spreading Factor [6 .. 12]"),
  COMMAND_PROVIDE_GETSET(BW, "LoRa Bandwidth [0 .. 9]\r\n"
                         "\t\t0 =   7.81 kHz\r\n"
                         "\t\t1 =  10.42 kHz\r\n"
                         "\t\t2 =  15.63 kHz\r\n"
                         "\t\t3 =  20.83 kHz\r\n"
                         "\t\t4 =  31.25 kHz\r\n"
                         "\t\t5 =  41.67 kHz\r\n"
                         "\t\t6 =  62.5  kHz\r\n"
                         "\t\t7 = 125    kHz\r\n"
                         "\t\t8 = 250    kHz\r\n"
                         "\t\t9 = 500    kHz\r\n"),

  COMMAND_PROVIDE_GETSET(RISE, "FSK Power Ramp Up Time [0 .. 7]\r\n"
                         "\t\t0 =   10 us\r\n"
                         "\t\t1 =   20 us\r\n"
                         "\t\t2 =   40 us\r\n"
                         "\t\t3 =   80 us\r\n"
                         "\t\t4 =  200 us\r\n"
                         "\t\t5 =  800 us\r\n"
                         "\t\t6 = 1700 us\r\n"
                         "\t\t7 = 3400 us\r\n"),
  COMMAND_PROVIDE_GETSET(BR, "FSK Bitrate [600 .. 300000 bits/s]"),
  COMMAND_PROVIDE_GETSET(FDEV, "FSK Frequency Deviation [Hz]"),
  COMMAND_PROVIDE_GETSET(BT, "FSK Gaussian Pulse Shape BT [0 .. 4]\r\n"
                         "\t\t(lower BT means wider spreading)\r\n"
                         "\t\t0 - off, no spreading\r\n"
                         "\t\t1 - 0.3\r\n"
                         "\t\t2 - 0.5\r\n"
                         "\t\t3 - 0.7\r\n"
                         "\t\t4 -   1\r\n"),

  COMMAND_PROVIDE_GETSET(REGION, "Region number"),
  COMMAND_PROVIDE_GETSET(SUBREGION, "Subregion number"),

  {
    .string = AT_BEACON_ON,
    .size_string = sizeof(AT_BEACON_ON) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BEACON_ON ": Start sending beacons (uses REGION and SUBREGION registers)\r\n",
#endif /* NO_HELP */
    .get = at_beacon_get,
    .set = at_beacon_set,
    .run = at_beacon_run,
  },

  {
    .string = AT_MOD_LORA,
    .size_string = sizeof(AT_MOD_LORA) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_MOD_LORA ": Set modulation for this EUI to LoRa\r\n\t\t"
    "(uses DE, CR, SF and BW registers) (use AT+MOD_LORA=0xabcd1234)\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_mod_lora,
    .run = at_return_error,
  },

  {
    .string = AT_MOD_FSK,
    .size_string = sizeof(AT_MOD_FSK) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_MOD_FSK ": Set modulation for this EUI to FSK\r\n\t\t"
    "(uses RISE, BR, FDEV and BT registers) (use AT+MOD_FSK=0xabcd1234)\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_mod_fsk,
    .run = at_return_error,
  },

  {
    .string = AT_MOD_TEST_LORA,
    .size_string = sizeof(AT_MOD_TEST_LORA) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_MOD_TEST_LORA ": Test LoRa modulation\r\n\t\t"
    "(uses DE, CR, SF and BW registers)\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_return_error,
    .run = at_mod_test_lora,
  },

  {
    .string = AT_MOD_TEST_FSK,
    .size_string = sizeof(AT_MOD_TEST_FSK) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_MOD_TEST_FSK ": Test FSK modulation\r\n\t\t"
    "(uses RISE, BR, FDEV and BT registers)\r\n",
#endif /* NO_HELP */
    .get = at_return_error,
    .set = at_return_error,
    .run = at_mod_test_fsk,
  },
};

static char circBuffer[CIRC_BUFF_SIZE];
static char command[CMD_SIZE];
static unsigned i = 0;
static uint32_t widx = 0;
static uint32_t ridx = 0;
static uint32_t charCount = 0;
static uint32_t circBuffOverflow = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Parse a command and process it
  * @param  The command
  * @retval None
  */
static void parse_cmd(const char *cmd);

/**
  * @brief  Print a string corresponding to an ATEerror_t
  * @param  The AT error code
  * @retval None
  */
static void com_error(ATEerror_t error_type);

/**
  * @brief  CMD_GetChar callback from ADV_TRACE
  * @param  rxChar th char received
  * @param  size
  * @param  error
  * @retval None
  */
static void CMD_GetChar(uint8_t *rxChar, uint16_t size, uint8_t error);

/**
  * @brief  CNotifies the upper layer that a charchter has been receveid
  * @param  None
  * @retval None
  */
static void (*NotifyCb)(void);

/**
  * @brief  Remove backspace and its preceding character in the Command string
  * @param  Command string to process
  * @retval 0 when OK, otherwise error
  */
static int32_t CMD_ProcessBackSpace(char *cmd);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void CMD_Init(void (*CmdProcessNotify)(void))
{
  at_init();    /*Preset AT registers to defaults*/

  UTIL_ADV_TRACE_StartRxProcess(CMD_GetChar);
  if (CmdProcessNotify != NULL)
  {
    NotifyCb = CmdProcessNotify;
  }
  widx = 0;
  ridx = 0;
  charCount = 0;
  i = 0;
  circBuffOverflow = 0;
}

void CMD_Process(void)
{
  /* Process all commands */
  if (circBuffOverflow == 1)
  {
    com_error(AT_TEST_PARAM_OVERFLOW);
    /*Full flush in case of overflow */
    UTILS_ENTER_CRITICAL_SECTION();
    ridx = widx;
    charCount = 0;
    circBuffOverflow = 0;
    UTILS_EXIT_CRITICAL_SECTION();
    i = 0;
  }

  while (charCount != 0)
  {
#if 0 /* echo On    */
    APP_PRINTF("%c", circBuffer[ridx]);
#endif /* 0 */

    if (circBuffer[ridx] == AT_ERROR_RX_CHAR)
    {
      ridx++;
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      UTILS_ENTER_CRITICAL_SECTION();
      charCount--;
      UTILS_EXIT_CRITICAL_SECTION();
      com_error(AT_RX_ERROR);
      i = 0;
    }
    else if ((circBuffer[ridx] == '\r') || (circBuffer[ridx] == '\n'))
    {
      ridx++;
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      UTILS_ENTER_CRITICAL_SECTION();
      charCount--;
      UTILS_EXIT_CRITICAL_SECTION();

      if (i != 0)
      {
        command[i] = '\0';
        UTILS_ENTER_CRITICAL_SECTION();
        CMD_ProcessBackSpace(command);
        UTILS_EXIT_CRITICAL_SECTION();
        parse_cmd(command);
        i = 0;
      }
    }
    else if (i == (CMD_SIZE - 1))
    {
      i = 0;
      com_error(AT_TEST_PARAM_OVERFLOW);
    }
    else
    {
      command[i++] = circBuffer[ridx++];
      if (ridx == CIRC_BUFF_SIZE)
      {
        ridx = 0;
      }
      UTILS_ENTER_CRITICAL_SECTION();
      charCount--;
      UTILS_EXIT_CRITICAL_SECTION();
    }
  }
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static int32_t CMD_ProcessBackSpace(char *cmd)
{
  uint32_t i = 0;
  uint32_t bs_cnt = 0;
  uint32_t cmd_len = 0;
  /*get command length and number of backspace*/
  while (cmd[cmd_len] != '\0')
  {
    if (cmd[cmd_len] == '\b')
    {
      bs_cnt++;
    }
    cmd_len++;
  }
  /*for every backspace, remove backspace and its preceding character*/
  for (i = 0; i < bs_cnt; i++)
  {
    int curs = 0;
    int j = 0;

    /*set cursor to backspace*/
    while (cmd[curs] != '\b')
    {
      curs++;
    }
    if (curs > 0)
    {
      for (j = curs - 1; j < cmd_len - 2; j++)
      {
        cmd[j] = cmd[j + 2];
      }
      cmd[j++] = '\0';
      cmd[j++] = '\0';
      cmd_len -= 2;
    }
    else
    {
      return -1;
    }
  }
  return 0;
}

static void CMD_GetChar(uint8_t *rxChar, uint16_t size, uint8_t error)
{
  charCount++;
  if (charCount == (CIRC_BUFF_SIZE + 1))
  {
    circBuffOverflow = 1;
    charCount--;
  }
  else
  {
    circBuffer[widx++] = *rxChar;
    if (widx == CIRC_BUFF_SIZE)
    {
      widx = 0;
    }
  }

  if (NotifyCb != NULL)
  {
    NotifyCb();
  }
}

static void parse_cmd(const char *cmd)
{
  ATEerror_t status = AT_OK;
  const struct ATCommand_s *Current_ATCommand;
  int i;

  if ((cmd[0] != 'A') || (cmd[1] != 'T'))
  {
    status = AT_ERROR;
  }
  else if (cmd[2] == '\0')
  {
    /* status = AT_OK; */
  }
  else if (cmd[2] == '?')
  {
#ifdef NO_HELP
#else
    APP_PPRINTF("AT+<CMD>?        : Help on <CMD>\r\n"
                "AT+<CMD>         : Run <CMD>\r\n"
                "AT+<CMD>=<value> : Set the value\r\n"
                "AT+<CMD>=?       : Get the value\r\n");
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      APP_PPRINTF(ATCommand[i].help_string);
    }
    while (1 != UTIL_ADV_TRACE_IsBufferEmpty())
    {
      /* Wait that all printfs are completed*/
    }
#endif /* NO_HELP */
  }
  else
  {
    /* point to the start of the command, excluding AT */
    status = AT_ERROR;
    cmd += 2;
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      if (strncmp(cmd, ATCommand[i].string, ATCommand[i].size_string) == 0)
      {
        Current_ATCommand = &(ATCommand[i]);
        /* point to the string after the command to parse it */
        cmd += Current_ATCommand->size_string;

        /* parse after the command */
        switch (cmd[0])
        {
          case '\0':    /* nothing after the command */
            status = Current_ATCommand->run(cmd);
            break;
          case '=':
            if ((cmd[1] == '?') && (cmd[2] == '\0'))
            {
              status = Current_ATCommand->get(cmd + 1);
            }
            else
            {
              status = Current_ATCommand->set(cmd + 1);
            }
            break;
          case '?':
#ifndef NO_HELP
            APP_PRINTF(Current_ATCommand->help_string);
#endif /* NO_HELP */
            status = AT_OK;
            break;
          default:
            /* not recognized */
            break;
        }

        /* we end the loop as the command was found */
        break;
      }
    }
  }

  com_error(status);
}

static void com_error(ATEerror_t error_type)
{
  if (error_type > AT_MAX)
  {
    error_type = AT_MAX;
  }
  APP_PPRINTF(ATError_description[error_type]);
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
