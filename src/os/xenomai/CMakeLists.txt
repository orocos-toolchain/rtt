FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp )

GLOBAL_ADD_INCLUDE( os/xenomai ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})

