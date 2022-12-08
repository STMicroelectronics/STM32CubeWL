/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ee.c
  * @author  MCD Application Team
  * @brief   Implementation of the EEPROM emulator
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

/* Includes ------------------------------------------------------------------*/
#include "ee.h"
#include "ee_conf.h"
#include "flash_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  /* Absolute base address of the bank in Flash memory */
  uint32_t address;

  /* Total number of pages in a pool (constant) */
  uint8_t  nb_pages;

  /* Current write page (can be ACTIVE or RECEIVE state) */
  uint8_t  current_write_page;

  /* Total number of elements written in valid and active pages */
  uint16_t nb_written_elements;

  /* Write position inside the current write page */
  uint16_t next_write_offset;

} EE_var_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

#ifndef M_BEGIN
#define  M_BEGIN                        do{
#endif /* !M_BEGIN */
#ifndef M_END
#define M_END  }                       while(0)
#endif /* !M_END */

#ifndef DBG_EE
#define DBG_EE( X )
#endif /* !DBG_EE */

/* Page header size in bytes (4 words used to save page state) */
#define EE_HEADER_SIZE             (4 * HW_FLASH_WIDTH)

/* Maximum number of elements by page */
#define EE_NB_MAX_ELT \
  ((HW_FLASH_PAGE_SIZE - EE_HEADER_SIZE) / HW_FLASH_WIDTH)

/* Flash words special values */
#define EE_ERASED                  0xFFFFFFFFFFFFFFFFULL
#define EE_PROGRAMMED              0xAAAAAAAAAAAAAAAAULL

/* No page define */
#define EE_PAGE_NOT_FOUND          0xFFFFFFFFUL

/* Element tag in flash */
#define EE_TAG                     0x8000UL

/* Page state definition */
enum
{
  EE_STATE_ERASED  = 0,   /* page is erased */
  EE_STATE_RECEIVE = 1,   /* used during transfer for new page */
  EE_STATE_ACTIVE  = 2,   /* page contains valid data and is not full */
  EE_STATE_VALID   = 3,   /* page contains valid data and is full */
  EE_STATE_ERASING = 4,   /* used during transfer for old pages */
};

/* Macro to get flash address from page index */
#define EE_FLASH_ADDR( pv, p_ ) \
  ((pv)->address + ((p_) * HW_FLASH_PAGE_SIZE))

/* Macro to get flash page from page index */
#define EE_FLASH_PAGE( pv, p_ ) \
  ((((pv)->address - HW_FLASH_ADDRESS) / HW_FLASH_PAGE_SIZE) + (p_))

/* Macro to get first page index of following pool, among circular pool list */
#define EE_NEXT_POOL( pv ) \
  (((pv)->current_write_page < (pv)->nb_pages) ? (pv)->nb_pages : 0)

/* Check Configuration */
#if (CFG_EE_BANK0_SIZE & ((2 * HW_FLASH_PAGE_SIZE) - 1))
#error EE: wrong value of CFG_EE_BANK0_SIZE
#endif /* (CFG_EE_BANK0_SIZE & ((2 * HW_FLASH_PAGE_SIZE) - 1)) */
#if (CFG_EE_BANK1_SIZE & ((2 * HW_FLASH_PAGE_SIZE) - 1))
#error EE: wrong value of CFG_EE_BANK1_SIZE
#endif /* (CFG_EE_BANK1_SIZE & ((2 * HW_FLASH_PAGE_SIZE) - 1)) */
#if ((CFG_EE_BANK0_MAX_NB > \
      EE_NB_MAX_ELT * CFG_EE_BANK0_SIZE / (2 * HW_FLASH_PAGE_SIZE)) || \
     (CFG_EE_BANK0_MAX_NB > 0x4000U))
#error EE: CFG_EE_BANK0_MAX_NB too big
#endif /* CFG_EE_BANK0_MAX_NB */
#if ((CFG_EE_BANK1_SIZE > 0) && \
     ((CFG_EE_BANK1_MAX_NB > \
       EE_NB_MAX_ELT * CFG_EE_BANK1_SIZE / (2 * HW_FLASH_PAGE_SIZE)) || \
      (CFG_EE_BANK1_MAX_NB > 0x4000U)))
#error EE: CFG_EE_BANK1_MAX_NB too big
#endif /* CFG_EE_BANK1_SIZE */

/* Macro used in CRC computation (one byte CRC step) */
#define EE_CRC16_STEP( v, x, crc ) \
  M_BEGIN \
  x = ((crc>>8) ^ v) & 0xFF; \
  x ^= x>>4; \
  crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x; \
  M_END

/* Macro to get a 64-bit pointer from an address represented as an integer */
#ifndef EE_PTR
#define EE_PTR( x_ )               ((uint64_t*)(uintptr_t)(x_))
#endif /* !EE_PTR */

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
EE_var_t EE_var[CFG_EE_BANK1_SIZE ? 2 : 1];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static void EE_Reset(EE_var_t *pv, uint32_t address, uint8_t nb_pages);

static int32_t EE_Recovery(EE_var_t *pv);

static int32_t EE_Transfer(EE_var_t *pv, uint16_t addr, uint32_t page);

static int32_t EE_WriteEl(EE_var_t *pv, uint16_t addr, uint32_t data);

static int32_t EE_ReadEl(const EE_var_t *pv,
                         uint16_t addr, uint32_t *data, uint32_t page);

static int32_t EE_SetState(const EE_var_t *pv, uint32_t page, uint32_t state);

static uint32_t EE_GetState(const EE_var_t *pv, uint32_t page);

static uint16_t EE_Crc(uint64_t v);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

int32_t EE_Init(int32_t format, uint32_t base_address)
{
  /* USER CODE BEGIN EE_Init_1 */

  /* USER CODE END EE_Init_1 */
  int32_t status;
  uint16_t total_nb_pages;

  /* Reset global variables of both banks */

  EE_Reset(&EE_var[0],
           base_address,
           CFG_EE_BANK0_SIZE / (2 * HW_FLASH_PAGE_SIZE));

  if (CFG_EE_BANK1_SIZE)
  {
    EE_Reset(&EE_var[1],
             base_address + CFG_EE_BANK0_SIZE,
             CFG_EE_BANK1_SIZE / (2 * HW_FLASH_PAGE_SIZE));
  }

  /* If format mode is set, start from scratch */

  if (format)
  {
    /* Force erase of all pages */
    total_nb_pages =
      2 * (EE_var[0].nb_pages + (CFG_EE_BANK1_SIZE ? EE_var[1].nb_pages : 0));

    if (FLASH_IF_Erase((void *)EE_FLASH_ADDR(EE_var, 0), total_nb_pages * FLASH_PAGE_SIZE) != 0)
    {
      return EE_ERASE_ERROR;
    }

    /* Set first page of each pool in ACTIVE State */
    status = EE_SetState(&EE_var[0], 0, EE_STATE_ACTIVE);

    if (CFG_EE_BANK1_SIZE && (status == EE_OK))
    {
      status = EE_SetState(&EE_var[1], 0, EE_STATE_ACTIVE);
    }

    return status;
  }

  /* else, try to recover the EEPROM emulation state from flash */

  status = EE_Recovery(&EE_var[0]);

  if (CFG_EE_BANK1_SIZE && (status == EE_OK))
  {
    status = EE_Recovery(&EE_var[1]);
  }

  return status;
  /* USER CODE BEGIN EE_Init_2 */

  /* USER CODE END EE_Init_2 */
}

int32_t EE_Read(int32_t bank, uint16_t addr, uint32_t *data)
{
  /* USER CODE BEGIN EE_Read_1 */

  /* USER CODE END EE_Read_1 */
  EE_var_t *pv = &EE_var[CFG_EE_BANK1_SIZE && bank];

  /* Read element starting from active page */
  return EE_ReadEl(pv, addr, data, pv->current_write_page);
  /* USER CODE BEGIN EE_Read_2 */

  /* USER CODE END EE_Read_2 */
}

int32_t EE_Write(int32_t bank, uint16_t addr, uint32_t data)
{
  /* USER CODE BEGIN EE_Write_1 */

  /* USER CODE END EE_Write_1 */
  EE_var_t *pv = &EE_var[CFG_EE_BANK1_SIZE && bank];
  uint32_t page;

  /* Check if current pool is full */
  if (pv->nb_written_elements < EE_NB_MAX_ELT * pv->nb_pages)
  {
    /* If not full, write the virtual address and value in the EEPROM */
    return EE_WriteEl(pv, addr, data);
  }

  DBG_EE(EE_2);

  /* If full, we need to write in other pool and perform pool transfer */
  page = EE_NEXT_POOL(pv);

  /* Check next page state: it must be ERASED */
  if (EE_GetState(pv, page) != EE_STATE_ERASED)
  {
    return EE_STATE_ERROR;
  }

  /* Mark the ERASED page at RECEIVE state */
  if (EE_SetState(pv, page, EE_STATE_RECEIVE) != EE_OK)
  {
    return EE_WRITE_ERROR;
  }

  DBG_EE(EE_3);

  /* Reset global variables */
  pv->current_write_page = page;
  pv->nb_written_elements = 0;
  pv->next_write_offset = EE_HEADER_SIZE;

  /* Write the variable passed as parameter in the new active page */
  if (EE_WriteEl(pv, addr, data) != EE_OK)
  {
    return EE_WRITE_ERROR;
  }

  DBG_EE(EE_4);

  /* Set the previous ACTIVE pool to ERASING and copy the latest written
     values to the new pool */
  if (EE_Transfer(pv, addr, page) != EE_OK)
  {
    return EE_WRITE_ERROR;
  }

  DBG_EE(EE_5);

  /* A clean is required */
  return EE_CLEAN_NEEDED;
  /* USER CODE BEGIN EE_Write_2 */

  /* USER CODE END EE_Write_2 */
}

int32_t EE_Clean(int32_t bank)
{
  /* USER CODE BEGIN EE_Clean_1 */

  /* USER CODE END EE_Clean_1 */
  EE_var_t *pv = &EE_var[CFG_EE_BANK1_SIZE && bank];
  uint32_t page;

  /* Get first page of unused pool */
  page = EE_NEXT_POOL(pv);

  /* At least, the first page of the pool should be in ERASING state */
  if (EE_GetState(pv, page) != EE_STATE_ERASING)
  {
    return EE_STATE_ERROR;
  }

  DBG_EE(EE_1);

  /* Erase all the pages of the pool */
  if (FLASH_IF_Erase((void *)EE_FLASH_ADDR(pv, page), pv->nb_pages * FLASH_PAGE_SIZE)
      != 0)
  {
    return EE_ERASE_ERROR;
  }

  return EE_OK;
  /* USER CODE BEGIN EE_Clean_2 */

  /* USER CODE END EE_Clean_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static void EE_Reset(EE_var_t *pv, uint32_t address, uint8_t nb_pages)
{
  /* USER CODE BEGIN EE_Reset_1 */

  /* USER CODE END EE_Reset_1 */
  /* Reset global variables of the bank */
  pv->address = address;
  pv->nb_pages = nb_pages;
  pv->current_write_page = 0;
  pv->nb_written_elements = 0;
  pv->next_write_offset = EE_HEADER_SIZE;
  /* USER CODE BEGIN EE_Reset_2 */

  /* USER CODE END EE_Reset_2 */
}

static int32_t EE_Recovery(EE_var_t *pv)
{
  /* USER CODE BEGIN EE_Recovery_1 */

  /* USER CODE END EE_Recovery_1 */
  uint32_t page;
  uint32_t first_page;
  uint32_t state;
  uint32_t prev_state;
  uint32_t flash_addr;
  uint32_t i;

  /* Search all pages for a reliable RECEIVE page then ACTIVE page */
  for (state = EE_STATE_RECEIVE; state <= EE_STATE_ACTIVE; state++)
  {
    for (page = 0; page < 2UL * pv->nb_pages; page++)
    {
      if (state != EE_GetState(pv, page))
      {
        continue;
      }

      if ((page == 0) || (page == pv->nb_pages))
      {
        /* Check if state is reliable by checking state of next page */
        uint32_t next_page = page + 1;
        /*make sure next page is in range, else modulo*/
        if (next_page == pv->nb_pages * 2)
        {
          next_page = 0;
        }
        if (EE_GetState(pv, next_page) != EE_STATE_ERASED)
        {
          continue;
        }
      }
      else
      {
        prev_state = EE_GetState(pv, page - 1);

        if (prev_state != state)
        {
          /* Check if state is reliable by checking state of previous page */
          if (prev_state != EE_STATE_VALID)
          {
            continue;
          }
        }
        else
        {
          /* If page and previous page are the same, mark previous as VALID */
          if (EE_SetState(pv, page - 1, EE_STATE_VALID) != EE_OK)
          {
            return EE_WRITE_ERROR;
          }
        }
      }

      /* Update write page */
      pv->current_write_page = page;

      /* Count elements already in ACTIVE or RECEIVE page */
      flash_addr = EE_FLASH_ADDR(pv, page) + EE_HEADER_SIZE;
      for (i = 0; i < EE_NB_MAX_ELT; i++)
      {
        /* Check if current element is valid */
        if (*EE_PTR(flash_addr) == EE_ERASED)
        {
          break;
        }

        /* Update global variables accordingly */
        pv->nb_written_elements++;
        pv->next_write_offset += HW_FLASH_WIDTH;

        /* Next element address */
        flash_addr += HW_FLASH_WIDTH;
      }

      /* Count elements already transferred in previous pool pages */
      while (!((page == 0) || (page == pv->nb_pages)))
      {
        /* Update number of elements written in pool */
        pv->nb_written_elements += EE_NB_MAX_ELT;

        page--;
      }

      /* If we have found a RECEIVE page, it means that pool transfer
         has been interrupted by reset */
      if (state == EE_STATE_RECEIVE)
      {
        /* Resume pool transfer */
        if (EE_Transfer(pv, EE_TAG, page) != EE_OK)
        {
          return EE_WRITE_ERROR;
        }
      }

      /* RECEIVE/ACTIVE page found, check if some erasing is needed */

      /* Get first page of unused pool */
      first_page = EE_NEXT_POOL(pv);

      /* Erase all the pages not already erased in the pool */
      for (page = first_page; page < first_page + pv->nb_pages; page++)
      {
        if (EE_GetState(pv, page) != EE_STATE_ERASED)
        {
          if (FLASH_IF_Erase((void *)EE_FLASH_ADDR(pv, page), FLASH_PAGE_SIZE) != 0)
          {
            return EE_ERASE_ERROR;
          }
        }
      }

      return EE_OK;
    }
  }

  /* No true RECEIVE or ACTIVE page has been found */
  return EE_STATE_ERROR;
  /* USER CODE BEGIN EE_Recovery_2 */

  /* USER CODE END EE_Recovery_2 */
}

static int32_t EE_Transfer(EE_var_t *pv, uint16_t addr, uint32_t page)
{
  /* USER CODE BEGIN EE_Transfer_1 */

  /* USER CODE END EE_Transfer_1 */
  uint32_t state;
  uint32_t var;
  uint32_t data;
  uint32_t last_page;

  /* Input "page" is the first page of the new pool;
     We compute "last_page" as the last page of the old pool to be set
     in ERASING state (all pages in old pool are assumed to be either VALID
     or ACTIVE, except in case of recovery, where some pages may be already
     in ERASING state.
     However, in case of recovery, we do not not need to set ERASING,
     as initialization phase erases the inactive pool. */
  last_page =
    (page < pv->nb_pages) ? (2 * pv->nb_pages - 1) : (pv->nb_pages - 1);

  if (addr != EE_TAG)
  {
    /* Loop on all old pool pages in descending order */
    page = last_page;
    while (1)
    {
      state = EE_GetState(pv, page);

      if ((state == EE_STATE_ACTIVE) || (state == EE_STATE_VALID))
      {
        /* Set page state to ERASING */
        if (EE_SetState(pv, page, EE_STATE_ERASING) != EE_OK)
        {
          return EE_WRITE_ERROR;
        }
      }

      DBG_EE(EE_6);

      /* Check if start of pool is reached */
      if ((page == 0) || (page == pv->nb_pages))
      {
        break;
      }

      page--;
    }
  }

  /* Now, we can copy variables from one pool to the other */

  for (var = 0; var < EE_NB_MAX_ELT * pv->nb_pages; var++)
  {
    /* Check each variable except the one passed as parameter
       (and except the ones already transferred in case of recovery) */
    if ((var != addr) &&
        ((addr != EE_TAG) ||
         (EE_ReadEl(pv, var, &data, pv->current_write_page) != EE_OK)))
    {
      /* Read the last variable update */
      if (EE_ReadEl(pv, var, &data, last_page) == EE_OK)
      {
        DBG_EE(EE_7);

        /* In case variable corresponding to the virtual address was found,
           copy the variable to the new active page */
        if (EE_WriteEl(pv, var, data) != EE_OK)
        {
          return EE_WRITE_ERROR;
        }
      }
    }
  }

  /* Transfer is now done, mark the receive state page as active */
  return EE_SetState(pv, pv->current_write_page, EE_STATE_ACTIVE);
  /* USER CODE BEGIN EE_Transfer_2 */

  /* USER CODE END EE_Transfer_2 */
}

static int32_t EE_WriteEl(EE_var_t *pv, uint16_t addr, uint32_t data)
{
  /* USER CODE BEGIN EE_WriteEl_1 */

  /* USER CODE END EE_WriteEl_1 */
  uint32_t page;
  uint32_t flash_addr;
  uint64_t el;

  /* It is assumed here that the current pool is not full
     and that free pages in this pool are in ERASED state */

  /* Check if active page is full */
  if (pv->next_write_offset >= HW_FLASH_PAGE_SIZE)
  {
    /* Get current active page */
    page = pv->current_write_page;

    /* Set new page as was previous one (active or receive) */
    if (EE_SetState(pv, page + 1, EE_GetState(pv, page)) != EE_OK)
    {
      return EE_WRITE_ERROR;
    }

    DBG_EE(EE_8);

    /* Set current page in valid state */
    if (EE_SetState(pv, page, EE_STATE_VALID) != EE_OK)
    {
      return EE_WRITE_ERROR;
    }

    /* Update global variables to use next page */
    pv->current_write_page = page + 1;
    pv->next_write_offset = EE_HEADER_SIZE;
  }

  /* Build element to be written in flash */
  if (addr == EE_TAG)
  {
    el = 0ULL;
  }
  else
  {
    /* Build element from virtual addr and data, plus CRC */
    el = ((((uint64_t)data) << 32) | ((EE_TAG | (addr & 0x3FFFUL)) << 16));
    el |= EE_Crc(el);
  }

  /* Compute write address */
  flash_addr = EE_FLASH_ADDR(pv, pv->current_write_page) + pv->next_write_offset;

  /* Write element in flash */
  if (FLASH_IF_Write((void *)flash_addr, (const void *)&el, 8) != 0)
  {
    return EE_WRITE_ERROR;
  }

  /* Increment global variables relative to write operation done */
  pv->next_write_offset += HW_FLASH_WIDTH;
  pv->nb_written_elements++;

  return EE_OK;
  /* USER CODE BEGIN EE_WriteEl_2 */

  /* USER CODE END EE_WriteEl_2 */
}

static int32_t EE_ReadEl(const EE_var_t *pv,
                         uint16_t addr, uint32_t *data, uint32_t page)
{
  /* USER CODE BEGIN EE_ReadEl_1 */

  /* USER CODE END EE_ReadEl_1 */
  uint32_t flash_addr;
  uint32_t offset;
  uint64_t el;

  /* Search variable in the pool (in decreasing page order from "page") */
  while (1)
  {
    /* Check each page address starting from end */
    flash_addr = EE_FLASH_ADDR(pv, page);
    for (offset = HW_FLASH_PAGE_SIZE - HW_FLASH_WIDTH;
         offset >= EE_HEADER_SIZE; offset -= HW_FLASH_WIDTH)
    {
      /* Read one element from flash */
      el = *EE_PTR(flash_addr + offset);

      /* Compare the read address with the input address and check CRC:
         in case of failed CRC, data is corrupted and has to be skipped */
      if ((el != EE_ERASED) && (el != 0ULL) &&
          (((el & 0x3FFFFFFFUL) >> 16) == addr) &&
          (EE_Crc(el) == (uint16_t)el))
      {
        /* Get variable data */
        *data = (uint32_t)(el >> 32);

        /* Variable is found */
        return EE_OK;
      }
    }

    /* Check if start of pool is reached */
    if ((page == 0) || (page == pv->nb_pages))
    {
      /* Variable is not found */
      return EE_NOT_FOUND;
    }

    page--;
  }
  /* USER CODE BEGIN EE_ReadEl_2 */

  /* USER CODE END EE_ReadEl_2 */
}

static int32_t EE_SetState(const EE_var_t *pv, uint32_t page, uint32_t state)
{
  /* USER CODE BEGIN EE_SetState_1 */

  /* USER CODE END EE_SetState_1 */
  uint32_t flash_addr;
  uint64_t programmed_state = EE_PROGRAMMED;

  flash_addr = EE_FLASH_ADDR(pv, page) + ((state - 1) * HW_FLASH_WIDTH);

  DBG_EE(EE_0);

  /* Set new page state inside page header */
  if (FLASH_IF_Write((void *)flash_addr, (const void *)&programmed_state, 8) != 0)
  {
    return EE_WRITE_ERROR;
  }

  return EE_OK;
  /* USER CODE BEGIN EE_SetState_2 */

  /* USER CODE END EE_SetState_2 */
}

static uint32_t EE_GetState(const EE_var_t *pv, uint32_t page)
{
  /* USER CODE BEGIN EE_GetState_1 */

  /* USER CODE END EE_GetState_1 */
  uint32_t state;
  uint32_t flash_addr;

  flash_addr = EE_FLASH_ADDR(pv, page) + EE_HEADER_SIZE;

  for (state = EE_STATE_ERASING; state > EE_STATE_ERASED; state--)
  {
    flash_addr -= HW_FLASH_WIDTH;

    /* If page header word is not ERASED, return word index as page state */
    if (*EE_PTR(flash_addr) != EE_ERASED)
    {
      break;
    }
  }

  return state;
  /* USER CODE BEGIN EE_GetState_2 */

  /* USER CODE END EE_GetState_2 */
}

static uint16_t EE_Crc(uint64_t v)
{
  /* USER CODE BEGIN EE_Crc_1 */

  /* USER CODE END EE_Crc_1 */
  uint32_t x = 0;
  uint32_t crc = 0;

  v >>= 16;
  EE_CRC16_STEP((uint32_t)v, x, crc);
  v >>= 8;
  EE_CRC16_STEP((uint32_t)v, x, crc);
  v >>= 8;
  EE_CRC16_STEP((uint32_t)v, x, crc);
  v >>= 8;
  EE_CRC16_STEP((uint32_t)v, x, crc);
  v >>= 8;
  EE_CRC16_STEP((uint32_t)v, x, crc);
  v >>= 8;
  EE_CRC16_STEP((uint32_t)v, x, crc);

  return crc;
  /* USER CODE BEGIN EE_Crc_2 */

  /* USER CODE END EE_Crc_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
