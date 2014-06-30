<?xml version='1.0'?>
<!DOCTYPE xsl:stylesheet [
 <!ENTITY % ISOnum PUBLIC
   "ISO 8879:1986//ENTITIES Numeric and Special Graphic//EN//XML"
   "ISOnum.pen">
 %ISOnum;
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="html"/>

<xsl:template match="catalog-demo">
<html>
 <head>
  <title>Demonstration of entity resolution using catalogs</title>
 </head>
 <body>
  <xsl:apply-templates/>
  <p>
   This footer is applied by the stylesheet. The following entity is resolved
   by the parser when it interprets the stylesheet
   <br/>Use &amp;frac14; to represent &frac14; (one-quarter symbol)
   <br/>The ISOnum entity set was declared in the header of the stylesheet.
  </p>
 </body>
</html>
</xsl:template>

<xsl:template match="section">
 <xsl:apply-templates/>
<hr/>
</xsl:template>

<xsl:template match="para">
<p>
 <xsl:apply-templates/>
</p>
</xsl:template>

<xsl:template match="link">
<a href="{@href}"><xsl:apply-templates/></a>
</xsl:template>

</xsl:stylesheet>
