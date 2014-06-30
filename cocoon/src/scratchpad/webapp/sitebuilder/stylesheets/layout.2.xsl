<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>
	<!-- Match The Root Node -->
	<xsl:template match="page">
         <HTML>
          <HEAD>
           <TITLE>Sitebuilder</TITLE>
           <link rel="stylesheet" href="/cocoon/sitebuilder/staticFile/sitebuilder.css" type="text/css" />

          </HEAD>
          <BODY bgcolor="white">
           <center>
           <table width="700"       align="center" 
		  bgcolor="#FFFFFF" name="t_corpo" 
		  cellpadding="5"   cellspacing="5" 
		  border="0"        vspace="0" 
		  hspace="0"
		  >
            <TR>
             <td valign="top" align="center" width="80">
              <xsl:apply-templates select="layout-leftbar"/>
             </td>
             <td width="580" bgcolor="#FFFFFF" valign="top">
              <xsl:apply-templates select="layout-content"/>
             </td>
            </TR>
            </table>
            </center>
          </BODY>
         </HTML>
	</xsl:template>

        <!-- Match page components -->
        <xsl:template match="layout-leftbar|layout-content">
         <xsl:apply-templates/>
        </xsl:template>

        <!-- Copy non-positioning markup -->
        <xsl:template match="@*|*|text()|processing-instruction()" priority="-1">
	 <xsl:copy>
          <xsl:apply-templates select="@*|*|text()|processing-instruction()"/>
         </xsl:copy>
        </xsl:template>

</xsl:stylesheet>








