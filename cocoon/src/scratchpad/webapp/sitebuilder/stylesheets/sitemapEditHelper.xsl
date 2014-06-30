<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>
 <xsl:variable name="nextElementId"><xsl:value-of select="count(/descendant::node()/@id) + 1" /></xsl:variable>
 
 <xsl:template name="emitNextElementId">
  <xsl:attribute name="id"><xsl:value-of select="$nextElementId" /></xsl:attribute>
 </xsl:template>

 <xsl:template match="@*|*|text()|processing-instruction()" priority="-1">
  <xsl:copy>
   <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
  </xsl:copy>
 </xsl:template>

</xsl:stylesheet>




