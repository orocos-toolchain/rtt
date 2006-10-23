#!/bin/sh

if [ ! -d html ]; then
    mkdir html
fi
cd html

echo "<table>" > index.html

for i in `find ../../packages -name "*.cxx" | sort`; do
    objname=`find ../build-gcov -name ".*_$(basename $i cxx)o"`
    if [ x$objname = x ]; then
	#echo "$i not built..."
	continue;
    fi
    #echo "executing: gcov $i -o $objname"
    percent=`gcov -l $i -o $objname | grep ".cxx.gcov" -b1 | grep "%" | awk -F'%' {'print $1'} | awk -F':' {'print $2'}`
    name=`echo $i | awk -F'/' {'print $NF'}`
    basename=`echo $name | awk -F'.' {'print $1'}` 

    if [ "x$percent" = "x" ]; then
        echo "<tr>" >> index.html
        echo "<td bgcolor=#ffffff> $basename seems to be untested.<br>" >> index.html
        continue;   
    fi
    intpercent=`echo "$percent/1" | bc`
    if [ $intpercent -lt 33 ]; then
        color="#ffaaaa"
    else if [ $intpercent -lt 66 ]; then
        color="#ffff77"
        else
            color="#aaffaa"
        fi
    fi

    echo "<tr>" >> index.html
    echo "<td bgcolor=$color><a href=\"$name.gcov\">$basename</a><br>" >> index.html
    echo "<td bgcolor=$color>$percent% tested"  >> index.html
done

echo "</table><p>Last generated `date`</p>" >> index.html
