

INCLUDE (${PROJ_SOURCE_DIR}/config/TaoIDL.cmake)
INCLUDE (${PROJ_SOURCE_DIR}/config/DependentOption.cmake)

#this option was set in global_rules.cmake
IF(ENABLE_CORBA)
  MESSAGE( "CORBA Support enabled.")

  # Add includes / lib paths if necessary
  IF( NOT ${ACE_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ACE_DIR} )
    LINK_DIRECTORIES( ${ACE_DIR}/lib )
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${ACE_DIR}" CACHE INTERNAL "")
    SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -L${ACE_DIR}/lib" CACHE INTERNAL "")
  ENDIF( NOT ${ACE_DIR} STREQUAL /usr/include )
  IF( NOT ${TAO_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${TAO_DIR} )
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${TAO_DIR}" CACHE INTERNAL "")
  ENDIF( NOT ${TAO_DIR} STREQUAL /usr/include )
  IF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ORBSVCS_DIR} )
    SET(RTT_CFLAGS "${RTT_CFLAGS} -I${ORBSVCS_DIR}" CACHE INTERNAL "")
  ENDIF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )
  # Is used for building  the library
  LINK_LIBRARIES( TAO TAO_IDL_BE TAO_PortableServer TAO_CosNaming ACE  )

  FILE( GLOB IDLS [^.]*.idl )
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  # exclude some files:
  LIST( REMOVE_ITEM HPPS CorbaDataObject.hpp CorbaBuffer.hpp)

  ORO_ADD_CORBA_SERVERS(CPPS HPPS ${IDLS} )
  SET( ENV{GLOBAL_GENERATED_SRCS} "${CPPS}" )
  #MESSAGE("CPPS: $ENV{GLOBAL_GENERATED_SRCS}")
  #MESSAGE("HPPS: ${HPPS}")
  GLOBAL_ADD_INCLUDE( rtt/corba ${HPPS})
  #GLOBAL_ADD_SRC( ${CPPS} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  IF ( BUILD_STATIC )
    ADD_LIBRARY(orocos-rtt-corba STATIC ${CPPS})
  ELSE( BUILD_STATIC )
    ADD_LIBRARY(orocos-rtt-corba SHARED ${CPPS})
    SET_TARGET_PROPERTIES( orocos-rtt-corba PROPERTIES
      DEFINE_SYMBOL "RTT_DLL_EXPORT"
      SOVERSION "${RTT_VERSION}"
      )
  ENDIF ( BUILD_STATIC )

  SET_TARGET_PROPERTIES( orocos-rtt-corba PROPERTIES
      COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD} ${RTT_CFLAGS} -D_REENTRANT"
      )

  INSTALL_TARGETS(/lib orocos-rtt-corba)

ENDIF(ENABLE_CORBA)