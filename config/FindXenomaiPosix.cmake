################################################################################
#
# CMake script for finding the XENOMAI 2 or XENOMAI 3 posix skin.
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

if ( XenomaiPosix_FIND_QUIETLY )
  set( XENOMAI_FIND_QUIETLY "QUIET")
endif()

if ( XenomaiPosix_FIND_REQUIRED )
  set( XENOMAI_FIND_REQUIRED "REQUIRED")
endif()

# Find headers and libraries
if(XENOMAI_ROOT_DIR)
  # Use location specified by environment variable
  find_program(XENOMAI_XENO_CONFIG NAMES xeno-config  PATHS ${XENOMAI_ROOT_DIR}/bin NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_program(XENOMAI_XENO_CONFIG NAMES xeno-config )
endif()

if(NOT XENOMAI_XENO_CONFIG )
  message(FATAL_ERROR "Your Xenomai installation is broken: I can not determine Xenomai Posix cflags/ldflags without xeno-config.")
else()
  execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --version OUTPUT_VARIABLE XENOMAI_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REPLACE "." ";" XENOMAI_VERSION_LIST ${XENOMAI_VERSION} )
  list(GET XENOMAI_VERSION_LIST 0 XENOMAI_VERSION_MAJOR)
  list(GET XENOMAI_VERSION_LIST 1 XENOMAI_VERSION_MINOR)
  list(GET XENOMAI_VERSION_LIST 2 XENOMAI_VERSION_PATCH)
endif()

if(${XENOMAI_VERSION_MAJOR} EQUAL 3)
    # NOTE: Copperplate auto-initialization
    #set(XENOMAI_LDFLAGS_EXTRA_ARGS "--no-auto-init")
    set(XENOMAI_LDFLAGS_EXTRA_ARGS "--no-mode-check" "--no-auto-init")
endif()

if(NOT XENOMAI_SKIN_NAME)
    message(SEND_ERROR "The only supported Xenomai versions are 2.x and 3.x, your version is ${XENOMAI_VERSION}")
endif()

execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=posix --cflags  OUTPUT_VARIABLE XENOMAI_POSIX_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=posix --ldflags ${XENOMAI_LDFLAGS_EXTRA_ARGS} OUTPUT_VARIABLE XENOMAI_POSIX_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(XENOMAI_LDFLAGS_LIST ${XENOMAI_POSIX_LDFLAGS})
separate_arguments(XENOMAI_LDFLAGS_LIST)
FOREACH(flag ${XENOMAI_LDFLAGS_LIST})
    if(${flag} MATCHES "bootstrap" AND ${XENOMAI_LIBRARY} MATCHES "bootstrap")
        set(bootstrap_element ${flag})
        break()
    endif()
ENDFOREACH()

if(NOT ${bootstrap_element} EQUAL -1)
    list(REMOVE_ITEM XENOMAI_LDFLAGS_LIST ${bootstrap_element})
endif()

set(XENOMAI_POSIX_LIBRARY ${XENOMAI_LDFLAGS_LIST})
# string(STRIP ${XENOMAI_POSIX_LDFLAGS} XENOMAI_POSIX_LIBRARY)

string(REGEX MATCHALL "-I([^ ]+)" XENOMAI_POSIX_INCLUDE_DIR ${XENOMAI_POSIX_CFLAGS})
string(REGEX MATCHALL "-D([^ ]+)" XENOMAI_POSIX_COMPILE_DEFINITION ${XENOMAI_POSIX_CFLAGS})
add_definitions(${XENOMAI_POSIX_COMPILE_DEFINITION})
string(REPLACE "-I" ";" XENOMAI_POSIX_INCLUDE_DIR ${XENOMAI_POSIX_INCLUDE_DIR})

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI_POSIX_PROCESS_INCLUDES XENOMAI_POSIX_INCLUDE_DIR)
set(XENOMAI_POSIX_PROCESS_LIBS XENOMAI_POSIX_LIBRARY)

libfind_process(XENOMAI_POSIX)
