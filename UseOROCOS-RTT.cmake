########################################################################################################################
#
# CMake package use file for OROCOS-RTT.
# It is assumed that find_package(OROCOS-RTT ...) has already been invoked.
# See orocos-rtt-config.cmake for information on how to load OROCOS-RTT into your CMake project.
# To include this file from your CMake project, the OROCOS-RTT_USE_FILE_PATH variable is used:
# include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake)
#
########################################################################################################################

if(OROCOS-RTT_FOUND)
  # Include directories
  include_directories(${OROCOS-RTT_INCLUDE_DIRS})

  # Preprocessor definitions
  add_definitions(${OROCOS-RTT_DEFINITIONS})

  # TODO: Add macros discussed in the workshop
endif()