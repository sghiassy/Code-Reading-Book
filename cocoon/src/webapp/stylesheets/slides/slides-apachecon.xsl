<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="slide">
   <html>
    <head>
     <title><xsl:value-of select="title"/></title>
     <link rel="stylesheet" type="text/css" href="style" title="Style"/>
    </head>
    <body bgcolor="white">
     <table width="100%">
      <tr>
       <td>
        <table width="100%">
         <tr>
          <td width="100%"><img src="../images/cocoon.gif" border="0"/></td>
          <td>
           <xsl:apply-templates select="navigation"/>
          </td>
         </tr>
        </table>
       </td>
      </tr>
      <tr>
       <td>
        <div id="main">
         <xsl:apply-templates select="layout"/>
        </div>
       </td>
      </tr>
     </table>
    </body>
   </html>
  </xsl:template>

  <xsl:template match="navigation">
   <table>
    <tr>
     <td>
      <xsl:if test="previous">
       <a href="{previous/@href}"><img src="../images/slides/previous.png" border="0"/></a>
      </xsl:if>
      <xsl:if test="not(previous)">
       <img src="../images/slides/previous.png" border="0"/>
      </xsl:if>
     </td>
     <td>
      <xsl:if test="previous-section">
       <a href="{previous-section/@href}"><img src="../images/slides/previous-section.png" border="0"/></a>
      </xsl:if>
      <xsl:if test="not(previous-section)">
       <img src="../images/slides/previous-section.png" border="0"/>
      </xsl:if>
     </td>
     <td>
      <xsl:if test="home">
       <a href="{home/@href}"><img src="../images/slides/home.png" border="0"/></a>
      </xsl:if>
      <xsl:if test="not(home)">
       <img src="../images/slides/home.png" border="0"/>
      </xsl:if>
     </td>
     <td>
      <xsl:if test="next-section">
       <a href="{next-section/@href}"><img src="../images/slides/next-section.png" border="0"/></a>
      </xsl:if>
      <xsl:if test="not(next-section)">
       <img src="../images/slides/next-section.png" border="0"/>
      </xsl:if>
     </td>
     <td>
      <xsl:if test="next">
       <a href="{next/@href}"><img src="../images/slides/next.png" border="0"/></a>
      </xsl:if>
      <xsl:if test="not(next)">
       <img src="../images/slides/next.png" border="0"/>
      </xsl:if>
     </td>
    </tr>
   </table>
  </xsl:template>

  <xsl:template match="layout">
   <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="p|ol|ul|li|em|strong|br">
   <xsl:copy>
    <xsl:apply-templates/>
   </xsl:copy>
  </xsl:template>

  <xsl:template match="fork">
   <a href="{@uri}" target="_blank">
    <xsl:apply-templates/>
   </a>
  </xsl:template>

  <xsl:template match="link">
   <a href="{@uri}">
    <xsl:apply-templates/>
   </a>
  </xsl:template>

  <xsl:template match="title">
   <h1>
    <xsl:apply-templates/>
   </h1>
  </xsl:template>

  <xsl:template match="subtitle">
   <h3>
    <xsl:text> [</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>]</xsl:text>
   </h3>
  </xsl:template>

  <xsl:template match="quote">
   <p class="quote">
    <xsl:apply-templates/>
   </p>
  </xsl:template>

  <xsl:template match="source">
   <pre>
    <xsl:apply-templates/>
   </pre>
  </xsl:template>

  <xsl:template match="figure">
   <p class="figure" align="center">
    <img src="{@src}"/>
   </p>
  </xsl:template>
  
</xsl:stylesheet>
