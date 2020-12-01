/**
  ******************************************************************************
  * @file    mbmuxif_sys.c
  * @author  MCD Application Team
  * @brief   allows CM4 applic to handle the SYSTEM MBMUX channel.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "mbmuxif_sys.h"
#include "stm32_mem.h"
#include "cmsis_os.h"
#include "stm32_timer.h"
#include "sys_app.h"
#include "msg_id.h"
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
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;
static __IO uint8_t AllowSeqencerForSysCmd = 0;
static __IO uint8_t MbSystemRespRcvFlag;

UTIL_MEM_PLACE_IN_SECTION("MAPPING_TABLE") static  MBMUX_ComTable_t MBSYS_RefTable UTIL_MEM_ALIGN(4) ;

MBMUX_ComTable_t *pMb_RefTable = &MBSYS_RefTable;
MBMUX_ComParam_t *SystemComObj;

UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemCmdRespBuff[MAX_PARAM_OF_SYS_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemNotifAckBuff[MAX_PARAM_OF_SYS_NOTIF_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemPrioACmdRespBuff[MAX_PARAM_OF_SYS_PRIOA_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t
aSystemPrioANotifAckBuff[MAX_PARAM_OF_SYS_PRIOA_NOTIF_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemPrioBCmdRespBuff[MAX_PARAM_OF_SYS_PRIOB_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t
aSystemPrioBNotifAckBuff[MAX_PARAM_OF_SYS_PRIOB_NOTIF_FUNCTIONS];/*shared*/

static osSemaphoreId_t Sem_MbSystemRespRcv;

osThreadId_t Thd_SysNotifRcvProcessId;

const osThreadAttr_t Thd_SysNotifRcvProcess_attr =
{
  .name = CFG_MB_SYS_PROCESS_NAME,
  .attr_bits = CFG_MB_SYS_PROCESS_ATTR_BITS,
  .cb_mem = CFG_MB_SYS_PROCESS_CB_MEM,
  .cb_size = CFG_MB_SYS_PROCESS_CB_SIZE,
  .stack_mem = CFG_MB_SYS_PROCESS_STACK_MEM,
  .priority = CFG_MB_SYS_PROCESS_PRIORITY,
  .stack_size = CFG_MB_SYS_PROCESS_STACk_SIZE
};
static void Thd_SysNotifRcvProcess(void *argument);

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MBMUXIF_IsrSystemRespRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemNotifRcvCb(void *ComObj);
static void MBMUXIF_TaskSystemNotifRcv(void);
static void MBMUXIF_IsrSystemPrioARespRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioANotifRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioBRespRcvCb(void *ComObj);
static void MBMUXIF_IsrSystemPrioBNotifRcvCb(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief Inits the MBMUX and registers SYS channel to the mailbox and SYS task to the sequencer
  * @param none
  * @retval   0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS
  */
int8_t MBMUXIF_SystemInit(void)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_SystemInit_1 */

  /* USER CODE END MBMUXIF_SystemInit_1 */

  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  if (OptionsBytesStruct.IPCCdataBufAddr != (uint32_t) pMb_RefTable)
  {
    APP_PPRINTF("\r\n WARNING: There is a difference between the MAPPING_TABLE placement in memory: 0x%X \r\n",  (uint32_t) pMb_RefTable);
    APP_PPRINTF(" and the address calculated according to the IPCCDBA option byte: 0x%X \r\n", OptionsBytesStruct.IPCCdataBufAddr);
    APP_PPRINTF(" The execution enters now in a while(1){}  \r\n");
    APP_PPRINTF(" please check the CM4\\MbMux\\mbmuxif_sys.c or update the scatter file \r\n\r\n");

    /* USER CODE BEGIN MBMUXIF_SystemInit_Prevent_OB_Programming */
      /* If you are sure you need to reprogram the IPCCDBA set #if to 0 (to avoid while(1){}) */
      /* Please first check the  CM4 scatter file.*/
      /* Make sure that the MAPPING_TABLE is placed in SRAM2 (memory with retention) */
      /* Examples of scatter files available in the LoraWAN and Sigfox DualCore Applications pack*/
      /* */
      /* After HAL_FLASH_OB_Launch you will have to unplug and plug the board from power after this operation.*/
      /* Additionally possible Issue under investigation, i.e.: */
      /* In some cases happen that unplugging the chip power is not sufficient */
      /* if so you need to reboot from RAM putting the BOOT0 pin to 3v3 */
      /* attach with Cube Programmer and full erase the chip memory. */
      /* */
      /* PS: of course if you are attached with the debugger, it will not respond anymore after HAL_FLASH_OB_Launch*/
#if 1
    /* USER CODE END MBMUXIF_SystemInit_Prevent_OB_Programming */
    while(1){}
#else

    /* write FLASH->IPCCBR option byte */
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();

    /*Write IPCCDBA Option Byte based on (FLASH_IPCC_data_buffer - SRAM1_BASE) >> 4*/
    OptionsBytesStruct.OptionType  = OPTIONBYTE_IPCC_BUF_ADDR;
    OptionsBytesStruct.IPCCdataBufAddr = (uint32_t) pMb_RefTable;

    if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /*
      Error occurred while setting option bytes configuration.
      User can add here some code to deal with this error.
      To know the code error, user can call function 'HAL_FLASH_GetError()'
      */
      Error_Handler();
    }

    /* Start the Option Bytes programming process */
    if (HAL_FLASH_OB_Launch() != HAL_OK)
    {
      /*
      Error occurred while reloading option bytes configuration.
      User can add here some code to deal with this error.
      To know the code error, user can call function 'HAL_FLASH_GetError()'
      */
      Error_Handler();
    }

    /* Notice: After HAL_FLASH_OB_Launch() the chip is reset so next code is not executed */
    /* Implemented anyway to be aligned with other projects (SBSFU, etc) */
    /* Lock the Options Bytes ***************************************************/
    (void) HAL_FLASH_OB_Lock();
    /* Lock the Flash to disable the flash control register access  *********/
    (void) HAL_FLASH_Lock();
#endif
  }
  Sem_MbSystemRespRcv = osSemaphoreNew(1, 0, NULL);   /*< Create the semaphore and make it busy at initialization */

  MBSYS_RefTable.ChipRevId = LL_DBGMCU_GetRevisionID();

  MBMUX_Init(pMb_RefTable);

  ret = MBMUX_RegisterFeature(FEAT_INFO_SYSTEM_ID, MBMUX_CMD_RESP, MBMUXIF_IsrSystemRespRcvCb, aSystemCmdRespBuff, sizeof(aSystemCmdRespBuff));
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_SYSTEM_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemNotifRcvCb, aSystemNotifAckBuff, sizeof(aSystemNotifAckBuff));
  }
  if (ret >= 0)
  {
    ret = 0;
    Thd_SysNotifRcvProcessId = osThreadNew(Thd_SysNotifRcvProcess, NULL, &Thd_SysNotifRcvProcess_attr);
  }

#if defined (DEBUGGER_ON) && (DEBUGGER_ON == 1)
  /* hold Cm0 from executing MBMUXIF_SystemInit after booting (after PWR_CR4_C2BOOT is set by Cm4)*/
  /* it allows Cm0 debugging step by step its own MBMUXIF_SystemInit */
  MBMUXIF_SetCpusSynchroFlag(CPUS_BOOT_SYNC_PREVENT_CPU2_TO_START);
#elif defined (DEBUGGER_ON) && (DEBUGGER_ON == 0) /* DEBUGGER_OFF */
  MBMUXIF_SetCpusSynchroFlag(CPUS_BOOT_SYNC_ALLOW_CPU2_TO_START);
#else
#warning "DEBUGGER_ON not defined or out of range <0,1>"
#endif /* DEBUGGER_ON */

  /* USER CODE BEGIN MBMUXIF_SystemInit_Last */

  /* USER CODE END MBMUXIF_SystemInit_Last */
  return ret;
}

/**
  * @brief Registers to the mailbox a SYSTEM feature channel used on higher PRIORITY
  * @param none
  * @retval   0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS
  */
int8_t MBMUXIF_SystemPrio_Add(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  int8_t ret=0;

  /* USER CODE BEGIN MBMUXIF_SystemPrio_Add_1 */

  /* USER CODE END MBMUXIF_SystemPrio_Add_1 */

  switch (SystemPrioFeat)
  {
    case FEAT_INFO_SYSTEM_CMD_PRIO_A_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_CMD_RESP, MBMUXIF_IsrSystemPrioARespRcvCb, aSystemPrioACmdRespBuff, sizeof(aSystemPrioACmdRespBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemPrioANotifRcvCb, aSystemPrioANotifAckBuff, sizeof(aSystemPrioANotifAckBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_CMD_PRIO_B_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_CMD_RESP, MBMUXIF_IsrSystemPrioBRespRcvCb, aSystemPrioBCmdRespBuff, sizeof(aSystemPrioBCmdRespBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_NOTIF_ACK, MBMUXIF_IsrSystemPrioBNotifRcvCb, aSystemPrioBNotifAckBuff, sizeof(aSystemPrioBNotifAckBuff));
      break;
    }
    default:
      break;
  }

  if (ret >= 0)
  {
    /* Note: trace works on ISR, no need to associate a SEQ_TASK */

    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(SystemPrioFeat);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  /* USER CODE BEGIN MBMUXIF_SystemPrio_Add_Last */

  /* USER CODE END MBMUXIF_SystemPrio_Add_Last */
  return ret;
}

/**
  * @brief Set synchro flag between the two CPUs
  * @param flag: 0xFFFF: hold Cm0 before it runs its MBMUX_Init,
  *              0x5555: Cm0 allowed to run its MBMUX_Init
  *              0xAAAA: Cm0 has completed initialisation
  *              0x9999: RTC PRIO CHANNEL has been registered
  * @retval  none
  */
void MBMUXIF_SetCpusSynchroFlag(uint16_t flag)
{
  pMb_RefTable->SynchronizeCpusAtBoot = flag;
  /* USER CODE BEGIN MBMUXIF_SetCpusSynchroFlag */

  /* USER CODE END MBMUXIF_SetCpusSynchroFlag */
}

/**
  * @brief gives green light to Cm0 to Initialised MBMUX on its side
  * @param none
  * @retval  none
  */
void MBMUXIF_WaitCm0MbmuxIsInitialised(void)
{
  while (pMb_RefTable->SynchronizeCpusAtBoot != CPUS_BOOT_SYNC_CPU2_INIT_COMPLETED)
  {
  }
  /* USER CODE BEGIN MBMUXIF_WaitCm0MbmuxIsInitialised */

  /* USER CODE END MBMUXIF_WaitCm0MbmuxIsInitialised */
}

/**
  * @brief gives back the pointer to the com buffer associated to System feature Cmd
  * @param none
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(SystemPrioFeat, MBMUX_CMD_RESP);
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureCmdComPtr */

  /* USER CODE END MBMUXIF_GetSystemFeatureCmdComPtr */
  if (com_param_ptr == NULL)
  {
    while (1) {} /* ErrorHandler() : feature isn't registered */
  }
  return com_param_ptr;
}

/**
  * @brief To allow SystemCmd to use the sequencer (hence entering in low power)
  * @note  For compatib with RTOS, the sequencer (scheduler) is not used until the main loop process is reached (after Init). The applic can call this API at the end of the Init
  * @param none
  * @retval   none
  */
void MBMUXIF_SystemAllowSequencerForSysCmd(void)
{
  AllowSeqencerForSysCmd = 1;
  /* USER CODE BEGIN MBMUXIF_SystemAllowSequencerForSysCmd */

  /* USER CODE END MBMUXIF_SystemAllowSequencerForSysCmd */
}

/**
  * @brief Sends a System-Cmd via Ipcc and Wait for the response
  * @param none
  * @retval   none
  */
void MBMUXIF_SystemSendCmd(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendCmd_1 */

  /* USER CODE END MBMUXIF_SystemSendCmd_1 */

  if (SystemPrioFeat == FEAT_INFO_SYSTEM_ID)
  {

    MbSystemRespRcvFlag = 0;  /* To avoid using Sequencer during Init sequence for SysCmd  */
    if (MBMUX_CommandSnd(FEAT_INFO_SYSTEM_ID) == 0)
    {
      if (AllowSeqencerForSysCmd)
      {
        osSemaphoreAcquire(Sem_MbSystemRespRcv, osWaitForever);
      }
      else
      {
        /* To avoid using Sequencer (& LowPower) during Init sequence for SysCmd  */
        while (!MbSystemRespRcvFlag) {}
      }
    }
    else
    {
      Error_Handler();
    }
  }
  else
  {
  /* USER CODE BEGIN MBMUXIF_SystemSendCmd_2 */

  /* USER CODE END MBMUXIF_SystemSendCmd_2 */
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendCmd_Last */

  /* USER CODE END MBMUXIF_SystemSendCmd_Last */
}

/**
  * @brief Sends a System-Ack  via Ipcc without waiting for the ack
  * @param none
  * @retval   none
  */
void MBMUXIF_SystemSendAck(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendAck_1 */

  /* USER CODE END MBMUXIF_SystemSendAck_1 */
  if (MBMUX_AcknowledgeSnd(SystemPrioFeat) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendAck_Last */

  /* USER CODE END MBMUXIF_SystemSendAck_Last */
}

/* USER CODE BEGIN ExpoF */

/* USER CODE END ExpoF */

/* Exported services --------------------------------------------------------*/

/**
  * @brief   Gets the pointer to CM0PLUS capabilities info table
  * @param   none
  * @retval  pointer to CM0PLUS capabilities info table
  */
FEAT_INFO_List_t *MBMUXIF_SystemSendCm0plusInfoListReq(void)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t ret = 0;
  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusInfoListReq_1 */

  /* USER CODE END MBMUXIF_SystemSendCm0plusInfoListReq_1 */

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_GET_INFO_LIST_MSG_ID;
    com_obj->ParamCnt = 0;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for ack event */

    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }

  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusInfoListReq_Last */

  /* USER CODE END MBMUXIF_SystemSendCm0plusInfoListReq_Last */
  return (FEAT_INFO_List_t *) ret;   /* need to verify */
}

/**
  * @brief returns the pointer to the Capability Info table of a given feature
  * @param e_featID ID of the feature from which retrieve the ptr
  * @retval  LoRaMacInfoTable pointer
  */
FEAT_INFO_Param_t *MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_IdTypeDef e_featID)
{
  FEAT_INFO_List_t *p_cm0plus_supprted_features_list = NULL;
  FEAT_INFO_Param_t  *p_feature = NULL;
  uint8_t i;
  uint8_t cm0plus_nr_of_supported_features;
  uint8_t found = 0;

  /* USER CODE BEGIN MBMUXIF_SystemGetFeatCapabInfoPtr_1 */

  /* USER CODE END MBMUXIF_SystemGetFeatCapabInfoPtr_1 */

  p_cm0plus_supprted_features_list = MBMUXIF_SystemSendCm0plusInfoListReq();

  if (p_cm0plus_supprted_features_list != NULL)
  {
    cm0plus_nr_of_supported_features = p_cm0plus_supprted_features_list->Feat_Info_Cnt;

    for (i = 0; i < cm0plus_nr_of_supported_features;  i++)
    {
      p_feature = i + p_cm0plus_supprted_features_list->Feat_Info_TableAddress;
      if (p_feature->Feat_Info_Feature_Id == e_featID)
      {
        found = 1;
        break;
      }
    }
  }

  if (!found)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN MBMUXIF_SystemGetFeatCapabInfoPtr_Last */

  /* USER CODE END MBMUXIF_SystemGetFeatCapabInfoPtr_Last */
  return  p_feature;
}

/**
  * @brief Asks CM0PLUS to register its Cmd and Ack callbacks relative to the requested feature
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp, 1 for Notif/Ack
  * @retval   0 ok, else error code
  */
int8_t MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_IdTypeDef e_featID)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t ret = 0;
  uint32_t *com_buffer ;
  uint16_t i = 0;
  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusRegistrationCmd_1 */

  /* USER CODE END MBMUXIF_SystemSendCm0plusRegistrationCmd_1 */

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_REGISTER_FEATURE_MSG_ID;
    com_buffer = com_obj->ParamBuf;
    com_buffer[i++] = (uint32_t) e_featID;
    com_obj->ParamCnt = i;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for event */
    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }

  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusRegistrationCmd_Last */

  /* USER CODE END MBMUXIF_SystemSendCm0plusRegistrationCmd_Last */
  return (int8_t) ret;
}

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  SYSTEM response callbacks: set event to release waiting task
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemRespRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemRespRcvCb_1 */

  /* During "SystemInit sequence" Flag is used instead of Sequencer */
  MbSystemRespRcvFlag = 1;
  if (AllowSeqencerForSysCmd) /* To avoid using Sequencer during Init sequence */
  {
    osSemaphoreRelease(Sem_MbSystemRespRcv);
  }
  /* USER CODE BEGIN MBMUXIF_IsrSystemRespRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemRespRcvCb_Last */
}

/**
  * @brief  SYSTEM notification callbacks: schedules a task in order to quit the ISR
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemNotifRcvCb_1 */
  SystemComObj = (MBMUX_ComParam_t *) ComObj;
  osThreadFlagsSet(Thd_SysNotifRcvProcessId, 1);
  /* USER CODE BEGIN MBMUXIF_IsrSystemNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemNotifRcvCb_Last */
}

static void Thd_SysNotifRcvProcess(void *argument)
{
  UNUSED(argument);
  /* USER CODE BEGIN Thd_SysNotifRcvProcess_1 */

  /* USER CODE END Thd_SysNotifRcvProcess_1 */
  for (;;)
  {
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
    MBMUXIF_TaskSystemNotifRcv();  /*what you want to do*/
  }
  /* USER CODE BEGIN Thd_SysNotifRcvProcess_Last */

  /* USER CODE END Thd_SysNotifRcvProcess_Last */
}

/**
  * @brief  SYSTEM task to process the notification
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_TaskSystemNotifRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskSystemNotifRcv_1 */

  /* USER CODE END MBMUXIF_TaskSystemNotifRcv_1 */
  Process_Sys_Notif(SystemComObj);
  /* USER CODE BEGIN MBMUXIF_TaskSystemNotifRcv_Last */

  /* USER CODE END MBMUXIF_TaskSystemNotifRcv_Last */
}

/**
  * @brief  SYSTEM PRIO_A response callbacks: available for new usage
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioARespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioARespRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioARespRcvCb */
}

/**
  * @brief  SYSTEM PRIO_A notification callbacks: used by RTC
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioANotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioANotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemPrioANotifRcvCb_1 */
  UTIL_TIMER_IRQ_Handler();
  /* clear the IPCC flag (necessary even if this Notif is not blocking */
  MBMUXIF_SystemSendAck(FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID);
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioANotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemPrioANotifRcvCb_Last */
}

/**
  * @brief  SYSTEM PRIO_B response callbacks: available for new usage
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioBRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBRespRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBRespRcvCb */
}

/**
  * @brief  SYSTEM PRIO_B notification callbacks: available for new usage
  * @param  pointer to the SYSTEM com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSystemPrioBNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBNotifRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBNotifRcvCb */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
