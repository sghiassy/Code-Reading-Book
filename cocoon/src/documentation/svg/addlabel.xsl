<?xml version="1.0"?>

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:NetUtils="org.apache.cocoon.util.NetUtils"
    version="1.0">

<xsl:param name="label"/>

 <xsl:template match="label">
    <xsl:value-of select="NetUtils:decodePath($label)"/>
 </xsl:template>

  <xsl:template match="@*|node()">
   <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
   </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
