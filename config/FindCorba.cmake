#
# Detect CORBA using user's CORBA_IMPLEMENTATION
#
if (ENABLE_CORBA)
  SET(CORBA_FOUND)
    IF(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        # Look for TAO and ACE
	if(${OROCOS_TARGET} MATCHES "win32")
	  set(XTRA_TAO_LIBS AnyTypeCode ValueType) #note: capital T
	endif()
	if(${OROCOS_TARGET} MATCHES "macosx|gnulinux")
	  set(XTRA_TAO_LIBS AnyTypeCode Valuetype) #note: small T
	endif()
        find_package(TAO REQUIRED IDL PortableServer CosNaming Messaging ${XTRA_TAO_LIBS})
	    SET(CORBA_FOUND ${TAO_FOUND})
        IF(NOT TAO_FOUND)
            MESSAGE(FATAL_ERROR "Cannot find TAO")
        ELSE(NOT TAO_FOUND)
            MESSAGE(STATUS "CORBA enabled: ${TAO_FOUND_COMPONENTS}")

	    # Copy flags:
        SET(CORBA_INCLUDE_DIRS ${TAO_INCLUDE_DIRS})
	    SET(CORBA_CFLAGS ${TAO_CPP_FLAGS})
        SET(CORBA_LIBRARIES ${TAO_LIBRARIES})
	    SET(CORBA_DEFINITIONS ${TAO_DEFINITIONS})
	    # Flag used in rtt-corba-config.h
	    SET(CORBA_IS_TAO 1)

	    if( TAO_Messaging_FOUND )
	      SET(CORBA_TAO_HAS_MESSAGING 1)
	    endif()

 	    # Including a TAO header is sufficient to depend on this library.
	    set(CORBA_USER_LINK_LIBS TAO_PortableServer TAO)

       ENDIF(NOT TAO_FOUND)
    ELSEIF(${CORBA_IMPLEMENTATION} STREQUAL "OMNIORB")
        find_package(OmniORB REQUIRED)
	    SET(CORBA_FOUND ${OMNIORB4_FOUND})
        IF(NOT OMNIORB4_FOUND)
            MESSAGE(FATAL_ERROR "cannot find OmniORB4")
        ELSE(NOT OMNIORB4_FOUND)
            MESSAGE(STATUS "CORBA enabled: OMNIORB")

	    # Copy flags:
	    SET(CORBA_INCLUDE_DIRS ${OMNIORB4_INCLUDE_DIR})
	    SET(CORBA_CFLAGS ${OMNIORB4_CPP_FLAGS})
	    SET(CORBA_LIBRARIES ${OMNIORB4_LIBRARIES})
	    SET(CORBA_DEFINITIONS ${OMNIORB4_DEFINITIONS})
	    # Flag used in rtt-corba-config.h
	    SET(CORBA_IS_OMNIORB 1)

        # Including an Omniorb header is sufficient to depend on this library.
        set(CORBA_USER_LINK_LIBS omniORB4 omnithread )

        ENDIF(NOT OMNIORB4_FOUND)
    ELSE(${CORBA_IMPLEMENTATION} STREQUAL "TAO")
        MESSAGE(FATAL_ERROR "Unknown CORBA implementation '${CORBA_IMPLEMENTATION}': must be TAO or OMNIORB.")
    ENDIF(${CORBA_IMPLEMENTATION} STREQUAL "TAO")

	# Bail if we were required to find all components and missed at least one
	IF (CORBA_FIND_REQUIRED AND NOT CORBA_FOUND)
      MESSAGE (FATAL_ERROR "Could not find CORBA")
	ENDIF ()

endif (ENABLE_CORBA)
