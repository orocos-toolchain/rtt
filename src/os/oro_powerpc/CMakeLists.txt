FILE( GLOB CPPS [^.]*.cpp [^.]*.c)
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt/os/oro_powerpc ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
