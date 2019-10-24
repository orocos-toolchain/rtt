### Trace with lttng-ust

IF (NOT OROBLD_DISABLE_LTTNG_UST)

  MESSAGE("Building tracing lib.")

  INCLUDE_DIRECTORIES(   ${Lttng_ust_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
  ADD_LIBRARY(           orocos-rtt-traces-${OROCOS_TARGET} SHARED "lttng_ust.c")
  SET_TARGET_PROPERTIES( orocos-rtt-traces-${OROCOS_TARGET} PROPERTIES
                         SOVERSION     "${RTT_VERSION_MAJOR}.${RTT_VERSION_MINOR}"
                         VERSION       "${RTT_VERSION}")
  TARGET_LINK_LIBRARIES( orocos-rtt-traces-${OROCOS_TARGET} ${Lttng_ust_LIBRARIES})

  INSTALL(TARGETS        orocos-rtt-traces-${OROCOS_TARGET}
                         LIBRARY DESTINATION lib)

ENDIF(NOT OROBLD_DISABLE_LTTNG_UST)

