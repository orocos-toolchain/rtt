<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:src="http://nwalsh.com/xmlns/litprog/fragment"
  exclude-result-prefixes="src" version="1.0">

<!-- 
  These parameters are specific for the Orocos project and override
  the default ones. They must be set first !
 -->
  <xsl:param name="draft.mode" select="'no'"/>
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="fop.extensions" select="'1'"/>

<!-- Include the default settings -->
  <xsl:include href="/usr/share/sgml/docbook/stylesheet/xsl/nwalsh/fo/docbook.xsl"/>


</xsl:stylesheet>