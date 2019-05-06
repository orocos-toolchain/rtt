#!/bin/bash

if [ "$1" = "" ]; then
    echo Generate Free Software Header
    echo Usage: $0 \"Filename
    exit 1
fi

filename=`basename $1`
author="`git log --follow --format="%an" -- $1 | tail -n 1`"
echo $author
email="`git log --follow --format="%ae" -- $1 | tail -n 1`"
echo $email
date="`git log --follow --format="%ad" -- $1 | tail -n 1`"
echo $date
begin_date="`git log --follow --format="%ad" -- $1 | tail -n 1 | cut -d" " -f 1,2,3,5`"
echo $begin_date
year="`git log --follow --format="%ad" -- $1 | tail -n 1 | cut -d" " -f 5`"
echo $year

export "LANG=C"

echo "/***************************************************************************" > $1.tmp.header
echo -e "  tag: $author $date $filename\n" >> $1.tmp.header
echo "                        $filename -  description" >> $1.tmp.header
echo "                           -------------------" >> $1.tmp.header
echo "    begin                : $begin_date" >> $1.tmp.header
echo "    copyright            : (C) $year $author" >> $1.tmp.header
echo "    email                : $email" >> $1.tmp.header
echo "" >> $1.tmp.header
echo " ***************************************************************************" >> $1.tmp.header
echo " *   This library is free software; you can redistribute it and/or         *" >> $1.tmp.header
echo " *   modify it under the terms of the GNU General Public                   *" >> $1.tmp.header
echo " *   License as published by the Free Software Foundation;                 *" >> $1.tmp.header
echo " *   version 2 of the License.                                             *" >> $1.tmp.header
echo " *                                                                         *" >> $1.tmp.header
echo " *   As a special exception, you may use this file as part of a free       *" >> $1.tmp.header
echo " *   software library without restriction.  Specifically, if other files   *" >> $1.tmp.header
echo " *   instantiate templates or use macros or inline functions from this     *" >> $1.tmp.header
echo " *   file, or you compile this file and link it with other files to        *" >> $1.tmp.header
echo " *   produce an executable, this file does not by itself cause the         *" >> $1.tmp.header
echo " *   resulting executable to be covered by the GNU General Public          *" >> $1.tmp.header
echo " *   License.  This exception does not however invalidate any other        *" >> $1.tmp.header
echo " *   reasons why the executable file might be covered by the GNU General   *" >> $1.tmp.header
echo " *   Public License.                                                       *" >> $1.tmp.header
echo " *                                                                         *" >> $1.tmp.header
echo " *   This library is distributed in the hope that it will be useful,       *" >> $1.tmp.header
echo " *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *" >> $1.tmp.header
echo " *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *" >> $1.tmp.header
echo " *   General Public License for more details.                              *" >> $1.tmp.header
echo " *                                                                         *" >> $1.tmp.header
echo " *   You should have received a copy of the GNU General Public             *" >> $1.tmp.header
echo " *   License along with this library; if not, write to the Free Software   *" >> $1.tmp.header
echo " *   Foundation, Inc., 59 Temple Place,                                    *" >> $1.tmp.header
echo " *   Suite 330, Boston, MA  02111-1307  USA                                *" >> $1.tmp.header
echo " *                                                                         *" >> $1.tmp.header
echo " ***************************************************************************/" >> $1.tmp.header
echo "" >> $1.tmp.header
echo "" >> $1.tmp.header

cat $1.tmp.header $1 > $1.tmp.full
mv $1.tmp.full $1
rm $1.tmp.header
