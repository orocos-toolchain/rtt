
#
# Parses arguments or options
#
# From: http://www.cmake.org/Wiki/CMakeMacroParseArguments
#
# For each item in options, PARSE_ARGUMENTS will create a variable
# with that name, prefixed with prefix_. So, for example, if prefix is
# MY_MACRO and options is OPTION1;OPTION2, then PARSE_ARGUMENTS will
# create the variables MY_MACRO_OPTION1 and MY_MACRO_OPTION2. These
# variables will be set to true if the option exists in the command
# line or false otherwise.
#
# For each item in arg_names, PARSE_ARGUMENTS will create a variable
# with that name, prefixed with prefix_. Each variable will be filled
# with the arguments that occur after the given arg_name is encountered
# up to the next arg_name or the end of the arguments. All options are
# removed from these lists. PARSE_ARGUMENTS also creates a
# prefix_DEFAULT_ARGS variable containing the list of all arguments up
# to the first arg_name encountered.
#
MACRO(ORO_PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})  
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    SET(larg_names ${arg_names})
    LIST(FIND larg_names "${arg}" is_arg_name)     
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})
      LIST(FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
         SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
         SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(ORO_PARSE_ARGUMENTS)

#
# Parses an Autoproj or rosbuild manifest.xml file and stores the dependencies in RESULT.
# Relies on xpath. If no manifest is found, returns an empty RESULT.
#
# Usage: orocos_get_manifest_deps DEPS)
#
function( orocos_get_manifest_deps RESULT)
  if ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml )
    message("[orocos_get_manifest_deps] Note: this package has no manifest.xml file. No dependencies can be auto-configured.")
    return()
  endif ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml )

  find_program(XPATH_EXE xpath )
  if (NOT XPATH_EXE)
    message("[orocos_get_manifest_deps] Warning: xpath not found. Can't read dependencies in manifest.xml file.")
  else(NOT XPATH_EXE)
    IF (APPLE)
      execute_process(COMMAND ${XPATH_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml "package/depend/@package" RESULT_VARIABLE RES OUTPUT_VARIABLE DEPS)
      SET(REGEX_STR " package=\"([^\"]+)\"")
    ELSE (APPLE)
      execute_process(COMMAND ${XPATH_EXE} -q -e "package/depend/@package" ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml RESULT_VARIABLE RES OUTPUT_VARIABLE DEPS)
      SET(REGEX_STR " package=\"([^\"]+)\"\n")
    ENDIF (APPLE)
    if (NOT RES EQUAL 0)
      message(SEND_ERROR "Error: xpath found but returned non-zero:${DEPS}")
    endif (NOT RES EQUAL 0)

    string(REGEX REPLACE "${REGEX_STR}" "\\1;" RR_RESULT ${DEPS})

    #message("Deps are: '${DEPS}'")
    set(${RESULT} ${RR_RESULT} PARENT_SCOPE)
    #message("Dependencies are: '${${RESULT}}'")
  endif (NOT XPATH_EXE)

endfunction( orocos_get_manifest_deps RESULT)

#
# Parses a Catkin package.xml file and stores the dependencies in RESULT.
# Relies on xpath. If no file is found, returns an empty RESULT.
#
# Usage: orocos_get_catkin_deps DEPS)
#
function( orocos_get_catkin_deps RESULT)

  set(_PACKAGE_XML_PATH "${PROJECT_SOURCE_DIR}/package.xml")

  if ( NOT EXISTS ${_PACKAGE_XML_PATH} )
    message("Note: this package has no Catkin package.xml file. No dependencies can be auto-configured.")
    return()
  endif ( NOT EXISTS ${_PACKAGE_XML_PATH} )

  find_program(XPATH_EXE xpath )
  if (NOT XPATH_EXE)
    message("Warning: xpath not found. Can't read dependencies in manifest.xml file.")
  else(NOT XPATH_EXE)
    IF (APPLE)
      execute_process(COMMAND ${XPATH_EXE} ${_PACKAGE_XML_PATH} "package/build_depend/text()" RESULT_VARIABLE RES OUTPUT_VARIABLE DEPS)
      #SET(REGEX_STR " package=\"([^\"]+)\"")
    ELSE (APPLE)
      execute_process(COMMAND ${XPATH_EXE} -q -e "package/build_depend/text()" ${_PACKAGE_XML_PATH} RESULT_VARIABLE RES OUTPUT_VARIABLE DEPS)
      #SET(REGEX_STR " package=\"([^\"]+)\"\n")
    ENDIF (APPLE)
    if (NOT RES EQUAL 0)
      message(SEND_ERROR "Error: xpath found but returned non-zero:${DEPS}")
    endif (NOT RES EQUAL 0)

    #string(REGEX REPLACE "${REGEX_STR}" "\\1;" RR_RESULT ${DEPS})

    message("[orocos_get_catkin_deps] Deps from ${_PACKAGE_XML_PATH} are: '${DEPS}'")
    set(${RESULT} ${RR_RESULT} PARENT_SCOPE)
    #message("Dependencies are: '${${RESULT}}'")
  endif (NOT XPATH_EXE)

endfunction( orocos_get_catkin_deps RESULT)

#
# Find a package, pick up its compile and link flags. It does this by locating
# and reading the .pc file generated by that package. In case no such .pc file
# exists (or is not found), it is assumed that no flags are necessary.
#
# This macro is called by orocos_use_package()
#
# It sets these variables for each package:
#   ${PACKAGE}_LIBRARIES        The fully resolved link libraries for this package.
#   ${PACKAGE}_INCLUDE_DIRS     The include directories for this package.
#   ${PACKAGE}_LIBRARY_DIRS     The library directories for this package.
#   ${PACKAGE}_CFLAGS_OTHER     The compile flags other than -I for this package.
#   ${PACKAGE}_LDFLAGS_OTHER    The linker flags other than -L and -l for thfully resolved link libraries for this package.
#   ${PACKAGE}_<LIB>_LIBRARY    Each fully resolved link library <LIB> in the above list.
#   
# It will also aggregate the following variables for all packages found in this
# scope:
#   USE_OROCOS_LIBRARIES
#   USE_OROCOS_INCLUDE_DIRS
#   USE_OROCOS_LIBRARY_DIRS
#   USE_OROCOS_CFLAGS_OTHER
#   USE_OROCOS_LDFLAGS_OTHER
#  
#   USE_OROCOS_COMPILE_FLAGS    All exported compile flags from packages within the current scope.
#   USE_OROCOS_LINK_FLAGS       All exported link flags from packages within the current scope.
# 
# Usage: orocos__package( myrobot )
#
macro( orocos_find_package PACKAGE )
  if ( "${PACKAGE}" STREQUAL "rtt")
  else()
    # Try to use rosbuild to find PACKAGE
    if(ORO_USE_ROSBUILD)
      # use rospack to find package directories of *all* dependencies.
      # We need these because a .pc file may depend on another .pc file in another package.
      # This package + the packages this package depends on:
      rosbuild_find_ros_package(${PACKAGE})

      if(DEFINED ${PACKAGE}_PACKAGE_PATH AND EXISTS "${${PACKAGE}_PACKAGE_PATH}/manifest.xml")
        # call orocos_use_package recursively for dependees
        rosbuild_invoke_rospack(${PACKAGE} ${PACKAGE} DEPENDS1 depends1)
        if(${PACKAGE}_DEPENDS1)
          string(REPLACE "\n" ";" ${PACKAGE}_DEPENDS1 ${${PACKAGE}_DEPENDS1})
          foreach(dependee ${${PACKAGE}_DEPENDS1})
            # Avoid using a package that has already been found
            if(NOT DEFINED ${dependee}_COMP_${OROCOS_TARGET}_FOUND)
              orocos_find_package(${dependee})
            endif()
          endforeach()
        endif()

        # add PACKAGE_PATH/lib/pkgconfig to the PKG_CONFIG_PATH
        set(ENV{PKG_CONFIG_PATH} "${${PACKAGE}_PACKAGE_PATH}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
      endif()
    endif()

    # Now we are ready to get the flags from the .pc files:
    #pkg_check_modules(${PACKAGE}_COMP ${PACKAGE}-${OROCOS_TARGET})
    pkg_search_module(${PACKAGE}_COMP_${OROCOS_TARGET} ${PACKAGE}-${OROCOS_TARGET} ${PACKAGE})
    if (${PACKAGE}_COMP_${OROCOS_TARGET}_FOUND)
      # Use find_libraries to find each library:
      unset(${PACKAGE}_LIBRARIES CACHE)
      foreach(COMP_LIB ${${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARIES})
        # Two options: COMP_LIB is an absolute path-to-lib (must start with ':') or just a libname:
        if ( ${COMP_LIB} MATCHES "^:(.+)" OR EXISTS ${COMP_LIB})
          if (EXISTS "${CMAKE_MATCH_1}" )
            # absolute path (shared lib):
            set( ${PACKAGE}_${COMP_LIB}_LIBRARY "${CMAKE_MATCH_1}" )
          endif()
          if (EXISTS "${COMP_LIB}" )
            # absolute path (static lib):
            set( ${PACKAGE}_${COMP_LIB}_LIBRARY "${COMP_LIB}" )
          endif()
        else()
          # libname:
          find_library(${PACKAGE}_${COMP_LIB}_LIBRARY NAMES ${COMP_LIB} HINTS ${${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARY_DIRS})
        endif()
        if(${PACKAGE}_${COMP_LIB}_LIBRARY)
        else(${PACKAGE}_${COMP_LIB}_LIBRARY)
          message(SEND_ERROR "In package >>>${PACKAGE}<<< : could not find library ${COMP_LIB} in directory ${${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARY_DIRS}, although its .pc file says it should be there.\n\n Try to do 'make clean; rm -rf lib' and then 'make' in the package >>>${PACKAGE}<<<.\n\n")
        endif(${PACKAGE}_${COMP_LIB}_LIBRARY)
        list(APPEND ${PACKAGE}_LIBRARIES "${${PACKAGE}_${COMP_LIB}_LIBRARY}")
      endforeach(COMP_LIB ${${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARIES})

      # Add some output variables to the cache to make them accessible from outside this scope
      set(${PACKAGE}_INCLUDE_DIRS "${${PACKAGE}_COMP_${OROCOS_TARGET}_INCLUDE_DIRS}" CACHE INTERNAL "")
      set(${PACKAGE}_LIBRARY_DIRS "${${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARY_DIRS}" CACHE INTERNAL "")
      set(${PACKAGE}_LIBRARIES "${${PACKAGE}_LIBRARIES}" CACHE INTERNAL "")
      # The flags are space separated, so no need to quote here:
      set(${PACKAGE}_CFLAGS_OTHER ${${PACKAGE}_COMP_${OROCOS_TARGET}_CFLAGS_OTHER} CACHE INTERNAL "")
      set(${PACKAGE}_LDFLAGS_OTHER ${${PACKAGE}_COMP_${OROCOS_TARGET}_LDFLAGS_OTHER} CACHE INTERNAL "")

      # Add compiler and linker flags to the USE_OROCOS_XXX_FLAGS variables used in the orocos_add_x macros
      list(APPEND USE_OROCOS_COMPILE_FLAGS ${${PACKAGE}_COMP_${OROCOS_TARGET}_CFLAGS_OTHER})
      list(APPEND USE_OROCOS_LINK_FLAGS ${${PACKAGE}_COMP_${OROCOS_TARGET}_LDFLAGS_OTHER})
      # This probably does not work since lists are ';' separated and not ' ' separated:
      list(REMOVE_DUPLICATES USE_OROCOS_COMPILE_FLAGS)
      list(REMOVE_DUPLICATES USE_OROCOS_LINK_FLAGS)

      # Store aggregated variables
      list(APPEND USE_OROCOS_INCLUDE_DIRS ${${PACKAGE}_INCLUDE_DIRS})
      list(APPEND USE_OROCOS_LIBRARIES ${${PACKAGE}_LIBRARIES})
      list(APPEND USE_OROCOS_LIBRARY_DIRS ${${PACKAGE}_LIBRARY_DIRS})
      list(APPEND USE_OROCOS_CFLAGS_OTHER ${${PACKAGE}_CFLAGS_OTHER})
      list(APPEND USE_OROCOS_LDFLAGS_OTHER ${${PACKAGE}_LDFLAGS_OTHER})

    else (${PACKAGE}_COMP_${OROCOS_TARGET}_FOUND)
      message("[UseOrocos] ${PACKAGE} does not provide a .pc file for exporting its build/link flags (or one of it 'Requires' dependencies was not found).")
    endif (${PACKAGE}_COMP_${OROCOS_TARGET}_FOUND)
  endif()  
endmacro( orocos_find_package PACKAGE )

#
# Find a package, pick up its include dirs and link with its libraries.
# It does this by locating and reading the .pc file generated by that package.
# In case no such .pc file exists (or is not found), it is assumed that no
# flags are necessary.
#
# This macro is called for you by UseOrocos-RTT.cmake for each dependency
# listed in your rosbuild or Autoproj manifest.xml file. 
#
# By default, this will add linker flags from all the dependencies to all
# targets in this scope unless OROCOS_NO_AUTO_LINKING is set to True.
# 
# Internally it calls orocos_find_package(), which exports serveral variables
# containing build flags exported by dependencies. See the
# orocos_find_package() documentation for more details.
#
# Usage: orocos_use_package( myrobot )
#
macro( orocos_use_package PACKAGE )
  # Check a flag so we don't over-link
  if(NOT ${PACKAGE}_${OROCOS_TARGET}_USED)
    # Get the package and dependency build flags
    orocos_find_package(${PACKAGE})

    # Include the aggregated include directories
    include_directories(${PACKAGE}_INCLUDE_DIRS})

    # Only link in case there is something *and* the user didn't opt-out:
    if (NOT OROCOS_NO_AUTO_LINKING AND ${PACKAGE}_COMP_${OROCOS_TARGET}_LIBRARIES)
      link_libraries( ${${PACKAGE}_LIBRARIES} )
      message("[orocos_use_package] Linking all targets with libraries from package '${PACKAGE}'. To disable this, set OROCOS_NO_AUTO_LINKING to true.")
      #message("Linking with ${PACKAGE}: ${${PACKAGE}_LIBRARIES}")
    endif ()

    # Set a flag so we don't over-link
    set(${PACKAGE}_${OROCOS_TARGET}_USED true)
  endif()
endmacro( orocos_use_package PACKAGE )

macro(_orocos_list_to_string _string _list)
    set(${_string})
    foreach(_item ${_list})
        string(LENGTH "${${_string}}" _len)
        if(${_len} GREATER 0)
          set(${_string} "${${_string}} ${_item}")
        else(${_len} GREATER 0)
          set(${_string} "${_item}")
        endif(${_len} GREATER 0)
    endforeach(_item)
endmacro(_orocos_list_to_string)

macro(orocos_add_compile_flags target)
  set(args ${ARGN})
  separate_arguments(args)
  get_target_property(_flags ${target} COMPILE_FLAGS)
  if(NOT _flags)
    set(_flags ${ARGN})
  else(NOT _flags)
    separate_arguments(_flags)
    list(APPEND _flags "${args}")
  endif(NOT _flags)

  _orocos_list_to_string(_flags_str "${_flags}")
  set_target_properties(${target} PROPERTIES
                        COMPILE_FLAGS "${_flags_str}")
endmacro(orocos_add_compile_flags)

macro(orocos_add_link_flags target)
  set(args ${ARGN})
  separate_arguments(args)
  get_target_property(_flags ${target} LINK_FLAGS)
  if(NOT _flags)
    set(_flags ${ARGN})
  else(NOT _flags)
    separate_arguments(_flags)
    list(APPEND _flags "${args}")
  endif(NOT _flags)

  _orocos_list_to_string(_flags_str "${_flags}")
  set_target_properties(${target} PROPERTIES
                        LINK_FLAGS "${_flags_str}")
endmacro(orocos_add_link_flags)

