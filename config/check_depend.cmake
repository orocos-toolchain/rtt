# Modules path (for searching FindXXX.cmake files)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config")

include (SelectOneLibrary)

SET(CMAKE_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
MESSAGE(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")


###########################################################
#                                                         #
# Some global options we need to set here                 #
#                                                         #
###########################################################
#
# STATIC or SHARED
#
OPTION( BUILD_STATIC "Build Orocos RTT as a static library." ${FORCE_BUILD_STATIC})
#
# CORBA
#
OPTION( ENABLE_CORBA "Enable CORBA" OFF)
IF (NOT CORBA_IMPLEMENTATION)
  SET( CORBA_IMPLEMENTATION "TAO" CACHE STRING "The implementation of CORBA to use (allowed values: TAO or OMNIORB )" )
ELSE(NOT CORBA_IMPLEMENTATION)
  SET( CORBA_IMPLEMENTATION ${CORBA_IMPLEMENTATION} CACHE STRING "The implementation of CORBA to use (allowed values: TAO or OMNIORB )" )
ENDIF (NOT CORBA_IMPLEMENTATION)
#
# CORBA Remote OperationCallers in C++
#
OPTION( ORO_REMOTING "Enable transparant Remote OperationCallers Calls in C++" ON )
# Force remoting when CORBA is enabled.
IF ( ENABLE_CORBA AND NOT ORO_REMOTING )
  MESSAGE( "Forcing ORO_REMOTING to ON")
  SET( ORO_REMOTING ON CACHE BOOL "Enable transparant Remote OperationCallers and Commands in C++" FORCE)
ENDIF( ENABLE_CORBA AND NOT ORO_REMOTING )

# Is modified by target selection below
OPTION(OS_NO_ASM "Do not use any assembler instruction, but stick to ISO C++ as much as possible. This will exclude lock-free and atomic algorithms." OFF )
if (OS_NO_ASM AND Boost_VERSION LESS 103600)
  message(SEND_ERROR "OS_NO_ASM was turned on, but this requires Boost v1.36.0 or newer.")
endif()

OPTION(PLUGINS_ENABLE "Enable plugins" ON)
OPTION(PLUGINS_STD_TYPES_SUPPORT "Enable support for the std::string and std::vector<double> types in the RTT typekit & transports." ON)
    
OPTION(ORO_NO_EMIT_CORBA_IOR "Do not emit CORBA IORs if name service not used" OFF)

###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

# Look for boost We look up all components in one place because this macro does
# not support multiple invocations in some CMake versions.
find_package(Boost 1.38 COMPONENTS filesystem system unit_test_framework thread serialization)

# Look for boost
if ( PLUGINS_ENABLE )
  if (NOT Boost_FILESYSTEM_FOUND OR NOT Boost_SYSTEM_FOUND)
    message(SEND_ERROR "Plugins require Boost Filesystem and System libraries, but they were not found.")
  endif()
  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${Boost_FILESYSTEM_INCLUDE_DIRS} ${Boost_SYSTEM_INCLUDE_DIRS} ${Boost_SERIALIZATION_INCLUDE_DIRS})
  list(APPEND OROCOS-RTT_LIBRARIES ${Boost_FILESYSTEM_LIBRARIES} ${Boost_SYSTEM_LIBRARIES} ${Boost_SERIALIZATION_LIBRARIES}) 
endif()

if(Boost_INCLUDE_DIR)
  #message(STATUS "Boost found in ${Boost_INCLUDE_DIR}")  # already printed by find_package(Boost)
  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${Boost_INCLUDE_DIR} )
  if(OROCOS_TARGET STREQUAL "win32")
    add_definitions(-DBOOST_ALL_NO_LIB)
  endif()
    
  # We don't link with boost here. It depends on the options set by the user.
  #list(APPEND OROCOS-RTT_LIBRARIES ${Boost_LIBRARIES} )
else(Boost_INCLUDE_DIR)
  message(FATAL_ERROR "Boost_INCLUDE_DIR not found ! Add it to your CMAKE_PREFIX_PATH !")
endif(Boost_INCLUDE_DIR)

# Look for linux capabilities (7)
find_library(LINUX_CAP_NG_LIBRARY cap-ng)

# Look for Xerces 

# If a nonstandard path is used when crosscompiling, uncomment the following lines
# IF(NOT CMAKE_CROSS_COMPILE) # NOTE: There now exists a standard CMake variable named CMAKE_CROSSCOMPILING
#   set(XERCES_ROOT_DIR /path/to/xerces CACHE INTERNAL "" FORCE) # you can also use set(ENV{XERCES_ROOT_DIR} /path/to/xerces)
# ENDIF(NOT CMAKE_CROSS_COMPILE)

OPTION(OROBLD_FORCE_TINY_DEMARSHALLER "Force usage of TinyDemarshaller." OFF)

IF (NOT OROBLD_FORCE_TINY_DEMARSHALLER)
  find_package(Xerces)
ENDIF (NOT OROBLD_FORCE_TINY_DEMARSHALLER)

# Look for Boost Uuid or libuuid
find_path(Boost_UUID_INCLUDE_DIR boost/uuid/uuid.hpp PATHS ${Boost_INCLUDE_DIRS})
if(Boost_UUID_INCLUDE_DIR)
  message(STATUS "Found Boost Uuid in ${Boost_UUID_INCLUDE_DIR}.")
  set(ORO_HAVE_BOOST_UUID TRUE)
else()
  find_library(uuid_LIBRARY uuid)
  if(uuid_LIBRARY)
    message(STATUS "Found libuuid at ${uuid_LIBRARY}.")
    set(ORO_HAVE_LIBUUID TRUE)
    list(APPEND OROCOS-RTT_LIBRARIES ${uuid_LIBRARY})
  else()
    message(FATAL_ERROR "Either Boost Uuid (Boost >=1.42) or libuuid is required to build RTT!")
  endif()
endif()


if(XERCES_FOUND)
  set(OROPKG_SUPPORT_XERCES_C TRUE CACHE INTERNAL "" FORCE)
  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${XERCES_INCLUDE_DIRS} )
  list(APPEND OROCOS-RTT_LIBRARIES ${XERCES_LIBRARIES} ) 
  set(ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE "\"marsh/CPFMarshaller.hpp\"")
  set(OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER "CPFMarshaller")
  set(ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE "\"marsh/CPFDemarshaller.hpp\"")
  set(OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER "CPFDemarshaller")
else(XERCES_FOUND)
  set(OROPKG_SUPPORT_XERCES_C FALSE CACHE INTERNAL "" FORCE)
  set(ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE "\"marsh/CPFMarshaller.hpp\"")
  set(OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER "CPFMarshaller")
  set(ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE "\"marsh/TinyDemarshaller.hpp\"")
  set(OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER "TinyDemarshaller")
endif(XERCES_FOUND)

# Check for OS/Target specific dependencies:
message(STATUS "Orocos target is ${OROCOS_TARGET}")
string(TOUPPER ${OROCOS_TARGET} OROCOS_TARGET_CAP)

if ( NOT ";lxrt;gnulinux;xenomai;macosx;win32;" MATCHES ".*;${OROCOS_TARGET};.*")
  message( FATAL_ERROR "OROCOS_TARGET=${OROCOS_TARGET} is an unkown target. Please use one of lxrt;gnulinux;xenomai;macosx;win32.")
endif()

# Setup flags for RTAI/LXRT
if(OROCOS_TARGET STREQUAL "lxrt")
  set(OROPKG_OS_LXRT TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(LINUX_SOURCE_DIR ${LINUX_SOURCE_DIR} CACHE PATH "Path to Linux source dir (required for lxrt target)" FORCE)
  set(OS_HAS_TLSF TRUE)

  find_package(RTAI REQUIRED)
  find_package(Pthread REQUIRED)

  add_definitions( -Wall )

  if(RTAI_FOUND)
    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${RTAI_INCLUDE_DIRS} ${PTHREAD_INCLUDE_DIRS})
    list(APPEND OROCOS-RTT_LIBRARIES ${RTAI_LIBRARIES} ${PTHREAD_LIBRARIES} dl) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
    list(APPEND MQ_LIBRARIES "rt")
  endif()
else()
  set(OROPKG_OS_LXRT FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for Xenomai
if(OROCOS_TARGET STREQUAL "xenomai")
  set(OROPKG_OS_XENOMAI TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(OS_HAS_TLSF TRUE)

  find_package(Xenomai REQUIRED)
  find_package(Pthread REQUIRED)
  find_package(XenomaiPosix)

  add_definitions( -Wall )

  if(XENOMAI_FOUND)
    # Input for .pc and .cmake generated files:
    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${XENOMAI_INCLUDE_DIRS} ${PTHREAD_INCLUDE_DIRS})
    list(APPEND OROCOS-RTT_LIBRARIES ${XENOMAI_LIBRARIES} ${PTHREAD_LIBRARIES} dl) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
    # Direct input only for .pc file:
    list(APPEND RTT_USER_LDFLAGS ${XENOMAI_LDFLAGS} )
    list(APPEND RTT_USER_CFLAGS ${XENOMAI_CFLAGS} )
    if (XENOMAI_POSIX_FOUND)
      set(MQ_LDFLAGS ${XENOMAI_POSIX_LDFLAGS} )
      set(MQ_CFLAGS ${XENOMAI_POSIX_CFLAGS} )
      set(MQ_INCLUDE_DIRS ${XENOMAI_POSIX_INCLUDE_DIRS})
      set(MQ_LIBRARIES ${XENOMAI_POSIX_LIBRARIES})
    endif()
  endif()
else()
  set(OROPKG_OS_XENOMAI FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for GNU/Linux
if(OROCOS_TARGET STREQUAL "gnulinux")
  set(OROPKG_OS_GNULINUX TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(OS_HAS_TLSF TRUE)

  find_package(Boost 1.36 COMPONENTS thread )
  find_package(Pthread REQUIRED)

  include(CheckLibraryExists)
  check_library_exists(pthread "pthread_setname_np" "" ORO_HAVE_PTHREAD_SETNAME_NP)

  add_definitions( -Wall )

  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${PTHREAD_INCLUDE_DIRS})
  list(APPEND OROCOS-RTT_USER_LINK_LIBS ${PTHREAD_LIBRARIES} rt) # For libraries used in inline (fosi/template) code.

  list(APPEND OROCOS-RTT_LIBRARIES ${PTHREAD_LIBRARIES} rt dl) 
  list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
else()
  set(OROPKG_OS_GNULINUX FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for Mac-OSX
if(OROCOS_TARGET STREQUAL "macosx")
  set(OROPKG_OS_MACOSX TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(OS_HAS_TLSF TRUE)

  if (NOT Boost_THREAD_FOUND)
	message(SEND_ERROR "Boost thread library not found but required on macosx.")
  endif ()

  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${Boost_THREAD_INCLUDE_DIRS} ${Boost_SYSTEM_INCLUDE_DIRS} )

  SELECT_ONE_LIBRARY("Boost_THREAD_LIBRARY" BOOST_THREAD_LIB)
  LIST(APPEND OROCOS-RTT_USER_LINK_LIBS ${BOOST_THREAD_LIB})

  SELECT_ONE_LIBRARY("Boost_SYSTEM_LIBRARY" BOOST_SYSTEM_LIB)
  LIST(APPEND OROCOS-RTT_USER_LINK_LIBS ${BOOST_SYSTEM_LIB})

  message( "Forcing ORO_OS_USE_BOOST_THREAD to ON")
  set( ORO_OS_USE_BOOST_THREAD ON CACHE BOOL "Forced enable use of Boost.thread on macosx." FORCE)

  # Force OFF on mqueue transport on macosx
  message("Forcing ENABLE_MQ to OFF for macsox")
  set(ENABLE_MQ OFF CACHE BOOL "This option is forced to OFF by the build system on macosx platform." FORCE)

  # see also src/CMakeLists.txt as it adds the boost_thread library to OROCOS_RTT_LIBRARIES
  list(APPEND OROCOS-RTT_LIBRARIES ${PTHREAD_LIBRARIES} dl) 
  list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 

else()
  set(OROPKG_OS_MACOSX FALSE CACHE INTERNAL "" FORCE)
endif()


# Setup flags for ecos
if(OROCOS_TARGET STREQUAL "ecos")
  set(OROPKG_OS_ECOS TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(OS_HAS_TLSF FALSE)

  # We can't really use 'UseEcos.cmake' because we're building a library
  # and not a final application
  find_package(Ecos REQUIRED)

  if(Ecos_FOUND)

    set(ECOS_SUPPORT TRUE CACHE INTERNAL "" FORCE)

    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${ECOS_INCLUDE_DIRS} )
    list(APPEND OROCOS-RTT_LIBRARIES ${ECOS_LIBRARIES} pthread) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 

    message( "Turning BUILD_STATIC ON for ecos.")
    set( FORCE_BUILD_STATIC ON CACHE INTERNAL "Forces to build Orocos RTT as a static library (forced to ON by Ecos)" FORCE)
    set( BUILD_STATIC ON CACHE BOOL "Build Orocos RTT as a static library (forced to ON by Ecos)" FORCE)
  endif()
else()
  set(OROPKG_OS_ECOS FALSE CACHE INTERNAL "" FORCE)
endif()

if(OROCOS_TARGET STREQUAL "win32")
  set(OROPKG_OS_WIN32 TRUE CACHE INTERNAL "" FORCE)
  message("Forcing OS_HAS_TLSF to OFF for WIN32")
  set(OS_HAS_TLSF FALSE)
  # Force OFF on mqueue transport on WIN32 platform
  message("Forcing ENABLE_MQ to OFF for WIN32")
  set(ENABLE_MQ OFF CACHE BOOL "This option is forced to OFF by the build system on WIN32 platform." FORCE)
  if (MINGW)
    #--enable-all-export and --enable-auto-import are already set by cmake.
    #but we need it here for the unit tests as well.
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-auto-import -Wl,--export-all-symbols")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-auto-import")
    list(APPEND OROCOS-RTT_LIBRARIES wsock32.lib winmm.lib)
  endif()
  if (MSVC)
    if (NOT MSVC80)
        set(NUM_PARALLEL_BUILD 4 CACHE STRING "Number of parallel builds")
        set(PARALLEL_FLAG "/MP${NUM_PARALLEL_BUILD}")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4355 /wd4251 /wd4180 /wd4996 /wd4250 /wd4748 /bigobj /Oi ${PARALLEL_FLAG}")
    list(APPEND OROCOS-RTT_LIBRARIES kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib  ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib Ws2_32.lib winmm.lib)
  endif()
  list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
  set(CMAKE_DEBUG_POSTFIX "d")
else(OROCOS_TARGET STREQUAL "win32")
  set(OROPKG_OS_WIN32 FALSE CACHE INTERNAL "" FORCE)
endif(OROCOS_TARGET STREQUAL "win32")

if( NOT OROCOS-RTT_DEFINITIONS )
  message(FATAL_ERROR "No suitable OROCOS_TARGET found. Please check your setup or provide additional search paths to cmake.")
endif()

# The machine type is tested using compiler macros in rtt-config.h.in
# Add found include dirs.
INCLUDE_DIRECTORIES( ${OROCOS-RTT_INCLUDE_DIRS} )

#
# Set compiler specific options
#
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(RTT_GCC_HASVISIBILITY TRUE)
  # Disable line wrapping for g++ such that eclipse can parse the errors.
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  SET(RTT_GCC_HASVISIBILITY TRUE)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

#
# Set flags for code coverage, and setup coverage target
#
IF (BUILD_ENABLE_COVERAGE)

  FIND_PACKAGE(Lcov REQUIRED)

  # for required flags see
  # http://git.benboeckel.net/?p=chasmd.git;a=blob;f=CMakeLists.txt
  # http://www.cmake.org/Wiki/CTest:Coverage
  # man gcov

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
  SET(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -fprofile-arcs -ftest-coverage")

  # coverage
  ADD_CUSTOM_TARGET(coverage)
  ADD_CUSTOM_COMMAND(TARGET coverage
    COMMAND mkdir -p coverage
    COMMAND ${LCOV_LCOV_EXECUTABLE} --directory . --zerocounters
    COMMAND make check
	# all coverage data
    COMMAND ${LCOV_LCOV_EXECUTABLE} --directory . --capture --output-file ./coverage/all.info
	# RTT-only coverage data
    COMMAND ${LCOV_LCOV_EXECUTABLE} --output-file ./coverage/rtt.info -e ./coverage/all.info '${CMAKE_SOURCE_DIR}/*'
	# generate based on RTT-only data
    COMMAND ${LCOV_GENHTML_EXECUTABLE} -t "Orocos RTT coverage" -p "${CMAKE_SOURCE_DIR}"  -o ./coverage ./coverage/rtt.info
    COMMAND echo "Open ${CMAKE_BINARY_DIR}/coverage/index.html to view the coverage analysis results."
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
  # todo dependancy of coverage on test?

ENDIF (BUILD_ENABLE_COVERAGE)

#
# Check for Doxygen and enable documentation building
#
find_package( Doxygen )
IF ( DOXYGEN_EXECUTABLE )
  MESSAGE( STATUS "Found Doxygen -- API documentation can be built" )
ELSE ( DOXYGEN_EXECUTABLE )
  MESSAGE( STATUS "Doxygen not found -- unable to build documentation" )
ENDIF ( DOXYGEN_EXECUTABLE )

#
# Detect CORBA using user's CORBA_IMPLEMENTATION
#
find_package(Corba REQUIRED)

