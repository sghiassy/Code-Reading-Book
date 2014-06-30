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
 <xsl:template match="map:sitemap">
  <table class="sitebuilder_sitemapView">
   <tr>
    <td class="sitebuilder_sitemapViewTitle">
     Edit Sitemap
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/view/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[View]
     </a><br />

    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Generators
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newGenerator/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]
     </a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:generators/map:generator" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Transformers
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newTransformer/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]
     </a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:transformers/map:transformer" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Readers
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newReader/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:readers/map:reader" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Serializers
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newSerializer/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:serializers/map:serializer" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Selectors
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newSelector/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:selectors/map:selector" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Matchers
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newMatcher/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:matchers/map:matcher" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Actions
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newAction/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:actions/map:action" />
    </td>
   </tr>
   <tr>
    <td class="sitebuilder_sitemapViewPartTitle">
     Pipelines
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/newPipeline/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Add]</a><br />
    </td>
   </tr>
   <xsl:apply-templates select="map:pipelines/map:pipeline" />
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewTitle">
     End of Sitemap Description
    </td>
   </tr>  
  </table>
 </xsl:template>

 <xsl:template match="map:generator|map:transformer|map:reader|map:serializer|map:selector|map:matcher|map:action">
  <xsl:choose>
   <xsl:when test="@name=../@default">
    <b><xsl:value-of select="@name" /></b><br />
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="@name" /><br />
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>
 
 <xsl:template match="map:pipeline">
  <tr>
   <td class="sitebuilder_sitemapViewPartList">
    Pipeline <xsl:value-of select="position()" /> 
    <xsl:if test="@internal-only"> (Internal Pipeline)</xsl:if>
    <br/>
    <div style="margin-left: 2em">
     <xsl:apply-templates select="map:match" />
    </div>
   </td>
   <td class="sitebuilder_sitemapViewEdit">
    <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Remove]</a><br/>
    <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/pipeline/edit/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="@id" /></xsl:attribute>[Edit]</a><br />
   </td>
  </tr>
 </xsl:template>
 
 <xsl:template match="map:match">
  Matches: "<i><xsl:value-of select="@pattern" /></i>"<br/>
 </xsl:template>

 
</xsl:stylesheet>
