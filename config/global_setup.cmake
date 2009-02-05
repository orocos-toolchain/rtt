#
# SET PROJECT WIDE OPTIONS, INCLUDE DIRECTORIES ETC.
#
# Prefer to prefix with ENABLE_X Check dependencies with a FindX.cmake file.
#

#
# Include cmake modules required to look for dependencies
#
INCLUDE( config/DependentOption.cmake )

SET(CMAKE_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
MESSAGE("CMAKE_VERSION: ${CMAKE_VERSION}")

#
# If we're using gcc, make sure the version is OK.
#
SET(RTT_CXXFLAGS "")
# this is a workaround distcc:
IF ( CMAKE_CXX_COMPILER_ARG1 )
  STRING(REPLACE " " "" CMAKE_CXX_COMPILER_ARG1 ${CMAKE_CXX_COMPILER_ARG1} )
  #MESSAGE("Invoking: '${CMAKE_CXX_COMPILER_ARG1} -dumpversion'")
  EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER_ARG1} -dumpversion RESULT_VARIABLE CXX_HAS_VERSION OUTPUT_VARIABLE CXX_VERSION)
ELSE ( CMAKE_CXX_COMPILER_ARG1 )
  #MESSAGE("Invoking: ${CMAKE_CXX_COMPILER} -dumpversion")
  EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion RESULT_VARIABLE CXX_HAS_VERSION OUTPUT_VARIABLE CXX_VERSION)
ENDIF ( CMAKE_CXX_COMPILER_ARG1 )

IF ( ${CXX_HAS_VERSION} EQUAL 0 )
  # We are assuming here that -dumpversion is gcc specific.
  IF( CXX_VERSION MATCHES "4\\.[0-9](\\.[0-9])?" )
    MESSAGE("Detected gcc4: ${CXX_VERSION}")
    #SET(RTT_CXXFLAGS "-fvisibility-inlines-hidden")
    SET(RTT_GCC_HASVISIBILITY TRUE)
  ELSE(CXX_VERSION MATCHES "4\\.[0-9](\\.[0-9])?")
    IF( CXX_VERSION MATCHES "3\\.[0-9](\\.[0-9])?" )
      MESSAGE("Detected gcc3: ${CXX_VERSION}")
    ELSE( CXX_VERSION MATCHES "3\\.[0-9](\\.[0-9])?" )
      MESSAGE("ERROR: You seem to be using gcc version:")
      MESSAGE("${CXX_VERSION}")
      MESSAGE( FATAL_ERROR "ERROR: For gcc, Orocos requires version 4.x or 3.x")
    ENDIF( CXX_VERSION MATCHES "3\\.[0-9](\\.[0-9])?" )
  ENDIF(CXX_VERSION MATCHES "4\\.[0-9](\\.[0-9])?")
ELSE ( ${CXX_HAS_VERSION} EQUAL 0)
  MESSAGE("Could not determine gcc version: ${CXX_HAS_VERSION}")
ENDIF ( ${CXX_HAS_VERSION} EQUAL 0)

#
# Check for Doxygen and enable documentation building
#
INCLUDE( ${CMAKE_ROOT}/Modules/FindDoxygen.cmake )
IF ( DOXYGEN )
  MESSAGE( STATUS "Found Doxygen -- API documentation can be built" )
ELSE ( DOXYGEN )
  MESSAGE( STATUS "Doxygen not found -- unable to build documentation" )
ENDIF ( DOXYGEN )


#
# An option for tests, to make it easy to turn off all tests
#
OPTION( ENABLE_TESTS "Turn me off to disable compilation of all tests" OFF )
IF( ENABLE_TESTS )
  IF (NOT CMAKE_CROSS_COMPILE )
    FIND_LIBRARY(CPPUNIT NAMES cppunit PATHS /usr/local/lib /usr/lib )
    FIND_PATH( CPPUNIT_HEADERS cppunit/TestRunner.h)
  ELSE (NOT CMAKE_CROSS_COMPILE )
    FIND_LIBRARY(CPPUNIT NAMES cppunit NO_DEFAULT_PATH )
  ENDIF (NOT CMAKE_CROSS_COMPILE )
  IF ( CPPUNIT AND CPPUNIT_HEADERS)
    MESSAGE("-- Looking for Cppunit - found")
  ELSE ( CPPUNIT AND CPPUNIT_HEADERS )
    MESSAGE( FATAL_ERROR "-- Looking for Cppunit - not found")
  ENDIF ( CPPUNIT AND CPPUNIT_HEADERS )
ENDIF(ENABLE_TESTS )

#
# STATIC or SHARED
#
OPTION( BUILD_STATIC "Build Orocos RTT as a static library." ${FORCE_BUILD_STATIC})

#
# CORBA
#
OPTION( ENABLE_CORBA "Enable CORBA" OFF)
MESSAGE(${CORBA_IMPLEMENTATION})
IF (NOT CORBA_IMPLEMENTATION)
    SET( CORBA_IMPLEMENTATION TAO CACHE STRING "the implementation of corba to use (TAO or OMNIORB)" )
ENDIF (NOT CORBA_IMPLEMENTATION)

if (ENABLE_CORBA)
    IF(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        # Look for TAO and ACE
        INCLUDE(${PROJ_SOURCE_DIR}/config/FindTAO.cmake)
        IF(NOT FOUND_TAO)
            MESSAGE(FATAL_ERROR "cannot find TAO")
        ENDIF(NOT FOUND_TAO)
    ELSEIF(${CORBA_IMPLEMENTATION} STREQUAL "OMNIORB")
        INCLUDE(${PROJ_SOURCE_DIR}/config/FindOmniORB.cmake)
        IF(NOT OMNIORB4_FOUND)
            MESSAGE(FATAL_ERROR "cannot find OmniORB4")
        ENDIF(NOT OMNIORB4_FOUND)
    ELSE(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        MESSAGE(FATAL_ERROR "unknown CORBA implementation ${CORBA_IMPLEMENTATION}")
    ENDIF(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
endif (ENABLE_CORBA)

OPTION( ORO_REMOTING "Enable transparant Remote Methods and Commands in C++" ${ENABLE_CORBA} )
# Force remoting when CORBA is enabled.
IF ( ENABLE_CORBA AND NOT ORO_REMOTING )
  MESSAGE( "Forcing ORO_REMOTING to ON")
  SET( ORO_REMOTING ON CACHE BOOL "Enable transparant Remote Methods and Commands in C++" FORCE)
ENDIF( ENABLE_CORBA AND NOT ORO_REMOTING )


#IF (ENABLE_CORBA)
  # Finally:
#  IF (BUILD_STATIC)
#    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lorocos-rtt-corba -lorocos-rtt -lTAO -lTAO_IDL_BE -lTAO_PortableServer -lTAO_CosNaming -lACE" CACHE INTERNAL "")
#  ELSE(BUILD_STATIC)
    # shared.
#  ENDIF (BUILD_STATIC)

#ENDIF (ENABLE_CORBA)


