<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>
  <xsl:variable name="tablecolor">#d0f0d0</xsl:variable>
  <xsl:variable name="mediumtablecolor">#e0ffe0</xsl:variable>
  <xsl:variable name="lighttablecolor">#f0fff0</xsl:variable>
 
  <xsl:template match="sqltbl">
    <xsl:element name="table"> 
      <xsl:attribute name="border">1</xsl:attribute>
      <xsl:attribute name="align">center</xsl:attribute>
      <xsl:attribute name="bgcolor"><xsl:value-of select="$tablecolor"/></xsl:attribute>
      <xsl:element name="tr"> 
        <xsl:apply-templates select="child::*[1]" mode="head"/>
      </xsl:element> 
      <xsl:for-each select="child::*">
        <xsl:element name="tr"> 
          <xsl:apply-templates select="."/>
        </xsl:element> 
      </xsl:for-each>
    </xsl:element> 
  </xsl:template>

  <xsl:template match="sqltblrow" mode="head">
    <xsl:for-each select="./child::*">
      <xsl:element name="th">
        <xsl:value-of select="name()"/>
      </xsl:element>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="sqltblrow">
    <xsl:for-each select="./child::*">
      <xsl:element name="td">
        <xsl:attribute name="valign">top</xsl:attribute>
        <xsl:apply-templates/>
      </xsl:element>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="sql-set">
     <xsl:element name="table">
        <xsl:attribute name="border">1</xsl:attribute>
        <xsl:attribute name="bgcolor"><xsl:value-of select="$mediumtablecolor"/></xsl:attribute>
	<xsl:for-each select="./child::*">
	   <xsl:element name="tr">
	      <xsl:element name="td">
	         <xsl:apply-templates/>
	      </xsl:element>
	   </xsl:element>
	</xsl:for-each>
     </xsl:element>
  </xsl:template>

  <xsl:template name="sql-set-item">
     <xsl:value-of select="."/>
     <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="sql-list">
     <xsl:element name="table">
        <xsl:attribute name="border">1</xsl:attribute>
        <xsl:attribute name="bgcolor"><xsl:value-of select="$mediumtablecolor"/></xsl:attribute>
	<xsl:variable name="i"><xsl:value-of select="position()"/></xsl:variable>
	<xsl:for-each select="./child::*">
	   <xsl:element name="tr">
	      <xsl:element name="td">
	         <xsl:attribute name="bgcolor"><xsl:value-of select="$lighttablecolor"/></xsl:attribute>
	         <xsl:value-of select="@pos"/>.
	      </xsl:element>
	      <xsl:element name="td">
	         <xsl:apply-templates/>
	      </xsl:element>
	   </xsl:element>
	</xsl:for-each>
     </xsl:element>
  </xsl:template>

  <xsl:template name="sql-list-item">
     <xsl:value-of select="."/>
     <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="sql-row">
     <xsl:element name="table">
        <xsl:attribute name="border">1</xsl:attribute>
        <xsl:attribute name="bgcolor"><xsl:value-of select="$mediumtablecolor"/></xsl:attribute>
	<xsl:element name="tr">
	   <xsl:for-each select="./child::*">
	      <xsl:element name="td">
	         <xsl:apply-templates/>
	      </xsl:element>
	   </xsl:for-each>
	</xsl:element>
     </xsl:element>
  </xsl:template>

  <xsl:template name="sql-row-item">
     <xsl:value-of select="."/>
     <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="@*|node()" priority="-1">
   <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
   </xsl:copy>
  </xsl:template>


  <xsl:template match="@*|node()" priority="-1" mode="head">
   <xsl:copy>
    <xsl:apply-templates select="@*|node()" mode="head"/>
   </xsl:copy>
  </xsl:template>

  <xsl:template match="error">
     <xsl:element name="table">
     <xsl:attribute name="align">center</xsl:attribute>
     <xsl:attribute name="width">100%</xsl:attribute>
     <xsl:attribute name="bgcolor">#ffe0e0</xsl:attribute>
        <xsl:element name="tr">
	   <xsl:element name="td">
	   <xsl:attribute name="align">center</xsl:attribute>
	      <xsl:element name="big">	   
                 <xsl:apply-templates/>
	      </xsl:element>
	   </xsl:element>	
        </xsl:element>
     </xsl:element>
  </xsl:template>

</xsl:stylesheet>
