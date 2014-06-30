<?xml version="1.0" encoding="iso-8859-1"?>

<xsl:stylesheet version="1.0" 
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     xmlns:map="http://apache.org/cocoon/sitemap/1.0"
     >
 <xsl:template match="/">
  <div>
   <p>Please wait while operation executes...</p>
   <xsl:apply-templates />
  </div>
 </xsl:template>

 <xsl:template match="result[@type='success']">
  <p>Operation executed successfully</p>
  <p>Please click 
   <a>
    <xsl:attribute name="href"><xsl:value-of select="@redirect"/></xsl:attribute>
    here
   </a>
    to proceed</p>
    <script language="javascript">document.location="<xsl:value-of select="@redirect"/>"</script>
 </xsl:template>


 <xsl:template match="result[@type='failure']">
  <p>Operation failed</p>
  <p>Please click 
   <a>
    <xsl:attribute name="href"><xsl:value-of select="@redirect"/></xsl:attribute>
    here
   </a>
    to proceed</p>
   <hr/>
   <p>Exception stack trace</p>
   <xsl:value-of select="stacktrace" />
 </xsl:template>


</xsl:stylesheet>