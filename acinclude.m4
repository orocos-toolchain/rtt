dnl Autoconf macros use the AC_ prefix, Automake macros use
dnl the AM_ prefix. Our extensions use the ACX_ prefix when
dnl they are general enough to be used outside of the project.
dnl The OROCOS_ prefix is used for Orocos specific macros.
dnl
dnl 

m4_define([OROCOS_BOOSTPKG],[
AC_LANG_CPLUSPLUS
# Check for boost
AC_CHECK_HEADERS([boost/call_traits.hpp],
[],
[
AC_MSG_ERROR([

Could not find Boost headers. Please install Boost.

You can find Boost at http://www.boost.org/
or if you are a Debian GNU/Linux user, just do:

apt-get install libboost-dev libboost-graph-dev

RedHat and others may call these boost-devel or libboost-devel.

and be sure to rerun bootstrap.sh in the main directory,
so that the package management system can detect the installation.
])
])
AC_LANG_C
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
 DATE=`date +"%Y%m%d_%k%M"`
dnl BUILD=$(svn info packages |grep Revision | sed -e's/Revision: //')
dnl VERSION=acx_version-$BUILD
 VERSION=acx_version
 PACKAGE_VERSION=$VERSION
 AC_SUBST(MAJOR_VERSION)
 AC_SUBST(MINOR_VERSION)
 AC_SUBST(MICRO_VERSION)
 AC_SUBST(VERSION)
 AC_SUBST(PACKAGE_VERSION)
 AC_SUBST(DATE)
 AC_SUBST(BUILD)
]) # ACX_VERSION_POST





dnl ACX_ARG_DEBUG
dnl Flag to choose to enable debugging information
m4_define([ACX_ARG_DEBUG],[
 AC_ARG_ENABLE(debug,
  [AC_HELP_STRING([--enable-debug],[Build debugging information])],
  [ DEBUG=1
  ],
  [ DEBUG=0]
 )
 if test x$DEBUG = x1; then
  #ACX_CXXFLAGS="$ACX_CXXFLAGS -g -O0 ";
  #ACX_CFLAGS="$ACX_CFLAGS -g -O0 ";
  #i did this to get the order -O2 -O0 right
  CFLAGS="$CFLAGS -g -O0"
  CXXFLAGS="$CXXFLAGS -g -O0"
 fi
]) # ACX_ARG_DEBUG





dnl AC_PROG_INSURE
dnl Flag to choose to use Insure++
m4_define([AC_PROG_INSURE],[
# Insure++ is a proriatary code checker/prober
AC_ARG_ENABLE(insure,
 [AC_HELP_STRING([--enable-insure],[Compile with Insure++])],
 [
  AC_CHECK_PROG(insure,insure,insure,no)
  if test "x$insure" = "xinsure"; then
   if test -z $CXX; then
    CXX=insure
   fi;
   if test -z $CC; then
    CC=insure
   fi;
  fi;
 ]
)
]) # AC_PROG_INSURE






dnl AC_PROG_COLORGCC
dnl Check if colorgcc is available and if so,
dnl if use hasn't specified a particular compiler,
dnl use colorgcc.
m4_define([AC_PROG_COLORGCC],[
AC_CHECK_PROG(colorgcc,colorgcc,colorgcc,no)
if test "x$colorgcc" = "xcolorgcc"; then
 if test -z $CXX; then
  CXX=colorgcc
 fi;
 if test -z $CC; then
  CC=colorgcc
 fi;
fi;
])


dnl AC_PROG_DOXYGEN
dnl Check if doxygen is available.
m4_define([AC_PROG_DOXYGEN],[
AC_CHECK_PROG(DOXYGEN,doxygen,doxygen,no)
if test "x$DOXYGEN" = "xno"; then
	DOXYGEN="echo Not generating docs from source code"
fi
AC_SUBST(DOXYGEN)
])

dnl AC_PROG_DOT
dnl Check if dot is available.
m4_define([AC_PROG_DOT],[
AC_CHECK_PROG(ORODOT,dot,YES,NO)
AC_SUBST(ORODOT)
])





dnl AC_PROG_XMLPROCESSOR
dnl Check if xsltproc is available.
m4_define([AC_PROG_XMLPROCESSOR],[
AC_CHECK_PROG(xml,xsltproc,xsltproc,no)
AM_CONDITIONAL(HAVE_XMLPROCESSOR, test "x$xml" = "xxsltproc")
if test "x$xml" = "xno"; then
XMLPROCESSOR="echo"
else
XMLPROCESSOR=$xml
fi;
AC_SUBST(XMLPROCESSOR)
])

dnl AC_PROG_DIA
dnl Check if Dia is available.
m4_define([AC_PROG_DIA],[
AC_CHECK_PROG(DIA,dia,dia,no)
dnl if test "x$DIA" = "xno"; then
dnl DIA="echo Not generating documentation figures"
# AC_MSG_WARN([
# Dia was not found on your system. Using Dia you can generate
# figures for the documentation for the Orocos project.

# You can download Dia from http://www.lysator.liu.se/~alla/dia/ 
# or if you are using Debian GNU/Linux just use: apt-get install dia
# to install Dia.
# ])
dnl fi
dnl AC_MSG_CHECKING( Dia location)
dnl DIA_LOCATION=`which dia`
dnl AC_MSG_RESULT($DIA_LOCATION)

dnl AC_MSG_CHECKING(Dia edition)
dnl DIA_VERSION=`strings $DIA_LOCATION|awk '/Dia-Version:/ {print $2}'`
dnl DIA_VERSION=`echo $DIA_VERSION|cut -d: -f2`
dnl AC_MSG_RESULT($DIA_VERSION)

dnl AC_MSG_CHECKING(Dia's freshness)
dnl DIA_VERSION_MAJOR=`echo $DIA_VERSION|cut -d. -f1`
dnl DIA_VERSION_MINOR=`echo $DIA_VERSION|cut -d. -f2`
dnl let DIA_VERSION_VALUE=${DIA_VERSION_MAJOR}*100+${DIA_VERSION_MINOR}
dnl echo $DIA_VERSION_VALUE
dnl if test $DIA_VERSION_VALUE -lt 90; then
dnl  echo "$0: Dia $DIA_VERSION detected, need 0.90 or newer";
# AC_MSG_RESULT(not ok)
# AC_MSG_WARN([
# Dia found on your system is too old. Using Dia you can generate
# figures for the documentation for the Orocos project.

# You can download Dia from http://www.lysator.liu.se/~alla/dia/ 
# or if you are using Debian GNU/Linux just use: apt-get install dia
# to install Dia.
# ])
dnl DIA="echo Not generating documentation figures"
dnl else
dnl AC_MSG_RESULT(ok)
dnl fi;
AC_SUBST(DIA)
])

dnl AC_PROG_FOP
dnl Check if Fop is available.
m4_define([AC_PROG_FOP],[
AC_CHECK_PROG(FOP,fop,fop,no)
if test "x$FOP" = "xno"; then
FOP="echo Not generating PDF documentation"
# AC_MSG_WARN([
# FOP was not found on your system. Using FOP you can generate
# a PDF-file containing the Orocos project documentation.

# FOP is a print formatter driven by XSL formatting objects.

# If you are using Debian GNU/Linux just use: apt-get install libfop-java
# to install FOP.
# ])
fi
AC_SUBST(FOP)
])


dnl ACX_CHECK_DOCBOOK
dnl Check if XSL is available.
m4_define([ACX_DOCBOOK],[
DOCBOOK_XSL_PATH=/usr/share/sgml/docbook/stylesheet/xsl/nwalsh/html/docbook.xsl

AM_CONDITIONAL(HAVE_XMLPROCESSOR, test "x$xml" = "xxsltproc")
XMLPROCESSOR=$xml
AC_SUBST(XMLPROCESSOR)
])





m4_define([ACX_LIB_GCC],[
AC_MSG_CHECKING(for GCC library)
GCCLIB=$(dirname $($CC --print-file-name=libgcc.a) )
AC_SUBST(GCCLIB)
AC_MSG_RESULT($GCCLIB)
])





m4_define([ACX_LIB_STDCXX],[
AC_MSG_CHECKING(for Standard C++ library)
STDCXXLIB=`$CXX -print-file-name=libstdc++.a`
AC_SUBST(STDCXXLIB)
AC_MSG_RESULT($STDCXXLIB)
])






dnl ======================= Preprocessor Flags ===========================

dnl removed by ps







dnl =================== Start of Autoconf Orocos extension macros ====================

dnl OROCOS_INIT(name,major,minor)
m4_define([OROCOS_INIT],[
# Define the version number of the package
# Format: major,minor,micro,build

# use packages version number as build number,
# Since svn uses global version numbers this is actually cool !
#TMPBUILD = $(svn info packages |grep Revision | sed -e's/Revision: //')
# define([svn_version],[$TMPBUILD])

#ACX_VERSION($2,$3,$4,svn_version)

# # Check if Autoconf version is recent enough
# AC_PREREQ(2.53)

# # Include version control information
# AC_REVISION($revision)

# # Initialize Autoconf with package name and version
# AC_INIT($1,acx_version)

# # Tell Autoconf to dump files into the config subdir
# AC_CONFIG_AUX_DIR(config)
# AC_CONFIG_SRCDIR([config.h.in])
# AM_CONFIG_HEADER([config.h])

# dnl Initialize Automake
# AM_INIT_AUTOMAKE(1.6.0)

# ACX_MY_ARGS="$*"

#ACX_VERSION_POST

dnl Default installation path
AC_PREFIX_DEFAULT([/usr/local/orocos])

dnl Work around dirty Autoconf -g -02 bug
if test $CFLAGS; then
 ACX_CFLAGS="$CFLAGS"
fi
if test $CXXFLAGS; then
 ACX_CXXFLAGS="$CXXFLAGS"
fi

AC_PROG_CXX
AC_PROG_CC

CFLAGS=""
CXXFLAGS=""
dnl End work around

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_COLORGCC
AC_PROG_INSURE
AC_PROG_XMLPROCESSOR
AC_PROG_DOXYGEN
AC_PROG_DOT
AC_PROG_DIA
AC_PROG_FOP

AC_LANG_CPLUSPLUS
AC_CHECK_HEADER([cppunit/Test.h], [
AM_PATH_CPPUNIT(1.9.6)
],[
AC_MSG_WARN([

Could not find CppUnit headers. The Orocos
Test Suite (ie make check) will not be available.

You can find CppUnit at http://sourceforge.net/projects/cppunit
or if you are a Debian GNU/Linux user, just do:

  apt-get install libcppunit-dev

])
])
AC_LANG_C

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
dnl AC_TYPE_SIZE_T
dnl AC_HEADER_TIME

dnl Checks for library functions.
dnl AC_FUNC_MALLOC
dnl AC_FUNC_VPRINTF

dnl AC_CHECK_FUNCS([gethrtime gettimeofday])

ACX_LIB_GCC
ACX_LIB_STDCXX

])

m4_define([OROCOS_DEFAULT_FLAGS],[
ACX_CFLAGS="$ACX_CFLAGS $1"
ACX_CXXFLAGS="$ACX_CXXFLAGS $2"
ACX_CXXFLAGS="$ACX_CXXFLAGS $3"
])



m4_define([OROCOS_EXPORT_FLAGS],[
AM_CFLAGS="m4_normalize([$ACX_CFLAGS])"
AM_CXXFLAGS="m4_normalize([$ACX_CXXFLAGS])"
AM_CPPFLAGS="m4_normalize([$ACX_CPPFLAGS])"
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_CPPFLAGS)
])


m4_define([OROCOS_OUTPUT_INFO],[
echo "
 Configuration:
   Source code location:                ${ECOS_REP}
   Version:                             ${VERSION}
   Target:                              ${ECOS_TARGET}

   Compiler:                            ${CXX}
   C compiler flags:                    ${AM_CFLAGS} ${CFLAGS}
   C++ compiler flags:                  ${AM_CXXFLAGS} ${CXXFLAGS}
"
echo Run \'make docs\' to generate the documentation.
echo Type \'make new_packages\' to create a package build directory
echo Type \'make configure_packages\' to configure the packages in a GUI
echo Type \'make\' to build the configured system
echo
echo Alternatively, you can \'make all\' to use the default configuration.
echo
echo Read the Orocos Installation Manual for detailed instructions
echo   on what these targets mean.
])


m4_define([OROCOS_GENERAL],
[
# Autoconf code equal for all OROCOS packages
OROCOS_INIT($1,$2,$3,$4,$5)

OROCOS_ARG_TARGETOS

dnl add debugging flags at the end
ACX_ARG_DEBUG
dnl gcov flags
ACX_GCOV

# Checks for libraries.
dnl AC_CHECK_LIB(pthread,pthread_create)

# Checks for header files.
dnl AC_HEADER_STDC
dnl AC_CHECK_HEADERS([stddef.h string.h sys/time.h unistd.h])

DETECT_PACKAGES

]) 


m4_define([DETECT_PACKAGES],[
DETECT_CORBAPKG
DETECT_BOOSTPKG
DETECT_XERCESCPKG
DETECT_COMEDIPKG
DETECT_READLINE
DETECT_RTAI
DETECT_XENOMAI
])

dnl Check for gcov flag
m4_define([ACX_GCOV],[
AC_ARG_ENABLE(gcov, [AC_HELP_STRING([--enable-gcov],[Generate code for gcov ])],
	[ 
	CFLAGS="$CFLAGS -ftest-coverage -fprofile-arcs"
	CXXFLAGS="$CXXFLAGS -ftest-coverage -fprofile-arcs"
	])
])

m4_define([OROCOS_OUTPUT],[
AC_OUTPUT
OROCOS_OUTPUT_INFO
])


    m4_define([OROCOS_ARG_TARGETOS],[
            AC_MSG_CHECKING(which target OS you want to configure for)

    	    AC_ARG_WITH(linux,
            	[AC_HELP_STRING([--with-linux],[Specify (patched) Linux path])],
            	[ with_linux_given=true ],[ with_linux_given=false ])

            AC_ARG_WITH(gnulinux,
                [AC_HELP_STRING([--with-gnulinux],[Use userspace GNU/Linux (NOT REALTIME)])],
                [
                AC_MSG_RESULT(GNU/Linux)
		ECOS_TARGET=gnulinux
                ],
    [
    AC_ARG_WITH(rtai,
            [AC_HELP_STRING([--with-rtai],[Use RTAI (non-LXRT)])],
            [
            AC_MSG_RESULT(RTAI)
	    ECOS_TARGET=rtai
            if test $with_linux_given == false; then
		AC_MSG_ERROR([
You must specify the location of your patched linux kernel headers when using RTAI.
For example : --with-linux=/usr/src/linux-rtai])
	    fi
    ],
    [
    AC_ARG_WITH(lxrt,
            [AC_HELP_STRING([--with-lxrt[=/usr/realtime] ],[Use Lxrt, specify installation directory])],
            [
		if test x"$withval" != xyes; then LXRT_DIR="$withval"; else LXRT_DIR="/usr/realtime"; fi

            AC_MSG_RESULT(LXRT)
	    ECOS_TARGET=lxrt
	    TARGET_LIBS="$TARGET_LIBS -L$LXRT_DIR/lib -llxrt"
	    TARGET_FLAGS="$TARGET_FLAGS -I$LXRT_DIR/include"
            if test $with_linux_given == false; then
		AC_MSG_ERROR([
You must specify the location of your patched linux kernel headers when using Lxrt.
For example : --with-linux=/usr/src/linux-rtai])
	    fi
    ],
    [
    AC_ARG_WITH(xenomai,
            [AC_HELP_STRING([--with-xenomai[=/usr/realtime] ],[Use Xenomai, specify installation directory])],
            [
		if test x"$withval" != xyes; then XENOMAI_DIR="$withval"; else XENOMAI_DIR="/usr/realtime"; fi

            AC_MSG_RESULT(XENOMAI)
	    ECOS_TARGET=xenomai
	    TARGET_LIBS="$TARGET_LIBS -L$XENOMAI_DIR/lib -lnative"
	    TARGET_FLAGS="$TARGET_FLAGS -I$XENOMAI_DIR/include"
#             if test $with_linux_given == false; then
# 		AC_MSG_ERROR([
# You must specify the location of your patched linux kernel headers when using Xenomai.
# For example : --with-linux=/usr/src/linux-rtai])
# 	    fi
    ],
    [
    AC_ARG_WITH(ecos,
            [AC_HELP_STRING([--with-ecos],[Use eCos])],
            [
            AC_MSG_RESULT(eCos)
	    ECOS_TARGET=ecos
            AC_MSG_ERROR([
                The eCos operating system is not supported yet
                ])
            ],
            [
dnl Default to gnulinux
                AC_MSG_RESULT(GNU/Linux)
                ECOS_TARGET=gnulinux
            ]
            )
    ])
    ])
    ])
    ])
    ])
AC_SUBST(ECOS_TARGET)
AC_SUBST(TARGET_LIBS)
AC_SUBST(TARGET_FLAGS)
    ]
    ) # OROCOS_ARG_TARGETOS



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
No Xerces-C 2.X installation found.
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
AC_CHECK_HEADERS([ comedilib.h ],
[
PACKAGES="support/comedi/current/comedi.cdl $PACKAGES"
COMEDI_DIR="."
],
[

#
# THIS part only done if comedilib.h is not in default include path !

AC_MSG_CHECKING(for Comedi dir)
AC_ARG_WITH(comedi, [ AC_HELP_STRING([--with-comedi=/usr/src/comedi/include],[Specify location of comedilib.h ]) ],
	            [ if test x"$withval" != xyes; then COMEDI_DIR="$withval";fi],[ COMEDI_DIR="/usr/src/comedi/include" ])

if test -f $COMEDI_DIR/comedilib.h; then
  # gnu linux comedilib
  PACKAGES="support/comedi/current/comedi.cdl $PACKAGES"
  CPPFLAGS="-I$COMEDI_DIR"
  AC_MSG_RESULT(gnulinux header found in $COMEDI_DIR)
else
  if test -f $COMEDI_DIR/linux/comedilib.h; then
    # lxrt comedi package
    PACKAGES="support/comedi/current/comedi.cdl $PACKAGES"
    CPPFLAGS="-I$COMEDI_DIR"
    AC_MSG_RESULT(lxrt header found in $COMEDI_DIR/linux)
  else
    # no comedi found
    AC_MSG_RESULT(not found)
   #AC_MSG_WARN([No comedi installation found !
   #(tried : $COMEDI_DIR/comedilib.h and $COMEDI_DIR/linux/comedilib.h).
   #Comedi will be unavailable.])
  fi
fi
])
AC_SUBST(COMEDI_DIR)
])

m4_define([DETECT_CORBAPKG],
[

# This macro also generates h/cpp files from the idl header.

AC_ARG_WITH(ace, [ AC_HELP_STRING([--with-ace=/usr/include],[Specify location of ace/config-all.h ]) ],
	[ if test x"$withval" != xyes; then ACE_DIR="$withval";fi],[ 
	if test x$ACE_ROOT = x; then ACE_DIR="/usr/include"; else ACE_DIR=$ACE_ROOT; fi ])
AC_ARG_WITH(tao, [ AC_HELP_STRING([--with-tao=/usr/include],[Specify location of tao/ORB.h ]) ],
	[ if test x"$withval" != xyes; then TAO_DIR="$withval";fi],[ 
	if test x$TAO_ROOT = x; then TAO_DIR="/usr/include"; else TAO_DIR=$TAO_ROOT; fi ])

AC_MSG_CHECKING(for ace/tao dir)
if [ test -f $ACE_DIR/ace/config-all.h && test -f $TAO_DIR/tao/ORB.h ] ; then
  # corbalib
  PACKAGES="support/corba/current/corba.cdl $PACKAGES"
  if test x$ACE_DIR != x/usr/include ; then
     ACX_CFLAGS="$ACX_CFLAGS -I$ACE_DIR"
     ACX_CXXFLAGS="$ACX_CXXFLAGS -I$ACE_DIR"
  fi
  if test x$TAO_DIR != x/usr/include ; then
     ACX_CFLAGS="$ACX_CFLAGS -I$TAO_DIR"
     ACX_CXXFLAGS="$ACX_CXXFLAGS -I$TAO_DIR"
  fi
  CPPFLAGS="-I$ACE_DIR -I$TAO_DIR"
  AC_MSG_RESULT(Ace/Tao headers found)
  if test -x $TAO_ROOT/TAO_IDL/tao_idl; then
	TAO_IDL=$TAO_ROOT/TAO_IDL/tao_idl
  else
	PATH=$PATH:$ACE_ROOT/bin
	TAO_IDL=$(which tao_idl)
	if test x$TAO_IDL=x; then
	    AC_MSG_WARN(headers found, but no tao_idl in path or $TAO_ROOT !)
	fi
  fi
  TARGET_LIBS="$TARGET_LIBS -L$ACE_ROOT/lib -lTAO -lTAO_IDL_BE -lACE -lTAO_PortableServer -lTAO_CosNaming"
  AC_MSG_RESULT($ACE_DIR)
else
  # no corba found
  AC_MSG_RESULT(not found)
fi
AC_SUBST(ACE_DIR)
AC_SUBST(TAO_DIR)
AC_SUBST(TAO_IDL)
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

m4_define([DETECT_GCC],
[
AC_MSG_CHECKING(for GCC version)
ORO_GCC_VERSION=gcc`$CC -dumpversion | sed "s/\./ /g" |awk '{ print $1 }' `
AC_SUBST(ORO_GCC_VERSION)

PACKAGES="support/gcc/current/gcc.cdl $PACKAGES"
AC_MSG_RESULT($ORO_GCC_VERSION)
])



m4_define([DETECT_RTAI],
[
AC_MSG_CHECKING(for RTAI/LXRT Installation)
AC_ARG_WITH(rtai, [ --with-rtai[=/usr/realtime] Specify location of RTAI/LXRT ],
	[ if test x"$withval" != xyes; then RTAI_DIR="$withval"; fi ])
AC_ARG_WITH(lxrt, [ --with-lxrt[=/usr/realtime] Equivalent to --with-rtai ],
	[ if test x"$withval" != xyes; then RTAI_DIR="$withval"; fi ])
AC_ARG_WITH(linux,
	 [AC_HELP_STRING([--with-linux],[Specify RTAI-patched Linux directory (without /include).])],
	 [ if test x"$withval" != xyes; then LINUX_KERNEL_DIR="$withval"; fi ])

if test x"$RTAI_DIR" = x; then
   RTAI_DIR="/usr/realtime"
fi
if test x"$LINUX_KERNEL_DIR" = x; then
   LINUX_KERNEL_DIR="/usr/src/linux"
fi
LINUX_KERNEL_HEADERS="$LINUX_KERNEL_DIR/include"
AC_MSG_RESULT($RTAI_DIR with kernel headers in $LINUX_KERNEL_HEADERS)

AC_SUBST(RTAI_DIR)
AC_SUBST(LINUX_KERNEL_HEADERS)
AC_SUBST(LINUX_KERNEL_DIR)

CPPFLAGS="-I$RTAI_DIR/include -I$LINUX_KERNEL_HEADERS"
AC_CHECK_HEADERS([rtai_config.h], [
  AC_CHECK_HEADERS([rtai_lxrt.h],
  [
    PACKAGES="support/rtai/current/rtai.cdl $PACKAGES"
    RTAI_VERSION=3
  ],
  [
    AC_MSG_WARN([No RTAI/LXRT installation found (rtai_config.h, rtai_lxrt.h). LXRT will be unavailable.])
  ])
],[
dnl try old rtai style
  AC_CHECK_HEADERS([rtai_lxrt_user.h], 
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

m4_define([DETECT_XENOMAI],
[
AC_MSG_CHECKING(for XENOMAI 2.0 Installation)
AC_ARG_WITH(xenomai, [ --with-xenomai[=/usr/realtime] Specify location of XENOMAI ],
	[ if test x"$withval" != xyes; then XENOMAI_DIR="$withval"; fi ])

if test x"$XENOMAI_DIR" = x; then
   XENOMAI_DIR="/usr/realtime"
fi

AC_MSG_RESULT($XENOMAI_DIR with kernel headers in $LINUX_KERNEL_HEADERS)

AC_SUBST(XENOMAI_DIR)

CPPFLAGS="-I$XENOMAI_DIR/include -I$LINUX_KERNEL_HEADERS"
AC_CHECK_HEADERS([xeno_config.h], [
    PACKAGES="support/xenomai/current/xenomai.cdl $PACKAGES"
    XENOMAI_VERSION=2
],[])
if test x"$XENOMAI_VERSION" = x; then
   XENOMAI_VERSION=0
fi
AC_SUBST(XENOMAI_VERSION)
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

 if test x$subvsn = xyes -a x$SVN != x; then

 echo "{ print $""2  }" > print-svn.awk
 SVN_VERSION=$(svn info . 2>/dev/null \
	| grep "Revision" | awk -f print-svn.awk )
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
#AC_PROG_DOXYGEN

dnl Work around dirty Autoconf -g -02 bug
if test $CFLAGS; then
 ACX_CFLAGS="$CFLAGS"
fi
if test $CXXFLAGS; then
 ACX_CXXFLAGS="$CXXFLAGS"
fi

AC_PROG_CXX
AC_PROG_CC

CFLAGS=""
CXXFLAGS=""
dnl End work around

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
])



m4_define([PACKAGES_OUTPUT],[
AC_OUTPUT
PACKAGES_OUTPUT_INFO
])

dnl Borrowed from CPPUNIT themselves.
dnl
dnl AM_PATH_CPPUNIT(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AM_PATH_CPPUNIT],
[

AC_ARG_WITH(cppunit-prefix,[  --with-cppunit-prefix=PFX   Prefix where CppUnit is installed (optional)],
            cppunit_config_prefix="$withval", cppunit_config_prefix="")
AC_ARG_WITH(cppunit-exec-prefix,[  --with-cppunit-exec-prefix=PFX  Exec prefix where CppUnit is installed (optional)],
            cppunit_config_exec_prefix="$withval", cppunit_config_exec_prefix="")

  if test x$cppunit_config_exec_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --exec-prefix=$cppunit_config_exec_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_exec_prefix/bin/cppunit-config
     fi
  fi
  if test x$cppunit_config_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --prefix=$cppunit_config_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_prefix/bin/cppunit-config
     fi
  fi

  AC_PATH_PROG(CPPUNIT_CONFIG, cppunit-config, no)
  cppunit_version_min=$1

  AC_MSG_CHECKING(for Cppunit - version >= $cppunit_version_min)
  no_cppunit=""
  if test "$CPPUNIT_CONFIG" = "no" ; then
    no_cppunit=yes
  else
    CPPUNIT_CFLAGS=`$CPPUNIT_CONFIG --cflags`
    CPPUNIT_LIBS=`$CPPUNIT_CONFIG --libs`
    cppunit_version=`$CPPUNIT_CONFIG --version`

    cppunit_major_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    cppunit_minor_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    cppunit_micro_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    cppunit_major_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    cppunit_minor_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    cppunit_micro_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    cppunit_version_proper=`expr \
        $cppunit_major_version \> $cppunit_major_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \> $cppunit_minor_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \= $cppunit_minor_min \& \
        $cppunit_micro_version \>= $cppunit_micro_min `

    if test "$cppunit_version_proper" = "1" ; then
      AC_MSG_RESULT([$cppunit_major_version.$cppunit_minor_version.$cppunit_micro_version])
    else
      AC_MSG_RESULT(no)
      no_cppunit=yes
    fi
  fi

  if test "x$no_cppunit" = x ; then
     ifelse([$2], , :, [$2])     
  else
     CPPUNIT_CFLAGS=""
     CPPUNIT_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(CPPUNIT_CFLAGS)
  AC_SUBST(CPPUNIT_LIBS)
])



