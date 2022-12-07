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
#include "mpu.h"
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
  * @brief  Secure Memory Test.
  */
/*!< Address used to test HDP */
#define TEST_PROTECTIONS_HDP_FLASH_ADDRESS          ((uint32_t) SE_KEY_REGION_ROM_START)

/**
  * @brief  WRP Test.
  */
/*!< Address used to test WRP protection */
#define TEST_PROTECTIONS_WRP_FLASH_ADDRESS            ((uint32_t) SE_IF_REGION_ROM_START)
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

/**
  * @brief  GTZC Test.
  */
/*!< GTZC Test error code */
#define TEST_GTZC_ERROR_CODE                          ((uint32_t)0xFFU)

/** @defgroup  TEST_PROTECTIONS_Private_Variables Private Variables
  * @{
  */
static uint8_t uRead_WRP[TEST_PROTECTIONS_WRP_FLASH_SIZE];
static __IO uint32_t m_uTamperEvent = 0U;                /*!< Tamper Event */
static __IO uint32_t m_uGtzcTzicEvent = 0U;              /*!< GTZC_TZIC Event */
/**
  * @}
  */

/** @defgroup  TEST_PROTECTIONS_Private_Functions Private Functions
  * @{
  */
static void TEST_PROTECTIONS_RunSE_CODE(void);
static void TEST_PROTECTIONS_RunSE_VDATA(void);
static void TEST_PROTECTIONS_RunSecUserMem_CODE(void);
static void TEST_PROTECTIONS_RunWRP(void);
static void TEST_PROTECTIONS_RunTAMPER(void);
static void TEST_PROTECTIONS_RunIWDG(void);
static void TEST_PROTECTIONS_CORRUPT_RunMenu(void);
static void TEST_PROTECTIONS_Run_GTZC(void);
static void TEST_PROTECTIONS_GtzcPrintTestingSubMenu(void);
static void TEST_PROTECTIONS_Run_KMS_DATA_STORAGE(void);
static void TEST_PROTECTIONS_Run_SYNC_FLAG(void);
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
          TEST_PROTECTIONS_RunSE_CODE();
          break;
        case '3' :
          TEST_PROTECTIONS_RunSE_VDATA();
          break;
        case '4' :
          TEST_PROTECTIONS_RunSecUserMem_CODE();
          break;
        case '5' :
          TEST_PROTECTIONS_RunWRP();
          break;
        case '6' :
          TEST_PROTECTIONS_RunIWDG();
          break;
        case '7' :
          TEST_PROTECTIONS_RunTAMPER();
          break;
        case '8' :
          TEST_PROTECTIONS_Run_GTZC();
          break;
        case '9' :
          TEST_PROTECTIONS_Run_KMS_DATA_STORAGE();
          break;
        case 'a' :
          TEST_PROTECTIONS_Run_SYNC_FLAG();
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
  printf("  Test SE isolation - CODE ------------------------------ 2\r\n\n");
  printf("  Test SE isolation - VDATA ----------------------------- 3\r\n\n");
  printf("  Test Protection: HDP ---------------------------------- 4\r\n\n");
  printf("  Test Protection: WRP ---------------------------------- 5\r\n\n");
  printf("  Test Protection: IWDG --------------------------------- 6\r\n\n");
  printf("  Test Protection: TAMPER ------------------------------- 7\r\n\n");
  printf("  Test Protection: GTZSC -------------------------------- 8\r\n\n");
  printf("  Test KMS_DataStorage isolation ------------------------ 9\r\n\n");
  printf("  Test synchronization flag protection ------------------ a\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
  printf("  Selection :\r\n\n");
}

/**
  * @brief  TEST Run Secure User Memory
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunSecUserMem_CODE(void)
{
  uint32_t u_read_key = 0;
  printf("\r\n====== Test Protection: HDP \r\n\n");
  printf("  -- Reading address: 0x%x\r\n\n", TEST_PROTECTIONS_HDP_FLASH_ADDRESS);

  /* Try to read a 32-bit data from the Flash protected by HDP */
  u_read_key = *((uint32_t *)TEST_PROTECTIONS_HDP_FLASH_ADDRESS);

  /* Should not get here if HDP is available and enabled  */
  printf("  -- Address: 0x%x = 0x%x\r\n\n", TEST_PROTECTIONS_HDP_FLASH_ADDRESS, u_read_key);
  if(u_read_key == 0U)
  {
    printf("  -- 0 has been read: HDP is enabled.\r\n\n");
  }
  else
  {
    printf("  -- !! HDP privileged CODE protection is NOT ENABLED !!\r\n\n");
  }
}

/**
  * @brief  TEST Run SE_CODE
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunSE_CODE(void)
{

  uint8_t code[4U];
  m_uGtzcTzicEvent = 0;
  printf("\r\n====== Test Protection: GTZC privileged - CODE ==========\r\n\n");
  printf("  -- Dumping code\r\n\n");

  /* Dumping the first 4 bytes */
  code[0] = * (uint8_t *)(TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS);
  code[1] = * (uint8_t *)(TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS + 1);
  code[2] = * (uint8_t *)(TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS + 2);
  code[3] = * (uint8_t *)(TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS + 3);

  printf("  -- Dump : %x %x %x %x \r\n\n", code[0], code[1], code[2], code[3]);
  if (m_uGtzcTzicEvent == 0U)
  {
   printf("  -- !! GTZC privileged CODE protection is NOT ENABLED !!\r\n\n");
  }
  else
  {
    printf("\r\n\n  -- TZIC Event detected!!\r\n\n  -- System reset requested!!!\r\n\n");
    SVC_NVIC_SystemReset();
  }
}
/**
  * @brief  TEST Run SE_VDATA
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_RunSE_VDATA(void)
{
  uint32_t u_read_fw_vdata = 0;
  m_uGtzcTzicEvent = 0;
  printf("\r\n====== Test Protection: GTZC privileged - VDATA ==========\r\n\n");
  printf("  -- Reading address: 0x%x\r\n\n", TEST_PROTECTIONS_SE_ISOLATED_VDATA_SRAM_ADDRESS);

  /* Try to read a 32-bit data from the SRAM2 protected by the GTZC */
  u_read_fw_vdata = *((uint32_t *)TEST_PROTECTIONS_SE_ISOLATED_VDATA_SRAM_ADDRESS);

  /* Should not get here if MPU was available and enabled  */
  printf("  -- Address: 0x%x = %d\r\n\n", TEST_PROTECTIONS_SE_ISOLATED_VDATA_SRAM_ADDRESS, u_read_fw_vdata);
  if (m_uGtzcTzicEvent == 0U)
  {
    printf("  -- !! GTZC privileged VDATA protection is NOT ENABLED !!\r\n\n");
  }
  else
  {
    printf("\r\n\n  -- TZIC Event detected!!\r\n\n  -- System reset requested!!!\r\n\n");
    SVC_NVIC_SystemReset();
  }
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

  /* On this series, MPU should be disable to allow flash corruption.
     Switching in privileged mode is requested to be able to disable MPU */
  printf("  -- Disable MPU protection to be able to erase\r\n");
  SVC_MPU_Disable();

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
    SVC_NVIC_SystemReset();
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
  printf("  Corrupt image from SLOT_ACTIVE_2 ---------------------- 2\r\n\n");
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
        case '2' :
          slot_number = SLOT_ACTIVE_2;
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
    SVC_NVIC_SystemReset();
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
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void TEST_PROTECTIONS_GtzcPrintTestingSubMenu(void)
{
  printf("\r\n====== Test Protection: GTZC ===========================\r\n\n");
  printf("  Test Protection: TZSC (Flash) ------------------------- 1\r\n\n");
  printf("  Test Protection: TZSC (RAM) --------------------------- 2\r\n\n");
  printf("  Test Protection: TZSC (AES) --------------------------- 3\r\n\n");
  printf("  Test Protection: TZSC (PKA) --------------------------- 4\r\n\n");
  printf("  Test Protection: TZSC (RNG) --------------------------- 5\r\n\n");
  printf("  Test Protection: TZSC (SUBGHZSPI) --------------------- 6\r\n\n");
  printf("  Test Protection: TZSC (TZIC)--------------------------- 7\r\n\n");
  printf("  Test Protection: TZSC (TZSC)--------------------------- 8\r\n\n");
  printf("  Test Protection: TZIC (PWR) --------------------------- 9\r\n\n");
  printf("  Test Protection: TZIC (FLASHIF) ----------------------- a\r\n\n");
  printf("  Test Protection: TZIC (DMA1) -------------------------- b\r\n\n");
  printf("  Test Protection: TZIC (DMA2) -------------------------- c\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
}

/**
  * @brief  TEST Run TZIC
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_Run_GTZC(void)
{
  m_uGtzcTzicEvent = 0;
  uint8_t key = 0U;
  uint8_t exit = 0U;
  uint8_t invalid = 0U;
  uint8_t error_code = 0U;
  uint32_t value = 0U;

  TEST_PROTECTIONS_GtzcPrintTestingSubMenu();

  while (exit == 0U)
  {
    key = 0U;
    invalid = 0U;

    /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload
       register */
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      printf("If GTZC (TZSC and TZIC) is configured, you should get a TZIC interrupt.\r\n\n");
      switch (key)
      {
        case '1' :
          value = *((uint32_t *)TEST_PROTECTIONS_SE_ISOLATED_CODE_FLASH_ADDRESS);
          break;
        case '2' :
          /* Try to read a 32-bit data from the SRAM2 protected by the MPU */
          value = *((uint32_t *)TEST_PROTECTIONS_SE_ISOLATED_VDATA_SRAM_ADDRESS);
          break;
        case '3' :
          value = READ_REG(AES->SR);
          break;
        case '4' :
          value = READ_REG(PKA->SR);
          break;
        case '5' :
          value = READ_REG(RNG->SR);
          break;
        case '6' :
          value = READ_REG(SUBGHZSPI->SR);
          break;
        case '7' :
          value = READ_REG(GTZC_TZIC->MISR1);
          break;
        case '8' :
          /* All GTZSC registers can be read by non-secure unprivileged accesses */

          /* All peripheral aren't privileged in PRIVCFGR1 so there shouldn't be an illegal access */
          error_code = 0;
          value = READ_REG(GTZC_TZSC->SECCFGR1);
          WRITE_REG(GTZC_TZSC->SECCFGR1, 0U);
          if(value != READ_REG(GTZC_TZSC->SECCFGR1))
          {
            error_code = TEST_GTZC_ERROR_CODE;
          }

          /* This should generate an illegal access */
          value = READ_REG(GTZC_TZSC->PRIVCFGR1);
          WRITE_REG(GTZC_TZSC->PRIVCFGR1, 0U);
          if(value != READ_REG(GTZC_TZSC->PRIVCFGR1))
          {
            error_code = TEST_GTZC_ERROR_CODE;
          }
          break;
        case '9' :
          /* The other PWR registers can be read and written by non-secure unprivileged accesses */
          value = READ_REG(PWR->SECCFGR);
          break;
        case 'a' :
          MODIFY_REG(FLASH->WRP1AR, FLASH_WRP1AR_WRP1A_STRT, 0x1F);
          break;
        case 'b' :
          __HAL_RCC_DMA1_CLK_ENABLE();
          __HAL_RCC_DMA1_FORCE_RESET();
          __HAL_RCC_DMA1_RELEASE_RESET();
          SET_BIT(DMA1_Channel1->CCR, DMA_CCR_PRIV);
          __HAL_RCC_DMA1_CLK_DISABLE();
          break;
        case 'c' :
          __HAL_RCC_DMA2_CLK_ENABLE();
          __HAL_RCC_DMA2_FORCE_RESET();
          __HAL_RCC_DMA2_RELEASE_RESET();
          SET_BIT(DMA2_Channel1->CCR, DMA_CCR_PRIV);
          __HAL_RCC_DMA2_CLK_ENABLE();
          break;
        case 'x' :
          exit = 1U;
          break;

        default:
          printf("Invalid Number !\r");
          invalid = 1U;
          break;
      }
    }

    if ((key != 0) && (exit != 1) && (invalid != 1))
    {
      if ((key == 'a') || (key == 'b') || (key == 'c'))
      {
        printf("  -- Register successfully written for the choice %c\r\n", key);
      }
      else if ((key == '8') && (error_code == TEST_GTZC_ERROR_CODE))
      {
        printf("The GTZC registers aren't properly locked !! \r\n\n");
      }
      else if (key == '8')
      {
        printf("  -- The GTZC registers are locked.\r\n");
      }
      else
      {
        printf("  -- Read value for the choice %c: %d\r\n", key, value);
      }

      /* Wait 1 second */
      WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
      HAL_Delay(1U);

      if (m_uGtzcTzicEvent == 0U)
      {
        printf("GTZC is NOT ENABLED in this use case !! \r\n\n");
      }
      else
      {
        printf("\r\n\n  -- TZIC Event detected!!\r\n\n  -- System reset requested!!!\r\n\n");
        SVC_NVIC_SystemReset();
      }

      TEST_PROTECTIONS_GtzcPrintTestingSubMenu();
    }
  }
}

/**
  * @brief  TEST Run KMS_DataStorage
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_Run_KMS_DATA_STORAGE(void)
{
  uint32_t u_read_fw_code = 0;
  printf("\r\n====== Test Protection: MPU privileged - KMS_DataStorage \r\n\n");
  printf("  -- Reading address: 0x%x\r\n\n", KMS_DATASTORAGE_START);

  /* Try to read a 32-bit data from the Flash protected by the MPU */
  u_read_fw_code = *((uint32_t *)KMS_DATASTORAGE_START);

  /* Should not get here if MPU is available and enabled  */
  printf("  -- Address: 0x%x = 0x%x\r\n\n", KMS_DATASTORAGE_START, u_read_fw_code);
  printf("  -- !! MPU privileged CODE protection is NOT ENABLED !!\r\n\n");
}

/**
  * @brief  TEST Run SYNC_FLAG
  * @param  None.
  * @retval None.
  */
static void TEST_PROTECTIONS_Run_SYNC_FLAG(void)
{
  __IO uint32_t *pSyncFlag = (__IO uint32_t *)M4_M0PLUS_FLAG_RAM_START;
  uint32_t u_read_flag_value = 0;
  printf("\r\n====== Test Protection: MPU Read Only - Synchronization flag \r\n\n");
  printf("  -- Reading address: 0x%x\r\n\n", M4_M0PLUS_FLAG_RAM_START);

  /* Try to change the value of the synchronization flag */
  u_read_flag_value = *pSyncFlag;
  *pSyncFlag = 0U;

  /* Restore the value */
  *pSyncFlag = u_read_flag_value;

  /* Should not get here if MPU is available and enabled  */
  printf("  -- Address: 0x%x = 0x%x\r\n\n", M4_M0PLUS_FLAG_RAM_START, *pSyncFlag);
  printf("  -- !! MPU read only FLAG protection is NOT ENABLED !!\r\n\n");
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
  * @brief  Implement the Cube_Hal Callback generated on the GTZC TZIC IRQ.
  * @param  None
  * @retval None
  */
void CALLBACK_GTZC_TZIC(void)
{
  /*Set GTZC TZIC event variable*/
  m_uGtzcTzicEvent = 1U;
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
