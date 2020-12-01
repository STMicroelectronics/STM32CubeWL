@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
cls

echo ###########################################
echo # 0- Set all global variables
echo ###########################################
call ..\setenv.bat
set compile_all_projects=1
set rebuild=1

REM Get Arguments
:continue
if not "%1"=="" (
  if "%1"=="-app" (
    set compile_all_projects=0
  )
  if "%1"=="-norebuild" (
    set rebuild=0
  )
  if [%1]==[/?] goto help
  shift
  goto continue
)

if %compile_all_projects% equ 1 (
  echo ###########################################
  echo # 1- Compile all SBSFU projects
  echo ###########################################
  call :compile_func ..\..\2_Images_KMS_Blob\EWARM\Project.ewp || goto :eof
  call :compile_func ..\..\2_Images_SECoreBin\EWARM\Project.ewp || goto :eof
  call :compile_func ..\..\2_Images_SBSFU\EWARM\cm0plus.ewp || goto :eof
  call :compile_func ..\..\2_Images_SBSFU\EWARM\cm4.ewp || goto :eof
)

echo ###########################################
echo # 2- Compile Application projects
echo ###########################################
call :compile_func ..\..\LoRaWAN_End_Node_DualCore\EWARM\LoRaWAN_End_Node_DualCore_CM0PLUS.ewp || goto :eof
call :compile_func ..\..\LoRaWAN_End_Node_DualCore\EWARM\LoRaWAN_End_Node_DualCore_CM4.ewp || goto :eof

exit /B 0

:--------------------------------------
:compile_func
  if %rebuild% equ 1 (
    %EWARM_EXE% "%~dp0%~1" -clean * -parallel 10
    %EWARM_EXE% "%~dp0%~1" -make * -parallel 10
  ) else (
    %EWARM_EXE% "%~dp0%~1" * -parallel 10
  )
  if %errorlevel% neq 0 (
    echo Error: Compilation of %~1 Failed. Abort of compile batch ...
    exit /B 1
  )
  exit /B 0

:help
  echo ###########################################
  echo # HELP
  echo ###########################################
  echo Usage: %0 [-app ^| -norebuild ^| /?]
  echo.
  echo -app        Compile only Application projects (exclude SBSFU projects)
  echo -norebuild  Partial compilation
  echo ?           Print Help
  goto eof
