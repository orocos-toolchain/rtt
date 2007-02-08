#
# SET PROJECT WIDE OPTIONS, INCLUDE DIRECTORIES ETC.
#
# Prefer to prefix with ENABLE_X Check dependencies with a FindX.cmake file.
#

#
# Include cmake modules required to look for dependencies
#
INCLUDE( config/DependentOption.cmake )

#
# If we're using gcc, make sure the version is OK.
#
#STRING( REGEX MATCH gcc USING_GCC ${CMAKE_C_COMPILER} )
# IF (  ${CMAKE_C_COMPILER} MATCHES gcc )
#   EXEC_PROGRAM( ${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE CMAKE_C_COMPILER_VERSION )
#   # Why doesn't this work?
#   #STRING( REGEX MATCHALL "gcc\.*" VERSION_STRING ${CMAKE_C_COMPILER} )
#   IF( CMAKE_C_COMPILER_VERSION MATCHES ".*4\\.[0-9]\\.[0-9]" )
#     MESSAGE("gcc version: ${CMAKE_C_COMPILER_VERSION}")
# #   ELSE(CMAKE_C_COMPILER_VERSION MATCHES ".*4\\.[0-9]\\.[0-9]")
# #     MESSAGE("ERROR: You seem to be using gcc version:")
# #     MESSAGE("${CMAKE_C_COMPILER_VERSION}")
# #     MESSAGE( FATAL_ERROR "ERROR: For gcc, Orocos requires version 4.x")
#   ENDIF(CMAKE_C_COMPILER_VERSION MATCHES ".*4\\.[0-9]\\.[0-9]")
# ENDIF (  ${CMAKE_C_COMPILER} MATCHES gcc )


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
# CORBA
#
DEPENDENT_OPTION( ENABLE_CORBA "Enable CORBA (using TAO)" ON "ACE_CONFIG AND TAO_ORB AND TAO_ORBSVCS;NOT ORO_EMBEDDED" OFF)
IF (ENABLE_CORBA)
  # Add includes / lib paths if necessary
  IF( NOT ${ACE_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ACE_DIR} )
    LINK_DIRECTORIES( ${ACE_DIR}/lib )
  ENDIF( NOT ${ACE_DIR} STREQUAL /usr/include )
  IF( NOT ${TAO_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${TAO_DIR} )
  ENDIF( NOT ${TAO_DIR} STREQUAL /usr/include )
  IF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ORBSVCS_DIR} )
  ENDIF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )

  # Finally:
  LINK_LIBRARIES( TAO TAO_IDL_BE TAO_PortableServer TAO_CosNaming TAO_CosProperty TAO_CosEvent ACE  )
ENDIF (ENABLE_CORBA)

