<?xml version="1.0" encoding="iso-8859-1"?>

<!-- 
Author: Sergio Carvalho "scarvalho@criticalsoftware.com"
Changelog:

First release: 

Description: Sub-stylesheet to present a Cocoon 2 sitemap.xmap file in HTML.
-->
<xsl:stylesheet version="1.0" 
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     xmlns:map="http://apache.org/cocoon/sitemap/1.0"
>
 <xsl:param name="siteName" />

 <xsl:template match="/">
  <table class="sitebuilder_sitemapView">
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewTitle">
     Open Site
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Sitemaps
    </td>
   </tr>
   <xsl:apply-templates select="descendant::map:mount" />
  </table>
 </xsl:template>


 <xsl:template match="map:mount">
  <tr>
   <td class="sitebuilder_sitemapViewPartList">
    <xsl:value-of select="substring-before(@uri-prefix,'/')" />
   </td>
   <td class="sitebuilder_sitemapViewEdit">
    <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/view/?siteName=<xsl:value-of select="$siteName" /><xsl:value-of select="substring-before(@src,'/')" /></xsl:attribute>[Open]
    </a>
   </td>
  </tr> 
 </xsl:template>

</xsl:stylesheet>
