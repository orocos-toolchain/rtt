###########################################################
# Add everything necessary for a directory to use Corba (ACE/TAO)
#
# Sets the following in the cache
# ORO_ACETAO_DEFINITIONS  - list of additional definitions (prefixed with -D as applicable)
# ORO_ACETAO_INCLUDE_DIRS - list of include directories (not prefixed with -I)
# ORO_ACETAO_LIBRARIES    - list of link libraries (not prefixed with -l)
# ORO_ACETAO_LIBRARY_PATH - list of library directories (not prefixed with -L)
#
# Also this file does do
# ADD_DEFINITIONS(${ORO_ACETAO_DEFINITIONS})
# INCLUDE_DIRECTORIES(${ORO_ACETAO_INCLUDE_DIRS})
# LINK_DIRECTORIES(${ORO_ACETAO_LIBRARY_PATH})
#
# but depending on their needs, caller must do either
# LINK_LIBRARIES(${ORO_ACETAO_LIBRARIES})
# or
# TARGET_LINK_LIBRARIES(prog-or-lib ... ${ORO_ACETAO_LIBRARIES})
###########################################################

#
# Deal with ACE/TAO/Orbsrvcs having different include directories
# Remove duplicates if possible.
#
SET(ORO_ACETAO_INCLUDE_DIRS ${ACE_DIR} ${TAO_DIR} ${ORBSVCS_DIR} CACHE INTERNAL "")
IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
  LIST(REMOVE_DUPLICATES ORO_ACETAO_INCLUDE_DIRS)
ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)


# this is ../lib as this file assumes that the ACE_DIR/TAO_DIR is of the
# form /path/to/ace/include ... ie the dir containing the include files.
# See top of config/FindCorbaDeps.cmake: the check against /usr/include
SET(ORO_ACETAO_LIBRARY_PATH ${TAO_DIR}/../lib CACHE INTERNAL "")

SET(ORO_ACETAO_LIBRARIES TAO TAO_PortableServer TAO_CosNaming ACE CACHE INTERNAL "")
IF( NOT TAO_15 )
  SET(ORO_ACETAO_LIBRARIES ${ORO_ACETAO_LIBRARIES} TAO_IDL_BE CACHE INTERNAL "")
ENDIF( NOT TAO_15 )

IF(APPLE)
  # Mac OS X needs this define (or _POSIX_C_SOURCE) to pick up some type
  # definitions that ACE/TAO needs. Personally, I think this is a bug in
  # ACE/TAO, but ....
  SET(ORO_ACETAO_DEFINITIONS "-D_DARWIN_C_SOURCE" CACHE INTERNAL "")
  ADD_DEFINITIONS(${ORO_ACETAO_DEFINITIONS})
  # and needs additional libraries 
  SET(ORO_ACETAO_LIBRARIES ${ORO_ACETAO_LIBRARIES} TAO_AnyTypeCode CACHE INTERNAL "")
ENDIF(APPLE)

#MESSAGE("ORO_ACETAO_LIBRARIES = ${ORO_ACETAO_LIBRARIES}")

INCLUDE_DIRECTORIES(${ORO_ACETAO_INCLUDE_DIRS})
LINK_DIRECTORIES(${ORO_ACETAO_LIBRARY_PATH})
