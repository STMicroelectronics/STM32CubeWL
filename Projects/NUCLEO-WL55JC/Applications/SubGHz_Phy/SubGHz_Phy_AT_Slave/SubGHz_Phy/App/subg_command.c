/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subg_command.c
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
#include "subg_at.h"
#include "subg_command.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* comment the following to have help message */
/* #define NO_HELP */
/* #define AT_RADIO_ACCESS */

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
  "\r\nAT_BUSY_ERROR\r\n",          /* AT_BUSY_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n", /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_NO_NETWORK_JOINED\r\n",   /* AT_NO_NET_JOINED */
  "\r\nAT_RX_ERROR\r\n",            /* AT_RX_ERROR */
  "\r\nAT_NO_CLASS_B_ENABLE\r\n",   /* AT_NO_CLASS_B_ENABLE */
  "\r\nAT_DUTYCYCLE_RESTRICTED\r\n", /* AT_DUTYCYCLE_RESTRICTED */
  "\r\nAT_CRYPTO_ERROR\r\n",        /* AT_CRYPTO_ERROR */
  "\r\nerror unknown\r\n",          /* AT_MAX */
};

/**
  * @brief  Array of all supported AT Commands
  */
static const struct ATCommand_s ATCommand[] =
{
  /* General commands */
  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VER" Get the FW version\r\n",
#endif /* !NO_HELP */
    .get = AT_version_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },

  {
    .string = AT_VL,
    .size_string = sizeof(AT_VL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VL"=<Level><CR>. Set the Verbose Level=[0:Off .. 3:High]\r\n",
#endif /* !NO_HELP */
    .get = AT_verbose_get,
    .set = AT_verbose_set,
    .run = AT_return_error,
  },

  {
    .string = AT_LTIME,
    .size_string = sizeof(AT_LTIME) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_LTIME" Get the local time in UTC format\r\n",
#endif /* !NO_HELP */
    .get = AT_LocalTime_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },

  {
    .string = AT_RESET,
    .size_string = sizeof(AT_RESET) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RESET" Trig a MCU reset\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_reset,
  },

  /* Radio tests commands */
  {
    .string = AT_TTONE,
    .size_string = sizeof(AT_TTONE) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TTONE" Starts RF Tone test\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_test_txTone,
  },

  {
    .string = AT_TRSSI,
    .size_string = sizeof(AT_TRSSI) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TRSSI" Starts RF RSSI tone test\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_test_rxRssi,
  },

  {
    .string = AT_TCONF,
    .size_string = sizeof(AT_TCONF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TCONF"=<Freq in Hz>:<Power in dBm>:<Lora Bandwidth <0 to 6>, or Rx FSK Bandwidth in Hz>:<Lora SF or FSK datarate (bps)>:<CodingRate 4/5, 4/6, 4/7, 4/8>:\r\n\
         <Lna>:<PA Boost>:<Modulation 0:FSK, 1:Lora, 2:BPSK, 3:MSK>:<PayloadLen in Bytes>:<FskDeviation in Hz>:<LowDrOpt 0:off, 1:on, 2:Auto>:\r\n\
         <BTproduct: 0:no Gaussian Filter Applied, 1:BT=0,3, 2:BT=0,5, 3:BT=0,7, 4:BT=1><CR>. Configure RF test\r\n"
    "AT+TCONF=868000000:14:50000:50000:4/5:0:0:0:16:25000:2:3 /*FSK*/\r\n"
    "AT+TCONF=868000000:14:10000:10000:4/5:0:0:3:16:25000:2:3 /*MSK*/\r\n"
    "AT+TCONF=868000000:14:4:12:4/5:0:0:1:16:25000:2:3 /*LORA*/\r\n",
#endif /* !NO_HELP */
    .get = AT_test_get_config,
    .set = AT_test_set_config,
    .run = AT_return_error,
  },

  {
    .string = AT_TTX,
    .size_string = sizeof(AT_TTX) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TTX"=<PacketNb><CR>. Starts RF Tx test: Nb of packets sent\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_tx,
    .run = AT_return_error,
  },

  {
    .string = AT_TRX,
    .size_string = sizeof(AT_TRX) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TRX"=<PacketNb><CR>. Starts RF Rx test: Nb of packets expected\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_rx,
    .run = AT_return_error,
  },

  {
    .string = AT_TTH,
    .size_string = sizeof(AT_TTH) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TTH"=<Fstart>,<Fstop>,<Fdelta>,<PacketNb><CR>. Starts RF Tx hopping test from Fstart to Fstop in Hz or MHz, Fdelta in Hz\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_test_tx_hopping,
    .run = AT_return_error,
  },

  {
    .string = AT_TOFF,
    .size_string = sizeof(AT_TOFF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TOFF" Stops on-going RF test\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_test_stop,
  },

  /* Radio access commands */
#ifdef AT_RADIO_ACCESS
  {
    .string = AT_REGW,
    .size_string = sizeof(AT_REGW) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_REGW"=<Addr>:<Data><CR> Write Radio Register Addr=[4 hex char], Data=[2 hex char]\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_write_register,
    .run = AT_return_error,
  },

  {
    .string = AT_REGR,
    .size_string = sizeof(AT_REGR) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_REGR"=<Addr><CR>. Read Radio Register Addr=[4 hex char]\r\n",
#endif /* !NO_HELP */
    .get = AT_return_error,
    .set = AT_read_register,
    .run = AT_return_error,
  },
#endif /*AT_RADIO_ACCESS*/

  /* USER CODE BEGIN ATCommand */

  /* USER CODE END ATCommand */
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
  widx = 0;
  ridx = 0;
  charCount = 0;
  i = 0;
  circBuffOverflow = 0;
  /* USER CODE BEGIN CMD_Init_2 */

  /* USER CODE END CMD_Init_2 */
}

void CMD_Process(void)
{
  /* USER CODE BEGIN CMD_Process_1 */

  /* USER CODE END CMD_Process_1 */
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
    AT_PPRINTF("%c", circBuffer[ridx]);
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
