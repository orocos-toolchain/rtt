# Process this file with autoconf to produce a configure script.

# This macro executes the Orocos common Autoconf code
# Specify the name and version number as a parameter
OROCOS_GENERAL(orocos,0,11,0,cvs)

if test -e configure.ac; then
    AC_ERROR([
    You should not configure and compile in the source tree.
    Instead, do the following :
    mkdir build
    cd build
    ../configure
    make
    
    For more information on options, use ./configure --help])
fi

# Set the default C flags and C++ flags (in that order)
OROCOS_DEFAULT_FLAGS(
  [-pipe -Wall -Wstrict-prototypes -O2],
  [-pipe -Wall -Wstrict-prototypes -O2])


OROCOS_EXPORT_FLAGS

AC_CONFIG_FILES([ 
  Doxyfile Makefile 
  doc/Makefile
  src/Makefile 
  ])


OROCOS_OUTPUT
