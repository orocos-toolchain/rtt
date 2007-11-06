OPTION(BUILD_DOCS "Build the documentation" ON)

include(DocBookToHTML.cmake)
include(DocBookToPDF.cmake)
include(AnyToPNG.cmake)

IF (BUILD_DOCS)
  #list the xml files:
  SET( XML_SOURCES
    xml/orocos-overview.xml xml/orocos-installation.xml xml/orocos-overview-devel.xml
    xml/orocos-user-manual.xml xml/orocos-components-manual.xml xml/orocos-devel-manual.xml
    xml/orocos-rtt-changes.xml xml/orocos-user-manual.xml xml/orocos-ocl-intro.xml
    xml/orocos-toolkit-plugin.xml xml/orocos-faq.xml )

  SEPARATE_ARGUMENTS(XML_SOURCES)

  DOCBOOK_TO_HTML( HTML_OUTPUT xsl/orocos-html.xsl 
    CATALOG
    xml/catalog.xml
    FILES 
    ${XML_SOURCES} )

  LIST(REMOVE_ITEM XML_SOURCES xml/orocos-faq.xml)

  DOCBOOK_TO_PDF( PDF_OUTPUT xsl/orocos-pdf.xsl
    CATALOG
    xml/catalog.xml
    FILES
    ${XML_SOURCES} )

  FILE( GLOB PNGS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.png )
  FILE( GLOB DIAS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.dia )
  FILE( GLOB SVGS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.svg )
  FILE( GLOB ICONS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/icons/[^.]*.svg )

  ANY_TO_PNG( RESULT_PNGS ${PNGS} ${DIAS} ${SVGS} ${ICONS})

  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/xml/catalog.xml.in ${CMAKE_CURRENT_BINARY_DIR}/xml/catalog.xml IMMEDIATE @ONLY)
  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/xml/orocos.ent.in ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos.ent IMMEDIATE @ONLY)

  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css
	 COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/orocos-html.css ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css
	 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/orocos-html.css
	 )


  ADD_CUSTOM_TARGET(docpng DEPENDS ${RESULT_PNGS})
  ADD_CUSTOM_TARGET(dochtml DEPENDS ${HTML_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css)
  ADD_CUSTOM_TARGET(docpdf DEPENDS ${PDF_OUTPUT})
  ADD_DEPENDENCIES(dochtml docpng)
  ADD_DEPENDENCIES(docpdf docpng)

  ADD_CUSTOM_TARGET(message ALL echo "Use: 'make docapi dochtml docpdf' to build the documentation.")

  INSTALL_FILES(/doc/liborocos-rtt/api ${CMAKE_CURRENT_BINARY_DIR}/api/*)
  INSTALL_FILES(/doc/liborocos-rtt/xml ${CMAKE_CURRENT_BINARY_DIR}/xml/*)
ENDIF (BUILD_DOCS)
