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
  IF( CXX_VERSION MATCHES "4\\.[0-9]\\.[0-9]" )
    MESSAGE("Detected gcc4: ${CXX_VERSION}")
    #SET(RTT_CXXFLAGS "-fvisibility-inlines-hidden")
    SET(RTT_GCC_HASVISIBILITY TRUE)
  ELSE(CXX_VERSION MATCHES "4\\.[0-9]\\.[0-9]")
    IF( CXX_VERSION MATCHES "3\\.[0-9]\\.[0-9]" )
      MESSAGE("Detected gcc3: ${CXX_VERSION}")
    ELSE( CXX_VERSION MATCHES "3\\.[0-9]\\.[0-9]" )
      MESSAGE("ERROR: You seem to be using gcc version:")
      MESSAGE("${CXX_VERSION}")
      MESSAGE( FATAL_ERROR "ERROR: For gcc, Orocos requires version 4.x or 3.x")
    ENDIF( CXX_VERSION MATCHES "3\\.[0-9]\\.[0-9]" )
  ENDIF(CXX_VERSION MATCHES "4\\.[0-9]\\.[0-9]")
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

#
# STATIC or SHARED
#
OPTION( BUILD_STATIC "Build Orocos RTT as a static library." ${FORCE_BUILD_STATIC})

#
# CORBA
#
DEPENDENT_OPTION( ENABLE_CORBA "Enable CORBA (using TAO)" OFF "ACE_CONFIG AND TAO_ORB AND TAO_ORBSVCS;NOT ORO_EMBEDDED" OFF)
#IF (ENABLE_CORBA)
  # Finally:
#  IF (BUILD_STATIC)
#    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lorocos-rtt-corba -lorocos-rtt -lTAO -lTAO_IDL_BE -lTAO_PortableServer -lTAO_CosNaming -lACE" CACHE INTERNAL "")
#  ELSE(BUILD_STATIC)
    # shared.
#  ENDIF (BUILD_STATIC)

#ENDIF (ENABLE_CORBA)


