<?xml version="1.0"?>

<html xmlns:error="http://apache.org/cocoon/error/2.0"
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
      xsl:version="1.0">
  <head>
    <title>Requested resource is not found</title>
  </head>
  <body bgcolor="#ffffff">
    <p>
      <font face="Verdana" size="+2">Requested resource is not found</font>
    </p>
    <p>
      <font face="Verdana">
        Error occured during request processing:
        <b><xsl:value-of select="error:notify/error:message"/></b>
      </font>
    </p>
    <br/>
    <font face="Verdana" size="-1">The corresponding exception stacktrace:</font>
    <pre>
      <xsl:value-of select="translate(error:notify/error:extra[2],'&#13;',' ')"/>
    </pre>
    <font face="Verdana" size="-2">
      This special error layout is specified in the sub sitemap.
    </font>
  </body>
</html>
