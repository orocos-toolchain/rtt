
  ADD_LIBRARY(service_plugin SHARED plugins_test_services.cpp)

  # Allows us to build the plugin not in the debug/ or release/ subdir
  if (MSVC)
    set(PREFIX_HACK PREFIX "../")
  endif (MSVC)

  SET_TARGET_PROPERTIES( service_plugin PROPERTIES
    VERSION "${RTT_VERSION}"
    SOVERSION "${RTT_SOVERSION}"
    OUTPUT_NAME service_plugin-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    ${PREFIX_HACK}
    )
  IF (UNIX AND NOT APPLE)
	SET_TARGET_PROPERTIES( service_plugin PROPERTIES
	  LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()
  target_link_libraries(service_plugin orocos-rtt-${OROCOS_TARGET}_dynamic)


  ADD_LIBRARY(global_service_plugin SHARED plugins_test_global_services.cpp)

  SET_TARGET_PROPERTIES( global_service_plugin PROPERTIES
    VERSION "${RTT_VERSION}"
    SOVERSION "${RTT_SOVERSION}"
    OUTPUT_NAME global_service_plugin-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    ${PREFIX_HACK}
    )
  IF (UNIX AND NOT APPLE)
    SET_TARGET_PROPERTIES( global_service_plugin PROPERTIES
      LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()
  target_link_libraries(global_service_plugin orocos-rtt-${OROCOS_TARGET}_dynamic)
