###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

# The cmake version is f*cked up, i.e. the spirit of cmake.
INCLUDE( config/CheckIncludeFileCXX.cmake )

# Look for boost
IF (NOT CMAKE_CROSS_COMPILE)

# This is the Orocos version of the macro:
CHECK_INCLUDE_FILE_CXX( boost/shared_ptr.hpp BOOST)
CHECK_INCLUDE_FILE_CXX( boost/spirit.hpp BOOST_SPIRIT)
CHECK_INCLUDE_FILE_CXX( boost/graph/adjacency_list.hpp BOOST_GRAPH)
IF(BOOST AND BOOST_GRAPH AND BOOST_SPIRIT)
  MESSAGE("-- Looking for Boost headers - found")
  SET(ORO_SUPPORT_BOOST TRUE CACHE INTERNAL "" FORCE)
ELSE(BOOST AND BOOST_GRAPH AND BOOST_SPIRIT)
  MESSAGE("-- Looking for Boost headers - not found")
  SET(ORO_SUPPORT_BOOST FALSE CACHE INTERNAL "" FORCE)
  MESSAGE( FATAL_ERROR "Install Boost C++ (libboost-dev, libboost-graph-dev) version 0.32.0 or newer.")
ENDIF(BOOST AND BOOST_GRAPH AND BOOST_SPIRIT)
ENDIF (NOT CMAKE_CROSS_COMPILE)

# Look for Xerces (Do not change these SET statements !)
IF (NOT CMAKE_CROSS_COMPILE )
  FIND_LIBRARY(XERCES NAMES xerces-c 
    PATHS /usr/local/lib /usr/lib )
  CHECK_INCLUDE_FILE_CXX( xercesc/util/PlatformUtils.hpp XERCES_HEADERS)
ELSE (NOT CMAKE_CROSS_COMPILE )
  FIND_LIBRARY(XERCES NAMES xerces-c 
    NO_DEFAULT_PATH
    )
ENDIF (NOT CMAKE_CROSS_COMPILE )
IF ( XERCES AND XERCES_HEADERS)
  MESSAGE("-- Looking for Xerces - found")
  SET(OROPKG_SUPPORT_XERCES_C TRUE CACHE INTERNAL "" FORCE)
  LINK_LIBRARIES(xerces-c)
  SET(ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE "\"marsh/CPFMarshaller.hpp\"")
  SET(OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER "CPFMarshaller")
  SET(ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE "\"marsh/CPFDemarshaller.hpp\"")
  SET(OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER "CPFDemarshaller")
ELSE ( XERCES AND XERCES_HEADERS )
  IF (NOT XERCES_HEADERS)
    MESSAGE("-- Looking for Xerces - headers not found")
  ENDIF (NOT XERCES_HEADERS)

  SET(OROPKG_SUPPORT_XERCES_C FALSE CACHE INTERNAL "" FORCE)

  SET(ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE "\"marsh/CPFMarshaller.hpp\"")
  SET(OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER "CPFMarshaller")
  SET(ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE "\"marsh/TinyDemarshaller.hpp\"")
  SET(OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER "TinyDemarshaller")
ENDIF ( XERCES AND XERCES_HEADERS )

SET( OROCOS_TARGET gnulinux CACHE STRING "The Operating System target. One of [lxrt gnulinux xenomai]")
STRING(TOUPPER ${OROCOS_TARGET} OROCOS_TARGET_CAP)

SET(LINUX_SOURCE_DIR ${LINUX_SOURCE_DIR} CACHE PATH "path to linux source dir" FORCE)

# Look for TAO and ACE
INCLUDE(${PROJ_SOURCE_DIR}/config/FindCorbaDeps.cmake)


IF(OROCOS_TARGET STREQUAL "lxrt")
  SET(OROPKG_OS_LXRT TRUE CACHE INTERNAL "" FORCE)
  # Look for LXRT
  SET(RTAI_INSTALL_DIR ${RTAI_INSTALL_DIR} CACHE PATH "path to rtai installation dir" FORCE)
  IF (RTAI_INSTALL_DIR STREQUAL "")
    SET(RTAI_INSTALL_DIR "/usr/realtime")
  ENDIF (RTAI_INSTALL_DIR STREQUAL "")
  IF(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
    MESSAGE("-- Looking for LXRT - found in ${RTAI_INSTALL_DIR}")
    SET(OROPKG_SUPPORT_RTAI TRUE CACHE INTERNAL "" FORCE)
    INCLUDE_DIRECTORIES(${RTAI_INSTALL_DIR}/include ${LINUX_SOURCE_DIR}/include)
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${RTAI_INSTALL_DIR}/include -I${LINUX_SOURCE_DIR}/include" CACHE INTERNAL "")
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -L${RTAI_INSTALL_DIR}/lib -llxrt -lpthread" CACHE INTERNAL "")
    LINK_LIBRARIES(lxrt pthread dl)
    LINK_DIRECTORIES(${RTAI_INSTALL_DIR}/lib)
  ELSE(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
    MESSAGE(FATAL_ERROR "-- Looking for LXRT - not found (tried: ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)")
    SET(OROPKG_SUPPORT_RTAI FALSE CACHE INTERNAL "" FORCE)
  ENDIF(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
ELSE(OROCOS_TARGET STREQUAL "lxrt")
  SET(RTAI_INSTALL_DIR "/usr/realtime" CACHE INTERNAL "path to rtai installation dir")
  SET(OROPKG_OS_LXRT FALSE CACHE INTERNAL "" FORCE)
ENDIF(OROCOS_TARGET STREQUAL "lxrt")


IF(OROCOS_TARGET STREQUAL "xenomai")
  # Look for Xenomai
  SET(XENOMAI_INSTALL_DIR ${XENOMAI_INSTALL_DIR} CACHE PATH "path to xenomai installation dir" FORCE)
  IF (XENOMAI_INSTALL_DIR STREQUAL "")
    SET(XENOMAI_INSTALL_DIR "/usr/realtime")
  ENDIF (XENOMAI_INSTALL_DIR STREQUAL "")
  SET(OROPKG_OS_XENOMAI TRUE CACHE INTERNAL "" FORCE)
  # Standard path of Xenomai: ( XENOMAI_INSTALL_DIR == /usr/realtime )
  IF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native/task.h)
    MESSAGE("-- Looking for XENOMAI - found in ${XENOMAI_INSTALL_DIR}/include")
    SET( XENOMAI_INCLUDE_DIR "${XENOMAI_INSTALL_DIR}/include" )
  ENDIF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native/task.h)
  # Debian package of Xenomai: ( XENOMAI_INSTALL_DIR == /usr )
  IF(EXISTS ${XENOMAI_INSTALL_DIR}/include/xenomai/native/task.h)
    MESSAGE("-- Looking for XENOMAI - found in ${XENOMAI_INSTALL_DIR}/include/xenomai")
    SET( XENOMAI_INCLUDE_DIR "${XENOMAI_INSTALL_DIR}/include/xenomai" )
  ENDIF(EXISTS ${XENOMAI_INSTALL_DIR}/include/xenomai/native/task.h)

  IF ( XENOMAI_INCLUDE_DIR )
    SET(XENOMAI_SUPPORT TRUE CACHE INTERNAL "" FORCE)
    INCLUDE_DIRECTORIES( ${XENOMAI_INCLUDE_DIR} )
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${XENOMAI_INCLUDE_DIR}" CACHE INTERNAL "")
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -L${XENOMAI_INSTALL_DIR}/lib -lnative -lpthread" CACHE INTERNAL "")
    LINK_LIBRARIES(native pthread dl)
    LINK_DIRECTORIES(${XENOMAI_INSTALL_DIR}/lib)
  ELSE( XENOMAI_INCLUDE_DIR )
    MESSAGE(FATAL_ERROR "-- Looking for XENOMAI - not found (tried: ${XENOMAI_INSTALL_DIR}/include/native/task.h and ${XENOMAI_INSTALL_DIR}/include/xenomai/native/task.h)")
    SET(XENOMAI_SUPPORT FALSE CACHE INTERNAL "" FORCE)
  ENDIF( XENOMAI_INCLUDE_DIR )
ELSE(OROCOS_TARGET STREQUAL "xenomai")
  SET(XENOMAI_INSTALL_DIR "/usr/realtime" CACHE INTERNAL "path to xenomai installation dir")
  SET(OROPKG_OS_XENOMAI FALSE CACHE INTERNAL "" FORCE)
ENDIF(OROCOS_TARGET STREQUAL "xenomai")


IF(OROCOS_TARGET STREQUAL "gnulinux")
  SET(OROPKG_OS_GNULINUX TRUE CACHE INTERNAL "" FORCE)
  SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lpthread -lrt" CACHE INTERNAL "")
  LINK_LIBRARIES(pthread dl rt)
ELSE(OROCOS_TARGET STREQUAL "gnulinux")
  SET(OROPKG_OS_GNULINUX FALSE CACHE INTERNAL "" FORCE)
ENDIF(OROCOS_TARGET STREQUAL "gnulinux")

IF(OROCOS_TARGET STREQUAL "ecos")

  # Look for Ecos
  SET(ECOS_INSTALL_DIR ${ECOS_INSTALL_DIR} CACHE PATH "path to ecos installation dir" FORCE)
  IF (ECOS_INSTALL_DIR STREQUAL "")
    SET(ECOS_INSTALL_DIR "/opt/ecos/install")
  ENDIF (ECOS_INSTALL_DIR STREQUAL "")
  SET(OROPKG_OS_ECOS TRUE CACHE INTERNAL "" FORCE)
  IF(EXISTS ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)
    MESSAGE("-- Looking for ECOS - found in ${ECOS_INSTALL_DIR}")
    SET(ECOS_SUPPORT TRUE CACHE INTERNAL "" FORCE)
    INCLUDE_DIRECTORIES(${ECOS_INSTALL_DIR}/include)
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${ECOS_INSTALL_DIR}/include" CACHE INTERNAL "")
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -L${ECOS_INSTALL_DIR}/lib -ltarget" CACHE INTERNAL "")
    LINK_LIBRARIES( target )
    LINK_DIRECTORIES(${ECOS_INSTALL_DIR}/lib)
  ELSE(EXISTS ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)
    MESSAGE(FATAL_ERROR "-- Looking for ECOS - not found (tried: ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)")
    SET(ECOS_SUPPORT FALSE CACHE INTERNAL "" FORCE)
  ENDIF(EXISTS ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)

  MESSAGE( "Turning BUILD_STATIC ON for ecos.")
  SET( FORCE_BUILD_STATIC ON CACHE INTERNAL "" FORCE)
  SET( BUILD_STATIC ON CACHE BOOL "Build Orocos RTT as a static library" FORCE)

ELSE(OROCOS_TARGET STREQUAL "ecos")
  SET(OROPKG_OS_ECOS FALSE CACHE INTERNAL "" FORCE)
  SET(ECOS_INSTALL_DIR "/opt/ecos/install" CACHE INTERNAL "path to ecos installation dir")
ENDIF(OROCOS_TARGET STREQUAL "ecos")


# The machine type is tested using compiler macros in rtt-config.h.in
