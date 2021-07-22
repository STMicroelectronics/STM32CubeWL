/*
** LinkerScript
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM1) + LENGTH(RAM1); /* end of "SRAM1" Ram type memory */


/* Memory Regions
/****** FLASH Part dedicated to CPU1 ******/
#define ROM_CPU1_START 					0x08000000
#define ROM_CPU1_LENGTH					0x00020000
/****** FLASH Part dedicated to CPU2 ******/
#define ROM_CPU2_START 					0x08020000
#define ROM_CPU2_LENGTH 				0x00020000

/****** SRAM1 dedicated to CPU1 ******/
#define SRAM1_CPU1_START 				0x20000000 
#define SRAM1_CPU1_LENGTH    			0x00004000
/****** SRAM1 dedicated to CPU2 ***** */
#define SRAM1_CPU2_START  				0x20004000
#define SRAM1_CPU2_LENGTH    			0x00004000

/****** SRAM2 dedicated to CPU1 ******/
#define SRAM2_CPU1_START  				0x20008000
#define SRAM2_CPU1_LENGTH    			0x00004000
/****** SRAM2 dedicated to CPU2 ******/
#define SRAM2_CPU2_START  				0x2000C000
#define SRAM2_CPU2_LENGTH   			0x00004000


MEMORY
{

  #if (CPU1 )
  
  ROM    (rx)    : ORIGIN = ROM_CPU1_START , LENGTH = ROM_CPU1_LENGTH   /* Flash memory dedicated to CM4 */
  RAM1   (xrw)   : ORIGIN = SRAM1_CPU1_START, LENGTH = SRAM1_CPU1_LENGTH    /* Non-backup SRAM1 dedicated to CM4 */
  RAM2   (xrw)   : ORIGIN = SRAM2_CPU1_START, LENGTH = SRAM2_CPU1_LENGTH    /* Backup SRAM2 dedicated to CM4 */
  
  #else
  ROM    (rx)    : ORIGIN = ROM_CPU2_START , LENGTH = ROM_CPU2_LENGTH   /* Flash memory dedicated to CM4 */
  RAM1   (xrw)   : ORIGIN = SRAM1_CPU2_START, LENGTH = SRAM1_CPU2_LENGTH    /* Non-backup SRAM1 dedicated to CM4 */
  RAM2   (xrw)   : ORIGIN = SRAM2_CPU2_START, LENGTH = SRAM2_CPU2_LENGTH    /* Backup SRAM2 dedicated to CM4 */
  #endif
}

_Min_Heap_Size = 0x200 ; /* required amount of heap  */
_Min_Stack_Size = 0x400 ; /* required amount of stack */

/* Sections */
SECTIONS
{
  /* The startup code into "ROM" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >ROM

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
  } >ROM

  /* Constant data into "ROM" Rom type memory */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >ROM

  .ARM.extab   : { 
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >ROM
  
  .ARM : {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >ROM

  .preinit_array     :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(8);
  } >ROM
  
  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(8);
  } >ROM
  
  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(8);
  } >ROM

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "SRAM1" Ram type memory */
  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
    
  } >RAM1 AT> ROM

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
  } >RAM1

  /* Data section into "SRAM1" Ram type memory: Non-backup SRAM1 dedicated to CM4 */
  . = ALIGN(8);
  RAM1_region :
  {
    _sRAM_region = .;         /* define a global symbol at section start */
    *(.RAM1_region)

    . = ALIGN(8);
    _eRAM1_region = .;         /* define a global symbol at section end */
  } >RAM1

  /* Data section into "SRAM2" Ram type memory: Backup SRAM2 dedicated to CM4 */
  . = ALIGN(8);
  RAM2_region :
  {
    _sRAM2_region = .;         /* define a global symbol at section start */
    *(.RAM2_region)

    . = ALIGN(8);
    _eRAM2_region = .;         /* define a global symbol at section end */
  } >RAM2

  /* User_heap_stack section, used to check that there is enough "SRAM1" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM1
  
_ROM2_CPU2_base = ROM_CPU2_START;
_ROM2_CPU2_length = ROM_CPU2_LENGTH;
_SRAM1_CPU2_base = SRAM1_CPU2_START;
_SRAM1_CPU2_length = SRAM1_CPU2_LENGTH ;
_SRAM2_CPU2_base = SRAM2_CPU2_START;
_SRAM2_CPU2_length = SRAM2_CPU2_LENGTH ;
  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }



  .ARM.attributes 0 : { *(.ARM.attributes) }
}
