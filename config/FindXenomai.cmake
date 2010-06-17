################################################################################
#
# CMake script for finding XENOMAI.
# If the optional XENOMAI_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the XENOMAI_ROOT_DIR/include and XENOMAI_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  XENOMAI_FOUND: Boolean that indicates if the package was found
#  XENOMAI_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{XENOMAI_ROOT_DIR} STREQUAL "")
  set(XENOMAI_ROOT_DIR $ENV{XENOMAI_ROOT_DIR} CACHE PATH "Xenomai base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(XENOMAI_ROOT_DIR)
endif()

# Header files to find
set(header_NAME    native/task.h)

# Libraries to find
set(XENOMAI_NAME   xenomai)
set(XENOMAI_NATIVE_NAME   native)

# Find headers and libraries
if(XENOMAI_ROOT_DIR)
  # Use location specified by environment variable
  find_path(XENOMAI_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${XENOMAI_ROOT_DIR}/include PATH_SUFFIXES xenomai NO_DEFAULT_PATH)
  find_library(XENOMAI_LIBRARY         NAMES ${XENOMAI_NAME}       PATHS ${XENOMAI_ROOT_DIR}/lib     NO_DEFAULT_PATH)
  find_library(XENOMAI_NATIVE_LIBRARY         NAMES ${XENOMAI_NATIVE_NAME}       PATHS ${XENOMAI_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_path(XENOMAI_INCLUDE_DIR       NAMES ${header_NAME} PATH_SUFFIXES xenomai )
  find_library(XENOMAI_LIBRARY        NAMES ${XENOMAI_NAME})
  find_library(XENOMAI_NATIVE_LIBRARY        NAMES ${XENOMAI_NATIVE_NAME})
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI_PROCESS_INCLUDES XENOMAI_INCLUDE_DIR)
if ( XENOMAI_LIBRARY )
  set(XENOMAI_PROCESS_LIBS XENOMAI_LIBRARY XENOMAI_NATIVE_LIBRARY)
else( XENOMAI_LIBRARY )
  set(XENOMAI_PROCESS_LIBS XENOMAI_NATIVE_LIBRARY)
endif( XENOMAI_LIBRARY )


libfind_process(XENOMAI)
