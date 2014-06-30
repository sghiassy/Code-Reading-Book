<?xml version="1.0" encoding="ISO-8859-1"?>
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
  A collection of utility templates to help logicsheet writers, and ensure
  a consistent behaviour of logicsheets using them.

  To use them, just add &lt;xsl:include href="logicsheet-util.xsl"/&gt; at the
  top of your logicsheet.
  <br/>
  Note : some templates rely on the <code>$namespace-uri</code> variable which must
  be set in the including logicsheet to its namespace URI.

  @author <a href="mailto:sylvain@apache.org">Sylvain Wallez</a>
  @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:11 $
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--
  Namespace prefix for this logicsheet.
-->
<xsl:param name="namespace-prefix">
  <xsl:call-template name="get-namespace-prefix"/>
</xsl:param>

<!--
  Find the namespace prefix used in the source document for a given namespace URI.

  @param uri the namespace URI (default value = $namespace-uri)
  @return the namespace prefix
-->
<xsl:template name="get-namespace-prefix">
  <xsl:param name="uri"><xsl:value-of select="$namespace-uri"/></xsl:param>
  <!-- Search it on the root element -->
  <xsl:variable name="ns-attr" select="/*/namespace::*[string(.) = $uri]"/>
  <xsl:choose>
    <xsl:when test="$ns-attr"><xsl:value-of select="local-name($ns-attr)"/></xsl:when>
    <xsl:otherwise>
      <!-- Examine all nodes (requires the XSL processor to traverse the DOM) -->
      <xsl:variable name="ns-attr2" select="//*/namespace::*[string(.) = $uri]"/>
      <xsl:choose>
        <xsl:when test="$ns-attr2"><xsl:value-of select="local-name($ns-attr2)"/></xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="error">
            <xsl:with-param name="message">Cannot find namespace prefix for <xsl:value-of select="$uri"/></xsl:with-param>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!--
  Break on error.
  @param message explanation of the error
-->
<xsl:template name="error">
  <xsl:param name="message"/>
  <xsl:message terminate="yes"><xsl:value-of select="$message"/></xsl:message>
</xsl:template>

<!--
  Ignore $namespace-prefix:param elements used in get-parameter and get-string-parameter
-->
<xsl:template match="*[namespace-uri(.) = $namespace-uri and local-name(.) = 'param']" priority="-1"/>

<!--
  Break on error on all elements belonging to this logicheet's namespace not
  handled by a template.
-->
<xsl:template match="*[namespace-uri(.) = $namespace-uri and local-name(.) != 'param']" priority="-1">
  <xsl:call-template name="error">
    <xsl:with-param name="message">Unknown logicsheet tag : <xsl:value-of select="name(.)"/></xsl:with-param>
  </xsl:call-template>
</xsl:template>

<!--
  Copy all nodes that were not handled by a dedicated template.
-->
<xsl:template match="@*|node()" priority="-2">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

<!--
  Get the value of a logicsheet tag parameter as a String (borrowed from ESQL).

  @name the name of the parameter
  @default the default value (String expression)
  @required true if the parameter is required
-->
  <xsl:template name="get-string-parameter">
    <xsl:param name="name"/>
    <xsl:param name="default"/>
    <xsl:param name="required">false</xsl:param>

    <xsl:variable name="qname">
      <xsl:value-of select="concat($namespace-prefix, ':param')"/>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="@*[name(.) = $name]">"<xsl:value-of select="@*[name(.) = $name]"/>"</xsl:when>
      <xsl:when test="(*[name(.) = $qname])[@name = $name]">
        <xsl:call-template name="get-nested-string">
          <xsl:with-param name="content" select="(*[name(.) = $qname])[@name = $name]"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="string-length($default) = 0">
            <xsl:choose>
              <xsl:when test="$required = 'true'">
                <xsl:call-template name="error">
                  <xsl:with-param name="message">[Logicsheet processor]
Parameter '<xsl:value-of select="$name"/>' missing in dynamic tag &lt;<xsl:value-of select="name(.)"/>&gt;
                  </xsl:with-param>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>""</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise><xsl:copy-of select="$default"/></xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="get-nested-string">
    <xsl:param name="content"/>
      <xsl:choose>
        <!-- if $content has sub-elements, concatenate them -->
        <xsl:when test="$content/*">
          ""
          <xsl:for-each select="$content/node()">
            <xsl:choose>
              <xsl:when test="name(.)">
                <!-- element -->
                <xsl:choose>
                  <xsl:when test="namespace-uri(.)='http://apache.org/xsp' and local-name(.)='text'">
                    <!-- xsp:text element -->
                    + "<xsl:value-of select="."/>"
                  </xsl:when>
                  <xsl:otherwise>
                    <!-- other elements -->
                    + <xsl:apply-templates select="."/>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:when>
              <xsl:otherwise>
                <!-- text node -->
                + "<xsl:value-of select="translate(.,'&#9;&#10;&#13;','   ')"/>"
              </xsl:otherwise>
            </xsl:choose>
          </xsl:for-each>
        </xsl:when>
        <!-- else return the text value of $content -->
        <xsl:otherwise>"<xsl:value-of select="normalize-space($content)"/>"</xsl:otherwise>
      </xsl:choose>
  </xsl:template>

<!--
  Get the value of a logicsheet tag parameter either as a primitive value or an object.
  @name the name of the parameter
  @default the default value
  @required true if the parameter is required
-->
  <xsl:template name="get-parameter">
    <xsl:param name="name"/>
    <xsl:param name="default"/>
    <xsl:param name="required">false</xsl:param>

    <xsl:variable name="qname">
      <xsl:value-of select="concat($namespace-prefix, ':param')"/>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="@*[name(.) = $name]"><xsl:value-of select="@*[name(.) = $name]"/></xsl:when>
      <xsl:when test="(*[name(.) = $qname])[@name = $name]">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content" select="(*[name(.) = $qname])[@name = $name]"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="string-length($default) = 0">
            <xsl:choose>
              <xsl:when test="$required = 'true'">
                <xsl:call-template name="error">
                  <xsl:with-param name="message">[Logicsheet processor]
Parameter '<xsl:value-of select="$name"/>' missing in dynamic tag &lt;<xsl:value-of select="name(.)"/>&gt;
                  </xsl:with-param>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>null</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise><xsl:copy-of select="$default"/></xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="get-nested-content">
    <xsl:param name="content"/>
    <xsl:choose>
      <xsl:when test="$content/*">
        <xsl:apply-templates select="$content/*"/>
      </xsl:when>
      <xsl:otherwise><xsl:value-of select="$content"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
