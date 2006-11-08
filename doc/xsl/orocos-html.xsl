<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.w3.org/TR/xhtml1/transitional"
                exclude-result-prefixes="#default">

<!-- Change this to the path to where you have installed Norman
     Walsh's XSL stylesheets.  -->
<xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/xhtml/docbook.xsl"/>

<!-- 
  These parameters are specific for the Orocos project and override
  the default ones. They must be set after import !
 -->
  <xsl:param name="draft.mode" select="'no'"/>
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="chapter.autolabel" select="1"></xsl:param>
  <xsl:param name="appendix.autolabel" select="1"></xsl:param>
  <xsl:param name="section.autolabel" select="1"></xsl:param>
  <xsl:param name="section.autolabel.max.depth" select="3"></xsl:param>

<!--   For HTML output, use the standard image size. -->
<!--   <xsl:param name="ignore.image.scaling" select="1"/> -->

  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="admon.graphics.path" select="'images/icons/'"/>
  <xsl:param name="admon.graphics.extension" select="'.png'"/>

  <xsl:param name="html.stylesheet" select="'orocos-html.css'"></xsl:param>

<!-- use this to select the extension for html output across all files. -->
  <xsl:param name="graphic.default.extension" select="'png'"></xsl:param>

  <xsl:param name="hyphenate.verbatim" select="1"></xsl:param>
  <xsl:param name="monospace.font.family" select="'monospace'"></xsl:param>

<xsl:attribute-set name="monospace.verbatim.properties"
                   use-attribute-sets="verbatim.properties monospace.properties">
  <xsl:attribute name="wrap-option">wrap</xsl:attribute>
  <xsl:attribute name="hyphenation-character">&#x21BA;</xsl:attribute>
</xsl:attribute-set>

<!-- convert classname to to doxygen filenames
     xsltproc does not support fn:replace, so needed the nwalsh 'string.subst' template
     to do it, otherwise, the shorter commented line could have been used :-( -->
<!-- This template only applies if the format is of 
      
         <classname>Namespace::Classname</classname>
     Otherwise, the default docbook style is applied.
-->
  <xsl:variable name="orocos.doxygen.path" select="'../api/html'" />
  <xsl:variable name="orocos.doxygen.ext" select="'html'" />
  <xsl:template match="classname">
    <xsl:variable name="orocos.doxygen.classname">
      <xsl:call-template name="string.subst">
          <xsl:with-param name="string" select="substring-after( ., '::')"></xsl:with-param>
          <xsl:with-param name="target" select="'_'"></xsl:with-param>
          <xsl:with-param name="replacement" select="'__'"></xsl:with-param>
      </xsl:call-template>
<!--      <xsl:value-of select="substring-after( replace(.,'_','__'), '::')" /> -->
    </xsl:variable>
    <xsl:variable name="orocos.doxygen.namespace">
      <xsl:call-template name="string.subst">
          <xsl:with-param name="string" select="substring-before( ., '::')"></xsl:with-param>
          <xsl:with-param name="target" select="'_'"></xsl:with-param>
          <xsl:with-param name="replacement" select="'__'"></xsl:with-param>
      </xsl:call-template>
<!--       <xsl:value-of select="substring-before( replace(.,'_','__'), '::')" /> -->
<!-- Doxygen escapes the following characters with underscore: -->
<!--       <xsl:value-of select="replace(.,'_','__')" /> -->
<!--       <xsl:value-of select="replace(.,'.','_8')" /> -->
<!--       <xsl:value-of select="replace(.,':','_1')" /> -->
    </xsl:variable>
    <xsl:variable name="orocos.doxygen.filename">
      <xsl:text>class</xsl:text>
      <xsl:value-of select="$orocos.doxygen.namespace" />
      <xsl:text>_1_1</xsl:text><!-- ':' maps to '_1' -->
      <xsl:value-of select="$orocos.doxygen.classname" />
    </xsl:variable>
      
    <xsl:choose>
    <xsl:when test="contains(.,'::')">
      <a href="{$orocos.doxygen.path}/{$orocos.doxygen.filename}.html">
      <xsl:value-of select="$orocos.doxygen.classname" />
      </a>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="inline.monoseq"/>
    </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
      
      

</xsl:stylesheet>
