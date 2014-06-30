<?xml version="1.0"?>
<!--
 *****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * _________________________________________________________________________ *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************
-->

<!--
 * @author <a href="mailto:ricardo@apache.org>Ricardo Rocha</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
-->

<!-- XSP Request logicsheet for the Java language -->

<xsl:stylesheet
  version="1.0"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:xsp-request="http://apache.org/xsp/request/2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >

  <xsl:template match="xsp-request:get-uri">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getURI(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:uri> -->
        <xsp:logic>
          XSPRequestHelper.getURI(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-sitemap-uri">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getSitemapURI(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:sitemap-uri> -->
        <xsp:logic>
          XSPRequestHelper.getSitemapURI(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-session-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="default">
      <xsl:choose>
        <xsl:when test="@default">"<xsl:value-of select="@default"/>"</xsl:when>
        <xsl:when test="xsp-request:default">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="xsp-request:default"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>null</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getSessionAttribute(objectModel, 
          <xsl:copy-of select="$name"/>, <xsl:copy-of select="$default"/>))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:session-attribute name="..."> -->
        <xsp:logic>
          XSPRequestHelper.getSessionAttribute(objectModel, 
          this.contentHandler, 
          <xsl:copy-of select="$name"/>, <xsl:copy-of select="$default"/>);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-session-id">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getSessionId(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:session-id> -->
        <xsp:element name="xsp-request:session-id">
          <xsp:logic>
            (XSPRequestHelper.getSessionId(objectModel))
          </xsp:logic>
        </xsp:element>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-parameter">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="default">
      <xsl:choose>
        <xsl:when test="@default">"<xsl:value-of select="@default"/>"</xsl:when>
        <xsl:when test="xsp-request:default">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="xsp-request:default"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>null</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="form-encoding">
      <xsl:call-template name="value-for-form-encoding"/>
    </xsl:variable>

    <xsl:variable name="container-encoding">
      <xsl:call-template name="value-for-container-encoding"/>
    </xsl:variable>

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getParameter(objectModel, 
          <xsl:copy-of select="$name"/>, <xsl:copy-of select="$default"/>, 
          <xsl:copy-of select="$form-encoding"/>, 
          <xsl:copy-of select="$container-encoding"/>))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:parameter name="..."> -->
        <xsp:logic>
          XSPRequestHelper.getParameter(objectModel, this.contentHandler, 
          <xsl:copy-of select="$name"/>, <xsl:copy-of select="$default"/>, 
          <xsl:copy-of select="$form-encoding"/>, 
          <xsl:copy-of select="$container-encoding"/>);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- emitting output as xml only -->
  <xsl:template match="xsp-request:get-parameter-values">

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'xml'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="form-encoding">
      <xsl:call-template name="value-for-form-encoding"/>
    </xsl:variable>

    <xsl:variable name="container-encoding">
      <xsl:call-template name="value-for-container-encoding"/>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'array'">
        <xsp:expr>
          (XSPRequestHelper.getParameterValues(objectModel, 
          <xsl:copy-of select="$name"/>, 
          <xsl:copy-of select="$form-encoding"/>, 
          <xsl:copy-of select="$container-encoding"/>))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:parameter-values name="..."> -->
        <xsp:logic>
          XSPRequestHelper.getParameterValues(objectModel, this.contentHandler,
          <xsl:copy-of select="$name"/>, 
          <xsl:copy-of select="$form-encoding"/>, 
          <xsl:copy-of select="$container-encoding"/>);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- emitting output as xml only -->
  <xsl:template match="xsp-request:get-parameter-names">
    <xsp:logic>
      XSPRequestHelper.getParameterNames(objectModel, this.contentHandler);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="xsp-request:get-header">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getHeader(objectModel, 
          <xsl:copy-of select="$name"/>))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:header name="..."> -->
        <xsp:logic>
          XSPRequestHelper.getHeader(objectModel, this.contentHandler, 
          <xsl:copy-of select="$name"/>);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- emitting output as xml only -->
  <xsl:template match="xsp-request:get-header-names">
    <xsp:logic>
      XSPRequestHelper.getHeaderNames(objectModel, this.contentHandler);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="xsp-request:get-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          String.valueOf(XSPRequestHelper.getAttribute(objectModel,
          <xsl:copy-of select="$name"/>))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'object'">
        <xsp:expr>
          XSPRequestHelper.getAttribute(objectModel,
          <xsl:copy-of select="$name"/>)
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="xsp-request:attribute">
          <xsp:attribute name="name" value="{$name}"/>
          <xsp:logic>
            XSPRequestHelper.getAttribute(objectModel, 
            <xsl:copy-of select="$name"/>)
          </xsp:logic>
        </xsp:element>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
  
  <!-- emitting output as xml only -->
  <xsl:template match="xsp-request:get-attribute-names">
    <xsp:logic>
      XSPRequestHelper.getAttributeNames(objectModel, this.contentHandler);
    </xsp:logic>
  </xsl:template>
	
  <xsl:template match="xsp-request:remove-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsp:logic>
      XSPRequestHelper.removeAttribute(objectModel,
      <xsl:copy-of select="$name"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="xsp-request:get-requested-url">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>XSPRequestHelper.getRequestedURL(objectModel)</xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="xsp-request:requested-url">
          <xsp:logic>
            XSPRequestHelper.getRequestedURL(objectModel)
          </xsp:logic>
        </xsp:element>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-remote-address">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getRemoteAddr(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:remote-addres> -->
        <xsp:logic>
          XSPRequestHelper.getRemoteAddr(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-remote-user">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getRemoteUser(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:remote-user> -->
        <xsp:logic>
          XSPRequestHelper.getRemoteUser(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-context-path">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getContextPath(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:context-path> -->
        <xsp:logic>
          XSPRequestHelper.getContextPath(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-server-name">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getServerName(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="xsp-request:server-name">
          <xsp:expr>
            (XSPRequestHelper.getServerName(objectModel))
          </xsp:expr>
        </xsp:element>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-server-port">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getServerPort(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="xsp-request:server-port">
          <xsp:expr>
            (XSPRequestHelper.getServerPort(objectModel))
          </xsp:expr>
        </xsp:element>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-method">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getMethod(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:method> -->
        <xsp:logic>
          XSPRequestHelper.getMethod(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-query-string">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getQueryString(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:query-string> -->
        <xsp:logic>
          XSPRequestHelper.getQueryString(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-protocol">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getProtocol(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:protocol> -->
        <xsp:logic>
          XSPRequestHelper.getProtocol(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xsp-request:get-remote-host">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          (XSPRequestHelper.getRemoteHost(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'xml'">
        <!-- <xsp-request:remote-host> -->
        <xsp:logic>
          XSPRequestHelper.getRemoteHost(objectModel, this.contentHandler);
        </xsp:logic>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="value-for-as">
    <xsl:param name="default"/>
    <xsl:choose>
      <xsl:when test="@as"><xsl:value-of select="@as"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="$default"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="value-for-name">
    <xsl:choose>
      <xsl:when test="@name">"<xsl:value-of select="@name"/>"</xsl:when>
      <xsl:when test="xsp-request:name">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content" select="xsp-request:name"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>null</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="value-for-form-encoding">
    <xsl:choose>
      <xsl:when test="@form-encoding">"<xsl:value-of 
          select="@form-encoding"/>"</xsl:when>
      <xsl:when test="xsp-request:form-encoding">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content" select="xsp-request:form-encoding"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>null</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="value-for-container-encoding">
    <xsl:choose>
      <xsl:when test="@container-encoding">"<xsl:value-of 
          select="@container-encoding"/>"</xsl:when>
      <xsl:when test="xsp-request:container-encoding">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content" 
            select="xsp-request:container-encoding"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>null</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="get-nested-content">
    <xsl:param name="content"/>
    <xsl:choose>
      <xsl:when test="$content/*">
        <xsl:apply-templates select="$content/*"/>
      </xsl:when>
      <xsl:otherwise>"<xsl:value-of select="$content"/>"</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*|*|text()|processing-instruction()">
    <xsl:copy>
      <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="xsp-request:name"/>
  <xsl:template match="xsp-request:default"/>
  <xsl:template match="xsp-request:form-encoding"/>
  <xsl:template match="xsp-request:container-encoding"/>

</xsl:stylesheet>
