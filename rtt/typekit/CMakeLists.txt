IF (PLUGINS_ENABLE_TYPEKIT)
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/typekit ${HPPS})
  GLOBAL_ADD_INCLUDE( rtt/typekit ${CMAKE_CURRENT_BINARY_DIR}/rtt-typekit-config.h)

  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt/typekit )
  
  #SET_SOURCE_FILES_PROPERTIES( ${CPPS} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")    

  SET(RTT_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
  set(DLL_EXPORT_PREFIX typekit)
  ADD_RTT_TYPEKIT(rtt-typekit ${RTT_VERSION} ${CPPS})

ENDIF (PLUGINS_ENABLE_TYPEKIT)
