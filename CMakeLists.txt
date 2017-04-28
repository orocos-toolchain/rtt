#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# for CMake 2.6 corrected behaviour (see "cmake --help-policy CMP0003")
IF(POLICY CMP0003)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(POLICY CMP0003)

# Suppress cmake policy warning in included rtt_macros.cmake file in CMake >= 3.0.2 (see "cmake --help-policy CMP0026")
if(POLICY CMP0026)
  cmake_policy(SET CMP0026 OLD)
endif()

###################################################
#                                                 #
#       Start project customization section       #
#                                                 #
###################################################

PROJECT(orocos-rtt)


SET( RTT_VERSION 2.9.0 )
STRING( REGEX MATCHALL "[0-9]+" RTT_VERSIONS ${RTT_VERSION} )
LIST( GET RTT_VERSIONS 0 RTT_VERSION_MAJOR)
LIST( GET RTT_VERSIONS 1 RTT_VERSION_MINOR)
LIST( GET RTT_VERSIONS 2 RTT_VERSION_PATCH)
SET( RTT_SOVERSION ${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR} )
MESSAGE( "Orocos RTT version ${RTT_VERSION}" )

SET( PROJ_SOURCE_DIR ${orocos-rtt_SOURCE_DIR} )
SET( PROJ_BINARY_DIR ${orocos-rtt_BINARY_DIR} )

# Load the optional orocos-rtt.cmake file.
INCLUDE(${PROJ_SOURCE_DIR}/orocos-rtt.cmake OPTIONAL)
if (NOT EXISTS ${PROJ_SOURCE_DIR}/orocos-rtt.cmake)
  # Setup the default settings
  MESSAGE(
    "No orocos-rtt.cmake file loaded, using default settings."
    "See orocos-rtt.default.cmake")
  INCLUDE(${PROJ_SOURCE_DIR}/orocos-rtt.default.cmake)
endif ()

# On Windows, the default CMAKE_INSTALL_PREFIX is either:
# C:\Program Files\orocos-rtt or C:\Program Files (x86)\orocos-rtt
# However, we want it to be either:
# C:\Program Files\orocos or C:\Program Files (x86)\orocos
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  IF (MSVC)
    STRING(REPLACE "${PROJECT_NAME}" "orocos" OROCOS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    SET(CMAKE_INSTALL_PREFIX
      ${OROCOS_INSTALL_PREFIX} CACHE PATH "Orocos install prefix" FORCE
      )
  ENDIF(MSVC)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Export file name
set(LIBRARY_EXPORT_FILE "orocos-rtt-${OROCOS_TARGET}-libraries")

# The installation dir suffix for the 'orocos' directory
if ( NOT OROCOS_SUFFIX )
  set(OROCOS_SUFFIX /${OROCOS_TARGET}) # defaults to the target name
endif( NOT OROCOS_SUFFIX )

# Make compilation verbose
#SET(CMAKE_VERBOSE_MAKEFILE TRUE)

###################################################
#                                                 #
#       End project customization section         #
#                                                 #
###################################################

OPTION(ENABLE_TESTS "Turn on to enable the testing framework." OFF)
SET(BUILD_TEST_TIMEOUT 1500 CACHE STRING "Global timeout on all tests (seconds).")
IF(ENABLE_TESTS)
  SET(DART_TESTING_TIMEOUT "${BUILD_TEST_TIMEOUT}" CACHE STRING "" FORCE)
  INCLUDE(CTest)
  ENABLE_TESTING()
ENDIF()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

# turn on code coverage of tests
include (CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(BUILD_ENABLE_COVERAGE "Turn on code coverage of all tests." OFF "ENABLE_TESTS" OFF)
IF (BUILD_ENABLE_COVERAGE)
  # coverage requires test
  SET(ENABLE_TESTS ON CACHE BOOL "" FORCE)
ENDIF (BUILD_ENABLE_COVERAGE)

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

# Main package configuration file + target specific file
CONFIGURE_FILE(orocos-rtt-config.cmake.in
               orocos-rtt-config-${OROCOS_TARGET}.cmake @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/orocos-rtt-config.cmake 
              ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-config-${OROCOS_TARGET}.cmake
        DESTINATION "${CONFIG_FILE_PATH}")

# Version file
CONFIGURE_FILE(orocos-rtt-config-version.cmake.in
               orocos-rtt-config-version.cmake @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-config-version.cmake DESTINATION "${CONFIG_FILE_PATH}")

# Exported targets
INSTALL(EXPORT ${LIBRARY_EXPORT_FILE} DESTINATION "${CONFIG_FILE_PATH}"
                                      NAMESPACE   "${IMPORTED_TARGET_PREFIX}")

# Package use file + helper files
INSTALL(FILES UseOROCOS-RTT.cmake UseOROCOS-RTT-helpers.cmake config/cmake_uninstall.cmake.in DESTINATION "${CONFIG_FILE_PATH}")

# Install package.xml
INSTALL(FILES package.xml DESTINATION share/rtt)

# Install an env-hook in etc/orocos/${OROCOS_TARGET}/profile.d
configure_file(env-hooks/00.rtt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/env-hooks/00.rtt.sh @ONLY)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/env-hooks/00.rtt.sh
  DESTINATION etc/orocos/profile.d
)

# Install a catkin env-hook if catkin is installed
find_package(catkin QUIET)
if(catkin_FOUND)
  catkin_add_env_hooks(00.rtt SHELLS sh DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/env-hooks)
endif()
