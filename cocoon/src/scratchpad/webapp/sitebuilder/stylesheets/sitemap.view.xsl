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
     View Sitemap
    </td>
    <td class="sitebuilder_sitemapViewEdit">
     <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/sitemap/edit/?siteName=<xsl:value-of select="$siteName" /></xsl:attribute>[Edit]

     </a>

    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Generators
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:generators/map:generator" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Transformers
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:transformers/map:transformer" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Readers
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:readers/map:reader" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Serializers
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:serializers/map:serializer" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Selectors
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:selectors/map:selector" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Matchers
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:matchers/map:matcher" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Actions
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:components/map:actions/map:action" />
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartTitle">
     Pipelines
    </td>
   </tr>
   <tr>
    <td colspan="2" class="sitebuilder_sitemapViewPartList">
     <xsl:apply-templates select="map:pipelines/map:pipeline" />
    </td>
   </tr>
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
  Pipeline <xsl:value-of select="position()" /> 
  <xsl:if test="@internal-only"> (Internal Pipeline)</xsl:if>
  <br/>
  <div style="margin-left: 2em">
   <xsl:apply-templates select="map:match" />
  </div>
 </xsl:template>
 
 <xsl:template match="map:match">
  Matches: "<i><xsl:value-of select="@pattern" /></i>"<br/>
 </xsl:template>

 
</xsl:stylesheet>
