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
    MESSAGE(FATAL_ERROR "-- Looking for LXRT - not found")
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
  IF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native/task.h)
    MESSAGE("-- Looking for XENOMAI - found in ${XENOMAI_INSTALL_DIR}")
    SET(XENOMAI_SUPPORT TRUE CACHE INTERNAL "" FORCE)
    INCLUDE_DIRECTORIES(${XENOMAI_INSTALL_DIR}/include)
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${XENOMAI_INSTALL_DIR}/include" CACHE INTERNAL "")
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -L${XENOMAI_INSTALL_DIR}/lib -lnative -lpthread" CACHE INTERNAL "")
    LINK_LIBRARIES(native pthread dl)
    LINK_DIRECTORIES(${XENOMAI_INSTALL_DIR}/lib)
  ELSE(EXISTS ${XENOMAI_INSTALL_DIR}/include/native/task.h)
    MESSAGE(FATAL_ERROR "-- Looking for XENOMAI - not found")
    SET(XENOMAI_SUPPORT FALSE CACHE INTERNAL "" FORCE)
  ENDIF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native/task.h)
ELSE(OROCOS_TARGET STREQUAL "xenomai")
  SET(XENOMAI_INSTALL_DIR "/usr/realtime" CACHE INTERNAL "path to xenomai installation dir")
  SET(OROPKG_OS_XENOMAI FALSE CACHE INTERNAL "" FORCE)
ENDIF(OROCOS_TARGET STREQUAL "xenomai")


IF(OROCOS_TARGET STREQUAL "gnulinux")
  SET(OROPKG_OS_GNULINUX TRUE CACHE INTERNAL "" FORCE)
  SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lpthread" CACHE INTERNAL "")
  LINK_LIBRARIES(pthread dl)
ELSE(OROCOS_TARGET STREQUAL "gnulinux")
  SET(OROPKG_OS_GNULINUX FALSE CACHE INTERNAL "" FORCE)
ENDIF(OROCOS_TARGET STREQUAL "gnulinux")


# The machine type is tested using compiler macros in rtt-config.h.in
