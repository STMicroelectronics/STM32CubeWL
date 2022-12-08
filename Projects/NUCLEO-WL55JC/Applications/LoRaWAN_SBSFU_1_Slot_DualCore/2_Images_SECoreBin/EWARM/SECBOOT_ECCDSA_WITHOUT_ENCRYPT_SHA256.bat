::Post build for SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
:: arg1 is the build directory
:: arg2 is the elf file path+name
:: arg3 is the bin file path+name
:: arg4 is the firmware Id (1/2/3)
:: arg5 is the version
:: arg6 when present forces "bigelf" generation
@echo off
set "projectdir=%1"
set "execname=%~n3"
set "elf=%2"
set "bin=%3"
set "fwid=%4"
set "version=%5"

set "SBSFUBootLoader=%~d0%~p0\\..\\.."
::The default installation path of the Cube Programmer tool is: "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
::If you installed it in another location, please update the %programmertool% variable below accordingly.
set "programmertool="C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin\\STM32_Programmer_CLI""
set "userAppBinary=%projectdir%\\..\\EWARM\\Binary\\"

set "sfb=%userAppBinary%\\%execname%.sfb"
set "sign=%userAppBinary%\\%execname%.sign"
set "headerbin=%userAppBinary%\\%execname%sfuh.bin"
set "bigbinary=%userAppBinary%\\SBSFU_%execname%.bin"
set "elfbackup=%userAppBinary%\\SBSFU_%execname%.elf"

set "oemkey="
set "ecckey=%SBSFUBootLoader%\\2_Images_SECoreBin\\Binary\\ECCKEY%fwid%.txt"
IF "%fwid%"=="2" (
set "priorbin=%userAppBinary%\\SBSFU_LoRaWAN_End_Node_DualCore_CM0Plus.bin"
set "sbsfuelf=%SBSFUBootLoader%\\2_Images_SBSFU\\EWARM\\2_Images_SBSFU_CM4\\Exe\\Project.out"
goto setkeys
)
set "sbsfuelf=%SBSFUBootLoader%\\2_Images_SBSFU\\EWARM\\2_Images_SBSFU_CM0PLUS\\Exe\\Project.out"
:setkeys
set "mapping=%SBSFUBootLoader%\\Linker_Common\\EWARM\\mapping_fwimg.icf"
set "magic=SFU%fwid%"
set "offset=512"

::comment this line to force python
::python is used if windows executable not found
pushd %projectdir%\..\..\..\..\..\..\Middlewares\ST\STM32_Secure_Engine\Utilities\KeysAndImages
set basedir=%cd%
popd
goto exe:
goto py:
:exe
::line for window executable
echo Postbuild with windows executable
set "prepareimage=%basedir%\\win\\prepareimage\\prepareimage.exe"
set "python="
if exist %prepareimage% (
goto postbuild
)
:py
::line for python
echo Postbuild with python script
set "prepareimage=%basedir%\\prepareimage.py"
set "python=python "
:postbuild

::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%userAppBinary%" (
mkdir "%userAppBinary%" > %projectdir%\output_%fwid%.txt 2>&1
IF %ERRORLEVEL% NEQ 0 goto :error
)

::PostBuild is fired if elf has been regenerated from last run, so elf is different from backup elf
if exist %elfbackup% (
fc %elfbackup% %elf% >NUL && (goto nothingtodo) || echo "elf has been modified, processing required"
)

:: no encryption
set "command=%python%%prepareimage% sha256 %bin% %sign% > %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

:: no encryption so pack the binary file
set "command=%python%%prepareimage% pack -m %magic% -k %ecckey% -p 1 -r 44 -v %version% -f %bin% -t %sign% %sfb% -o %offset% >> %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
set "command=%python%%prepareimage% header -m %magic% -k %ecckey% -p 1 -r 44 -v %version% -f %bin% -t %sign%  -o %offset% %headerbin% >> %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

::get Header address when not contiguous with firmware
if exist %projectdir%\header_%fwid%.txt (
  del %projectdir%\header_%fwid%.txt
)
set "command=%python%%prepareimage% extract -d "__ICFEDIT_SLOT_Active_%fwid%_header__" %mapping% > %projectdir%\header_%fwid%.txt"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
set /P header=<%projectdir%\header_%fwid%.txt >> %projectdir%\output_%fwid%.txt 2>>&1
echo header %header% >> %projectdir%\output_%fwid%.txt 2>>&1

if exist %priorbin%.baseadd (goto addpriorbin)
set extramergebin=
goto buildmergecmd
:addpriorbin
set /P priorbinbaseadd=<%priorbin%.baseadd
set extramergebin=%priorbin%@%priorbinbaseadd%;
:buildmergecmd
set "command=%python%%prepareimage% mergev2 -v 0  -b %extramergebin%%headerbin%@%header% -f %sbsfuelf%;%elf% %bigbinary% >> %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error


:bigelf
IF  "%~6"=="" goto :finish
echo "Generating the global elf file (SBSFU and userApp)"
echo "Generating the global elf file (SBSFU and userApp)" >> %projectdir%\output_%fwid%.txt
set "command=%programmertool% -ms %elf% %headerbin% %sbsfuelf% >> %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
set "command=copy %elf% %elfbackup%  >> %projectdir%\output_%fwid%.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

:finish
::backup and clean up the intermediate file
del %sign%
del %sfu%
del %headerbin%

exit 0

:error
echo "%command% : failed" >> %projectdir%\\output_%fwid%.txt
:: remove the elf to force the regeneration
if exist %elf%(
  del %elf%
)
if exist %elfbackup%(
  del %elfbackup%
)
echo %command% : failed

pause
exit 1

:nothingtodo
exit 0
