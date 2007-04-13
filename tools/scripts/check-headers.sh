#!/bin/bash

CC=gcc
path=/usr/local/include

echo "Checking files in $path..."
cd $path
tname=$(tempfile -p check -s .cpp)
oname=/tmp/$(basename $tname cpp)s
for i in $(find rtt -maxdepth 2 -type f -name "*.h" -o -name "*.hpp"); do 
    echo  "Checking $i..." 
    echo -e "#include <$i>\n" > $tname 
    ${CC} -pipe -Wall -S $tname -I. -o $oname
done 
rm -f $tname $oname
