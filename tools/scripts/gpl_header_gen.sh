#!/bin/bash

if [ "$1" = "" -o "$2" = "" -o "$3" = "" ]; then
    echo Generate Free Software Header
    echo Usage: $0 \"Copyright holder\" EMail-Address Filename
    exit 1
fi

filename=`basename $3`

export "LANG=C"

echo "/***************************************************************************" > $3.tmp.header
echo -e "  tag: $1 " `date` " $filename\n" >> $3.tmp.header
echo "                        $filename -  description" >> $3.tmp.header
echo "                           -------------------" >> $3.tmp.header
echo "    begin                : "`date "+%a %B %d %Y"` >> $3.tmp.header
echo "    copyright            : (C) "`date +%Y` $1 >> $3.tmp.header
#echo "    copyright            : (C) "`date +%Y` `cat /etc/passwd | ( IFS=: ; while read lognam pw id gp fname home sh; do echo $home \"$fname\"; done ) | /bin/grep "/home/$USER" | sed "s/\/home\/$USER\ \"//" | sed "s/,,,\"//"` >> $3.tmp.header
echo "    email                : "$2 >> $3.tmp.header
echo "" >> $3.tmp.header
echo " ***************************************************************************" >> $3.tmp.header
echo " *                                                                         *" >> $3.tmp.header
echo " *   This program is free software; you can redistribute it and/or modify  *" >> $3.tmp.header
echo " *   it under the terms of the GNU General Public License as published by  *" >> $3.tmp.header
echo " *   the Free Software Foundation; either version 2 of the License, or     *" >> $3.tmp.header
echo " *   (at your option) any later version.                                   *" >> $3.tmp.header
echo " *                                                                         *" >> $3.tmp.header
echo " ***************************************************************************/" >> $3.tmp.header
echo "" >> $3.tmp.header
echo "" >> $3.tmp.header

cat $3.tmp.header $3 > $3.tmp.full
mv $3.tmp.full $3
rm $3.tmp.header
