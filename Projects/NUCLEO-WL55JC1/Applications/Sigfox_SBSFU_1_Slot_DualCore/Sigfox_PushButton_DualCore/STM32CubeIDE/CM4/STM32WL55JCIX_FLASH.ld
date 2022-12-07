/*
** LinkerScript
** Note: For specific memory allocation, linker and startup files must be customized.
**       Refer to STM32CubeIDE user guide (UM2609), chapter "Modify the linker script".
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(APPLI_RAM) + LENGTH(APPLI_RAM); /* end of "SRAM1" Ram type memory */

_Min_Heap_Size  = 0x400; /* required amount of heap  */
_Min_Stack_Size = 0x1000; /* required amount of stack */

INCLUDE mapping_fwimg.ld
INCLUDE mapping_sbsfu.ld

/* Memories definition */
MEMORY
{
  ISR_VECTOR (rx)    : ORIGIN = (SLOT_Active_2_start + 0x200), LENGTH = Vector_size
  APPLI_ROM  (xrw)   : ORIGIN = (SLOT_Active_2_start + Vector_size + 0x200), LENGTH = (SLOT_Active_2_end - SLOT_Active_2_start + 1 - 0x200 - Vector_size)
  APPLI_RAM  (xrw)   : ORIGIN = M4_APP_region_RAM_start, LENGTH = M4_APP_region_RAM_size
  RAM_SHARED (xrw)   : ORIGIN = M4_SHARED_region_RAM_start, LENGTH = M4_SHARED_region_RAM_size
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
  } >ISR_VECTOR

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

  /* Initialized data sections into "SRAM1" Ram type memory */
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

  /* Uninitialized data section into "MAPPING_TABLE" Ram type memory */
  MAPPING_TABLE (NOLOAD) :
  {
    . = ALIGN(8);
    _sMAPPING_TABLE = .;
    *(MAPPING_TABLE)

    . = ALIGN(8);
    _eMAPPING_TABLE = .;
  } >RAM_SHARED

  /* Uninitialized data section into "MB_MEM1" Ram type memory */
  MB_MEM1 (NOLOAD) : 
  {
    . = ALIGN(8);
    _sMB_MEM1 = .;
    *(MB_MEM1)

    . = ALIGN(8);
    _eMB_MEM1 = .;
  } >RAM_SHARED

  /* Extra ROM section (last one) to make sure the binary size is a multiple of the AES block size (16 bytes) and F7 flash writing unit (4 bytes)*/
  .align16 :
  {
    . = . + 1;         /* _edata=. is aligned on 8 bytes so could be aligned on 16 bytes: add 1 byte gap */
    . = ALIGN(16) - 1; /* increment the location counter until next 16 bytes aligned address (-1 byte)   */
    BYTE(0);           /* allocate 1 byte (value is 0) to be a multiple of 16 bytes                      */
  } >APPLI_ROM

  /* Uninitialized data section into "SRAM1" Ram type memory */
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

  /* Data section into "SRAM1" Ram type memory: Non-backup SRAM1 dedicated to CM4 */
  . = ALIGN(8);
  RAM1_region :
  {
    _sRAM1_region = .;         /* define a global symbol at section start */
    *(.RAM1_region)

    . = ALIGN(8);
    _eRAM1_region = .;         /* define a global symbol at section end */
  } >APPLI_RAM


  /* User_heap_stack section, used to check that there is enough "SRAM1" Ram  type memory left */
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
