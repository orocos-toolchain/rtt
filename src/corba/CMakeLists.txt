INCLUDE (${PROJ_SOURCE_DIR}/config/TaoIDL.cmake)
INCLUDE (${PROJ_SOURCE_DIR}/config/DependentOption.cmake)

#this option was set in global_rules.cmake
IF(ENABLE_CORBA)
  MESSAGE( "CORBA Support enabled.")
  # Finally:
  
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

  SET(RTT_LINKFLAGS "${RTT_LINKFLAGS} -lTAO -lTAO_IDL_BE -lTAO_PortableServer -lTAO_CosNaming -lACE" CACHE INTERNAL "")

  FILE( GLOB IDLS [^.]*.idl )
  FILE( GLOB CPPS [^.]*.cpp )
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  # exclude some files:
  LIST( REMOVE_ITEM HPPS CorbaDataObject.hpp CorbaBuffer.hpp)

  ORO_ADD_CORBA_SERVERS(CPPS HPPS ${IDLS} )
  SET( ENV{GLOBAL_GENERATED_SRCS} "${CPPS}" )

  #This has been added in order to generate files before make -j<N>
  # make -j<N> causes corruption of generated files because of overwriting
  ADD_CUSTOM_TARGET( idl DEPENDS ${CPPS} ${HPPS} COMMENT "Generating sources from .idl files")

  #MESSAGE("CPPS: $ENV{GLOBAL_GENERATED_SRCS}")
  #MESSAGE("HPPS: ${HPPS}")
  GLOBAL_ADD_INCLUDE( rtt/corba ${HPPS})
  #GLOBAL_ADD_SRC( ${CPPS} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

IF ( BUILD_STATIC )
  ADD_LIBRARY(orocos-rtt-corba_static-${OROCOS_TARGET} STATIC ${CPPS})
  SET_TARGET_PROPERTIES( orocos-rtt-corba_static-${OROCOS_TARGET} 
  PROPERTIES DEFINE_SYMBOL "RTT_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-corba-${OROCOS_TARGET}
  CLEAN_DIRECT_OUTPUT 1
  VERSION "${RTT_VERSION}"
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD} ${RTT_CFLAGS} -D_REENTRANT -DOROCOS_TARGET=${OROCOS_TARGET}" )
ENDIF( BUILD_STATIC )

  ADD_LIBRARY(orocos-rtt-corba_dynamic-${OROCOS_TARGET} SHARED ${CPPS})
  SET_TARGET_PROPERTIES( orocos-rtt-corba_dynamic-${OROCOS_TARGET} PROPERTIES
  DEFINE_SYMBOL "RTT_DLL_EXPORT"
  OUTPUT_NAME orocos-rtt-corba-${OROCOS_TARGET}
  CLEAN_DIRECT_OUTPUT 1
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS_ADD} ${RTT_CFLAGS} -D_REENTRANT -DOROCOS_TARGET=${OROCOS_TARGET}"
  SOVERSION "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
  VERSION "${RTT_VERSION}")


CONFIGURE_FILE( ${PROJ_SOURCE_DIR}/src/corba/orocos-rtt-corba.pc.in ${PROJ_BINARY_DIR}/src/corba/orocos-rtt-corba-${OROCOS_TARGET}.pc @ONLY)

IF ( BUILD_STATIC )
  INSTALL_TARGETS(/lib orocos-rtt-corba_static-${OROCOS_TARGET})
ENDIF( BUILD_STATIC )

  INSTALL_TARGETS(/lib orocos-rtt-corba_dynamic-${OROCOS_TARGET})
  INSTALL_FILES( /lib/pkgconfig FILES orocos-rtt-corba-${OROCOS_TARGET}.pc)

ENDIF(ENABLE_CORBA)
