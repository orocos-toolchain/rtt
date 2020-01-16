################################################################################
# Copyright 2020 Antoine HOARAU <antoine [at] flr.io>
# 
# CMake script for finding the XENOMAI 2 native or XENOMAI 3 alchemy/posix/rtdm skin.
# If the optional XENOMAI_ROOT_DIR environment variable exists, header files and
# libraries will be searched in the XENOMAI_ROOT_DIR/include and XENOMAI_ROOT_DIR/lib
# directories, respectively. Otherwise the default CMake search process will be
# used.
#
# This script creates the following variables for each skin.
#
#  Native/Alchemy skin :
#
#  XENOMAI_FOUND: Boolean that indicates if the package was found
#  XENOMAI_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI_LIBRARIES: Package libraries
#  XENOMAI_DEFINITIONS: Package compile definitions (-D)
#  XENOMAI_CFLAGS_OTHER: Package compiler flags (-f)
#  XENOMAI_LDFLAGS_OTHER: Everything that is not -L,-l,-Wl, or *bootstrap(-pic).o 
#  XENOMAI_CFLAGS: Raw compile flags (from xeno-config)
#  XENOMAI_LDFLAGS: Raw linker flags (from xeno-config)
#
#  And it also sets the XENOMAI_POSIX_* and XENOMAI_RTDM_* variables.
#
#  Posix Interface : 
#
#  XENOMAI_POSIX_FOUND: Boolean that indicates if the package was found
#  XENOMAI_POSIX_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI_POSIX_LIBRARIES: Package libraries
#  XENOMAI_POSIX_DEFINITIONS: Package compile definitions (-D)
#  XENOMAI_POSIX_CFLAGS_OTHER: Package compiler flags (-f)
#  XENOMAI_POSIX_LDFLAGS_OTHER: Everything that is not -L,-l,-Wl, or *bootstrap(-pic).o 
#  XENOMAI_POSIX_CFLAGS: Raw compile flags (from xeno-config)
#  XENOMAI_POSIX_LDFLAGS: Raw linker flags (from xeno-config)
#
#  RTDM Interface :
#
#  XENOMAI_RTDM_FOUND: Boolean that indicates if the package was found
#  XENOMAI_RTDM_INCLUDE_DIRS: Paths to the necessary header files
#  XENOMAI_RTDM_LIBRARIES: Package libraries
#  XENOMAI_RTDM_DEFINITIONS: Package compile defitions (-D)
#  XENOMAI_RTDM_CFLAGS_OTHER: Package compiler flags (-f)
#  XENOMAI_RTDM_LDFLAGS_OTHER: Everything that is not -L,-l,-Wl, or *bootstrap(-pic).o 
#  XENOMAI_RTDM_CFLAGS: Raw compile flags (from xeno-config)
#  XENOMAI_RTDM_LDFLAGS: Raw linker flags (from xeno-config)
#
#  NOTE: You still need FindRTnet.cmake for rtnet support on xenomai 2.x
################################################################################

include(FindPackageHandleStandardArgs)

# Get hint from environment variable (if any)
if(NOT $ENV{XENOMAI_ROOT_DIR} STREQUAL "")
    set(XENOMAI_ROOT_DIR $ENV{XENOMAI_ROOT_DIR} CACHE PATH "Xenomai base directory location (optional, used for nonstandard installation paths)" FORCE)
    mark_as_advanced(XENOMAI_ROOT_DIR)
endif()

# Find headers and libraries
if(XENOMAI_ROOT_DIR)
    # Use location specified by environment variable
    find_program(XENOMAI_XENO_CONFIG NAMES xeno-config  PATHS ${XENOMAI_ROOT_DIR}/bin NO_DEFAULT_PATH)
else()
    # Use default CMake search process
    find_program(XENOMAI_XENO_CONFIG NAMES xeno-config)
endif()

mark_as_advanced(XENOMAI_XENO_CONFIG)

function(find_xeno_skin_variables prefix skin_name)
    
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}
                    OUTPUT_VARIABLE ${prefix}_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_VARIABLE ${prefix}_LDFLAGS_ERROR)
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --cflags
                    OUTPUT_VARIABLE ${prefix}_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_VARIABLE ${prefix}_CFLAGS_ERROR)

    if(${prefix}_LDFLAGS_ERROR)
        message(FATAL_ERROR "Could not determine ldflags with command ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
    endif()

    if(${prefix}_CFLAGS_ERROR)
        message(FATAL_ERROR "Could not determine cflags with command ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --cflags")
    endif()

    set(${prefix}_FOUND TRUE)

    string(STRIP ${prefix}_LDFLAGS "${${prefix}_LDFLAGS}")
    string(STRIP ${prefix}_CFLAGS "${${prefix}_CFLAGS}")
    string(REPLACE " " ";" _${prefix}_LDFLAGS ${${prefix}_LDFLAGS})
    string(REPLACE " " ";" _${prefix}_CFLAGS ${${prefix}_CFLAGS})
    
    foreach(_entry ${_${prefix}_LDFLAGS})
      string(REGEX MATCH "^-L(.+)|^-l(.+)|^(-Wl,.+)|^(.*bootstrap(-pic)?.o)" _lib ${_entry})
      if(_lib)
        list(APPEND ${prefix}_LIBRARY ${_lib})
      endif()
    endforeach()
    foreach(_entry ${_${prefix}_CFLAGS})
      string(REGEX MATCH "^-I.+" _include_dir ${_entry})
      string(REGEX MATCH "^-D.+" _definition ${_entry})
      string(REGEX MATCH "^-f.+" _compile_flag ${_entry})
      if(_include_dir)
        string(REGEX REPLACE "^-I" "" _include_dir ${_include_dir})
        list(APPEND ${prefix}_INCLUDE_DIR ${_include_dir})
      elseif(_definition)
        string(REGEX REPLACE "^-D" "" _definition ${_definition})
        list(APPEND ${prefix}_DEFINITIONS ${_definition})
      elseif(_compile_flag)
        list(APPEND ${prefix}_CFLAGS_OTHER ${_compile_flag})
      endif()
    endforeach()

    message(STATUS "
    ==========================================
    Xenomai ${XENOMAI_VERSION} ${skin_name} skin
        libs          : ${${prefix}_LIBRARY}
        include       : ${${prefix}_INCLUDE_DIR}
        definitions   : ${${prefix}_DEFINITIONS}
        cflags other  : ${${prefix}_CFLAGS_OTHER}
        ldflags other : ${${prefix}_LDFLAGS_OTHER}
        ldflags       : ${${prefix}_LDFLAGS}
        cflags        : ${${prefix}_CFLAGS}
    ==========================================
    ")

    set(${prefix}_FOUND ${${prefix}_FOUND} CACHE INTERNAL "")
    
    set(${prefix}_INCLUDE_DIRS ${${prefix}_INCLUDE_DIR} CACHE INTERNAL "")
    set(${prefix}_LIBRARIES ${${prefix}_LIBRARY} CACHE INTERNAL "")
    set(${prefix}_DEFINITIONS ${${prefix}_DEFINITIONS} CACHE INTERNAL "")
    set(${prefix}_CFLAGS_OTHER ${${prefix}_CFLAGS_OTHER} CACHE INTERNAL "")
    set(${prefix}_LDFLAGS_OTHER ${${prefix}_CFLAGS_OTHER} CACHE INTERNAL "")
    
    set(${prefix}_LDFLAGS ${${prefix}_LDFLAGS} CACHE INTERNAL "")
    set(${prefix}_CFLAGS ${${prefix}_CFLAGS} CACHE INTERNAL "")

    mark_as_advanced(${prefix}_LIBRARIES ${prefix}_INCLUDE_DIRS ${prefix}_DEFINITIONS ${prefix}_LDFLAGS ${prefix}_CFLAGS ${prefix}_CFLAGS_OTHER ${prefix}_LDFLAGS_OTHER)
endfunction()

function(handle_standard_args prefix)
    find_package_handle_standard_args(${prefix} DEFAULT_MSG ${prefix}_LIBRARIES ${prefix}_INCLUDE_DIRS ${prefix}_DEFINITIONS ${prefix}_LDFLAGS ${prefix}_CFLAGS ${prefix}_CFLAGS_OTHER)
endfunction()

if(XENOMAI_XENO_CONFIG)
    # Detect Xenomai version
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --version OUTPUT_VARIABLE XENOMAI_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "." ";" XENOMAI_VERSION_LIST ${XENOMAI_VERSION})
    string(REPLACE "-" ";" XENOMAI_VERSION_LIST "${XENOMAI_VERSION_LIST}") # Handle 3.1-dev (git)
    list(GET XENOMAI_VERSION_LIST 0 XENOMAI_VERSION_MAJOR)
    list(GET XENOMAI_VERSION_LIST 1 XENOMAI_VERSION_MINOR)
    list(GET XENOMAI_VERSION_LIST 2 XENOMAI_VERSION_PATCH)

    # Xenomai 2.x native skin (rt_)
    if(${XENOMAI_VERSION_MAJOR} EQUAL 2)
        set(XENOMAI_SKIN_NAME   native)
    endif()

    # Xenomai 3.x alchemy skin (rt_)
    if(${XENOMAI_VERSION_MAJOR} EQUAL 3)
        set(XENOMAI_SKIN_NAME   alchemy)
        # NOTE: --auto-init-solib bootstraps xenomai_init()
        set(XENO_CONFIG_LDFLAGS_EXTRA_ARGS "--auto-init-solib")
    endif()

    if(NOT XENOMAI_SKIN_NAME)
        message(FATAL_ERROR "Only Xenomai 2.x and 3.x are supported, your version is ${XENOMAI_VERSION}")
    endif()

    find_xeno_skin_variables(XENOMAI ${XENOMAI_SKIN_NAME})
    find_xeno_skin_variables(XENOMAI_POSIX posix)
    find_xeno_skin_variables(XENOMAI_RTDM rtdm)
else()
    set(XENOMAI_FOUND FALSE)
    set(XENOMAI_POSIX_FOUND FALSE)
    set(XENOMAI_RTDM_FOUND FALSE)
endif()

if(Xenomai_FIND_QUIETLY)
    set(XENOMAI_FIND_QUIETLY True)
    set(XENOMAI_POSIX_FIND_QUIETLY True)
    set(XENOMAI_RTDM_FIND_QUIETLY True)
endif()

find_package_handle_standard_args(Xenomai VERSION_VAR XENOMAI_VERSION REQUIRED_VARS XENOMAI_XENO_CONFIG)

handle_standard_args(XENOMAI)
handle_standard_args(XENOMAI_POSIX)
handle_standard_args(XENOMAI_RTDM)
