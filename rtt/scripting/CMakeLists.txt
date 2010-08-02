
IF (PLUGINS_ENABLE_SCRIPTING)
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/scripting ${HPPS})
  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt/scripting )
           
  SET(RTT_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
  set(DLL_EXPORT_PREFIX scripting)
  ADD_RTT_PLUGIN( rtt-scripting ${RTT_VERSION} ${CPPS})
  GLOBAL_ADD_INCLUDE( rtt/scripting ${CMAKE_CURRENT_BINARY_DIR}/rtt-scripting-config.h)
  

ENDIF (PLUGINS_ENABLE_SCRIPTING)
