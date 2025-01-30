/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ee.h
  * @author  MCD Application Team
  * @brief   Header of the EEPROM emulator
  *****************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *****************************************************************************
  */
/* USER CODE END Header */

#ifndef EE_H__
#define EE_H__

/**
  * Description
  * -----------
  * the EE module implements an EEPROM emulator in one C file ("ee.c").
  * Its interface is defined below in this file ("ee.h").
  * Up to two independent banks can be configured.
  * Data granularity to store and read in this EEPROM emulator is a 32-bit word.
  *
  * Configuration and dependencies
  * ------------------------------
  * the EE module includes a generic header file "common.h":
  * this file has to be defined by the user and must provide the following:
  *
  * - M_BEGIN, M_END and DBG() macro definitions.
  *
  * - Hardware flash driver definitions:
  *
  *      * HW_FLASH_PAGE_SIZE (in bytes)
  *
  *      * HW_FLASH_WIDTH (in bytes)
  *
  *      * int HW_FLASH_Write( uint32_t address,
  *                            uint64_t data );
  *
  *      * int HW_FLASH_Erase( uint32_t page,
  *                            uint16_t n,
  *                            int32_t interrupt );
  *
  * - Configuration definitions:
  *
  *     * CFG_EE_BANK0_SIZE
  *       Size of the first bank in bytes (must be greater than 0).
  *       It must be a multiple of page size.
  *
  *     * CFG_EE_BANK0_MAX_NB
  *       Maximum number of data that can be stored in the first bank.
  *
  *     * CFG_EE_BANK1_SIZE
  *       Size of the second bank in bytes (can be 0 if the bank is not used).
  *       It must be a multiple of page size.
  *
  *     * CFG_EE_BANK1_MAX_NB
  *       Maximum number of data that can be stored in the second bank.
  *
  * Notes
  * -----
  * - a corrupted word in FLASH detected by the user software shall be set to 0.
  *   The EEPROM emulation software will then handle it properly.
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Definition of the functions return value */
enum
{
  EE_OK = 0,
  EE_NOT_FOUND,     /* read data is not found in flash */
  EE_CLEAN_NEEDED,  /* data is written but a "clean" is needed */
  EE_ERASE_ERROR,   /* an error occurs during flash erase */
  EE_WRITE_ERROR,   /* an error occurs during flash write */
  EE_STATE_ERROR    /* state of flash is incoherent (needs clean or format) */
};

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * EE_Init
  *
  * Initialization of EEPROM emulation module. It must be called once at reset.
  *
  * format: 0 -> recover EE state from flash and restore the pages
  *              to a known good state in case of power loss.
  *         1 -> erase all the pages: starts from scratch.
  *              This format mode can be activated the very first time EEPROM
  *              emulation is used, to prepare flash pages for EEPROM emulation
  *              with empty EEPROM
  *
  * base_address: absolute start address of flash area used for EEPROM
  *               emulation. It must be a multiple of flash page size.
  *
  * return: EE_OK in case of success
  *         EE..._ERROR in case of error
  */

extern int32_t EE_Init(int32_t format,
                       uint32_t base_address);

/**
  * EE_Read
  *
  * Returns the last stored variable data, if found, which corresponds to
  * the passed virtual address
  *
  * bank:   index of the bank (0 or 1)
  *
  * addr:   variable virtual address
  *
  * data:   pointer to a 32-bit word (allocated by the caller) containing the
  *         variable value in case of success.
  *
  * return: EE_OK in case of success
  *         EE_NOT_FOUND in case this virtual address has never been written to
  *         EE..._ERROR in case of error
  */

extern int32_t EE_Read(int32_t bank, uint16_t addr, uint32_t *data);

/**
  * EE_Write
  *
  * Writes/updates variable data in EEPROM emulator.
  * Triggers internal pages transfer if flash pool is full.
  *
  * bank:   index of the bank (0 or 1)
  *
  * addr:   variable virtual address
  *
  * data:   32-bit data word to be written
  *
  * return: EE_OK in case of success
  *         EE_CLEAN_NEEDED if success but user has to trigger flash cleanup
  *                         by calling EE_Clean()
  *         EE..._ERROR in case of error
  */

extern int32_t EE_Write(int32_t bank, uint16_t addr, uint32_t data);

/**
  * EE_Clean
  *
  * Erase obsolete pool of pages in polling mode.
  * This function should be called when EE_Write() has returned EE_CLEAN_NEEDED
  * (and only in that case)
  *
  * bank:   index of the bank (0 or 1)
  *
  * return: EE_OK in case of success
  *         EE..._ERROR in case of error
  */

extern int32_t EE_Clean(int32_t bank);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#endif /* EE_H__ */
