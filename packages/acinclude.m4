dnl
dnl WARNING: Do not alter this file. It is overwritten automatically. 
dnl If you need to make changes to this file, apply the changes to the
dnl orocos-core.m4 file in the orocos-core package instead.
dnl

dnl Autoconf macros use the AC_ prefix, Automake macros use
dnl the AM_ prefix. Our extensions use the ACX_ prefix when
dnl they are general enough to be used outside of the project.
dnl The OROCOS_ prefix is used for Orocos specific macros.
dnl
dnl 

m4_define([DETECT_BOOSTPKG],[
AC_LANG_CPLUSPLUS
# Check for boost
AC_CHECK_HEADERS([boost/call_traits.hpp],
[
PACKAGES="support/boost/current/boost.cdl $PACKAGES"
],
[
AC_MSG_ERROR([

Could not find Boost headers. Please install Boost.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:
apt-get install libboost-dev
])
])
AC_LANG_C
])

m4_define([DETECT_BOOST_GRAPHPKG],[
AC_LANG_CPLUSPLUS
# Check for boost Graph
AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp],
[
PACKAGES="support/boost_graph/current/boost_graph.cdl $PACKAGES"
],
[
AC_MSG_ERROR([

Could not find the Boost Graph Library headers.
Please install Boost and Boost Graph.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:
apt-get install libboost-graph-dev
])
])
AC_LANG_C
])




m4_define([DETECT_XERCESCPKG],
[
AC_LANG_CPLUSPLUS

AC_CHECK_HEADERS([ xercesc/sax2/SAX2XMLReader.hpp ],
[
PACKAGES="support/xercesc/current/xercesc.cdl $PACKAGES"
],
[
  AC_MSG_WARN([No xerces installation found. XML parsing will be unavailable.])
  AC_LANG_C
])
])


m4_define([DETECT_COMEDIPKG],
[
AC_CHECK_HEADERS([ comedi.h ],
[
PACKAGES="support/comedi/current/comedi.cdl $PACKAGES"
],
[
  AC_MSG_WARN([No comedi installation found (comedi.h). Comedi will be unavailable.])
])
])

m4_define([DETECT_COMEDILIBPKG],
[
AC_CHECK_HEADERS([ comedilib.h ],
[
PACKAGES="support/comedilib/current/comedilib.cdl $PACKAGES"
],
[
  AC_MSG_WARN([No comedi-lib installation found (comedilib.h). Comedi-lib will be unavailable.])
])
])




dnl ACX_VERSION(MAJOR-VERSION,MINOR-VERSION,MICRO-VERSION,BUILD-NUMBER)
dnl Define the version number of the package
dnl Format: MAJOR.MINOR.MICRO-BUILD
dnl Written in M4 because AC_INIT arguments must be static but may
dnl use M4 processing.
m4_define([ACX_VERSION],[
 define([acx_major_version],[$1])
 define([acx_minor_version],[$2])
 define([acx_micro_version],[$3])
 define([acx_version],[acx_major_version.acx_minor_version.acx_micro_version])
]) # ACX_VERSION






dnl ACX_VERSION_POST()
dnl Post init of Autoconf version number stuff
m4_define([ACX_VERSION_POST],[
 MAJOR_VERSION=acx_major_version
 MINOR_VERSION=acx_minor_version
 MICRO_VERSION=acx_micro_version
 # XXX need to find the global SVN repos number here !
 BUILD=$(svn info . |grep Revision | sed -e's/Revision: //')
 DATE=`date +"%Y%m%d_%k%M"`
 VERSION=acx_version
 VERSION="$VERSION-$BUILD"
 PACKAGE_VERSION=$VERSION
 AC_SUBST(MAJOR_VERSION)
 AC_SUBST(MINOR_VERSION)
 AC_SUBST(MICRO_VERSION)
 AC_SUBST(VERSION)
 AC_SUBST(PACKAGE_VERSION)
 AC_SUBST(DATE)
 AC_SUBST(BUILD)
]) # ACX_VERSION_POST




dnl AC_PROG_DOXYGEN
dnl Check if doxygen is available.
m4_define([AC_PROG_DOXYGEN],[
AC_CHECK_PROG(DOXYGEN,doxygen,doxygen,no)
if test "x$DOXYGEN" = "xno"; then
DOXYGEN="@echo Not generating docs from source code"
AC_WARN([
Doxygen was not found on your system. Using doxygen you can generate
API documentation for the Orocos project.

You can download Doxygen from http://doxygen.sourceforge.net/
or if you are using Debian GNU/Linux just use: apt-get install doxygen
to install Doxygen.
])
fi
AC_SUBST(DOXYGEN)
])




dnl OROCOS_INIT(name,major,minor,micro)
m4_define([PACKAGES_INIT],[
# Define the version number of the package
# Format: major,minor,micro,build
ACX_VERSION($2,$3,$4)

# Check if Autoconf version is recent enough
AC_PREREQ(2.53)

# Include version control information
AC_REVISION($revision)

# Initialize Autoconf with package name and version
AC_INIT($1,acx_version)

ACX_VERSION_POST

# Tell Autoconf to dump files into the config subdir
#AC_CONFIG_AUX_DIR(config)
#AC_CONFIG_SRCDIR([config.h.in])
#AM_CONFIG_HEADER([config.h])

dnl Initialize Automake
#AM_INIT_AUTOMAKE(1.7.1)

dnl Default installation path
#AC_PREFIX_DEFAULT([/usr/local/orocos])
dnl Checks for programs.
#AC_PROG_AWK
#AC_PROG_INSTALL
#AC_PROG_RANLIB
#AC_PROG_XMLPROCESSOR
AC_PROG_DOXYGEN

])

m4_define([PACKAGES_OUTPUT_INFO],[
echo "
************************************************************
 Configuration:
   Version:                             ${VERSION}
   Generated Packages:                  ${PACKAGES}
************************************************************
"
echo "
DO NOT FORGET TO 'make db' to generate the ecos.db file. The configure script
made some pseudo packages which inform the ecos system what is 
installed on your system. You should only re-run configure if
you have installed new support software.
"
if test "x$DOXYGEN" = "xdoxygen"; then
echo -e "Run 'doxygen' to build only the API documentation.\n"
fi
])



m4_define([PACKAGES_OUTPUT],[
AC_OUTPUT
PACKAGES_OUTPUT_INFO
])

