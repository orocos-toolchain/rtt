
macro( ANY_TO_PNG RESULT)
  #SEPARATE_ARGUMENTS( ARGN )
  foreach (_current_FILE ${ARGN})
    if ( _current_FILE MATCHES ".*.dia" )
      string(REPLACE ".dia" ".png" _current_PNGFILE ${_current_FILE})

      #MESSAGE( "Converting ${_current_FILE} to ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE}" )
      add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE}
	COMMAND LC_NUMERIC="C" dia -t png --nosplash -e ${_current_PNGFILE} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	)
      list( APPEND ${RESULT} ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE} )
    endif ( _current_FILE MATCHES ".*.dia" )

    if ( _current_FILE MATCHES ".*.svg" )
      string(REPLACE ".svg" ".png" _current_PNGFILE ${_current_FILE})

      #MESSAGE( "Converting ${_current_FILE} to ${_current_PNGFILE}" )
      add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE}
	COMMAND inkscape -z -D -d 60 -e ${_current_PNGFILE} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	)
      list( APPEND ${RESULT} ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE} )
    endif ( _current_FILE MATCHES ".*.svg" )

    if ( _current_FILE MATCHES ".*.png" )
      if ( NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
       set(_current_PNGFILE ${_current_FILE})
       #MESSAGE( "Copying ${_current_FILE} to ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE}" )
       add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE}
	 COMMAND mkdir -p $$\(dirname ${_current_PNGFILE} \)
	 COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} ${_current_PNGFILE}
	 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	 )
       list( APPEND ${RESULT} ${CMAKE_CURRENT_BINARY_DIR}/${_current_PNGFILE} )
      endif ( NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    endif ( _current_FILE MATCHES ".*.png" )
  endforeach (_current_FILE ${ARGN})
  #MESSAGE( "Processing RESULT is: ${${RESULT}}" )
    
endmacro( ANY_TO_PNG RESULT)

