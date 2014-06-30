<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ns1="http://foo.bar.com/slashdot"
                xmlns:ns2="http://foo.bar.com/moreover"
>
	<xsl:include href="../page/simple-page2html.xsl"/>
    
	<!-- Match The Root Node -->
	<xsl:template match="/">
		<HTML>
            <BODY>
                <xsl:apply-templates/>
            </BODY>
		</HTML>
	</xsl:template>

	<xsl:template match="HTML">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="ns1:HTML">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="ns2:HTML">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="BODY">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="ns1:BODY">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="ns2:BODY">
        <xsl:apply-templates/>
    </xsl:template>

	<xsl:template match="@*|*|text()|processing-instruction()" priority="-1">
		<xsl:copy>
			<xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>
