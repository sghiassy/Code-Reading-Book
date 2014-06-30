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
<!-- modified by Berin Loritsch "bloritsch@apache.org" -->
<!--        BL: Modified to work with Cocoon 2         -->


<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:util="http://apache.org/xsp/util/2.0"
>
  <xsl:template match="xsp:page">
    <xsp:page>
      <xsl:apply-templates select="@*"/>

      <xsp:structure>
        <xsp:include>java.net.URL</xsp:include>
        <xsp:include>java.util.Date</xsp:include>
        <xsp:include>java.io.FileReader</xsp:include>
        <xsp:include>java.text.SimpleDateFormat</xsp:include>
        <xsp:include>org.apache.cocoon.components.language.markup.xsp.XSPUtil</xsp:include>
      </xsp:structure>

      <xsl:if test="util:cacheable">
       <xsp:logic>
        public boolean hasContentChanged( org.apache.cocoon.environment.Request request ) {
          return false;
        }
       </xsp:logic>
      </xsl:if>

      <xsl:apply-templates/>
    </xsp:page>
  </xsl:template>

  <xsl:template match="util:cacheable"/>

  <!-- Multiline string (based on code by JQ)  - DOESN'T WORK YET!
  <xsl:template match="util:string">
    <xsl:choose>
      <xsl:when test="contains(., '
') or contains(.,
'
')">
        <xsl:call-template name="get-nested-strings">
          <xsl:with-param name="content" select="."/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>"<xsl:value-of select="."/>"</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="get-nested-strings">
    <xsl:param name="content"/>
      <xsl:variable name="first" select="substring-before($content, '
')"/>
      <xsl:variable name="rest" select="substring-after($content, '
')"/>
      <xsl:text>"</xsl:text><xsl:value-of select="$first"/><xsl:text>"</xsl:text>
      <xsl:if test="$rest">
        <xsl:text> + "\n" +
</xsl:text>
        <xsl:call-template name="get-nested-strings">
          <xsl:with-param name="content" select="$rest"/>
        </xsl:call-template>
      </xsl:if>
  </xsl:template>
  -->

  <!-- Include URL contents as SAX -->
  <xsl:template match="util:include-uri">
    <xsl:variable name="href">
      <xsl:choose>
        <xsl:when test="@href">"<xsl:value-of select="@href"/>"</xsl:when>
        <xsl:when test="util:href">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="util:href"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>

    <xsp:logic> {
      String __name = String.valueOf(<xsl:copy-of select="$href"/>);
      {
          org.apache.cocoon.components.parser.Parser newParser = null;
          org.apache.cocoon.components.url.URLFactory factory = null;

          try {
              newParser = (org.apache.cocoon.components.parser.Parser) this.manager.lookup(org.apache.cocoon.components.parser.Parser.ROLE);
              factory = (org.apache.cocoon.components.url.URLFactory) this.manager.lookup(org.apache.cocoon.components.url.URLFactory.ROLE);
              URL __url = factory.getURL(__name);
              InputSource __is = new InputSource(__url.openStream());
              __is.setSystemId(__url.toString());

              XSPUtil.include(__is, this.contentHandler, newParser);
          } catch (Exception e) {
              getLogger().error("Could not include page", e);
          } finally {
              if (factory != null) this.manager.release((Component) factory);
              if (newParser != null) this.manager.release((Component) newParser);
          }
      }
    } </xsp:logic>
  </xsl:template>

   <!-- Include file contents as SAX -->
   <xsl:template match="util:include-file">
     <xsl:variable name="name">
       <xsl:choose>
         <xsl:when test="@name">"<xsl:value-of select="@name"/>"</xsl:when>
         <xsl:when test="util:name">
           <xsl:call-template name="get-nested-content">
             <xsl:with-param name="content" select="util:name"/>
           </xsl:call-template>
         </xsl:when>
       </xsl:choose>
     </xsl:variable>
     <xsp:logic>
       {
          org.apache.cocoon.components.parser.Parser newParser = null;

          try {
              newParser = (org.apache.cocoon.components.parser.Parser) this.manager.lookup(org.apache.cocoon.components.parser.Parser.ROLE);
              InputSource __is = new InputSource(
                      new FileReader(
                         XSPUtil.relativeFilename(
                           <xsl:copy-of select="$name"/>,
                           objectModel
                        )
                      )
                  );


              XSPUtil.include(__is, this.contentHandler, newParser);
            } catch (Exception e) {
              getLogger().error("Could not include page", e);
            } finally {
              if (newParser != null) this.manager.release((Component) newParser);
            }
       }
     </xsp:logic>
   </xsl:template>

  <!-- Include expression as SAX -->
  <xsl:template match="util:include-expr">
    <xsl:variable name="expr">
      <xsl:choose>
        <xsl:when test="@expr">"<xsl:value-of select="@expr"/>"</xsl:when>
        <xsl:when test="util:expr">
          <xsl:apply-templates select="util:expr/*|util:expr/text()"/>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>

    <xsp:logic>
      {
          org.apache.cocoon.components.parser.Parser newParser = null;

          try {
              newParser = (org.apache.cocoon.components.parser.Parser) this.manager.lookup(org.apache.cocoon.components.parser.Parser.ROLE);
              InputSource __is = new InputSource(
                      new StringReader(
                          String.valueOf(<xsl:copy-of select="$expr"/>)
                      )
                  );


              XSPUtil.include(__is, this.contentHandler, newParser);
          } catch (Exception e) {
              getLogger().error("Could not include page", e);
          } finally {
              if (newParser != null) this.manager.release((Component) newParser);
          }
      }
    </xsp:logic>
  </xsl:template>

  <!-- Include file contents as text -->
  <xsl:template match="util:get-file-contents">
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="@name">"<xsl:value-of select="@name"/>"</xsl:when>
        <xsl:when test="util:name">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="util:name"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="@encoding">
	    <xsp:expr>
	      XSPUtil.getFileContents(
		    XSPUtil.relativeFilename(
		          String.valueOf(<xsl:copy-of select="$name"/>),
		      this.objectModel
		    ), "<xsl:value-of select="@encoding"/>"
		      )
	    </xsp:expr>
	  </xsl:when>
      <xsl:otherwise>
	    <xsp:expr>
	      XSPUtil.getFileContents(
		    XSPUtil.relativeFilename(
		          String.valueOf(<xsl:copy-of select="$name"/>),
		      this.objectModel
		    ))
	    </xsp:expr>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!-- Counters -->
  <xsl:template match="util:counter">
    <xsl:choose>
      <xsl:when test="@scope = 'session'">
        <xsp:expr>XSPUtil.getSessionCount(this.session)</xsp:expr>
      </xsl:when>
      <xsl:otherwise>
        <xsp:expr>XSPUtil.getCount()</xsp:expr>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Date -->
  <xsl:template match="util:time">
    <xsl:variable name="format">
      <xsl:choose>
        <xsl:when test="@format">"<xsl:value-of select="@format"/>"</xsl:when>
        <xsl:when test="util:format">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="util:format"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>

    <xsp:expr>
      XSPUtil.formatDate(
        new Date(),
    String.valueOf(<xsl:copy-of select="$format"/>).trim()
      )
    </xsp:expr>
  </xsl:template>

  <!-- Standard Templates -->
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

</xsl:stylesheet>
