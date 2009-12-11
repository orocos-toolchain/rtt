FILE( GLOB CPPS [^.]*.cpp )
FILE( GLOB HPPS [^.]*.hpp [^.]*.inl )

## Exceptions:
if ( OS_NO_ASM )
  file( GLOB ASM_FILES BufferLockFree.cpp)
  list( REMOVE_ITEM CPPS ${ASM_FILES} )
endif()

GLOBAL_ADD_INCLUDE( rtt/base ${HPPS})
GLOBAL_ADD_SRC( ${CPPS})
