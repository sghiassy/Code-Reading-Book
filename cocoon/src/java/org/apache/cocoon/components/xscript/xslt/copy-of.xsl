<?xml version="1.0" encoding="utf-8"?>

<!-- Author: Ovidiu Predescu "ovidiu@cup.hp.com" -->
<!-- Date: July 27, 2001 -->
<!-- Implement the xscript:copy-of support -->

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xalan="http://xml.apache.org/xalan"
  xmlns:saxon="http://icl.com/saxon"
  exclude-result-prefixes="xalan"
  >

  <xsl:param name="xpath"/>

  <xsl:template match="/">
    <xsl:choose>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'xalan')">
        <xsl:copy-of select="xalan:evaluate($xpath)"/>
      </xsl:when>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'saxon')">
        <xsl:copy-of select="saxon:evaluate($xpath)"/>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
