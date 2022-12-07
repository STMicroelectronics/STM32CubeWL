/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_app.c
  * @author  MCD Application Team
  * @brief   Initializes HW and SW system entities (not related to the radio)
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
#include <stdio.h>
#include "platform.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "stm32_systime.h"
#include "stm32_lpm.h"
#include "timer_if.h"
#include "utilities_def.h"
#include "sys_debug.h"
#if defined (SECURE_UNPRIVILEGE_ENABLE) && (SECURE_UNPRIVILEGE_ENABLE == 1)
#include "sys_privileged_services.h"
#elif !defined (SECURE_UNPRIVILEGE_ENABLE)
#error SECURE_UNPRIVILEGE_ENABLE not defined
#endif /* SECURE_UNPRIVILEGE_ENABLE */
#include "sgfx_eeprom_if.h"
#include "msg_id.h"
#include "mbmuxif_sys.h"
#include "mbmuxif_trace.h"
#include "mbmuxif_radio.h"
#include "features_info.h"
#include "mbmuxif_sigfox.h"
#ifdef ALLOW_KMS_VIA_MBMUX /* currently not supported */
/* #include "mbmuxif_kms.h" */
#endif /* ALLOW_KMS_VIA_MBMUX */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define MAX_TS_SIZE (int) 16

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static EXTI_HandleTypeDef Exti41;
static uint8_t SYS_TimerInitialisedFlag = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief Radio NVIC IRQ & MSI Wakeup SystemClock setting
  */
static void System_Init(void);

/**
  * @brief  it calls UTIL_ADV_TRACE_VSNPRINTF
  */
static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...);

/**
  * @brief  Init the Exti used for sync the two CPU after SBSFU download
  */
static void MX_EXTI_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SystemApp_Init(void)
{
  int8_t init_status;
  /* USER CODE BEGIN SystemApp_Init_1 */

  /* USER CODE END SystemApp_Init_1 */

  /* RTC_Init: normally already executed by overloading HAL_InitTick(), but need to be sure before notify Cm4 */
  /*Initialize timer and RTC*/
  UTIL_TIMER_Init();
  SYS_TimerInitialisedFlag = 1;
  /* #warning "should be removed when proper obl is done" */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  E2P_Init();

  /*Init low power manager*/
  UTIL_LPM_Init();
  /* Disable Stand-by mode */
  UTIL_LPM_SetOffMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);

#if defined (LOW_POWER_DISABLE) && (LOW_POWER_DISABLE == 1)
  /* Disable Stop Mode */
  UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#elif !defined (LOW_POWER_DISABLE)
#error LOW_POWER_DISABLE not defined
#endif /* LOW_POWER_DISABLE */

  /* Init the Exti used for sync the two CPU after SBSFU download */
  MX_EXTI_Init();

#if defined (SECURE_UNPRIVILEGE_ENABLE) && (SECURE_UNPRIVILEGE_ENABLE == 1)
  ThumbState_RemapMspAndSwitchToPspStack();
  ThumbState_EnterUnprivilegedMode();
#elif !defined (SECURE_UNPRIVILEGE_ENABLE)
#error SECURE_UNPRIVILEGE_ENABLE not defined
#endif /* SECURE_UNPRIVILEGE_ENABLE */

  /* Wait for CPU1 to be ready */
  while (HAL_EXTI_GetPending(&Exti41, EXTI_TRIGGER_RISING_FALLING) != 0x01u)
  {
    /* Wait CPU1 Event */
  }
  /* Acknowledge CPU1 event */
  HAL_EXTI_ClearPending(&Exti41, EXTI_TRIGGER_RISING_FALLING);

  /* Init Feat_Info table */
  FEAT_INFO_Init();

  /* Note: the trace is initialized in the context of MBMUXIF because it uses the MB on Cm0 side */
  init_status = MBMUXIF_SystemInit();
  if (init_status < 0)
  {
    Error_Handler();
  }

  System_Init();

  /* Registers Sigfox Notif to the sequencer */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbSigfoxNotifRcv), UTIL_SEQ_RFU, MBMUXIF_SigfoxSendNotifTask);

  /* USER CODE BEGIN SystemApp_Init_2 */

  /* USER CODE END SystemApp_Init_2 */
}

void TimestampNow(uint8_t *buff, uint16_t *size)
{
  /* USER CODE BEGIN TimestampNow_1 */

  /* USER CODE END TimestampNow_1 */
  SysTime_t curtime = SysTimeGet();
  tiny_snprintf_like((char *)buff, MAX_TS_SIZE, "%ds%03d:", curtime.Seconds, curtime.SubSeconds);
  *size = strlen((char *)buff);
  /* USER CODE BEGIN TimestampNow_2 */

  /* USER CODE END TimestampNow_2 */
}

#ifdef ALLOW_KMS_VIA_MBMUX /* currently not supported */
void Process_Kms_Cmd(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Kms_Cmd_1 */

  /* USER CODE END Process_Kms_Cmd_1 */
  uint32_t *com_buffer = MBMUX_SEC_VerifySramBufferPtr(ComObj->ParamBuf, ComObj->BufSize);

  APP_LOG(TS_ON, VLEVEL_L, ">CM0PLUS(KMS)\r\n");

  /* process Command */
  switch (ComObj->MsgId)
  {
    case KMS_CRYPTO_HMAC_SHA256_MSG_ID:
      APP_LOG(TS_ON, VLEVEL_L, " * CM0 Cmd rcv : KMS_CRYPTO_HMAC_SHA256_MSG_ID\r\n");
      APP_LOG(TS_ON, VLEVEL_L, " * CM0 Crypto aKey length %d\r\n", com_buffer[1]);
      APP_LOG(TS_ON, VLEVEL_L, " * CM0 Crypto aKey string %s\r\n", (char *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0;
      ComObj->ReturnVal = (uint32_t) -5; /* dummy value for test */
      break;

    default:
      break;
  }

  /* send Response */
  APP_LOG(TS_ON, VLEVEL_L, "<CM0PLUS(KMS)\r\n");
  MBMUX_ResponseSnd(FEAT_INFO_KMS_ID);

  /* USER CODE BEGIN Process_Kms_Cmd_2 */

  /* USER CODE END Process_Kms_Cmd_2 */
}
#endif /* ALLOW_KMS_VIA_MBMUX */

void Process_Sys_Cmd(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Sys_Cmd_1 */

  /* USER CODE END Process_Sys_Cmd_1 */
  APP_LOG(TS_ON, VLEVEL_L, ">CM0PLUS(System)\r\n");

  /* process Command */
  switch (ComObj->MsgId)
  {
    case SYS_OTHER_MSG_ID:
      /* USER CODE BEGIN Process_Sys_Cmd_switch_msg_id */

      /* USER CODE END Process_Sys_Cmd_switch_msg_id */
      break;

    /* USER CODE BEGIN Process_Sys_Cmd_switch_case */

    /* USER CODE END Process_Sys_Cmd_switch_case */

    default:
      /* USER CODE BEGIN Process_Sys_Cmd_switch_default */

      /* USER CODE END Process_Sys_Cmd_switch_default */
      break;
  }

  /* send Response */
  APP_LOG(TS_ON, VLEVEL_M, "<CM0PLUS(System)\r\n");
  MBMUX_ResponseSnd(FEAT_INFO_SYSTEM_ID);
  /* USER CODE BEGIN Process_Sys_Cmd_2 */

  /* USER CODE END Process_Sys_Cmd_2 */
}

void UTIL_SEQ_EvtIdle(uint32_t TaskId_bm, uint32_t EvtWaited_bm)
{
  /**
    * overwrites the __weak UTIL_SEQ_EvtIdle() in stm32_seq.c
    * all to process all tack except TaskId_bm
    */
  /* USER CODE BEGIN UTIL_SEQ_EvtIdle_1 */

  /* USER CODE END UTIL_SEQ_EvtIdle_1 */
  UTIL_SEQ_Run(~TaskId_bm);
  /* USER CODE BEGIN UTIL_SEQ_EvtIdle_2 */

  /* USER CODE END UTIL_SEQ_EvtIdle_2 */
  return;
}

/**
  * @brief redefines __weak function in stm32_seq.c such to enter low power
  */
void UTIL_SEQ_Idle(void)
{
  /* USER CODE BEGIN UTIL_SEQ_Idle_1 */

  /* USER CODE END UTIL_SEQ_Idle_1 */
  UTIL_LPM_EnterLowPower();
  /* USER CODE BEGIN UTIL_SEQ_Idle_2 */

  /* USER CODE END UTIL_SEQ_Idle_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void System_Init(void)
{
  /* USER CODE BEGIN System_Init_1 */

  /* USER CODE END System_Init_1 */
  /* No need of Enable Internal Wake-up line for CPU2 */
  LL_C2_PWR_DisableInternWU();

  /**< Disable all wakeup interrupt on CPU2  except RTC(17,20) IPCC_CPU2(37), Radio(44,45), Debugger(46) */
  /* This is to avoid that Cm0 woke up as consequence of IRQs that are meant to reach only Cm4 */
  LL_C2_EXTI_DisableIT_0_31(~0  & (~(LL_EXTI_LINE_17 | LL_EXTI_LINE_20)));
  LL_C2_EXTI_DisableIT_32_63((~0) & (~(LL_EXTI_LINE_37 | (0x1U << (7u)) | LL_EXTI_LINE_44 | LL_EXTI_LINE_45 | LL_EXTI_LINE_46)));

  /* Enable Radio IRQ lines interrupt for CPU2 */
  LL_C2_PWR_SetRadioIRQTrigger(LL_PWR_RADIO_IRQ_TRIGGER_WU_IT);
  /* Enable Radio EXTI lines for CPU2 (Needed for low power STOP mode */
  LL_C2_EXTI_EnableIT_32_63(LL_EXTI_LINE_44);
  LL_C2_EXTI_EnableIT_32_63(LL_EXTI_LINE_45);

  /* Ensure that MSI is wake-up system clock */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
  /* USER CODE BEGIN System_Init_Last */

  /* USER CODE END System_Init_Last */
}

/* Disable StopMode when traces need to be printed */
void UTIL_ADV_TRACE_PreSendHook(void)
{
  /* USER CODE BEGIN UTIL_ADV_TRACE_PreSendHook_1 */

  /* USER CODE END UTIL_ADV_TRACE_PreSendHook_1 */
  UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_DISABLE);
  /* USER CODE BEGIN UTIL_ADV_TRACE_PreSendHook_2 */

  /* USER CODE END UTIL_ADV_TRACE_PreSendHook_2 */
}
/* Re-enable StopMode when traces have been printed */
void UTIL_ADV_TRACE_PostSendHook(void)
{
  /* USER CODE BEGIN UTIL_LPM_SetStopMode_1 */

  /* USER CODE END UTIL_LPM_SetStopMode_1 */
  UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
  /* USER CODE BEGIN UTIL_LPM_SetStopMode_2 */

  /* USER CODE END UTIL_LPM_SetStopMode_2 */
}

static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...)
{
  /* USER CODE BEGIN tiny_snprintf_like_1 */

  /* USER CODE END tiny_snprintf_like_1 */
  va_list vaArgs;
  va_start(vaArgs, strFormat);
  UTIL_ADV_TRACE_VSNPRINTF(buf, maxsize, strFormat, vaArgs);
  va_end(vaArgs);
  /* USER CODE BEGIN tiny_snprintf_like_2 */

  /* USER CODE END tiny_snprintf_like_2 */
}

/**
  * Enable EXTI controller
  * Configure line 41
  */
static void MX_EXTI_Init(void)
{
  EXTI_ConfigTypeDef exit41config;

  exit41config.Line = EXTI_LINE_41;
  exit41config.Mode = EXTI_MODE_INTERRUPT;
  exit41config.Trigger = EXTI_TRIGGER_RISING;
  HAL_EXTI_SetConfigLine(&Exti41, &exit41config);
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/* HAL overload functions ---------------------------------------------------------*/

/**
  * @note This function overwrites the __weak one from HAL
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Don't enable SysTick if TIMER_IF is based on other counters (e.g. RTC) */
  /* USER CODE BEGIN HAL_InitTick_1 */

  /* USER CODE END HAL_InitTick_1 */
  return HAL_OK;
  /* USER CODE BEGIN HAL_InitTick_2 */

  /* USER CODE END HAL_InitTick_2 */
}

/**
  * @note This function overwrites the __weak one from HAL
  */
uint32_t HAL_GetTick(void)
{
  uint32_t ret = 0;
  /* TIMER_IF can be based on other counter the SysTick e.g. RTC */
  /* USER CODE BEGIN HAL_GetTick_1 */

  /* USER CODE END HAL_GetTick_1 */
  if (SYS_TimerInitialisedFlag == 0)
  {
    /* TIMER_IF_GetTimerValue should be used only once UTIL_TIMER_Init() is initialized */
    /* If HAL_Delay or a TIMEOUT countdown is necessary during initialization phase */
    /* please use temporarily another timebase source (SysTick or TIMx), which implies also */
    /* to rework the above function HAL_InitTick() and to call HAL_IncTick() on the timebase IRQ */
    /* Note: when TIMER_IF is based on RTC, stm32wlxx_hal_rtc.c calls this function before TimeServer is functional */
    /* RTC TIMEOUT will not expire, i.e. if RTC has an hw problem it will keep looping in the RTC_Init function */
    /* USER CODE BEGIN HAL_GetTick_EarlyCall */

    /* USER CODE END HAL_GetTick_EarlyCall */
  }
  else
  {
    ret = TIMER_IF_GetTimerValue();
  }
  /* USER CODE BEGIN HAL_GetTick_2 */

  /* USER CODE END HAL_GetTick_2 */
  return ret;
}

/**
  * @note This function overwrites the __weak one from HAL
  */
void HAL_Delay(__IO uint32_t Delay)
{
  /* TIMER_IF can be based on other counter the SysTick e.g. RTC */
  /* USER CODE BEGIN HAL_Delay_1 */

  /* USER CODE END HAL_Delay_1 */
  TIMER_IF_DelayMs(Delay);
  /* USER CODE BEGIN HAL_Delay_2 */

  /* USER CODE END HAL_Delay_2 */
}

/* USER CODE BEGIN Overload_HAL_weaks */

/* USER CODE END Overload_HAL_weaks */
