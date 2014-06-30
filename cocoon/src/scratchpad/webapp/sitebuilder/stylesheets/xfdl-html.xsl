<?xml version="1.0" encoding="iso-8859-1"?>

<xsl:stylesheet version="1.0" 
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     >
     
 <!-- Entry template. This template can be matched or called. -->
 <!-- XFDL mode is set on every other template to speed processing -->
 <!-- and avoid conflicts.                                    -->
 <xsl:template match="XFDL" name="xfdl.XFDL">
  <form method="post">
   <!-- The form action is that of the first submit button. -->
   <!-- Without DOM handling, HTML forms can't be submitted -->
   <!-- to several URLs.                                    -->
   <xsl:attribute name="action"><xsl:value-of select="page/button[type='done' or type='submit'][1]/url/*[1]" /></xsl:attribute>
   <xsl:apply-templates mode="XFDL" />
  </form>
 </xsl:template>

 <!-- ** XFDL mode templates ** -->
 <!-- bgcolor option -->
 <xsl:template match="bgcolor" mode="XFDL" />

 <!-- Page -->
 <xsl:template match="page" mode="XFDL">
  <div>
   <table cellpadding="0"   cellspacing="0" border="0">
    <xsl:apply-templates mode="XFDL-page" />
   </table>
  </div>
 </xsl:template>

 <!-- ** XFDL-page mode templates ** -->
 <!-- bgcolor option -->
 <xsl:template match="bgcolor" mode="XFDL-page" />
 
 <xsl:template match="label" mode="XFDL-page">
  <tr>
   <td colspan="2">
    <xsl:value-of select="text()" />
   </td>
  </tr>
 </xsl:template>

 <!-- Field Item -->
 <xsl:template match="field" mode="XFDL-page">
  <xsl:choose>
   <xsl:when test="visible='off' or visible='false'">
    <input type="hidden">
     <xsl:attribute name="name"><xsl:value-of select="@sid" /></xsl:attribute>
     <xsl:attribute name="value"><xsl:value-of select="value" /></xsl:attribute>
    </input>
   </xsl:when>
   <xsl:otherwise>
    <tr>
     <td>
      <xsl:value-of select="label" />
     </td>
     <td>
      <xsl:choose>
       <xsl:when test="editstate='readonly'"><xsl:value-of select="value" />
        <input type="hidden">
         <xsl:attribute name="name"><xsl:value-of select="@sid" /></xsl:attribute>
         <xsl:attribute name="value"><xsl:value-of select="value" /></xsl:attribute>
        </input>
       </xsl:when>
       <xsl:otherwise>
        <input>
         <xsl:attribute name="name"><xsl:value-of select="@sid" /></xsl:attribute>
         <xsl:attribute name="value"><xsl:value-of select="value" /></xsl:attribute>
        </input>
       </xsl:otherwise>
      </xsl:choose>
     </td>
    </tr>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>

 <!-- Button Item -->
 <xsl:template match="button" mode="XFDL-page">
  <tr>
   <td colspan="2">
    <input type="submit">
     <xsl:if test="@value"><xsl:attribute name="value"><xsl:value-of select="@value" /></xsl:attribute></xsl:if>
    </input>
   </td>
  </tr>
 </xsl:template>

 <!-- XFDL requires us to pass on non-XFDL markup -->
 <xsl:template name="XFDL-copy-template">
  <xsl:copy>
   <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
  </xsl:copy>
 </xsl:template>

 <xsl:template match="@*|*|text()|processing-instruction()" priority="-1" mode="XFDL">
  <xsl:call-template name="XFDL-copy-template" />
 </xsl:template>

 <xsl:template match="@*|*|text()|processing-instruction()" priority="-1" mode="XFDL-page">
  <xsl:call-template name="XFDL-copy-template" />
 </xsl:template>

 <xsl:template match="@*|*|text()|processing-instruction()" priority="-1" mode="XFDL-item">
  <xsl:call-template name="XFDL-copy-template" />
 </xsl:template>
</xsl:stylesheet>
