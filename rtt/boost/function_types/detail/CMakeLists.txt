FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp )
FILE( GLOB INLS [^.]*.inl )

GLOBAL_ADD_INCLUDE( rtt/boost/function_types/detail ${HPPS} ${INLS})
GLOBAL_ADD_SRC( ${CPPS})
