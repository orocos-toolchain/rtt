IF (OROCOS_TARGET STREQUAL "win32")
  FILE( GLOB CPPS [^.]*.cpp [^.]*.c)
  FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

  GLOBAL_ADD_INCLUDE( rtt/os/win32 ${HPPS})
  GLOBAL_ADD_SRC( ${CPPS})
ENDIF (OROCOS_TARGET STREQUAL "win32")