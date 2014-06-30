<?xml version="1.0"?>

<!-- Author: Berin Loritsch "bloritsch@infoplanning.com" -->
<!-- Version: 1.0 -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="document">
    <html>
      <head>
        <script language="JavaScript" type="text/javascript" src="resources/script.js"/>
        <title><xsl:value-of select="document/header/title"/></title>
        <xsl:if test="@redirect">
          <meta http-equiv="Refresh" content="{@time};URL={@redirect}"/>
        </xsl:if>
      </head>
      <body text="#000000" link="#039acc" vlink="#0086b2" alink="#cc0000"
            topmargin="4" leftmargin="4" marginwidth="4" marginheight="4"
            bgcolor="#ffffff">
        <!-- THE TOP BAR (HEADER) -->
        <table width="100%" cellspacing="0" cellpadding="0" border="0">
          <tr>
            <td width="135" height="60" rowspan="3" valign="top" align="left">
              <img width="135" height="60" src="images/logo.gif" hspace="0" vspace="0" border="0"/>
            </td>
            <td width="100%" height="5" valign="top" align="left" colspan="2" background="images/line.gif">
              <img width="1" height="5" src="images/line.gif" hspace="0" vspace="0" border="0" align="left"/>
            </td>
            <td width="29" height="60"  rowspan="3" valign="top" align="left">
              <img width="29" height="60" src="images/right.gif" hspace="0" vspace="0" border="0"/>
            </td>
          </tr>
          <tr>
            <td width="100%" height="35" valign="top" align="left" colspan="2" bgcolor="#0086b2"><font size="5" face="arial,helvetica,sanserif" color="#ffffff"><div style="text-align: right;"><xsl:value-of select="header/title"/></div></font></td>
          </tr>
          <tr>
            <td width="100%" height="20" valign="top" align="left" bgcolor="#0086b2" background="images/bottom.gif">
              <img width="3" height="20" src="images/bottom.gif" hspace="0" vspace="0" border="0" align="left"/>
            </td>
            <td align="right" bgcolor="#0086b2" height="20" valign="top" width="288" background="images/bottom.gif">
              <table border="0" cellpadding="0" cellspacing="0" width="288">
                <tr>
                  <td width="96" height="20" valign="top" align="left">
                    <a href="http://xml.apache.org/" onMouseOver="rolloverOn('xml');" onMouseOut="rolloverOff('xml');" target="new">
                      <img alt="http://xml.apache.org/" width="96" height="20" src="images/button-xml-lo.gif"
                           name="xml" hspace="0" vspace="0" border="0"
                           onLoad="rolloverLoad('xml','images/button-xml-hi.gif','images/button-xml-lo.gif');"/>
                    </a>
                  </td>
                  <td width="96" height="20" valign="top" align="left">
                    <a href="http://www.apache.org/" onMouseOver="rolloverOn('asf');" onMouseOut="rolloverOff('asf');" target="new">
                      <img alt="http://www.apache.org/" width="96" height="20" src="images/button-asf-lo.gif"
                           name="asf" hspace="0" vspace="0" border="0"
                           onLoad="rolloverLoad('asf','images/button-asf-hi.gif','images/button-asf-lo.gif');"/>
                    </a>
                  </td>
                  <td width="96" height="20" valign="top" align="left">
                    <a href="http://www.w3.org/" onMouseOver="rolloverOn('w3c');" onMouseOut="rolloverOff('w3c');" target="new">
                      <img alt="http://www.w3.org/" width="96" height="20" src="images/button-w3c-lo.gif"
                           name="w3c" hspace="0" vspace="0" border="0"
                           onLoad="rolloverLoad('w3c','images/button-w3c-hi.gif','images/button-w3c-lo.gif');"/>
                    </a>
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </table>
        
        <!-- THE MAIN PANEL (SIDEBAR AND CONTENT) -->
        <table width="100%" cellspacing="0" cellpadding="0" border="0">
          <tr>
            <!-- THE SIDE BAR -->
            <td width="120" valign="top" align="left">
              <div width="120" style="background-color: #a0a0a0;">
                <font face="arial,helvetica,sanserif" color="#ffffff" size="-1">
                <img width="120" height="14" src="images/join.gif" hspace="0" vspace="0" border="0"/><br/>
                <xsl:apply-templates select="document('../menu.xml')"/>
                <img width="120" height="14" src="images/close.gif" hspace="0" vspace="0" border="0"/><br/>
                </font>
              </div>
            </td>
            <!-- THE CONTENT PANEL -->
            <td width="*" valign="top" align="left">
              <table border="0" cellspacing="0" cellpadding="3">
                <tr><td><br/><xsl:apply-templates/></td></tr>
              </table>
            </td>
          </tr>
        </table>
        
        <br/>
        
        <table width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr><td bgcolor="#0086b2"><img src="images/dot.gif" width="1" height="1"/></td></tr>
          <tr>
            <td align="center"><font face="arial,helvetica,sanserif" size="-1" color="#0086b2"><i>
              Copyright &#169; 1999-2001.
              All Rights Reserved.
            </i></font></td>
          </tr>
        </table>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="body">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="menu">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="group">
    <a class="menu" href="{@link}"><font size="+1" color="#ffff80"><xsl:value-of select="@title"/></font></a><br/>
    <xsl:apply-templates select="item"/>
  </xsl:template>

  <xsl:template match="item">
    <a class="menu" href="{@link}"><font color="#ffffff"><xsl:value-of select="@title"/></font></a><br/>
  </xsl:template>

  <xsl:template match="flash">
    <div align="center">
    <object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://active.macromedia.com/flash2/cabs/swflash.cab#version=4,0,0,0" id="{@id}" height="{@height}" width="{@width}">
      <param value="{@movie}" name="movie"/>
      <param value="{@quality}" name="quality"/>
      <param value="{@bg}" name="bgcolor"/>
      <embed src="{@movie}" quality="{@quality}" bgcolor="{@bg}" height="{@height}" width="{@width}" type="application/x-shockwave-flash" pluginspace="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"/>
    </object>
    </div><p/>
  </xsl:template>

  <xsl:template match="header"/>

  <xsl:template match="s1">
   <div align="right">
    <table border="0" width="98%" cellspacing="0" cellpadding="0">
      <tr>
        <td width="9" height="7" valign="bottom" align="right"><img src="images/bar-top-left.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-top.gif"><img src="images/void.gif" width="1" height="5" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="7" valign="bottom" align="left"><img src="images/bar-top-right.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" background="images/bar-border-left.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
        <td width="100%" bgcolor="#0086b2">
          <font size="+1" face="arial,helvetica,sanserif" color="#ffffff">
            <img src="images/void.gif" width="5" height="5" vspace="0" hspace="0" border="0"/><b><xsl:value-of select="@title"/></b></font>
         </td>
        <td width="9" background="images/bar-border-right.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" height="12" valign="top" align="right"><img src="images/bar-bottom-left.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-bottom.gif"><img src="images/void.gif" height="12" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="12" valign="top" align="left"><img src="images/bar-bottom-right.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
      </tr>
     </table>
     <table border="0" width="98%" cellspacing="0" cellpadding="0">
      <tr>
       <td>
        <font face="arial,helvetica,sanserif" color="#000000"><xsl:apply-templates/></font>
       </td>
      </tr>
    </table>
   </div>
   <br/>
  </xsl:template>

  <xsl:template match="s2">
   <div align="right">
    <table border="0" width="95%" cellspacing="0" cellpadding="0">
      <tr>
        <td width="9" height="7" valign="bottom" align="right"><img src="images/bar-top-left.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-top.gif"><img src="images/void.gif" width="1" height="5" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="7" valign="bottom" align="left"><img src="images/bar-top-right.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" background="images/bar-border-left.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
        <td width="100%" bgcolor="#0086b2">
          <font face="arial,helvetica,sanserif" color="#ffffff">
            <img src="images/void.gif" width="5" height="5" vspace="0" hspace="0" border="0"/><b><xsl:value-of select="@title"/></b></font>
         </td>
        <td width="9" background="images/bar-border-right.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" height="12" valign="top" align="right"><img src="images/bar-bottom-left.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-bottom.gif"><img src="images/void.gif" width="1" height="12" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="12" valign="top" align="left"><img src="images/bar-bottom-right.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
      </tr>
     </table>
     <table border="0" width="95%" cellspacing="0" cellpadding="0">
      <tr>
       <td>
        <font face="arial,helvetica,sanserif" color="#000000"><xsl:apply-templates/></font>
       </td>
      </tr>
    </table>
   </div>
   <br/>
  </xsl:template>

  <xsl:template match="s3">
   <div align="right">
    <table border="0" width="90%" cellspacing="0" cellpadding="0">
      <tr>
        <td width="9" height="7" valign="bottom" align="right"><img src="images/bar-top-left.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-top.gif"><img src="images/void.gif" width="1" height="5" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="7" valign="bottom" align="left"><img src="images/bar-top-right.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" background="images/bar-border-left.gif"><img src="iamges/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
        <td width="100%" bgcolor="#0086b2">
          <font size="-1" face="arial,helvetica,sanserif" color="#ffffff">
            <img src="images/void.gif" width="5" height="5" vspace="0" hspace="0" border="0"/><b><xsl:value-of select="@title"/></b></font>
         </td>
        <td width="9" background="images/bar-border-right.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" height="12" valign="top" align="right"><img src="images/bar-bottom-left.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-bottom.gif"><img src="images/void.gif" width="1" height="12" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="12" valign="top" align="left"><img src="images/bar-bottom-right.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
      </tr>
     </table>
     <table border="0" width="90%" cellspacing="0" cellpadding="0">
      <tr>
       <td>
        <font face="arial,helvetica,sanserif" color="#000000"><xsl:apply-templates/></font>
       </td>
      </tr>
    </table>
   </div>
   <br/>
  </xsl:template>

  <xsl:template match="s4">
   <div align="right">
    <table border="0" width="85%" cellspacing="0" cellpadding="0">
      <tr>
        <td width="9" height="7" valign="bottom" align="right"><img src="images/bar-top-left.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-top.gif"><img src="images/void.gif" width="1" height="5" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="7" valign="bottom" align="left"><img src="images/bar-top-right.gif" width="9" height="7" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" background="images/bar-border-left.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
        <td width="100%" bgcolor="#0086b2">
          <font size="-2" face="arial,helvetica,sanserif" color="#ffffff">
            <img src="images/void.gif" width="5" height="5" vspace="0" hspace="0" border="0"/><b><xsl:value-of select="@title"/></b></font>
         </td>
        <td width="9" background="images/bar-border-right.gif"><img src="images/void.gif" width="9" height="1" vspace="0" hspace="0" border="0"/></td>
      </tr>
      <tr>
        <td width="9" height="12" valign="top" align="right"><img src="images/bar-bottom-left.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
        <td background="images/bar-border-bottom.gif"><img src="images/void.gif" width="1" height="12" vspace="0" hspace="0" border="0"/></td>
        <td width="9" height="12" valign="top" align="left"><img src="images/bar-bottom-right.gif" width="9" height="12" vspace="0" hspace="0" border="0"/></td>
      </tr>
     </table>
     <table border="0" width="85%" cellspacing="0" cellpadding="0">
      <tr>
       <td>
        <font face="arial,helvetica,sanserif" color="#000000"><xsl:apply-templates/></font>
       </td>
      </tr>
    </table>
   </div>
   <br/>
  </xsl:template>
  
  <xsl:template match="form">
    <form name="form_{@name}" action="{@handler}" method="POST">
      <xsl:apply-templates/>
    </form>
  </xsl:template>

  <xsl:template match="submit">
    <input type="submit" name="cocoon-action" value="{@name}"/>
  </xsl:template>

  <xsl:template match="reset">
    <input type="reset" value="{@name}"/>
  </xsl:template>
  
  <xsl:template match="parameter">
    <input type="hidden" name="{@name}" value="{@value}"/>
  </xsl:template>
  
  <xsl:template match="radio">
    <xsl:for-each select="option">
      <xsl:choose>
        <xsl:when test="position()=1">
          <input type="radio" name="{../@group}" value="{@value}" checked="true"/>
        </xsl:when>
        <xsl:otherwise>
          <input type="radio" name="{../@group}" value="{@value}"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:value-of select="."/><br/>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="text">
    <xsl:choose>
      <xsl:when test="@required='true'">
        <span style="color: #ff0000;">*</span>
      </xsl:when>
      <xsl:otherwise>&#160;</xsl:otherwise>
    </xsl:choose>
    <input type="text" name="{@name}" size="{@size}" value="{@value}"/>
  </xsl:template>
  
  <xsl:template match="check">
    <xsl:element name="input">
      <xsl:attribute name="type">checkbox</xsl:attribute>
      <xsl:attribute name="value">1</xsl:attribute>
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:if test="@checked='true'">
        <xsl:attribute name="checked"/>
      </xsl:if>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="p">
    <p align="justify"><xsl:apply-templates/></p>
  </xsl:template>

  <xsl:template match="note">
   <p>
    <table width="100%" cellspacing="3" cellpadding="0" border="0">
      <tr>
        <td width="28" valign="top">
          <img src="images/note.gif" width="28" height="29" vspace="0" hspace="0" border="0" alt="Note"/>
        </td>
        <td valign="top">
          <font size="-1" face="arial,helvetica,sanserif" color="#000000">
            <i>
              <xsl:apply-templates/>
            </i>
          </font>
        </td>
      </tr>  
    </table>
   </p>
  </xsl:template>

  <xsl:template match="source">
   <div align="center">
    <table cellspacing="4" cellpadding="0" border="0">
    <tr>
      <td bgcolor="#0086b2" width="1" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
      <td bgcolor="#0086b2" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
      <td bgcolor="#0086b2" width="1" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
    </tr>
    <tr>
      <td bgcolor="#0086b2" width="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
      <td bgcolor="#ffffff"><pre><xsl:apply-templates/></pre></td>
      <td bgcolor="#0086b2" width="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
    </tr>
    <tr>
      <td bgcolor="#0086b2" width="1" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
      <td bgcolor="#0086b2" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
      <td bgcolor="#0086b2" width="1" height="1"><img src="images/void.gif" width="1" height="1" vspace="0" hspace="0" border="0"/></td>
    </tr>
    </table>
   </div>
  </xsl:template>

 <xsl:template match="figure">
  <p align="center"><img src="{@src}" alt="{@alt}" border="0" vspace="4" hspace="4"/></p>
 </xsl:template>
 
 <xsl:template match="img">
   <img src="{@src}" alt="{@alt}" border="0" vspace="4" hspace="4" align="right"/>
 </xsl:template>

 <xsl:template match="icon">
   <img src="{@src}" alt="{@alt}" border="0" align="absmiddle"/>
 </xsl:template>

  <xsl:template match="footer"/>

  <xsl:template match="fixme"/>

 <xsl:template match="ul|ol|dl">
  <blockquote>
   <xsl:copy>
    <xsl:apply-templates/>
   </xsl:copy>
  </blockquote>
 </xsl:template>
 
 <xsl:template match="li">
  <xsl:copy>
   <xsl:apply-templates/>
  </xsl:copy>
 </xsl:template>

 <xsl:template match="sl">
  <ul>
   <xsl:apply-templates/>
  </ul>
 </xsl:template>

 <xsl:template match="dt">
  <li>
   <strong><xsl:value-of select="."/></strong>
   <xsl:text> - </xsl:text>
   <xsl:apply-templates select="dd"/>   
  </li>
 </xsl:template>

  <xsl:template match="table">
    <table width="100%" border="0" cellspacing="2" cellpadding="2">
      <caption><xsl:value-of select="caption"/></caption>
      <xsl:apply-templates/>
    </table>
  </xsl:template>

  <xsl:template match="tr">
    <tr><xsl:apply-templates/></tr>
  </xsl:template>

  <xsl:template match="th">
    <td bgcolor="#039acc" colspan="{@colspan}" rowspan="{@rowspan}" valign="center" align="center">
      <font color="#ffffff" size="-1" face="arial,helvetica,sanserif">
        <b><xsl:apply-templates/></b>&#160;
      </font>
    </td>
  </xsl:template>

  <xsl:template match="td">
    <td bgcolor="#a0ddf0" colspan="{@colspan}" rowspan="{@rowspan}" valign="top" align="left">
      <font color="#000000" size="-1" face="arial,helvetica,sanserif">
        <xsl:apply-templates/>&#160;
      </font>
    </td>
  </xsl:template>

  <xsl:template match="tn">
    <td bgcolor="#ffffff" colspan="{@colspan}" rowspan="{@rowspan}">
      &#160;
    </td>
  </xsl:template>
  
  <xsl:template match="caption"/>

  <xsl:template match="overview">
    <div class="ovw"><xsl:apply-templates/></div>
  </xsl:template>

  <xsl:template match="quote">
    <div class="ctr"><xsl:apply-templates/></div>
  </xsl:template>

  <xsl:template match="rule">
    <br/><img src="images/hstrip.gif" width="364" height="18"/><br/>
  </xsl:template>

 <xsl:template match="link">
   <a href="{@href}"><xsl:apply-templates/></a>
 </xsl:template>

 <xsl:template match="connect">
  <xsl:apply-templates/>
 </xsl:template>

 <xsl:template match="jump">
   <a href="{@href}#{@anchor}"><xsl:apply-templates/></a>
 </xsl:template>

 <xsl:template match="fork">
   <a href="{@href}" target="_blank"><xsl:apply-templates/></a>
 </xsl:template>

 <xsl:template match="anchor">
   <a name="{@id}"><xsl:comment>anchor</xsl:comment></a>
 </xsl:template>  

 <xsl:template match="strong">
   <b><xsl:apply-templates/></b>
 </xsl:template>

 <xsl:template match="em">
    <i><xsl:apply-templates/></i>
 </xsl:template>

 <xsl:template match="code">
    <code><font face="courier, monospaced"><xsl:apply-templates/></font></code>
 </xsl:template>
 
  <xsl:template match="connect">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="bottom">
    <div class="bot"><xsl:apply-templates/></div>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
