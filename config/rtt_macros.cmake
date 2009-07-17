#
# Include and link against required stuff
#

#
# RTT supplies header files which should be included when 
# using RTT. Each subdir should use this macro
# to supply its header-files.
#
# Usage: GLOBAL_ADD_INCLUDE( RELATIVE_LOCATION hpp1, hpp2 ...)
MACRO( GLOBAL_ADD_INCLUDE COMPONENT_LOCATION )
  INSTALL(FILES ${ARGN} DESTINATION include/${COMPONENT_LOCATION} COMPONENT headers)
ENDMACRO( GLOBAL_ADD_INCLUDE COMPONENT_LOCATION )

#
# Sources should add themselves by calling 'GLOBAL_ADD_SRC' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
#
# This gives a centralised location where all sources are registered
# and lets us make the library in just one place.
#
#
# Usage: GLOBAL_ADD_SRC( src1 src2 src3 )
#
MACRO( GLOBAL_ADD_SRC )
  STRING(LENGTH "${ARGN}" NOTEMPTY)
  IF(NOTEMPTY)
    SET (ENV{GLOBAL_LIBRARY_SRCS} "$ENV{GLOBAL_LIBRARY_SRCS}${ARGN};" )
  ENDIF(NOTEMPTY) 
ENDMACRO( GLOBAL_ADD_SRC )



MACRO( GLOBAL_ADD_TESTS TEST_NAME )
  IF(BUILD_TESTS)
    STRING(LENGTH "${ARGN}" NOTEMPTY)
    IF(NOTEMPTY)
	ADD_EXECUTABLE( ${TEST_NAME} ${ARGN} )
    ENDIF(NOTEMPTY) 
  ENDIF(BUILD_TESTS)
ENDMACRO( GLOBAL_ADD_TESTS TEST_NAME)

  
