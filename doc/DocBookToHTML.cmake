
#Usage : DOCBOOK_TO_HTML( stylesheet [CATALOG catalog1 ...] FILES source1.xml ... )
macro( DOCBOOK_TO_HTML XSLT_SHEET )
  set(_in_catalogs FALSE)
  set(_in_files FALSE)
  set(_catalog_FILES "")

  foreach (_current_FILE ${ARGN})

    # process catalogs
    if ( _current_FILE STREQUAL "CATALOG")
      
      set(_in_catalogs TRUE)
      
    else ( _current_FILE STREQUAL "CATALOG")
      
      if ( _in_catalogs )
	# Detect break out of loop...
	if (_current_FILE STREQUAL "FILES")
	  set(_in_catalogs FALSE)
	else(_current_FILE STREQUAL "FILES")
	  set( _catalog_FILES ${_catalog_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE})
	endif(_current_FILE STREQUAL "FILES")
      endif ( _in_catalogs )

    endif ( _current_FILE STREQUAL "CATALOG")


    if ( _current_FILE STREQUAL "FILES")
      set(_in_files TRUE)
    else ( _current_FILE STREQUAL "FILES")
      if ( _in_files)
	string(REPLACE ".xml" ".html" _current_HTMLFILE ${_current_FILE})
	MESSAGE( "Converting ${_current_FILE} to ${_current_HTMLFILE}" )
	add_custom_command(OUTPUT ${_current_HTMLFILE}
	  COMMAND XML_CATALOG_FILES=${_catalog_FILES} xsltproc --xinclude ${CMAKE_CURRENT_SOURCE_DIR}/${XSLT_SHEET} ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} > ${_current_HTMLFILE}
	  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${XSLT_SHEET} ${_catalog_FILES}
	  )
        #add_custom_target(dochtml DEPENDS ${_current_HTMLFILE} )
      endif ( _in_files)
    endif ( _current_FILE STREQUAL "FILES")
  endforeach (_current_FILE ${ARGN})
endmacro( DOCBOOK_TO_HTML XSLT_SHEET )
