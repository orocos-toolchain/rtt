################################################################################
#
# CMake script for finding the XENOMAI3 native skin.
# If the optional XENOMAI_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the XENOMAI_ROOT_DIR/include and XENOMAI_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  XENOMAI3_FOUND: Boolean that indicates if the package was found
#  XENOMAI3_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI3_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{XENOMAI_ROOT_DIR} STREQUAL "")
  set(XENOMAI_ROOT_DIR $ENV{XENOMAI_ROOT_DIR} CACHE PATH "Xenomai 3 base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(XENOMAI_ROOT_DIR)
endif()

if ( Xenomai_FIND_QUIETLY )
  set( XENOMAI3_FIND_QUIETLY "QUIET")
endif()

if ( Xenomai_FIND_REQUIRED )
  set( XENOMAI3_FIND_REQUIRED "REQUIRED")
endif()

# Header files to find
set(header_NAME    cobalt/pthread.h)

# Libraries to find
set(XENOMAI3_NAME   cobalt)

# Find headers and libraries
if(XENOMAI_ROOT_DIR)
  # Use location specified by environment variable
  find_program(XENOMAI3_XENO_CONFIG NAMES xeno-config  PATHS ${XENOMAI_ROOT_DIR}/bin NO_DEFAULT_PATH)
  find_path(XENOMAI3_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${XENOMAI_ROOT_DIR}/include PATH_SUFFIXES xenomai NO_DEFAULT_PATH)
  find_library(XENOMAI3_LIBRARY         NAMES ${XENOMAI3_NAME}       PATHS ${XENOMAI_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_program(XENOMAI3_XENO_CONFIG NAMES xeno-config )
  find_path(XENOMAI3_INCLUDE_DIR       NAMES ${header_NAME} PATH_SUFFIXES xenomai )
  find_library(XENOMAI3_LIBRARY        NAMES ${XENOMAI3_NAME})
endif()

if( XENOMAI3_LIBRARY AND XENOMAI3_INCLUDE_DIR AND NOT XENOMAI3_XENO_CONFIG )
  message(SEND_ERROR "Your Xenomai installation is broken: I can not determine Xenomai Cobalt cflags/ldflags without xeno-config.")
else()
  # NOTE: --auto-init-solib adds boostrap_pic to build shared libs
  execute_process(COMMAND ${XENOMAI3_XENO_CONFIG} --skin=cobalt --ldflags --auto-init-solib OUTPUT_VARIABLE XENOMAI3_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${XENOMAI3_XENO_CONFIG} --skin=cobalt --cflags OUTPUT_VARIABLE XENOMAI3_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()


# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI3_PROCESS_INCLUDES XENOMAI3_INCLUDE_DIR)
set(XENOMAI3_PROCESS_LIBS XENOMAI3_LIBRARY)


libfind_process(XENOMAI3)
