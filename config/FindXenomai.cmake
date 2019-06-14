################################################################################
#
# CMake script for finding the XENOMAI 2 native or XENOMAI 3 alchemy skin.
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

if ( Xenomai_FIND_QUIETLY )
    set( XENOMAI_FIND_QUIETLY "QUIET")
endif()

if ( Xenomai_FIND_REQUIRED )
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

if(XENOMAI_XENO_CONFIG )
    # Detect Xenomai version
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --version OUTPUT_VARIABLE XENOMAI_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "." ";" XENOMAI_VERSION_LIST ${XENOMAI_VERSION})
    string(REPLACE "-" ";" XENOMAI_VERSION_LIST "${XENOMAI_VERSION_LIST}")
    list(GET XENOMAI_VERSION_LIST 0 XENOMAI_VERSION_MAJOR)
    list(GET XENOMAI_VERSION_LIST 1 XENOMAI_VERSION_MINOR)
    list(GET XENOMAI_VERSION_LIST 2 XENOMAI_VERSION_PATCH)
else()
    message(FATAL_ERROR "Your Xenomai installation is broken: I can not determine Xenomai cflags/ldflags without xeno-config.")
endif()

# Here we have xeno-config
if(${XENOMAI_VERSION_MAJOR} EQUAL 2)
    set(XENOMAI_SKIN_NAME   native)
endif()

if(${XENOMAI_VERSION_MAJOR} EQUAL 3)
    set(XENOMAI_SKIN_NAME   alchemy)
    # NOTE: --auto-init-solib bootstraps xenomai_init()
    set(XENO_CONFIG_LDFLAGS_EXTRA_ARGS "--auto-init-solib")
endif()

if(NOT XENOMAI_SKIN_NAME)
    message(FATAL_ERROR "Only Xenomai 2.x and 3.x are supported, your version is ${XENOMAI_VERSION}")
endif()

message(STATUS "Xenomai ${XENOMAI_VERSION} detected, searching for ${XENOMAI_SKIN_NAME} skin.")

execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${XENOMAI_SKIN_NAME} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}
                OUTPUT_VARIABLE XENOMAI_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_VARIABLE XENOMAI_LDFLAGS_ERROR)
execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${XENOMAI_SKIN_NAME} --cflags ${XENOMAI_COMPAT}
                OUTPUT_VARIABLE XENOMAI_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_VARIABLE XENOMAI_CFLAGS_ERROR)

if( XENOMAI_LDFLAGS_ERROR)
    message(FATAL_ERROR "Could not determine ldflags with command ${XENOMAI_XENO_CONFIG} --skin=${XENOMAI_SKIN_NAME} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
endif()

if( XENOMAI_CFLAGS_ERROR)
    message(FATAL_ERROR "Could not determine cflags with command ${XENOMAI_XENO_CONFIG} --skin=${XENOMAI_SKIN_NAME} --cflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
endif()

string(STRIP XENOMAI_LDFLAGS ${XENOMAI_LDFLAGS})
string(STRIP XENOMAI_CFLAGS ${XENOMAI_CFLAGS})
string(REPLACE " " ";" _XENOMAI_LDFLAGS ${XENOMAI_LDFLAGS})
string(REPLACE " " ";" _XENOMAI_CFLAGS ${XENOMAI_CFLAGS})

# Set/reset variables
set(XENOMAI_LIBRARY "")
set(XENOMAI_INCLUDE_DIR "")
set(XENOMAI_DEFINITIONS "")
set(XENOMAI_OTHER_LDFLAGS "")
set(XENOMAI_OTHER_CFLAGS "")

foreach(_entry ${_XENOMAI_LDFLAGS})
  string(REGEX MATCH "^-L(.+)|^-l(.+)|^(-Wl,.+)|^(.*bootstrap(-pic)?.o)" _lib ${_entry})
  if(_lib)
    list(APPEND XENOMAI_LIBRARY ${_lib})
  else()
    list(APPEND XENOMAI_OTHER_LDFLAGS ${_entry})
  endif()
endforeach()
foreach(_entry ${_XENOMAI_CFLAGS})
  string(REGEX MATCH "^-I.+" _include_dir ${_entry})
  string(REGEX MATCH "^-D.+" _definition ${_entry})
  if(_include_dir)
    string(REGEX REPLACE "^-I" "" _include_dir ${_include_dir})
    list(APPEND XENOMAI_INCLUDE_DIR ${_include_dir})
  elseif(_definition)
    string(REGEX REPLACE "^-D" "" _definition ${_definition})
    list(APPEND XENOMAI_DEFINITIONS ${_definition})
  else()
    list(APPEND XENOMAI_OTHER_CFLAGS ${_entry})
  endif()
endforeach()
string(REPLACE ";" " " XENOMAI_OTHER_LDFLAGS "${XENOMAI_OTHER_LDFLAGS}")
string(REPLACE ";" " " XENOMAI_OTHER_CFLAGS "${XENOMAI_OTHER_CFLAGS}")

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XENOMAI_PROCESS_INCLUDES XENOMAI_INCLUDE_DIR)
set(XENOMAI_PROCESS_LIBS XENOMAI_LIBRARY)

message(STATUS "
==========================================
Xenomai ${XENOMAI_VERSION} ${XENOMAI_SKIN_NAME} skin
    libs        : ${XENOMAI_LIBRARY}
    include     : ${XENOMAI_INCLUDE_DIR}
    definitions : ${XENOMAI_DEFINITIONS}
    ldflags     : ${XENOMAI_LDFLAGS}
    cflags      : ${XENOMAI_CFLAGS}
==========================================
")

libfind_process(XENOMAI)
