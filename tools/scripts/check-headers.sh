#!/bin/bash

CC=g++

echo "Run this script from your build directory."

tname=$(tempfile -p check -s .cpp)
oname=/tmp/$(basename $tname cpp)s
for i in $(find rtt/ -maxdepth 1 -type f -name "*.h" -o -name "*.hpp"); do 
    echo  "Checking $i..." 
    echo -e "#include <$i>\n" > $tname 
    ${CC} -pipe -Wall -S $tname -I. -I rtt -I src -I src/os -I src/os/gnulinux -o $oname -DOROCOS_TARGET=gnulinux
done 
rm -f $tname $oname
