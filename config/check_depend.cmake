###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

# Modules path (for searching FindXXX.cmake files)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config")

# Look for boost
find_package(Boost 1.32 COMPONENTS program_options REQUIRED)

if(Boost_FOUND)
  list(APPEND OROCOS-RTT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} )
  list(APPEND OROCOS-RTT_LIBRARIES ${Boost_LIBRARIES} ) 
endif(Boost_FOUND)

# Look for Xerces 

# If a nonstandard path is used when crosscompiling, uncomment the following lines
# IF(NOT CMAKE_CROSS_COMPILE) # NOTE: There now exists a standard CMake variable named CMAKE_CROSSCOMPILING
#   set(XERCES_ROOT_DIR /path/to/xerces CACHE INTERNAL "" FORCE) # you can also use set(ENV{XERCES_ROOT_DIR} /path/to/xerces)
# ENDIF(NOT CMAKE_CROSS_COMPILE)

find_package(Xerces)

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
set( OROCOS_TARGET gnulinux CACHE STRING "The Operating System target. One of [lxrt gnulinux xenomai macosx]")
string(TOUPPER ${OROCOS_TARGET} OROCOS_TARGET_CAP)
message("Orocos target is ${OROCOS_TARGET}")

# Setup flags for RTAI/LXRT
if(OROCOS_TARGET STREQUAL "lxrt")
  set(OROPKG_OS_LXRT TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)
  set(LINUX_SOURCE_DIR ${LINUX_SOURCE_DIR} CACHE PATH "Path to Linux source dir (required for lxrt target)" FORCE)

  find_package(RTAI REQUIRED)

  if(RTAI_FOUND)
    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${RTAI_INCLUDE_DIRS} )
    list(APPEND OROCOS-RTT_LIBRARIES ${RTAI_LIBRARIES} pthread dl) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
  endif()
else()
  set(OROPKG_OS_LXRT FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for Xenomai
if(OROCOS_TARGET STREQUAL "xenomai")
  set(OROPKG_OS_XENOMAI TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)

  find_package(Xenomai REQUIRED)

  if(XENOMAI_FOUND)
    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${XENOMAI_INCLUDE_DIRS} )
    list(APPEND OROCOS-RTT_LIBRARIES ${XENOMAI_LIBRARIES} pthread dl) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
  endif()
else()
  set(OROPKG_OS_XENOMAI FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for GNU/Linux
if(OROCOS_TARGET STREQUAL "gnulinux")
  set(OROPKG_OS_GNULINUX TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)

  list(APPEND OROCOS-RTT_LIBRARIES pthread dl rt) 
  list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
else()
  set(OROPKG_OS_GNULINUX FALSE CACHE INTERNAL "" FORCE)
endif()

# Setup flags for Mac-OSX
if(OROCOS_TARGET STREQUAL "macosx")
  set(OROPKG_OS_MACOSX TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)

  list(APPEND OROCOS-RTT_LIBRARIES pthread dl) 
  list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 
else()
  set(OROPKG_OS_MACOSX FALSE CACHE INTERNAL "" FORCE)
endif()


# Setup flags for ecos
IF(OROCOS_TARGET STREQUAL "ecos")
  set(OROPKG_OS_ECOS TRUE CACHE INTERNAL "This variable is exported to the rtt-config.h file to expose our target choice to the code." FORCE)

  # We can't really use 'UseEcos.cmake' because we're building a library
  # and not a final application
  find_package(Ecos REQUIRED)

  if(Ecos_FOUND)

    set(ECOS_SUPPORT TRUE CACHE INTERNAL "" FORCE)

    list(APPEND OROCOS-RTT_INCLUDE_DIRS ${ECOS_INCLUDE_DIRS} )
    list(APPEND OROCOS-RTT_LIBRARIES ${ECOS_LIBRARIES} pthread dl) 
    list(APPEND OROCOS-RTT_DEFINITIONS "OROCOS_TARGET=${OROCOS_TARGET}") 

    message( "Turning BUILD_STATIC ON for ecos.")
    set( FORCE_BUILD_STATIC ON CACHE INTERNAL "Forces to build Orocos RTT as a static library (forced to ON by Ecos)" FORCE)
    set( BUILD_STATIC ON CACHE BOOL "Build Orocos RTT as a static library (forced to ON by Ecos)" FORCE)
  endif()
else()
  set(OROPKG_OS_ECOS FALSE CACHE INTERNAL "" FORCE)
endif()


# The machine type is tested using compiler macros in rtt-config.h.in
