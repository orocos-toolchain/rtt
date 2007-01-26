# Locate TAO install directory

# This module defines
# BLF_INSTALL where to find include, lib, bin, etc.
# BLF_FOUND, is set to true

INCLUDE (${PROJ_SOURCE_DIR}/config/FindPkgConfig.cmake)

    MESSAGE ( STATUS "Looking for TAO with orbsvcs...")

  # DOES NOT WORK: TAO_ID.pc not found on debian system... detect .so files instead !
  #PKGCONFIG( "TAO_PortableServer" TAO_FOUND TAO_INCLUDE_DIRS TAO_DEFINES TAO_LINK_DIRS TAO_LIBS )
  #PKGCONFIG( "TAO_CosNaming" TAONAMING_FOUND TAONAMING_INCLUDE_DIRS TAONAMING_DEFINES TAONAMING_LINK_DIRS TAONAMING_LIBS )

  SET(ACE_DIR $ENV{ACE_ROOT})
  SET(TAO_DIR $ENV{TAO_ROOT})
  MESSAGE( "ACE_DIR is ${ACE_DIR}" )

  IF (NOT ACE_DIR )
    SET(ACE_DIR /usr/include)
  ENDIF (NOT ACE_DIR )
  IF (NOT TAO_DIR )
    SET(TAO_DIR /usr/include)
  ENDIF (NOT TAO_DIR )

  # See if headers are present.
  FIND_FILE(ACE_CONFIG config-all.h ${ACE_DIR}/ace )
  FIND_FILE(TAO_ORB ORB.h ${TAO_DIR}/tao )
  # try to find orbsvcs (FIX: include CosNaming.idl ourselves ??)
  IF (NOT ORBSVCS_DIR )
    FIND_FILE(TAO_ORBSVCS CosNaming.idl ${TAO_DIR}/orbsvcs/orbsvcs)
    IF (TAO_ORBSVCS)
      SET( ORBSVCS_DIR ${TAO_DIR}/orbsvcs )
    ELSE (TAO_ORBSVCS)
      FIND_FILE(TAO_ORBSVCS CosNaming.idl ${TAO_DIR}/orbsvcs )
      SET( ORBSVCS_DIR ${TAO_DIR} )
    ENDIF (TAO_ORBSVCS)
  ENDIF (NOT ORBSVCS_DIR )

  IF (NOT ACE_CONFIG )
    MESSAGE( "ACE config-all.h not found in ${ACE_DIR}/ace.")
  ELSE(NOT ACE_CONFIG ) 
   MESSAGE( "ACE config-all.h found in ${ACE_DIR}/ace.")
  ENDIF (NOT ACE_CONFIG )
  IF (NOT TAO_ORB )
    MESSAGE( "TAO ORB.h not found in ${TAO_DIR}/tao.")
  ELSE (NOT TAO_ORB )
    MESSAGE( "TAO ORB.h found in ${TAO_DIR}/tao.")
  ENDIF (NOT TAO_ORB )
  IF (NOT TAO_ORBSVCS )
    MESSAGE( "TAO orbsvcs/CosNaming.idl not found in ${ORBSVCS_DIR}.")
  ELSE (NOT TAO_ORBSVCS )
    MESSAGE( "TAO orbsvcs/CosNaming.idl found in ${ORBSVCS_DIR}.")
  ENDIF (NOT TAO_ORBSVCS )

  IF (ACE_CONFIG AND TAO_ORB AND TAO_ORBSVCS )
    MESSAGE ( "TAO with orbsvcs found.")

  # Add includes / lib paths if necessary
  IF( NOT ${ACE_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ACE_DIR} )
    LINK_DIRECTORIES( ${ACE_DIR}/lib )
  ENDIF( NOT ${ACE_DIR} STREQUAL /usr/include )
  IF( NOT ${TAO_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${TAO_DIR} )
  ENDIF( NOT ${TAO_DIR} STREQUAL /usr/include )
  IF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )
    INCLUDE_DIRECTORIES( ${ORBSVCS_DIR} )
  ENDIF( NOT ${ORBSVCS_DIR} STREQUAL /usr/include )

  # Finally:
  LINK_LIBRARIES( TAO TAO_IDL_BE TAO_PortableServer TAO_CosNaming TAO_CosProperty TAO_CosEvent ACE  )

  FIND_PROGRAM( ORO_TAOIDL_EXECUTABLE tao_idl PATHS ${ACE_ROOT}/bin NO_DEFAULT_PATHS )
  FIND_PROGRAM( ORO_TAOIDL_EXECUTABLE tao_idl )

    IF( NOT ORO_TAOIDL_EXECUTABLE )
      MESSAGE( FATAL "TAO Headers found but no tao_idl !")
    ELSE( NOT ORO_TAOIDL_EXECUTABLE )
      MESSAGE( "tao_idl: ${ORO_TAOIDL_EXECUTABLE}")
    ENDIF( NOT ORO_TAOIDL_EXECUTABLE )
  
ENDIF (ACE_CONFIG AND TAO_ORB AND TAO_ORBSVCS )


