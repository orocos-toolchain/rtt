FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp )

GLOBAL_ADD_INCLUDE( rtt/base ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
