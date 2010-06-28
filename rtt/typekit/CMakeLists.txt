
IF (PLUGINS_ENABLE_TYPEKIT)
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/typekit ${HPPS})
  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt/typekit )
  
  ADD_LIBRARY(rtt-typekit-dynamic_${OROCOS_TARGET} SHARED ${CPPS})
  SET_TARGET_PROPERTIES( rtt-typekit-dynamic_${OROCOS_TARGET} PROPERTIES
    DEFINE_SYMBOL "RTT_DLL_EXPORT"
    SOVERSION "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
    VERSION "${RTT_VERSION}"
    OUTPUT_NAME rtt-typekit-${OROCOS_TARGET}
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}"
    LINK_FLAGS "-Wl,-zdefs"
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    CLEAN_DIRECT_OUTPUT 1)
  target_link_libraries(rtt-typekit-dynamic_${OROCOS_TARGET} orocos-rtt-dynamic_${OROCOS_TARGET})
  
  install(TARGETS rtt-typekit-dynamic_${OROCOS_TARGET}
          LIBRARY DESTINATION lib/orocos/types )

  get_target_property(TYPEKITLIB_DIR rtt-typekit-dynamic_${OROCOS_TARGET} LOCATION)

  add_custom_command(TARGET rtt-typekit-dynamic_${OROCOS_TARGET} POST_BUILD 
          COMMAND cmake -E copy ${TYPEKITLIB_DIR} ${PROJ_BINARY_DIR}/rtt/types)
  
  SET_SOURCE_FILES_PROPERTIES( ${SRCS} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")

ENDIF (PLUGINS_ENABLE_TYPEKIT)
