<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="page">
   <html>
    <head>
     <title>
      <xsl:value-of select="title"/>
     </title>
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
   <p align="left">
    <i><xsl:apply-templates/></i>
   </p>
   <xsl:if test="not(following-sibling::para)">
     <p align="left"><i>Notes from the stylesheet:<br/>
       Page above was processed by static XSL stylesheet.<br/>
       My source could be found in the file 
       <b>cocoon/sub/stylesheets/simple-page2html.xsl</b>.<br/>
       I know nothing about the environment which is using me.
       </i>
     </p>
   </xsl:if>
  </xsl:template>

  <xsl:template match="@*|node()" priority="-1">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
