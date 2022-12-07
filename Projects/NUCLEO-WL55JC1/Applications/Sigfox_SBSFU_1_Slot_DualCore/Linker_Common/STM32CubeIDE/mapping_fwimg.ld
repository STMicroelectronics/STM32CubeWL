/*
** LinkerScript
*/

/* Slots must be aligned on sector size */

/* Active slot #1 (56 kbytes) */
SLOT_Active_1_header  = 0x0803F800;
SLOT_Active_1_start   = 0x0801C000;
SLOT_Active_1_end     = 0x08029FFF;

/* Active slot #2 (100 kbytes) */
SLOT_Active_2_header  = 0x0803F000;
SLOT_Active_2_start   = 0x08003000;
SLOT_Active_2_end     = 0x0801BFFF;

/* Dwl slot #1 : (2 kbytes) */
SLOT_Blob_Dwl_1_start = 0x08002800;
SLOT_Blob_Dwl_1_end   = 0x08002FFF;

/* Slots not configured */
SLOT_Active_3_header  = 0x00000000;
SLOT_Active_3_start   = 0x00000000;
SLOT_Active_3_end     = 0x00000000;

