# Process this file with autoconf to produce a configure script.
PACKAGES_INIT([Orocos Packages],0,14,0)

if ! test -e configure.ac; then
   echo "
   You should only invoke this configure script from within
   the original packages directory, where it is generated.
   It will create packages for you in-place.
"
fi

AC_CHECK_PROGS(PYTHON, python python1.5,no)
if test $PYTHON = no; then
 AC_MSG_ERROR([
The Packaging system requires Python 2.3 
])
fi;

AC_CHECK_PROGS(TCL, tclsh tclsh8.3,no)
if test $TCL = no; then
AC_MSG_ERROR([
The Packaging system requires tcl 8.3 
])
fi;

DETECT_BOOSTPKG
DETECT_BOOST_GRAPHPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_COMEDILIBPKG

AC_CONFIG_FILES([ 
  Doxyfile Makefile
  $PACKAGES ])


PACKAGES_OUTPUT