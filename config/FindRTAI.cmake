################################################################################
#
# CMake script for finding RTAI/LXRT.
# If the optional RTAI_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the RTAI_ROOT_DIR/include and RTAI_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  RTAI_FOUND: Boolean that indicates if the package was found
#  RTAI_INCLUDE_DIRS: Paths to the necessary header files
#  RTAI_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{RTAI_ROOT_DIR} STREQUAL "")
  set(RTAI_ROOT_DIR $ENV{RTAI_ROOT_DIR} CACHE PATH "RTAI base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(RTAI_ROOT_DIR)
endif()

# Header files to find
set(header_NAME rtai_lxrt.h)

# Libraries to find
set(rtai_NAME   lxrt)

# Find headers and libraries
if(RTAI_ROOT_DIR)
  # Use location specified by environment variable
  find_path(RTAI_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${RTAI_ROOT_DIR}/include PATH_SUFFIXES rtai NO_DEFAULT_PATH)
  find_library(RTAI_LIBRARY         NAMES ${rtai_NAME}          PATHS ${RTAI_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_path(RTAI_INCLUDE_DIR       NAMES ${header_NAME} PATH_SUFFIXES rtai )
  find_library(RTAI_LIBRARY        NAMES ${rtai_NAME})
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(RTAI_PROCESS_INCLUDES RTAI_INCLUDE_DIR)
set(RTAI_PROCESS_LIBS RTAI_LIBRARY)

# If user provides Linux source dir, check and append it as well
if( LINUX_SOURCE_DIR )
  set(linux_header_NAME linux/kernel.h)
  # We check for both the source dir or an installation dir:
  find_path(Linux_INCLUDE_DIR      NAMES ${linux_header_NAME} PATHS  ${LINUX_SOURCE_DIR} PATH_SUFFIXES include NO_DEFAULT_PATH)
  list(APPEND RTAI_PROCESS_INCLUDES Linux_INCLUDE_DIR)
endif()

libfind_process(RTAI)
