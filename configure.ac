# Process this file with autoconf to produce a configure script.

# Check if Autoconf version is recent enough
AC_PREREQ(2.53)

# Include version control information
#AC_REVISION($revision)

# Initialize Autoconf with package name and version
AC_INIT(orocos-base,0.16.0)
ACX_MY_ARGS="$*"

# Tell Autoconf to dump files into the config subdir
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

dnl Initialize Automake
AM_INIT_AUTOMAKE(1.6.0)

OROCOS_GENERAL(AC_PACKAGE_NAME,AC_PACKAGE_VERSION)

if test -e configure.ac; then
    echo "
    You should not configure and compile in the source tree.
    Instead, do the following :
    mkdir build
    cd build
    ../configure
    make
    
    For more information on options, use ./configure --help"
#    exit 1
fi


# Set the default C flags and C++ flags (in that order)
OROCOS_DEFAULT_FLAGS(
  [-pipe -Wall -Wstrict-prototypes -O2],
  [-pipe -Wall -Wstrict-prototypes -O2])


OROCOS_EXPORT_FLAGS

AC_MSG_CHECKING(for repository)
ECOS_REP=$(cd $(dirname $0); pwd)/packages
AC_SUBST(ECOS_REP)
AC_MSG_RESULT($ECOS_REP)

AC_MSG_CHECKING(for docdir)
DOC_DIR=$(cd $(dirname $0); pwd)/doc
AC_SUBST(DOC_DIR)
AC_MSG_RESULT($DOC_DIR)

# Setup the packages
echo "Running configure in packages..."
cd $ECOS_REP
echo "./configure $ACX_MY_ARGS"
./configure $ACX_MY_ARGS
echo "Running Make db in packages..."
make db
cd -


AC_CONFIG_FILES([ 
  Doxyfile Makefile 
  doc/Makefile
  doc/orocos-manual.xml
  tests/Makefile
  ])

OROCOS_OUTPUT

