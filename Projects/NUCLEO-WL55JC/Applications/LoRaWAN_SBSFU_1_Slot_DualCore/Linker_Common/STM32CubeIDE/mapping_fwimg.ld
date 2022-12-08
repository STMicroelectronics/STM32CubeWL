/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Dwl slot #1 : (2 kbytes) */
SLOT_Blob_Dwl_1_start = 0x08002800;
SLOT_Blob_Dwl_1_end   = 0x08002FFF;

/* Active slot #2 (96 kbytes) */
SLOT_Active_2_header  = 0x0803F000;
SLOT_Active_2_start   = 0x08003000;
SLOT_Active_2_end     = 0x0801AFFF;

/***** NVM FLASH Data *****/
/* WARNING: Symbols not used. The NVMS address is defined in lora_app.c */
LW_NVM_ROM_start__    = 0x0801B000;
LW_NWM_ROM_end__      = 0x0801BFFF;

/* Active slot #1 (58 kbytes) */
SLOT_Active_1_header  = 0x0803F800;
SLOT_Active_1_start   = 0x0801C000;
SLOT_Active_1_end     = 0x0802A7FF;

/* Slots not configured */
SLOT_Active_3_header  = 0x00000000;
SLOT_Active_3_start   = 0x00000000;
SLOT_Active_3_end     = 0x00000000;

