<?xml version="1.0" encoding="iso-8859-1"?>
<!-- 
Author: Sergio Carvalho "scarvalho@criticalsoftware.com"
Changelog:

First release: 

Description: 
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:map="http://apache.org/cocoon/sitemap/1.0">
  <xsl:param name="siteName" />

  <xsl:param name="pipelineId" />

  <xsl:param name="matchId" />

  <xsl:template match="map:match">
    <table class="sitebuilder_sitemapView">
      <tr>
        <td class="sitebuilder_sitemapViewTitle">Edit Match for "
        <em class="sitebuilder_sitemapViewTitle">
          <xsl:value-of select="@pattern" />
        </em>

        "</td>

        <td class="sitebuilder_sitemapViewEdit">
          <a class="sitebuilder_sitemapViewEdit">
          <xsl:attribute name="href">/cocoon/sitebuilder/pipeline/edit/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" /></xsl:attribute>

          [Edit Pipeline]</a>

          <br />
        </td>
      </tr>

      <tr>
        <td class="sitebuilder_sitemapViewPartTitle">Generator</td>

        <td class="sitebuilder_sitemapViewEdit">
          <xsl:if test="count(map:generate)+count(map:read)+count(map:aggregate) = 0">
            <a class="sitebuilder_sitemapViewEdit">
            <xsl:attribute name="href">/cocoon/sitebuilder/match/newAggregate/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" /></xsl:attribute>
            [Add Aggregation]</a>

            <br />

            <a class="sitebuilder_sitemapViewEdit">
            <xsl:attribute name="href">/cocoon/sitebuilder/match/newGenerate/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" /></xsl:attribute>

            [Add Generator]</a>

            <br />

            <a class="sitebuilder_sitemapViewEdit">
            <xsl:attribute name="href">/cocoon/sitebuilder/match/newRead/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" /></xsl:attribute>

            [Add Reader]</a>

            <br />
          </xsl:if>
        </td>
      </tr>

      <xsl:call-template name="generator_edit" />

      <xsl:if test="count(map:read) = 0">
        <tr>
          <td class="sitebuilder_sitemapViewPartTitle">Transformers</td>

          <td class="sitebuilder_sitemapViewEdit">
              <a class="sitebuilder_sitemapViewEdit">
              <xsl:attribute name="href">/cocoon/sitebuilder/match/newTransform/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" /></xsl:attribute>

              [Add]</a>

              <br />
          </td>
        </tr>

        <xsl:apply-templates select="map:transform" />

        <tr>
          <td class="sitebuilder_sitemapViewPartTitle">Serializer</td>

          <td class="sitebuilder_sitemapViewEdit">
            <xsl:if test="count(map:serialize) = 0">
              <a class="sitebuilder_sitemapViewEdit">
              <xsl:attribute name="href">/cocoon/sitebuilder/match/newSerialize/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" /></xsl:attribute>

              [Add]</a>

              <br />
            </xsl:if>
          </td>
        </tr>

        <xsl:call-template name="serializer_edit" />
      </xsl:if>

      <tr>
        <td colspan="2" class="sitebuilder_sitemapViewTitle">End of Match Description</td>
      </tr>
    </table>
  </xsl:template>

  <xsl:template name="generator_edit">
    <xsl:choose>
      <xsl:when test="count(map:generate|map:read|map:aggregate) &gt; 0">
        <xsl:apply-templates select="map:generate|map:read|map:aggregate" />
      </xsl:when>

      <xsl:otherwise>
        <tr>
          <td class="sitebuilder_sitemapViewPartList">
            <div class="sitebuilder_error">No Generator defined! Please click add to define it.</div>
          </td>

          <td class="sitebuilder_sitemapViewEdit">
          </td>
        </tr>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="serializer_edit">
    <xsl:choose>
      <xsl:when test="count(map:serialize) &gt; 0">
        <xsl:apply-templates select="map:serialize" />
      </xsl:when>

      <xsl:otherwise>
        <tr>
          <td class="sitebuilder_sitemapViewPartList">
            <div class="sitebuilder_error">No Serializer defined! Please click add to define it.</div>
          </td>

          <td class="sitebuilder_sitemapViewEdit">
          </td>
        </tr>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="map:aggregate">
    <tr>
      <td class="sitebuilder_sitemapViewPartList">
        Aggregation under <em>&lt;<xsl:if test="@ns"><xsl:value-of select="@ns" />:</xsl:if><xsl:value-of select="@element" />&gt;</em> of:
      </td>
      <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
            <a class="sitebuilder_sitemapViewEdit">
            <xsl:attribute name="href">/cocoon/sitebuilder/match/newGeneratePart/?siteName=<xsl:value-of select="$siteName" />&amp;pipelineId=<xsl:value-of select="$pipelineId" />&amp;matchId=<xsl:value-of select="$matchId" />&amp;aggregateId=<xsl:value-of select="@id" /></xsl:attribute>
            [Add Part]</a>
            <br />
      </td>
    </tr>
    <xsl:apply-templates select="map:part" />
  </xsl:template>

  <xsl:template match="map:part">
   <tr>
    <td class="sitebuilder_sitemapViewPartList">
    <div class="sitebuilder_aggregateParts"><xsl:value-of select="@src" /> under <xsl:if test="@ns">'<xsl:value-of select="@ns" />':</xsl:if><em>&lt;<xsl:value-of select="@element" />&gt;</em></div>
    </td>
    <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
    </td>
   </tr>
  </xsl:template>

  <xsl:template match="map:read">
    <tr>
      <td class="sitebuilder_sitemapViewPartList">
       Reader for <xsl:value-of select="@src" />
      </td>
      <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
      </td>
    </tr>
  </xsl:template>



  <xsl:template match="map:generate">
    <tr>
      <td class="sitebuilder_sitemapViewPartList">
      <xsl:choose>
        <xsl:when test="@type">
          <xsl:value-of select="@type" />
        </xsl:when>

        <xsl:otherwise>
          <em>default</em>
        </xsl:otherwise>
      </xsl:choose>

      @ 
      <xsl:value-of select="@src" />
      </td>
      <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="map:transform">
    <tr>
      <td class="sitebuilder_sitemapViewPartList">
      <xsl:choose>
        <xsl:when test="@type">
          <xsl:value-of select="@type" />
        </xsl:when>

        <xsl:otherwise>
          <em>default</em>
        </xsl:otherwise>
      </xsl:choose>

      @ 
      <xsl:value-of select="@src" />
      </td>
      <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="map:serialize">
    <tr>
      <td class="sitebuilder_sitemapViewPartList">
        <xsl:choose>
          <xsl:when test="@type">
            <xsl:value-of select="@type" />
          </xsl:when>

          <xsl:otherwise>
            <em>default</em>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td class="sitebuilder_sitemapViewEdit">
       <a class="sitebuilder_sitemapViewEdit"><xsl:attribute name="href">/cocoon/sitebuilder/match/deleteElement/process?siteName=<xsl:value-of select="$siteName" />&amp;elementId=<xsl:value-of select="@id" />&amp;followup_siteName=<xsl:value-of select="$siteName" />&amp;followup_pipelineId=<xsl:value-of select="$pipelineId" />&amp;followup_matchId=<xsl:value-of select="$matchId" /></xsl:attribute>[Remove]</a><br/>
      </td>
    </tr>
  </xsl:template>


</xsl:stylesheet>

