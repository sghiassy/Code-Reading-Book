<?xml version="1.0"?>
<!--

 ============================================================================
                   The Apache Software License, Version 1.1
 ============================================================================

 Copyright (C) 1999-2002 The Apache Software Foundation. All rights reserved.

 Redistribution and use in source and binary forms, with or without modifica-
 tion, are permitted provided that the following conditions are met:

 1. Redistributions of  source code must  retain the above copyright  notice,
    this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. The end-user documentation included with the redistribution, if any, must
    include  the following  acknowledgment:  "This product includes  software
    developed  by the  Apache Software Foundation  (http://www.apache.org/)."
    Alternately, this  acknowledgment may  appear in the software itself,  if
    and wherever such third-party acknowledgments normally appear.

 4. The names "Cocoon" and  "Apache Software Foundation"  must not be used to
    endorse  or promote  products derived  from this  software without  prior
    written permission. For written permission, please contact
    apache@apache.org.

 5. Products  derived from this software may not  be called "Apache", nor may
    "Apache" appear  in their name,  without prior written permission  of the
    Apache Software Foundation.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
 APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
 DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
 ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
 (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 This software  consists of voluntary contributions made  by many individuals
 on  behalf of the Apache Software  Foundation and was  originally created by
 Stefano Mazzocchi  <stefano@apache.org>. For more  information on the Apache
 Software Foundation, please see <http://www.apache.org/>.

-->

<!-- written by Ricardo Rocha "ricardo@apache.org" -->
<!-- ported by Berin Loritsch "bloritsch@apache.org" -->


<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:session="http://apache.org/xsp/session/2.0"
>
  <!-- *** ServletSession Templates *** -->

  <xsl:template match="session:get-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'object'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="default">
      <xsl:call-template name="value-for-default">
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="session:attribute">
          <xsp:expr>
            XSPRequestHelper.getSessionAttribute(objectModel,
            String.valueOf(<xsl:copy-of select="$name"/>),
            <xsl:copy-of select="$default"/>)
          </xsp:expr>
        </xsp:element>
      </xsl:when>
      <xsl:when test="$as = 'object'">
        <xsp:expr>
          XSPRequestHelper.getSessionAttribute(objectModel,
          String.valueOf(<xsl:copy-of select="$name"/>),
          <xsl:copy-of select="$default"/>)
        </xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:get-attribute-names">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'array'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:logic>
          List v = XSPRequestHelper.getSessionAttributeNames(objectModel);
        </xsp:logic>

        <xsp:element name="session:attribute-names">
          <xsp:logic>
            for (int i = 0; i &lt; v.size(); i++) {
            <xsp:element name="session:attribute-name">
              <xsp:expr>v.get(i)</xsp:expr>
            </xsp:element>
            }
          </xsp:logic>
        </xsp:element>
      </xsl:when>

      <xsl:when test="$as = 'array'">
        <xsp:expr>
          XSPRequestHelper.getSessionAttributeNames(objectModel)
        </xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:get-creation-time">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'long'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="session:creation-time">
          <xsp:expr>
            new Date(XSPRequestHelper.getSessionCreationTime(objectModel))
          </xsp:expr>
        </xsp:element>
      </xsl:when>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          new Date(XSPRequestHelper.getSessionCreationTime(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'long'">
        <xsp:expr>
          XSPRequestHelper.getSessionCreationTime(objectModel)
        </xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:get-id">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'string'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="session:id">
          <xsp:expr>XSPRequestHelper.getSessionId(objectModel)</xsp:expr>
        </xsp:element>
      </xsl:when>
      <xsl:when test="$as = 'string'">
        <xsp:expr>XSPRequestHelper.getSessionId(objectModel)</xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:get-last-accessed-time">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'long'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="session:last-accessed-time">
          <xsp:expr>
            new Date(XSPRequestHelper.getSessionLastAccessedTime(objectModel))
          </xsp:expr>
        </xsp:element>
      </xsl:when>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          new Date(XSPRequestHelper.getSessionLastAccessedTime(objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'long'">
        <xsp:expr>
          XSPRequestHelper.getSessionLastAccessedTime(objectModel)
        </xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:get-max-inactive-interval">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'int'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="$as = 'xml'">
        <xsp:element name="session:max-inactive-interval">
          <xsp:expr>
            XSPRequestHelper.getSessionMaxInactiveInterval(objectModel)
          </xsp:expr>
        </xsp:element>
      </xsl:when>
      <xsl:when test="$as = 'string'">
        <xsp:expr>
          String.valueOf(XSPRequestHelper.getSessionMaxInactiveInterval(
          objectModel))
        </xsp:expr>
      </xsl:when>
      <xsl:when test="$as = 'int'">
        <xsp:expr>
          XSPRequestHelper.getSessionMaxInactiveInterval(objectModel)
        </xsp:expr>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="session:invalidate">
    <xsp:logic>
      XSPRequestHelper.invalidateSession(objectModel);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="session:is-new">
    <xsl:variable name="as">
      <xsl:call-template name="value-for-as">
        <xsl:with-param name="default" select="'boolean'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsp:expr>
      <xsl:choose>
        <xsl:when test="$as = 'xml'">
          <xsp:element name="session:is-new">
            <xsp:expr>XSPRequestHelper.isSessionNew(objectModel)</xsp:expr>
          </xsp:element>
        </xsl:when>
        <xsl:when test="$as = 'string'">
          <xsp:expr>
            String.valueOf(XSPRequestHelper.isSessionNew(objectModel))
          </xsp:expr>
        </xsl:when>
        <xsl:when test="$as = 'boolean'">
          <xsp:expr>XSPRequestHelper.isSessionNew(objectModel)</xsp:expr>
        </xsl:when>
      </xsl:choose>
    </xsp:expr>
  </xsl:template>

  <xsl:template match="session:remove-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsp:logic>
      XSPRequestHelper.removeSessionAttribute(objectModel,
      String.valueOf(<xsl:copy-of select="$name"/>)
      );
    </xsp:logic>
  </xsl:template>

  <xsl:template match="session:set-attribute">
    <xsl:variable name="name">
      <xsl:call-template name="value-for-name"/>
    </xsl:variable>

    <xsl:variable name="content">
      <xsl:call-template name="get-nested-content">
        <xsl:with-param name="content" select="*|text()"/>
      </xsl:call-template>
    </xsl:variable>

    <xsp:logic>
      XSPRequestHelper.setSessionAttribute(objectModel,
      String.valueOf(<xsl:copy-of select="$name"/>),
      <xsl:copy-of select="$content"/>
      );
    </xsp:logic>
  </xsl:template>

  <xsl:template match="session:set-max-inactive-interval">
    <xsl:variable name="interval">
      <xsl:choose>
        <xsl:when test="@interval">"<xsl:value-of 
            select="@interval"/>"</xsl:when>
        <xsl:when test="session:interval">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="session:interval"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>

    <xsp:logic>
      request.getSession().setMaxInactiveInterval(
      Integer.parseInt(
      String.valueOf(<xsl:copy-of select="$interval"/>)
      ));
    </xsp:logic>
  </xsl:template>

  <!-- encode an URL with the session ID -->
  <xsl:template match="session:encode-url">
    <xsl:variable name="href">"<xsl:value-of 
        select="@href"/>"</xsl:variable>

    <xsp:element name="a">
      <xsp:attribute name="href">
        <xsp:expr>
          XSPResponseHelper.encodeURL(objectModel, 
          String.valueOf(<xsl:copy-of select="$href"/>))
        </xsp:expr>
      </xsp:attribute>
      <xsl:value-of select="."/>
    </xsp:element>
  </xsl:template>

  <!-- encode an URL with the session ID as a form-->
  <xsl:template match="session:form-encode-url">
    <xsl:variable name="action">"<xsl:value-of 
        select="@action"/>"</xsl:variable>
    <xsl:variable name="method">"<xsl:value-of 
        select="@method"/>"</xsl:variable>
    <xsl:variable name="onsubmit">"<xsl:value-of 
        select="@onsubmit"/>"</xsl:variable>

    <xsp:element name="form">
      <xsp:attribute name="action">
        <xsp:expr>
          XSPResponseHelper.encodeURL(objectModel,
          String.valueOf(<xsl:copy-of select="$action"/>))
        </xsp:expr>
      </xsp:attribute>
      <xsp:attribute name="method">
        <xsp:expr><xsl:copy-of select="$method"/></xsp:expr>
      </xsp:attribute>
      <xsp:attribute name="onsubmit">
        <xsp:expr><xsl:copy-of select="$onsubmit"/></xsp:expr>
      </xsp:attribute>
      <xsl:apply-templates/>
    </xsp:element>
  </xsl:template>

  <xsl:template name="value-for-name">
    <xsl:choose>
      <xsl:when test="@name">"<xsl:value-of select="@name"/>"</xsl:when>
      <xsl:when test="session:name">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content" select="session:name"/>
        </xsl:call-template>
      </xsl:when>
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

  <xsl:template name="value-for-as">
    <xsl:param name="default"/>
    <xsl:choose>
      <xsl:when test="@as"><xsl:value-of select="@as"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="$default"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="value-for-default">
    <xsl:choose>
      <xsl:when test="@default">"<xsl:value-of select="@default"/>"</xsl:when>
      <xsl:otherwise>""</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*|*|text()|processing-instruction()">
    <xsl:copy>
      <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
