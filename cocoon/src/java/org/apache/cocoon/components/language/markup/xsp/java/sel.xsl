<?xml version="1.0"?>
<!--

 ============================================================================
                   The Apache Software License, Version 1.2
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

<!--
 * @author <a href="mailto:tcurdt@dff.st">Torsten Curdt</a>
-->

<xsl:stylesheet version="1.0"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:xsp="http://apache.org/xsp"
        xmlns:sel="http://apache.org/xsp/sel/1.0"
        exclude-result-prefixes="sel"
>

<xsl:template match="xsp:page">
  <xsp:page>
    <xsl:copy-of select="@*"/>
    
    <!-- create an output method for every subpage so that 64K limit doesn't strike back -->
    <xsl:for-each select=".//sel:subpage|.//sel:default-subpage">
      <xsp:logic>
      private void _<xsl:value-of select="translate(parent::sel:subpage-set/@parameter,' -','__')"/>_<xsl:value-of select="translate(@name,' -','__')"/>_case() throws SAXException {
        AttributesImpl xspAttr = new AttributesImpl();
        <xsl:apply-templates/>
      }
      </xsp:logic>
    </xsl:for-each>

    <xsl:apply-templates select="*[not(name()='sel:subpage' or name()='sel:default-subpage')]"/>

  </xsp:page>
</xsl:template>

<!-- choice of subpages. select page to display with sitemap parameter. -->
<xsl:template match="sel:subpage-set">
  <xsp:logic>
    String selection = parameters.getParameter("<xsl:value-of select="translate(@parameter,' -','__')"/>", null);
  </xsp:logic>
  
  <!-- display named subpages -->
  <xsl:for-each select="sel:subpage">
    <xsp:logic>
      <xsl:if test="position() != 1"> else </xsl:if>
      if ("<xsl:value-of select="@name"/>".equals(selection)){
        _<xsl:value-of select="translate(parent::sel:subpage-set/@parameter,' -','__')"/>_<xsl:value-of select="translate(@name,' -','__')"/>_case();
      }
    </xsp:logic>
  </xsl:for-each>

  <!-- display default page -->
  <xsl:for-each select="sel:default-subpage">
    <xsp:logic>
      else {
        _<xsl:value-of select="translate(parent::sel:subpage-set/@parameter,' -','__')"/>_<xsl:value-of select="translate(@name,' -','__')"/>_case();
      }
    </xsp:logic>
  </xsl:for-each>

</xsl:template>

<!--##################################################################################-->

<xsl:template match="@*|node()" priority="-2"><xsl:copy><xsl:apply-templates select="@*|node()"/></xsl:copy></xsl:template>
<xsl:template match="text()" priority="-1"><xsl:value-of select="."/></xsl:template>

</xsl:stylesheet>
