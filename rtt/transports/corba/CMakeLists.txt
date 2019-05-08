# this option was set in global_rules.cmake
IF(ENABLE_CORBA)
  MESSAGE( "Building CORBA Transport library.")

  OPTION(PLUGINS_CORBA_TRANSPORT_TYPES "Enable corba transport for RTT typekit types." ON)
  if (NOT PLUGINS_STD_TYPES_SUPPORT)
    ADD_DEFINITIONS( -DRTT_NO_STD_TYPES )
  endif (NOT PLUGINS_STD_TYPES_SUPPORT)

  OPTION(PLUGINS_CORBA_PORTS_DISABLE_SIGNAL "Do not send remote signals for ports to the client side." OFF)
  if (PLUGINS_CORBA_PORTS_DISABLE_SIGNAL)
    MESSAGE("Disabled remote signalling in CORBA Transport library.")
  endif (PLUGINS_CORBA_PORTS_DISABLE_SIGNAL)
  set(RTT_CORBA_PORTS_DISABLE_SIGNAL ${PLUGINS_CORBA_PORTS_DISABLE_SIGNAL})  # for rtt-corba-config.h

  OPTION(PLUGINS_CORBA_PORTS_WRITE_ONEWAY "Use oneway calls for port writes." OFF)
  if (PLUGINS_CORBA_PORTS_WRITE_ONEWAY)
    MESSAGE("Enabled oneway port writes in CORBA Transport library.")
  endif (PLUGINS_CORBA_PORTS_WRITE_ONEWAY)
  set(RTT_CORBA_PORTS_WRITE_ONEWAY ${PLUGINS_CORBA_PORTS_WRITE_ONEWAY})  # for rtt-corba-config.h

  OPTION(PLUGINS_CORBA_SEND_ONEWAY_OPERATIONS "Send oneway operations if collect arity is zero." OFF)
  if (PLUGINS_CORBA_SEND_ONEWAY_OPERATIONS)
    MESSAGE("Enabled oneway operation sending in CORBA Transport library.")
  endif (PLUGINS_CORBA_SEND_ONEWAY_OPERATIONS)
  set(RTT_CORBA_SEND_ONEWAY_OPERATIONS ${PLUGINS_CORBA_SEND_ONEWAY_OPERATIONS})  # for rtt-corba-config.h

  OPTION(PLUGINS_CORBA_NO_CHECK_OPERATIONS "Check operation signature when constructing remote operation callers." OFF)
  if (PLUGINS_CORBA_NO_CHECK_OPERATIONS)
    MESSAGE("Disabled operation signature checking in CORBA Transport library.")
  endif (PLUGINS_CORBA_NO_CHECK_OPERATIONS)
  set(RTT_CORBA_NO_CHECK_OPERATIONS ${PLUGINS_CORBA_NO_CHECK_OPERATIONS})  # for rtt-corba-config.h

  # Clang 2.9 needs this in order to get around undefined symbols of inlined operator>>= functions !
  GET_FILENAME_COMPONENT(CORBA_CXX_NAME "${CMAKE_CXX_COMPILER}" NAME)
  if ( ${CORBA_CXX_NAME} STREQUAL "clang++")
    ADD_DEFINITIONS( -DOMNI_NO_INLINE_FRIENDS )
  endif()

  INCLUDE_DIRECTORIES(BEFORE ${PROJ_SOURCE_DIR} ${PROJ_SOURCE_DIR}/rtt ${PROJ_SOURCE_DIR}/rtt/os $ ${PROJ_SOURCE_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt ${PROJ_BINARY_DIR}/rtt/os ${PROJ_BINARY_DIR}/rtt/os/${OROCOS_TARGET} )
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt/transports/corba ${CORBA_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(BEFORE ${PROJ_BINARY_DIR}/rtt/typekit ) # For rtt-typekit-config.h
  
  LINK_DIRECTORIES(${CORBA_LINK_DIRECTORIES})

#  STRING(REPLACE ";" "  " RTT_CORBA_CFLAGS2 "${CORBA_CFLAGS}")
#  MESSAGE(STATUS "YYY ${RTT_CORBA_CFLAGS2}")
#  SET(RTT_CORBA_CFLAGS "${RTT_CFLAGS} ${RTT_CORBA_CFLAGS2}")
#  MESSAGE(STATUS "YYY ${RTT_CORBA_CFLAGS}")
#  SET(RTT_CORBA_LINKFLAGS ${RTT_CFLAGS} ${CORBA_LDFLAGS})
#  LINK_LIBRARIES( ${CORBA_LIBRARIES} )

  FILE( GLOB IDLS [^.]*.idl )
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  LIST(REMOVE_ITEM CPPS ${CMAKE_CURRENT_SOURCE_DIR}/CorbaLib.cpp)

  ORO_ADD_CORBA_SERVERS(CPPS HPPS ${IDLS} )

  #This has been added in order to generate files before make -j<N>
  # make -j<N> causes corruption of generated files because of overwriting
  ADD_CUSTOM_TARGET( idl DEPENDS ${CPPS} ${HPPS} COMMENT "Generating sources from .idl files")

  #MESSAGE("CPPS: ${CPPS}")
  #MESSAGE("HPPS: ${HPPS}")
  GLOBAL_ADD_INCLUDE( rtt/transports/corba ${HPPS} OrocosTypes.idl)
  #GLOBAL_ADD_SRC( ${CPPS} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  list(APPEND OROCOS-RTT_DEFINITIONS ${CORBA_DEFINITIONS})

IF ( BUILD_STATIC )
  ADD_LIBRARY(orocos-rtt-corba-static_${OROCOS_TARGET} STATIC ${CPPS})
  SET_TARGET_PROPERTIES( orocos-rtt-corba-static_${OROCOS_TARGET} 
  PROPERTIES DEFINE_SYMBOL "RTT_CORBA_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-corba-${OROCOS_TARGET}
  CLEAN_DIRECT_OUTPUT 1
  VERSION "${RTT_VERSION}"
  COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
ENDIF( BUILD_STATIC )

  ADD_LIBRARY(orocos-rtt-corba-${OROCOS_TARGET}_dynamic SHARED ${CPPS})
  TARGET_LINK_LIBRARIES(orocos-rtt-corba-${OROCOS_TARGET}_dynamic 
	orocos-rtt-${OROCOS_TARGET}_dynamic
	${CORBA_LIBRARIES}
	)
  SET_TARGET_PROPERTIES( orocos-rtt-corba-${OROCOS_TARGET}_dynamic PROPERTIES
  DEFINE_SYMBOL "RTT_CORBA_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-corba-${OROCOS_TARGET}
  CLEAN_DIRECT_OUTPUT 1
  COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
  VERSION "${RTT_VERSION}"
  SOVERSION "${RTT_SOVERSION}"
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

create_pc_flags( "${CORBA_DEFINITIONS}" "${CORBA_INCLUDE_DIRS}" "${CORBA_LIBRARIES}" RTT_CORBA_DEFINES RTT_CORBA_CFLAGS RTT_CORBA_LINKFLAGS)
create_pc_flags( "${RTT_DUMMY}" "${RTT_DUMMY}" "${CORBA_USER_LINK_LIBS}" RTT_DUMMY RTT_DUMMY RTT_CORBA_USER_LINKFLAGS)

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/orocos-rtt-corba.pc.in ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-corba-${OROCOS_TARGET}.pc @ONLY)
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/rtt-corba-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/rtt-corba-config.h @ONLY)

IF ( BUILD_STATIC )
  INSTALL(TARGETS             orocos-rtt-corba-static_${OROCOS_TARGET}
          EXPORT              ${LIBRARY_EXPORT_FILE}
          ARCHIVE DESTINATION lib )
ENDIF( BUILD_STATIC )

  if (PLUGINS_CORBA_TRANSPORT_TYPES)
    SET(RTT_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}")
    ADD_RTT_TYPEKIT(rtt-transport-corba ${RTT_VERSION} CorbaLib.cpp)
    target_link_libraries(rtt-transport-corba-${OROCOS_TARGET}_plugin orocos-rtt-corba-${OROCOS_TARGET}_dynamic )
  endif()


  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/orocos-rtt-corba-${OROCOS_TARGET}.pc DESTINATION  lib/pkgconfig )
  INSTALL(TARGETS             orocos-rtt-corba-${OROCOS_TARGET}_dynamic
          EXPORT              ${LIBRARY_EXPORT_FILE}
          ARCHIVE DESTINATION lib
          LIBRARY DESTINATION lib
          RUNTIME DESTINATION bin )
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/TransportPlugin.hpp ${CMAKE_CURRENT_BINARY_DIR}/rtt-corba-config.h ${IDLS} DESTINATION include/rtt/transports/corba )

ENDIF(ENABLE_CORBA)

