FILE( GLOB CPPS [^.]*.cpp [^.]*.c)
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt/os ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})

ADD_SUBDIRECTORY( gnulinux )
ADD_SUBDIRECTORY( lxrt )
ADD_SUBDIRECTORY( xenomai )
ADD_SUBDIRECTORY( oro_i386 )
ADD_SUBDIRECTORY( oro_x86_64 )
