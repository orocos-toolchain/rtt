FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.inl )

GLOBAL_ADD_INCLUDE( rtt/base ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
