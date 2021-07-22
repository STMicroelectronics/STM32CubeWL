@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
cls

set "current_dir=%~dp0"

echo ###########################################
echo # 0- Set all global variables
echo ###########################################
call %current_dir%..\setenv.bat

echo ###########################################
echo # 1- Disable security
echo ###########################################
call %current_dir%..\disable_security.bat || goto :eof
if %errorlevel% neq 0 goto :eof

echo ###########################################
echo # 2- Erase Memory
echo ###########################################
%CUBEPROG_EXE% -c port=SWD mode=UR -e all
if %errorlevel% neq 0 (
  echo Error: Full Memory Erase Failure
  goto :eof
)

echo ###########################################
echo # 3- Download binaries
echo ###########################################
call :download_file %current_dir%..\..\Sigfox_PushButton_DualCore\STM32CubeIDE\Binary\SBSFU_Sigfox_PushButton_DualCore_CM4.bin Big_Binary 0x08000000 || goto :eof

echo Power cycle the board (unplug/plug USB cable) to apply the SBSFU security mechanisms...
exit /B 0

:--------------------------------------
:download_file
  if not exist %~1 (
    echo Error: %~1 File not found. Check your build log ...
    exit /B 1
  )

  if "%~3" == "" (
    echo Downloading %~2 binary ...
    %CUBEPROG_EXE% -c port=SWD mode=UR -d %~1 -v >nul 2>nul
  ) else (
    echo Downloading %~2 binary @%~3 ...
    %CUBEPROG_EXE% -c port=SWD mode=UR -d %~1 %~3 -v >nul 2>nul
  )
  if %errorlevel% neq 0 (
    echo Error: %~2 Download Failed
    exit /B 1
  )
  
  %CUBEPROG_EXE% -c port=SWD mode=HOTPLUG -hardRst >nul 2>nul
  if %errorlevel% neq 0 (
    echo Error: Reset after download Failed
    exit /B 1
  )
  echo Done
  exit /B 0
