<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template match="root">
		<html>
			<head>
				<title>
					<xsl:value-of select="title"/>
				</title>
			</head>
			<body bgcolor="white" style="font-family: Verdana, Arial, Helvetica">
				<h2>
					<font color="navy">
						<xsl:value-of select="title"/>
					</font>
				</h2>
				<h5><xsl:value-of select="sub-title"/></h5>
				<table width="100%">
					<tr>
						<td align="left">
							<xsl:apply-templates select="menu[1]"/>
						</td>
						<td align="right">
							<xsl:apply-templates select="menu[2]"/>
						</td>						
					</tr>
				</table>
				<hr align="left" noshade="noshade" size="1"/>
				<small><font color="red"><i><xsl:apply-templates select="annotation"/></i></font></small>
				
				<xsl:apply-templates select="content" />
				
				<hr align="left" noshade="noshade" size="1"/> 
				<xsl:apply-templates select="bottom"/>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="menu">
		<font size="-1">
			<xsl:for-each select="item">
				<xsl:apply-templates select="."/>
				<xsl:if test="position() != last()"><xsl:text> | </xsl:text></xsl:if>
			</xsl:for-each>
		</font>	
	</xsl:template>
	
	<xsl:template match="link">
		<a href="{href}"><xsl:value-of select="normalize-space(title)"/></a>
	</xsl:template>
	
	<xsl:template match="item/title">
		<font color="maroon"><xsl:copy-of select="normalize-space(.)"/></font>
	</xsl:template>
	
	<xsl:template match="content">
		<xsl:apply-templates />
	</xsl:template>
	
	<xsl:template match="para">
		<p>
			<font color="navy"><b><xsl:value-of select="position() div 2"/>. <xsl:value-of select="@name"/> </b>: <xsl:value-of select="@title"/></font><br/>
			<font size="-1"><xsl:value-of select="."/></font>
		</p>	
	</xsl:template>
	
	<xsl:template match="bottom">
		<small><b><xsl:value-of select="copyright"/></b></small>		
	</xsl:template>
	
</xsl:stylesheet>
