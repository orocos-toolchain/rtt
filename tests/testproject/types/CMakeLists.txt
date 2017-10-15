
  ADD_LIBRARY(typekit_plugin SHARED plugins_test_types.cpp)

  # Allows us to build the plugin not in the debug/ or release/ subdir
  if (MSVC)
    set(PREFIX_HACK PREFIX "../")
  endif (MSVC)

  SET_TARGET_PROPERTIES( typekit_plugin PROPERTIES
    VERSION "${RTT_VERSION}"
    SOVERSION "${RTT_SOVERSION}"
    OUTPUT_NAME typekit_plugin-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    ${PREFIX_HACK}
    )
  IF (UNIX AND NOT APPLE)
	SET_TARGET_PROPERTIES( typekit_plugin PROPERTIES
	  LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()
  target_link_libraries(typekit_plugin orocos-rtt-${OROCOS_TARGET}_dynamic)

