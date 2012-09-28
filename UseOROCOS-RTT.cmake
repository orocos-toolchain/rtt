########################################################################################################################
#
# CMake package use file for OROCOS-RTT.
# It is assumed that find_package(OROCOS-RTT ...) has already been invoked.
# See orocos-rtt-config.cmake for information on how to load OROCOS-RTT into your CMake project.
# To include this file from your CMake project, the OROCOS-RTT_USE_FILE_PATH variable is used:
#   include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake) 
# or even shorter:
#   include(${OROCOS-RTT_USE_FILE})
#
########################################################################################################################

if(OROCOS-RTT_FOUND)
  include(FindPkgConfig)
  include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT-helpers.cmake)
  # Include directories
  include_directories(${OROCOS-RTT_INCLUDE_DIRS})

  # Preprocessor definitions
  add_definitions(${OROCOS-RTT_DEFINITIONS})
  set(ROS_ROOT $ENV{ROS_ROOT})

  if (ROS_ROOT AND NOT NO_ROS_PACKAGE )
    set(ROS_PACKAGE_PATH $ENV{ROS_PACKAGE_PATH})
    #In bash: for i in $(echo "$ROS_PACKAGE_PATH" | sed -e's/:/ /g'); do if expr match "`pwd`" "$i"; then is_ros_package=1; fi; done > /dev/null
    string(REPLACE ":" ";" ROS_PACKAGE_PATH ${ROS_PACKAGE_PATH})
    foreach( rpath IN LISTS ROS_PACKAGE_PATH )
      # This is a bit tricky since overlapping directory names may give false positives:
      file(TO_CMAKE_PATH ${rpath} path) # removes trailing '/'
      #message(" ${rpath} -> ${path}")
      if ( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${path}" OR "${CMAKE_CURRENT_SOURCE_DIR}" MATCHES "${path}/" )
	set(IS_ROS_PACKAGE TRUE)
	message("This package is in your ROS_PACKAGE_PATH, so I'm using rosbuild-style package building.")
	
      endif()
    endforeach()
    if(NOT IS_ROS_PACKAGE)
      message("ROS_ROOT was detected but this package is NOT in your ROS_PACKAGE_PATH. I'm not using any rosbuild-style building.")
      # These were set by roscpp cmake macros:
      unset( EXECUTABLE_OUTPUT_PATH )
      unset( LIBRARY_OUTPUT_PATH )
    endif()
  endif()

  # This is for not allowing undefined symbols when using gcc
  if (CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE)
    SET(USE_OROCOS_LINK_FLAGS "-Wl,-z,defs")
  else (CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE)
    SET(USE_OROCOS_LINK_FLAGS " ")
  endif (CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE)
  # Suppress API decoration warnings in Win32:
  if (MSVC)
    set(USE_OROCOS_COMPILE_FLAGS "/wd4251" )
  else (MSVC)
    set(USE_OROCOS_COMPILE_FLAGS " " )
  endif (MSVC)

  # On windows, the CMAKE_INSTALL_PREFIX is forced to the Orocos-RTT path.
  # There's two alternatives to disable this behavior:
  #
  # 1. Use the ORO_DEFAULT_INSTALL_PREFIX variable to modify the default
  #    installation path:
  #
  #     set(ORO_DEFAULT_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  #     include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake)
  #
  # 2. Force a non-default CMAKE_INSTALL_PREFIX prior to executing cmake:
  #
  #     cmake -DCMAKE_INSTALL_PREFIX="<your install prefix>" [...]
  #
  # In all cases, the Orocos macros will always honor any change to the cached
  # CMAKE_INSTALL_PREFIX variable.
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT DEFINED ORO_DEFAULT_INSTALL_PREFIX)
    if(WIN32)
        set(ORO_DEFAULT_INSTALL_PREFIX "orocos")
    endif(WIN32)
  endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT DEFINED ORO_DEFAULT_INSTALL_PREFIX)

  # For backwards compatibility. Was only used on WIN32 targets:
  if(DEFINED INSTALL_PATH)
    set(ORO_DEFAULT_INSTALL_PREFIX ${INSTALL_PATH})
  endif(DEFINED INSTALL_PATH)

  if(DEFINED ORO_DEFAULT_INSTALL_PREFIX)
    if(ORO_DEFAULT_INSTALL_PREFIX STREQUAL "orocos")
        set (CMAKE_INSTALL_PREFIX ${OROCOS-RTT_PATH} CACHE PATH "Install prefix forced to orocos by ORO_DEFAULT_INSTALL_PREFIX" FORCE)
    else(ORO_DEFAULT_INSTALL_PREFIX STREQUAL "orocos")
        set (CMAKE_INSTALL_PREFIX ${ORO_DEFAULT_INSTALL_PREFIX} CACHE PATH "Install prefix forced by ORO_DEFAULT_INSTALL_PREFIX" FORCE)
    endif(ORO_DEFAULT_INSTALL_PREFIX STREQUAL "orocos")
  endif(DEFINED ORO_DEFAULT_INSTALL_PREFIX)
  
  # Infer package name from directory name.                                                                                                                                                                                                  
  get_filename_component(orocos_package ${CMAKE_SOURCE_DIR} NAME)
  message("[UseOrocos] Building package ${orocos_package}")

  # By default, install libs in /target/ subdir in order to allow
  # multi-target installs.
  if ( NOT DEFINED OROCOS_SUFFIX )
    set (OROCOS_SUFFIX "/${OROCOS_TARGET}")
  endif()
  
  if (IS_ROS_PACKAGE)
    if ( NOT ROSBUILD_init_called )
      if (NOT DEFINED ROSBUILD_init_called )
	include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake) # Prevent double inclusion ! This file is not robust against that !
      endif()
      rosbuild_init()
    endif()

    # In ros builds, we need to set the pkg-config path such that RTT is found by
    # the typekit/typegen/pc files logic:
    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${rtt_PACKAGE_PATH}/install/lib/pkgconfig")
    #message("Setting PKG_CONFIG_PATH to $ENV{PKG_CONFIG_PATH}")

    # We only need the direct dependencies, the rest is resolved
    # by the .pc files.
    rosbuild_invoke_rospack(${orocos_package} pkg DEPS depends1)
    string(REGEX REPLACE "\n" ";" pkg_DEPS2 "${pkg_DEPS}" )
    foreach(ROSDEP ${pkg_DEPS2})
        orocos_use_package( ${ROSDEP} ) 
    endforeach(ROSDEP ${pkg_DEPS2}) 
  else (IS_ROS_PACKAGE)
    # Fall back to 'manually' processing the manifest.xml file.
    orocos_get_manifest_deps( DEPS )
    #message("Dependencies are: ${DEPS}")
    foreach(DEP ${DEPS})
        orocos_use_package( ${DEP} ) 
    endforeach(DEP ${DEPS}) 
    
  endif (IS_ROS_PACKAGE)

  # Necessary to find rtt when we scan the manifest.xml file.
  if (IS_ROS_PACKAGE)
    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${rtt_PACKAGE_PATH}/install/lib/pkgconfig")
  endif(IS_ROS_PACKAGE)

  # Necessary for correctly building mixed libraries on win32.
  if(OROCOS_TARGET STREQUAL "win32")
    set(CMAKE_DEBUG_POSTFIX "d")
  endif(OROCOS_TARGET STREQUAL "win32")


#
# Include and link against required stuff
#
#From: http://www.cmake.org/Wiki/CMakeMacroParseArguments
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

# Components should add themselves by calling 'OROCOS_COMPONENT' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
# You can set a variable COMPONENT_VERSION x.y.z to set a version or 
# specify the optional VERSION parameter. For ros builds, the version
# number is ignored.
#
# Usage: orocos_component( COMPONENT_NAME src1 src2 src3 [INSTALL lib/orocos/${PROJECT_NAME}] [VERSION x.y.z] )
#
macro( orocos_component COMPONENT_NAME )
  
  ORO_PARSE_ARGUMENTS(ADD_COMPONENT
    "INSTALL;VERSION"
    ""
    ${ARGN}
    )
  SET( SOURCES ${ADD_COMPONENT_DEFAULT_ARGS} )
  SET( LIB_NAME "${COMPONENT_NAME}-${OROCOS_TARGET}")
  # Extract install directory:
  if ( ADD_COMPONENT_INSTALL )
    set(AC_INSTALL_DIR ${ADD_COMPONENT_INSTALL}${OROCOS_SUFFIX})
    set(AC_INSTALL_RT_DIR bin)
  else()
    set(AC_INSTALL_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
    set(AC_INSTALL_RT_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
  endif()
  
  # Set library name:
  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32")
      set( COMPONENT_LIB_NAME ${COMPONENT_NAME}-${OROCOS_TARGET})
  else()
      set( COMPONENT_LIB_NAME ${COMPONENT_NAME})
  endif()

  # Set component version:
  if (COMPONENT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${COMPONENT_VERSION})
  endif(COMPONENT_VERSION)
  if (ADD_COMPONENT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${ADD_COMPONENT_VERSION})
  endif(ADD_COMPONENT_VERSION)
  MESSAGE( "[UseOrocos] Building component ${COMPONENT_NAME} in library ${COMPONENT_LIB_NAME}" )

  # Clear the dependencies such that a target switch can be detected:
  unset( ${COMPONENT_NAME}_LIB_DEPENDS )

  # Use rosbuild in ros environments:
  if (IS_ROS_PACKAGE)
    rosbuild_add_library(${COMPONENT_NAME} ${SOURCES} )
    SET_TARGET_PROPERTIES( ${COMPONENT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/orocos${OROCOS_SUFFIX}
    )
  else()
    ADD_LIBRARY( ${COMPONENT_NAME} SHARED ${SOURCES} )
  endif()

  # Prepare component lib for out-of-the-ordinary lib directories
  SET_TARGET_PROPERTIES( ${COMPONENT_NAME} PROPERTIES
    OUTPUT_NAME ${COMPONENT_LIB_NAME}
    DEFINE_SYMBOL "RTT_COMPONENT"
    ${LIB_COMPONENT_VERSION}
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX};${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
    )
  orocos_add_compile_flags(${COMPONENT_NAME} ${USE_OROCOS_COMPILE_FLAGS})
  orocos_add_link_flags(${COMPONENT_NAME} ${USE_OROCOS_LINK_FLAGS})
  TARGET_LINK_LIBRARIES( ${COMPONENT_NAME} ${OROCOS-RTT_LIBRARIES} ) #${OROCOS-RTT_TYPEKIT_LIBRARIES} )

  # Install
  # On win32, component runtime (.dll) should go in orocos folder
  if( ${OROCOS_TARGET} STREQUAL "win32" )
    INSTALL(TARGETS ${COMPONENT_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_DIR})
  else()
    INSTALL(TARGETS ${COMPONENT_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})
  endif()


  # Add current dir as link lookup-dir
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  # Necessary for .pc file generation
  list(APPEND OROCOS_DEFINED_COMPS " -l${COMPONENT_LIB_NAME}")
endmacro( orocos_component )

# Utility libraries should add themselves by calling 'orocos_library()' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
# You can set a variable COMPONENT_VERSION x.y.z to set a version or 
# specify the optional VERSION parameter. For ros builds, the version
# number is ignored.
#
# Usage: orocos_library( libraryname src1 src2 src3 [VERSION x.y.z] )
#
macro( orocos_library LIB_TARGET_NAME )

  ORO_PARSE_ARGUMENTS(ORO_LIBRARY
    "INSTALL;VERSION"
    ""
    ${ARGN}
    )
  SET( SOURCES ${ORO_LIBRARY_DEFAULT_ARGS} )
  if ( ORO_LIBRARY_INSTALL )
    set(AC_INSTALL_DIR ${ORO_LIBRARY_INSTALL})
    set(AC_INSTALL_RT_DIR bin)
  else()
    set(AC_INSTALL_DIR lib)
    set(AC_INSTALL_RT_DIR bin)
  endif()
  
  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
  else()
      set( LIB_NAME ${LIB_TARGET_NAME})
  endif()

  # Clear the dependencies such that a target switch can be detected:
  unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

  MESSAGE( "[UseOrocos] Building library ${LIB_TARGET_NAME}" )
  if (IS_ROS_PACKAGE)
    rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
  else()
    ADD_LIBRARY( ${LIB_TARGET_NAME} SHARED ${SOURCES} )
  endif()

  if (COMPONENT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${COMPONENT_VERSION})
  endif(COMPONENT_VERSION)
  if (ORO_LIBRARY_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${ORO_LIBRARY_VERSION})
  endif(ORO_LIBRARY_VERSION)

  SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIB_NAME}
    ${LIB_COMPONENT_VERSION}
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
    )
  orocos_add_compile_flags(${LIB_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS})
  orocos_add_link_flags(${LIB_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS})
  TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} ${OROCOS-RTT_LIBRARIES} ) #${OROCOS-RTT_TYPEKIT_LIBRARIES} )

  INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})

  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  # Necessary for .pc file generation
  list(APPEND OROCOS_DEFINED_LIBS " -l${LIB_NAME}")
endmacro( orocos_library )

# Executables should add themselves by calling 'orocos_executable()'
# instead of 'ADD_EXECUTABLE' in CMakeLists.txt.
#
# Usage: orocos_executable( executablename src1 src2 src3 [INSTALL bin] )
#
macro( orocos_executable EXE_TARGET_NAME )

  ORO_PARSE_ARGUMENTS(ORO_EXECUTABLE
    "INSTALL"
    ""
    ${ARGN}
    )
  SET( SOURCES ${ORO_EXECUTABLE_DEFAULT_ARGS} )
  if ( ORO_EXECUTABLE_INSTALL )
    set(AC_INSTALL_DIR ${ORO_EXECUTABLE_INSTALL})
    set(AC_INSTALL_RT_DIR bin)
  else()
    set(AC_INSTALL_DIR lib)
    set(AC_INSTALL_RT_DIR bin)
  endif()

  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32")
      set( EXE_NAME ${EXE_TARGET_NAME}-${OROCOS_TARGET})
  else()
      set( EXE_NAME ${EXE_TARGET_NAME})
  endif()

  MESSAGE( "Building executable ${EXE_TARGET_NAME}" )
  if (IS_ROS_PACKAGE)
    rosbuild_add_executable(${EXE_TARGET_NAME} ${SOURCES} )
  else()
    ADD_EXECUTABLE( ${EXE_TARGET_NAME} ${SOURCES} )
  endif()
  SET_TARGET_PROPERTIES( ${EXE_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXE_NAME}
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/bin;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
    )
    if(CMAKE_DEBUG_POSTFIX)
        set_target_properties( ${EXE_TARGET_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} )
    endif(CMAKE_DEBUG_POSTFIX)
  orocos_add_compile_flags(${EXE_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS})
  orocos_add_link_flags(${EXE_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS})

  TARGET_LINK_LIBRARIES( ${EXE_TARGET_NAME} ${OROCOS-RTT_LIBRARIES} )

  # We install the exe, the user must make sure that the install dir is not
  # beneath the ROS package (if any).
  INSTALL(TARGETS ${EXE_TARGET_NAME} RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})

  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
endmacro( orocos_executable )

# Type headers should add themselves by calling 'orocos_typegen_headers()'
# They will be processed by typegen to generate a typekit from it, with the
# name of the current project. 
#
# Usage: orocos_typegen_headers( robotdata.hpp sensordata.hpp )
#
macro( orocos_typegen_headers )

  MESSAGE( "[UseOrocos] Generating typekit for ${PROJECT_NAME}..." )
  
  # Works in top level source dir:
  find_program(TYPEGEN_EXE typegen)
  if (NOT TYPEGEN_EXE)
    message(FATAL_ERROR "'typegen' not found in path. Can't build typekit. Did you 'source env.sh' ?")
  else (NOT TYPEGEN_EXE)

    execute_process( COMMAND ${TYPEGEN_EXE} --output typekit ${PROJECT_NAME} ${ARGN} 
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
      )
    # work around generated manifest.xml file:
    execute_process( COMMAND ${CMAKE} remove -f typekit/manifest.xml )
    add_subdirectory( typekit )
    list(APPEND OROCOS_DEFINED_TYPES " -l${PROJECT_NAME}-typekit-${OROCOS_TARGET}")
  endif (NOT TYPEGEN_EXE)
endmacro( orocos_typegen_headers )

# typekit libraries should add themselves by calling 'orocos_typekit()' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
# You can set a variable COMPONENT_VERSION x.y.z to set a version or 
# specify the optional VERSION parameter. For ros builds, the version
# number is ignored.
#
# Usage: orocos_typekit( typekitname src1 src2 src3 [INSTALL lib/orocos/project/types] [VERSION x.y.z] )
#
macro( orocos_typekit LIB_TARGET_NAME )

  ORO_PARSE_ARGUMENTS(ORO_TYPEKIT
    "INSTALL;VERSION"
    ""
    ${ARGN}
    )
  SET( SOURCES ${ORO_TYPEKIT_DEFAULT_ARGS} )
  if ( ORO_TYPEKIT_INSTALL )
    set(AC_INSTALL_DIR ${ORO_TYPEKIT_INSTALL})
    set(AC_INSTALL_RT_DIR bin)
  else()
    set(AC_INSTALL_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
    set(AC_INSTALL_RT_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
  endif()
  if (COMPONENT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${COMPONENT_VERSION})
  endif(COMPONENT_VERSION)
  if (ORO_TYPEKIT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${ORO_TYPEKIT_VERSION})
  endif(ORO_TYPEKIT_VERSION)

  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
  else()
      set( LIB_NAME ${LIB_TARGET_NAME})
  endif()

  # Clear the dependencies such that a target switch can be detected:
  unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

  MESSAGE( "[UseOrocos] Building typekit library ${LIB_TARGET_NAME}" )
  if (IS_ROS_PACKAGE)
    rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
    SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/orocos${OROCOS_SUFFIX}/types
    )
  else()
    ADD_LIBRARY( ${LIB_TARGET_NAME} SHARED ${SOURCES} )
  endif()
  SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIB_NAME}
    ${LIB_COMPONENT_VERSION}
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/types;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
    )
  TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} ${OROCOS-RTT_LIBRARIES} )

  # On win32, typekit runtime (.dll) should go in orocos/types folder
  if( ${OROCOS_TARGET} STREQUAL "win32" )
    INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_DIR})
  else()
    INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})
  endif()

  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  # Necessary for .pc file generation
  list(APPEND OROCOS_DEFINED_TYPES " -l${LIB_NAME}")
endmacro( orocos_typekit )

# plugin libraries should add themselves by calling 'orocos_plugin()' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
# You can set a variable COMPONENT_VERSION x.y.z to set a version or 
# specify the optional VERSION parameter. For ros builds, the version
# number is ignored.
#
# Usage: orocos_plugin( pluginname src1 src2 src3 [INSTALL lib/orocos/project/plugins] [VERSION x.y.z])
#
macro( orocos_plugin LIB_TARGET_NAME )

  ORO_PARSE_ARGUMENTS(ORO_PLUGIN
    "INSTALL;VERSION"
    ""
    ${ARGN}
    )
  SET( SOURCES ${ORO_PLUGIN_DEFAULT_ARGS} )
  if ( ORO_PLUGIN_INSTALL )
    set(AC_INSTALL_DIR ${ORO_PLUGIN_INSTALL})
    set(AC_INSTALL_RT_DIR bin)
  else()
    set(AC_INSTALL_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins )
    set(AC_INSTALL_RT_DIR lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins )
  endif()
  if (COMPONENT_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${COMPONENT_VERSION})
  endif(COMPONENT_VERSION)
  if (ORO_PLUGIN_VERSION)
    set( LIB_COMPONENT_VERSION VERSION ${ORO_PLUGIN_VERSION})
  endif(ORO_PLUGIN_VERSION)

  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
  else()
      set( LIB_NAME ${LIB_TARGET_NAME})
  endif()

  # Clear the dependencies such that a target switch can be detected:
  unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

  if (IS_ROS_PACKAGE)
    MESSAGE( "[UseOrocos] Building plugin library ${LIB_TARGET_NAME} in ROS tree." )
    rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
    SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/orocos${OROCOS_SUFFIX}/plugins
    )
  else()
    MESSAGE( "[UseOrocos] Building plugin library ${LIB_TARGET_NAME}" )
    ADD_LIBRARY( ${LIB_TARGET_NAME} SHARED ${SOURCES} )
  endif()
  SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIB_NAME}
    ${LIB_COMPONENT_VERSION}
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/plugins;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
    )
  orocos_add_compile_flags(${LIB_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS})
  orocos_add_link_flags(${LIB_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS})
  TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} ${OROCOS-RTT_LIBRARIES} ) #${OROCOS-RTT_TYPEKIT_LIBRARIES} )

  # On win32, plugins runtime (.dll) should go in orocos/plugins folder
  if( ${OROCOS_TARGET} STREQUAL "win32" )
    INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_DIR})
  else()
    INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})
  endif()


  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  # Necessary for .pc file generation
  list(APPEND OROCOS_DEFINED_PLUGINS " -l${LIB_NAME}")
endmacro( orocos_plugin )

# service libraries should add themselves by calling 'orocos_service()' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
#
# Usage: orocos_service( servicename src1 src2 src3 )
#
macro( orocos_service LIB_TARGET_NAME )
  orocos_plugin( ${LIB_TARGET_NAME} ${ARGN} )
endmacro( orocos_service )

#
# Components supply header files which should be included when 
# using these components. Each component should use this macro
# to install its header-files. They are installed by default
# in include/orocos/${PROJECT_NAME}
#
# Usage example: orocos_install_header( hardware.hpp control.hpp )
macro( orocos_install_headers )
  ORO_PARSE_ARGUMENTS(ORO_INSTALL_HEADER
    "INSTALL"
    ""
    ${ARGN}
    )
  set( SOURCES ${ORO_INSTALL_HEADER_DEFAULT_ARGS} )
  if ( ORO_INSTALL_HEADER_INSTALL )
    set(AC_INSTALL_DIR ${ORO_INSTALL_HEADER_INSTALL})
  else()
    set(AC_INSTALL_DIR include/orocos/${PROJECT_NAME} )
  endif()
  install( FILES ${SOURCES} DESTINATION ${AC_INSTALL_DIR} )
endmacro( orocos_install_headers )

#
# Adds the uninstall target, not present by default in CMake.
#
# Usage example: orocos_uninstall_target()
macro( orocos_uninstall_target )
  if (NOT OROCOS_UNINSTALL_DONE AND NOT TARGET uninstall)
    CONFIGURE_FILE(
      "${OROCOS-RTT_USE_FILE_PATH}/cmake_uninstall.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
      IMMEDIATE @ONLY)

    ADD_CUSTOM_TARGET(uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
  endif (NOT OROCOS_UNINSTALL_DONE AND NOT TARGET uninstall)
  set(OROCOS_UNINSTALL_DONE)
endmacro( orocos_uninstall_target )

#
# Generate package files for the whole project. Do this as the very last
# step in your project's CMakeLists.txt file.
#
# Allows to set a name for the .pc file (without extension)
# and a version (defaults to 1.0). The name and version you provide will
# be used unmodified.
#
# If you didn't specify VERSION but COMPONENT_VERSION has been set,
# that variable will be used to set the version number.
#
# You may specify a dependency list of .pc files to depend on with DEPENDS. You will need this
# to set the include paths correctly if a public header of
# this package includes a header of another (non-Orocos) package. This dependency
# will end up in the Requires: field of the .pc file.
#
# You may specify a dependency list of .pc files of Orocos packages with DEPENDS_TARGET
# This is similar to DEPENDS, but the -<target> suffix is added for every package name.
# This dependency will end up in the Requires: field of the .pc file.
#
# orocos_generate_package( [name] [VERSION version] [DEPENDS packagenames....])
#
macro( orocos_generate_package )

  oro_parse_arguments(ORO_CREATE_PC
    "VERSION;DEPENDS;DEPENDS_TARGETS"
    ""
    ${ARGN}
    )

  # Check version
  if (NOT ORO_CREATE_PC_VERSION)
    if (COMPONENT_VERSION)
      set( ORO_CREATE_PC_VERSION ${COMPONENT_VERSION})
      message("[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION} from COMPONENT_VERSION.")
    else (COMPONENT_VERSION)
      set( ORO_CREATE_PC_VERSION "1.0")
      message("[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION} (default version).")
    endif (COMPONENT_VERSION)
  else (NOT ORO_CREATE_PC_VERSION)
    message("[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION}.")
  endif (NOT ORO_CREATE_PC_VERSION)

  # Create filename
  if ( ORO_CREATE_PC_DEFAULT_ARGS )
    set(PC_NAME ${ORO_CREATE_PC_DEFAULT_ARGS})
  else ( ORO_CREATE_PC_DEFAULT_ARGS )
    set(PC_NAME ${PROJECT_NAME} )
    if ( NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${${PROJECT_NAME}_SOURCE_DIR} )
      # Append -subdir-subdir-... to pc name:
      file(RELATIVE_PATH RELPATH ${${PROJECT_NAME}_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
      string(REPLACE "/" "-" PC_NAME_SUFFIX ${RELPATH} )
      set(PC_NAME ${PC_NAME}-${PC_NAME_SUFFIX})
    endif ( NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${${PROJECT_NAME}_SOURCE_DIR} )
    set(PC_NAME ${PC_NAME}-${OROCOS_TARGET})
  endif ( ORO_CREATE_PC_DEFAULT_ARGS )

  # Create dependency list
  foreach( DEP ${ORO_CREATE_PC_DEPENDS_TARGETS})
     list(APPEND ${ORO_CREATE_PC_DEPENDS} ${DEP}-${OROCOS_TARGET})
  endforeach()

  # Create lib-path list
  set(PC_LIBS "Libs: ")
  if (OROCOS_DEFINED_LIBS)
    set(PC_LIBS "${PC_LIBS} -L\${libdir} ${OROCOS_DEFINED_LIBS}")
  endif (OROCOS_DEFINED_LIBS)
  if (OROCOS_DEFINED_COMPS)
    set(PC_LIBS "${PC_LIBS} -L\${orocos_libdir} ${OROCOS_DEFINED_COMPS}")
  endif (OROCOS_DEFINED_COMPS)
  if (OROCOS_DEFINED_PLUGINS)
    set(PC_LIBS "${PC_LIBS} -L\${orocos_libdir}/plugins ${OROCOS_DEFINED_PLUGINS}")
  endif (OROCOS_DEFINED_PLUGINS)
  if (OROCOS_DEFINED_TYPES)
    set(PC_LIBS "${PC_LIBS} -L\${orocos_libdir}/types ${OROCOS_DEFINED_TYPES}")
  endif (OROCOS_DEFINED_TYPES)

  set(PC_PREFIX ${CMAKE_INSTALL_PREFIX})
  set(PC_LIB_DIR "\${libdir}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}")
  set(PC_CONTENTS "prefix=@PC_PREFIX@
libdir=\${prefix}/lib
includedir=\${prefix}/include/orocos
orocos_libdir=${PC_LIB_DIR}

Name: ${PC_NAME}
Description: ${PC_NAME} package for Orocos
Requires: orocos-rtt-${OROCOS_TARGET} ${ORO_CREATE_PC_DEPENDS}
Version: ${ORO_CREATE_PC_VERSION}
${PC_LIBS}
Cflags: -I\${includedir}
")
  string(CONFIGURE "${PC_CONTENTS}" INSTALLED_PC_CONTENTS @ONLY)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PC_NAME}.pc ${INSTALLED_PC_CONTENTS})

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PC_NAME}.pc DESTINATION lib/pkgconfig )
  #install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml DESTINATION  lib/orocos${OROCOS_SUFFIX}/level0 )

  # For ros package trees, we install the .pc file also next to the manifest file:
  if (IS_ROS_PACKAGE)
    set(PC_PREFIX ${PROJECT_SOURCE_DIR})
    set(PC_LIB_DIR "\${libdir}/orocos${OROCOS_SUFFIX}") # Without package name suffix !
    # For some reason, @PC_PREFIX@ is being filled in in PC_CONTENTS above,
    # so we need to reset it. CMake bug ???
  set(PC_CONTENTS "# Rationale:
# - The prefix is equal to the package directory.
# - The libdir is where the libraries were built, ie, package/lib
# - The include dir in cflags allows top-level headers and in package/include/package/header.h
# - If this doesn't fit your package layout, don't use orocos_generate_package() and write the .pc file yourself

prefix=@PC_PREFIX@
libdir=\${prefix}/lib
includedir=\${prefix}/include
orocos_libdir=${PC_LIB_DIR}

Name: ${PC_NAME}
Description: ${PC_NAME} package for Orocos
Requires: orocos-rtt-${OROCOS_TARGET} ${ORO_CREATE_PC_DEPENDS}
Version: ${ORO_CREATE_PC_VERSION}
${PC_LIBS}
Cflags: -I\${includedir} -I\${prefix}/..
")
    string(CONFIGURE "${PC_CONTENTS}" ROS_PC_CONTENTS @ONLY)
    file(WRITE ${PROJECT_SOURCE_DIR}/${PC_NAME}.pc ${ROS_PC_CONTENTS})
  endif (IS_ROS_PACKAGE)

  # Also set the uninstall target:
  orocos_uninstall_target()

endmacro( orocos_generate_package )

else(OROCOS-RTT_FOUND)
    message(FATAL_ERROR "UseOrocos.cmake file included, but OROCOS-RTT_FOUND not set ! Be sure to run first find_package(OROCOS-RTT) before including this file.")
endif(OROCOS-RTT_FOUND)
