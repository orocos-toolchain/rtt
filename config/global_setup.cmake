#
# Include cmake modules required to look for dependencies
#
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )

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
  MESSAGE( STATUS "Found Doxygen -- documentation can be built" )

  OPTION( GENERATE_DOCUMENTATION "Build Documentation" OFF )

ELSE ( DOXYGEN )
  MESSAGE( STATUS "Doxygen not found -- unable to build documentation" )
ENDIF ( DOXYGEN )


#
# An option for tests, to make it easy to turn off all tests
#
OPTION( BUILD_TESTS "Turn me off to disable compilation of all tests" OFF )

