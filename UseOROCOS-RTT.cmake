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

cmake_minimum_required(VERSION 2.8.3)

if(OROCOS-RTT_FOUND)
  include(FindPkgConfig)
  include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT-helpers.cmake)
  # Include directories
  include_directories(${OROCOS-RTT_INCLUDE_DIRS})

  # Preprocessor definitions
  add_definitions(${OROCOS-RTT_DEFINITIONS})

  # Check for client meta-buildsystem tools
  # 
  # Tool support for:
  #   - Catkin
  #   - rosbuild
  #
  # If the client is using rosbuild, and has called rosbuild_init(), then we
  # will assume that he or she wants to build targets with rosbuild libraries.
  # 
  # If the client has not called rosbuild_init() then we check if they have
  # called `find_package(catkin ...)` if they have, and catkin has been found,
  # then we can assume this is a catkin build.
  #
  if(COMMAND rosbuild_init AND ROSBUILD_init_called)
    message(STATUS "[UseOrocos] Building package ${PROJECT_NAME} with rosbuild macros because rosbuild_init() has been called.")
    set(ORO_USE_ROSBUILD True CACHE BOOL "Build packages with rosbuild in-source support.")
    # TODO: Uncomment the following if we want to force people to call rosbuild_init
    # if the function is available
    #if ( NOT ROSBUILD_init_called )
    #  if (NOT DEFINED ROSBUILD_init_called )
    #    include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake) # Prevent double inclusion ! This file is not robust against that !
    #  endif()
    #  rosbuild_init()
    #endif()
  elseif(catkin_FOUND)
    message(STATUS "[UseOrocos] Building package ${PROJECT_NAME} with catkin develspace support.")
    set(ORO_USE_CATKIN True CACHE BOOL "Build packages with Catkin develspace support.")
  else()
    message(STATUS "[UseOrocos] Building package ${PROJECT_NAME} without an external buildtool like rosbuild or Catkin")
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

  message(STATUS "[UseOrocos] Using Orocos RTT in ${PROJECT_NAME}")

  # Set to true to indicate that these macros are available.
  set(USE_OROCOS_RTT 1)

  # By default, install libs in /target/ subdir in order to allow
  # multi-target installs.
  if ( NOT DEFINED OROCOS_SUFFIX )
    set (OROCOS_SUFFIX "/${OROCOS_TARGET}")
  endif()

  if (ORO_USE_ROSBUILD)
    # Infer package name from directory name.
    get_filename_component(ORO_ROSBUILD_PACKAGE_NAME ${PROJECT_SOURCE_DIR} NAME)

    # Modify default rosbuild output paths if using Eclipse
    if (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")
      message(WARNING "[UseOrocos] Eclipse Generator detected. I'm setting EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH")
      message(WARNING "[UseOrocos] This will not affect the real output paths of libraries and executables!")
      #set the default path for built executables to the "bin" directory
      set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
      #set the default path for built libraries to the "lib" directory
      set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
    endif()

    # Set output directories for rosbuild in-source builds,
    # but respect deprecated LIBRARY_OUTPUT_PATH, EXECUTABLE_OUTPUT_PATH and ARCHIVE_OUTPUT_PATH variables
    # as they are set by rosbuild_init() and commonly used in rosbuild CMakeLists.txt files
    if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
      if(DEFINED LIBRARY_OUTPUT_PATH)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
      else()
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
      endif()
    endif()
    if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      if(DEFINED EXECUTABLE_OUTPUT_PATH)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
      else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
      endif()
    endif()
    if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
      if(DEFINED ARCHIVE_OUTPUT_PATH)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_PATH})
      else()
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
      endif()
    endif()
    set(ORO_COMPONENT_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
    set(ORO_TYPEKIT_OUTPUT_DIRECTORY   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
    set(ORO_PLUGIN_OUTPUT_DIRECTORY    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins)

    # We only need the direct dependencies, the rest is resolved by the .pc
    # files.
    rosbuild_invoke_rospack(${ORO_ROSBUILD_PACKAGE_NAME} pkg DEPS depends1)
    string(REGEX REPLACE "\n" ";" pkg_DEPS2 "${pkg_DEPS}" )
    foreach(ROSDEP ${pkg_DEPS2})
      orocos_use_package( ${ROSDEP} )
    endforeach(ROSDEP ${pkg_DEPS2})

  elseif(ORO_USE_CATKIN)
    # Disable auto-linking
    set(OROCOS_NO_AUTO_LINKING True)

    # Parse package.xml file
    if(NOT _CATKIN_CURRENT_PACKAGE)
      catkin_package_xml()
    endif()

    # Set output directories for catkin
    catkin_destinations()
    set(ORO_COMPONENT_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
    set(ORO_TYPEKIT_OUTPUT_DIRECTORY   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
    set(ORO_PLUGIN_OUTPUT_DIRECTORY    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins)

    # Make sure a devel prefix's pkgconfig path is available if it hasn't already been source'd
    if (NOT $ENV{PKG_CONFIG_PATH} MATCHES "${CATKIN_DEVEL_PREFIX}/lib/pkgconfig")
      set(ENV{PKG_CONFIG_PATH} "${CATKIN_DEVEL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    endif()
    if("$ENV{VERBOSE}")
      message(STATUS "[UseOrocos] PKG_CONFIG_PATH: $ENV{PKG_CONFIG_PATH}")
    endif()

    # Get catkin build_depend dependencies
    orocos_get_catkin_deps( DEPS )
    #message("orocos_get_manifest_deps are: ${DEPS}")
    foreach(DEP ${DEPS})
      # We use OROCOS_ONLY so that we only find .pc files with the orocos target on them
      orocos_use_package( ${DEP} OROCOS_ONLY) 
    endforeach(DEP ${DEPS}) 

  else()
    # Set output directories relative to CMAKE_LIBRARY_OUTPUT_DIRECTORY or built in the current binary directory (cmake default).
    if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
      set(ORO_COMPONENT_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
      set(ORO_TYPEKIT_OUTPUT_DIRECTORY   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
      set(ORO_PLUGIN_OUTPUT_DIRECTORY    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins)
    else()
      set(ORO_COMPONENT_OUTPUT_DIRECTORY orocos${OROCOS_SUFFIX}/${PROJECT_NAME})
      set(ORO_TYPEKIT_OUTPUT_DIRECTORY   orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types)
      set(ORO_PLUGIN_OUTPUT_DIRECTORY    orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins)
    endif()

    # Fall back to manually processing the Autoproj manifest.xml file.
    orocos_get_manifest_deps( DEPS )
    #message("orocos_get_manifest_deps are: ${DEPS}")
    foreach(DEP ${DEPS})
      orocos_use_package( ${DEP} ) 
    endforeach(DEP ${DEPS}) 
  endif()

  # Output the library and runtime destinations
  if("$ENV{VERBOSE}")
    message(STATUS "[UseOrocos] Building library targets in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    message(STATUS "[UseOrocos] Building runtime targets in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  endif()

  # Necessary for correctly building mixed libraries on win32.
  if(OROCOS_TARGET STREQUAL "win32")
    set(CMAKE_DEBUG_POSTFIX "d")
  endif(OROCOS_TARGET STREQUAL "win32")

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
    if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32" OR ${OROCOS_TARGET} STREQUAL "macosx")
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

    # Clear the dependencies such that a target switch can be detected:
    unset( ${COMPONENT_NAME}_LIB_DEPENDS )

    # Use rosbuild in ros environments:
    if (ORO_USE_ROSBUILD)
      MESSAGE( STATUS "[UseOrocos] Building component ${COMPONENT_NAME} in library ${COMPONENT_LIB_NAME} in rosbuild source tree." )
      rosbuild_add_library(${COMPONENT_NAME} ${SOURCES} )
    else()
      MESSAGE( STATUS "[UseOrocos] Building component ${COMPONENT_NAME} in library ${COMPONENT_LIB_NAME}" )
      ADD_LIBRARY( ${COMPONENT_NAME} SHARED ${SOURCES} )
    endif()

    # Prepare component lib for out-of-the-ordinary lib directories
    SET_TARGET_PROPERTIES( ${COMPONENT_NAME} PROPERTIES
      OUTPUT_NAME ${COMPONENT_LIB_NAME}
      LIBRARY_OUTPUT_DIRECTORY ${ORO_COMPONENT_OUTPUT_DIRECTORY}
      DEFINE_SYMBOL "RTT_COMPONENT"
      ${LIB_COMPONENT_VERSION}
      INSTALL_RPATH_USE_LINK_PATH 1
      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME};${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
      )
    if(APPLE)
      SET_TARGET_PROPERTIES( ${COMPONENT_NAME} PROPERTIES
	INSTALL_NAME_DIR "@rpath"
	LINK_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX},-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
	)
    endif()
    orocos_add_compile_flags( ${COMPONENT_NAME} ${USE_OROCOS_COMPILE_FLAGS})
    orocos_add_link_flags( ${COMPONENT_NAME} ${USE_OROCOS_LINK_FLAGS})
    TARGET_LINK_LIBRARIES( ${COMPONENT_NAME}
      ${OROCOS-RTT_LIBRARIES} 
      #${OROCOS-RTT_TYPEKIT_LIBRARIES} 
      )

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
    list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS "${COMPONENT_NAME}")
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
  
  if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32" OR ${OROCOS_TARGET} STREQUAL "macosx")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
    else()
      set( LIB_NAME ${LIB_TARGET_NAME})
    endif()

    # Clear the dependencies such that a target switch can be detected:
    unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

    if (ORO_USE_ROSBUILD)
      MESSAGE( STATUS "[UseOrocos] Building library ${LIB_TARGET_NAME} in rosbuild source tree." )
      rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
    else()
      MESSAGE( STATUS "[UseOrocos] Building library ${LIB_TARGET_NAME}" )
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
    orocos_add_compile_flags( ${LIB_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS} )
    orocos_add_link_flags( ${LIB_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS} )
    TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} 
      ${OROCOS-RTT_LIBRARIES} 
      #${OROCOS-RTT_TYPEKIT_LIBRARIES} 
      )
    if(APPLE)
      SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
	INSTALL_NAME_DIR "@rpath"
	LINK_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
	)
    endif()

    INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})

    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

    # Necessary for .pc file generation
    list(APPEND OROCOS_DEFINED_LIBS " -l${LIB_NAME}")
    list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS "${LIB_TARGET_NAME}")
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

    if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32" OR ${OROCOS_TARGET} STREQUAL "macosx")
      set( EXE_NAME ${EXE_TARGET_NAME}-${OROCOS_TARGET})
    else()
      set( EXE_NAME ${EXE_TARGET_NAME})
    endif()

    if (ORO_USE_ROSBUILD)
      MESSAGE( STATUS "[UseOrocos] Building executable ${EXE_TARGET_NAME} in rosbuild source tree." )
      rosbuild_add_executable(${EXE_TARGET_NAME} ${SOURCES} )
    else()
      MESSAGE( STATUS "[UseOrocos] Building executable ${EXE_TARGET_NAME}" )
      ADD_EXECUTABLE( ${EXE_TARGET_NAME} ${SOURCES} )
    endif()

    SET_TARGET_PROPERTIES( ${EXE_TARGET_NAME} PROPERTIES
      OUTPUT_NAME ${EXE_NAME}
      INSTALL_RPATH_USE_LINK_PATH 1
      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/bin;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
      )
    if(APPLE)
      SET_TARGET_PROPERTIES( ${EXE_TARGET_NAME} PROPERTIES
	INSTALL_NAME_DIR "@rpath"
	LINK_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/bin,-rpath,${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
	)
    endif()

    if(CMAKE_DEBUG_POSTFIX)
      set_target_properties( ${EXE_TARGET_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} )
    endif(CMAKE_DEBUG_POSTFIX)
    orocos_add_compile_flags(${EXE_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS})
    orocos_add_link_flags(${EXE_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS})

    TARGET_LINK_LIBRARIES( ${EXE_TARGET_NAME} 
      ${OROCOS-RTT_LIBRARIES} 
      )

    # We install the exe, the user must make sure that the install dir is not
    # beneath the ROS package (if any).
    INSTALL(TARGETS ${EXE_TARGET_NAME} RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})

    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  endmacro( orocos_executable )

  # Type headers should add themselves by calling 'orocos_typegen_headers()'
  # They will be processed by typegen to generate a typekit from it, with the
  # name of the current project. You may also pass additional options to typegen
  # before listing your header files. 
  # 
  # Use 'DEPENDS <packagename> ...' to add dependencies on other (typegen) packages.
  # This macro passes the -x OROCOS_TARGET flag to typegen automatically, so there
  # is no need to include the -OROCOS_TARGET suffix in the <packagename>
  #
  # NOTE: if you use a subdir for your headers, e.g. include/robotdata.hpp, it
  # will install this header into pkgname/include/robotdata.hpp ! Most likely
  # not what you want. So call this macro from the include dir itself.
  #
  # Usage: orocos_typegen_headers( robotdata.hpp sensordata.hpp DEPENDS orocos_kdl )
  #
  macro( orocos_typegen_headers )
    ORO_PARSE_ARGUMENTS(ORO_TYPEGEN_HEADERS
      "DEPENDS"
      ""
      ${ARGN}
      )

    if ( ORO_TYPEGEN_HEADERS_DEPENDS )
      set (ORO_TYPEGEN_HEADERS_DEP_INFO_MSG "using: ${ORO_TYPEGEN_HEADERS_DEP_INFO_MSG}")
    endif()
    MESSAGE( STATUS "[UseOrocos] Generating typekit for ${PROJECT_NAME} ${ORO_TYPEGEN_HEADERS_DEP_INFO_MSG}..." )

    # Works in top level source dir:
    set(TYPEGEN_EXE typegen-NOTFOUND) #re-check for typegen each time !
    find_program(TYPEGEN_EXE typegen)
    if (NOT TYPEGEN_EXE)
      message(FATAL_ERROR "'typegen' not found in path. Can't build typekit. Did you 'source env.sh' ?")
    else (NOT TYPEGEN_EXE)

      foreach( IMP ${ORO_TYPEGEN_HEADERS_DEPENDS} )
        set(ORO_TYPEGEN_HEADERS_IMPORTS  "${ORO_TYPEGEN_HEADERS_IMPORTS} -i ${IMP}" )
      endforeach()
      # Working directory is necessary to be able to find the source files.
      execute_process( COMMAND ${TYPEGEN_EXE} --output ${PROJECT_SOURCE_DIR}/typekit ${PROJECT_NAME} ${ORO_TYPEGEN_HEADERS_IMPORTS} ${ORO_TYPEGEN_HEADERS_DEFAULT_ARGS} 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
        )
      # work around generated manifest.xml file:
      #execute_process( COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/typekit/manifest.xml )
      add_subdirectory( ${PROJECT_SOURCE_DIR}/typekit ${PROJECT_BINARY_DIR}/typekit)

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

    if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32" OR ${OROCOS_TARGET} STREQUAL "macosx")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
    else()
      set( LIB_NAME ${LIB_TARGET_NAME})
    endif()

    # Clear the dependencies such that a target switch can be detected:
    unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

    MESSAGE( STATUS "[UseOrocos] Building typekit library ${LIB_TARGET_NAME}" )
    if (ORO_USE_ROSBUILD)
      rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
    else()
      ADD_LIBRARY( ${LIB_TARGET_NAME} SHARED ${SOURCES} )
    endif()
    SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
      OUTPUT_NAME ${LIB_NAME}
      LIBRARY_OUTPUT_DIRECTORY ${ORO_TYPEKIT_OUTPUT_DIRECTORY}
      ${LIB_COMPONENT_VERSION}
      INSTALL_RPATH_USE_LINK_PATH 1
      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/types;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
      )
    if(APPLE)
      SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
	INSTALL_NAME_DIR "@rpath"
	LINK_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/types,-rpath,${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
	)
    endif()
    TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} 
      ${OROCOS-RTT_LIBRARIES} 
      )

    # On win32, typekit runtime (.dll) should go in orocos/types folder
    if( ${OROCOS_TARGET} STREQUAL "win32" )
      INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_DIR})
    else()
      INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})
    endif()

    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

    # Necessary for .pc file generation
    list(APPEND OROCOS_DEFINED_TYPES " -l${LIB_NAME}")
    list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS "${LIB_TARGET_NAME}")
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

    if ( ${OROCOS_TARGET} STREQUAL "gnulinux" OR ${OROCOS_TARGET} STREQUAL "lxrt" OR ${OROCOS_TARGET} STREQUAL "xenomai" OR ${OROCOS_TARGET} STREQUAL "win32" OR ${OROCOS_TARGET} STREQUAL "macosx")
      set( LIB_NAME ${LIB_TARGET_NAME}-${OROCOS_TARGET})
    else()
      set( LIB_NAME ${LIB_TARGET_NAME})
    endif()

    # Clear the dependencies such that a target switch can be detected:
    unset( ${LIB_TARGET_NAME}_LIB_DEPENDS )

    if (ORO_USE_ROSBUILD)
      MESSAGE( STATUS "[UseOrocos] Building plugin library ${LIB_TARGET_NAME} in rosbuild source tree." )
      rosbuild_add_library(${LIB_TARGET_NAME} ${SOURCES} )
    else()
      MESSAGE( STATUS "[UseOrocos] Building plugin library ${LIB_TARGET_NAME}" )
      ADD_LIBRARY( ${LIB_TARGET_NAME} SHARED ${SOURCES} )
    endif()

    SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
      OUTPUT_NAME ${LIB_NAME}
      LIBRARY_OUTPUT_DIRECTORY ${ORO_PLUGIN_OUTPUT_DIRECTORY}
      ${LIB_COMPONENT_VERSION}
      INSTALL_RPATH_USE_LINK_PATH 1
      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}/plugins;${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
      )
    if(APPLE)
      SET_TARGET_PROPERTIES( ${LIB_TARGET_NAME} PROPERTIES
	INSTALL_NAME_DIR "@rpath"
	LINK_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/plugins,-rpath,${CMAKE_INSTALL_PREFIX}/${AC_INSTALL_DIR}"
	)
    endif()
    orocos_add_compile_flags( ${LIB_TARGET_NAME} ${USE_OROCOS_COMPILE_FLAGS})
    orocos_add_link_flags( ${LIB_TARGET_NAME} ${USE_OROCOS_LINK_FLAGS})
    TARGET_LINK_LIBRARIES( ${LIB_TARGET_NAME} 
      ${OROCOS-RTT_LIBRARIES}
      #${OROCOS-RTT_TYPEKIT_LIBRARIES} 
      )

    # On win32, plugins runtime (.dll) should go in orocos/plugins folder
    if( ${OROCOS_TARGET} STREQUAL "win32" )
      INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_DIR})
    else()
      INSTALL(TARGETS ${LIB_TARGET_NAME} LIBRARY DESTINATION ${AC_INSTALL_DIR} ARCHIVE DESTINATION lib RUNTIME DESTINATION ${AC_INSTALL_RT_DIR})
    endif()


    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

    # Necessary for .pc file generation
    list(APPEND OROCOS_DEFINED_PLUGINS " -l${LIB_NAME}")
    list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS "${LIB_TARGET_NAME}")
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
  # Components supply header files and directories which should be included when
  # using these components. Each component should use this macro
  # to install its header-files. They are installed by default
  # in include/orocos/${PROJECT_NAME}
  #
  # Usage example: orocos_install_header(
  #                  FILES hardware.hpp control.hpp
  #                  DIRECTORY include/${PROJECT_NAME}
  #                )
  #
  macro( orocos_install_headers )
    ORO_PARSE_ARGUMENTS(ORO_INSTALL_HEADER
      "INSTALL;FILES;DIRECTORY"
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

    if( ORO_INSTALL_HEADER_FILES )
      install( FILES ${ORO_INSTALL_HEADER_FILES} DESTINATION ${AC_INSTALL_DIR} )
    endif()

    if( ORO_INSTALL_HEADER_DIRECTORY )
      install( DIRECTORY ${ORO_INSTALL_HEADER_DIRECTORY} DESTINATION ${AC_INSTALL_DIR} )
    endif()

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
  # You may specify a dependency list of .pc files of Orocos packages with DEPENDS_TARGETS
  # This is similar to DEPENDS, but the -<target> suffix is added for every package name.
  # This dependency will end up in the Requires: field of the .pc file.
  #
  # orocos_generate_package( [name] [VERSION version] [DEPENDS packagenames....])
  #
  macro( orocos_generate_package )

    oro_parse_arguments(ORO_CREATE_PC
      "VERSION;DEPENDS;DEPENDS_TARGETS;INCLUDE_DIRS"
      ""
      ${ARGN}
      )

    # Check version
    if (NOT ORO_CREATE_PC_VERSION)
      if (COMPONENT_VERSION)
        set( ORO_CREATE_PC_VERSION ${COMPONENT_VERSION})
        message(STATUS "[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION} from COMPONENT_VERSION.")
      elseif (${PROJECT_NAME}_VERSION)
        set( ORO_CREATE_PC_VERSION ${${PROJECT_NAME}_VERSION})
        message(STATUS "[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION} from ${PROJECT_NAME}_VERSION (package.xml).")
      else ()
        set( ORO_CREATE_PC_VERSION "1.0")
        message(STATUS "[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION} (default version).")
      endif (COMPONENT_VERSION)
    else (NOT ORO_CREATE_PC_VERSION)
      message(STATUS "[UseOrocos] Generating package version ${ORO_CREATE_PC_VERSION}.")
    endif (NOT ORO_CREATE_PC_VERSION)

    # Create filename
    if ( ORO_CREATE_PC_DEFAULT_ARGS )
      set(PC_NAME ${ORO_CREATE_PC_DEFAULT_ARGS})
    else ( ORO_CREATE_PC_DEFAULT_ARGS )
      set(PACKAGE_NAME ${PROJECT_NAME} )
      if ( NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${PROJECT_NAME}_SOURCE_DIR )
        # Append -subdir-subdir-... to pc name:
        file(RELATIVE_PATH RELPATH ${${PROJECT_NAME}_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
        string(REPLACE "/" "-" PC_NAME_SUFFIX ${RELPATH} )
        set(PACKAGE_NAME ${PACKAGE_NAME}-${PC_NAME_SUFFIX})
      endif ( NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${PROJECT_NAME}_SOURCE_DIR )
      set(PC_NAME ${PACKAGE_NAME}-${OROCOS_TARGET})
    endif ( ORO_CREATE_PC_DEFAULT_ARGS )

    # Create dependency list
    foreach( DEP ${ORO_CREATE_PC_DEPENDS_TARGETS})
      list(APPEND ORO_CREATE_PC_DEPENDS ${DEP}-${OROCOS_TARGET})
    endforeach()
    string(REPLACE ";" " " ORO_CREATE_PC_DEPENDS "${ORO_CREATE_PC_DEPENDS}")

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
    set(PC_EXTRA_INCLUDE_DIRS "")
    set(PC_COMMENT "# This pkg-config file is for use in an installed system")

    set(PC_CONTENTS "# Orocos pkg-config file generated by orocos_generate_package() 
\@PC_COMMENT\@
prefix=\@PC_PREFIX\@
libdir=\${prefix}/lib
includedir=\${prefix}/include/orocos
orocos_libdir=\@PC_LIB_DIR\@

Name: \@PC_NAME\@
Description: \@PC_NAME\@ package for Orocos
Requires: orocos-rtt-\@OROCOS_TARGET\@ \@ORO_CREATE_PC_DEPENDS\@
Version: \@ORO_CREATE_PC_VERSION\@
\@PC_LIBS\@
Cflags: -I\${includedir} \@PC_EXTRA_INCLUDE_DIRS\@
")

    string(CONFIGURE "${PC_CONTENTS}" INSTALLED_PC_CONTENTS @ONLY)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PC_NAME}.pc ${INSTALLED_PC_CONTENTS})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PC_NAME}.pc DESTINATION lib/pkgconfig )
    #install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml DESTINATION  lib/orocos${OROCOS_SUFFIX}/level0 )

    # Add ORO_CREATE_PC_INCLUDE_DIRS arguments to ${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS
    if(ORO_CREATE_PC_INCLUDE_DIRS)
      foreach(include_dir ${ORO_CREATE_PC_INCLUDE_DIRS})
        if(IS_ABSOLUTE ${include_dir})
          list(APPEND ${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS "${include_dir}")
        else()
          list(APPEND ${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}")
        endif()
      endforeach()

    else()
      # If the directory ${PROJECT_SOURCE_DIR}/include/orocos exists, always export it as a fallback
      if(EXISTS "${PROJECT_SOURCE_DIR}/include/orocos")
        list(APPEND ${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include/orocos")
      endif()
    endif()

    # Generate additional pkg-config files for other build toolchains
    if (ORO_USE_ROSBUILD)
      message(STATUS "[UseOrocos] Generating pkg-config file for rosbuild package.")

      # For ros package trees, we install the .pc file also next to the manifest file:
      set(PC_PREFIX ${PROJECT_SOURCE_DIR})
      #set(PC_LIB_DIR "\${libdir}/orocos${OROCOS_SUFFIX}") # Without package name suffix !
      set(PC_EXTRA_INCLUDE_DIRS "-I\${prefix}/..")
      foreach(include_dir ${${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS})
        if(NOT include_dir STREQUAL "${PC_PREFIX}/include/orocos")
          set(PC_EXTRA_INCLUDE_DIRS "${PC_EXTRA_INCLUDE_DIRS} -I${include_dir}")
        endif()
      endforeach()
        
      set(PC_COMMENT "# This pkg-config file is for use in a rosbuild source tree\n"
        "# Rationale:\n"
        "# - The prefix is equal to the package directory.\n"
        "# - The libdir is where the libraries were built, ie, package/lib\n"
        "# - The include dir in cflags allows top-level headers and in package/include/package/header.h\n"
        "# - If this doesn't fit your package layout, don't use orocos_generate_package() and write the .pc file yourself")

      string(CONFIGURE "${PC_CONTENTS}" ROSBUILD_PC_CONTENTS @ONLY)
      file(WRITE ${PROJECT_SOURCE_DIR}/lib/pkgconfig/${PC_NAME}.pc ${ROSBUILD_PC_CONTENTS})

    elseif (ORO_USE_CATKIN)
      message(STATUS "[UseOrocos] Generating pkg-config file for package in Catkin devel space.")

      # For catkin workspaces we also install a pkg-config file in the develspace
      set(PC_COMMENT "# This pkg-config file is for use in a Catkin devel space")
      set(PC_PREFIX ${CATKIN_DEVEL_PREFIX})
      set(PC_EXTRA_INCLUDE_DIRS "")
      foreach(include_dir ${${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS})
        if(NOT include_dir STREQUAL "${PC_PREFIX}/include/orocos")
          set(PC_EXTRA_INCLUDE_DIRS "${PC_EXTRA_INCLUDE_DIRS} -I${include_dir}")
        endif()
      endforeach()
      #set(PC_LIB_DIR "\${libdir}/orocos${OROCOS_SUFFIX}/${PROJECT_NAME}")

      string(CONFIGURE "${PC_CONTENTS}" CATKIN_PC_CONTENTS @ONLY)
      file(WRITE ${CATKIN_DEVEL_PREFIX}/lib/pkgconfig/${PC_NAME}.pc ${CATKIN_PC_CONTENTS})

    endif()

    # Store a list of exported targets and include directories on the cache so that other packages within the same workspace can link to them.
    if(${PROJECT_NAME}_EXPORTED_TARGETS)
      message(STATUS "[UseOrocos] Exporting targets ${${PROJECT_NAME}_EXPORTED_TARGETS}.")
      set(${PC_NAME}_EXPORTED_OROCOS_TARGETS ${${PROJECT_NAME}_EXPORTED_TARGETS} CACHE INTERNAL "Targets exported by package ${PC_NAME}")
    endif()
    if(${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS)
      message(STATUS "[UseOrocos] Exporting include directories ${${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS}.")
      set(${PC_NAME}_EXPORTED_OROCOS_INCLUDE_DIRS ${${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS} CACHE INTERNAL "Include directories exported by package ${PC_NAME}")
    endif()

    # Also set the uninstall target:
    orocos_uninstall_target()

    # Call catkin_package() here if the user has not called it before.
    if( ORO_USE_CATKIN
        AND NOT ${PROJECT_NAME}_CATKIN_PACKAGE
        AND NOT ORO_CREATE_PC_DEFAULT_ARGS # no package name given in orocos_generate_package()
        AND CMAKE_CURRENT_SOURCE_DIR STREQUAL ${PROJECT_NAME}_SOURCE_DIR
        AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/package.xml" )

      # Always assume that catkin is a buildtool_depend. This silently disables a FATAL_ERROR in catkin_package().
      # See https://github.com/ros/catkin/commit/7482dda520e94db5b532b57220dfefb10eeda15b
      list(APPEND ${PROJECT_NAME}_BUILDTOOL_DEPENDS catkin)

      catkin_package(
        INCLUDE_DIRS ${${PROJECT_NAME}_EXPORTED_INCLUDE_DIRS}
      )
    endif()

  endmacro( orocos_generate_package )

else(OROCOS-RTT_FOUND)
  message(FATAL_ERROR "UseOrocos.cmake file included, but OROCOS-RTT_FOUND not set ! Be sure to run first find_package(OROCOS-RTT) before including this file.")
endif(OROCOS-RTT_FOUND)
