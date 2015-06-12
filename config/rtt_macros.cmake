#
# Include and link against required stuff
#

#
# RTT supplies header files which should be included when 
# using RTT. Each subdir should use this macro
# to supply its header-files.
#
# Usage: GLOBAL_ADD_INCLUDE( RELATIVE_LOCATION hpp1, hpp2 ...)
MACRO( GLOBAL_ADD_INCLUDE COMPONENT_LOCATION )
  INSTALL(FILES ${ARGN} DESTINATION include/${COMPONENT_LOCATION} COMPONENT headers)
ENDMACRO( GLOBAL_ADD_INCLUDE COMPONENT_LOCATION )

#
# Sources should add themselves by calling 'GLOBAL_ADD_SRC' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
#
# This gives a centralised location where all sources are registered
# and lets us make the library in just one place.
#
#
# Usage: GLOBAL_ADD_SRC( src1 src2 src3 )
#
MACRO( GLOBAL_ADD_SRC )
  STRING(LENGTH "${ARGN}" NOTEMPTY)
  IF(NOTEMPTY)
    SET (ENV{GLOBAL_LIBRARY_SRCS} "$ENV{GLOBAL_LIBRARY_SRCS}${ARGN};" )
  ENDIF(NOTEMPTY) 
ENDMACRO( GLOBAL_ADD_SRC )

#
# Usage: ADD_RTT_TYPEKIT( name version file1.cpp [file2.cpp...] )
#
# Requires: OROCOS_TARGET, 
#           PROJ_BINARY_DIR (optional, if given plugin is also installed in PROJ_BINARY_DIR/orocos/types ),
#           RTT_DEFINITIONS (optional, added as COMPILE_DEFINITIONS to target's properties).
#
# The resulting CMake target's name is '${name}-${OROCOS_TARGET}_plugin'
# The resulting filename is '${name}-${OROCOS_TARGET}[.dll|.so|...]'
#
macro(ADD_RTT_TYPEKIT name version)
  ADD_LIBRARY(${name}-${OROCOS_TARGET}_plugin SHARED ${ARGN})
  SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES
    VERSION "${version}"
    OUTPUT_NAME ${name}-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${RTT_DEFINITIONS}"
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX};${CMAKE_INSTALL_PREFIX}/lib;${AC_INSTALL_DIR}"
	INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/types"
    CLEAN_DIRECT_OUTPUT 1)
  IF (UNIX AND NOT APPLE)
    SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES
         LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()
  if (DLL_EXPORT_PREFIX)
    string(TOUPPER ${DLL_EXPORT_PREFIX} UDLL_EXPORT_PREFIX )
    set(DEFINE_PREFIX "RTT_${UDLL_EXPORT_PREFIX}_DLL_EXPORT")
    SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES DEFINE_SYMBOL "${DEFINE_PREFIX}")
  endif()

  target_link_libraries(${name}-${OROCOS_TARGET}_plugin orocos-rtt-${OROCOS_TARGET}_dynamic)

  if(DLL_EXPORT_PREFIX)
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/rtt-${DLL_EXPORT_PREFIX}-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/rtt-${DLL_EXPORT_PREFIX}-config.h @ONLY)
  endif()

  # Note: typkits don't get the symlinks
  install(TARGETS ${name}-${OROCOS_TARGET}_plugin
          EXPORT              ${LIBRARY_EXPORT_FILE}
          ARCHIVE DESTINATION lib/orocos${OROCOS_SUFFIX}/types
          LIBRARY DESTINATION lib/orocos${OROCOS_SUFFIX}/types
          RUNTIME DESTINATION lib/orocos${OROCOS_SUFFIX}/types)

  get_target_property(TYPEKITLIB_DIR ${name}-${OROCOS_TARGET}_plugin LOCATION)
  get_target_property(DEBUG_TYPEKITLIB_DIR ${name}-${OROCOS_TARGET}_plugin DEBUG_LOCATION)

  # Only copy if it was built:
  if (WIN32)
      # In win32, we don't know which one will be built, so we prefix it with 'if exist', which is not portable.
      # Only the first comment is displayed, so its a non-filename specific comment.
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/types"
          COMMAND if exist ${TYPEKITLIB_DIR} ${CMAKE_COMMAND} -E copy "${TYPEKITLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/types"
          COMMENT "Copying ${name}-${OROCOS_TARGET}_plugin to ${PROJ_BINARY_DIR}/rtt/types" VERBATIM)
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/types"
          COMMAND if exist ${DEBUG_TYPEKITLIB_DIR} ${CMAKE_COMMAND} -E copy "${DEBUG_TYPEKITLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/types"
          VERBATIM)
  else()
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/types"
          COMMAND ${CMAKE_COMMAND} -E copy "${TYPEKITLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/types"
          COMMENT "Copying ${TYPEKITLIB_DIR} to ${PROJ_BINARY_DIR}/rtt/types" VERBATIM)
  endif()
  
endmacro(ADD_RTT_TYPEKIT name)

#
# Usage: ADD_RTT_PLUGIN( name version file1.cpp [file2.cpp...] )
#
# Requires: OROCOS_TARGET, 
#           PROJ_BINARY_DIR (optional, if given plugin is also installed in PROJ_BINARY_DIR/orocos/plugins ),
#           RTT_DEFINITIONS (optional, added as COMPILE_DEFINITIONS to target's properties).
#           DLL_EXPORT_PREFIX (optional, defines the symbol RTT_${UDLL_EXPORT_PREFIX}_DLL_EXPORT used for DLL import/export symbols on win32 
#
# The resulting CMake target's name is '${name}-${OROCOS_TARGET}_plugin'
# The resulting filename is '${name}-${OROCOS_TARGET}[.dll|.so|...]'
#
macro(ADD_RTT_PLUGIN name version)
  ADD_LIBRARY(${name}-${OROCOS_TARGET}_plugin SHARED ${ARGN})
  SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES
    VERSION "${version}"
    OUTPUT_NAME ${name}-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${RTT_DEFINITIONS}"
    INSTALL_RPATH_USE_LINK_PATH 1
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/types;${CMAKE_INSTALL_PREFIX}/lib;${AC_INSTALL_DIR}"
	INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib/orocos${OROCOS_SUFFIX}/plugins"
    CLEAN_DIRECT_OUTPUT 1)
  IF (UNIX AND NOT APPLE)
    SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES
         LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()

  if (DLL_EXPORT_PREFIX)
    string(TOUPPER ${DLL_EXPORT_PREFIX} UDLL_EXPORT_PREFIX )
    set(DEFINE_PREFIX "RTT_${UDLL_EXPORT_PREFIX}_DLL_EXPORT")
    SET_TARGET_PROPERTIES( ${name}-${OROCOS_TARGET}_plugin PROPERTIES DEFINE_SYMBOL "${DEFINE_PREFIX}")
  endif()

  target_link_libraries(${name}-${OROCOS_TARGET}_plugin orocos-rtt-${OROCOS_TARGET}_dynamic)
  
  if(DLL_EXPORT_PREFIX)
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/rtt-${DLL_EXPORT_PREFIX}-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/rtt-${DLL_EXPORT_PREFIX}-config.h @ONLY)
  endif()

  # Note: plugins do get the symlinks
  install(TARGETS ${name}-${OROCOS_TARGET}_plugin
          ARCHIVE DESTINATION lib/orocos${OROCOS_SUFFIX}/plugins
          LIBRARY DESTINATION lib/orocos${OROCOS_SUFFIX}/plugins
          RUNTIME DESTINATION lib/orocos${OROCOS_SUFFIX}/plugins)

  get_target_property(PLUGINLIB_DIR ${name}-${OROCOS_TARGET}_plugin LOCATION)
  get_target_property(DEBUG_PLUGINLIB_DIR ${name}-${OROCOS_TARGET}_plugin DEBUG_LOCATION)

  if (WIN32)
      # In win32, we don't know which one will be built, so we prefix it with 'if exist', which is not portable.
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/plugins"
          COMMAND if exist ${PLUGINLIB_DIR} ${CMAKE_COMMAND} -E copy "${PLUGINLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/plugins"
          COMMENT "Copying ${name}-${OROCOS_TARGET}_plugin to ${PROJ_BINARY_DIR}/rtt/plugins" VERBATIM)
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/plugins"
          COMMAND if exist ${DEBUG_PLUGINLIB_DIR} ${CMAKE_COMMAND} -E copy "${DEBUG_PLUGINLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/plugins"
          VERBATIM)
  else()
      add_custom_command(TARGET ${name}-${OROCOS_TARGET}_plugin POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJ_BINARY_DIR}/rtt/plugins"
          COMMAND ${CMAKE_COMMAND} -E copy "${PLUGINLIB_DIR}" "${PROJ_BINARY_DIR}/rtt/plugins"
          COMMENT "Copying ${PLUGINLIB_DIR} to ${PROJ_BINARY_DIR}/rtt/plugins" VERBATIM)
  endif()
  
endmacro(ADD_RTT_PLUGIN name)

