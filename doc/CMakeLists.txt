OPTION(BUILD_DOCS "Build the documentation" ON)

include(DocBookToHTML.cmake)
include(DocBookToPDF.cmake)
include(AnyToPNG.cmake)

IF (BUILD_DOCS)
  #list the xml files:
  SET( XML_SOURCES xml/orocos-overview.xml xml/orocos-installation.xml xml/orocos-overview-devel.xml xml/orocos-user-manual.xml xml/orocos-components-manual.xml xml/orocos-devel-manual.xml )

    string(REPLACE ".xml" ".html " HTML_OUTPUT ${XML_SOURCES})
    string(REPLACE ".xml" ".pdf "  PDF_OUTPUT ${XML_SOURCES})

    SEPARATE_ARGUMENTS(XML_SOURCES)
    SEPARATE_ARGUMENTS(HTML_OUTPUT)
    SEPARATE_ARGUMENTS(PDF_OUTPUT)

#   FOREACH( XFILE ${XML_SOURCES} )
#     string(REPLACE ".xml" ".html" HFILE ${XFILE})
#     string(REPLACE ".xml" ".pdf"  PFILE ${XFILE})
#     SET( HTML_OUTPUT "${HTML_OUTPUT} ${HFILE}")
#     SET( PDF_OUTPUT "${PDF_OUTPUT} ${PFILE}")
#   ENDFOREACH( XFILE ${XML_SOURCES} )


  DOCBOOK_TO_HTML( xsl/orocos-html.xsl 
    CATALOG
    xml/catalog.xml
    FILES 
    ${XML_SOURCES} )

  DOCBOOK_TO_PDF( xsl/orocos-pdf.xsl
    CATALOG
    xml/catalog.xml
    FILES
    ${XML_SOURCES} )

  FILE( GLOB PNGS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.png )
  FILE( GLOB DIAS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.dia )
  FILE( GLOB SVGS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.svg )
  FILE( GLOB ICONS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/icons/[^.]*.png )

  ANY_TO_PNG( RESULT_PNGS ${PNGS} ${DIAS} ${SVGS} ${ICONS})

  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/xml/catalog.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/xml/catalog.xml IMMEDIATE @ONLY)

  ADD_CUSTOM_TARGET(docpng DEPENDS ${RESULT_PNGS})
  ADD_CUSTOM_TARGET(dochtml DEPENDS ${HTML_OUTPUT} ${RESULT_PNGS})
  ADD_CUSTOM_TARGET(docpdf DEPENDS ${PDF_OUTPUT} ${RESULT_PNGS})

  ADD_CUSTOM_TARGET(message ALL echo "Use: make dochtml docpdf to build the documentation.")


ENDIF (BUILD_DOCS)
