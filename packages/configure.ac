# Process this file with autoconf to produce a configure script.

if ! test -e configure.ac; then
   echo "
   You should only invoke this configure script from within
   the original packages directory, where it is generated.
   It will create packages for you in-place.
"


PACKAGES_INIT("Orocos Packages",0,10,0)

DETECT_BOOSTPKG
DETECT_BOOST_GRAPHPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_COMEDILIBPKG

AC_CONFIG_FILES([ 
  Doxyfile Makefile
  $PACKAGES ])


PACKAGES_OUTPUT