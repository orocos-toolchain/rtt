FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp )

GLOBAL_ADD_INCLUDE( boost/function_types ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})

ADD_SUBDIRECTORY( detail )