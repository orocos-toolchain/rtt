
# This macro is a dirty hack, Orocos specific. Don't base your macros on it, look into the other
# Find*cmake files for better examples

# This script creates the following variables:
#  Ecos_FOUND: Boolean that indicates if the package was found
#  Ecos_INCLUDE_DIRS: Paths to the necessary header files
#  Ecos_LIBRARIES: Package libraries

  set(ECOS_INSTALL_DIR ${ECOS_INSTALL_DIR} CACHE PATH "path to ecos installation dir" FORCE)

  if (ECOS_INSTALL_DIR STREQUAL "")
    set(ECOS_INSTALL_DIR "/opt/ecos/install")
  endif (ECOS_INSTALL_DIR STREQUAL "")

  if(EXISTS ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)
    message("-- Looking for ECOS - found in ${ECOS_INSTALL_DIR}")
    set(ECOS_FOUND TRUE)
    set(ECOS_INCLUDE_DIRS ${ECOS_INSTALL_DIR}/include )
    set(ECOS_LIBRARIES -L${ECOS_INSTALL_DIR}/lib -ltarget )
  else()
    message(FATAL_ERROR "-- Looking for ECOS - not found (tried: ${ECOS_INSTALL_DIR}/include/pkgconf/system.h)")
    set(ECOS_FOUND FALSE)
  ENDIF()

