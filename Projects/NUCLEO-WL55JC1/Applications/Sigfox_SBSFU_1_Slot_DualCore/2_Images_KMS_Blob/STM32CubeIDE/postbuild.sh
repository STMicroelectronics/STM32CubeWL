#!/bin/bash - 
#Post build for SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256
# arg1 is the build directory
# arg2 is the bin file path+name without extension
# arg3 is the version

projectdir=$1
execname=$2
version=$3

userAppBinary=$projectdir"/../Binary"
bin=$execname".bin"

kmsu=$userAppBinary"/"$execname".kmsu"
kmsb=$userAppBinary"/"$execname".kmsb"
sign=$userAppBinary"/"$execname".sign"

iv=$userAppBinary"/iv.bin"
oemkey=$userAppBinary"/OEM_KEY_COMPANY1_key_AES_CBC.bin"
ecckey=$userAppBinary"/ECCKEY.txt"

current_directory=`pwd`
cd $1/../../../../../../Middlewares/ST/STM32_Secure_Engine/Utilities/KeysAndImages
basedir=`pwd`
cd $current_directory
# test if window executable usable
prepareimage=$basedir"/win/prepareimage/prepareimage.exe"
uname | grep -i -e windows -e mingw

if [ $? == 0 ] && [   -e "$prepareimage" ]; then
  echo "prepareimage with windows executable"
  cmd=""
else
  # line for python
  echo "prepareimage with python script"
  prepareimage=$basedir/prepareimage.py
  cmd="python"
fi

echo "$cmd $prepareimage" > $1/output.txt
# Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $userAppBinary ]; then
mkdir $userAppBinary
fi

command=$cmd" "$prepareimage" enc -k "$oemkey" -i "$iv" "$bin" "$kmsu
echo $command
$command >> "$projectdir"/output.txt
ret=$?
if [ $ret == 0 ]; then
# -p 16 indicates that the input file must be a multiple of the AES block size (16 bytes). Padding is done before computing the sha256 if needed.
  command=$cmd" "$prepareimage" sha256 "$bin" "$sign" -p 16"
  echo $command
  $command >> $projectdir"/output.txt"
  ret=$?
  if [ $ret == 0 ]; then 
    command=$cmd" "$prepareimage" pack -k "$ecckey" -m "KMSB" -r 28 -v "$version" -i "$iv" -f "$kmsu" -t "$sign" -o 320 "$kmsb
    echo $command
    $command >> $projectdir"/output.txt"
    ret=$?
  fi
fi

if [ $ret == 0 ]; then
  rm $sign
  rm $kmsu
  exit 0
else 
  echo "$command : failed" >> $projectdir"/output.txt"
  echo $command : failed
  read -n 1 -s
  exit 1
fi