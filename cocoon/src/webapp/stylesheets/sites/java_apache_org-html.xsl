<?xml version="1.0"?>

<!-- Author: Stefano Mazzocchi "stefano@apache.org" -->
<!-- Version: $Id: java_apache_org-html.xsl,v 1.1 2002/01/03 12:31:46 giacomo Exp $ -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="page">
   <html>
    <head>
     <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
     <meta name="Author" content="{author}"/>
     <meta name="Version" content="{version}"/>
     <title><xsl:value-of select="title"/></title>
    </head>

    <body bgcolor="#ffffff">
    <center>
     <table border="0" width="100%" bgcolor="#000000" cellspacing="0" cellpadding="0">
      <tr>
       <td>
        <table border="0" width="100%" cellspacing="2" cellpadding="5">
         <tr>
          <td bgcolor="#F0F0F0">
           <table border="0" width="100%" cellspacing="0" cellpadding="3">
            <tr>
             <td width="100%" align="center">
              <table border="0" width="100%" cellspacing="10">
               <tr>
                <td valign="top" width="60%">
                 <xsl:apply-templates select="newscolumn"/>
                </td>
                <td valign="top" width="40%">
                 <xsl:apply-templates select="statuscolumn"/>
                </td>
               </tr>
              </table>
             </td>
            </tr>
            <tr>
             <td width="100%" align="left">
              <small><small>
               <xsl:value-of select="legal"/>
              </small></small>
             </td>
            </tr>
           </table>
          </td>
         </tr>
        </table>
       </td>
      </tr>
     </table>
    </center>
    </body>
   </html>
  </xsl:template>

  <xsl:template match="newscolumn">
   <table border="0" width="100%" bgcolor="#000000" cellspacing="0" cellpadding="0">
    <tr>
     <td width="100%">
      <table border="0" width="100%" cellpadding="4">
       <tr>
        <td bgcolor="#C0C0C0" align="right">
         <big><big><b><xsl:text>News</xsl:text></b></big></big>
        </td>
       </tr>
       <xsl:apply-templates/>
      </table>
     </td>
    </tr>
   </table>
  </xsl:template>

  <xsl:template match="news">
   <tr>
    <td bgcolor="#E0E0E0" align="left">
     <a href="{link}">
      <strong><xsl:value-of select="title"/></strong>
     </a>
    </td>
   </tr>
   <tr>
    <td bgcolor="#ffffff" align="left">
     <strong><xsl:value-of select="date"/></strong>
     <xsl:text> - </xsl:text>
     <xsl:value-of select="content"/>
    </td>
   </tr>
  </xsl:template>

  <xsl:template match="statuscolumn">
   <table border="0" width="100%" bgcolor="#000000" cellspacing="0" cellpadding="0">
    <tr>
     <td width="100%">
      <table border="0" width="100%" cellpadding="4">
       <tr>
        <td bgcolor="#C0C0C0" colspan="2" align="right">
         <big><big><b><xsl:text>Status</xsl:text></b></big></big>
        </td>
       </tr>
       <xsl:apply-templates/>
      </table>
     </td>
    </tr>
   </table>
  </xsl:template>

  <xsl:template match="project">
   <tr>
    <td bgcolor="#E0E0E0" colspan="2" align="left">
     <a href="{link}">
      <b><xsl:value-of select="title"/></b>
     </a>
    </td>
   </tr>
   <xsl:apply-templates select="release"/>
  </xsl:template>

  <xsl:template match="release">
   <tr>
    <td bgcolor="#FFFFFF" align="center" rowspan="2">
     <b><xsl:value-of select="version"/></b>
    </td>
    <td bgcolor="#D0FFD0" align="left">
     <b><xsl:value-of select="status"/></b>
    </td>
   </tr>
   <tr>
    <td bgcolor="#FFFFFF" align="left">
     <small><xsl:value-of select="comment"/></small>
    </td>
   </tr>
  </xsl:template>

</xsl:stylesheet>
