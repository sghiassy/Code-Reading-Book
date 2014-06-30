<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:error="http://apache.org/cocoon/error/2.0">

<xsl:template match="error:notify">
 <document>
   <header>
     <title>
       <xsl:if test="@type">
         <xsl:value-of select="@type"/>:
       </xsl:if>
       <xsl:value-of select="error:title"/>
     </title>
   </header>
   <body>
     <s1 title="Details">
       <s2 title="Message">
         <xsl:if test="@type">
           <xsl:value-of select="@type"/>:
         </xsl:if>
         <xsl:value-of select="error:message"/>
       </s2>
       <xsl:if test="@sender">
         <s2 title="From">
           <p><xsl:value-of select="@sender"/></p>
         </s2>
       </xsl:if>
       <s2 title="Source">
         <p><xsl:value-of select="error:source"/></p>
       </s2>
       <s2 title="Description">
         <p><xsl:value-of select="error:description"/></p>
       </s2>
     </s1>
     <s1 title="Stack Traces">
       <xsl:apply-templates select="error:extra"/>
     </s1>
   </body>
 </document>
</xsl:template>

  <xsl:template match="error:extra">
    <note><code><xsl:value-of select="."/></code></note>
 </xsl:template>
</xsl:stylesheet>
