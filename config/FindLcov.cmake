################################################################################
#
# CMake script for finding lcov.
# The default CMake search process is used to locate files.
#
# This script creates the following variables:
#
#  LCOV_FOUND               Boolean that indicates if the package was found
#  LCOV_LCOV_EXECUTABLE     Where to find the 'lcov' tool
#  LCOV_GENHTML_EXECUTABLE  Where to find the 'genhtml' tool
#
################################################################################

include(FindPackageHandleStandardArgs)

find_program(LCOV_LCOV_EXECUTABLE lcov)
find_program(LCOV_GENHTML_EXECUTABLE genhtml)

# Set LCOV_FOUND honoring the QUIET and REQUIRED arguments
find_package_handle_standard_args(LCOV DEFAULT_MSG LCOV_LCOV_EXECUTABLE LCOV_GENHTML_EXECUTABLE)

# Advanced options for not cluttering the cmake UIs
mark_as_advanced(LCOV_LCOV_EXECUTABLE LCOV_GENHTML_EXECUTABLE)
