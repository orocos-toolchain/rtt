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

DETECT_CORBAPKG
DETECT_BOOSTPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_READLINE
DETECT_RTAI
#DETECT_GCC
#if I put this in a macro, the $1 sign disapears, and I could not find out
# how to properly escape it.
AC_MSG_CHECKING(for GCC version)
ORO_GCC_VERSION=gcc$($CC -dumpversion | sed "s/\./ /g" |awk '{ print $1 }')
AC_SUBST(ORO_GCC_VERSION)

PACKAGES="support/gcc/current/gcc.cdl $PACKAGES"
AC_MSG_RESULT($ORO_GCC_VERSION)


AC_CONFIG_FILES([ 
  Makefile
  $PACKAGES ])


PACKAGES_OUTPUT