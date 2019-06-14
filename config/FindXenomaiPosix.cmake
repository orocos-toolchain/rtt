################################################################################
#
# CMake script for finding the XENOMAI 2/3 posix skin.
# If the optional XENOMAI_POSIX_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the XENOMAI_POSIX_ROOT_DIR/include and XENOMAI_POSIX_ROOT_DIR/lib
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
if(NOT $ENV{XENOMAI_POSIX_ROOT_DIR} STREQUAL "")
    set(XENOMAI_POSIX_ROOT_DIR $ENV{XENOMAI_POSIX_ROOT_DIR} CACHE PATH "Xenomai base directory location (optional, used for nonstandard installation paths)" FORCE)
    mark_as_advanced(XENOMAI_POSIX_ROOT_DIR)
endif()

if(NOT XENOMAI_POSIX_ROOT_DIR AND XENOMAI_ROOT_DIR)
    set(XENOMAI_POSIX_ROOT_DIR ${XENOMAI_ROOT_DIR})
endif()

if ( Xenomai_FIND_QUIETLY )
    set( XENOMAI_POSIX_FIND_QUIETLY "QUIET")
endif()

if ( Xenomai_FIND_REQUIRED )
    set( XENOMAI_POSIX_FIND_REQUIRED "REQUIRED")
endif()

# Find headers and libraries
if(XENOMAI_POSIX_ROOT_DIR)
    # Use location specified by environment variable
    find_program(XENOMAI_POSIX_XENO_CONFIG NAMES xeno-config  PATHS ${XENOMAI_POSIX_ROOT_DIR}/bin NO_DEFAULT_PATH)
else()
    # Use default CMake search process
    find_program(XENOMAI_POSIX_XENO_CONFIG NAMES xeno-config )
endif()

if(XENOMAI_POSIX_XENO_CONFIG )
    # Detect Xenomai version
    execute_process(COMMAND ${XENOMAI_POSIX_XENO_CONFIG} --version OUTPUT_VARIABLE XENOMAI_POSIX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "." ";" XENOMAI_POSIX_VERSION_LIST ${XENOMAI_POSIX_VERSION})
    string(REPLACE "-" ";" XENOMAI_POSIX_VERSION_LIST "${XENOMAI_POSIX_VERSION_LIST}")
    list(GET XENOMAI_POSIX_VERSION_LIST 0 XENOMAI_POSIX_VERSION_MAJOR)
    list(GET XENOMAI_POSIX_VERSION_LIST 1 XENOMAI_POSIX_VERSION_MINOR)
    list(GET XENOMAI_POSIX_VERSION_LIST 2 XENOMAI_POSIX_VERSION_PATCH)
else()
    message(FATAL_ERROR "Your Xenomai installation is broken: I can not determine Xenomai cflags/ldflags without xeno-config.")
endif()

# Here we have xeno-config
if(${XENOMAI_POSIX_VERSION_MAJOR} EQUAL 2)
    set(XENOMAI_POSIX_SKIN_NAME   posix)
endif()

if(${XENOMAI_POSIX_VERSION_MAJOR} EQUAL 3)
    set(XENOMAI_POSIX_SKIN_NAME   posix)
    set(XENO_CONFIG_LDFLAGS_EXTRA_ARGS "--no-auto-init")
endif()

if(NOT XENOMAI_POSIX_SKIN_NAME)
    message(FATAL_ERROR "Only Xenomai 2.x and 3.x are supported, your version is ${XENOMAI_POSIX_VERSION}")
endif()

message(STATUS "Xenomai ${XENOMAI_POSIX_VERSION} detected, searching for ${XENOMAI_POSIX_SKIN_NAME} skin.")

execute_process(COMMAND ${XENOMAI_POSIX_XENO_CONFIG} --skin=${XENOMAI_POSIX_SKIN_NAME} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}
                OUTPUT_VARIABLE XENOMAI_POSIX_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE XENOMAI_POSIX_LDFLAGS_ERROR)
execute_process(COMMAND ${XENOMAI_POSIX_XENO_CONFIG} --skin=${XENOMAI_POSIX_SKIN_NAME} --cflags ${XENOMAI_POSIX_COMPAT}
                OUTPUT_VARIABLE XENOMAI_POSIX_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE XENOMAI_POSIX_CFLAGS_ERROR)

if( XENOMAI_POSIX_LDFLAGS_ERROR)
    message(FATAL_ERROR "Could not determine ldflags with command ${XENOMAI_POSIX_XENO_CONFIG} --skin=${XENOMAI_POSIX_SKIN_NAME} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
endif()

if( XENOMAI_POSIX_CFLAGS_ERROR)
    message(FATAL_ERROR "Could not determine cflags with command ${XENOMAI_POSIX_XENO_CONFIG} --skin=${XENOMAI_POSIX_SKIN_NAME} --cflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
endif()

string(STRIP XENOMAI_POSIX_LDFLAGS ${XENOMAI_POSIX_LDFLAGS})
string(STRIP XENOMAI_POSIX_CFLAGS ${XENOMAI_POSIX_CFLAGS})
string(REPLACE " " ";" _XENOMAI_POSIX_LDFLAGS ${XENOMAI_POSIX_LDFLAGS})
string(REPLACE " " ";" _XENOMAI_POSIX_CFLAGS ${XENOMAI_POSIX_CFLAGS})

# Set/reset variables
set(XENOMAI_POSIX_LIBRARY "")
set(XENOMAI_POSIX_INCLUDE_DIR "")
set(XENOMAI_POSIX_DEFINITIONS "")
set(XENOMAI_POSIX_OTHER_LDFLAGS "")
set(XENOMAI_POSIX_OTHER_CFLAGS "")

foreach(_entry ${_XENOMAI_POSIX_LDFLAGS})
  string(REGEX MATCH "^-L(.+)|^-l(.+)|^(-Wl,.+)|^(.*bootstrap(-pic)?.o)" _lib ${_entry})
  if(_lib)
    list(APPEND XENOMAI_POSIX_LIBRARY ${_lib})
  else()
    list(APPEND XENOMAI_POSIX_OTHER_LDFLAGS ${_entry})
  endif()
endforeach()
foreach(_entry ${_XENOMAI_POSIX_CFLAGS})
  string(REGEX MATCH "^-I.+" _include_dir ${_entry})
  string(REGEX MATCH "^-D.+" _definition ${_entry})
  if(_include_dir)
    string(REGEX REPLACE "^-I" "" _include_dir ${_include_dir})
    list(APPEND XENOMAI_POSIX_INCLUDE_DIR ${_include_dir})
  elseif(_definition)
    string(REGEX REPLACE "^-D" "" _definition ${_definition})
    list(APPEND XENOMAI_POSIX_DEFINITIONS ${_definition})
  else()
    list(APPEND XENOMAI_POSIX_OTHER_CFLAGS ${_entry})
  endif()
endforeach()
string(REPLACE ";" " " XENOMAI_POSIX_OTHER_LDFLAGS "${XENOMAI_POSIX_OTHER_LDFLAGS}")
string(REPLACE ";" " " XENOMAI_POSIX_OTHER_CFLAGS "${XENOMAI_POSIX_OTHER_CFLAGS}")

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI_POSIX_PROCESS_INCLUDES XENOMAI_POSIX_INCLUDE_DIR)
set(XENOMAI_POSIX_PROCESS_LIBS XENOMAI_POSIX_LIBRARY)

message(STATUS "
==========================================
Xenomai ${XENOMAI_POSIX_VERSION} ${XENOMAI_POSIX_SKIN_NAME} skin
    libs        : ${XENOMAI_POSIX_LIBRARY}
    include     : ${XENOMAI_POSIX_INCLUDE_DIR}
    definitions : ${XENOMAI_POSIX_DEFINITIONS}
    ldflags     : ${XENOMAI_POSIX_LDFLAGS}
    cflags      : ${XENOMAI_POSIX_CFLAGS}
==========================================
")


libfind_process(XENOMAI_POSIX)
