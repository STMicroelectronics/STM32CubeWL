/**
  ******************************************************************************
  * @file    data_init.c
  * @author  MCD Application Team
  * @brief   Data section (RW + ZI) initialization.
  *          This file provides set of firmware functions to manage SE low level
  *          interface functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright(c) 2017 STMicroelectronics International N.V.
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
#include "stdint.h"

extern uint32_t Image$$SE_region_RAM$$RW$$Base;
extern uint32_t Load$$SE_region_RAM$$RW$$Base;
extern uint32_t Image$$SE_region_RAM$$RW$$Length;
extern uint32_t Image$$SE_region_RAM$$ZI$$Base;
extern uint32_t Image$$SE_region_RAM$$ZI$$Length;


#define data_ram Image$$SE_region_RAM$$RW$$Base
#define data_rom Load$$SE_region_RAM$$RW$$Base
#define data_rom_length Image$$SE_region_RAM$$RW$$Length
#define bss Image$$SE_region_RAM$$ZI$$Base
#define bss_length Image$$SE_region_RAM$$ZI$$Length;


/**
  * @brief  Copy initialized data from ROM to RAM.
  * @param  None.
  * @retval None.
  */
void LoopCopyDataInit(void) 
{
  uint32_t i;
	uint8_t* src = (uint8_t*)&data_rom;
	uint8_t* dst = (uint8_t*)&data_ram;
	uint32_t len = (uint32_t)&data_rom_length;

	for(i=0; i < len; i++)
  {  
		dst[i] = src[i];
  }
}
  
/**
  * @brief  Clear the zero-initialized data section.
  * @param  None.
  * @retval None.
  */
void LoopFillZerobss(void) 
{
  uint32_t i;
	uint8_t* dst = (uint8_t*)&bss;
	uint32_t len = (uint32_t)&bss_length;
	
	/* Clear the zero-initialized data section */
	for(i=0; i < len; i++)
  {  
		dst[i] = 0;
  }
}
	
/**
  * @brief  Data section initialization.
  * @param  None.
  * @retval None.
  */
 void __arm_data_init(void) {
	 	LoopFillZerobss();
	  LoopCopyDataInit();
} 
 

