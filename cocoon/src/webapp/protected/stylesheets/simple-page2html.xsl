<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="page">
    <html>
      <head>
        <title>
          <xsl:value-of select="title"/>
        </title>
        <META content="0" http-equiv="expires"/>
        <META content="nocache" http-equiv="pragma"/>
      </head>
      <body bgcolor="white" alink="red" link="blue" vlink="blue">
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="title">
    <h2 style="color: navy; text-align: center">
      <xsl:apply-templates/>
    </h2>
  </xsl:template>

  <xsl:template match="para">
    <p align="center">
      <i><xsl:apply-templates/></i>
    </p>
  </xsl:template>

  <xsl:template match="form">
    <form method="POST" action="{@target}">
      <xsl:apply-templates/>
    </form>
  </xsl:template>

  <xsl:template match="input">
    <center>
      <xsl:value-of select="@title"/>
      <input type="{@type}" name="{@name}" value="{.}"/>
    </center><br/>
  </xsl:template>

  <xsl:template match="linkbar">
    <center>
      [
      <a href="login"> login </a>
      |
      <a href="protected"> protected </a>
      |
      <a href="do-logout"> logout </a>
      ]
    </center>
  </xsl:template>

</xsl:stylesheet>
<!-- vim: set et ts=2 sw=2: -->
