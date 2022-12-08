@echo off
echo prebuild.bat : started > %1\\output.txt
set "SBSFUBootLoader=%~d0%~p0\\..\\.."
::comment this line to force python
::python is used if windows executable not found
pushd %1\..\..\..\..\..\..\Middlewares\ST\STM32_Secure_Engine\Utilities\KeysAndImages
set basedir=%cd%
popd
goto exe:
goto py:
:exe
::line for window executable
echo Prebuild with windows executable
set "prepareimage=%basedir%\\win\\prepareimage.exe"
set "python="
if exist %prepareimage% (
goto prebuild
)
:py
::line for python
echo Prebuild with python script
set "prepareimage=%basedir%\\prepareimage.py"
set "python=python "
echo "python: %prepareimage%" >> %1\\output.txt 2>>&1
:prebuild
set "crypto_h=%1\\..\\Inc\\se_crypto_config.h"

::clean
if exist %1\\crypto.txt (
  del %1\\crypto.txt
)

if exist %1\\postbuild.bat (
  del %1\\postbuild.bat
)

::get crypto name
set "command=%python%%prepareimage% conf %crypto_h% > %1\\crypto.txt"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set /P crypto=<%1\\crypto.txt >> %1\\output.txt 2>>&1
echo crypto %crypto% selected >> %1\\output.txt 2>>&1

set "cortex=V6M"

set "KMS_config=%1\kms_platf_objects_config.h"
set "KMS_tmp=%1\kms_platf_objects_config.h.tmp"

::Patch KMS embedded keys with Blob Keys
set "kmsblob_ecckey=%SBSFUBootLoader%\\2_Images_KMS_Blob\\Binary\\ECCKEY.txt"
set "command=%python%%prepareimage% inject -k %kmsblob_ecckey% -f %1\..\Inc\kms_platf_objects_config.h.pattern -p @ECDSA_BLOB_KEY@ %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "kmsblob_oemkey=%SBSFUBootLoader%\\2_Images_KMS_Blob\\Binary\\OEM_KEY_COMPANY1_key_AES_CBC.bin"
set "command=%python%%prepareimage% inject -k %kmsblob_oemkey% -f %KMS_tmp% -p @AES_BLOB_KEY@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

::Provide data for the blob encryption feature
set "kmsblob_aeskey=%SBSFUBootLoader%\\2_Images_KMS_Blob\\Binary\\AES_DATA_STORAGE_key.bin"
set "command=%python%%prepareimage% inject -k %kmsblob_aeskey% -f %KMS_tmp% -p @AES_DATA_STORAGE_KEY@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "kmsblob_aesdata=%SBSFUBootLoader%\\2_Images_KMS_Blob\\Binary\\AES_DATA_STORAGE_data.bin"
set "command=%python%%prepareimage% inject -k %kmsblob_aesdata% -f %KMS_tmp% -p @AES_DATA_STORAGE_DATA@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

if "%crypto%"=="SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM" (
  set "type=GCM"
  goto AES128
)
if "%crypto%"=="SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256" (
  set "type=CBC"
  goto AES128
)
::For KMS dummy AES keys are needed (reusing AES_CBC inputs files)
if "%crypto%"=="SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256" (
  set "type=CBC"
  goto AES128
)
goto end

:AES128
set "oemkey=%1\\..\\Binary\\OEM_KEY_COMPANY1_key_AES_%type%.bin"
set "command=%python%%prepareimage% inject -k %oemkey% -f %KMS_tmp% -p @AES_SBSFU_KEY_1@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

set "oemkey=%1\\..\\Binary\\OEM_KEY_COMPANY2_key_AES_%type%.bin"
IF NOT EXIST %oemkey% goto AES128_end
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=%python%%prepareimage% inject -k %oemkey% -f %KMS_tmp% -p @AES_SBSFU_KEY_2@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

set "oemkey=%1\\..\\Binary\\OEM_KEY_COMPANY3_key_AES_%type%.bin"
IF NOT EXIST %oemkey% goto AES128_end
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=%python%%prepareimage% inject -k %oemkey% -f %KMS_tmp% -p @AES_SBSFU_KEY_3@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

:AES128_end
::For KMS dummy ECC keys are needed
::if "%crypto%"=="SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM" goto end
goto ECDSA

:ECDSA
set "ecckey=%1\\..\\Binary\\ECCKEY1.txt"
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=%python%%prepareimage% inject -k %ecckey% -f %KMS_tmp% -p @ECDSA_SBSFU_KEY_1@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

set "ecckey=%1\\..\\Binary\\ECCKEY2.txt"
IF NOT EXIST %ecckey% goto end
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=%python%%prepareimage% inject -k %ecckey% -f %KMS_tmp% -p @ECDSA_SBSFU_KEY_2@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

set "ecckey=%1\\..\\Binary\\ECCKEY3.txt"
IF NOT EXIST %ecckey% goto end
set "command=copy %KMS_config% %KMS_tmp%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
set "command=%python%%prepareimage% inject -k %ecckey% -f %KMS_tmp% -p @ECDSA_SBSFU_KEY_3@ %KMS_config%"
%command%
IF %ERRORLEVEL% NEQ 0 goto error

goto end


:end
del %KMS_tmp%
set "command=copy %1\\%crypto%.bat %1\\postbuild.bat"
%command%
IF %ERRORLEVEL% NEQ 0 goto error
exit 0

:error
echo %command% : failed >> %1\\output.txt 2>&1
echo %command% : failed
pause
exit 1
