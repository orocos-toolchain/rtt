/** @section gendocs Generating Documentation

The doc/ directory  is structured as follows:

* 'make docpdf dochtml':
  The docbook-xml manuals (Component Builder's Guide,
  Getting Started,...) are in the doc/xml directory.
  They are used to generated pdf and html documents.
  Each XML file is one chapter.

  The images used in these manuals are stored in doc/xml/images

* 'make docapi':
  The API documentation is generated in doc/api, 
  using the current configuration.

* 'xsl/':
  Contains the 'translation' sheets to translate an
  XML file to a HTML file or an XML file to an
  intermediate 'fo' format.

* 'sgml/':
  Contains some older docbook documents.

* 'cpf.dtd':
  A 'Document Type Definition' which tells an XML 
  formatter or parser how the Orocos Property XML
  files must look.

*/
