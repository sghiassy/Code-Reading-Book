<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:NetUtils="org.apache.cocoon.util.NetUtils"
                version="1.0">

  <xsl:param name="resource"/>

  <xsl:template match="book">
    <menu>
      <xsl:apply-templates/>
    </menu>
  </xsl:template>

  <xsl:template match="project">
  </xsl:template>

  <xsl:template match="menu[position()=1]">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="menu">
     <!-- Encode label to escape any reserved characters such as space -->
     <xsl:variable name="encLabel" select="NetUtils:encodePath(@label)"/>
     <tr>
       <td align="left" valign="top">
         <img border="0"
              height="20"
              hspace="0"
              src="graphics/{$encLabel}-separator.jpg"
              vspace="0"
              width="120"
              alt="{@label}"/>
         <br/>
       </td>
     </tr>
     <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="menu-item">
    <xsl:if test="not(@type) or @type!='hidden'">
     <xsl:variable name="encLabel" select="NetUtils:encodePath(@label)"/>
     <tr>
      <td align="left" valign="top">
       <xsl:choose>
         <xsl:when test="@href=$resource">
          <img alt="{@label}"
               src="graphics/{$encLabel}-label_select.jpg"
               border="0"
               height="12"
               width="120"
               name="{@label}"
               hspace="0"
               vspace="0"/>
         </xsl:when>
         <xsl:otherwise>
          <a href="{@href}" onMouseOut="rolloverOff('{@label}')"
                            onMouseOver="rolloverOn('{@label}')">
           <img alt="{@label}"
                src="graphics/{$encLabel}-label.jpg"
                border="0"
                height="12"
                width="120"
                name="{@label}"
                hspace="0"
                vspace="0"
                onLoad="rolloverLoad('{@label}', 'graphics/{$encLabel}-label_over.jpg', 'graphics/{$encLabel}-label.jpg');"/>
         </a>
        </xsl:otherwise>
       </xsl:choose>
      </td>
     </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template match="external">
    <xsl:if test="not(@type) or @type!='hidden'">
     <xsl:variable name="encLabel" select="NetUtils:encodePath(@label)"/>
     <tr>
	<td align="left" valign="top">
        <a href="{@href}" target="new"
                          onMouseOut="rolloverOff('{@label}')"
                          onMouseOver="rolloverOn('{@label}')">
          <img alt="{@label}"
               src="graphics/{$encLabel}-label.jpg"
               border="0"
               height="12"
               hspace="0"
               name="{@label}"
               vspace="0"
               width="120"
               onLoad="rolloverLoad('{@label}', 'graphics/{$encLabel}-label_over.jpg', 'graphics/{$encLabel}-label.jpg');"/>
        </a>
	</td>
     </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template match="node()|@*" priority="-1"/>
</xsl:stylesheet>

