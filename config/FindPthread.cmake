################################################################################
#
# CMake script for finding libpthread/pthread.h
# If the optional PTHREAD_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the PTHREAD_ROOT_DIR/include and PTHREAD_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  PTHREAD_FOUND: Boolean that indicates if the package was found
#  PTHREAD_INCLUDE_DIRS: Paths to the necessary header files
#  PTHREAD_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{PTHREAD_ROOT_DIR} STREQUAL "")
  set(PTHREAD_ROOT_DIR $ENV{PTHREAD_ROOT_DIR} CACHE PATH "PTHREAD base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(PTHREAD_ROOT_DIR)
endif()

# Header files to find
set(header_NAME pthread.h)

# Libraries to find
set(pthread_NAME   pthread)

# Find headers and libraries
if(PTHREAD_ROOT_DIR)
  # Use location specified by environment variable
  find_path(PTHREAD_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${PTHREAD_ROOT_DIR}/include PATH_SUFFIXES pthread NO_DEFAULT_PATH)
  find_library(PTHREAD_LIBRARY         NAMES ${pthread_NAME}       PATHS ${PTHREAD_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_path(PTHREAD_INCLUDE_DIR       NAMES ${header_NAME} PATH_SUFFIXES pthread )
  find_library(PTHREAD_LIBRARY        NAMES ${pthread_NAME})
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(PTHREAD_PROCESS_INCLUDES PTHREAD_INCLUDE_DIR)
set(PTHREAD_PROCESS_LIBS PTHREAD_LIBRARY)

libfind_process(PTHREAD)
