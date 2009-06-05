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
    MESSAGE(STATUS "Detected gcc4: ${CXX_VERSION}")
    SET(RTT_GCC_HASVISIBILITY TRUE)
  ELSE(CXX_VERSION MATCHES "4\\.[0-9](\\.[0-9])?")
    IF( CXX_VERSION MATCHES "3\\.[0-9](\\.[0-9])?" )
      MESSAGE(STATUS "Detected gcc3: ${CXX_VERSION}")
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
find_package( Doxygen )
IF ( DOXYGEN_EXECUTABLE )
  MESSAGE( STATUS "Found Doxygen -- API documentation can be built" )
ELSE ( DOXYGEN_EXECUTABLE )
  MESSAGE( STATUS "Doxygen not found -- unable to build documentation" )
ENDIF ( DOXYGEN_EXECUTABLE )


#
# An option for tests, to make it easy to turn off all tests
#
OPTION( ENABLE_TESTS "Turn me off to disable compilation of all tests" OFF )

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
ENDIF (NOT CORBA_IMPLEMENTATION)

if (ENABLE_CORBA)
    IF(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        # Look for TAO and ACE
	find_package(ACE REQUIRED)
        find_package(TAO REQUIRED IDL PortableServer CosNaming)
        IF(NOT TAO_FOUND)
            MESSAGE(FATAL_ERROR "Cannot find TAO")
        ELSE(NOT TAO_FOUND)
            MESSAGE(STATUS "CORBA enabled: ${TAO_FOUND_COMPONENTS}")

	    # Copy flags:
            SET(CORBA_INCLUDE_DIRS ${TAO_INCLUDE_DIRS})
            SET(CORBA_LIBRARIES ${TAO_LIBRARIES})
	    SET(CORBA_DEFINITIONS ${TAO_DEFINITIONS})

        ENDIF(NOT TAO_FOUND)
    ELSEIF(${CORBA_IMPLEMENTATION} STREQUAL "OMNIORB")
        INCLUDE(${PROJ_SOURCE_DIR}/config/FindOmniORB.cmake)
        IF(NOT OMNIORB4_FOUND)
            MESSAGE(FATAL_ERROR "cannot find OmniORB4")
        ELSE(NOT OMNIORB4_FOUND)
            MESSAGE(STATUS "CORBA enabled: OMNIORB")

	    # Copy flags:
	    SET(CORBA_LIBRARIES ${OMNIORB4_LIBRARIES})
	    SET(CORBA_CFLAGS ${OMNIORB4_CPP_FLAGS})
	    SET(CORBA_INCLUDE_DIRS ${OMNIORB4_INCLUDE_DIR})
	    SET(CORBA_DEFINITIONS ${OMNIORB4_DEFINITIONS})

        ENDIF(NOT OMNIORB4_FOUND)
    ELSE(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        MESSAGE(FATAL_ERROR "Unknown CORBA implementation '${CORBA_IMPLEMENTATION}': must be TAO or OMNIORB.")
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


