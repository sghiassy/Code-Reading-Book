<?xml version="1.0" encoding="utf-8"?>

<!--
  Author: Ovidiu Predescu "ovidiu@cup.hp.com"

  With ideas from an early prototype implemented by Pankaj Kumar
  "pankaj_kumar@hp.com"

  Date: July 21, 2001
 -->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:xscript="http://apache.org/xsp/xscript/1.0"
  xmlns:soap="http://apache.org/xsp/soap/3.0"
  >

  <xsl:include href="xscript-lib.xsl"/>

  <xsl:template match="xsp:page">
    <xsp:page>
      <xsl:apply-templates select="@*"/>
      <xsp:structure>
        <xsp:include>org.apache.cocoon.components.language.markup.xsp.SOAPHelper</xsp:include>
      </xsp:structure>
      <xsl:apply-templates/>
    </xsp:page>
  </xsl:template>

  <xsl:template match="soap:call">
    <xsl:variable name="url">
      <xsl:choose>
        <xsl:when test="soap:url"><xsl:value-of select="soap:url"/></xsl:when>
        <xsl:when test="@url">"<xsl:value-of select="@url"/>"</xsl:when>
        <xsl:otherwise>""</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="method">
      <xsl:choose>
        <xsl:when test="soap:method"><xsl:value-of select="soap:method"/></xsl:when>
        <xsl:when test="@method">"<xsl:value-of select="@method"/>"</xsl:when>
        <xsl:otherwise>""</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="scope">
      <xsl:call-template name="xscript-get-scope">
        <xsl:with-param name="scope" select="@scope"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="context">
      <xsl:call-template name="xscript-get-context">
        <xsl:with-param name="scope" select="@scope"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="creation-scope">
      <xsl:call-template name="xscript-get-scope-for-creation">
        <xsl:with-param name="scope" select="@scope"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="creation-context">
      <xsl:call-template name="xscript-get-context-for-creation">
        <xsl:with-param name="scope" select="@scope"/>
      </xsl:call-template>
    </xsl:variable>

    <xscript:variable name="soap:call">
      <xsl:choose>
        <xsl:when test="soap:env">
          <xsl:apply-templates select="soap:env"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="soap-env"/>
        </xsl:otherwise>
      </xsl:choose>
    </xscript:variable>
    <xsl:variable name="object">
      <xsl:call-template name="xscript-get">
        <xsl:with-param name="name" select="'soap:call'"/>
        <xsl:with-param name="scope" select="$scope"/>
        <xsl:with-param name="context" select="$context"/>
        <xsl:with-param name="as" select="'object'"/>
      </xsl:call-template>
    </xsl:variable>
    <xsp:logic>
      getLogger().debug("URL context is " + XSPRequestHelper.getRequestedURL(objectModel));
      getLogger().debug("XScriptObject for soap:call is\n" + <xsl:value-of select="$object"/>);
      try {
        xscriptManager.put("soap:call",
          ((new SOAPHelper(manager,
                           XSPRequestHelper.getRequestedURL(objectModel),
                           <xsl:value-of select="$url"/>,
                           <xsl:value-of select="$method"/>,
                           <xsl:value-of select="$object"/>
                           )).invoke()),
          <xsl:value-of select="$creation-scope"/>,
          <xsl:value-of select="$creation-context"/>);
        getLogger().debug("SOAP result is\n" + <xsl:value-of select="$object"/>);
        <xscript:get name="soap:call"/>
      }
      catch (Exception ex) {
        <soap-err:error xmlns:soap-err="http://apache.org/xsp/soap/3.0"><xsp:expr>ex</xsp:expr></soap-err:error>
        getLogger().error(ex.toString(), ex);
      }
    </xsp:logic>
  </xsl:template>

  <xsl:template match="soap:env" name="soap-env">
    <SOAP-ENV:Envelope
      xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
      >
      <xsl:if test="soap:header">
          <xsl:apply-templates select="soap:header"/>
      </xsl:if>
    <xsl:choose>
      <xsl:when test="soap:body">
          <xsl:apply-templates select="soap:body"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="soap-body"/>
      </xsl:otherwise>
    </xsl:choose>
    </SOAP-ENV:Envelope>
  </xsl:template>

  <xsl:template match="soap:header" name="soap-header">
      <SOAP-ENV:Header
        xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
        >
        <xsl:apply-templates select="*[name() != 'soap:url'
                                       and name() != 'soap:method'
                                       and name() != 'soap:namespace']"/>
      </SOAP-ENV:Header>
  </xsl:template>
  
  <xsl:template match="soap:body" name="soap-body">
    <xsp:logic>
      <xsl:for-each select="soap:namespace">
        // Generate the namespace defined with soap:namespace
        this.contentHandler.startPrefixMapping(
          "<xsl:value-of select="@prefix"/>",
          "<xsl:value-of select="@uri"/>");
      </xsl:for-each>
    </xsp:logic>
    <SOAP-ENV:Body
      xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
      >
      <xsl:apply-templates select="*[name() != 'soap:url'
                                     and name() != 'soap:method'
                                     and name() != 'soap:namespace']"/>
    </SOAP-ENV:Body>
    <xsp:logic>
      <xsl:for-each select="soap:namespace">
        // End the namespace defined with soap:namespace
        this.contentHandler.endPrefixMapping(
          "<xsl:value-of select="@prefix"/>");
      </xsl:for-each>
    </xsp:logic>
  </xsl:template>
  
  <xsl:template match="soap:enc">
      <xsp:attribute name="SOAP-ENV:encodingStyle">http://schemas.xmlsoap.org/soap/encoding/</xsp:attribute>
  </xsl:template>

  <xsl:template match="@*|node()" priority="-1">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
