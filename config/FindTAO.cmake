#
# This Find function defines:
# TAO_FOUND
# TAO_INCLUDE_DIRS
# TAO_LIBRARIES
# TAO_CLIENT_LIBRARIES
# TAO_DEFINITIONS

MESSAGE ( STATUS "Looking for TAO with orbsvcs...")

SET (TAO_FOUND TRUE)
SET (TAO_LIBRARIES "")
SET (TAO_INCLUDE_DIRS "")
SET (TAO_DEFINITIONS "")

# Verify that we got some ACE up in this joint
FIND_PACKAGE (ACE)
IF (ACE_FOUND)
    LIST (APPEND TAO_FOUND_COMPONENTS "ACE")
    LIST (APPEND TAO_LIBRARIES ${ACE_LIBRARIES})
    LIST (APPEND TAO_INCLUDE_DIRS ${ACE_INCLUDE_DIR})
    SET (TAO_DEFINITIONS ${ACE_DEFINITIONS} "_REENTRANT")
    list (APPEND TAO_CLIENT_LIBRARIES  ${ACE_LIBRARIES})
ELSE ()
    LIST (APPEND TAO_MISSING_COMPONENTS "ACE")
    SET (TAO_FOUND FALSE)
ENDIF ()

# See if TAO_ROOT is not already set in CMake, ignore if hints are set correctly
IF (NOT TAO_ROOT AND NOT TAO_INCLUDE_HINTS AND NOT TAO_LIBRARY_HINTS AND NOT TAO_RUNTIME_HINTS)
    # See if TAO_ROOT is set in process environment
    IF ( NOT $ENV{TAO_ROOT} STREQUAL "" )
        SET (TAO_ROOT "$ENV{TAO_ROOT}")
	MESSAGE(STATUS "Detected TAO_ROOT set to '${TAO_ROOT}'")
    # If ACE_ROOT is set, maybe TAO is there too
    ELSEIF (ACE_ROOT)
        SET (TAO_ROOT "${ACE_ROOT}")
	MESSAGE(STATUS "Set TAO_ROOT to '${TAO_ROOT}'")
    ENDIF ()
ENDIF ()

# If TAO_ROOT is available, set up our hints
IF (TAO_ROOT)
    SET (TAO_INCLUDE_HINTS HINTS "${TAO_ROOT}/include" "${TAO_ROOT}/TAO" "${TAO_ROOT}")
    SET (TAO_LIBRARY_HINTS HINTS "${TAO_ROOT}/lib" "${ACE_ROOT}/lib")
    SET (TAO_RUNTIME_HINTS HINTS "${TAO_ROOT}/bin" "${ACE_ROOT}/bin")
ENDIF ()

# See if headers are present.
find_path(TAO_INCLUDE_DIR NAMES "tao/corba.h" ${TAO_INCLUDE_HINTS})
find_path(TAO_IDL_DIR NAMES "tao/orb.idl" PATHS /usr/share/idl ${TAO_INCLUDE_HINTS})
find_library (TAO_LIBRARY NAMES TAO  ${TAO_LIBRARY_HINTS})
find_library (TAOD_LIBRARY NAMES TAO${CMAKE_DEBUG_POSTFIX} ${TAO_LIBRARY_HINTS})
# Set TAO_LIBRARY ala boost: debug;libdebug;optimized;lib
if (TAO_LIBRARY)
  #message("TAO_LIBRARY found: ${TAO_LIBRARY}")
  SET(TAO_LIBRARY optimized ${TAO_LIBRARY})
else()
  SET(TAO_LIBRARY "")
endif()
if (TAOD_LIBRARY)
  #message("TAOD_LIBRARY found: ${TAOD_LIBRARY}")
  SET(TAO_LIBRARY debug ${TAOD_LIBRARY} ${TAO_LIBRARY})
endif()

# A test for seeing which version of TAO.. :-(
find_path(TAO_15 NAMES "tao/AnyTypeCode/Any.h" ${TAO_INCLUDE_HINTS} )

IF (TAO_INCLUDE_DIR AND TAO_LIBRARY)
    SET (TAO_TAO_FOUND TRUE)
    LIST (APPEND TAO_FOUND_COMPONENTS "TAO")
    LIST (APPEND TAO_LIBRARIES ${TAO_LIBRARY})
    LIST (APPEND TAO_INCLUDE_DIRS ${TAO_INCLUDE_DIR})
    list (APPEND TAO_CLIENT_LIBRARIES  ${TAO_LIBRARY})
ELSE ()
    SET (TAO_FOUND FALSE)
    LIST (APPEND TAO_MISSING_COMPONENTS "TAO")
ENDIF ()
MARK_AS_ADVANCED (TAO_INCLUDE_DIR TAO_LIBRARY)

# try to find orbsvcs. May be in two locations.
IF (NOT ORBSVCS_DIR )
    find_path(TAO_ORBSVCS NAMES "orbsvcs/CosNaming.idl" PATHS ${TAO_IDL_DIR} ${TAO_INCLUDE_HINTS} PATH_SUFFIXES orbsvcs )
    SET( ORBSVCS_DIR ${TAO_ORBSVCS} )
ENDIF (NOT ORBSVCS_DIR )

IF (NOT TAO_INCLUDE_DIR )
    MESSAGE( STATUS "TAO tao/corba.h not found.")
ELSE ()
    MESSAGE( STATUS "TAO tao/corba.h found in ${TAO_INCLUDE_DIR}.")
ENDIF ()

IF (NOT TAO_15 )
    MESSAGE( STATUS "Assuming TAO < 1.5 (based on location of Any.h)")
    list(REMOVE_ITEM TAO_FIND_COMPONENTS AnyTypeCode )
ELSE (NOT TAO_15 )
    MESSAGE( STATUS "Assuming TAO >= 1.5 (based on location of Any.h)")
ENDIF (NOT TAO_15 )

IF (NOT TAO_ORBSVCS )
    MESSAGE( STATUS "TAO orbsvcs/CosNaming.idl not found.")
ELSE (NOT TAO_ORBSVCS )
    MESSAGE( STATUS "TAO orbsvcs/CosNaming.idl found in ${ORBSVCS_DIR}.")
    LIST (APPEND TAO_INCLUDE_DIRS ${ORBSVCS_DIR})
ENDIF (NOT TAO_ORBSVCS )

IF (ACE_FOUND AND TAO_FOUND AND TAO_ORBSVCS )
    MESSAGE ( "Looking for components: ${TAO_FIND_COMPONENTS}")

    # See what components were requested
    FOREACH (COMPONENT ${TAO_FIND_COMPONENTS})
      IF (COMPONENT STREQUAL "IDL")
        # special case for the IDL compiler program
        FIND_PROGRAM (TAO_IDL_EXECUTABLE "tao_idl" ${TAO_RUNTIME_HINTS})
        IF (TAO_IDL_EXECUTABLE)
          SET (TAO_IDL_FOUND TRUE)
          LIST (APPEND TAO_FOUND_COMPONENTS "IDL")
        ELSE ()
          SET (TAO_IDL_FOUND FALSE)
          SET (TAO_FOUND FALSE)
          LIST (APPEND TAO_MISSING_COMPONENTS "IDL")
        ENDIF ()
        MARK_AS_ADVANCED (TAO_IDL_EXECUTABLE)
      ELSE ()
        # Find a TAO shared library
        FIND_LIBRARY (TAO_${COMPONENT}_LIBRARY NAMES "TAO_${COMPONENT}" ${TAO_LIBRARY_HINTS})
        FIND_LIBRARY (TAO_${COMPONENT}D_LIBRARY NAMES "TAO_${COMPONENT}${CMAKE_DEBUG_POSTFIX}" ${TAO_LIBRARY_HINTS})
	# Set TAO_LIBRARY ala boost: debug;libdebug;optimized;lib
	if (TAO_${COMPONENT}_LIBRARY)
	  SET(TAO_${COMPONENT}_LIBRARY optimized ${TAO_${COMPONENT}_LIBRARY})
	else()
	  SET(TAO_${COMPONENT}_LIBRARY "")
	endif()
	if (TAO_${COMPONENT}D_LIBRARY)
	  SET(TAO_${COMPONENT}_LIBRARY debug ${TAO_${COMPONENT}D_LIBRARY} ${TAO_${COMPONENT}_LIBRARY})
	endif()
        IF (TAO_${COMPONENT}_LIBRARY)
          SET (TAO_${COMPONENT}_FOUND TRUE)
          LIST (APPEND TAO_FOUND_COMPONENTS ${COMPONENT})
          LIST (APPEND TAO_LIBRARIES ${TAO_${COMPONENT}_LIBRARY})
	  if( ${COMPONENT} STREQUAL PortableServer )
	    list (APPEND TAO_CLIENT_LIBRARIES  ${TAO_${COMPONENT}_LIBRARY})
	  endif()
        ELSE ()
          SET (TAO_${COMPONENT}_FOUND FALSE)
          SET (TAO_FOUND FALSE)
          LIST (APPEND TAO_MISSING_COMPONENTS ${COMPONENT})
        ENDIF ()
        MARK_AS_ADVANCED (TAO_${COMPONENT}_LIBRARY)
      ENDIF ()
    ENDFOREACH ()

    IF( NOT TAO_IDL_FOUND )
        MESSAGE( STATUS "TAO Headers found but no tao_idl !")
        SET(TAO_FOUND FALSE)
    ELSE( NOT TAO_IDL_FOUND )
        MESSAGE( STATUS "tao_idl: ${TAO_IDL_EXECUTABLE}")

	#Tweaking:
        IF(APPLE)
            # Mac OS X needs this define (or _POSIX_C_SOURCE) to pick up some type
            # definitions that ACE/TAO needs. Personally, I think this is a bug in
            # ACE/TAO, but ....
            LIST(APPEND TAO_DEFINITIONS "_DARWIN_C_SOURCE")
          ENDIF(APPLE)

        IF( NOT TAO_15 )
            LIST(APPEND TAO_LIBRARIES TAO_IDL_BE)
        ENDIF( NOT TAO_15 )

    ENDIF( NOT TAO_IDL_FOUND )
	
else (ACE_FOUND AND TAO_FOUND AND TAO_ORBSVCS )
  MESSAGE( "Not all found: ACE:${ACE_FOUND}; TAO:${TAO_FOUND}; ORBSVCS:${TAO_ORBSVCS}")
ENDIF (ACE_FOUND AND TAO_FOUND AND TAO_ORBSVCS )

MARK_AS_ADVANCED( TAO_15 TAO_ORBSVCS )

# Bail if we were required to find all components and missed at least one
IF (TAO_FIND_REQUIRED AND NOT TAO_FOUND)
    MESSAGE (FATAL_ERROR "Could not find TAO. Missing components: " ${TAO_MISSING_COMPONENTS})
ENDIF ()



# Generate all files required for a corba server app.
# ORO_ADD_CORBA_SERVERS( foo_SRCS foo_HPPS file.idl ... ) 
MACRO(ORO_ADD_CORBA_SERVERS _sources _headers)
   FOREACH (_current_FILE ${ARGN})

      GET_FILENAME_COMPONENT(_tmp_FILE ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_tmp_FILE} NAME_WE)
      GET_FILENAME_COMPONENT(_filedir ${_tmp_FILE} PATH)

      SET(_server  ${CMAKE_CURRENT_BINARY_DIR}/${_basename}S.cpp)
      SET(_serverh ${CMAKE_CURRENT_BINARY_DIR}/${_basename}S.h)

      set(DEFINE_TAO "-DCORBA_IS_TAO")
      # From TAO 1.5 onwards, the _T files are no longer generated
      IF( NOT TAO_15 )
          SET(_tserver )
          SET(_tserverh ${CMAKE_CURRENT_BINARY_DIR}/${_basename}S_T.h ${CMAKE_CURRENT_BINARY_DIR}/${_basename}S_T.inl ${CMAKE_CURRENT_BINARY_DIR}/${_basename}S_T.cpp)
	  set(DEFINE_TAO "")
      ENDIF( NOT TAO_15 )

      SET(_client  ${CMAKE_CURRENT_BINARY_DIR}/${_basename}C.cpp)
      SET(_clienth ${CMAKE_CURRENT_BINARY_DIR}/${_basename}C.h ${CMAKE_CURRENT_BINARY_DIR}/${_basename}C.inl)

      IF (NOT HAVE_${_basename}_SERVER_RULE)
         SET(HAVE_${_basename}_SERVER_RULE ON)
	 # CMake atrocity: if none of these OUTPUT files is used in a target in the current CMakeLists.txt file,
	 # the ADD_CUSTOM_COMMAND is plainly ignored and left out of the make files.
         ADD_CUSTOM_COMMAND(OUTPUT ${_tserver} ${_server} ${_client} ${_tserverh} ${_serverh} ${_clienth}
          COMMAND ${TAO_IDL_EXECUTABLE} -Wb,export_macro=RTT_CORBA_API -Wb,export_include=rtt-corba-config.h ${_current_FILE} -o ${CMAKE_CURRENT_BINARY_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${TAO_IDL_DIR} -I${ORBSVCS_DIR} ${DEFINE_TAO}
          DEPENDS ${_tmp_FILE}
         )
     ENDIF (NOT HAVE_${_basename}_SERVER_RULE)

     SET(${_sources} ${${_sources}} ${_server} ${_tserver} ${_client})
     SET(${_headers} ${${_headers}} ${_serverh} ${_tserverh} ${_clienth})

     SET_SOURCE_FILES_PROPERTIES(${_server} ${_serverh} ${_tserver} ${_client} ${_tserverh} ${_clienth} PROPERTIES GENERATED TRUE)
    ENDFOREACH (_current_FILE)
ENDMACRO(ORO_ADD_CORBA_SERVERS)

