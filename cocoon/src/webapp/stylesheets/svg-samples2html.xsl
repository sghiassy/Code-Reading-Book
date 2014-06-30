<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xlink="http://www.w3.org/1999/xlink">

 <xsl:template match="/">
  <html>
   <head>
    <title>Apache Cocoon 2.0.1</title>
   </head>
   <body bgcolor="#ffffff" link="#0086b2" vlink="#00698c" alink="#743e75">
    <table border="0" cellspacing="2" cellpadding="2" align="center" width="100%">
     <tr>
      <td colspan="3" align="center"><font face="arial,helvetica,sanserif" color="#000000">The Apache Software Foundation is proud to present...</font></td>
     </tr>
     <tr>
      <td width="30%"></td>
      <td width="40%" align="center"><img border="0" src="images/cocoon.gif"/></td>
      <td width="30%" align="center"><font face="arial,helvetica,sanserif" color="#000000"><b>version 2.0.1</b></font></td>
     </tr>
    </table>

    <xsl:apply-templates/>

    <p align="center">
     <font size="-1">
      Copyright &#169; 1999-2002 <a href="http://www.apache.org">The Apache Software Foundation</a>.<br/>
      All rights reserved.
     </font>
    </p>
   </body>
  </html>
 </xsl:template>
 
 <xsl:template name="generate-group-heading">
 	<xsl:param name="position"/>
	<svg:svg width="200" height="15" xmlns:svg="http://www.w3.org/2000/svg">
		<xsl:choose>
			<xsl:when test="$position = 'left'">
				<svg:g style="stroke: black; stroke-width: 2px;">
					<svg:line x1="5" y1="5" x2="8" y2="5"/>
					<svg:line x1="5" y1="5" x2="5" y2="15"/>
				</svg:g>
				<svg:text x="10px" y="10px" style="font-family:sans; font-size:15px; fill: #0086b2; text-anchor:start;">
					<xsl:value-of select="@name"/>
				</svg:text>
			</xsl:when>
			<xsl:otherwise>
				<svg:g style="stroke: black; stroke-width: 2px;">
					<svg:line x1="195" y1="5" x2="192" y2="5"/>
					<svg:line x1="195" y1="5" x2="195" y2="15"/>
				</svg:g>
				<svg:text x="190px" y="10px" style="font-family:sans; font-size:15px; fill: #0086b2; text-anchor:end;">
					<xsl:value-of select="@name"/>
				</svg:text>
			</xsl:otherwise>
		</xsl:choose>
	</svg:svg>
 </xsl:template>
 
 <xsl:template name="generate-item">
 	<xsl:param name="position"/>
	<svg:svg width="200" height="15" xmlns:svg="http://www.w3.org/2000/svg">
		<xsl:choose>
			<xsl:when test="$position = 'left'">
				<svg:g style="stroke: black; stroke-width: 2px;">
					<xsl:choose>
						<xsl:when test="position() = last()">
							<svg:line x1="5" y1="-1" x2="5" y2="5"/>
						</xsl:when>
						<xsl:otherwise>
							<svg:line x1="5" y1="-1" x2="5" y2="15"/>
						</xsl:otherwise>
					</xsl:choose>
					<svg:line x1="5" y1="5" x2="18" y2="5"/>
				</svg:g>
				<svg:text x="20px" y="10px" style="font-family:sans; font-size:15px; fill: black; text-anchor:start;"> 
					<xsl:value-of select="@name"/>
				</svg:text>
			</xsl:when>
			<xsl:otherwise>
				<svg:g style="stroke: black; stroke-width: 2px;">
					<xsl:choose>
						<xsl:when test="position() = last()">
							<svg:line x1="195" y1="-1" x2="195" y2="5"/>
						</xsl:when>
						<xsl:otherwise>
							<svg:line x1="195" y1="-1" x2="195" y2="15"/>
						</xsl:otherwise>
					</xsl:choose>
					<svg:line x1="195" y1="5" x2="182" y2="5"/>
				</svg:g>
				<svg:text x="180px" y="10px" style="font-family:sans; font-size:15px; fill: black; text-anchor:end;"> 
					<xsl:value-of select="@name"/>
				</svg:text>
			</xsl:otherwise>
		</xsl:choose>
	</svg:svg>
 </xsl:template>
 
 <xsl:template name="generate-group">
 	<xsl:param name="position"/>
	<tr>
		<td>
			<xsl:call-template name="generate-group-heading">
				<xsl:with-param name="position" select="$position"/>
			</xsl:call-template>
		</td>
	</tr>
	<xsl:for-each select="sample">
		<tr>
			<td>
				<a href="{@href}">
					<xsl:call-template name="generate-item">
						<xsl:with-param name="position" select="$position"/>
					</xsl:call-template>
				</a>
			</td>
		</tr>
	</xsl:for-each>
 </xsl:template>
 
 <xsl:template match="samples">
 	<xsl:variable name="half" select="round(count(group) div 2)"/>
 	<table cellspacing="0" cellpadding="0" border="0" align="center">
		<tr>
			<td valign="top">
				<table cellspacing="0" cellpadding="0" border="0">
					<xsl:for-each select="group[position() &lt;= $half]">
						<xsl:call-template name="generate-group">
							<xsl:with-param name="position">left</xsl:with-param>
						</xsl:call-template>
					</xsl:for-each>
				</table>
			</td>
			<td valign="top">
				<table cellspacing="0" cellpadding="0" border="0">
					<xsl:for-each select="group[position() &gt; $half]">
						<xsl:call-template name="generate-group">
							<xsl:with-param name="position">right</xsl:with-param>
						</xsl:call-template>
					</xsl:for-each>
				</table>
			</td>
		</tr>
	</table>
 </xsl:template>
 
</xsl:stylesheet>
