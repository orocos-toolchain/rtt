#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# for CMake 2.6 corrected behaviour (see "cmake --help-policy CMP0003")
IF(COMMAND cmake_policy AND ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy AND ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)

###################################################
#                                                 #
#       Start project customization section       #
#                                                 #
###################################################

PROJECT(orocos-rtt)

SET( RTT_VERSION 2.0.0 )
STRING( REGEX MATCHALL "[0-9]+" RTT_VERSIONS ${RTT_VERSION} )
LIST( GET RTT_VERSIONS 0 RTT_VERSION_MAJOR)
LIST( GET RTT_VERSIONS 1 RTT_VERSION_MINOR)
LIST( GET RTT_VERSIONS 2 RTT_VERSION_PATCH)

MESSAGE( "Orocos RTT version ${VERSION} (${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}.${RTT_VERSION_PATCH})" )

SET( PROJ_SOURCE_DIR ${orocos-rtt_SOURCE_DIR} )
SET( PROJ_BINARY_DIR ${orocos-rtt_BINARY_DIR} )

# Load the optional orocos-rtt.cmake file.
INCLUDE(${PROJ_SOURCE_DIR}/orocos-rtt.cmake OPTIONAL)
if (NOT EXISTS ${PROJ_SOURCE_DIR}/orocos-rtt.cmake)
  # Setup the default settings
  INCLUDE(${PROJ_SOURCE_DIR}/orocos-rtt.default.cmake)
  MESSAGE(
    "No orocos-rtt.cmake file loaded, using default settings."
    "See orocos-rtt.default.cmake")
endif ()

#Use these variables to store build-local flags.
#They are used when the targets are configured.
IF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")
  SET ( CMAKE_CXX_FLAGS_ADD "${CMAKE_CXX_FLAGS}" CACHE STRING "Specify here your additional C++ flags.")
  SET ( CMAKE_C_FLAGS_ADD "${CMAKE_C_FLAGS}"  CACHE STRING "Specify here your additional C flags.")
  SET ( CMAKE_LD_FLAGS_ADD "${CMAKE_LD_FLAGS}"  CACHE STRING "Specify here your additional link flags.")
ENDIF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")

# Make compilation verbose
SET(CMAKE_VERBOSE_MAKEFILE TRUE)

###################################################
#                                                 #
#       End project customization section         #
#                                                 #
###################################################

# MESSAGE("Looking for compiler:")
INCLUDE (${CMAKE_ROOT}/Modules/CMakeDetermineCXXCompiler.cmake)

###################################################
#                                                 #
#       Do some global set-up & checking          #
#                                                 #
###################################################

# Find all installed libraries, compiler etc.
INCLUDE(config/check_depend.cmake)

## Uninstall target
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/config/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

INCLUDE(config/rtt_macros.cmake)

###########################################################
#                                                         #
# Visit subdirs                                           #
#                                                         #
###########################################################

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(tests)

#############################
#                           #
# Building Doxygen documents#
#                           #
#############################

ADD_CUSTOM_TARGET(docs echo "Documentation built.")
ADD_DEPENDENCIES(docs dochtml docpdf)

IF (DOXYGEN)
  ADD_CUSTOM_TARGET(docapi "doxygen" "Doxyfile")
  ADD_DEPENDENCIES(docs docapi)
  CONFIGURE_FILE(Doxyfile.in Doxyfile @ONLY)

  # provide a link to src. Used only by Doxygen.
  EXECUTE_PROCESS(COMMAND create_symlink ${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/rtt ERROR_QUIET)

ENDIF (DOXYGEN)


