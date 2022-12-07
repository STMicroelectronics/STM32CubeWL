/*
** LinkerScript
*/

/******************************************************************************/
/*                               ROM section                                  */
/******************************************************************************/

/* Vector table */
INTVECT_start                  = 0x08000000;
Vector_size                    = 0x200;

/* SE Code region protected by MPU isolation */
SE_Code_region_ROM_start       = INTVECT_start + Vector_size;
SE_CallGate_region_ROM_start   = SE_Code_region_ROM_start; /* No need to do +4 as we have dummy bytes in SE_CoreBin .ld file */
SE_CallGate_region_ROM_end     = SE_Code_region_ROM_start + 0xFF;

/* SE Embedded Keys */
SE_Key_region_ROM_start        = SE_CallGate_region_ROM_end + 0x1;
SE_Key_region_ROM_end          = SE_Key_region_ROM_start + 0x2FF;

/* SE Startup */
SE_Startup_region_ROM_start    = SE_Key_region_ROM_end + 0x1;
SE_Code_NoKey_region_ROM_start = SE_Startup_region_ROM_start + 0x100;
SE_Code_region_ROM_end         = SE_Startup_region_ROM_start + 0x80FF;

/* User Embedded Keys */
User_Key_region_ROM_start      = SE_Code_region_ROM_end - 0x2FF; /* 0x08008400; */
User_Key_region_ROM_end        = SE_Code_region_ROM_end;


/* SE IF ROM */
SE_IF_region_ROM_start         = SE_Code_region_ROM_end + 0x1; 
SE_IF_region_ROM_end           = SE_IF_region_ROM_start + 0xEFF;

/* SBSFU Code region */
SB_region_ROM_start            = SE_IF_region_ROM_end + 0x1;
SB_region_ROM_end              = 0x08012FFF;

/* KMS Data Storage (NVMS) region protected area */
/* KMS Data Storage need for 2 images : 4 kbytes * 2 ==> 8 kbytes */
KMS_DataStorage_start          = 0x08013000;
KMS_DataStorage_end            = 0x08014FFF;

/* ROM regions size definition */
SE_CallGate_region_ROM_size    = SE_CallGate_region_ROM_end - SE_CallGate_region_ROM_start + 0x1;
SE_Key_region_ROM_size         = SE_Key_region_ROM_end - SE_Key_region_ROM_start + 0x1;
SE_Startup_region_ROM_size     = SE_Code_NoKey_region_ROM_start - SE_Startup_region_ROM_start;
SE_Code_region_ROM_size        = SE_Code_region_ROM_end - SE_Code_region_ROM_start + 0x1;
SE_Code_NoKey_region_ROM_size  = SE_Code_region_ROM_end - SE_Code_NoKey_region_ROM_start + 0x1;
User_Key_region_ROM_size       = User_Key_region_ROM_end - User_Key_region_ROM_start + 0x1;
SE_IF_region_ROM_size          = SE_IF_region_ROM_end - SE_IF_region_ROM_start + 0x1;
SB_region_ROM_size             = SB_region_ROM_end - SB_region_ROM_start + 0x1;
KMS_DataStorage_size           = KMS_DataStorage_end - KMS_DataStorage_start + 0x1;

/******************************************************************************/
/*                              RAM section                                   */
/*                                                                            */
/******************************************************************************/

/* SE RAM region */
SE_region_RAM_start            = 0x20000000;
SE_region_RAM_stack_top        = 0x20000C00;                /* Secure Engine's private stack */
SE_region_RAM_end              = 0x200033FF;

/* SBSFU RAM region */
SB_region_RAM_start            = SE_region_RAM_end + 0x1;
SB_region_RAM_end              = 0x2000EFFF;

/* NVM RAM Data region */
LW_NVM_region_RAM_start        = SB_region_RAM_end + 0x1;
LW_NVM_region_RAM_end          = 0x2000FFFF;

/* RAM regions size definition */
SE_region_RAM_size             = SE_region_RAM_end - SE_region_RAM_stack_top + 0x1;
SB_region_RAM_size             = SB_region_RAM_end - SB_region_RAM_start + 0x1;
LW_NVM_region_RAM_size         = LW_NVM_region_RAM_end - LW_NVM_region_RAM_start + 0x1;

/******************************************************************************/
/*                              RAM/ROM regions                               */
/*                                                                            */
/******************************************************************************/

MEMORY
{
 INTVECT_ROM_region (rx)          : ORIGIN = INTVECT_start, LENGTH = Vector_size
 SE_CallGate_ROM_region (rx)      : ORIGIN = SE_CallGate_region_ROM_start, LENGTH = SE_CallGate_region_ROM_size
 SE_Key_region_ROM (rx)           : ORIGIN = SE_Key_region_ROM_start, LENGTH = SE_Key_region_ROM_size
 SE_Startup_ROM_region (rx)       : ORIGIN = SE_Startup_region_ROM_start, LENGTH = SE_Startup_region_ROM_size
 SE_ROM_region (rx)               : ORIGIN = SE_CallGate_region_ROM_start, LENGTH = SE_Code_region_ROM_size
 SE_Code_NoKey_ROM_region (rx)    : ORIGIN = SE_Code_NoKey_region_ROM_start, LENGTH = SE_Code_NoKey_region_ROM_size
 User_Key_region_ROM (rx)         : ORIGIN = User_Key_region_ROM_start, LENGTH = User_Key_region_ROM_size
 SE_IF_ROM_region (rx)            : ORIGIN = SE_IF_region_ROM_start, LENGTH = SE_IF_region_ROM_size
 SB_ROM_region (rx)               : ORIGIN = SB_region_ROM_start, LENGTH = SB_region_ROM_size
 SE_RAM_region (xrw)              : ORIGIN = SE_region_RAM_stack_top, LENGTH = SE_region_RAM_size
 SB_RAM_region (xrw)              : ORIGIN = SB_region_RAM_start, LENGTH = SB_region_RAM_size
 LW_NVM_region_RAM (xrw)          : ORIGIN = LW_NVM_region_RAM_start, LENGTH = LW_NVM_region_RAM_size
}
