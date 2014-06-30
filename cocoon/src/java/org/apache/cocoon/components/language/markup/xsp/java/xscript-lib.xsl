<?xml version="1.0" encoding="utf-8"?>

<!--
  Author: Ovidiu Predescu "ovidiu@cup.hp.com"

  Date: September 19, 2001
 -->

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:xsp-request="http://apache.org/xsp/request/2.0"
  xmlns:xscript="http://apache.org/xsp/xscript/1.0"
  xmlns:xalan="http://xml.apache.org/xalan"
  xmlns:saxon="http://icl.com/saxon"
  >

  <xsl:template name="xscript-variable">
    <!-- PUBLIC: create a new XScript variable -->
    <xsl:param name="name"/>
    <xsl:param name="href"/>
    <xsl:param name="scope"/>
    <xsl:param name="context"/>

    <xsl:choose>
      <xsl:when test="$href">
        <xsl:call-template name="xscript-variable-from-url">
          <xsl:with-param name="name" select="$name"/>
          <xsl:with-param name="scope" select="$scope"/>
          <xsl:with-param name="context" select="$context"/>
          <xsl:with-param name="href" select="$href"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="xscript-variable-inline">
          <xsl:with-param name="name" select="$name"/>
          <xsl:with-param name="scope" select="$scope"/>
          <xsl:with-param name="context" select="$context"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="xscript-get">
    <!-- PUBLIC: obtain the value of an XScript variable -->
    <xsl:param name="name"/>
    <xsl:param name="as"/>
    <xsl:param name="scope"/>
    <xsl:param name="context"/>

    <xsl:variable name="object">
      <xsl:choose>
        <xsl:when test="contains($scope, 'ALL_SCOPES')">
          xscriptManager.getFirst("<xsl:value-of select="$name"/>",
                                  <xsl:value-of select="$context"/>)
        </xsl:when>
        <xsl:otherwise>
          xscriptManager.get("<xsl:value-of select="$name"/>",
                             <xsl:value-of select="$scope"/>,
                             <xsl:value-of select="$context"/>)
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$as = 'object'">
        <xsp:logic><xsl:value-of select="$object"/></xsp:logic>
      </xsl:when>
      <xsl:otherwise>
        <!-- insert the content of the XScript variable in the SAX
             event stream -->
        <xsp:logic>
          try {
            <xsl:value-of select="$object"/>.toSAX(this.contentHandler);
          }
          catch (IllegalArgumentException ex) {
            <xscript:error><xsp:expr>ex.getMessage()</xsp:expr></xscript:error>
          }
        </xsp:logic>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="xscript-remove">
    <!-- PUBLIC: undeclare an XScript variable -->
    <xsl:param name="name"/>
    <xsl:param name="scope"/>
    <xsl:param name="context"/>
    <xsp:logic>
      <xsl:choose>
        <xsl:when test="contains($scope, 'ALL_SCOPES')">
          xscriptManager.removeFirst("<xsl:value-of select="$name"/>",
                                     <xsl:value-of select="$context"/>);
        </xsl:when>
        <xsl:otherwise>
          xscriptManager.remove("<xsl:value-of select="$name"/>",
                                <xsl:value-of select="$scope"/>,
                                <xsl:value-of select="$context"/>);
        </xsl:otherwise>
      </xsl:choose>
    </xsp:logic>
  </xsl:template>

  <xsl:template name="xscript-transform">
    <!-- PUBLIC: transform an XScriptObject pointed to by a variable
         using another XScriptObject, which is assumed to contain an
         XSLT stylesheet. -->
    <xsl:param name="name"/>
    <xsl:param name="scope"/>
    <xsl:param name="context">""</xsl:param>
    <xsl:param name="stylesheet"/>
    <xsl:param name="stylesheet-scope"/>
    <xsl:param name="stylesheet-context">""</xsl:param>
    <xsl:param name="parameters"/>

    <xsl:variable name="object">
      <xsl:call-template name="xscript-get">
        <xsl:with-param name="name" select="$name"/>
        <xsl:with-param name="scope" select="$scope"/>
        <xsl:with-param name="context" select="$context"/>
        <xsl:with-param name="as" select="'object'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="stylesheet-object">
      <xsl:call-template name="xscript-get">
        <xsl:with-param name="name" select="$stylesheet"/>
        <xsl:with-param name="scope" select="$stylesheet-scope"/>
        <xsl:with-param name="context" select="$stylesheet-context"/>
        <xsl:with-param name="as" select="'object'"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="params">
      params<xsl:value-of select="count(ancestor-or-self::*)"/>
    </xsl:variable>

    <xsp:logic>
      {
        Parameters <xsl:value-of select="$params"/> = new Parameters();
        <xsl:call-template name="xscript-parameter">
          <xsl:with-param name="params" select="$params"/>
          <xsl:with-param name="parameters" select="$parameters"/>
        </xsl:call-template>
        XScriptObject result = <xsl:value-of select="$object"/>.transform(
             <xsl:value-of select="$stylesheet-object"/>,
             <xsl:value-of select="$params"/>);
<!--
        System.out.println("input source =\n" + <xsl:value-of select="$object"/>);
        System.out.println("stylesheet source =\n" + <xsl:value-of select="$stylesheet-object"/>);
        System.out.println("transformation result =\n" + result);
-->
        result.toSAX(this.contentHandler);
      }
    </xsp:logic>
  </xsl:template>


  <!-- Helper templates used by this stylesheet, and possibly others
       as well -->

  <xsl:template name="xscript-variable-from-url">
    <!-- PUBLIC: create an XScript variable from an URL -->
    <xsl:param name="name"/>
    <xsl:param name="scope"/>
    <xsl:param name="context"/>
    <xsl:param name="href"/>
    <xsp:logic>
      xscriptManager.put("<xsl:value-of select="$name"/>",
                         new XScriptObjectFromURL(xscriptManager,
                                                  "<xsl:value-of select="$href"/>"),
                         <xsl:value-of select="$scope"/>,
                         <xsl:value-of select="$context"/>);
    </xsp:logic>
  </xsl:template>

  <xsl:template name="xscript-variable-inline">
    <!-- PUBLIC: create an XScript variable from inline XML -->
    <xsl:param name="name"/>
    <xsl:param name="scope"/>
    <xsl:param name="context"/>

    <xsl:variable name="xml-inline">
      xmlInline<xsl:value-of select="count(ancestor-or-self::*)"/>
    </xsl:variable>

    <xsl:variable name="oldHandler">
      oldHandler<xsl:value-of select="count(ancestor-or-self::*)"/>
    </xsl:variable>

    <xsp:logic>
      {
        XScriptObjectInlineXML <xsl:value-of select="$xml-inline"/>
          = new XScriptObjectInlineXML(xscriptManager);
        ContentHandler <xsl:value-of select="$oldHandler"/> = this.contentHandler;
        <xsl:value-of select="$xml-inline"/>.setNextContentHandler(<xsl:value-of select="$oldHandler"/>);
        this.contentHandler = <xsl:value-of select="$xml-inline"/>.getContentHandler();
        <xsl:apply-templates/>
        this.contentHandler = <xsl:value-of select="$oldHandler"/>;
        xscriptManager.put("<xsl:value-of select="$name"/>",
                           <xsl:value-of select="$xml-inline"/>,
                           <xsl:value-of select="$scope"/>,
                           <xsl:value-of select="$context"/>);
      }
    </xsp:logic>

  </xsl:template>

  <xsl:template name="xscript-get-scope">
    <!-- PUBLIC: obtain the Java expression for a given XScript
         variable scope. If no scope parameter is specified,
         ALL_SCOPES is assumed. -->
    <xsl:param name="scope" select="'all-scopes'"/>
    <xsl:choose>
      <xsl:when test="$scope = 'global'">org.apache.cocoon.components.xscript.XScriptManager.GLOBAL_SCOPE</xsl:when>
      <xsl:when test="$scope = 'page'">org.apache.cocoon.components.xscript.XScriptManager.PAGE_SCOPE</xsl:when>
      <xsl:when test="$scope = 'session'">org.apache.cocoon.components.xscript.XScriptManager.SESSION_SCOPE</xsl:when>
      <xsl:otherwise>org.apache.cocoon.components.xscript.XScriptManager.ALL_SCOPES</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="xscript-get-scope-for-creation">
    <!-- PUBLIC: obtain the Java expression for a given XScript
         variable scope. Similar with xscript-get-scope, but when
         defining an XScript variable, ALL_SCOPES doesn't make
         sense: if no scope parameter is specified, SESSION_SCOPE is
         assumed. -->
    <xsl:param name="scope" select="'session'"/>
    <xsl:choose>
      <xsl:when test="$scope = 'global'">org.apache.cocoon.components.xscript.XScriptManager.GLOBAL_SCOPE</xsl:when>
      <xsl:when test="$scope = 'page'">org.apache.cocoon.components.xscript.XScriptManager.PAGE_SCOPE</xsl:when>
      <xsl:otherwise>org.apache.cocoon.components.xscript.XScriptManager.SESSION_SCOPE</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="xscript-get-context">
    <!-- PUBLIC: obtain the Java expression for an XScript variable
         context. If the 'scope' parameter is not specified or when
         its value is 'all-scopes', an expression that contains both
         the session and the page scope is returned. This could be
         passed directly to the getFirst() method of XScriptManager.
         -->
    <xsl:param name="scope" select="'all-scopes'"/>
    <xsl:choose>
      <xsl:when test="$scope = 'global'">""</xsl:when>
      <!-- for the page scope use the context-path as context -->
      <xsl:when test="$scope = 'page'">(XSPRequestHelper.getContextPath(objectModel))</xsl:when>
      <xsl:when test="$scope = 'session'">(XSPRequestHelper.getSessionId(objectModel))</xsl:when>
      <xsl:otherwise>(XSPRequestHelper.getSessionId(objectModel)), (XSPRequestHelper.getContextPath(objectModel))</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="xscript-get-context-for-creation">
    <!-- PUBLIC: obtain the Java expression for an XScript variable
         context. If the `scope' parameter is not specified, the
         default is SESSION_SCOPE.
         -->
    <xsl:param name="scope" select="'session'"/>
    <xsl:choose>
      <xsl:when test="$scope = 'global'">""</xsl:when>
      <!-- for the page scope use the context-path as context -->
      <xsl:when test="$scope = 'page'">(XSPRequestHelper.getContextPath(objectModel))</xsl:when>
      <xsl:otherwise>(XSPRequestHelper.getSessionId(objectModel))</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="xscript-parameter">
    <xsl:param name="parameters"/>
    <xsl:param name="params"/>

    <xsl:choose>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'xalan')">
        <xsl:for-each select="xalan:nodeset($parameters)/xscript:parameter">
          <xsp:logic>
            <xsl:value-of select="$params"/>.setParameter(
              "<xsl:value-of select="@name"/>",
              "<xsl:value-of select="."/>");
          </xsp:logic>
        </xsl:for-each>
      </xsl:when>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'saxon')">
        <xsl:for-each select="saxon:node-set($parameters)/xscript:parameter">
          <xsp:logic>
            <xsl:value-of select="$params"/>.setParameter(
            "<xsl:value-of select="./@name"/>",
              "<xsl:value-of select="."/>");
          </xsp:logic>
        </xsl:for-each>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

