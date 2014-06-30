<?xml version="1.0" encoding="iso-8859-1"?>

<!-- 
Author: Sergio Carvalho "scarvalho@criticalsoftware.com"
Changelog:
First version: 2000/12/21
First release: 2000/12/30

Description: Sub-stylesheet handling the main menu content part of a page.
-->
<xsl:stylesheet version="1.0" 
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     xmlns:critical="http://www.criticalsoftware.com/NAMESPACES/CRITICAL/V1"
>

 <xsl:template match="critical:menu">
  <table class="sitebuilder_mainMenu">
   <xsl:apply-templates/>
   <tr height="4"><td/></tr>
  </table>
 </xsl:template>

 <xsl:template match="critical:menu//critical:channel">
   <tr class="sitebuilder_mainMenu">
    <td height="4"/>
   </tr>
   <tr>
   <!-- bgcolor="#000099" align="center" -->
    <td class="sitebuilder_mainMenuHeader">
      <xsl:value-of select="@name"/>
    </td>
   </tr>
   <xsl:apply-templates/>

  </xsl:template>

  <xsl:template match="critical:menu//critical:item">
   <tr> 
<!-- <div align="center"><font face="Verdana, Arial, Helvetica, sans-serif" size="-2" color="#330066"><b> -->
    <td class="sitebuilder_mainMenuItem" height="4"><a class="sitebuilder_mainMenuItem"><xsl:attribute name="href"><xsl:value-of select="@link"/></xsl:attribute><xsl:value-of select="@title"/></a></td>
   </tr>
  </xsl:template>

</xsl:stylesheet>




