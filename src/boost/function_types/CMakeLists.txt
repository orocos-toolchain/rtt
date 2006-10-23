FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp )

GLOBAL_ADD_INCLUDE( rtt/boost/function_types/detail ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})

