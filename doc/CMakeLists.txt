OPTION(BUILD_DOCS "Build the documentation" ON)

include(DocBookToHTML.cmake)
include(DocBookToPDF.cmake)

IF (BUILD_DOCS)
  #list the xml files:
  DOCBOOK_TO_HTML( ${CMAKE_SOURCE_DIR}/../xsl/orocos-html.xsl 
    CATALOG
    catalog.xml
    FILES
    orocos-overview.xml
    orocos-installation.xml
    orocos-overview-devel.xml
    orocos-user-manual.xml 
    orocos-components-manual.xml 
    orocos-devel-manual.xml)

  DOCBOOK_TO_PDF( ${CMAKE_SOURCE_DIR}/../xsl/orocos-pdf.xsl 
    CATALOG
    catalog.xml
    FILES
    orocos-overview.xml
    orocos-installation.xml
    orocos-overview-devel.xml
    orocos-user-manual.xml 
    orocos-components-manual.xml 
    orocos-devel-manual.xml)

ENDIF (BUILD_DOCS)
