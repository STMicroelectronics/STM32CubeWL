/*
** LinkerScript
*/

/* Entry Point */
ENTRY(SE_CallGate)

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0;         /* specific stack provided inside secured area */

INCLUDE mapping_fwimg.ld
INCLUDE mapping_sbsfu.ld

/* Define output sections */
SECTIONS
{
  .SE_CallGate_Code :
  {
    BYTE(0);           /* dummy byte to keep the callgate at INTVECT_start + Vector_size + 4 */
    BYTE(0);           /* dummy byte to keep the callgate at INTVECT_start + Vector_size + 4 */
    BYTE(0);           /* dummy byte to keep the callgate at INTVECT_start + Vector_size + 4 */
    BYTE(0);           /* dummy byte to keep the callgate at INTVECT_start + Vector_size + 4 */
    KEEP(*(.SE_CallGate_Code))
  } > SE_CallGate_ROM_region

  .SE_Startup_Code : { KEEP ( *se_startup.o (.text .text*)) } > SE_Startup_ROM_region
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
  } >SE_Code_NoKey_ROM_region

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >SE_Code_NoKey_ROM_region

  .ARM.extab   : 
  { 
  . = ALIGN(8);
  *(.ARM.extab* .gnu.linkonce.armextab.*)
  . = ALIGN(8);
  } >SE_Code_NoKey_ROM_region

  .ARM : {
	. = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
	. = ALIGN(8);
  } >SE_Code_NoKey_ROM_region

  .preinit_array     :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
	. = ALIGN(8);
  } >SE_Code_NoKey_ROM_region
  
  .init_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
	. = ALIGN(8);
  } >SE_Code_NoKey_ROM_region

  .fini_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
	. = ALIGN(8);
  } >SE_Code_NoKey_ROM_region

  .USER_embedded_Keys : { KEEP(*(.USER_embedded_Keys)) } > User_Key_region_ROM
  .SE_embedded_Keys : { KEEP(*(.SE_embedded_Keys)) } > SE_Key_region_ROM
  
  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >SE_RAM_region AT>SE_Code_NoKey_ROM_region 

  
  /* Uninitialized data section */
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
  } >SE_RAM_region

  /* User_heap_stack section, used to check that there is enough "SRAM1" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >SE_RAM_region
   
  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

