IF (PLUGINS_ENABLE_TYPEKIT)
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/typekit ${HPPS})
  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt/typekit )
  
  #SET_SOURCE_FILES_PROPERTIES( ${CPPS} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")    

  SET(RTT_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
  ADD_RTT_TYPEKIT(rtt-typekit ${RTT_VERSION} ${CPPS})

ENDIF (PLUGINS_ENABLE_TYPEKIT)
