# Locate TAO install directory

# This module defines
# BLF_INSTALL where to find include, lib, bin, etc.
# BLF_FOUND, is set to true

INCLUDE (${PROJ_SOURCE_DIR}/config/FindPkgConfig.cmake)

IF ( CMAKE_PKGCONFIG_EXECUTABLE )

  MESSAGE( "Using pkgconfig" )
  PKGCONFIG( "TAO_CosNaming" TAO_FOUND TAO_INCLUDE_DIRS TAO_DEFINES TAO_LINK_DIRS TAO_LIBS )
  
  IF( TAO_FOUND )
    MESSAGE("   Includes in: ${TAO_INCLUDE_DIRS}")
    MESSAGE("   Libraries in: ${TAO_LINK_DIRS}")
    MESSAGE("   Libraries: ${TAO_LIBS}")
    MESSAGE("   Defines: ${TAO_DEFINES}")
    
    INCLUDE_DIRECTORIES( ${TAO_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${TAO_LINK_DIRS} )
    
  ENDIF ( TAO_FOUND )
  
ELSE  ( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually
    MESSAGE( FATAL_ERROR "Can't find TAO")
ENDIF  ( CMAKE_PKGCONFIG_EXECUTABLE )