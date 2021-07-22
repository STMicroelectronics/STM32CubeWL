/*
** LinkerScript
*/

APPLI_region_ROM_start    = 0x00000000;
APPLI_region_ROM_length   = 0x8000;

/* Specify the memory areas */
MEMORY
{
 APPLI_region_ROM  : ORIGIN = APPLI_region_ROM_start, LENGTH = APPLI_region_ROM_length
}

/* Define output sections */
SECTIONS
{
  .rodata :
  {
    . = ALIGN(16);
    KEEP(*(.KMS_blob_Keys))
    . = ALIGN(16);
  } >APPLI_region_ROM
  
  /DISCARD/ :
  {
    *(.init)
    *(.fini)
  }
}