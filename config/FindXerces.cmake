################################################################################
#
# CMake script for finding Xerces.
# If the optional Xerces_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the Xerces_ROOT_DIR/include and Xerces_ROOT_DIR/libs
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables:
#  Xerces_FOUND: Boolean that indicates if the package was found
#  Xerces_INCLUDE_DIRS: Paths to the necessary header files
#  Xerces_LIBRARIES: Package libraries
#
################################################################################

include(LibFindMacros)

# Get hint from environment variable (if any)
if(NOT $ENV{Xerces_ROOT_DIR} STREQUAL "")
  set(Xerces_ROOT_DIR $ENV{Xerces_ROOT_DIR} CACHE PATH "Xerces base directory location (optional, used for nonstandard installation paths)" FORCE)
  mark_as_advanced(Xerces_ROOT_DIR)
endif()

# Header files to find
set(header_NAME xercesc/dom/DOM.hpp)

# Libraries to find
set(xerces_c_NAME      xerces-c)
set(xerces_depdom_NAME xerces-depdom)

# Find headers and libraries
if(Xerces_ROOT_DIR)
  # Use location specified by environment variable
  find_path(Xerces_INCLUDE_DIR        NAMES ${header_NAME}        PATHS ${Xerces_ROOT_DIR}/include NO_DEFAULT_PATH)
  find_library(Xerces_c_LIBRARY       NAMES ${xerces_c_NAME}      PATHS ${Xerces_ROOT_DIR}/lib     NO_DEFAULT_PATH)
  find_library(Xerces_depdom_LIBRARY  NAMES ${xerces_depdom_NAME} PATHS ${Xerces_ROOT_DIR}/lib     NO_DEFAULT_PATH)
else()
  # Use default CMake search process
  find_path(Xerces_INCLUDE_DIR       NAMES ${header_NAME})
  find_library(Xerces_c_LIBRARY      NAMES ${xerces_c_NAME})
  find_library(Xerces_depdom_LIBRARY NAMES ${xerces_depdom_NAME})
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Xerces_PROCESS_INCLUDES Xerces_INCLUDE_DIR)
set(Xerces_PROCESS_LIBS Xerces_c_LIBRARY
                        Xerces_depdom_LIBRARY)
libfind_process(Xerces)
