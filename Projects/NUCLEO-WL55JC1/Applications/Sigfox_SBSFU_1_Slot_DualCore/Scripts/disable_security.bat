@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

REM ###########################################
REM # 0- Set all global variables
REM ###########################################
if not defined CUBEPROG_EXE (
  call setenv.bat
)

echo ###########################################
echo # 1- Disable security
echo ###########################################
echo RDP: Read Out protection Level 1
call :write_ob RDP 0xBB || goto :eof

echo RDP+ESE: Read Out protection Level 0 + Security disabled
call :write_ob RDP 0xAA ESE 0x0 || goto :eof

echo WRP: Write Protection disabled
call :write_ob WRP1A_STRT 0x7F WRP1A_END 0x0 WRP1B_STRT 0x7F WRP1B_END 0x0 || goto :eof

echo ------ User Configuration ------
echo nRST: No reset generated when entering the Stop/Standby/Shutdown modes
call :write_ob nRST_STOP 0x1 nRST_STDBY 0x1 nRST_SHDW 0x1 || goto :eof

echo WDG_SW: Software window/independent watchdogs
call :write_ob WWDG_SW 0x1 IWDG_SW 0x1 || goto :eof

echo IWDG: Independent watchdog counter frozen in Stop/Standby modes
call :write_ob IWGD_STDBY 0x0 IWDG_STOP 0x0 || goto :eof

echo BOOT: CPU1+CPU2 CM0+ Boot lock disabled
call :write_ob BOOT_LOCK 0x0 C2BOOT_LOCK 0x0 || goto :eof

echo ------ Security Configuration ------
echo HDPAD: User Flash hide protection area access disabled
call :write_ob HDPAD 0x1 || goto :eof

echo SPISD: SPI3 security disabled
call :write_ob SUBGHSPISD 0x1 || goto :eof

echo SBRSA: Reset default value of SRAM Start address secure
call :write_ob SNBRSA 0x1F SBRSA 0x1F || goto :eof

echo SBRV: Reset default value of CPU2 Boot start address
call :write_ob SBRV 0x8000 || goto :eof

exit /B 0

:--------------------------------------
:write_ob
  set len_ob=0
  :next
  if not "%~1" == "" if not "%~2" == "" (
    set option_byte_name[!len_ob!]=%~1
    set option_byte_val[!len_ob!]=%~2
    set /a len_ob+=1
    shift
    shift
    goto next
  )
  set /a len_ob-=1
  set obj_str=
  for /L %%i in (0,1,%len_ob%) do (
    set obj_str=!option_byte_name[%%i]!=!option_byte_val[%%i]! !obj_str! 
  )

  %CUBEPROG_EXE% -c port=SWD mode=UR -q -ob !obj_str! >nul 2>nul
  if %errorlevel% neq 0 (
    echo Error: Command write -ob !obj_str! Failed
    exit /B 1
  )
  %CUBEPROG_EXE% -c port=SWD mode=UR -q -ob displ > temp.txt
  
  REM Get line containing %~1 Option Byte
  for /L %%i in (0,1,%len_ob%) do (
    for /f "tokens=3" %%h in ('findstr /c:"     !option_byte_name[%%i]!" temp.txt') do set "read_ob=%%h"
    if "!read_ob!" neq "!option_byte_val[%%i]!" (
      echo Error: Option Byte !option_byte_name[%%i]! not modified as expected: read=!read_ob! expected=!option_byte_val[%%i]!
      del /Q temp.txt >nul
      exit /B 1
    )
  )
  
  del /Q temp.txt >nul
  exit /B 0
