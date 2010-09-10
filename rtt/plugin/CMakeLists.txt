FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

if ( PLUGINS_ENABLE )
    GLOBAL_ADD_INCLUDE( rtt/plugin ${HPPS})
    GLOBAL_ADD_SRC( ${CPPS})
endif( PLUGINS_ENABLE )
