<!--
    [XSL-XSLT] This stylesheet automatically updated from an IE5-compatible XSL stylesheet to XSLT.
    The following problems which need manual attention may exist in this stylesheet:
    -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!-- [XSL-XSLT] Simulate lack of built-in templates -->
	<xsl:template match="@*|/|node()"/>
	<!-- Match The Root Node -->
	<xsl:template match="/">
		<HTML>
		<BODY>
		<xsl:apply-templates select="*"></xsl:apply-templates>
		</BODY>
		</HTML>
	</xsl:template>
	<!-- Match Everything Else -->
	<xsl:template match="*|@*|text()|node()|comment()|processing-instruction()">
		<xsl:copy>
			<xsl:apply-templates select="*|@*|text()|node()|comment()|processing-instruction()"></xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- Individual Templates -->
	<xsl:template match="moreovernews">
		<table border="0" width="100%">
			<tr>
				<td colspan="3" STYLE="background-color : #B0E0E6; font : x-small Arial, Helvetica, sans-serif;">
					<CENTER>
					<b>Current News from moreover.com</b>
					</CENTER>
				</td>
			</tr>
			<xsl:apply-templates select="article"></xsl:apply-templates>
		</table>
	</xsl:template>
	<xsl:template match="article">
		<tr>
			<xsl:if test="(position() mod 2) = 1">
				<xsl:attribute name="STYLE">background-color : lightgrey; font : x-small Arial, Helvetica, sans-serif;</xsl:attribute>
			</xsl:if>
			<xsl:if test="(position() mod 2) = 0">
				<xsl:attribute name="STYLE">font : x-small Arial, Helvetica, sans-serif;</xsl:attribute>
			</xsl:if>
			<td STYLE="font : bold;" width="50%">
				<a>
					<xsl:attribute name="HREF"><xsl:value-of select="url"></xsl:value-of></xsl:attribute>
					<xsl:attribute name="STYLE">text-decoration : none;</xsl:attribute>
					<xsl:attribute name="TARGET">_blank</xsl:attribute>
					<xsl:value-of select="headline_text"></xsl:value-of>
				</a>
			</td>
			<td STYLE="font-variant : small-caps;">
				<a>
					<xsl:attribute name="HREF"><xsl:value-of select="document_url"></xsl:value-of></xsl:attribute>
					<xsl:attribute name="STYLE">text-decoration : none;</xsl:attribute>
					<xsl:attribute name="TARGET">_blank</xsl:attribute>
					<xsl:value-of select="source"></xsl:value-of>
				</a>
			</td>
			<td>
				<xsl:value-of select="harvest_time"></xsl:value-of>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
