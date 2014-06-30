<?xml version="1.0"?>

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0">

  <!-- match the root book element -->
  <xsl:template match="/book">
    <sidebar>
      <xsl:apply-templates/>
    </sidebar>
  </xsl:template>

  <xsl:template match="/book/external">
    <link>
      <xsl:copy-of select="@href|@label"/>
    </link>
  </xsl:template>

  <xsl:template match="/book/page | /book/faq | /book/changes | /book/todo">
    <link href="{substring(@source,1,string-length(@source)-4)}">
      <xsl:copy-of select="@label|@id"/>
    </link>
   </xsl:template>

  <xsl:template match="/book/separator">
    <xsl:copy/>
  </xsl:template>

</xsl:stylesheet>
