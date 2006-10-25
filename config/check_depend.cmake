###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

# To check include files
INCLUDE (CheckIncludeFiles)

# For examples of dependency checks see orca-components/config/check_depend.cmake

# Look for Player (ctually looks for v.>=1.6)
# INCLUDE (${PROJ_SOURCE_DIR}/config/FindPlayer2.cmake)

SET(CURSES_INCLUDE_DIR 0 INTERNAL)
INCLUDE( ${CMAKE_ROOT}/Modules/FindCurses.cmake )
IF ( CURSES_INCLUDE_DIR )
    MESSAGE("-- Looking for libncurses - found")
ELSE ( CURSES_INCLUDE_DIR )
    MESSAGE("-- Looking for libncurses - not found")
ENDIF ( CURSES_INCLUDE_DIR )

# Look for boost
CHECK_INCLUDE_FILE_CXX(boost/shared_ptr.hpp BOOST_1)
CHECK_INCLUDE_FILE_CXX(boost/spirit.hpp BOOST_2)
CHECK_INCLUDE_FILE_CXX(boost/graph/adjacency_list.hpp BOOST_3)
IF ( BOOST_1 AND BOOST_2 AND BOOST_3 )
  MESSAGE("-- Looking for Boost headers - found")
  SET(BOOST_SUPPORT TRUE CACHE INTERNAL "" FORCE)
ELSE ( BOOST_1 AND BOOST_2 AND BOOST_3 )
  MESSAGE("-- Looking for Boost headers- not found")
  SET(BOOST_SUPPORT FALSE CACHE INTERNAL "" FORCE)
ENDIF ( BOOST_1 AND BOOST_2 AND BOOST_3 )

# Look for Xerces
SET(XERCES ${XERCES} INTERNAL "if xerces is found")
FIND_LIBRARY(XERCES NAMES xerces-c 
  PATHS /usr/local/lib /usr/lib ${XERCES_INCLUDE_DIR}/../lib)
IF ( XERCES )
  MESSAGE("-- Looking for Xerces - found")
  SET(XERCES_SUPPORT TRUE CACHE INTERNAL "" FORCE)
ELSE ( XERCES )
  MESSAGE("-- Looking for Xerces - not found")
  SET(XERCES_SUPPORT FALSE CACHE INTERNAL "" FORCE)
ENDIF ( XERCES )

SET( OROCOS_TARGET gnulinux CACHE STRING "The Operating System target. One of [lxrt gnulinux xenomai]")
SET(LINUX_SOURCE_DIR ${LINUX_SOURCE_DIR} CACHE PATH "path to linux source dir" FORCE)

# Look for TAO and ACE
INCLUDE(${PROJ_SOURCE_DIR}/config/FindCorbaDeps.cmake)
IF(TAO_FOUND)
  #EXEC_PROGRAM(tao_idl)
ENDIF(TAO_FOUND)


IF(OROCOS_TARGET STREQUAL "lxrt")
  # Look for LXRT
  SET(RTAI_INSTALL_DIR ${RTAI_INSTALL_DIR} CACHE PATH "path to rtai installation dir" FORCE)
  CHECK_INCLUDE_FILE_CXX(LXRT ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h -I${RTAI_INSTALL_DIR}/include -I${LINUX_SOURCE_DIR}/include)
  MESSAGE("Checking ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h: ${LXRT}")
  IF(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
  #IF(LXRT)
    MESSAGE("-- Looking for LXRT - found")
    SET(RTAI_SUPPORT TRUE CACHE INTERNAL "" FORCE)
    INCLUDE_DIRECTORIES(${RTAI_INSTALL_DIR}/include ${LINUX_SOURCE_DIR}/include)
  #ELSE(LXRT)
  ELSE(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
    MESSAGE("-- Looking for LXRT - not found")
    SET(RTAI_SUPPORT FALSE CACHE INTERNAL "" FORCE)
  #ENDIF(LXRT)
  ENDIF(EXISTS ${RTAI_INSTALL_DIR}/include/rtai_lxrt.h)
ELSE(OROCOS_TARGET STREQUAL "lxrt")
  SET(RTAI_INSTALL_DIR "/usr/realtime" CACHE INTERNAL "path to rtai installation dir")
ENDIF(OROCOS_TARGET STREQUAL "lxrt")


IF(OROCOS_TARGET STREQUAL "xenomai")
  # Look for Xenomai
  SET(XENOMAI_INSTALL_DIR ${XENOMAI_INSTALL_DIR} CACHE PATH "path to xenomai installation dir" FORCE)
  IF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native.h)
    MESSAGE("-- Looking for XENOMAI - found")
    SET(XENOMAI_SUPPORT TRUE CACHE INTERNAL FORCE)
  ELSE(EXISTS ${XENOMAI_INSTALL_DIR}/include/native.h)
    MESSAGE("-- Looking for XENOMAI - not found")
    SET(XENOMAI_SUPPORT FALSE CACHE INTERNAL FORCE)
  ENDIF(EXISTS ${XENOMAI_INSTALL_DIR}/include/native.h)
ELSE(OROCOS_TARGET STREQUAL "xenomai")
  SET(XENOMAI_INSTALL_DIR "/usr/realtime" CACHE INTERNAL "path to xenomai installation dir")
ENDIF(OROCOS_TARGET STREQUAL "xenomai")

#Check machine type:
IF(UNIX)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL "processor type (i386 and x86_64)")
  ELSE(CMAKE_UNAME)
    MESSAGE(SEND_ERROR "Can not determine machine-type")
  ENDIF(CMAKE_UNAME)
ELSE(UNIX)
  MESSAGE(SEND_ERROR "Machine-type determination only for unix types")
ENDIF(UNIX)

