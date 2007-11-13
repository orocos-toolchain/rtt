
#Usage : DOCBOOK_TO_PDF( stylesheet [CATALOG catalog1 ...] FILES source1.xml ... )
macro( DOCBOOK_TO_PDF RESULT XSLT_SHEET )
  set(_in_catalogs FALSE)
  set(_in_files FALSE)
  set(_catalog_FILES "")
  foreach (_current_FILE ${ARGN})

    if ( _current_FILE STREQUAL "CATALOG")
      
      set(_in_catalogs TRUE)
      
    else ( _current_FILE STREQUAL "CATALOG")

      if ( _in_catalogs )
	# Detect break out of loop...
	if (_current_FILE STREQUAL "FILES")
	  set(_in_catalogs FALSE)
	else (_current_FILE STREQUAL "FILES")
          set( _catalog_FILES ${_catalog_FILES} ${_current_FILE})
	endif(_current_FILE STREQUAL "FILES")
      endif ( _in_catalogs )

    endif ( _current_FILE STREQUAL "CATALOG")

    if ( _current_FILE STREQUAL "FILES")
      set(_in_files TRUE)
    else ( _current_FILE STREQUAL "FILES")
      if ( _in_files)
      string(REPLACE ".xml" ".pdf" _current_PDFFILE ${_current_FILE})
      string(REPLACE ".xml" ".fo" _current_FOPFILE ${_current_FILE})

      #MESSAGE( "Converting ${_current_FILE} to ${_current_PDFFILE}" )
      add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_current_PDFFILE}
	COMMAND XML_CATALOG_FILES=${_catalog_FILES} xsltproc --xinclude ${CMAKE_CURRENT_SOURCE_DIR}/${XSLT_SHEET} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} > ${_current_FOPFILE}
	COMMAND JAVA_OPTS="-Xmx768m" fop ${_current_FOPFILE} ${_current_PDFFILE}
        COMMAND rm ${_current_FOPFILE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${XSLT_SHEET} ${_catalog_FILES}
	)
      #add_custom_target(docpdf DEPENDS ${_current_PDFFILE} )
      list( APPEND ${RESULT} ${CMAKE_CURRENT_BINARY_DIR}/${_current_PDFFILE} )
      endif ( _in_files)
    endif ( _current_FILE STREQUAL "FILES")
  endforeach (_current_FILE ${ARGN})
endmacro( DOCBOOK_TO_PDF RESULT XSLT_SHEET )
