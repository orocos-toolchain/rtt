FILE( GLOB CPPS [^.]*.cpp [^.]*.c)
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

GLOBAL_ADD_INCLUDE( rtt/os ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})

# Supported Operating Systems
ADD_SUBDIRECTORY( gnulinux )
ADD_SUBDIRECTORY( lxrt )
ADD_SUBDIRECTORY( xenomai )
ADD_SUBDIRECTORY( ecos )
ADD_SUBDIRECTORY( macosx )
ADD_SUBDIRECTORY( win32 )

# Supported Hardware
ADD_SUBDIRECTORY( oro_i386 )
ADD_SUBDIRECTORY( oro_x86_64 )
ADD_SUBDIRECTORY( oro_powerpc )
ADD_SUBDIRECTORY( oro_msvc )

# Target specific config files
ADD_SUBDIRECTORY( targets )
