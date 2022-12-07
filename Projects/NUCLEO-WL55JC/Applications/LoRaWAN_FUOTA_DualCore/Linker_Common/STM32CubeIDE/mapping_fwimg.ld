/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Swap sector : 4 kbytes */  
SWAP_start           = 0x08002000;
SWAP_end             = 0x08002FFF;

/* Dwl slot #1 : (64 kbytes) */
SLOT_Dwl_1_start     = 0x08003000;
SLOT_Dwl_1_end       = 0x08012FFF;

/* Active slot #2 (44 kbytes) */
SLOT_Active_2_header = 0x0803F000;
SLOT_Active_2_start  = 0x08013000;
SLOT_Active_2_end    = 0x0801DFFF;

/***** NVM FLASH Data *****/
/* WARNING: Symbols not used. The NVMS address is defined in lora_app.c */
LW_NVM_ROM_start__   = 0x0801E000;
LW_NWM_ROM_end__     = 0x0801EFFF;

/* Active slot #1 (64 kbytes) */
SLOT_Active_1_header = 0x0803F800;
SLOT_Active_1_start  = 0x0801F000;
SLOT_Active_1_end    = 0x0802EFFF;

/* Slots not configured */
SLOT_Active_3_header = 0x00000000;
SLOT_Active_3_start  = 0x00000000;
SLOT_Active_3_end    = 0x00000000;
SLOT_Dwl_2_start     = 0x00000000;
SLOT_Dwl_2_end       = 0x00000000;
SLOT_Dwl_3_start     = 0x00000000;
SLOT_Dwl_3_end       = 0x00000000;

