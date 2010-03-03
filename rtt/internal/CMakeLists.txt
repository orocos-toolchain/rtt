FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.h [^.]*.inl)

## Exceptions:
if ( OS_NO_ASM )
  file( GLOB ASM_FILES ListLockFree.cpp)
  list( REMOVE_ITEM CPPS ${ASM_FILES} )
endif()

GLOBAL_ADD_INCLUDE( rtt/internal ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
