<%@ page language='java' session='false' %>
<%@ include file="cocoon_prefix.jspinclude" %>
<operation>
<% 
// Generate followup URLs. This is the minimum needed to provide some valid result. 
// If this fails, it will fail with an ugly Cocoon exception 
String successFollowupURI;
String failureFollowupURI;
{
  StringBuffer followupQueryStringBuffer = new StringBuffer();
  String       queryStringSeparator = "?";

  for (java.util.Enumeration e = request.getParameterNames() ; e.hasMoreElements() ;) {
   String parameterName = (String) e.nextElement();
   if (parameterName.startsWith("followup_") && 
       request.getParameter(parameterName)!=null && 
       request.getParameter(parameterName).length()>0 ) {
        followupQueryStringBuffer.append(queryStringSeparator);
        followupQueryStringBuffer.append(parameterName.substring(9));
        followupQueryStringBuffer.append("=");
        followupQueryStringBuffer.append(request.getParameter(parameterName));
        queryStringSeparator="&amp;";
   }
  }
  successFollowupURI = (request.getParameter("successURI")==null?"success":request.getParameter("successURI")) + followupQueryStringBuffer.toString();
  failureFollowupURI = (request.getParameter("failureURI")==null?"failure":request.getParameter("failureURI")) + followupQueryStringBuffer.toString();
}

try {

// Since input and output are the same file, the input 
// would get truncated when the output gets opened.
// Sitemaps are not very large, so we'll just read
// the whole thing into memory.

   String sitemap;
   String sitemapFilename = cocoonPrefix + request.getParameter("siteName") + "/sitemap.xmap";
   {
    // sitemapReader is the sitemap file     
    java.io.FileReader sitemapReader = new java.io.FileReader(sitemapFilename);
    // sitemapWriter will construct a string representation of the sitemap file contents
    java.io.StringWriter sitemapWriter = new java.io.StringWriter();

    // Read the whole thing into memory
    int lastReadChar = sitemapReader.read();
    while (lastReadChar != -1) {
     sitemapWriter.write(lastReadChar);
     lastReadChar = sitemapReader.read();
    }

    // Extract the string representation of the sitemap file
    sitemap = sitemapWriter.toString();

    // Clean up
    sitemapWriter.close();
    sitemapReader.close();
   }

// Now we build the transformation from the input parameters
// This XSLT file will append a new element named after the value of parameter 'elementName' 
// to the element identified by the XPath expression in parameter 'parentElementXPath'
// Parameters attribute_<postfix> will generate attributes for the new element. The attributes
// will be named after the postfix of the parameter, and its value is contained on the parameter.

   StringBuffer tb = new StringBuffer();

tb.append("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>"                                                                      );
tb.append(" <xsl:stylesheet version=\"1.0\" "                                                                                    );
tb.append("      xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\""                                                             );
tb.append("      xmlns:map=\"http://apache.org/cocoon/sitemap/1.0\""                                                             );
tb.append("     >"                                                                                                               );
tb.append("  <xsl:include href=\"" + cocoonPrefix + "sitebuilder/stylesheets/sitemapEditHelper.xsl\" />");


tb.append("  <xsl:template match=\"" + request.getParameter("parentElementXPath") + "\">"                                        );
tb.append("   <xsl:copy>"                                                                                                        );
tb.append("     <xsl:apply-templates select=\"@*|node()\"/> "                                                                    );
tb.append("    <" + request.getParameter("elementName"));

 for (java.util.Enumeration e = request.getParameterNames() ; e.hasMoreElements() ;) {
  String parameterName = (String) e.nextElement();
  if (parameterName.startsWith("attribute_") && request.getParameter(parameterName)!=null && request.getParameter(parameterName).length()>0 ) {
tb.append(" " + parameterName.substring(10) + "=\"" + request.getParameter(parameterName) + "\""                                               );
  }
 }

tb.append("    >"                                                                                                                );
tb.append("    <xsl:call-template name=\"emitNextElementId\" />");
tb.append("    </" + request.getParameter("elementName") + ">"                                                                   );
tb.append("   </xsl:copy> "                                                                                                      );
tb.append("  </xsl:template>"                                                                                                    );

//tb.append("  <xsl:template match=\"" + request.getParameter("parentElementXPath") +"/" + request.getParameter("elementName") + "\">" );
//tb.append("  </xsl:template>"                                                                                                    );

tb.append(" </xsl:stylesheet>"                                                                                                   );

    String transformation = tb.toString();

    java.io.PrintWriter outPrintWriter = new java.io.PrintWriter(new java.io.FileWriter(sitemapFilename));

      javax.xml.transform.TransformerFactory tFactory = 
                javax.xml.transform.TransformerFactory.newInstance();
      // Get the XML input document and the stylesheet, both in the servlet
      // engine document directory.
      javax.xml.transform.Source xmlSource = 
                new javax.xml.transform.stream.StreamSource
                             (new java.io.StringReader(sitemap));
      javax.xml.transform.Source xslSource = 
                new javax.xml.transform.stream.StreamSource
                             (new java.io.StringReader(transformation));

      // Generate the transformer.
      javax.xml.transform.Transformer transformer = 
                             tFactory.newTransformer(xslSource);

      // Perform the transformation, sending the outPrintWriterput to the response.
      transformer.transform(xmlSource, 
                           new javax.xml.transform.stream.StreamResult(outPrintWriter));
%>
<result type="success" redirect="<%=successFollowupURI %>" />
<%
} catch (Exception e) {
%>
<result type="failure" redirect="<%=failureFollowupURI %>" >
<stacktrace>
<%
  e.printStackTrace(new java.io.PrintWriter(out)); 
%>
</stacktrace>
</result>
<%
}
%>

</operation>