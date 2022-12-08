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

if not exist !CUBEIDE_EXE! (
  REM try with a short link
  set CUBEIDE_EXE="!CUBEIDE_EXE:"=!.lnk"
  if not exist !CUBEIDE_EXE! (
    echo Error: the command line !CUBEIDE_EXE! doesn't exist.
    echo        Check the setenv.bat content and your installation path
    exit /B 1
  )
)

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

set "workspacedir=%current_dir%..\..\Sigfox_PushButton_DualCore\STM32CubeIDE\Binary\ws"

if %compile_all_projects% equ 1 (
  echo ###########################################
  echo # 1- Compile all SBSFU projects
  echo ###########################################
  call :compile_func %current_dir%..\..\2_Images_KMS_Blob\STM32CubeIDE || goto :eof
  call :compile_func %current_dir%..\..\2_Images_SECoreBin\STM32CubeIDE || goto :eof
  call :compile_func %current_dir%..\..\2_Images_SBSFU\STM32CubeIDE\CM0PLUS || goto :eof
  call :compile_func %current_dir%..\..\2_Images_SBSFU\STM32CubeIDE\CM4 || goto :eof
)

echo ###########################################
echo # 2- Compile Application projects
echo ###########################################
call :compile_func %current_dir%..\..\Sigfox_PushButton_DualCore\STM32CubeIDE\CM0PLUS || goto :eof
call :compile_func %current_dir%..\..\Sigfox_PushButton_DualCore\STM32CubeIDE\CM4 || goto :eof

exit /B 0

:--------------------------------------
:compile_func
  if not exist %workspacedir% (
    mkdir %workspacedir% 2>NUL
    if %ERRORLEVEL% NEQ 0 exit /B 1
  ) else (
    del /q "%workspacedir%\*"
    for /D %%p in ("%workspacedir%\*.*") do rmdir "%%p" /s /q
  )
  %CUBEIDE_EXE% --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data "%workspacedir%" -import "%~1" -build all
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
