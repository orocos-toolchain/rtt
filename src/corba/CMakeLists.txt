

INCLUDE (${PROJ_SOURCE_DIR}/config/TaoIDL.cmake)
INCLUDE (${PROJ_SOURCE_DIR}/config/DependentOption.cmake)

#this option was set in global_rules.cmake
IF(ENABLE_CORBA)
  MESSAGE( "CORBA Support enabled.")

  FILE( GLOB IDLS [^.]*.idl )
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  ORO_ADD_CORBA_SERVERS(CPPS HPPS ${IDLS} )
  SET( ENV{GLOBAL_GENERATED_SRCS} "${CPPS}" )
  #MESSAGE("CPPS: $ENV{GLOBAL_GENERATED_SRCS}")
  #MESSAGE("HPPS: ${HPPS}")
  GLOBAL_ADD_INCLUDE( rtt/corba ${HPPS})
  #GLOBAL_ADD_SRC( ${CPPS} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

  IF ( BUILD_STATIC )
    ADD_LIBRARY(orocos-rtt-corba-static STATIC ${CPPS})
    SET_TARGET_PROPERTIES(orocos-rtt-corba-static
      PROPERTIES OUTPUT_NAME orocos-rtt-corba
      CLEAN_DIRECT_OUTPUT 1)
    INSTALL_TARGETS(/lib orocos-rtt-corba-static)
  ENDIF ( BUILD_STATIC )

  IF ( BUILD_DYNAMIC )
    #MESSAGE("CPPS: ${CPPS}")
    ADD_LIBRARY(orocos-rtt-corba-dynamic SHARED ${CPPS})
    SET_TARGET_PROPERTIES(orocos-rtt-corba-dynamic
      PROPERTIES OUTPUT_NAME orocos-rtt-corba
      CLEAN_DIRECT_OUTPUT 1
      SOVERSION ${VERSION} )
    INSTALL_TARGETS(/lib orocos-rtt-corba-dynamic)
  ENDIF ( BUILD_DYNAMIC )

ENDIF(ENABLE_CORBA)