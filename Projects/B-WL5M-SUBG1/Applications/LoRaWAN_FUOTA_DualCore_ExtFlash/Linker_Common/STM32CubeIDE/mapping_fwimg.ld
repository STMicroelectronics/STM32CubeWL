/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Swap sector : 4 kbytes */  
SWAP_start           = 0x00000000;
SWAP_end             = 0x00000000;

/* Dwl slot #2 : (2 kbytes) */
SLOT_Dwl_2_start     = 0x08006800;
SLOT_Dwl_2_end       = 0x08006FFF;

/* Active slot #2 (52 kbytes) */
SLOT_Active_2_header = 0x0803F000;
SLOT_Active_2_start  = 0x08007000;
SLOT_Active_2_end    = 0x08013FFF;

/***** NVM FLASH Data *****/
/* WARNING: Symbols not used. The NVMS address is defined in lora_app.c */
LW_NVM_ROM_start__   = 0x08014000;
LW_NWM_ROM_end__     = 0x08014FFF;

/* Active slot #1 (82 kbytes) */
SLOT_Active_1_header = 0x0803F800;
SLOT_Active_1_start  = 0x08015000;
SLOT_Active_1_end    = 0x080297FF;

/* Dwl slot #1 : (84 kbytes) */
SLOT_Dwl_1_start     = 0x90000000;
SLOT_Dwl_1_end       = 0x90014FFF;

/* Slots not configured */
SLOT_Active_3_header = 0x00000000;
SLOT_Active_3_start  = 0x00000000;
SLOT_Active_3_end    = 0x00000000;
SLOT_Dwl_3_start     = 0x00000000;
SLOT_Dwl_3_end       = 0x00000000;

