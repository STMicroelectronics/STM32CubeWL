/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Swap sector : 4 kbytes */  
SWAP_start           = 0x08015000;  
SWAP_end             = 0x08015FFF;

/* Dwl slot #1 : (80 kbytes) */
SLOT_Dwl_1_start     = 0x08016000;
SLOT_Dwl_1_end       = 0x08029FFF;

/* Active slot #1 (80 kbytes) */
SLOT_Active_1_start  = 0x0802A000;  
SLOT_Active_1_end    = 0x0803DFFF;
SLOT_Active_1_header = SLOT_Active_1_start;

/***** NVM FLASH Data *****/
/* WARNING: Symbols not used. The NVMS address is defined in lora_app.c */
LW_NVM_ROM_start__   = 0x0803F000;
LW_NWM_ROM_end__     = 0x0803FFFF;

/* Slots not configured */
SLOT_Active_2_header = 0x00000000;
SLOT_Active_2_start  = 0x00000000;  
SLOT_Active_2_end    = 0x00000000;
SLOT_Active_3_header = 0x00000000;
SLOT_Active_3_start  = 0x00000000;
SLOT_Active_3_end    = 0x00000000;
SLOT_Dwl_2_start     = 0x00000000;
SLOT_Dwl_2_end       = 0x00000000;
SLOT_Dwl_3_start     = 0x00000000;
SLOT_Dwl_3_end       = 0x00000000;

