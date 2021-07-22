/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Swap sector : 4 kbytes */  
SWAP_start           = 0x08015000;  
SWAP_end             = 0x08015FFF;

/* Active slot #1 (84 kbytes) */
SLOT_Active_1_start  = 0x0802B000;  
SLOT_Active_1_end    = 0x0803FFFF;
SLOT_Active_1_header = SLOT_Active_1_start;

/* Dwl slot #1 : (84 kbytes) */
SLOT_Dwl_1_start     = 0x08016000;
SLOT_Dwl_1_end       = 0x0802AFFF;

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

