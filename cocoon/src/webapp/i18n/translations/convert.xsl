<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output indent="yes" method="xml"/>

<!-- specify here the language of the catalogue you would like to create -->
<xsl:param name="lang">hy</xsl:param>

<xsl:template match="translations">
	<catalogue xml:lang="{$lang}">
		<xsl:apply-templates select="entry"/>
	</catalogue>
</xsl:template>

<xsl:template match="entry">
	<message key="{key}">
		<xsl:value-of select="translation[@lang=$lang]"/>
	</message>
</xsl:template>

</xsl:stylesheet>
