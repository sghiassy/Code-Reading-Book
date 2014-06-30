<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

 <xsl:template match="page">
  <svg width="450" height="160">
   <defs>
    <filter id="blur1"><feGaussianBlur stdDeviation="3"/></filter>
    <filter id="blur2"><feGaussianBlur stdDeviation="1"/></filter>
   </defs>
   
   <g title="this is a tooltip">
    <rect 
      style="fill:#0086B3;stroke:#000000;stroke-width:4;filter:url(#blur1);"
      x="30" y="30" rx="20" ry="20" width="400" height="80"/>
    <xsl:apply-templates/>
   </g>
  </svg>
 </xsl:template>
 
 <xsl:template match="para">
  <text style="fill:#FFFFFF;font-size:24;font-family:TrebuchetMS-Bold;filter:url(#blur2);" x="65" y="80">
   <xsl:apply-templates/>
  </text>
 </xsl:template>

</xsl:stylesheet>
