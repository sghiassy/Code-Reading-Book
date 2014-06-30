<?xml version="1.0"?>

<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0"
>

<xsl:param name="path"/>

<xsl:template match="/root">
  <html>
    <head>
      <title><xsl:value-of select="document/header/title"/></title>
    </head>
    <body>
      <table cellpadding="0" cellspacing="3">
        <tr>
          <td valign="top">
            <xsl:apply-templates select="sidebar"/>
          </td>
          <td width="10">&#160;</td>
          <td valign="top">
            <xsl:apply-templates select="document/body"/>
          </td>
        </tr>
      </table>
    </body>
  </html>
</xsl:template>

<xsl:template match="/root/sidebar">
  <table cellpadding="1" cellspacing="2" bgcolor="#000000">
    <tr bgcolor="#0086b2"><td>
      <table cellpadding="1" cellspacing="0">
        <xsl:apply-templates/>
      </table>
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="sidebar/link">
  <tr>
    <td>
      <xsl:choose>
        <xsl:when test="@href=$path">
          <font color="#FFFFFF"><xsl:value-of select="@label"/></font>
        </xsl:when>
        <xsl:otherwise>
          <a>
            <xsl:copy-of select="@href"/>
            <font color="#FFFFFF"><xsl:value-of select="@label"/></font>
          </a>
        </xsl:otherwise>
      </xsl:choose>
    </td>
  </tr>
</xsl:template>

<xsl:template match="sidebar/separator">
  <tr><td height="10"></td></tr>
</xsl:template>

<xsl:template match="/root/document/body">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="body//s1">
  <h1><xsl:value-of select="@title"/></h1>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="body//s2">
  <h2><xsl:value-of select="@title"/></h2>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="body//s3">
  <h3><xsl:value-of select="@title"/></h3>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="body//s4">
  <h4><xsl:value-of select="@title"/></h4>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="body//link">
  <a>
    <xsl:copy-of select="@href"/>
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="source">
  <listing style="color: green; font-size: 80%;">
    <xsl:apply-templates />
  </listing>
</xsl:template>

<xsl:template match="body//node()|body//@*" priority="-1">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="body//figure">
  <xsl:element name="img">
    <xsl:for-each select="@*">
      <xsl:attribute name="{name(.)}">
        <xsl:value-of select="."/>
      </xsl:attribute>
    </xsl:for-each>
  </xsl:element>
</xsl:template>

<xsl:template match="link">
  <a href="{@href}"><xsl:apply-templates/></a>
</xsl:template>

<xsl:template match="connect">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="jump">
  <a href="{@href}#{@anchor}"><xsl:apply-templates/></a>
</xsl:template>

<xsl:template match="fork">
  <a href="{@href}" target="_blank"><xsl:apply-templates/></a>
</xsl:template>

<xsl:template match="anchor">
  <a name="{@id}"><xsl:comment>anchor</xsl:comment></a>
</xsl:template>  

<xsl:template match="tr[position() mod 2 = 0]">
  <tr bgcolor="#B8EEFF">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </tr>
</xsl:template>  

<xsl:template match="th">
  <th bgcolor="#0086B2">
    <xsl:copy-of select="@*"/>
    <font color="#FFFFFF"><xsl:apply-templates/></font>
  </th>
</xsl:template>  

</xsl:stylesheet>
