<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output indent="yes" method="xml" />
<!--
	Usage patterns.
	******************
	1. Key generation mode is used to generate a dictionary file 
	from the existing containing only keys, empty translation nodes
	with lang attribute equal to $new-lang and, optionaly, translation 
	for another language - for convenience.
	Example: $mode = keys, $new-lang=fr, $keep-lang=en
	Result will be a dictionary template for French.
	
	2. Merging translations is useful when you need to add or update 
	translations for a new or existing language. You have to create a 
	translations file with new values and set $new-dict param.
	
	3. To add a new language you will need to generate a template with
	keys, translate them, then merge with the existing dictionary.
-->


<!-- Modes: 
		keys - generates a template with keys for the given language
		merge - adds/updates translations from the given file
-->
<xsl:param name="mode">merge</xsl:param>
<!-- The language to be added or used in keys mode -->
<xsl:param name="new-lang">de</xsl:param>
<!-- Translations for this language will be kept during key generation -->
<xsl:param name="keep-lang"></xsl:param>
<!-- New translations' file name - format is the same as for the dictionary -->
<xsl:param name="new-dict">simple_dict_de.xml</xsl:param>

<xsl:template match="translations">
	<xsl:copy>
		<xsl:choose>
			<xsl:when test="$mode='keys'">
				<xsl:apply-templates mode="keys" />
			</xsl:when>
			<xsl:otherwise>	
				<xsl:apply-templates />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>

<xsl:template match="entry" mode="keys">
	<xsl:copy>
		<xsl:apply-templates select="key | translation[@lang=$keep-lang]" />
		<translation lang="{$new-lang}">[translate]</translation>
	</xsl:copy>
</xsl:template>

<xsl:template match="entry" >
	<xsl:copy>
		<xsl:apply-templates />
		<xsl:if test="not(translation[@lang=$new-lang])">	
			<xsl:variable name="key" select="key/text()" />
			<xsl:variable name="value" select="document($new-dict)/translations/entry[key=$key]/translation[@lang=$new-lang]" />
			<translation lang="{$new-lang}"><xsl:value-of select="normalize-space($value)" /></translation>
		</xsl:if>
	</xsl:copy>
</xsl:template>

<xsl:template match="translation[@lang=$new-lang]" >
	<xsl:variable name="key" select="../key/text()" />
	<xsl:variable name="value" select="document($new-dict)/translations/entry[key=$key]/translation[@lang=$new-lang]" />	
	<xsl:copy>
		<xsl:apply-templates select="@*"/>
		<xsl:value-of select="normalize-space($value)" />
	</xsl:copy>
</xsl:template>

<xsl:template match="@* | * | text() | processing-instruction() | comment()" priority="-1" mode="keys">
	<xsl:copy>
     	<xsl:apply-templates select="@* | * | text() | processing-instruction() | comment()"/>
	</xsl:copy>
</xsl:template>

<xsl:template match="@* | * | text() | processing-instruction() | comment()" priority="-1">
	<xsl:copy>
     	<xsl:apply-templates select="@* | * | text() | processing-instruction() | comment()"/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>

