
#Usage : DOCBOOK_TO_PDF( stylesheet [CATALOG catalog1 ...] FILES source1.xml ... )
macro( DOCBOOK_TO_PDF XSLT_SHEET )
  set(_in_catalogs FALSE)
  set(_in_files FALSE)
  foreach (_current_FILE ${ARGN})

    if ( _current_FILE STREQUAL "CATALOG" OR _in_catalogs)
      set(_in_catalogs TRUE)
      # Detect break out of loop...
      if (_current_FILE STREQUAL "FILES")
	set(_in_catalogs FALSE)
      endif(_current_FILE STREQUAL "FILES")
      set( _catalog_FILES "${_catalog_FILES} ${_current_FILE}")
    endif ( _current_FILE STREQUAL "CATALOG" OR _in_catalogs)

    if ( _current_FILE STREQUAL "FILES" OR _in_files)
      set(_in_files TRUE)
      string(REPLACE ".xml" ".pdf" _current_PDFFILE ${_current_FILE})
      string(REPLACE ".xml" ".fo" _current_FOPFILE ${_current_FILE})

      add_custom_command(OUTPUT ${_current_PDFFILE}
	COMMAND XML_CATALOG_FILES=${_catalog_FILES} xsltproc --xinclude ${XSLT_SHEET} ${_current_FILE} > ${_current_FOPFILE}
	COMMAND fop ${_current_FOPFILE} ${_current_PDFFILE}
	DEPENDS ${_current_FILE} ${XSLT_SHEET} ${_catalog_FILES}
	)
    endif ( _current_FILE STREQUAL "FILES" OR _in_files)
  endforeach (_current_FILE ${ARGN})
endmacro( DOCBOOK_TO_PDF XSLT_SHEET )
