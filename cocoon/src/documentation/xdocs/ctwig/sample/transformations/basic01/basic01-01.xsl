<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:template match="page">
  <html>
		<head>
			<title><xsl:value-of select="title"/></title>
		</head>
    <body>
      <h1><xsl:value-of select="title"/></h1>
			<p><xsl:value-of select="greeting"/></p>
    </body>
  </html>
</xsl:template>

</xsl:stylesheet>
