#!/bin/bash - 
#prebuild script 
echo prebuild.sh : started > $1/output.txt
asmfile=$1/se_key.s
# comment this line to force python
# python is used if  executable not found
current_directory=`pwd`
cd $1/../../../../../../Middlewares/ST/STM32_Secure_Engine/Utilities/KeysAndImages
basedir=`pwd`
cd $current_directory
SBSFUBootLoader=$1/../..

# test if window executable usable
prepareimage=$basedir/win/prepareimage/prepareimage.exe
uname  | grep -i -e windows -e mingw > /dev/null 2>&1

if [ $? -eq 0 ] && [  -e "$prepareimage" ]; then
  echo "prepareimage with windows executable" >> $1/output.txt
  echo "prepareimage with windows executable"
  cmd=""
else
  # line for python
  echo "prepareimage with python script" >> $1/output.txt
  echo "prepareimage with python script"
  prepareimage=$basedir/prepareimage.py
  cmd=python
fi

echo "$cmd $prepareimage" >> $1/output.txt
crypto_h=$1/../Inc/se_crypto_config.h

#clean
if [ -e "$1/crypto.txt" ]; then
  rm $1"/crypto.txt"
fi

if [ -e "$asmfile" ]; then
  rm $asmfile
fi

if [ -e "$1"/postbuild.sh ]; then
  rm $1"/postbuild.sh"
fi

#get crypto name
command="$cmd $prepareimage conf "$crypto_h""
echo $command
crypto=`$command`
echo $crypto > $1"/crypto.txt"
echo "$crypto selected">> $1"/output.txt"
echo $crypto selected
ret=$?

kms_config=$1"/kms_platf_objects_config.h"
kms_tmp=$1"/kms_platf_objects_config.h.tmp"

#Patch KMS embedded keys with Blob Keys
if [ $ret -eq 0 ]; then
  kms_blob_ecckey=$SBSFUBootLoader"/2_Images_KMS_Blob/Binary/ECCKEY.txt"
  command=$cmd" "$prepareimage" inject -k "$kms_blob_ecckey" -f "$1"/../Inc/kms_platf_objects_config.h.pattern -p @ECDSA_BLOB_KEY@ "$kms_tmp
  echo $command
  $command
  ret=$?
  if [ $ret -eq 0 ]; then
    kms_blob_oemkey=$SBSFUBootLoader"/2_Images_KMS_Blob/Binary/OEM_KEY_COMPANY1_key_AES_CBC.bin"
    command=$cmd" "$prepareimage" inject -k "$kms_blob_oemkey" -f "$kms_tmp" -p @AES_BLOB_KEY@ "$kms_config
    echo $command
    $command
    ret=$?
  fi
fi    

# Provide data for the blob encryption feature
if [ $ret -eq 0 ]; then
  kmsblob_aeskey=$SBSFUBootLoader"/2_Images_KMS_Blob/Binary/AES_DATA_STORAGE_key.bin"
  command="cat "$kms_config
  $command > $kms_tmp
  command=$cmd" "$prepareimage" inject -k "$kmsblob_aeskey" -f "$kms_tmp" -p @AES_DATA_STORAGE_KEY@ "$kms_config
  echo $command
  $command
  ret=$?
  if [ $ret -eq 0 ]; then
    kmsblob_aesdata=$SBSFUBootLoader"/2_Images_KMS_Blob/Binary/AES_DATA_STORAGE_data.bin"
    command="cat "$kms_config
    $command > $kms_tmp
    command=$cmd" "$prepareimage" inject -k "$kmsblob_aesdata" -f "$kms_tmp" -p @AES_DATA_STORAGE_DATA@ "$kms_config
    echo $command
    $command
    ret=$?
  fi
fi

# Patch AES keys part
if [ $ret -eq 0 ]; then
  type="vide"
  if [ $crypto = "SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM" ]; then
    type="GCM"
  fi  
  if [ $crypto = "SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256" ]; then
    type="CBC"
  fi
  if [ $crypto = "SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256" ]; then
# For KMS dummy AES keys are needed (reusing AES_CBC inputs files)
    type="CBC"
  fi
  if [ $type != "vide" ]; then
    oemkey="$1/../Binary/OEM_KEY_COMPANY1_key_AES_$type.bin"
    command="cat "$kms_config
    $command > $kms_tmp
    ret=$?
    if [ $? -eq 0 ]; then
      command=$cmd" "$prepareimage" inject -k "$oemkey" -f "$kms_tmp" -p @AES_SBSFU_KEY_1@ "$kms_config
      echo $command
      $command
      ret=$?
    fi

    oemkey="$1/../Binary/OEM_KEY_COMPANY2_key_AES_$type.bin"
    if [ -e "$oemkey" ]; then
      command="cat "$kms_config
      $command > $kms_tmp
      ret=$?
      if [ $? -eq 0 ]; then
        command=$cmd" "$prepareimage" inject -k "$oemkey" -f "$kms_tmp" -p @AES_SBSFU_KEY_2@ "$kms_config
        echo $command
        $command
        ret=$?
      fi
    fi

    oemkey="$1/../Binary/OEM_KEY_COMPANY3_key_AES_$type.bin"
    if [ -e "$oemkey" ]; then
      command="cat "$kms_config
      $command > $kms_tmp
      ret=$?
      if [ $? -eq 0 ]; then
        command=$cmd" "$prepareimage" inject -k "$oemkey" -f "$kms_tmp" -p @AES_SBSFU_KEY_3@ "$kms_config
        echo $command
        $command
        ret=$?
      fi
    fi
  fi
fi

# Patch ECC keys part
if [ $ret -eq 0 ]; then
  type="vide"
  if [ $crypto = "SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256" ]; then
    type="ECC"
  fi  
  if [ $crypto = "SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256" ]; then
    type="ECC"
  fi
  if [ $crypto = "SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM" ]; then
# For KMS dummy ECC keys are needed
    type="ECC"
  fi
  if [ $type != "vide" ]; then
    ecckey="$1/../Binary/ECCKEY1.txt"
    command="cat "$kms_config
    $command > $kms_tmp
    ret=$?
    if [ $? -eq 0 ]; then
      command=$cmd" "$prepareimage" inject -k "$ecckey" -f "$kms_tmp" -p @ECDSA_SBSFU_KEY_1@ "$kms_config
      echo $command
      $command
      ret=$?
    fi

    ecckey="$1/../Binary/ECCKEY2.txt"
    if [ -e "$ecckey" ]; then
      command="cat "$kms_config
      $command > $kms_tmp
      ret=$?
      if [ $? -eq 0 ]; then
        command=$cmd" "$prepareimage" inject -k "$ecckey" -f "$kms_tmp" -p @ECDSA_SBSFU_KEY_2@ "$kms_config
        echo $command
        $command
        ret=$?
      fi
    fi

    ecckey="$1/../Binary/ECCKEY3.txt"
    if [ -e "$ecckey" ]; then
      command="cat "$kms_config
      $command > $kms_tmp
      ret=$?
      if [ $? -eq 0 ]; then
        command=$cmd" "$prepareimage" inject -k "$ecckey" -f "$kms_tmp" -p @ECDSA_SBSFU_KEY_3@ "$kms_config
        echo $command
        $command
        ret=$?
      fi
    fi
  fi
fi

rm $kms_tmp

if [ $ret -eq 0 ]; then
#no error recopy post build script
    uname  | grep -i -e windows -e mingw > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "recopy postbuild.sh script with "$crypto".sh script"
        command="cat "$1"/"$crypto".sh"
        $command > "$1"/"postbuild.sh"
        ret=$?
    else
        echo "create symbolic link postbuild.sh to "$crypto".sh"
        command="ln -s ./"$crypto".sh "$1"/postbuild.sh"
        $command
        ret=$?
    fi
fi

if [ $ret != 0 ]; then
#an error
echo $command : failed >> $1/output.txt 
echo $command : failed
read -n 1 -s
exit 1
fi
exit 0

