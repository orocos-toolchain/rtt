#!/bin/sh
# Check for a recent version of GNU Automake
AUTOMAKE_VERSION=1.7.1 #`automake --version|sed -n -e 's/automake (GNU automake) //p'`
AUTOMAKE_VERSION_MAJOR=`echo $AUTOMAKE_VERSION|cut -d. -f1`
AUTOMAKE_VERSION_MINOR=`echo $AUTOMAKE_VERSION|cut -d. -f2`
AUTOMAKE_VERSION_MINOR=`echo $AUTOMAKE_VERSION_MINOR|cut -d- -f1` # everything before -
AUTOMAKE_VERSION_MICRO=`echo $AUTOMAKE_VERSION|cut -d. -f3`
AUTOMAKE_VERSION_MICRO=0`echo $AUTOMAKE_VERSION_MICRO|cut -d- -f1` # remove -blah


let AUTOMAKE_VERSION_VALUE=${AUTOMAKE_VERSION_MAJOR}*100+${AUTOMAKE_VERSION_MINOR}*10+${AUTOMAKE_VERSION_MICRO}

if test $AUTOMAKE_VERSION_VALUE -lt 163 ; then
  echo "$0: automake $AUTOMAKE_VERSION detected, need 1.6.3 or newer"
  exit 1
fi

echo "$0: creating aclocal.m4"
aclocal
if test -d config; then
echo "$0: config directory exists";
else
echo "$0: creating config directory";
mkdir config;
fi
echo "$0: creating config.h.in"
autoheader
echo "$0: creating Makefile.in's"
automake -a
echo "$0: creating configurescript"
autoconf
