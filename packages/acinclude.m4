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
ORO_BOOST_DETECT=1
PACKAGES="support/boost/current/boost.cdl $PACKAGES"
],
[
ORO_BOOST_DETECT=0
AC_MSG_WARN([

Could not find Boost headers. Please install Boost.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:

apt-get install libboost-dev

and rerun the configure script
])
])
# Check for boost Graph
AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp],
[
   ORO_BOOST_GRAPH_DETECT=1
],
[
   ORO_BOOST_GRAPH_DETECT=0
AC_MSG_WARN([

Could not find the Boost Graph Library headers.
Please install Boost and Boost Graph.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:

apt-get install libboost-graph-dev

and rerun the configure script
])
])
# Check for boost Signal
AC_CHECK_HEADERS([boost/signal.hpp],
[
   ORO_BOOST_SIGNALS_DETECT=1
],
[
   ORO_BOOST_SIGNALS_DETECT=0
AC_MSG_WARN([

Could not find the Boost Signal Library headers.
Please install Boost and Boost Signal.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:

apt-get install libboost-signals-dev

and rerun the configure script
])
])
# Check for boost Spirit
AC_CHECK_HEADERS([boost/spirit.hpp],
[
   ORO_BOOST_PARSER_DETECT=1
],
[
   ORO_BOOST_PARSER_DETECT=0
AC_MSG_WARN([

Could not find the Boost Spirit Library headers.
Please install Boost and Boost Spirit.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:

apt-get install libboost-dev

and rerun the configure script
])
])
AC_LANG_C

AC_SUBST(ORO_BOOST_DETECT)
AC_SUBST(ORO_BOOST_GRAPH_DETECT)
AC_SUBST(ORO_BOOST_SIGNALS_DETECT)
AC_SUBST(ORO_BOOST_PARSER_DETECT)
])




m4_define([DETECT_XERCESCPKG],
[
AC_LANG_CPLUSPLUS

AC_CHECK_HEADERS([ xercesc/sax2/SAX2XMLReader.hpp ],
[
PACKAGES="support/xercesc/current/xercesc.cdl $PACKAGES"
],
[
  AC_MSG_WARN([
No Xerces-C 2.1 installation found.
XML parsing will be unavailable.

To install Xerces-C, Debian users can do :
apt-get install libxerces21-dev 

and rerun the bootstrap.sh script
])
  AC_LANG_C
])
])


m4_define([DETECT_COMEDIPKG],
[
AC_MSG_CHECKING(for comedi dir)
AC_ARG_WITH(comedi, [ --with-comedi Specify location of linux/comedi.h ],
	[ if test x"$withval" != x; then COMEDI_DIR="$withval"; fi ])
if test x"$COMEDI_DIR" = x; then
   COMEDI_DIR="/usr/realtime/include"
fi
AC_MSG_RESULT($COMEDI_DIR)
AC_SUBST(COMEDI_DIR)

CPPFLAGS="-I$COMEDI_DIR"
AC_CHECK_HEADERS([ linux/comedi.h ],
[
PACKAGES="support/comedi/current/comedi.cdl $PACKAGES"
],
[
  AC_MSG_WARN([No comedi installation found (comedi.h). Comedi will be unavailable.])
])
])

m4_define([DETECT_COMEDILIBPKG],
[
AC_MSG_CHECKING(for comedilib dir)
AC_ARG_WITH(comedilib, [ --with-comedilib Specify location of comedilib.h ],
	[ if test x"$withval" != x; then COMEDILIB_DIR="$withval"; fi ])
if test x"$COMEDILIB_DIR" = x; then
   COMEDILIB_DIR="/usr/realtime/include"
fi
AC_MSG_RESULT($COMEDILIB_DIR)
AC_SUBST(COMEDILIB_DIR)

CPPFLAGS="-I$COMEDILIB_DIR"
AC_CHECK_HEADERS([ comedilib.h],
[
PACKAGES="support/comedilib/current/comedilib.cdl $PACKAGES"
],
[
  AC_MSG_WARN([No comedi-lib installation found (comedilib.h). Comedi-lib will be unavailable.])
])
])

m4_define([DETECT_READLINE],
[
AC_CHECK_HEADERS([ readline/readline.h ],
[
PACKAGES="support/readline/current/readline.cdl $PACKAGES"
],
[
  AC_MSG_WARN([
No readline installation found (readline/readline.h). 
Readline will be unavailable.

To install GNU Readline, Debian users can do :
apt-get install libreadline4-dev 

and rerun the bootstrap.sh script
])
])
])

m4_define([DETECT_RTAI],
[
AC_MSG_CHECKING(for RTAI/LXRT Installation)
AC_ARG_WITH(rtai, [ --with-rtai[=/usr/realtime] Specify location of RTAI/LXRT ],
	[ if test x"$withval" != x; then RTAI_DIR="$withval"; fi ])
AC_ARG_WITH(lxrt, [ --with-lxrt[=/usr/realtime] Equivalent to --with-rtai ],
	[ if test x"$withval" != x; then RTAI_DIR="$withval"; fi ])

if test x"$RTAI_DIR" = x; then
   RTAI_DIR="/usr/realtime"
fi
AC_MSG_RESULT($RTAI_DIR)
AC_SUBST(RTAI_DIR)

CPPFLAGS="-I$RTAI_DIR/include"
AC_CHECK_HEADER([rtai_config.h], [
  AC_CHECK_HEADER([rtai_lxrt.h],
  [
    PACKAGES="support/rtai/current/rtai.cdl $PACKAGES"
    RTAI_VERSION=3
  ],
  [
    AC_MSG_WARN([No RTAI/LXRT installation found (rtai_config.h, rtai_lxrt.h). LXRT will be unavailable.])
  ])
],[
dnl try old rtai style
  AC_CHECK_HEADER([rtai_lxrt_user.h], 
  [
    PACKAGES="support/rtai/current/rtai.cdl $PACKAGES"
    RTAI_VERSION=2
  ],
  [
    AC_MSG_WARN([No RTAI/LXRT installation found ( rtai_lxrt_user.h ). LXRT will be unavailable.])
  ])
])
if test x"$RTAI_VERSION" = x; then
   RTAI_VERSION=0
fi
AC_SUBST(RTAI_VERSION)
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

 SVN=$(which svn)

 AC_MSG_CHECKING(for subversion)
 AC_ARG_ENABLE(subversion,
     [  --enable-subversion    Add a sub-version number (default=no) ],
     [ case "${enableval}" in
       yes) subvsn=yes ;;
       no)  subvsn=no ;;
       *) AC_MSG_ERROR(bad value ${subvsn} for --enable-subversion) ;;
     esac],[ subvsn=no ])

 if test $subvsn = yes -a x$SVN != x; then

 echo "{ print $""1  }" > print-svn.awk
 SVN_VERSION=$(svn log http://cvs.mech.kuleuven.ac.be/svn/orocos/orocos-trunk 2>/dev/null \
	| head -2 |tail -1 | awk -f print-svn.awk )
 fi;
 rm -f print-svn.awk

 if test x$SVN != x; then
    if test $subvsn = yes; then
	 BUILD=-$SVN_VERSION
	 AC_MSG_RESULT( yes -$SVN_VERSION )
    else
	 AC_MSG_RESULT( yes )
    fi;         
 else
	 AC_MSG_RESULT( no )
 fi;

 DATE=`date +"%Y%m%d_%k%M"`
 VERSION=acx_version
 VERSION="$VERSION$BUILD"
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
The configure script
made some pseudo packages which inform the ecos system what is 
installed on your system. You should only re-run the bootstrap.sh script if
you have installed new libraries.
"
if test "x$DOXYGEN" = "xdoxygen"; then
echo -e "Run 'doxygen' to build only the API documentation.\n"
fi
])



m4_define([PACKAGES_OUTPUT],[
AC_OUTPUT
PACKAGES_OUTPUT_INFO
])

