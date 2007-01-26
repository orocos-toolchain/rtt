
#Usage : DOCBOOK_TO_HTML( stylesheet [CATALOG catalog1 ...] FILES source1.xml ... )
macro( DOCBOOK_TO_HTML XSLT_SHEET )
  set(_in_catalogs FALSE)
  set(_in_files FALSE)

  foreach (_current_FILE ${ARGN})

    # process catalogs
    if ( _current_FILE STREQUAL "CATALOG")
      
      set(_in_catalogs TRUE)
      
    else ( _current_FILE STREQUAL "CATALOG")
      
      if ( _in_catalogs )
	# Detect break out of loop...
	if (_current_FILE STREQUAL "FILES")
	  set(_in_catalogs FALSE)
	endif(_current_FILE STREQUAL "FILES")
	set( _catalog_FILES "${_catalog_FILES} ${_current_FILE}")
      endif ( _in_catalogs )

    endif ( _current_FILE STREQUAL "CATALOG")


    if ( _current_FILE STREQUAL "FILES")
      set(_in_files TRUE)
    else ( _current_FILE STREQUAL "FILES")
      if ( _in_files)
	string(REPLACE ".xml" ".html" _current_HTMLFILE ${_current_FILE})
	MESSAGE( "Converting ${_current_FILE} to ${_current_HTMLFILE}" )
	add_custom_command(OUTPUT ${_current_HTMLFILE}
	  COMMAND XML_CATALOG_FILES=${_catalog_FILES} xsltproc --xinclude ${XSLT_SHEET} _current_FILE > ${_current_HTMLFILE}
	  DEPENDS ${_current_FILE} ${XSLT_SHEET} ${_catalog_FILES}
	  )
      endif ( _in_files)
    endif ( _current_FILE STREQUAL "FILES")
  endforeach (_current_FILE ${ARGN})
endmacro( DOCBOOK_TO_HTML XSLT_SHEET )
