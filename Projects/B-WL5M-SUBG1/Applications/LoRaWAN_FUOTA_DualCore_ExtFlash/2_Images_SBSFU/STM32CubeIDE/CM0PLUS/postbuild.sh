#!/bin/bash -
echo "Extract SE interface symbols"
arm-none-eabi-nm $1 > nm.txt
case "$(uname -s)" in
    Linux*|Darwin*)
      tr -d '\015' <../se_interface.txt >../se_interface_unix.txt
      grep -F -f ../se_interface_unix.txt nm.txt > symbol.list
      rm ../se_interface_unix.txt
      ;;
    *)
      grep -F -f ../se_interface.txt nm.txt > symbol.list
      ;;
esac
wc -l symbol.list
cat symbol.list | awk '{split($0,a,/[ \r]/); print a[3]" = 0x"a[1]";"}' > se_interface_app.ld
rm nm.txt
rm symbol.list
