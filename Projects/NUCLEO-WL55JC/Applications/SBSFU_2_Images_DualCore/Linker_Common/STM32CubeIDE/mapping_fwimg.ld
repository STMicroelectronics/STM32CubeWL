/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Swap sector : 4 kbytes */  
SWAP_start           = 0x08005800;  
SWAP_end             = 0x080067FF;

/* Active slot #1 (56 kbytes) */
SLOT_Active_1_header = 0x0803F800;
SLOT_Active_1_start  = 0x0801C000;  
SLOT_Active_1_end    = 0x08029FFF;

/* Active slot #2 (28 kbytes) */
SLOT_Active_2_header = 0x0803F000;
SLOT_Active_2_start  = 0x08015000;  
SLOT_Active_2_end    = 0x0801BFFF;

/* Dwl slot #1 : (56 kbytes) */
SLOT_Dwl_1_start     = 0x08006800;  
SLOT_Dwl_1_end       = 0x080147FF;

/* Slots not configured */
SLOT_Active_3_header = 0x00000000;
SLOT_Active_3_start  = 0x00000000;
SLOT_Active_3_end    = 0x00000000;
SLOT_Dwl_2_start     = 0x00000000;
SLOT_Dwl_2_end       = 0x00000000;
SLOT_Dwl_3_start     = 0x00000000;
SLOT_Dwl_3_end       = 0x00000000;

