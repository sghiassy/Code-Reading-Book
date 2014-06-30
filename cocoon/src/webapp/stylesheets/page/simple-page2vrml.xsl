<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="page">
		<!-- 
		     due to a DOM limitation, you must wrap your generated VRML
		     with a fake tag, here uses <vrml>, that is stripped out by the
		     text formatter 
		  -->

		<vrml><xsl:text>#VRML V2.0 utf8</xsl:text>
        	<xsl:apply-templates select="content"/>
		</vrml>
	</xsl:template>

	<xsl:template match="content">
		<xsl:text>
			Transform {
				translation 0 0 9
				rotation 0 0 1 0.0
				children Shape {
					appearance DEF WHITE Appearance {
						material Material {
							diffuseColor 1 1 1
						}
					}
					geometry Text {
						string [ " </xsl:text><xsl:value-of select="para"/><xsl:text> " ]
						fontStyle DEF MFS FontStyle {
							size 0.1
							family "SERIF"
							style "BOLD"
							justify "MIDDLE"
						}
					}
				}
			}
		</xsl:text>
</xsl:template>

</xsl:stylesheet>
