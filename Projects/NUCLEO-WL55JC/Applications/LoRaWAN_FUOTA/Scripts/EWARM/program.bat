@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
cls

echo ###########################################
echo # 0- Set all global variables
echo ###########################################
call ..\setenv.bat

echo ###########################################
echo # 1- Disable security
echo ###########################################
call ..\disable_security.bat || goto :eof
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
call :download_file ..\..\LoRaWAN_End_Node_DualCore\Binary\SBSFU_UserApp_M4.bin Big_Binary 0x08000000 || goto :eof

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
