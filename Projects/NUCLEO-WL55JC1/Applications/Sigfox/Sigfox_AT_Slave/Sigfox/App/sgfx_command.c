/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_command.c
  * @author  MCD Application Team
  * @brief   main command driver dedicated to command AT
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
#include "sgfx_at.h"
#include "sgfx_command.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private define ------------------------------------------------------------*/
/* comment the following to have help message */
/* #define NO_HELP */
/* #define NO_KEY_ADDR_EUI */
#define CMD_SIZE                        64

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  Structure defining an AT Command
  */
struct ATCommand_s
{
  const char *string;                       /*< command string, after the "AT" */
  const int32_t size_string;                /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param);     /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
#if !defined(NO_HELP)
  const char *help_string;                  /*< to be printed when ? after the string */
#endif /* !NO_HELP */
};
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
  "\r\nAT_BUSY_ERROR\r\n",          /* AT_BUSY_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n", /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_LIB_ERROR\r\n",           /* AT_LIB_ERROR */
  "\r\nAT_TX_TIMEOUT\r\n",          /* AT_TX_TIMEOUT */
  "\r\nAT_RX_TIMEOUT\r\n",          /* AT_RX_TIMEOUT */
  "\r\nAT_RX_ERROR\r\n",            /* AT_RX_ERROR */
  "\r\nAT_RECONF_ERROR\r\n",        /*AT_RECONF_ERROR,*/
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
    .help_string = "AT"AT_RESET ": Trig a MCU reset\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_reset,
  },

  {
    .string = AT_RFS,
    .size_string = sizeof(AT_RFS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RFS ": Restore EEPROM Factory Settings\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_restore_factory_settings,
  },

  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VER ": Get the FW version\r\n",
#endif /* !NO_HELP */
    .get = AT_version_get,
    .set = AT_return_error,
    .run = AT_version_get,
  },

  {
    .string = AT_ID,
    .size_string = sizeof(AT_ID) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_ID ": Get the ID\r\n",
#endif /* !NO_HELP */
    .get = AT_DevId_get,
    .set = AT_return_error,
    .run = AT_DevId_get,
  },

  {
    .string = AT_PAC,
    .size_string = sizeof(AT_PAC) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PAC ": Get the PAC\r\n",
#endif /* !NO_HELP */
    .get = AT_DevPac_get,
    .set = AT_return_error,
    .run = AT_DevPac_get,
  },

  {
    .string = AT_S410,
    .size_string = sizeof(AT_S410) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S410 ": 0:Private Key 1:Public Key\r\n",
#endif /* !NO_HELP */
    .get = AT_PublicKey_get,
    .set = AT_PublicKey_set,
    .run = AT_return_error,
  },

  {
    .string = AT_S411,
    .size_string = sizeof(AT_S411) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S411 ": Payload Encryption 0:Off 1:On\r\n",
#endif /* !NO_HELP */
    .get = AT_PayloadEncryption_get,
    .set = AT_PayloadEncryption_set,
    .run = AT_return_error,
  },

  {
    .string = AT_SENDB,
    .size_string = sizeof(AT_SENDB) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDB"=<Bit>,<Opt downlink>,<Opt TxRepeat><CR>. Send a bit to the sigfox network\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_SendBit,
    .run = AT_return_error,
  },

  {
    .string = AT_SENDF,
    .size_string = sizeof(AT_SENDF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDF"=<Payload>,<Opt downlink>,<Opt TxRepeat><CR>. Send a ASCII frame to the sigfox network\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_SendFrame,
    .run = AT_return_error,
  },

  {
    .string = AT_SENDH,
    .size_string = sizeof(AT_SENDH) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDH "=<PayloadLength>,<Payload>,<Opt downlink>,<Opt TxRepeat><CR>. Send a Hex frame to the sigfox network\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_SendHexFrame,
    .run = AT_return_error,
  },

  {
    .string = AT_CW,
    .size_string = sizeof(AT_CW) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CW"=<frequency><CR> frequency in Hz or in MHz. Run CW Test mode\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_cw,
    .run = AT_return_error,
  },

  {
    .string = AT_PN,
    .size_string = sizeof(AT_PN) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PN "=<frequency>,<bitrate><CR> frequency in Hz or in MHz, bitrate=[100, 600]. Run PRBS9 BPBSK Test mode\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_pn,
    .run = AT_return_error,
  },

  {
    .string = AT_MN,
    .size_string = sizeof(AT_MN) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_MN "=<timeOut><CR> TimeOut in seconds. Start Monarch Scan\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_scan_mn,
    .run = AT_scan_mn,
  },

  {
    .string = AT_TM,
    .size_string = sizeof(AT_TM) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TM"=<rc>,<mode><CR> mode=[0..12]. Run Sigfox Test mode\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_mode,
    .run = AT_return_error,
  },

  {
    .string = AT_BAT,
    .size_string = sizeof(AT_BAT) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BAT ": Get the battery Level in mV \r\n",
#endif /* !NO_HELP */
    .get = AT_bat_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },

  {
    .string = AT_S302,
    .size_string = sizeof(AT_S302) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S302 "=<power>,<CR>  power in dBm=[0..20]. Set Radio output power in dBm\r\n",
#endif /* !NO_HELP */
    .get = AT_power_get,
    .set = AT_power_set,
    .run = AT_return_error,
  },

  {
    .string = AT_S300,
    .size_string = sizeof(AT_S300) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S300 ": send an out of band message once.\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_outOfBand_run,
  },

  {
    .string = AT_S400,
    .size_string = sizeof(AT_S400) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S400 "=<param1><param2><param3>,<param4><CR>. Configure specific variables for standard.\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_ChannelConfigFcc_set,
    .run = AT_return_error,
  },

  {
    .string = AT_RC,
    .size_string = sizeof(AT_RC) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RC "<param1><CR> param1=[1:7]. Set/Get the Regional Config zones.\r\n",
#endif /* !NO_HELP */
    .get = AT_rc_get,
    .set = AT_rc_set,
    .run = AT_return_error,
  },

  {
    .string = AT_RSSICAL,
    .size_string = sizeof(AT_RSSICAL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RSSICAL "=<param1><CR> param1=rssi value in dB. Set/Get the rssi calibration value in dB.\r\n",
#endif /* !NO_HELP */
    .get = AT_rssi_cal_get,
    .set = AT_rssi_cal_set,
    .run = AT_return_error,
  },

  {
    .string = ATE,
    .size_string = sizeof(ATE) - 1,
#ifndef NO_HELP
    .help_string = "AT"ATE "=<param1><CR> [0,1]. Set/Get the echo state.\r\n",
#endif /* !NO_HELP */
    .get = AT_echo_get,
    .set = AT_echo_set,
    .run = AT_return_error,
  },

  {
    .string = AT_VL,
    .size_string = sizeof(AT_VL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VL "=<param1><CR> [0:3]. Set/Get the Verbose Level with integer from 0(VLEVEL_OFF) to 3(VLEVEL_H)\r\n",
#endif /* !NO_HELP */
    .get = AT_verbose_get,
    .set = AT_verbose_set,
    .run = AT_return_error,
  },
  /* USER CODE BEGIN ATCommand */

  /* USER CODE END ATCommand */
};

static char command[CMD_SIZE];
static unsigned i = 0;
static uint32_t IsBusy = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Parse a command and process it
  * @param  cmd The command
  */
static void parse_cmd(const char *cmd);

/**
  * @brief  Print a string corresponding to an ATEerror_t
  * @param  error_type The AT error code
  */
static void com_error(ATEerror_t error_type);

/**
  * @brief  CMD_GetChar callback from ADV_TRACE
  * @param  rxChar th char received
  * @param  size
  * @param  error
  */
static void CMD_GetChar(uint8_t *rxChar, uint16_t size, uint8_t error);

/**
  * @brief  CNotifies the upper layer that a character has been received
  */
static void (*NotifyCb)(void);

/**
  * @brief  Remove backspace and its preceding character in the Command string
  * @param  cmd string to process
  * @retval 0 when OK, otherwise error
  */
static int32_t CMD_ProcessBackSpace(char *cmd);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void CMD_Init(void (*CmdProcessNotify)(void))
{
  /* USER CODE BEGIN CMD_Init_1 */

  /* USER CODE END CMD_Init_1 */
  UTIL_ADV_TRACE_StartRxProcess(CMD_GetChar);
  /* register call back*/
  if (CmdProcessNotify != NULL)
  {
    NotifyCb = CmdProcessNotify;
  }
  /*flush command buffer*/
  for (i = 0; i < CMD_SIZE; i++)
  {
    command[i] = 0;
  }
  /*initialize i*/
  i = 0;
  /*initialize IsBusy*/
  IsBusy = 0;
  /* USER CODE BEGIN CMD_Init_2 */

  /* USER CODE END CMD_Init_2 */
}

void CMD_Process(void)
{
  /* USER CODE BEGIN CMD_Process_1 */

  /* USER CODE END CMD_Process_1 */
#if 0 /* echo On    */
  AT_PRINTF("%c", command[i]);
#endif /* 0 */
  if (command[i] == AT_ERROR_RX_CHAR)
  {
    i = 0;
    com_error(AT_RX_ERROR);
  }
  else if ((command[i] == '\r') || (command[i] == '\n'))
  {
    if (IsBusy == 1)
    {
      i = 0;
      com_error(AT_BUSY_ERROR);
    }
    else if (i != 0)
    {
      command[i] = '\0';
      IsBusy = 1;
      CMD_ProcessBackSpace(command);
      parse_cmd(command);
      IsBusy = 0;
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
    i++;
  }
  /* USER CODE BEGIN CMD_Process_2 */

  /* USER CODE END CMD_Process_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static int32_t CMD_ProcessBackSpace(char *cmd)
{
  /* USER CODE BEGIN CMD_ProcessBackSpace_1 */

  /* USER CODE END CMD_ProcessBackSpace_1 */
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
    int32_t curs = 0;
    int32_t j = 0;

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
  /* USER CODE BEGIN CMD_ProcessBackSpace_2 */

  /* USER CODE END CMD_ProcessBackSpace_2 */
}

static void CMD_GetChar(uint8_t *rxChar, uint16_t size, uint8_t error)
{
  /* USER CODE BEGIN CMD_GetChar_1 */

  /* USER CODE END CMD_GetChar_1 */
  command[i] = *rxChar;

  NotifyCb();
  /* USER CODE BEGIN CMD_GetChar_2 */

  /* USER CODE END CMD_GetChar_2 */
}

static void parse_cmd(const char *cmd)
{
  /* USER CODE BEGIN parse_cmd_1 */

  /* USER CODE END parse_cmd_1 */
  ATEerror_t status = AT_OK;
  const struct ATCommand_s *Current_ATCommand;
  int32_t i;

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
    AT_PPRINTF("AT+<CMD>?        : Help on <CMD>\r\n"
               "AT+<CMD>         : Run <CMD>\r\n"
               "AT+<CMD>=<value> : Set the value\r\n"
               "AT+<CMD>=?       : Get the value\r\n");
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      AT_PPRINTF(ATCommand[i].help_string);
    }

    while (1 != UTIL_ADV_TRACE_IsBufferEmpty())
    {
      /* Wait that all printfs are completed*/
    }

#endif /* !NO_HELP */
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
            AT_PPRINTF(Current_ATCommand->help_string);
#endif /* !NO_HELP */
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
  /* USER CODE BEGIN parse_cmd_2 */

  /* USER CODE END parse_cmd_2 */
}

static void com_error(ATEerror_t error_type)
{
  /* USER CODE BEGIN com_error_1 */

  /* USER CODE END com_error_1 */
  if (error_type > AT_MAX)
  {
    error_type = AT_MAX;
  }
  AT_PPRINTF(ATError_description[error_type]);
  /* USER CODE BEGIN com_error_2 */

  /* USER CODE END com_error_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
