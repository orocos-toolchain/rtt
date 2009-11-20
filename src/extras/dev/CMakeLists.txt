FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt/extras/dev ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
