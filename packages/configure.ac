# Process this file with autoconf to produce a configure script.
PACKAGES_INIT([Orocos Packages],0,20,0)

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

# AC_MSG_CHECKING(to build profiling)
# AC_ARG_ENABLE(profiling,
#      [  --enable-profiling    Enable profiling tests (default=no) ],
#      [ case "${enableval}" in
#        yes) profs=yes ;;
#        no)  profs=no ;;
#        *) AC_MSG_ERROR(bad value ${enableval} for --enable-profiling) ;;
#      esac],[ profs=no ])
# AM_CONDITIONAL(DOPROFTESTS, test x$profs = xyes)
# AC_MSG_RESULT( $profs )

# the support top directory
PACKAGES=support/common/current/support.cdl

DETECT_BOOSTPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_READLINE
DETECT_RTAI

AC_CONFIG_FILES([ 
  Makefile
  $PACKAGES ])


PACKAGES_OUTPUT