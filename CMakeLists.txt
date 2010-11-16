#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

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

SET( RTT_VERSION 2.1.1 )
STRING( REGEX MATCHALL "[0-9]+" RTT_VERSIONS ${RTT_VERSION} )
LIST( GET RTT_VERSIONS 0 RTT_VERSION_MAJOR)
LIST( GET RTT_VERSIONS 1 RTT_VERSION_MINOR)
LIST( GET RTT_VERSIONS 2 RTT_VERSION_PATCH)

SET(ROS_ROOT $ENV{ROS_ROOT})

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

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  IF (MSVC)
    SET(CMAKE_INSTALL_PREFIX
      "C:/Program Files/orocos" CACHE PATH "Orocos install prefix" FORCE
      )
  ENDIF(MSVC)
  IF(ROS_ROOT)
    SET(CMAKE_INSTALL_PREFIX
      ${PROJECT_SOURCE_DIR}/install CACHE PATH "Orocos install prefix for ROS builds" FORCE
      )
  ENDIF(ROS_ROOT)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#Use these variables to store build-local flags.
#They are used when the targets are configured.
IF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")
  SET ( CMAKE_CXX_FLAGS_ADD "${CMAKE_CXX_FLAGS}" CACHE STRING "Specify here your additional C++ flags.")
  SET ( CMAKE_C_FLAGS_ADD "${CMAKE_C_FLAGS}"  CACHE STRING "Specify here your additional C flags.")
  SET ( CMAKE_LD_FLAGS_ADD "${CMAKE_LD_FLAGS}"  CACHE STRING "Specify here your additional link flags.")
ENDIF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")

# Export file name
set(LIBRARY_EXPORT_FILE "orocos-rtt-${OROCOS_TARGET}-libraries")

# Make compilation verbose
#SET(CMAKE_VERBOSE_MAKEFILE TRUE)

###################################################
#                                                 #
#       End project customization section         #
#                                                 #
###################################################

OPTION(ENABLE_TESTS "Turn on to enable the testing framework." ON)
IF(ENABLE_TESTS)
  INCLUDE(CTest)
  ENABLE_TESTING()
ENDIF()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
ADD_SUBDIRECTORY(rtt)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(tests)

#############################
#                           #
# Building Doxygen documents#
#                           #
#############################

ADD_CUSTOM_TARGET(docs echo "Documentation built.")
ADD_DEPENDENCIES(docs dochtml docpdf)

IF (DOXYGEN_EXECUTABLE)
  ADD_CUSTOM_TARGET(docapi ${DOXYGEN_EXECUTABLE} "Doxyfile")
  ADD_DEPENDENCIES(docs docapi)
  CONFIGURE_FILE(Doxyfile.in Doxyfile @ONLY)
ENDIF (DOXYGEN_EXECUTABLE)

###############################
#                             #
# Package configuration files #
#                             #
###############################

set(CONFIG_FILE_PATH "lib/cmake/orocos-rtt") # Folder lib/cmake is searched by default in CMake >= 2.6.3
set(IMPORTED_TARGET_PREFIX "imported_") # Prefix used in other projects to distinguish imported targets from regular ones

# Main package configuration file
CONFIGURE_FILE(orocos-rtt-config.cmake.in
               orocos-rtt-config.cmake @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-config.cmake DESTINATION "${CONFIG_FILE_PATH}")

# Version file
CONFIGURE_FILE(orocos-rtt-config-version.cmake.in
               orocos-rtt-config-version.cmake @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-config-version.cmake DESTINATION "${CONFIG_FILE_PATH}")

# Exported targets
INSTALL(EXPORT ${LIBRARY_EXPORT_FILE} DESTINATION "${CONFIG_FILE_PATH}"
                                      NAMESPACE   "${IMPORTED_TARGET_PREFIX}")

# Package use file + helper files
INSTALL(FILES UseOROCOS-RTT.cmake config/cmake_uninstall.cmake.in DESTINATION "${CONFIG_FILE_PATH}")
