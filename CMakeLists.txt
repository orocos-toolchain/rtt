#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.2)
#MARK_AS_ADVANCED( FORCE CMAKE_BACKWARDS_COMPATIBILITY )

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

SET( RTT_VERSION 1.6.99 )
STRING( REGEX MATCHALL "[0-9]+" RTT_VERSIONS ${RTT_VERSION} )
LIST( GET RTT_VERSIONS 0 RTT_VERSION_MAJOR)
LIST( GET RTT_VERSIONS 1 RTT_VERSION_MINOR)
LIST( GET RTT_VERSIONS 2 RTT_VERSION_PATCH)

MESSAGE( "Orocos RTT version ${VERSION} (${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}.${RTT_VERSION_PATCH})" )

# Do not rely on environment variables to find the path, because that's confusing
IF(NOT CMAKE_INSTALL_PREFIX)
  SET( CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "Installation directory" FORCE)
  MESSAGE( STATUS "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )
ENDIF(NOT CMAKE_INSTALL_PREFIX)

SET( PROJ_SOURCE_DIR ${orocos-rtt_SOURCE_DIR} )
SET( PROJ_BINARY_DIR ${orocos-rtt_BINARY_DIR} )

# Set the default build type to debug.
#
# Set the build type (affects debugging symbols and optimization)
#
IF ( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE "RTT" CACHE STRING "Build type: None (Use CMAKE_C_FLAGS and CMAKE_CXX_FLAGS), RTT (Recommended), Release, Debug, RelWithDebInfo, MinSizeRel." FORCE )
  MESSAGE( "Setting build type to '${CMAKE_BUILD_TYPE}'" )
  SET( CMAKE_C_FLAGS_RTT "-O2 -DNDEBUG" CACHE STRING "The CFLAGS for RTT" )
  SET( CMAKE_CXX_FLAGS_RTT "-O2 -DNDEBUG" CACHE STRING "The CXXFLAGS flags for RTT"  )
ELSE ( NOT CMAKE_BUILD_TYPE )
  MESSAGE( "Build type set to '${CMAKE_BUILD_TYPE}' by user." )
  IF ( NOT CMAKE_BUILD_TYPE STREQUAL "RTT" )
    MARK_AS_ADVANCED(FORCE CMAKE_C_FLAGS_RTT CMAKE_CXX_FLAGS_RTT)
  ELSE ( NOT CMAKE_BUILD_TYPE STREQUAL "RTT" )
    MARK_AS_ADVANCED(CLEAR CMAKE_C_FLAGS_RTT CMAKE_CXX_FLAGS_RTT)
  ENDIF ( NOT CMAKE_BUILD_TYPE STREQUAL "RTT" )

ENDIF ( NOT CMAKE_BUILD_TYPE )

#Use these variables to store build-local flags.
#They are used when the targets are configured.
IF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")
  SET ( CMAKE_CXX_FLAGS_ADD "${CMAKE_CXX_FLAGS}" CACHE STRING "Specify here your additional flags.")
  SET ( CMAKE_C_FLAGS_ADD "${CMAKE_C_FLAGS}"  CACHE STRING "Specify here your additional flags.")
ENDIF ( NOT CMAKE_BUILD_TYPE STREQUAL "None")


# Make compilation verbose
#SET(CMAKE_VERBOSE_MAKEFILE TRUE)

###################################################
#                                                 #
#       End project customization section         #
#                                                 #
###################################################

#
# Set the build type (affects debugging symbols and optimization)
#
IF ( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE Release )
  MESSAGE( STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE ( NOT CMAKE_BUILD_TYPE )
  MESSAGE( STATUS "Build type set to '${CMAKE_BUILD_TYPE}' by user." )
ENDIF ( NOT CMAKE_BUILD_TYPE )

# MESSAGE("Looking for compiler:")
INCLUDE (${CMAKE_ROOT}/Modules/CMakeDetermineCXXCompiler.cmake)

###################################################
#                                                 #
#       Do some global set-up & checking          #
#                                                 #
###################################################

# Put your 'flags' inhere if you want them to apear in the
# rtt.pc file. Various .cmake scripts below do
# this. These flags should ONLY by -I or -D flags, NOT -O.. 
# -W... or alike. The user's application is compiled with these flags !
SET( RTT_CFLAGS "" CACHE INTERNAL "")
SET( RTT_LINKFLAGS "" CACHE INTERNAL "")
SET( RTT_USER_LINKFLAGS "" CACHE INTERNAL "")

INCLUDE(config/check_depend.cmake)
INCLUDE(config/global_setup.cmake)

## Uninstall target
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/config/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

INCLUDE(config/rtt_macros.cmake)

###########################################################
#                                                         #
# Create build                                            #
#                                                         #
###########################################################

ADD_DEFINITIONS( "-Wall" )
INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/src ${PROJ_BINARY_DIR}/src/os ${PROJ_BINARY_DIR}/src/os/${OROCOS_TARGET})
#INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/src ${PROJ_BINARY_DIR}/src/os)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(tests)

#CONFIGURE_FILE( orocos-rtt.pc.in orocos-rtt-${OROCOS_TARGET}.pc @ONLY)
#INSTALL_FILES( /lib/pkgconfig FILES orocos-rtt-${OROCOS_TARGET}.pc)

############################################################
##                                                         #
## Setup PACKAGING                                         #
##                                                         #
############################################################

IF ( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.4" )
OPTION( CPACK_PACKAGES "Set to ON to build the packages. Requires cmake >2.4" OFF )
IF (CPACK_PACKAGES)

INCLUDE(InstallRequiredSystemLibraries)

# ATTENTION: There is sometimes a _SOURCE_ version of an
# option as well, set both if necessary !

# Create .tar.gz and .tar.tbz2 files:
SET(CPACK_GENERATOR "TBZ2")
SET(CPACK_SOURCE_GENERATOR "TBZ2")

# The plain 'package' target works correctly.
SET(CPACK_IGNORE_FILES        "/CVS/;/.svn/;.swp$;.#;/#;/build/")
# Since the 'package_source' target does a bold copy, define a list of
# files which should be excluded. Note that 'subpattern' matching is used,
# thus to exclude a directory use /mydir/
SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;.swp$;.#;/#;/build/;~")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Orocos RTT")
SET(CPACK_PACKAGE_VENDOR "The Orocos Community")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_MAJOR_VERSION RTT_VERSION_MAJOR)
SET(CPACK_PACKAGE_MINOR_VERSION RTT_VERSION_MINOR)
SET(CPACK_PACKAGE_VERSION_PATCH RTT_VERSION_PATCH)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMAKE ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
SET(CPACK_STRIP_FILES "bin/cmake")
SET(CPACK_PACKAGE_EXECUTABLES "OrocosExec" "Orocos Executable")
INCLUDE(CPack)
ENDIF (CPACK_PACKAGES)
ELSE ( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.4" )

   MESSAGE ( "Disabling packaging for version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")

ENDIF ( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.4" )


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

  # provide a link to src. Used by Doxygen.
  EXECUTE_PROCESS(COMMAND ln -nfs ${PROJ_SOURCE_DIR}/src ${PROJ_BINARY_DIR}/rtt ERROR_QUIET)

ENDIF (DOXYGEN)


