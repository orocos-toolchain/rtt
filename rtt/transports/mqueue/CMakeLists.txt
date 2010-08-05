# this option was set in global_rules.cmake
IF(ENABLE_MQ)
  MESSAGE( "Building MQueue Transport library (Requires Boost >= 1.37.0).")

  find_package(Boost 1.37 REQUIRED serialization)


  FILE( GLOB CPPS Dispatcher.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  #MESSAGE("CPPS: $ENV{GLOBAL_GENERATED_SRCS}")
  #MESSAGE("HPPS: ${HPPS}")
  GLOBAL_ADD_INCLUDE( rtt/transports/mqueue ${HPPS})
  # Due to generation of some .h files in build directories, we also need to include some build dirs in our include paths.
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt/transports/mqueue ${MQ_INCLUDE_DIRS})

  if(NOT MQ_LDFLAGS)
    set(MQ_LDFLAGS "")
  endif()

IF ( BUILD_STATIC )
  ADD_LIBRARY(orocos-rtt-mqueue-${OROCOS_TARGET}_static STATIC ${CPPS})
  SET_TARGET_PROPERTIES( orocos-rtt-mqueue-${OROCOS_TARGET}_static 
  PROPERTIES DEFINE_SYMBOL "RTT_MQ_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-mqueue-${OROCOS_TARGET}
  DEBUG_OUTPUT_NAME orocos-rtt-mqueue-${OROCOS_TARGET}${CMAKE_DEBUG_LIB_SUFFIX}
  CLEAN_DIRECT_OUTPUT 1
  VERSION "${RTT_VERSION}"
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}"
  COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
ENDIF( BUILD_STATIC )

  ADD_LIBRARY(orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic SHARED ${CPPS})
  TARGET_LINK_LIBRARIES(orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic 
	orocos-rtt-${OROCOS_TARGET}_dynamic
	${MQ_LIBRARIES} ${Boost_SERIALIZATION_LIBRARY}
	) 
  SET_TARGET_PROPERTIES( orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic PROPERTIES
  DEFINE_SYMBOL "RTT_MQ_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-mqueue-${OROCOS_TARGET}
  LINK_FLAGS "${MQ_LDFLAGS}"
  CLEAN_DIRECT_OUTPUT 1
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD}"
  COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
  SOVERSION "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
  VERSION "${RTT_VERSION}")

create_pc_flags( "${MQ_DEFINITIONS}" "${MQ_INCLUDE_DIRS}" "${MQ_LIBRARIES}" RTT_MQ_DEFINES RTT_MQ_CFLAGS RTT_MQ_LINKFLAGS)

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/orocos-rtt-mqueue.pc.in ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-mqueue-${OROCOS_TARGET}.pc @ONLY)
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/rtt-mqueue-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/rtt-mqueue-config.h @ONLY)

IF ( BUILD_STATIC )
  INSTALL(TARGETS orocos-rtt-mqueue-${OROCOS_TARGET}_static ARCHIVE DESTINATION lib )
ENDIF( BUILD_STATIC )

  SET(RTT_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
  ADD_RTT_TYPEKIT( rtt-transport-mqueue ${RTT_VERSION} MQLib.cpp)
  target_link_libraries( rtt-transport-mqueue-${OROCOS_TARGET}_plugin orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic)

  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-mqueue-${OROCOS_TARGET}.pc DESTINATION  lib/pkgconfig )
  INSTALL(TARGETS orocos-rtt-mqueue-${OROCOS_TARGET}_dynamic LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtt-mqueue-config.h ${IDLS} DESTINATION include/rtt/transports/mqueue )

ENDIF(ENABLE_MQ)

