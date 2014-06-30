<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:param name="section"/>
  <xsl:param name="slide"/>

  <xsl:template match="slides">

   <slide style="{@style}">
     <xsl:if test="not($section)">
       <title>
        <xsl:value-of select="title"/>
       </title>
       <navigation>
        <next href="?section=1"/>
       </navigation>
       <layout>
        <title><a href="?section=1"><xsl:value-of select="title"/></a></title>
        <subtitle>
         <xsl:for-each select="authors/person">
          <xsl:value-of select="name"/>
           <xsl:text> (</xsl:text>
           <xsl:value-of select="email"/>
           <xsl:text>)</xsl:text>
          </xsl:for-each>
        </subtitle>
        <quote><xsl:value-of select="overview"/></quote>
       </layout>
     </xsl:if>

     <xsl:if test="$section">
      <xsl:if test="not($slide)">
       <xsl:apply-templates select="//section[position() = $section]"/>
      </xsl:if>
      <xsl:if test="$slide">
       <xsl:apply-templates select="//section[position() = $section]/slide[position() = $slide]"/>
      </xsl:if>
     </xsl:if>
   </slide>
  </xsl:template>

<!-- ================================ Section ============================== -->

  <xsl:template match="section">
    <title>
     <xsl:value-of select="title"/>
    </title>

    <navigation>
     <xsl:variable name="last">
      <xsl:value-of select="count(//section)"/>
     </xsl:variable>
     <xsl:variable name="previous-last">
      <xsl:value-of select="count(//section[position() = ($section - 1)]/slide)"/>
     </xsl:variable>

     <home href=""/>

     <xsl:if test="$section!=1">
      <previous-section href="?section={$section - 1}"/>
      <previous href="?section={$section - 1}&amp;slide={$previous-last}"/>
     </xsl:if>

     <xsl:if test="$section!=$last">
      <next-section href="?section={$section + 1}"/>
     </xsl:if>

     <next href="?section={$section}&amp;slide=1"/>
    </navigation>

    <layout>
     <title><xsl:value-of select="title"/></title>
     <subtitle><xsl:value-of select="subtitle"/></subtitle>
    </layout>
  </xsl:template>

<!-- ================================ Slide ============================== -->

  <xsl:template match="slide">
    <title>
     <xsl:value-of select="title"/>
    </title>

    <navigation>
     <xsl:variable name="last-section">
      <xsl:value-of select="count(//section)"/>
     </xsl:variable>
     <xsl:variable name="last">
      <xsl:value-of select="count(//section[position() = $section]/slide)"/>
     </xsl:variable>
     <xsl:variable name="previous-last">
      <xsl:value-of select="count(//section[position() = ($section - 1)]/slide)"/>
     </xsl:variable>

     <home href="?section={$section}"/>

     <xsl:if test="$slide!=1">
      <previous href="?section={$section}&amp;slide={$slide - 1}"/>
     </xsl:if>
     <xsl:if test="($slide=1)">
      <previous href="?section={$section}"/>
     </xsl:if>

     <xsl:if test="$slide!=$last">
      <next href="?section={$section}&amp;slide={$slide + 1}"/>
     </xsl:if>
     <xsl:if test="($slide=$last) and ($section!=$last-section)">
      <next href="?section={$section + 1}"/>
     </xsl:if>

     <xsl:if test="$section!=1">
      <previous-section href="?section={$section - 1}"/>
     </xsl:if>
     <xsl:if test="$section!=$last-section">
      <next-section href="?section={$section + 1}"/>
     </xsl:if>
    </navigation>

    <layout>
     <xsl:apply-templates/>
    </layout>
  </xsl:template>
  
  <xsl:template match="*|@*|text()">
   <xsl:copy>
    <xsl:apply-templates select="*|@*|text()"/>
   </xsl:copy>
  </xsl:template>
  
</xsl:stylesheet>
