# Process this file with autoconf to produce a configure script.
PACKAGES_INIT([Orocos Packages],0,18,0)

if ! test -e configure.ac; then
   echo "
   You should only invoke this configure script from within
   the original packages directory, where it is generated.
   It will create packages for you in-place.
"
fi

AC_CHECK_PROGS(PYTHON, python python2.2,no)
if test $PYTHON = no; then
 AC_MSG_ERROR([
The Packaging system requires at least Python 2.2 
])
fi;

AC_CHECK_PROGS(TCL, tclsh tclsh8.3,no)
if test $TCL = no; then
AC_MSG_ERROR([
The Packaging system requires tcl 8.3 
])
fi;

# the support top directory
PACKAGES=support/common/current/support.cdl

DETECT_BOOSTPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_COMEDILIBPKG
DETECT_READLINE
DETECT_RTAI

AC_CONFIG_FILES([ 
  Doxyfile Makefile
  $PACKAGES ])


PACKAGES_OUTPUT