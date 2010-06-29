
IF (PLUGINS_ENABLE_MARSHALLING)
  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(${PROJ_BINARY_DIR}/rtt/marsh )

  GLOBAL_ADD_INCLUDE( rtt/marsh PropertyMarshaller.hpp PropertyDemarshaller.hpp MarshallInterface.hpp )
  SET( CPPS PropertyMarshaller.cpp PropertyDemarshaller.cpp PropertyBagIntrospector.cpp)

  GLOBAL_ADD_INCLUDE( rtt/marsh CPFMarshaller.hpp
           XMLRPCDemarshaller.hpp XMLRPCMarshaller.hpp CPFDTD.hpp
           StreamProcessor.hpp Marshalling.hpp PropertyLoader.hpp)
  list(APPEND CPPS CPFDTD.cpp CPFMarshaller.cpp Marshalling.cpp MarshallingService.cpp PropertyLoader.cpp)

  IF (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
    GLOBAL_ADD_INCLUDE( rtt/marsh CPFDemarshaller.hpp)
    list(APPEND CPPS  CPFDemarshaller.cpp )
  ELSE (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
    GLOBAL_ADD_INCLUDE( rtt/marsh TinyDemarshaller.hpp )
    list(APPEND CPPS  TinyDemarshaller.cpp tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp )
  ENDIF (XERCES_FOUND AND NOT OS_NOEXCEPTIONS)
  
  ADD_LIBRARY(rtt-marshalling-dynamic_${OROCOS_TARGET} SHARED ${CPPS})
  SET_TARGET_PROPERTIES( rtt-marshalling-dynamic_${OROCOS_TARGET} PROPERTIES
    DEFINE_SYMBOL "RTT_DLL_EXPORT"
    SOVERSION "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
    VERSION "${RTT_VERSION}"
    OUTPUT_NAME rtt-marshalling-${OROCOS_TARGET}
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}"
    LINK_FLAGS "-Wl,-zdefs"
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    CLEAN_DIRECT_OUTPUT 1)
  target_link_libraries(rtt-marshalling-dynamic_${OROCOS_TARGET} orocos-rtt-dynamic_${OROCOS_TARGET})
  
  install(TARGETS rtt-marshalling-dynamic_${OROCOS_TARGET}
          ARCHIVE DESTINATION lib/orocos/plugins
          LIBRARY DESTINATION lib/orocos/plugins )

  get_target_property(PLUGINLIB_DIR rtt-marshalling-dynamic_${OROCOS_TARGET} LOCATION)

  add_custom_command(TARGET rtt-marshalling-dynamic_${OROCOS_TARGET} POST_BUILD 
          COMMAND cmake -E copy ${PLUGINLIB_DIR} ${PROJ_BINARY_DIR}/rtt/plugins)
  
ENDIF(PLUGINS_ENABLE_MARSHALLING)
