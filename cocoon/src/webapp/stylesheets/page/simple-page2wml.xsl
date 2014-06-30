<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

 <xsl:template match="page">
  <wml>
   <card id="index" title="{title}">
    <xsl:apply-templates select="content"/>
    <do type="accept" label="About">
     <go href="#about"/>
    </do>
   </card>

   <card id="about" title="About">
    <onevent type="ontimer">
     <prev/>
    </onevent>
    <timer value="25"/>
    <p align="center">
     <br/>
     <br/>
     <small>
      Copyright &#xA9; 2000<br/>
      Apache Software Foundation.<br/>
      All rights reserved.
     </small>
    </p>
   </card>
  </wml>
 </xsl:template>
  
 <xsl:template match="para">
  <p align="center">
   <xsl:apply-templates/>
  </p>
 </xsl:template>

</xsl:stylesheet>
