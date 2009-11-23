################################################################################
#
# CMake script for finding XENOMAI.
# If the optional XENOMAI_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the XENOMAI_ROOT_DIR/include and XENOMAI_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  XENOMAI_POSIX_FOUND: Boolean that indicates if the package was found
#  XENOMAI_POSIX_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI_POSIX_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{XENOMAI_ROOT_DIR} STREQUAL "")
  set(XENOMAI_ROOT_DIR $ENV{XENOMAI_ROOT_DIR} CACHE PATH "Xenomai base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(XENOMAI_ROOT_DIR)
endif()

# Header files to find
set(header_NAME    pthread.h)

# Libraries to find
set(XENOMAI_POSIX_NAME   pthread_rt)

# Find headers and libraries
if(XENOMAI_POSIX_ROOT_DIR)
  # Use location specified by environment variable
  find_program(XENOMAI_XENO_CONFIG NAMES xeno-config  PATHS ${XENOMAI_POSIX_ROOT_DIR}/bin NO_DEFAULT_PATH)
  find_path(XENOMAI_POSIX_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${XENOMAI_POSIX_ROOT_DIR}/include PATH_SUFFIXES xenomai/posix NO_DEFAULT_PATH)
  find_library(XENOMAI_POSIX_LIBRARY         NAMES ${XENOMAI_POSIX_NAME}       PATHS ${XENOMAI_POSIX_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_program(XENOMAI_XENO_CONFIG NAMES xeno-config )
  find_path(XENOMAI_POSIX_INCLUDE_DIR       NAMES ${header_NAME} PATH_SUFFIXES xenomai )
  find_library(XENOMAI_POSIX_LIBRARY        NAMES ${XENOMAI_POSIX_NAME})
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI_POSIX_PROCESS_INCLUDES XENOMAI_POSIX_INCLUDE_DIR)
set(XENOMAI_POSIX_PROCESS_LIBS XENOMAI_POSIX_LIBRARY)

execute_process(COMMAND xeno-config --posix-ldflags OUTPUT_VARIABLE XENOMAI_POSIX_LDFLAGS )
execute_process(COMMAND xeno-config --posix-cflags OUTPUT_VARIABLE XENOMAI_POSIX_CFLAGS )

message("Found XenomaiPosix in ${XENOMAI_POSIX_INCLUDE_DIR}")

libfind_process(XENOMAI_POSIX)
