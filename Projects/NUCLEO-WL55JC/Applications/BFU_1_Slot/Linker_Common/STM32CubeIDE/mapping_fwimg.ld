/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Active slot #1 (194 kbytes) */
SLOT_Active_1_start  = 0x0800F800;  
SLOT_Active_1_end    = 0x0803FFFF;
SLOT_Active_1_header = SLOT_Active_1_start;

/* Dwl slot #1 : (2 kbytes) */
SLOT_Blob_Dwl_1_start = 0x0800F000;  
SLOT_Blob_Dwl_1_end   = 0x0800F7FF;

/* Slots not configured */
SLOT_Active_2_header = 0x00000000;
SLOT_Active_2_start  = 0x00000000;  
SLOT_Active_2_end    = 0x00000000;
SLOT_Active_3_header = 0x00000000;
SLOT_Active_3_start  = 0x00000000;
SLOT_Active_3_end    = 0x00000000;

