<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:src="http://nwalsh.com/xmlns/litprog/fragment"
  exclude-result-prefixes="src" version="1.0">

<!-- Include the default settings -->
  <xsl:import href="/usr/share/sgml/docbook/stylesheet/xsl/nwalsh/fo/docbook.xsl"/>

<!-- 
  These parameters are specific for the Orocos project and override
  the default ones. They must be set after import !
 -->
  <xsl:param name="draft.mode" select="'no'"/>
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="fop.extensions" select="1"/>
  <xsl:param name="chapter.autolabel" select="1"></xsl:param>
  <xsl:param name="appendix.autolabel" select="1"></xsl:param>
  <xsl:param name="section.autolabel" select="1"></xsl:param>
  <xsl:param name="section.autolabel.max.depth" select="3"></xsl:param>

  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="admon.graphics.path" select="'images/'"/>
  <xsl:param name="admon.graphics.extension" select="'.eps'"/>

  <xsl:param name="shade.verbatim" select="1"></xsl:param>

<!-- use this to select the extension for pdf / ps output across all files. -->
  <xsl:param name="graphic.default.extension" select="'eps'"></xsl:param>

<xsl:attribute-set name="monospace.verbatim.properties" 
                   use-attribute-sets="verbatim.properties monospace.properties">
  <xsl:attribute name="wrap-option">wrap</xsl:attribute>
  <xsl:attribute name="hyphenation-character">&#x21BA;</xsl:attribute>
</xsl:attribute-set>
  <xsl:param name="hyphenate.verbatim" select="1"></xsl:param>
  <xsl:param name="monospace.font.family" select="'monospace'"/>


<!--   <xsl:param name="linenumbering.extension" select="0"></xsl:param> -->
<!--   <xsl:param name="use.extensions" select="1"></xsl:param> -->



</xsl:stylesheet>