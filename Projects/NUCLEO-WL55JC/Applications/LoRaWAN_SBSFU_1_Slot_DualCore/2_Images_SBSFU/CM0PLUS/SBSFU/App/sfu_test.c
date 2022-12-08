/**
  ******************************************************************************
  * @file    sfu_test.c
  * @author  MCD Application Team
  * @brief   Test Protections module.
  *          This file provides set of firmware functions to manage Test Protections
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sfu_test.h"
#include "main.h"
#include "sfu_trace.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level.h"
#include "string.h" /* required for strncmp */

#if defined(SFU_TEST_PROTECTION)

extern RTC_HandleTypeDef RtcHandle;

/* Private defines -----------------------------------------------------------*/
#define TEST_ISOLATION 1

/* Automatic tests : list of tests (Address, Message, Operation */
/* Flash programming by 64 bits */
#define SHIFT_FLASH_WRITE (8U-1U)

/* Private variables ---------------------------------------------------------*/
static ProtectionTest_t aProtectTests[] =
{
#ifdef SFU_MPU_PROTECT_ENABLE
  {SFU_RAM_BASE, "MPU SRAM start add", TEST_EXECUTE},
  {SFU_RAM_END - 3U, "MPU SRAM end add", TEST_EXECUTE},             /* -3 : previous 32 bits for execute test */
  {PERIPH_BASE, "Peripheral base add", TEST_EXECUTE},
  {((uint32_t)0x5FFFFFFF - 3U), "Peripheral end address", TEST_EXECUTE},    /* -3 : previous 32 bits for execute test */
  /* READ_FLASH is possible only in privileged mode */
  {INTVECT_START, "Init. Vector", TEST_READ_FLASH},
  {SLOT_ACTIVE_1_START, "MPU code active slot1 begin add", TEST_EXECUTE},
  {SLOT_ACTIVE_1_END - 3U, "MPU code active1 slot end add", TEST_EXECUTE}, /* -3 : alignment for 32bits execute test */
  {SLOT_ACTIVE_2_START, "MPU code active slot2 begin add", TEST_EXECUTE},
  {SLOT_ACTIVE_2_END - 3U, "MPU code active slot2 end add", TEST_EXECUTE}, /* -3 : alignment for 32bits execute test */
  {SLOT_ACTIVE_3_START, "MPU code active slot3 begin add", TEST_EXECUTE},
  {SLOT_ACTIVE_3_END - 3U, "MPU code active slot3 end add", TEST_EXECUTE}, /* -3 : alignment for 32bits execute test */
#endif /* SFU_MPU_PROTECT_ENABLE */
#ifdef SFU_WRP_PROTECT_ENABLE
  {INTVECT_START, "WRP start add", TEST_WRITE_FLASH},
  {SFU_ROM_ADDR_END - SHIFT_FLASH_WRITE, "WRP end add", TEST_WRITE_FLASH}, /* -7 : alignment for 64bits writing test */
  {SB_REGION_ROM_START, "WRP internal (SB) add", TEST_ERASE_FLASH},
#endif /* SFU_WRP_PROTECT_ENABLE */
#ifdef SFU_PCROP_PROTECT_ENABLE
  {SE_KEY_REGION_ROM_START, "PCROP start add", TEST_WRITE_FLASH},
  {SE_KEY_REGION_ROM_END - SHIFT_FLASH_WRITE, "PCROP end add", TEST_WRITE_FLASH}, /* -7 : alignment for 64bits
                                                                                         writing test */
  {SE_KEY_REGION_ROM_START, "PCROP start add", TEST_READ_FLASH},
  {SE_KEY_REGION_ROM_END, "PCROP end add", TEST_READ_FLASH},
#endif /* SFU_PCROP_PROTECT_ENABLE */
#if defined(TEST_ISOLATION)
  {SE_CODE_REGION_ROM_START, "Isolated code start add", TEST_WRITE_FLASH},        /* WRP protection applies  */
  {SE_CODE_REGION_ROM_END - SHIFT_FLASH_WRITE, "Isolated code end add", TEST_WRITE_FLASH},/* -7 : alignment for
                                                                         64bits writing test + WRP protection applies */
  {SE_CODE_REGION_ROM_START, "Isolated code start add", TEST_READ_FLASH},
  {SE_CODE_REGION_ROM_END, "Isolated code end add", TEST_READ_FLASH},
  {SE_STARTUP_REGION_ROM_START + 1U, "Isolated startup add", TEST_EXECUTE}, /* +1 : contains the SE startup code
                                                                    that initializes all the variables in the binary. */
  {SE_CODE_REGION_ROM_START, "Isolated code start add", TEST_EXECUTE},
  {SE_CODE_REGION_ROM_END - 3U, "Isolated code end add", TEST_EXECUTE}, /* -3 : alignment for 32bits execute test */
  {SE_REGION_RAM_START, "Isolated Vdata add", TEST_WRITE_RAM},
  {SE_REGION_RAM_END - 3U, "Isolated Vdata add", TEST_WRITE_RAM},     /* -3 : alignment for 32bits writing test */
  {SE_REGION_RAM_START, "Isolated Vdata add", TEST_READ_RAM},
  {SE_REGION_RAM_END, "Isolated Vdata add", TEST_READ_RAM},
  {SE_REGION_RAM_START, "Isolated Vdata add", TEST_EXECUTE},
  {SE_REGION_RAM_END - 3U, "Isolated Vdata add", TEST_EXECUTE},       /* -3 : alignment for 32bits writing test */
  {SLOT_ACTIVE_1_HEADER + SFU_IMG_IMAGE_OFFSET - (SHIFT_FLASH_WRITE + 1U), "Isolated NVdata add", TEST_WRITE_FLASH},
  /* -8 : previous 64 bits for writing test */
  {SLOT_ACTIVE_1_HEADER + SFU_IMG_IMAGE_OFFSET - 1U, "Isolated NVdata add", TEST_READ_FLASH},/* -1 : previous 8bits
                                                                                                   for reading test */
  {SLOT_ACTIVE_1_HEADER + SFU_IMG_IMAGE_OFFSET - 4U, "Isolated NVdata add", TEST_EXECUTE},   /* -4 : previous 32bits
                                                                                                   for execute test */
  {SLOT_ACTIVE_2_HEADER + SFU_IMG_IMAGE_OFFSET - (SHIFT_FLASH_WRITE + 1U), "Isolated NVdata add", TEST_WRITE_FLASH},
  /* -8 : previous 64 bits for writing test */
  {SLOT_ACTIVE_2_HEADER + SFU_IMG_IMAGE_OFFSET - 1U, "Isolated NVdata add", TEST_READ_FLASH},/* -1 : previous 8bits
                                                                                                   for reading test */
  {SLOT_ACTIVE_2_HEADER + SFU_IMG_IMAGE_OFFSET - 4U, "Isolated NVdata add", TEST_EXECUTE},   /* -4 : previous 32bits
                                                                                                   for execute test */
  {SLOT_ACTIVE_3_HEADER + SFU_IMG_IMAGE_OFFSET - (SHIFT_FLASH_WRITE + 1U), "Isolated NVdata add", TEST_WRITE_FLASH},
  /* -8 : previous 64 bits for writing test */
  {SLOT_ACTIVE_3_HEADER + SFU_IMG_IMAGE_OFFSET - 1U, "Isolated NVdata add", TEST_READ_FLASH},/* -1 : previous 8bits
                                                                                                   for reading test */
  {SLOT_ACTIVE_3_HEADER + SFU_IMG_IMAGE_OFFSET - 4U, "Isolated NVdata add", TEST_EXECUTE},   /* -4 : previous 32bits
                                                                                                   for execute test */
#endif /* TEST_ISOLATION  */
#ifdef SFU_MPU_PROTECT_ENABLE
  /* Testing the MPU protection of the OBs */
  /*
   * WL55JC
   * 0x1FFF 7800 Reserved user option bytes & RDP (USER & RDP)
   * 0x1FFF 7818 Reserved Write protection 1A
   * 0x1FFF 7820 Reserved Write protection 1B
   */
  {(uint32_t)0x1FFF7800, "OBs @ 0x1FFF7800", TEST_WRITE_FLASH},
  {(uint32_t)0x1FFF7818, "OBs @ 0x1FFF7818", TEST_WRITE_FLASH},
  {(uint32_t)0x1FFF7820, "OBs @ 0x1FFF7820", TEST_WRITE_FLASH},
#endif /* SFU_MPU_PROTECT_ENABLE  */
  {0xAAAAAAAA, "Execution successful", TEST_END},
};


/* Automatic test : list of operation */
uint8_t aTestOperation[][20] =
{
  "write 8 bytes",
  "read 1 byte",
  "erase 512 bytes",
  "write 4 bytes",
  "read 1 byte",
  "execute",
  "end"
};

/* Private function prototypes -----------------------------------------------*/
static void SFU_TEST_Protection(void);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Automatic test of PCROP/WRP/FWALL/MPU protections
  * @param  None.
  * @retval None.
  */
static void SFU_TEST_Protection(void)
{
  uint32_t test_idx;
  uint32_t flashErrCode = 0;
  uint64_t pattern = 0U;
  uint32_t page_error = 0U;
  FLASH_EraseInitTypeDef p_erase_init;
  TestStatus status = TEST_IN_PROGRESS;
  __IO uint8_t tmp;
  void (*func)(void);

  /*
    * Now checking the security
    * We will try to access protected @ and run protected code.
    * Each time a a flash error will be raised or a reset will be generated by IP protection.
    * At next boot we read the backup register to check the next @ until all addresses have been verified.
    * WatchDog should be disabled : while(1) loop in case HardFault when trying to execute code @ address protected by
    * FWALL/Code Isolation
    */

  do
  {
    /* slow down execution */
    HAL_Delay(100);

    /* Increment test number for next execution */
    test_idx = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, (test_idx + 1));
    printf("\r\n= [TEST] %s @ %s %08x", aTestOperation[aProtectTests[test_idx].type], aProtectTests[test_idx].msg,
           aProtectTests[test_idx].address);

    /* Remove test for slots not configured (taking into account SFU_IMG_IMAGE_OFFSET) */
    if ((aProtectTests[test_idx].address != 0U) && (aProtectTests[test_idx].address < 0xFFFFFF00U)
        && (aProtectTests[test_idx].address > 0x00000FFFU))
    {
      switch (aProtectTests[test_idx].type)
      {
        /* Trying to write 64bits in FLASH : WRP flag is set for WRP or PCROP protected area
           or reset generated if under FWALL or MPU protection */
        case TEST_WRITE_FLASH :
          HAL_FLASH_Unlock();
          HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, aProtectTests[test_idx].address, pattern);
          HAL_FLASH_Lock();
          flashErrCode = HAL_FLASH_GetError();
          printf("\r\nflash error code: %x", flashErrCode);
          if ((flashErrCode & HAL_FLASH_ERROR_WRP) == 0U)
          {
            status = TEST_ERROR;
          }

          /*
           * OBs protection test: no MPU enabled because the OB cannot be written directly.
           * A flash error should occur.
           */
          break;

        /* Trying to read in FLASH : RDP set in case of PCROP protected area
           or reset generated if under FWALL or MPU protection */
        case TEST_READ_FLASH :
          tmp = *(uint8_t *)(aProtectTests[test_idx].address);
          HAL_Delay(1);                                                 /* ensure Flag is set */
          /*
           * When Isolation activated : RESET should be generated
           * So the test is FAILED if we reach this line
           */
          if (0 == strncmp("Isolated", (const char *)aProtectTests[test_idx].msg, 8))
          {
            status = TEST_ERROR;
          }

          break;

        /* Trying to erase 512 bytes in FLASH : WRP flag set for WRP or PCROP protected area
           or reset generated if under FWALL or MPU protection */
        case TEST_ERASE_FLASH :
          HAL_FLASH_Unlock();
          p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
          p_erase_init.Page        = SFU_LL_FLASH_GetPage(aProtectTests[test_idx].address);
          p_erase_init.NbPages     = 1;
          HAL_FLASHEx_Erase(&p_erase_init, &page_error);
          HAL_FLASH_Lock();
          if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) == 0U)
          {
            status = TEST_ERROR;
          }
          break;

        /* Trying to write in RAM : reset generated if under FWALL or MPU protection */
        case TEST_WRITE_RAM :
          *(uint32_t *)aProtectTests[test_idx].address = 0x00000000;
          status = TEST_ERROR;
          break;

        /* Trying to read in RAM : reset generated if under FWALL or MPU protection */
        case TEST_READ_RAM :
          tmp = *(uint8_t *)aProtectTests[test_idx].address;
          printf(" value : %d ", tmp);
          status = TEST_ERROR;
          break;

          /* Trying to execute code : reset generated if under FWALL or MPU protection */
        case TEST_EXECUTE :
          func = (void(*)(void))(aProtectTests[test_idx].address);
          func();
          status = TEST_ERROR;
          break;

        /* End of execution */
        case TEST_END :
          status = TEST_COMPLETED;
          break;

        default :
          break;
      }
    }
  } while (status == TEST_IN_PROGRESS) ;

  if (status == TEST_ERROR)
  {
    SFU_TEST_Error();
  }
  else if (status == TEST_COMPLETED)
  {
    /* reset for next run (need a RESET of the platform to restart a session) */
    printf("\b\b\b\b\b\b\b\b");
    printf("\r\nYou can RESET the platform to restart the tests.\r\n");
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_DONE);              /* no more test in progress */
  }
}

/**
  * @brief  Initialization of backup register and start test
  * @param  None.
  * @retval None.
  */
void SFU_TEST_Init(void)
{
  /* Use register DR0 to know which if the test sequence is in progress or done */
  /* Use register DR1 to know which test to run after a reset */

  if (MAGIC_TEST_DONE == HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0))      /* Test done */
  {
    return;
  }
  else if (MAGIC_TEST_INIT != HAL_RTCEx_BKUPRead(&RtcHandle,
                                                 RTC_BKP_DR0)) /* Test not done nor in progress: Init magic (DR0) and
                                                                  reset test index (DR1) */
  {
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_INIT);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x0000);
  }

  /* Start test */
  SFU_TEST_Protection();
}

/**
  * @brief  Reset backup register to be able to execute a new test sequence
  * @param  None.
  * @retval None.
  */
void SFU_TEST_Reset(void)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_INIT);
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x0000);
}

/**
  * @brief  Error detected during test sequence
  * @param  None.
  * @retval None.
  */
void SFU_TEST_Error(void)
{
  printf(" ===> KO!!!!!");
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_DONE);
}

#endif /* SFU_TEST_PROTECTION */