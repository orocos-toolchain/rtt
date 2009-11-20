#!/bin/bash

CC=g++
target=gnulinux

echo "Run this script from your build directory. It checks the headers for the $target target. Modify this file to change the target."

if [ "x$*" == "x" ]; then
    list=$(find rtt/ -maxdepth 1 -type f -name "*.h" -o -name "*.hpp");
    corbalist=$(find rtt/transports -maxdepth 1 -type f -name "*.h" -o -name "*.hpp");
else
    list=$*;
fi

tname=$(tempfile -p check -s .cpp)
oname=/tmp/$(basename $tname cpp)s
ln -s ../src rtt
for i in $list $corbalist; do
    echo  "Checking $i..." 
    echo -e "#include <$i>\n" > $tname 
    ${CC} -pipe -Wall -S $tname -I. -I rtt -I src -I src/os -I src/os/$target -I src/transports/corba -o $oname -DOROCOS_TARGET=$target || exit 1
done 
rm -f $tname $oname rtt

