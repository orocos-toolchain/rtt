
  ADD_LIBRARY(testtypes_plugin SHARED plugins_test_types.cpp sequence_types.cpp struct_types.cpp array_types.cpp)

  # Allows us to build the plugin not in the debug/ or release/ subdir
  if (MSVC)
    set(PREFIX_HACK PREFIX "../")
  endif (MSVC)

  SET_TARGET_PROPERTIES( testtypes_plugin PROPERTIES
    SOVERSION "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
    VERSION "${RTT_VERSION}"
    OUTPUT_NAME typekit_plugin-${OROCOS_TARGET}
    COMPILE_DEFINITIONS "${OROCOS-RTT_DEFINITIONS}"
    ${PREFIX_HACK}
    )
  IF (UNIX AND NOT APPLE)
	SET_TARGET_PROPERTIES( testtypes_plugin PROPERTIES
	  LINK_FLAGS "-Wl,-zdefs")
  ENDIF ()
  target_link_libraries(testtypes_plugin orocos-rtt-${OROCOS_TARGET}_dynamic ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
