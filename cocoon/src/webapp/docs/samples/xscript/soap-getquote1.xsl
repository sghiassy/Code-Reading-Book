<?xml version="1.0" encoding="utf-8"?>

<!--
  Author: Ovidiu Predescu "ovidiu@cup.hp.com"

  Date: October 9, 2001
 -->

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:n="urn:xmethods-delayed-quotes"
  exclude-result-prefixes="soap n"
  >

  <xsl:template match="soap:Envelope">
    <b><xsl:value-of select="soap:Body/n:getQuoteResponse/Result"/></b>
  </xsl:template>

  <xsl:template match="@*|*|text()|processing-instruction()">
    <!-- Catch all template. Just pass along unmodified everything we
         don't handle. -->
    <xsl:copy>
      <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
