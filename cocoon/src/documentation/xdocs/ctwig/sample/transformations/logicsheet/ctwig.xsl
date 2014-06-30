<?xml version="1.0"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:ctwig="http://www.pigbite.com/xsl"
  version="1.0">

<xsl:template match="ctwig:greeting">
 
  <xsp:logic>
    String msg = "Hello World";
  </xsp:logic>

  <xsp:expr>msg</xsp:expr>
</xsl:template>

<xsl:template match="@*|node()" priority="-1">
 <xsl:copy>
  <xsl:apply-templates select="@*|node()"/>
 </xsl:copy>
</xsl:template>

</xsl:stylesheet>
