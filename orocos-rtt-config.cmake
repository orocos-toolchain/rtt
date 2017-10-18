########################################################################################################################
#
# CMake package configuration file for the OROCOS-RTT package.
# This script imports targets and sets up the variables needed to use the package.
# In case this file is installed in a nonstandard location, its location can be specified using the OROCOS-RTT_DIR cache
# entry.
#
# find_package COMPONENTS represent OROCOS-RTT plugins such as scripting, marshalling or corba-transport.
# The default search path for them is:
#  /path/to/OROCOS-RTTinstallation/lib/orocos/plugins
#  /path/to/OROCOS-RTTinstallation/lib/orocos/types
#
# For this script to find user-defined OROCOS-RTT plugins, the RTT_COMPONENT_PATH environment variable should be
# appropriately set. E.g., if the plugin is located at /path/to/plugins/libfoo-plugin.so,
# then add /path/to to RTT_COMPONENT_PATH
#
# This script sets the following variables:
#  OROCOS-RTT_FOUND: Boolean that indicates if OROCOS-RTT was found
#  OROCOS-RTT_INCLUDE_DIRS: Paths to the necessary header files
#  OROCOS-RTT_LIBRARIES: Libraries to link against to use OROCOS-RTT
#  OROCOS-RTT_LIBRARY_DIRS: Link directories for OROCOS-RTT
#  OROCOS-RTT_DEFINITIONS: Definitions to use when compiling code that uses OROCOS-RTT
#
#  OROCOS-RTT_PATH: Path of the RTT installation directory (its CMAKE_INSTALL_PREFIX).
#  OROCOS-RTT_COMPONENT_PATH: The component path of the installation (<prefix>/lib/orocos) + RTT_COMPONENT_PATH
#  OROCOS-RTT_PLUGIN_PATH: OROCOS-RTT_PLUGINS_PATH + OROCOS-RTT_TYPES_PATH
#  OROCOS-RTT_PLUGINS_PATH: The plugins path of the installation (<prefix>/lib/orocos/plugins) + RTT_COMPONENT_PATH * /plugins
#  OROCOS-RTT_TYPES_PATH: The types path of the installation (<prefix>/lib/orocos/types) + RTT_COMPONENT_PATH * /types
#
#  OROCOS-RTT_CORBA_FOUND: Boolean that indicates if corba transport support is available
#  OROCOS-RTT_CORBA_LIBRARIES: Libraries to link against to use OROCOS-RTT with corba transport support
#
#  OROCOS-RTT_MQUEUE_FOUND: Boolean that indicates if mqueue transport support is available
#  OROCOS-RTT_MQUEUE_LIBRARIES: Libraries to link against to use OROCOS-RTT with mqueue transport support
#
#  OROCOS-RTT_VERSION: Package version
#  OROCOS-RTT_VERSION_MAJOR: Package major version
#  OROCOS-RTT_VERSION_MINOR: Package minor version
#  OROCOS-RTT_VERSION_PATCH: Package patch version
#
#  OROCOS-RTT_USE_FILE_PATH: Path to package use file, so it can be included like so
#                            include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake)
#  OROCOS-RTT_USE_FILE     : Allows you to write: include( ${OROCOS-RTT_USE_FILE} )
#
# This script additionally sets variables for each requested find_package COMPONENTS (OROCOS-RTT plugins).
# For example, for the ''rtt-scripting'' plugin this would be:
#  OROCOS-RTT_RTT-SCRIPTING_FOUND: Boolean that indicates if the component was found
#  OROCOS-RTT_RTT-SCRIPTING_LIBRARY: Libraries to link against to use this component (notice singular _LIBRARY suffix)
#
# Note for advanced users: Apart from the OROCOS-RTT_*_LIBRARIES variables, non-COMPONENTS targets can be accessed by
# their imported name, e.g., target_link_libraries(bar @IMPORTED_TARGET_PREFIX@orocos-rtt-gnulinux_dynamic).
# This of course requires knowing the name of the desired target, which is why using the OROCOS-RTT_*_LIBRARIES
# variables is recommended.
#
# Example usage:
#  find_package(OROCOS-RTT 2.0.5 EXACT REQUIRED rtt-scripting foo) # Defines OROCOS-RTT_RTT-SCRIPTING_*
#  find_package(OROCOS-RTT QUIET COMPONENTS rtt-transport-mqueue foo) # Defines OROCOS-RTT_RTT-TRANSPORT-MQUEUE_*
#
########################################################################################################################


########################################################################################################################
#
# Initialization
#
########################################################################################################################

# Set the default target operating system, if unspecified
set(DOC_STRING "The Operating System target. One of [gnulinux lxrt macosx win32 xenomai]")
set(OROCOS_TARGET_ENV $ENV{OROCOS_TARGET}) # MUST use helper variable, otherwise not picked up !!!
if(OROCOS_TARGET_ENV)
  set(OROCOS_TARGET ${OROCOS_TARGET_ENV} CACHE STRING "${DOC_STRING}" FORCE)
  if(NOT OROCOS-RTT_FIND_QUIETLY AND NOT Orocos-RTT_FIND_QUIETLY)
    message(STATUS "- Detected OROCOS_TARGET environment variable. Using: ${OROCOS_TARGET}")
  endif()
else()
  if(NOT DEFINED OROCOS_TARGET)
    if(MSVC)
      set(OROCOS_TARGET win32    CACHE STRING "${DOC_STRING}")
    elseif(APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(OROCOS_TARGET macosx   CACHE STRING "${DOC_STRING}")
    else()
      set(OROCOS_TARGET gnulinux CACHE STRING "${DOC_STRING}")
    endif()
  endif()
endif()

# By default, install libs in /target/ subdir in order to allow
# multi-target installs.
if ( NOT OROCOS_SUFFIX )
  set (OROCOS_SUFFIX "/${OROCOS_TARGET}")
endif()

# Path to current file
get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Include target specific config
include(${SELF_DIR}/orocos-rtt-config-${OROCOS_TARGET}.cmake)

########################################################################################################################
#
# OROCOS-RTT core
#
########################################################################################################################

# Only check for new modules if already found.
# This test is very important, otherwise running this script multiple times in the same project will try to add multiple
# imported targets with the same name, yielding a configuration-time error
if(NOT OROCOS-RTT_FOUND)

# Import targets
set(OROCOS-RTT_IMPORT_FILE "${SELF_DIR}/orocos-rtt-${OROCOS_TARGET}-libraries.cmake")
if (EXISTS "${OROCOS-RTT_IMPORT_FILE}")
  include("${OROCOS-RTT_IMPORT_FILE}")

# Core RTT libraries
set(OROCOS-RTT_TARGET "${PREFIX}orocos-rtt-${OROCOS_TARGET}_dynamic")
if(TARGET ${OROCOS-RTT_TARGET})
  set(OROCOS-RTT_LIBRARIES ${OROCOS-RTT_TARGET})
else()
  message(FATAL_ERROR "Imported target ${PREFIX}orocos-rtt-${OROCOS_TARGET}_dynamic not found. Please contact a project developer to fix this issue")
endif()

# Typekit support
set(OROCOS-RTT_TYPEKIT_TARGET "${PREFIX}rtt-typekit-${OROCOS_TARGET}_plugin")
if(TARGET ${OROCOS-RTT_TYPEKIT_TARGET})
  set(OROCOS-RTT_TYPEKIT_FOUND TRUE)
  set(OROCOS-RTT_TYPEKIT_LIBRARIES ${OROCOS-RTT_TYPEKIT_TARGET})
endif()

# Corba support
set(OROCOS-RTT_CORBA_TARGET "${PREFIX}orocos-rtt-corba-${OROCOS_TARGET}_dynamic")
if(TARGET ${OROCOS-RTT_CORBA_TARGET})
  set(OROCOS-RTT_CORBA_FOUND TRUE)
  set(FOUND_TRANSPORTS "${FOUND_TRANSPORTS} corba")
  set(OROCOS-RTT_CORBA_LIBRARIES ${OROCOS-RTT_CORBA_TARGET})
endif()

# Mqueue support
set(OROCOS-RTT-MQUEUE_TARGET "${PREFIX}orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic")
if(TARGET ${OROCOS-RTT-MQUEUE_TARGET})
  set(OROCOS-RTT_MQUEUE_FOUND TRUE)
  set(FOUND_TRANSPORTS "${FOUND_TRANSPORTS} mqueue")
  set(OROCOS-RTT_MQUEUE_LIBRARIES ${OROCOS-RTT-MQUEUE_TARGET})
endif()
endif()

# Link directories
get_filename_component(OROCOS-RTT_PATH "${SELF_DIR}/../../.." ABSOLUTE)
get_filename_component(OROCOS-RTT_COMPONENT_PATH "${SELF_DIR}/../../../lib/orocos${OROCOS_SUFFIX}" ABSOLUTE)
get_filename_component(OROCOS-RTT_PLUGINS_PATH "${SELF_DIR}/../../../lib/orocos${OROCOS_SUFFIX}/plugins" ABSOLUTE)
get_filename_component(OROCOS-RTT_TYPES_PATH   "${SELF_DIR}/../../../lib/orocos${OROCOS_SUFFIX}/types"   ABSOLUTE)
set(OROCOS-RTT_LIBRARY_DIRS "${OROCOS-RTT_PATH}/lib;${OROCOS-RTT_COMPONENT_PATH};${OROCOS-RTT_PLUGINS_PATH};${OROCOS-RTT_TYPES_PATH}")

# Definitions
set(OROCOS-RTT_DEFINITIONS "-DOROCOS_TARGET=${OROCOS_TARGET}")

# Path to package use file
set(OROCOS-RTT_USE_FILE_PATH ${SELF_DIR})
set(OROCOS-RTT_USE_FILE ${SELF_DIR}/UseOROCOS-RTT.cmake)

# Confirm found, not cached !
message(STATUS "Orocos-RTT found in ${OROCOS-RTT_IMPORT_FILE}")
set(OROCOS-RTT_FOUND TRUE)

endif()


########################################################################################################################
#
# Components: This is called each time a find_package is done:
#
########################################################################################################################

# Default component search path
list(APPEND OROCOS-RTT_PLUGIN_PATH "${OROCOS-RTT_PLUGINS_PATH}"
                                      "${OROCOS-RTT_TYPES_PATH}")

# Append additional user-defined plugin search paths
file(TO_CMAKE_PATH "$ENV{RTT_COMPONENT_PATH}" ENV_RTT_COMPONENT_PATH)
foreach(CUSTOM_COMPONENT_PATH ${ENV_RTT_COMPONENT_PATH})
  list(APPEND OROCOS-RTT_PLUGIN_PATH "${CUSTOM_COMPONENT_PATH}/plugins"
                                        "${CUSTOM_COMPONENT_PATH}/types")
endforeach()
# Append additional user-defined component search paths
foreach(CUSTOM_COMPONENT_PATH ${ENV_RTT_COMPONENT_PATH})
  list(APPEND OROCOS-RTT_COMPONENT_PATH "${CUSTOM_COMPONENT_PATH}")
endforeach()

# Find components
foreach(COMPONENT ${OROCOS-RTT_FIND_COMPONENTS} ${Orocos-RTT_FIND_COMPONENTS})

  # Find individual plugin
  string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
  set(OROCOS-RTT_${COMPONENT_UPPER}_FOUND FALSE)
  unset(OROCOS-RTT_${COMPONENT_UPPER}_LIBRARY CACHE)

  find_library(OROCOS-RTT_${COMPONENT_UPPER}_LIBRARY NAMES ${COMPONENT} ${COMPONENT}-${OROCOS_TARGET}
                                                     PATHS ${OROCOS-RTT_PLUGIN_PATH} NO_DEFAULT_PATH)
  mark_as_advanced(OROCOS-RTT_${COMPONENT_UPPER}_LIBRARY)

  # Set *_FOUND variables and update a space-separated lists of found/missing components
  if(OROCOS-RTT_${COMPONENT_UPPER}_LIBRARY)
    set(OROCOS-RTT_${COMPONENT_UPPER}_FOUND TRUE)
    set(FOUND_COMPONENTS "${FOUND_COMPONENTS} ${COMPONENT}")
  else()
    set(OROCOS-RTT_${COMPONENT_UPPER}_FOUND FALSE)
    set(MISSING_COMPONENTS "${MISSING_COMPONENTS} ${COMPONENT}")
  endif()

endforeach()

# Signal error if required components are not found
if( (OROCOS-RTT_FIND_REQUIRED OR Orocos-RTT_FIND_REQUIRED) AND MISSING_COMPONENTS)
  message(FATAL_ERROR "Could not find the following required OROCOS-RTT plugins: ${MISSING_COMPONENTS}. You may want to append the plugins folder location to the RTT_COMPONENT_PATH environment variable. E.g., if the plugin is located at /path/to/plugins/libfoo-plugin.so, add /path/to to RTT_COMPONENT_PATH")
endif()


########################################################################################################################
#
# Print success message
#
########################################################################################################################

if(NOT OROCOS-RTT_FIND_QUIETLY AND NOT Orocos-RTT_FIND_QUIETLY)

  # Basic message
  if(FOUND_TRANSPORTS)
    set(AVAILABLE_TRANSPORTS "Available transports:${FOUND_TRANSPORTS}")
  endif()
  message(STATUS "Found orocos-rtt ${OROCOS-RTT_VERSION} for the ${OROCOS_TARGET} target. ${AVAILABLE_TRANSPORTS}")

  # List found components
  if(FOUND_COMPONENTS)
    message(STATUS "- Found requested orocos-rtt components:${FOUND_COMPONENTS}")
  endif()

  # List missing components
  if(MISSING_COMPONENTS)
    message(STATUS "- Could NOT find requested orocos-rtt components:${MISSING_COMPONENTS}")
  endif()
endif()
