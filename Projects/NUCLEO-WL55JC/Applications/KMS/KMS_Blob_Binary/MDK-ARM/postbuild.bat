::Post build for SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256
@echo off
::comment this line to force python
::python is used if windows executable not found
goto exe:
goto py:
:exe
::line for window executable
set "prepareimage=%1\\..\\..\\..\\..\\..\\..\\Middlewares\\ST\\STM32_Secure_Engine\\Utilities\\KeysAndImages\\win\\prepareimage.exe"
set "python="
if exist %prepareimage% (
goto postbuild
)
:py
::line for python
set "prepareimage=%1\\..\\..\\..\\..\\..\\..\\Middlewares\\ST\\STM32_Secure_Engine\\Utilities\\KeysAndImages\\prepareimage.py"
set "python=python "
:postbuild

set "bin=%1\\%2.bin"
set "kmsu=%1\\..\\Binary\\%2.kmsu"
set "kmsb=%1\\..\\Binary\\%2.kmsb"
set "sign=%1\\..\\Binary\\%2.sign"
set "iv=%1\\..\\Binary\\iv.bin"
set "oemkey=%1\\..\\Binary\\OEM_KEY_COMPANY1_key_AES_CBC.bin"
set "ecckey=%1\\..\\Binary\\ECCKEY.txt"
set "kmsBinary=%1\\..\\Binary\\"

::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%kmsBinary%" (
mkdir "%kmsBinary%"
)

:: When doing the AES CBC encrypt, prepareimage.py pads the input file if needed (to be a multiple of the AES block size = 16 bytes)
set "command=%python%%prepareimage% enc -k %oemkey% -i %iv% %bin% %kmsu%  > %1\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
:: -p 16 indicates that the input file must be a multiple of the AES block size (16 bytes). Padding is done before computing the sha256 if needed.
set "command=%python%%prepareimage% sha256 %1\\%2.bin %sign%  -p 16 >> %1\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
set "command=%python%%prepareimage% pack -k %ecckey%  -m "KMSB" -r 28 -v %3 -i %iv% -f %kmsu% -t %sign% -o 320 %kmsb% >> %1\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error


::clean up the intermediate file
del %sign%
del %kmsu%


exit 0
:error
:: remove the elf to force the regeneration
del %elf%
exit 1
