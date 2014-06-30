<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp-request="http://apache.org/xsp/request/2.0"
>

  <xsl:include href="page/simple-page2html.xsl"/>

  <xsl:template match="xsp-request:uri">
    <b><xsl:value-of select="."/></b>
  </xsl:template>

  <xsl:template match="xsp-request:parameter">
    <i><xsl:value-of select="@name"/></i>:<b><xsl:value-of select="."/></b>
  </xsl:template>

  <xsl:template match="xsp-request:parameter-values">
    <p>Parameter Values for "<xsl:value-of select="@name"/>":</p>

    <ul>
      <xsl:for-each select="xsp-request:value">
        <li>
	  <xsl:value-of select="."/>
	</li>
      </xsl:for-each>
    </ul>
  </xsl:template>

  <xsl:template match="xsp-request:parameter-names">
    <p>All Parameter Names:</p>

    <ul>
      <xsl:for-each select="xsp-request:name">
        <li>
	  <xsl:value-of select="."/>
	</li>
      </xsl:for-each>
    </ul>
  </xsl:template>

  <xsl:template match="xsp-request:headers">
    <p>Headers:</p>
    
    <ul>
      <xsl:for-each select="xsp-request:header">
	<li>
          <i><xsl:value-of select="@name"/></i>:
          <b><xsl:value-of select="."/></b>
	</li>
      </xsl:for-each>
    </ul>
    <br/>
  </xsl:template>

  <xsl:template match="xsp-request:header">
    <i><xsl:value-of select="@name"/></i>:<b><xsl:value-of select="."/></b>
  </xsl:template>

  <xsl:template match="xsp-request:header-names">
    <p>All Header names:</p>

    <ul>
      <xsl:for-each select="xsp-request:name">
        <li>
	  <xsl:value-of select="."/>
	</li>
      </xsl:for-each>
    </ul>
  </xsl:template>

  <xsl:template match="@*|node()" priority="-1">
   <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
   </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
