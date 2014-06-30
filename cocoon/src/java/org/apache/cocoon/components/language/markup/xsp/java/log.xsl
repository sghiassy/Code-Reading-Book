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
 * @author <a href="mailto:bloritsch@apache.org>Berin Loritsch</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:11 $
-->

<!-- XSP Response logicsheet for the Java language -->
<xsl:stylesheet
  version="1.0"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:log="http://apache.org/xsp/log/2.0"

  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>

  <xsl:template match="log:logger">
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="@name">"<xsl:value-of select="@name"/>"</xsl:when>
        <xsl:when test="name">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="log:name"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>""</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

<!-- Files are now under control of LogKitManager
    <xsl:variable name="filename">
      <xsl:choose>
        <xsl:when test="@filename">"<xsl:value-of select="@filename"/>"</xsl:when>
        <xsl:when test="filename">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="log:filename"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>""</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
-->

    <xsl:variable name="level">
      <xsl:choose>
        <xsl:when test="@level">"<xsl:value-of select="@level"/>"</xsl:when>
        <xsl:when test="level">
          <xsl:call-template name="get-nested-content">
            <xsl:with-param name="content" select="log:level"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>"DEBUG"</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsp:logic>
      if (getLogger() == null) {
          try {
            String category = <xsl:value-of select="$name"/>;
            org.apache.log.Logger logger = org.apache.log.Hierarchy.getDefaultHierarchy().getLoggerFor("cocoon" + (("".equals(category))? category : "." + category));
<!--
            if (!"".equals(<xsl:value-of select="$filename"/>)) {
                String file = this.avalonContext.get(org.apache.cocoon.Constants.CONTEXT_LOG_DIR) + <xsl:value-of select="$filename"/>;
                org.apache.log.LogTarget[] targets = new org.apache.log.LogTarget[] {
                    new org.apache.log.output.FileOutputLogTarget(file)
                };
                logger.setLogTargets(targets);
            }
-->
            logger.setPriority(org.apache.log.Priority.getPriorityForName(<xsl:value-of select="$level"/>));
            this.setLogger(logger);
          } catch (Exception e) {
            getLogger().error("Could not create logger for \"" +
                               <xsl:value-of select="$name"/> + "\".", e);
          }
      }
    </xsp:logic>
  </xsl:template>

  <xsl:template match="log:debug">
    <xsp:logic>
      if(getLogger() != null)
        getLogger().debug(<xsl:call-template name="get-log-message"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="log:info">
    <xsp:logic>
      if(getLogger() != null)
        getLogger().info(<xsl:call-template name="get-log-message"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="log:warn">
    <xsp:logic>
      if(getLogger() != null)
        getLogger().warn(<xsl:call-template name="get-log-message"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="log:error">
    <xsp:logic>
      if(getLogger() != null)
        getLogger().error(<xsl:call-template name="get-log-message"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template match="log:fatal-error">
    <xsp:logic>
      if(getLogger() != null)
        getLogger().fatalError(<xsl:call-template name="get-log-message"/>);
    </xsp:logic>
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

  <xsl:template name="get-log-message">
    <xsl:for-each select="./child::node()">
      <xsl:choose>
        <xsl:when test="xsp:expr"><xsl:apply-templates select="node()"/></xsl:when>
        <xsl:otherwise>"<xsl:value-of select="."/>"</xsl:otherwise>
      </xsl:choose>
      <xsl:if test="not(position() = last())"> + </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="@*|*|text()|processing-instruction()">
    <xsl:copy>
      <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
