FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt/plugin ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
