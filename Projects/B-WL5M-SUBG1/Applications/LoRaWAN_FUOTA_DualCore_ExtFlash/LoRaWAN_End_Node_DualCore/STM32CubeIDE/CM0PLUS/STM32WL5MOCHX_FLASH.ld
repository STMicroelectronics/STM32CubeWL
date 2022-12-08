/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32WL5MOCHx Device
**                      256Kbytes FLASH
**                      64Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is without any warranty
**                of any kind.
**
**  Note        : For specific memory allocation, linker and startup files must be customized.
**                Refer to STM32CubeIDE user guide (UM2609), chapter "Modify the linker script".
**
*****************************************************************************
** @attention
**
** Copyright (c) 2021 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(APPLI_RAM) + LENGTH(APPLI_RAM); /* end of "RAM" Ram type memory */

_Min_Heap_Size  = 0x200; /* required amount of heap  */
_Min_Stack_Size = 0x800; /* required amount of stack */

INCLUDE se_interface_app.ld
INCLUDE mapping_fwimg.ld
INCLUDE mapping_sbsfu.ld

/* Memories definition */
MEMORY
{
  APPLI_ROM  (xrw)   : ORIGIN = (SLOT_Active_1_start + 0x200), LENGTH = (SLOT_Active_1_end - SLOT_Active_1_start + 1 - 0x200)
  NVM_RAM    (rw)    : ORIGIN = LW_NVM_region_RAM_start, LENGTH = LW_NVM_region_RAM_size
  APPLI_RAM  (xrw)   : ORIGIN = SB_region_RAM_start, LENGTH = SB_region_RAM_size
  RAM_SHARED (xrw)   : ORIGIN = M0PLUS_SHARED_region_RAM_start, LENGTH = M0PLUS_SHARED_region_RAM_size
}

/* Sections */
SECTIONS
{
  /* The startup code into "ROM" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >APPLI_ROM

  .SE_IF_Code : {
    . = ALIGN(8);
    KEEP(*se_interface_app.o (.text .text*))
    KEEP(*se_interface_kms.o (.text .text*))
    . = ALIGN(8);
  } >SE_IF_ROM_region

  /* The program code and other data into "ROM" Rom type memory */
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >APPLI_ROM

  /* Constant data into "ROM" Rom type memory */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >APPLI_ROM

  .ARM.extab   : { 
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >APPLI_ROM
  
  .ARM : {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >APPLI_ROM

  .preinit_array     :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(8);
  } >APPLI_ROM
  
  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(8);
  } >APPLI_ROM
  
  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(8);
  } >APPLI_ROM

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "SRAM2" Ram type memory */
  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */

  } >APPLI_RAM AT> APPLI_ROM

  /* NVM RAM Data */
  LW_NVM_RAM (NOLOAD) :
  {
    . = ALIGN(8);
    *(.bss.LW_NVM_RAM)
    *(.bss.LW_NVM_BACKUP_RAM)
    . = ALIGN(8);
  } >NVM_RAM

  /* Used by the startup to initialize MB_MEM2 */
  _siMB_MEM2 = LOADADDR(.MB_MEM2);

  /* Initialized data sections into "MB_MEM2" Ram type memory */
  .MB_MEM2 :
  {
    . = ALIGN(8);
    _sMB_MEM2 = .;
    *(MB_MEM2)

    . = ALIGN(8);
    _eMB_MEM2 = .;
  } >RAM_SHARED AT> APPLI_ROM

  /* Initialized data sections into "MB_MEM3" Ram type memory */
  .MB_MEM3 (NOLOAD) :
  {
    . = ALIGN(8);
    _sMB_MEM3 = .;
    *(MB_MEM3)

    . = ALIGN(8);
    _eMB_MEM3 = .;
  } >RAM_SHARED

  /* Extra ROM section (last one) to make sure the binary size is a multiple of the AES block size (16 bytes) and F7 flash writing unit (4 bytes)*/
  .align16 :
  {
    . = . + 1;         /* _edata=. is aligned on 8 bytes so could be aligned on 16 bytes: add 1 byte gap */
    . = ALIGN(16) - 1; /* increment the location counter until next 16 bytes aligned address (-1 byte)   */
    BYTE(0);           /* allocate 1 byte (value is 0) to be a multiple of 16 bytes                      */
  } >APPLI_ROM

  /* Uninitialized data section into "SRAM2" Ram type memory */
  . = ALIGN(8);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(8);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >APPLI_RAM

  /* User_heap_stack section, used to check that there is enough "SRAM2" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >APPLI_RAM

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
