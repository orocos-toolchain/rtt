
macro( SVG_TO_PDF RESULT)
  #SEPARATE_ARGUMENTS( ARGN )
  foreach (_current_FILE ${ARGN})
    # Create the target directory.
    #MESSAGE( "Processing ${CMAKE_CURRENT_BINARY_DIR}/${_current_FILE}" )
    get_filename_component(DIRNAME ${CMAKE_CURRENT_BINARY_DIR}/${_current_FILE} PATH)
    get_filename_component(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${_current_FILE} NAME)
    get_filename_component(FILE_WE ${CMAKE_CURRENT_BINARY_DIR}/${_current_FILE} NAME_WE)
    file(MAKE_DIRECTORY ${DIRNAME})
    file(MAKE_DIRECTORY ${DIRNAME}/hires )

    if ( _current_FILE MATCHES ".*.svg" )
      string(REPLACE ".svg" ".pdf" _current_PDFFILE ${_current_FILE})

      #MESSAGE( "Converting ${_current_FILE} to ${_current_PDFFILE}" )
      add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_current_PDFFILE}
	COMMAND inkscape -z -A ${_current_PDFFILE} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE}
	)
      list( APPEND ${RESULT} ${CMAKE_CURRENT_BINARY_DIR}/${_current_PDFFILE} )
    endif ( _current_FILE MATCHES ".*.svg" )
  endforeach (_current_FILE ${ARGN})
  #MESSAGE( "Processing RESULT is: ${${RESULT}}" )
    
endmacro( SVG_TO_PDF RESULT)

