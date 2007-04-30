<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:src="http://nwalsh.com/xmlns/litprog/fragment"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  exclude-result-prefixes="src" version="1.0">

<!-- THIS XSL IS FOR GENERATING FO OUTPUT -->

<!-- Include the default settings -->
  <xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl"/>

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

  <!--###################################################
                      Center figure captions
    ################################################### -->
  
  <xsl:attribute-set name="informalfigure.properties">
    <xsl:attribute name="text-align">center</xsl:attribute>
    <xsl:attribute name="align">center</xsl:attribute>
  </xsl:attribute-set>
 
  <xsl:attribute-set name="figure.properties">
    <xsl:attribute name="text-align">center</xsl:attribute>
    <xsl:attribute name="align">center</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="component.title.properties">
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>

<!-- use this to select the tiny 'note','warning',... images format for ps/pdf -->
  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="admon.graphics.path" select="'images/icons/'"/>
  <xsl:param name="admon.graphics.extension" select="'.png'"/>

  <xsl:param name="shade.verbatim" select="1"></xsl:param>

  <!-- use this to select the image type used for pdf / ps output across all files. -->
  <!-- 'gif' is natively supported by fop, 'png' requires the SUN 'jimi-1.0' library. -->
  <xsl:param name="graphic.default.extension" select="'png'"></xsl:param>

  <xsl:param name="hyphenate.verbatim" select="0"></xsl:param>

<xsl:attribute-set name="monospace.verbatim.properties"
		   use-attribute-sets="verbatim.properties">
  <xsl:attribute name="wrap-option">wrap</xsl:attribute>
  <xsl:attribute name="hyphenation-character">\</xsl:attribute>
  <xsl:attribute name="font-size">11pt</xsl:attribute>
</xsl:attribute-set>

  <xsl:param name="monospace.font.family" select="'Times'"/>

<!-- convert classname to to doxygen filenames
     xsltproc does not support fn:replace, so needed the nwalsh 'string.subst' template
     to do it, otherwise, the shorter commented line could have been used :-( -->
<!-- This template only applies if the format is of 
      
         <classname>Namespace::Classname</classname>
     Otherwise, the default docbook style is applied.
-->
  <xsl:variable name="orocos.doxygen.path" select="'api/html'" />
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
      <!-- Only print the class name, no ulink or similar generated yet... -->
      <xsl:value-of select="$orocos.doxygen.classname" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="inline.monoseq"/>
    </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:param name="body.font.master" select="12" />

<!--   <xsl:param name="linenumbering.extension" select="0"></xsl:param> -->
<!--   <xsl:param name="use.extensions" select="1"></xsl:param> -->

<!-- Caption below figure -->
<xsl:template match="figure">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>
  
  <fo:block id="{$id}"
	    xsl:use-attribute-sets="formal.object.properties">
    <xsl:apply-templates/>
    <xsl:call-template name="formal.object.heading"/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>