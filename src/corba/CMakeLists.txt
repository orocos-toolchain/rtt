

INCLUDE (${PROJ_SOURCE_DIR}/config/TaoIDL.cmake)
INCLUDE (${PROJ_SOURCE_DIR}/config/DependentOption.cmake)

#this option was set in global_rules.cmake
IF(ENABLE_CORBA)
  MESSAGE( "CORBA Support enabled.")

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

  #
  # Set compilation flags:
  #
  IF (${RTT_CXXFLAGS})
  SET_SOURCE_FILES_PROPERTIES( ${CPPS} PROPERTIES
    COMPILE_FLAGS ${RTT_CXXFLAGS}
    )
  ENDIF (${RTT_CXXFLAGS})

  IF ( BUILD_STATIC )
    ADD_LIBRARY(orocos-rtt-corba STATIC ${CPPS})
  ELSE( BUILD_STATIC )
    ADD_LIBRARY(orocos-rtt-corba SHARED ${CPPS})
    SET_TARGET_PROPERTIES( orocos-rtt-corba PROPERTIES DEFINE_SYMBOL "RTT_DLL_EXPORT")
  ENDIF ( BUILD_STATIC )

  INSTALL_TARGETS(/lib orocos-rtt-corba)

ENDIF(ENABLE_CORBA)