/**
  ******************************************************************************
  * @file    test_protections.c
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

#define TEST_PROTECTIONS_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "test_protections.h"
#include "se_def.h"
#include "se_interface_application.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "sfu_fwimg_regions.h" /* required for corruption tests (a real user application should NOT need this file) */
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */
/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup TEST_PROTECTIONS Test protections
  * @{
  */

/** @defgroup  TEST_PROTECTIONS_Private_Defines Private Defines
  * @{
  */

/**
  * @brief  Isolated enclave Test.
  */
/*!< Address used to test SE CODE protection*/
#define TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS     ((uint32_t) SE_CALLGATE_REGION_ROM_START)
/*!< Address used to test SE VDATA protection*/
#define TEST_PROTECTIONS_SE_ISOLATED_VDATA_SRAM_ADDRESS     ((uint32_t) SE_REGION_RAM_START)

/**
  * @brief  WRP Test.
  */
/*!< Address used to test WRP protection */
#define TEST_PROTECTIONS_WRP_FLASH_ADDRESS            ((uint32_t) (KMS_DATASTORAGE_START - 0x1000))
/*!< WRP Test Size */
#define TEST_PROTECTIONS_WRP_FLASH_SIZE               ((uint32_t)0x800U)

/**
  * @brief  IWDG Test.
  */
/*!< IWDG Test delay in ms (it has to be greater than what used in SB)*/
#define TEST_PROTECTIONS_IWDG_DELAY                   ((uint32_t)10000U)

/**
  * @brief  TAMPER Test.
  */
#define TEST_PROTECTIONS_TAMPER_DELAY                 ((uint32_t)10U)         /*!< TAMPER Test delay in s */

/**
  * @brief  CORRUPT_IMAGE Test.
  */
/*!< CORRUPT_IMAGE Test: address where data will be corrupted: address of active slot + offset */
#define TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_ADDRESS(A)  ((uint32_t)(SlotStartAdd[A] \
                                                                     +SFU_IMG_IMAGE_OFFSET))
/*!< CORRUPT_IMAGE Test: size of data to be corrupted */
#define TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_SIZE     ((uint32_t)32U)

/** @defgroup  TEST_PROTECTIONS_Private_Variables Private Variables
  * @{
  */
static uint8_t uRead_WRP[TEST_PROTECTIONS_WRP_FLASH_SIZE]; /*!< RTC handler used for TAMPER Test  */
static uint32_t m_uTamperEvent = 0U;                /*!< Tamper Event */

/** @defgroup  TEST_PROTECTIONS_Private_Functions Private Functions
  * @{
  */
static void TEST_PROTECTIONS_RunWRP(void);
static void TEST_PROTECTIONS_RunTAMPER(void);
static void TEST_PROTECTIONS_RunIWDG(void);
static void TEST_PROTECTIONS_CORRUPT_RunMenu(void);
static void TEST_PROTECTIONS_PrintTestingMenu(void);
/**
  * @}
  */




/** @defgroup  TEST_PROTECTIONS_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  TEST_PROTECTIONS_Control_Functions Control Functions
  * @{
  */

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void TEST_PROTECTIONS_RunMenu(void)
{
  uint8_t key = 0U;
  uint8_t exit = 0U;

  /* Print Main Menu message */
  TEST_PROTECTIONS_PrintTestingMenu();

  while (exit == 0U)
  {
    key = 0U;

    /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the
       reload register*/
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          TEST_PROTECTIONS_CORRUPT_RunMenu();
          break;
        case '2' :
          TEST_PROTECTIONS_RunWRP();
          break;
        case '3' :
          TEST_PROTECTIONS_RunIWDG();
          break;
        case '4' :
          TEST_PROTECTIONS_RunTAMPER();
          break;
        case 'x' :
          exit = 1U;
          break;

        default:
          printf("Invalid Number !\r");
          break;
      }
      /*Print Main Menu message*/
      TEST_PROTECTIONS_PrintTestingMenu();
    }
  }
}


/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  TEST_PROTECTIONS_Private_Functions
  * @{
  */

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_PrintTestingMenu(void)
{
  printf("\r\n=================== Test Menu ============================\r\n\n");
  printf("  Test : CORRUPT ACTIVE IMAGE --------------------------- 1\r\n\n");
  printf("  Test Protection: WRP ---------------------------------- 2\r\n\n");
  printf("  Test Protection: IWDG --------------------------------- 3\r\n\n");
  printf("  Test Protection: TAMPER ------------------------------- 4\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
  printf("  Selection :\r\n\n");
}


/**
  * @brief  TEST Run WRP
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunWRP(void)
{
  uint32_t i = 0U;
  uint32_t address = 0U;

  printf("\r\n====== Test Protection: WRP ===========================\r\n\n");

  address = TEST_PROTECTIONS_WRP_FLASH_ADDRESS;

  /* 1 - Read Page to be used for restoring*/
  printf("  -- Reading 0x%x bytes at address: 0x%x (for backup)\r\n\n", TEST_PROTECTIONS_WRP_FLASH_SIZE,
         TEST_PROTECTIONS_WRP_FLASH_ADDRESS);

  for (i = 0U; i < TEST_PROTECTIONS_WRP_FLASH_SIZE; i++)
  {
    uRead_WRP[i] = *((uint8_t *)(address + i));
  }

  /* 2 - Erasing page */
  printf("  -- Erasing 0x%x bytes at address: 0x%x\r\n\n", TEST_PROTECTIONS_WRP_FLASH_SIZE,
         TEST_PROTECTIONS_WRP_FLASH_ADDRESS);

  /* Check that it is not allowed to erase this page */
  if (FLASH_If_Erase_Size((void *)address, TEST_PROTECTIONS_WRP_FLASH_SIZE) != HAL_OK)
  {
    /* Error returned during programmation. */
    /* Check that WRPERR flag is set */
    if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) != 0U)
    {
      printf("-- !! HAL_FLASH_ERROR_WRP: FLASH Write protected error flag !!\r\n\n");
    }
  }
  else
  {
    /* 3 - Writing Data previously read*/
    if (FLASH_If_Write((void *)address, uRead_WRP, TEST_PROTECTIONS_WRP_FLASH_SIZE) != HAL_OK)
    {
      /* Error returned during programmation. */
      printf("-- !! HAL_FLASH_ERROR: FLASH Write error\r\n\n");

      /* Check that WRPERR flag is set */
      if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) != 0U)
      {
        printf("-- !! HAL_FLASH_ERROR_WRP: FLASH Write protected error flag !!\r\n\n");
      }
    }
    else
    {
      printf("  -- Written successfully at address: 0x%x\r\n\n", TEST_PROTECTIONS_WRP_FLASH_ADDRESS);

      /*No Errors detected means WRP was not enabled*/
      printf("  -- !! WRP protection is NOT ENABLED !!\r\n\n");
    }
  }
}

/**
  * @brief  TEST Run CORRUPT_IMAGE
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunCORRUPT(uint32_t slot_number)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  uint8_t pattern[TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_SIZE] = {0};

  /* On this series, there is no MPU to disable to allow flash corruption. */

  /* Erase first sector of active slot */
  printf("  -- Erasing 0x%x bytes at address: 0x%x\r\n", TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_SIZE,
         TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_ADDRESS(slot_number));
  printf("  -- At next boot Signature Verification will fail. Download a new FW to restore FW image !!\r\n\n");

  /* On this series, the memory corruption is performed by writing again the flash (but not header).
     The header is preserved for anti-rollback check. */
  ret = FLASH_If_Write((void *)(TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_ADDRESS(slot_number)), (void *) &pattern,
                       TEST_PROTECTIONS_CORRUPT_IMAGE_FLASH_SIZE);

  /* This code may not be reached, due to the memory corruption performed.
     In this case, the execution will probably trig hard fault exception (while (1)),
     then watchdog reset. */
  if (ret == HAL_OK)
  {
    NVIC_SystemReset();
  }
  else
  {
    printf("-- !! HAL_FLASH_ERROR_CORRUPT_IMAGE: erasing failure ...\r\n\n");
  }
}

/**
  * @brief  Display the corruption menu
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_CORRUPT_PrintMenu(void)
{
  printf("\r\n============  Test: CORRUPT ACTIVE IMAGE ============\r\n\n");
  printf("  Corrupt image from SLOT_ACTIVE_1 ---------------------- 1\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Run get firmware info menu.
  * @param  None
  * @retval HAL Status.
  */
static void TEST_PROTECTIONS_CORRUPT_RunMenu(void)
{
  uint8_t key = 0U;
  uint32_t exit = 0U;
  uint32_t slot_number = 0U;

  /*Print Main Menu message*/
  TEST_PROTECTIONS_CORRUPT_PrintMenu();

  while (exit == 0U)
  {
    key = 0U;
    slot_number = 0U;

    /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload
       register */
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          slot_number = SLOT_ACTIVE_1;
          break;
        case 'x' :
          exit = 1U;
          break;
        default:
          printf("Invalid Number !\r");
          break;
      }

      if (exit != 1U)
      {
        if (SlotStartAdd[slot_number] == 0U)
        {
          printf("SLOT_ACTIVE_%d is not configured !\r", slot_number);
        }
        else
        {
          TEST_PROTECTIONS_RunCORRUPT(slot_number);
        }

        /*Print Main Menu message*/
        TEST_PROTECTIONS_CORRUPT_PrintMenu();
      }
    }
  }
}


/**
  * @brief  TEST Run TAMPER
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunTAMPER(void)
{
  uint32_t i = 0U;
  m_uTamperEvent = 0U;

  printf("\r\n====== Test Protection: TAMPER ========================\r\n\n");
  /* Print instructions*/
  printf("  -- Pull PC13 (CN7.23) to GND \r\n\n");
  printf("  -- -- Note: sometimes it may be enough to put your finger close to PC13 (CN7.23)\r\n\n");
  printf("  -- Should reset if TAMPER is enabled. \r\n\n");
  printf("  Waiting for 10 seconds...\r\n\n")  ;

  /* #2 - Wait 10 seconds*/
  while ((i < TEST_PROTECTIONS_TAMPER_DELAY) && (m_uTamperEvent == 0U))
  {
    /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload register
     */
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
    HAL_Delay(1000U);
    i++;
  }
  if (m_uTamperEvent == 0U)
  {
    printf("\r\n\n  -- Waited 10 seconds, if you have connected TAMPER pin to GND it means TAMPER protection ");
    printf("is NOT ENABLED !! \r\n\n");
  }
  else
  {
    printf("\r\n\n  -- TAMPER Event detected!!\r\n\n  -- System reset requested!!!\r\n\n");
    NVIC_SystemReset();
  }
}

/**
  * @brief  TEST Run IWDG
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunIWDG(void)
{
  printf("\r\n====== Test Protection: IWDG ===========================\r\n\n");

  /* Wait for TEST_PROTECTIONS_IWDG_DELAY*/
  printf("  -- Waiting %d (ms). Should reset if IWDG is enabled. \r\n\n", TEST_PROTECTIONS_IWDG_DELAY);

  HAL_Delay(TEST_PROTECTIONS_IWDG_DELAY);

  /* No Reset means IWDG was not enabled*/
  printf("  -- !! IWDG protection is NOT ENABLED !!\r\n\n");
}

/**
  * @}
  */

/** @defgroup TEST_PROTECTIONS_Callback_Functions Callback Functions
  * @{
  */

/**
  * @brief  Implement the Cube_Hal Callback generated on the Tamper IRQ.
  * @param  None
  * @retval None
  */
void CALLBACK_Antitamper(void)
{
  /*Set tamper event variable*/
  m_uTamperEvent = 1U;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
