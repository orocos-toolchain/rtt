IF (OROCOS_TARGET STREQUAL "xenomai")
  FILE( GLOB CPPS [^.]*.cpp [^.]*.c)
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/os/xenomai ${HPPS})
  GLOBAL_ADD_SRC( ${CPPS})
ENDIF (OROCOS_TARGET STREQUAL "xenomai")