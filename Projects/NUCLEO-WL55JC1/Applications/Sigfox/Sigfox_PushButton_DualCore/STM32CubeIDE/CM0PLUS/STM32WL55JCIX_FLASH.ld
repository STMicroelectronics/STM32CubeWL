/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32WL55xC Device
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
_estack = ORIGIN(RAM2) + LENGTH(RAM2); /* end of "RAM2" Ram type memory */

_Min_Heap_Size = 0x400; /* required amount of heap  */
_Min_Stack_Size = 0x1000; /* required amount of stack */

/* Memories definition */
MEMORY
{
  RAM_SHARED (xrw): ORIGIN = 0x20009000, LENGTH = 4K
  RAM2   (xrw)    : ORIGIN = 0x2000A800, LENGTH = 22K
  VM     (xrw)    : ORIGIN = 0x2000A000, LENGTH = 2K
  FLASH   (rx)    : ORIGIN = 0x08020000, LENGTH = 120K
  USER_Key_region_ROM (rx) : ORIGIN = 0x0803E500, LENGTH = 768
}

/* Sections */
SECTIONS
{
  /* The startup code into "FLASH" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >FLASH

  /* The program code and other data into "FLASH" Rom type memory */
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
  } >FLASH

  /* Constant data into "FLASH" Rom type memory */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH

  .ARM.extab   : {
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >FLASH

  .ARM : {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >FLASH

  .preinit_array     :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(8);
  } >FLASH

  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(8);
  } >FLASH

  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(8);
  } >FLASH

  .USER_embedded_Keys : {
    . = ALIGN(8);
    *(.USER_embedded_Keys.encrypted_sigfox_data) 
    *(.USER_embedded_Keys)
    . = ALIGN(8);
  } >USER_Key_region_ROM

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM2" Ram type memory */
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

  } >RAM2 AT> FLASH

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
  } >RAM_SHARED AT> FLASH

  /* Initialized data sections into "MB_MEM3" Ram type memory */
  .MB_MEM3 (NOLOAD) :
  {
    . = ALIGN(8);
    _sMB_MEM3 = .;
    *(MB_MEM3)

    . = ALIGN(8);
    _eMB_MEM3 = .;
  } >RAM_SHARED

  /* Uninitialized data section into "VM" Ram type memory */
  . = ALIGN(8);
  .bss :
  {
    . = ALIGN(8);
  } >VM

  /* Uninitialized data section into "RAM2" Ram type memory */
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
  } >RAM2

  /* User_heap_stack section, used to check that there is enough "RAM2" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM2

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
