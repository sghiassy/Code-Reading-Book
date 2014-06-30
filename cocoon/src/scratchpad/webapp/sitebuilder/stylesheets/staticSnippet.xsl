<?xml version="1.0" encoding="iso-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="staticSnippetTarget">/</xsl:param>
<xsl:param name="staticSnippetWidth">100</xsl:param>
<xsl:param name="staticSnippetHeight">100</xsl:param>

<!-- Match The Root Node -->
 <xsl:template match="dummy">
  <iframe scrolling="no" frameborder="0" marginwidth="0" marginheheight="0">
   <xsl:attribute name="src"><xsl:value-of select="$staticSnippetTarget" /></xsl:attribute>
   <xsl:attribute name="width"><xsl:value-of select="$staticSnippetWidth" /></xsl:attribute>
   <xsl:attribute name="height"><xsl:value-of select="$staticSnippetHeight" /></xsl:attribute>
   [Destaque Portugalmail]
  </iframe>
 </xsl:template>

</xsl:stylesheet>






