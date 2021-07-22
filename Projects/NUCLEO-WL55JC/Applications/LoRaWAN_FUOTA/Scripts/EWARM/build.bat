@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
cls

set "current_dir=%~dp0"

echo ###########################################
echo # 0- Set all global variables
echo ###########################################
call %current_dir%..\setenv.bat
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
  echo # 1- Compile all BFU projects
  echo ###########################################
  call :compile_func %current_dir%..\..\1_Image_KMS_Blob\EWARM\Project.ewp || goto :eof
  call :compile_func %current_dir%..\..\1_Image_SECoreBin\EWARM\Project.ewp || goto :eof
  call :compile_func %current_dir%..\..\1_Image_BFU\EWARM\Project.ewp || goto :eof
)

echo ###########################################
echo # 2- Compile Application project
echo ###########################################
call :compile_func %current_dir%..\..\LoRaWAN_End_Node\EWARM\LoRaWAN_End_Node.ewp || goto :eof

exit /B 0

:--------------------------------------
:compile_func
  if %rebuild% equ 1 (
    %EWARM_EXE% "%~1" -clean * -parallel 10
    %EWARM_EXE% "%~1" -make * -parallel 10
  ) else (
    %EWARM_EXE% "%~1" * -parallel 10
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
