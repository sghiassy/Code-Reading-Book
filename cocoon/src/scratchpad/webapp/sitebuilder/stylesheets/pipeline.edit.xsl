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
 <xsl:param name="pipelineId" />

 <xsl:template match="map:pipeline">
  <table class="sitebuilder_sitemapView">
   <tr>
    <td class="sitebuilder_sitemapViewTitle">
     Edit Pipeline
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/edit/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Edit Sitemap]
     </a><br />

    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Matches
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/pipeline/newMatch/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>

   <xsl:apply-templates select="map:match" />

   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewTitle">
     End of Pipeline Description
    </td>
   </tr>  
  </table>
 </xsl:template>

 <xsl:template match="map:match">
   <tr>
    <td class="sitebuilder_sitemapViewPartList">
     Match for <em>"<xsl:value-of select="@pattern" />"</em>
    </td>
    <td class="sitebuilder_sitemapViewEdit">
    <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/pipeline/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" /></xsl:attribute>[Remove]</a><br/>
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/edit/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="@id" /></xsl:attribute>[Edit]</a><br />
    </td>
   </tr>
 </xsl:template>

 
</xsl:stylesheet>
