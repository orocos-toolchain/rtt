# Process this file with autoconf to produce a configure script.

# Check if Autoconf version is recent enough
AC_PREREQ(2.53)

# Include version control information
#AC_REVISION($revision)

# Initialize Autoconf with package name and version
AC_INIT(orocos,0.23.0)
ACX_MY_ARGS="$*"
# Store the parameters to pass them to the packages/configure script.
#ACX_MY_ARGS="$@"

# Tell Autoconf to dump files into the config subdir
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

dnl Initialize Automake

AM_INIT_AUTOMAKE([gnu])

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
    exit 1
fi


# Set the default C flags and C++ flags (in that order)
OROCOS_DEFAULT_FLAGS(
  [],
  [])


PATH=$PATH:$(cd $(dirname $0);pwd)/tools/bin
AC_CHECK_PROG(ECOSCONFIG,ecosconfig,$(which ecosconfig),no)
if test "x$ECOSCONFIG" = "xno"; then
AC_MSG_ERROR("The program 'ecosconfig' not found on your system.
Orocos needs it to configure its packages properly. See the
Orocos.org download web page for information on how to obtain
ecosconfig ( and configtool ).
")
fi
AC_CHECK_PROG(CONFIGTOOL,configtool,$(which configtool),no)
if test "x$CONFIGTOOL" = "xno"; then
AC_MSG_ERROR("The program 'configtool' not found on your system.
Orocos needs it to configure its packages properly. See the
Orocos.org download web page for information on how to obtain
configtool ( and ecosconfig ).
")
fi


AC_MSG_CHECKING(for repository)
ECOS_REP=$(cd $(dirname $0); pwd)/packages
AC_SUBST(ECOS_REP)
AC_MSG_RESULT($ECOS_REP)

AC_MSG_CHECKING(for docdir)
DOC_DIR=$(cd $(dirname $0); pwd)/doc
AC_SUBST(DOC_DIR)
AC_MSG_RESULT($DOC_DIR)

AC_MSG_CHECKING(to build tests)
AC_ARG_ENABLE(tests,
     [  --enable-tests    Enable checking tests (default=yes) ],
     [ case "${enableval}" in
       yes) tests=yes ;;
       no)  tests=no ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-tests) ;;
     esac],[ tests=yes ])
AM_CONDITIONAL(DOTESTS, test x$tests = xyes)
AC_MSG_RESULT( $tests )

AC_MSG_CHECKING(to build profiling)
AC_ARG_ENABLE(profiling,
     [  --enable-profiling    Enable profiling tests (default=no) ],
     [ case "${enableval}" in
       yes) profs=yes ;;
       no)  profs=no ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-profiling) ;;
     esac],[ profs=no ])
AM_CONDITIONAL(DOPROFTESTS, test x$profs = xyes)
AC_MSG_RESULT( $profs )

# Setup the packages
echo "Running configure in packages..."
cd $ECOS_REP
# make sure configure is up-to-date
ORO_AUTOCONF=$(which autoconf)
if test x$ORO_AUTOCONF != x; then
echo "Running autogen..."
./autogen.sh
fi
echo "./configure $ACX_MY_ARGS"
# make distcheck hack :
chmod u+w . -R
./configure $ACX_MY_ARGS
echo "Running Make db in packages..."
make db
cd -

OROCOS_EXPORT_FLAGS

AC_CONFIG_FILES([ 
  Makefile 
  doc/Makefile
  doc/catalog.xml
  tests/Makefile
  Doxyfile
  packages/orocos.pc
  ])

OROCOS_OUTPUT

