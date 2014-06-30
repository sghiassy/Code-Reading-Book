<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    
    <xsl:template match="/">
        <html>
            <head>
                <title>Sample of Parent Component Managers</title>
            </head>
            <body alink="#743e75" vlink="#00698c" link="#0086b2" bgcolor="#ffffff">
                <table width="100%" height="100%">
                    <tr>
                        <td valign="center" align="center">
                            
                            <table bgcolor="#000000" border="0" cellspacing="2" cellpadding="2" align="center">
                                <tr>
                                    <td bgcolor="#0086b2" width="100%" align="left">
                                        <font size="+1" face="arial,helvetica,sanserif" color="#ffffff">Current Time</font>
                                    </td>
                                </tr>
                                <tr>
                                    <td width="100%" bgcolor="#ffffff" align="center">
                                        <p>
                                            <font color="#000000" face="arial,helvetica,sanserif" size="+0">
                                                The time below was obtained from a component managed by Cocoon2's parent component manager.<br/>
                                                See <a href="documents/parent-component-manager">parent-component-manager.xml</a> for more information.
                                            </font>
                                        </p>
                                        <p>
                                            <font color="#000000" face="arial,helvetica,sanserif" size="+4">
                                                <xsl:value-of select="time"/>
                                            </font>
                                        </p>
                                        <p>
                                            <font color="#000000" face="arial,helvetica,sanserif" size="+0">
                                                <a href="./">Back</a>
                                            </font>
                                        </p>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>
            </body>
        </html>
    </xsl:template>
    
</xsl:stylesheet>

