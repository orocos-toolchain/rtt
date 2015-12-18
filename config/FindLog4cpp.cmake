################################################################################
#
# CMake script for finding Log4cpp.
# The default CMake search process is used to locate files.
#
# This script creates the following variables:
#  LOG4CPP_FOUND: Boolean that indicates if the package was found
#  LOG4CPP_INCLUDE_DIRS: Paths to the necessary header files
#  LOG4CPP_LIBRARIES: Package libraries
#  LOG4CPP_LIBRARY_DIRS: Path to package libraries
#
################################################################################

include(FindPackageHandleStandardArgs)

# Find headers and libraries
find_path(LOG4CPP_INCLUDE_DIR NAMES log4cpp/Category.hh PATH_SUFFIXES orocos)
find_library(LOG4CPP_LIBRARY NAMES orocos-log4cpp)

# Set LOG4CPP_FOUND honoring the QUIET and REQUIRED arguments
find_package_handle_standard_args(LOG4CPP DEFAULT_MSG LOG4CPP_LIBRARY LOG4CPP_INCLUDE_DIR)

# Output variables
if(LOG4CPP_FOUND)
  # Include dirs
  set(LOG4CPP_INCLUDE_DIRS ${LOG4CPP_INCLUDE_DIR})

  # Libraries
  set(LOG4CPP_LIBRARIES ${LOG4CPP_LIBRARY})

  # Link dirs
  get_filename_component(LOG4CPP_LIBRARY_DIRS ${LOG4CPP_LIBRARY} PATH)
endif()

# Advanced options for not cluttering the cmake UIs
mark_as_advanced(LOG4CPP_INCLUDE_DIR LOG4CPP_LIBRARY)
