#!/bin/bash

echo "This scripts creates the debian/control and debian/*.install files from several template files."

targets="gnulinux lxrt xenomai"

# Prepare control file:
rm -f control
cp control.common control

for t in $targets; do 
    # append control-template.in to control file
    if test $t = xenomai; then tdev=", xenomai-dev | libxenomai-dev"; fi
    cat control-template.in | sed -e"s/@TARGET@/$t/g;s/@TARGET-DEV@/$tdev/g" >> control
    cat control-$t.in >> control
done

# Prepare *.install files:

for i in $(ls *template*install); do
    for t in $targets; do
	# Replace contents and write to *-target.install files:
	cat $i | sed -e"s/@TARGET@/$t/g" > $(echo "$i" | sed -e"s/template/$t/g")
    done
done
