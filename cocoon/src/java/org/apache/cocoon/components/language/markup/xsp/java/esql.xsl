<?xml version="1.0"?>
<!-- $Id: esql.xsl,v 1.4 2002/01/28 09:37:16 tcurdt Exp $-->
<!--

 ============================================================================
                   The Apache Software License, Version 1.2
 ============================================================================

 Copyright (C) 1999-2002 The Apache Software Foundation. All rights reserved.

 Redistribution and use in source and binary forms, with or without modifica-
 tion, are permitted provided that the following conditions are met:

 1. Redistributions of  source code must  retain the above copyright  notice,
    this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. The end-user documentation included with the redistribution, if any, must
    include  the following  acknowledgment:  "This product includes  software
    developed  by the  Apache Software Foundation  (http://www.apache.org/)."
    Alternately, this  acknowledgment may  appear in the software itself,  if
    and wherever such third-party acknowledgments normally appear.

 4. The names "Cocoon" and  "Apache Software Foundation"  must not be used to
    endorse  or promote  products derived  from this  software without  prior
    written permission. For written permission, please contact
    apache@apache.org.

 5. Products  derived from this software may not  be called "Apache", nor may
    "Apache" appear  in their name,  without prior written permission  of the
    Apache Software Foundation.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
 APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
 DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
 ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
 (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 This software  consists of voluntary contributions made  by many individuals
 on  behalf of the Apache Software  Foundation and was  originally created by
 Stefano Mazzocchi  <stefano@apache.org>. For more  information on the Apache
 Software Foundation, please see <http://www.apache.org/>.

-->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsp="http://apache.org/xsp"
  xmlns:esql="http://apache.org/cocoon/SQL/v2"
  xmlns:xspdoc="http://apache.org/cocoon/XSPDoc/v1"
>

<xsl:param name="XSP-ENVIRONMENT"/>
<xsl:param name="XSP-VERSION"/>
<xsl:param name="filename"/>
<xsl:param name="language"/>

<xsl:variable name="cocoon1-environment">Cocoon 1</xsl:variable>
<xsl:variable name="cocoon2-environment">Cocoon 2</xsl:variable>

<xsl:variable name="cocoon1-xsp-namespace-uri">http://www.apache.org/1999/XSP/Core</xsl:variable>
<xsl:variable name="cocoon2-xsp-namespace-uri">http://apache.org/xsp</xsl:variable>

<xsl:variable name="prefix">esql</xsl:variable>

<xsl:variable name="environment">
  <xsl:choose>
    <xsl:when test="starts-with($XSP-ENVIRONMENT,$cocoon1-environment)">
      <xsl:text>Cocoon1</xsl:text>
    </xsl:when>
    <xsl:when test="starts-with($XSP-ENVIRONMENT,$cocoon2-environment)">
      <xsl:text>Cocoon2</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>Cocoon2</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="xsp-namespace-uri">
  <xsl:choose>
    <xsl:when test="$environment = 'Cocoon1'">
      <xsl:value-of select="$cocoon1-xsp-namespace-uri"/>
    </xsl:when>
    <xsl:when test="$environment = 'Cocoon2'">
      <xsl:value-of select="$cocoon2-xsp-namespace-uri"/>
    </xsl:when>
  </xsl:choose>
</xsl:variable>

<xsl:template name="get-nested-content">
  <xsl:param name="content"/>
  <xsl:choose>
    <xsl:when test="$content/*">
      <xsl:apply-templates select="$content/*"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$content"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get-nested-string">
  <xsl:param name="content"/>
  <xsl:choose>
    <xsl:when test="$environment = 'Cocoon1'">
      <xsl:choose>
        <xsl:when test="$content/*">
          ""
          <xsl:for-each select="$content/node()">
            <xsl:choose>
              <xsl:when test="name(.)">
                + <xsl:apply-templates select="."/>
              </xsl:when>
              <xsl:otherwise>
                + "<xsl:value-of select="translate(.,'&#9;&#10;&#13;','   ')"/>"
              </xsl:otherwise>
            </xsl:choose>
          </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>
          "<xsl:value-of select="normalize-space($content)"/>"
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$environment = 'Cocoon2'">
      <xsl:choose>
        <xsl:when test="$content/*">
          ""
          <xsl:for-each select="$content/node()">
            <xsl:choose>
              <xsl:when test="name(.)">
                <xsl:choose>
                  <xsl:when test="namespace-uri(.)='http://apache.org/xsp' and local-name(.)='text'">
                    + "<xsl:value-of select="."/>"
                  </xsl:when>
                  <xsl:otherwise>
                    + <xsl:apply-templates select="."/>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:when>
              <xsl:otherwise>
                + "<xsl:value-of select="translate(.,'&#9;&#10;&#13;','   ')"/>"
              </xsl:otherwise>
            </xsl:choose>
          </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>"<xsl:value-of select="normalize-space($content)"/>"</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
  </xsl:choose>
</xsl:template>


  <xsl:template name="get-parameter">
    <xsl:param name="name"/>
    <xsl:param name="default"/>
    <xsl:param name="required">false</xsl:param>

    <xsl:variable name="qname">
      <xsl:value-of select="concat($prefix, ':param')"/>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="@*[name(.) = $name]">"<xsl:value-of select="@*[name(.) = $name]"/>"</xsl:when>
      <xsl:when test="(*[name(.) = $qname])[@name = $name]">
        <xsl:call-template name="get-nested-content">
          <xsl:with-param name="content"
                          select="(*[name(.) = $qname])[@name = $name]"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="string-length($default) = 0">
            <xsl:choose>
              <xsl:when test="$required = 'true'">
                <xsl:call-template name="error">
                  <xsl:with-param name="message">[Logicsheet processor]
Parameter '<xsl:value-of select="$name"/>' missing in dynamic tag &lt;<xsl:value-of select="name(.)"/>&gt;
                  </xsl:with-param>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>""</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise><xsl:copy-of select="$default"/></xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<xsl:template match="xsp:page">
  <xsp:page>
    <xsl:apply-templates select="@*"/>
    <xsp:structure>
      <xsp:include>java.sql.DriverManager</xsp:include>
      <xsp:include>java.sql.Connection</xsp:include>
      <xsp:include>java.sql.Statement</xsp:include>
      <xsp:include>java.sql.PreparedStatement</xsp:include>
      <xsp:include>java.sql.ResultSet</xsp:include>
      <xsp:include>java.sql.ResultSetMetaData</xsp:include>
      <xsp:include>java.sql.Struct</xsp:include>
      <xsp:include>java.sql.SQLException</xsp:include>
      <xsp:include>java.sql.Clob</xsp:include>
      <xsp:include>java.text.SimpleDateFormat</xsp:include>
      <xsp:include>java.text.DecimalFormat</xsp:include>
      <xsp:include>java.io.StringWriter</xsp:include>
      <xsp:include>java.io.PrintWriter</xsp:include>
      <xsp:include>java.io.BufferedInputStream</xsp:include>
      <xsp:include>java.io.InputStream</xsp:include>
      <xsp:include>java.util.Set</xsp:include>
      <xsp:include>java.util.List</xsp:include>
      <xsp:include>java.util.Iterator</xsp:include>
      <xsp:include>java.util.ListIterator</xsp:include>
      <xsp:include>java.sql.Struct</xsp:include>
      <xsp:include>java.sql.Types</xsp:include>
      <xsl:choose>
         <xsl:when test="$environment = 'Cocoon1'">
             <xsp:include>org.apache.cocoon.processor.xsp.library.sql.EsqlHelper</xsp:include>
             <xsp:include>org.apache.cocoon.processor.xsp.library.sql.EsqlQuery</xsp:include>
             <xsp:include>org.apache.cocoon.processor.xsp.library.sql.EsqlConnection</xsp:include>
         </xsl:when>
         <xsl:when test="$environment = 'Cocoon2'">
             <xsp:include>org.apache.cocoon.components.language.markup.xsp.EsqlHelper</xsp:include>
             <xsp:include>org.apache.cocoon.components.language.markup.xsp.EsqlQuery</xsp:include>
             <xsp:include>org.apache.cocoon.components.language.markup.xsp.EsqlConnection</xsp:include>
             <xsp:include>org.apache.cocoon.components.language.markup.xsp.EsqlConnectionCocoon2</xsp:include>
             <xsp:include>org.apache.cocoon.components.language.markup.xsp.XSPUtil</xsp:include>
         </xsl:when>
      </xsl:choose>
      <xsl:if test=".//esql:connection/esql:pool">
        <xsl:choose>
          <xsl:when test="$environment = 'Cocoon1'">
            <xsp:include>org.apache.turbine.services.db.PoolBrokerService</xsp:include>
            <xsp:include>org.apache.turbine.util.db.pool.DBConnection</xsp:include>
          </xsl:when>
          <xsl:when test="$environment = 'Cocoon2'">
            <xsp:include>org.apache.avalon.excalibur.datasource.DataSourceComponent</xsp:include>
          </xsl:when>
        </xsl:choose>
      </xsl:if>
    </xsp:structure>
    <xsp:logic>
      private Stack _esql_connections = new Stack();
      private EsqlConnection<xsl:value-of select="$environment"/> _esql_connection = null;
      private Stack _esql_queries = new Stack();
      private EsqlQuery _esql_query = null;
      private SQLException _esql_exception = null;
      private StringWriter _esql_exception_writer = null;

      <xsl:if test=".//esql:connection/esql:pool">
        <xsl:choose>
          <xsl:when test="$environment = 'Cocoon1'">
            static PoolBrokerService _esql_pool = PoolBrokerService.getInstance();
          </xsl:when>
          <xsl:when test="$environment = 'Cocoon2'">
            private static ComponentSelector _esql_selector = null;

            public void compose(ComponentManager manager) throws org.apache.avalon.framework.component.ComponentException {
              super.compose(manager);
              if (_esql_selector == null) {
                try {
                  _esql_selector = (ComponentSelector) manager.lookup(DataSourceComponent.ROLE + "Selector");
                } catch (ComponentException cme) {
                  getLogger().error("Could not look up the datasource component", cme);
                }
              }
            }
          </xsl:when>
        </xsl:choose>
      </xsl:if>

      <xsl:choose>
        <xsl:when test="$environment = 'Cocoon1'">
        </xsl:when>
        <xsl:when test="$environment = 'Cocoon2'">
            protected void _esql_printObject ( Object obj, AttributesImpl xspAttr) throws SAXException
            {
               if ( obj instanceof List) {
           ListIterator j=((List)obj).listIterator();
           <xsp:element name="sql-list">
                     <xsp:logic>
                       while (j.hasNext()){
                      <xsp:element name="sql-list-item">
                        <xsp:attribute name="pos"><xsp:expr>j.nextIndex()</xsp:expr></xsp:attribute>
                        <xsp:logic>this._esql_printObject(j.next(),xspAttr);</xsp:logic>
                      </xsp:element>
                       };
                     </xsp:logic>
                   </xsp:element>
               } else if ( obj instanceof Set ) {
            Iterator j=((Set)obj).iterator();
            <xsp:element name="sql-set">
                      <xsp:logic>
                        while (j.hasNext()){
                           <xsp:element name="sql-set-item">
                 <xsp:logic>this._esql_printObject(j.next(),xspAttr);</xsp:logic>
                   </xsp:element>
                        };
                      </xsp:logic>
                    </xsp:element>
           } else {
              <xsp:content><xsp:expr>obj</xsp:expr></xsp:content>;
           }
        }
        </xsl:when>
      </xsl:choose>
    </xsp:logic>
    <xsl:apply-templates/>
  </xsp:page>
</xsl:template>

<xsl:template match="xsp:page/*[not(self::xsp:*)]">
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsp:logic>
    </xsp:logic>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="esql:connection">
  <xsl:variable name="driver"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:driver"/></xsl:call-template></xsl:variable>
  <xsl:variable name="dburl"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:dburl"/></xsl:call-template></xsl:variable>
  <xsl:variable name="username"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:username"/></xsl:call-template></xsl:variable>
  <xsl:variable name="password"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:password"/></xsl:call-template></xsl:variable>
  <xsl:variable name="pool"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:pool"/></xsl:call-template></xsl:variable>
  <xsl:variable name="autocommit"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:autocommit"/></xsl:call-template></xsl:variable>
  <xsl:variable name="use-limit-clause"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:use-limit-clause"/></xsl:call-template></xsl:variable>
  <xsp:logic>
    if (_esql_connection != null) {
      _esql_connections.push(_esql_connection);
    }
    _esql_connection = new EsqlConnection<xsl:value-of select="$environment"/>();
    try {
      <xsl:choose>
        <xsl:when test="esql:pool and $environment = 'Cocoon1'">
          try {
            _esql_connection.db_connection = _esql_pool.getConnection(String.valueOf(<xsl:copy-of select="$pool"/>));
            _esql_connection.connection = _esql_connection.db_connection.getConnection();
          } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error opening pooled connection: "+String.valueOf(<xsl:copy-of select="$pool"/>)+": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>.getMessage());
          }
          if (_esql_connection.connection == null) {
            throw new RuntimeException("Could not open pooled connection: "+String.valueOf(<xsl:copy-of select="$pool"/>));
          }
        </xsl:when>
        <xsl:when test="esql:pool and $environment = 'Cocoon2'">
          try {
            _esql_connection.datasource = (DataSourceComponent) _esql_selector.select(String.valueOf(<xsl:copy-of select="$pool"/>));
            _esql_connection.connection = _esql_connection.datasource.getConnection();
          } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            getLogger().error("Could not get the datasource",_esql_exception_<xsl:value-of select="generate-id(.)"/>);
            throw new RuntimeException("Could not get the datasource "+_esql_exception_<xsl:value-of select="generate-id(.)"/>);
          }
        </xsl:when>
        <xsl:otherwise>
          <xsl:if test="esql:driver">
          try {
            Thread.currentThread().getContextClassLoader().loadClass(String.valueOf(<xsl:copy-of select="$driver"/>)).newInstance();
          } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error loading driver: "+String.valueOf(<xsl:copy-of select="$driver"/>));
          }
          </xsl:if>
          try {
            _esql_connection.setUrl(String.valueOf(<xsl:copy-of select="$dburl"/>));
            <xsl:if test="esql:username">
              _esql_connection.setUser(String.valueOf(<xsl:copy-of select="$username"/>));
            </xsl:if>
            <xsl:if test="esql:password">
              _esql_connection.setPassword(String.valueOf(<xsl:copy-of select="$password"/>));
            </xsl:if>
            <xsl:for-each select="esql:property">
              _esql_connection.setProperty("<xsl:value-of select="@name"/>",<xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="."/></xsl:call-template>);
            </xsl:for-each>
            _esql_connection.connection = DriverManager.getConnection(_esql_connection.getUrl(), _esql_connection.getInfo());
          } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error opening connection to dburl: "+String.valueOf(<xsl:copy-of select="$dburl"/>)+": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>.getMessage());
          }
        </xsl:otherwise>
      </xsl:choose>
      try {
        if ("false".equals(String.valueOf(<xsl:copy-of select="$autocommit"/>))) {
          if (_esql_connection.getAutoCommit()) {
            _esql_connection.setAutoCommit(false);
          }
        } else {
          if (!_esql_connection.getAutoCommit()) {
            _esql_connection.setAutoCommit(true);
          }
        }
      } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
        // do NOT: throw new RuntimeException("Error setting connection autocommit");
      }
      <xsl:if test="esql:use-limit-clause">
        _esql_connection.setLimitMethod(String.valueOf(<xsl:copy-of select="$use-limit-clause"/>));
      </xsl:if>
      <xsl:apply-templates/>
    } finally {
      try {
        if(!_esql_connection.getAutoCommit()) {
          _esql_connection.commit();
        }
        <xsl:choose>
          <xsl:when test="esql:pool and $environment = 'Cocoon1'">
            _esql_pool.releaseConnection(_esql_connection.db_connection);
          </xsl:when>
          <xsl:otherwise>
            _esql_connection.close();
          </xsl:otherwise>
        </xsl:choose>
        if (_esql_connections.empty()) {
          _esql_connection = null;
        } else {
          _esql_connection = (EsqlConnection<xsl:value-of select="$environment"/>)_esql_connections.pop();
        }
      } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>) {}
    }
  </xsp:logic>
</xsl:template>

<xsl:template match="esql:connection/esql:driver"/>
<xsl:template match="esql:connection/esql:dburl"/>
<xsl:template match="esql:connection/esql:username"/>
<xsl:template match="esql:connection/esql:password"/>
<xsl:template match="esql:connection/esql:pool"/>
<xsl:template match="esql:connection/esql:autocommit"/>
<xsl:template match="esql:connection/esql:use-limit-clause"/>
<xsl:template match="esql:connection/esql:property"/>

<xsl:template match="esql:connection//esql:execute-query">
  <xsl:variable name="query"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:query"/></xsl:call-template></xsl:variable>
  <xsl:variable name="maxrows"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:max-rows"/></xsl:call-template></xsl:variable>
  <xsl:variable name="skiprows"><xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="esql:skip-rows"/></xsl:call-template></xsl:variable>
  <xsp:logic>
    if (_esql_query != null) {
      _esql_queries.push(_esql_query);
    }
    _esql_query = new EsqlQuery( _esql_connection, String.valueOf(<xsl:copy-of select="$query"/>) );

    <xsl:if test="esql:max-rows">
      try {
        _esql_query.setMaxRows( Integer.parseInt(String.valueOf(<xsl:copy-of select="$maxrows"/>)) );
      } catch (NumberFormatException e) {}
    </xsl:if>

    <xsl:if test="esql:skip-rows">
      try {
        _esql_query.setSkipRows( Integer.parseInt(String.valueOf(<xsl:copy-of select="$skiprows"/>)) );
      } catch (NumberFormatException e) {}
    </xsl:if>

    try {
      <xsl:choose>
        <xsl:when test="esql:query//esql:parameter">
          try {
            _esql_query.prepareStatement();
          } catch (SQLException _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error preparing statement: " + _esql_query.getQueryString() + ": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>.getMessage());
          }
          <xsl:for-each select="esql:query//esql:parameter">
            try {
              <xsl:text>_esql_query.getPreparedStatement().</xsl:text>
              <xsl:choose>
                <xsl:when test="@type">
                  <xsl:variable name="type"><xsl:value-of select="concat(translate(substring(@type,1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(@type,2))"/></xsl:variable>
                  <xsl:text>set</xsl:text><xsl:value-of select="$type"/>(<xsl:value-of select="position()"/>,<xsl:call-template name="get-nested-content"><xsl:with-param name="content" select="."/></xsl:call-template>);<xsl:text>
</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>setString(</xsl:text><xsl:value-of select="position()"/>,String.valueOf(<xsl:call-template name="get-nested-string"><xsl:with-param name="content" select="."/></xsl:call-template>));<xsl:text>
</xsl:text>
                </xsl:otherwise>
              </xsl:choose>
            } catch (SQLException _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
              throw new RuntimeException("Error setting parameter on statement: " + _esql_query.getQueryString() + ": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>);
            }
          </xsl:for-each>
          try {
            _esql_query.execute();
          } catch (SQLException _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error executing prepared statement: " + _esql_query.getQueryString() + ": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>);
          }
        </xsl:when>
        <xsl:otherwise>
          _esql_query.createStatement();
          try {
            _esql_query.execute();
          } catch (SQLException _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
            throw new RuntimeException("Error executing statement: " + _esql_query.getQueryString() + ": "+_esql_exception_<xsl:value-of select="generate-id(.)"/>);
          }
        </xsl:otherwise>
      </xsl:choose>
      <xsl:choose>
        <xsl:when test="$environment = 'Cocoon1'">
          System.err.println("QUERY: " + _esql_query.getQueryString());
        </xsl:when>
        <xsl:otherwise>
          getLogger().debug("esql query: " + _esql_query.getQueryString());
        </xsl:otherwise>
      </xsl:choose>
      if (_esql_query.hasResultSet()) {
        do {
          _esql_query.getResultRows();

          if (_esql_query.nextRow()) {
            <xsl:apply-templates select="esql:results"/>
          }
          else {
            <xsl:apply-templates select="esql:no-results"/>
          }
          _esql_query.getResultSet().close();

        } while(_esql_query.getMoreResults());
      } else {
        int _esql_update_count = _esql_query.getStatement().getUpdateCount();
        if (_esql_update_count &gt;= 0) {
          <xsl:apply-templates select="esql:update-results/*"/>
        }
        else{
          <xsl:apply-templates select="esql:no-results"/>
        }
      }
      _esql_query.getStatement().close();
    } catch (SQLException _esql_exception_<xsl:value-of select="generate-id(.)"/>) {
      try {
        <xsl:choose>
          <xsl:when test="esql:error-results">
            _esql_exception = _esql_exception_<xsl:value-of select="generate-id(.)"/>;
            _esql_exception_writer = new StringWriter();
            _esql_exception.printStackTrace(new PrintWriter(_esql_exception_writer));
            <xsl:apply-templates select="esql:error-results"/>
            if (!_esql_connection.getAutoCommit()) {
              _esql_connection.rollback();
            }
          </xsl:when>
          <xsl:otherwise>
            if (!_esql_connection.getAutoCommit()) {
              _esql_connection.rollback();
            }
          </xsl:otherwise>
        </xsl:choose>
      } catch (Exception _esql_exception_<xsl:value-of select="generate-id(.)"/>_2) {}
    }
    if (_esql_queries.empty()) {
      _esql_query = null;
    } else {
      _esql_query = (EsqlQuery)_esql_queries.pop();
    }
  </xsp:logic>
</xsl:template>

<xsl:template match="esql:query//esql:parameter">"?"</xsl:template>

<xsl:template match="esql:execute-query//esql:results//esql:row-count">
  <xsp:expr>_esql_query.rowCount()</xsp:expr>
</xsl:template>

<xsl:template match="esql:execute-query//esql:results">
  <xsp:content>
    <xsl:apply-templates/>
  </xsp:content>
</xsl:template>

<xsl:template match="esql:execute-query//esql:error-results">
  <xsp:content>
    <xsl:apply-templates/>
  </xsp:content>
</xsl:template>

<xsl:template match="esql:execute-query//esql:no-results">
  <xsp:content>
    <xsl:apply-templates/>
  </xsp:content>
</xsl:template>

<xsl:template match="esql:update-results//esql:get-update-count">
  <xsp:expr>_esql_update_count</xsp:expr>
</xsl:template>

<xsl:template match="esql:results//esql:row-results">
  <xsl:variable name="group" select=".//esql:group"/>
  <xsp:logic>

    //create booleans for group change watches and strings for old values.
    <xsl:apply-templates select=".//esql:group" mode="vars"/>

    do {
      <xsp:content>
      <xsl:apply-templates/>
      </xsp:content>
      <xsl:if test="count($group) &lt; 1">	
        <xsl:call-template name="nextRow"/>
      </xsl:if>
    } while ( _esql_query.keepGoing() );

    if (_esql_query.getSkipRows() > 0 ) {
        <xsl:apply-templates select="ancestor::esql:results//esql:previous-results" mode="more"/>
    }

    if (_esql_query.nextRow()) {
        <xsl:apply-templates select="ancestor::esql:results//esql:more-results" mode="more"/>
    }
  </xsp:logic>
</xsl:template>

<xsl:template name="nextRow">
  //checking out early?
    if (_esql_query.getMaxRows() != -1 &amp;&amp; _esql_query.getCurrentRow() - _esql_query.getSkipRows() == _esql_query.getMaxRows()) {
      _esql_query.setKeepGoing( false );
    } else {	//if not, advance normally
      _esql_query.setKeepGoing( _esql_query.nextRow() );
    }
</xsl:template>

<xsl:template match="esql:results//esql:previous-results"/>

<xsl:template match="esql:results//esql:previous-results" mode="more">
  <xsp:content>
    <xsl:apply-templates/>
  </xsp:content>
</xsl:template>

<xsl:template match="esql:group" mode="vars">
  _esql_query.setGroupingVar("<xsl:value-of select="@group-on"/>Changed", new Boolean(true));
</xsl:template>

<xspdoc:desc>Allows header and footer elements around groups of consecutive records with identical values in column named by @group-on.  Facilitates a single query with joins to be used in lieu of some nested queries.</xspdoc:desc>
<xsl:template match="esql:group|esql:group//esql:group[.//esql:member]" priority="3">
<xsp:logic>
  if (((Boolean)_esql_query.getGroupingVar("<xsl:value-of select="@group-on"/>Changed")).booleanValue()){	
    //header contents
    <xsp:content>
      <xsl:apply-templates>
        <xsl:with-param name="group-on" select="@group-on"/>
      </xsl:apply-templates>
    </xsp:content>
  }
</xsp:logic>
</xsl:template>

<xsl:template match="esql:group//node()[.//esql:member]">
  <xsl:param name="group-on"/>
  <xsl:copy>
    <xsl:apply-templates select="@*|*|text()">
      <xsl:with-param name="group-on" select="$group-on"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xspdoc:desc>Used in conjunction with and nested inside esql:group.  Formatting for individual records goes within esql:member. Header and footer stuff goes in between group and member.</xspdoc:desc>
<xsl:template match="esql:member|esql:group//esql:member[.//esql:member]">
  <xsl:param name="group-on"/>
  <xsl:variable name="group" select=".//esql:group"/>
  <xsp:logic>
  }
    _esql_query.setGroupingVar("<xsl:value-of select="$group-on"/>Old", _esql_query.getResultSet().getString("<xsl:value-of select="$group-on"/>"));	
    <xsp:content>
      <xsl:apply-templates>
        <xsl:with-param name="group-on" select="$group-on"/>
      </xsl:apply-templates>
    </xsp:content>

    <xsl:if test="count($group) &lt; 1">	
      <xsl:call-template name="nextRow"/>
    </xsl:if>
  if ( _esql_query.keepGoing() ) {
    _esql_query.setGroupingVar("<xsl:value-of select="$group-on"/>Changed", new Boolean(!((String)_esql_query.getGroupingVar("<xsl:value-of select="$group-on"/>Old")).equals(_esql_query.getResultSet().getString("<xsl:value-of select="$group-on"/>"))));
  } else {
    _esql_query.setGroupingVar("<xsl:value-of select="$group-on"/>Changed", new Boolean(true));
  }
  if (((Boolean)_esql_query.getGroupingVar("<xsl:value-of select="$group-on"/>Changed")).booleanValue()) {	
    //footer contents
  </xsp:logic>
</xsl:template>

<xsl:template match="esql:results//esql:more-results"/>

<xsl:template match="esql:results//esql:more-results" mode="more">
  <xsp:content>
    <xsl:apply-templates/>
  </xsp:content>
</xsl:template>

<xspdoc:desc>results in a set of elements whose names are the names of the columns. the elements each have one text child, whose value is the value of the column interpreted as a string. No special formatting is allowed here. If you want to mess around with the names of the elements or the value of the text field, use the type-specific get methods and write out the result fragment yourself.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-columns">
  <xsl:variable name="tagcase"><xsl:value-of select="@tag-case"/></xsl:variable>
  <xsl:choose>
    <xsl:when test="$environment = 'Cocoon1'">
      <xsp:logic>
        for (int _esql_i=1; _esql_i &lt;= _esql_query.getResultSetMetaData().getColumnCount(); _esql_i++) {
          Node _esql_node = document.createElement(
            <xsl:choose>
              <xsl:when test="$tagcase='lower'">
                _esql_query.getResultSetMetaData().getColumnName(_esql_i).toLowerCase()
              </xsl:when>
              <xsl:when test="$tagcase='upper'">
                _esql_query.getResultSetMetaData().getColumnName(_esql_i).toUpperCase()
              </xsl:when>
              <xsl:otherwise>
                _esql_query.getResultSetMetaData().getColumnName(_esql_i)
              </xsl:otherwise>
            </xsl:choose>
          );
          _esql_node.appendChild(document.createTextNode(
            <xsl:call-template name="get-string-encoded">
              <xsl:with-param name="column-spec">_esql_i</xsl:with-param>
              <xsl:with-param name="resultset">_esql_query.getResultSet()</xsl:with-param>
            </xsl:call-template>
          ));
          xspCurrentNode.appendChild(_esql_node);
        }
      </xsp:logic>
    </xsl:when>
    <xsl:when test="$environment = 'Cocoon2'">
      <xsp:logic>
        for (int _esql_i = 1; _esql_i &lt;= _esql_query.getResultSetMetaData().getColumnCount(); _esql_i++) {
          String _esql_tagname = _esql_query.getResultSetMetaData().getColumnName(_esql_i);
          <xsp:element>
            <xsp:param name="name">
              <xsl:choose>
                <xsl:when test="$tagcase='lower'">
                  <xsp:expr>_esql_tagname.toLowerCase()</xsp:expr>
                </xsl:when>
                <xsl:when test="$tagcase='upper'">
                  <xsp:expr>_esql_tagname.toUpperCase()</xsp:expr>
                </xsl:when>
                <xsl:otherwise>
                  <xsp:expr>_esql_tagname</xsp:expr>
                </xsl:otherwise>
              </xsl:choose>
            </xsp:param>
            <xsp:logic>
              switch(_esql_query.getResultSet().getMetaData().getColumnType(_esql_i)){
                 case java.sql.Types.ARRAY:
                 case java.sql.Types.STRUCT:
                    <xsp:element name="sql-row">
                      <xsp:logic>
                        Object[] _esql_struct = ((Struct) _esql_query.getResultSet().getObject(_esql_i)).getAttributes();
                        for ( int _esql_k=0; _esql_k&lt;_esql_struct.length; _esql_k++){
                        <xsp:element name="sql-row-item"><xsp:logic>this._esql_printObject(_esql_struct[_esql_k],xspAttr);</xsp:logic></xsp:element>
                        }
                      </xsp:logic>
                    </xsp:element>
                    break;

                 case java.sql.Types.OTHER: // This is what Informix uses for Sets, Bags, Lists
                    this._esql_printObject(_esql_query.getResultSet().getObject(_esql_i), xspAttr);
                    break;

                 default:
                    // standard type
                    <xsp:content>
                    <xsp:expr>
                      <xsl:call-template name="get-string-encoded">
                        <xsl:with-param name="column-spec">_esql_i</xsl:with-param>
                        <xsl:with-param name="resultset">_esql_query.getResultSet()</xsl:with-param>
                      </xsl:call-template>
                    </xsp:expr></xsp:content>
              }
            </xsp:logic>
          </xsp:element>
        }
        this.characters("\n");
      </xsp:logic>
    </xsl:when>
    <xsl:otherwise>
      <xsp:logic>
        throw new RuntimeException("esql:get-columns is not supported in this environment: "+<xsl:value-of select="$environment"/>);
      </xsp:logic>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a string</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-string" name="get-string">
  <xsp:expr>
    <xsl:call-template name="get-string-encoded">
      <xsl:with-param name="column-spec"><xsl:call-template name="get-column"/></xsl:with-param>
      <xsl:with-param name="resultset"><xsl:call-template name="get-resultset"/></xsl:with-param>
    </xsl:call-template>
  </xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a date. if a format attribute exists, its value is taken to be a date format string as defined in java.text.SimpleDateFormat, and the result is formatted accordingly.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-date">
  <xsl:choose>
    <xsl:when test="@format">
      <xsp:expr><xsl:call-template name="get-resultset"/>.getDate(<xsl:call-template name="get-column"/>) == null ? "" : new SimpleDateFormat("<xsl:value-of select="@format"/>").format(<xsl:call-template name="get-resultset"/>.getDate(<xsl:call-template name="get-column"/>))</xsp:expr>
    </xsl:when>
    <xsl:otherwise>
      <xsp:expr><xsl:call-template name="get-resultset"/>.getDate(<xsl:call-template name="get-column"/>)</xsp:expr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a time. if a format attribute exists, its value is taken to be a date format string as defined in java.text.SimpleDateFormat, and the result is formatted accordingly.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-time">
  <xsl:choose>
    <xsl:when test="@format">
      <xsp:expr><xsl:call-template name="get-resultset"/>.getTime(<xsl:call-template name="get-column"/>) == null ? "" : new SimpleDateFormat("<xsl:value-of select="@format"/>").format(<xsl:call-template name="get-resultset"/>.getTime(<xsl:call-template name="get-column"/>))</xsp:expr>
    </xsl:when>
    <xsl:otherwise>
      <xsp:expr><xsl:call-template name="get-resultset"/>.getTime(<xsl:call-template name="get-column"/>)</xsp:expr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a timestamp. if a format attribute exists, its value is taken to be a date format string as defined in java.text.SimpleDateFormat, and the result is formatted accordingly.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-timestamp">
  <xsl:choose>
    <xsl:when test="@format">
      <xsp:expr><xsl:call-template name="get-resultset"/>.getTimestamp(<xsl:call-template name="get-column"/>) == null ? "" : new SimpleDateFormat("<xsl:value-of select="@format"/>").format(<xsl:call-template name="get-resultset"/>.getTimestamp(<xsl:call-template name="get-column"/>))</xsp:expr>
    </xsl:when>
    <xsl:otherwise>
      <xsp:expr><xsl:call-template name="get-resultset"/>.getTimestamp(<xsl:call-template name="get-column"/>)</xsp:expr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the value of the given column as true or false</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-boolean">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getBoolean(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a double. if a format attribute exists, its value is taken to be a decimal format string as defined in java.text.DecimalFormat, and the result is formatted accordingly.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-double">
  <xsl:choose>
    <xsl:when test="@format">
      <xsp:expr>new DecimalFormat("<xsl:value-of select="@format"/>").format(new Double(<xsl:call-template name="get-resultset"/>.getDouble(<xsl:call-template name="get-column"/>)))</xsp:expr>
    </xsl:when>
    <xsl:otherwise>
      <xsp:expr><xsl:call-template name="get-resultset"/>.getDouble(<xsl:call-template name="get-column"/>)</xsp:expr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a float. if a format attribute exists, its value is taken to be a decimal format string as defined in java.text.DecimalFormat, and the result is formatted accordingly.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-float">
  <xsl:choose>
    <xsl:when test="@format">
      <xsp:expr>new DecimalFormat("<xsl:value-of select="@format"/>").format(new Float(<xsl:call-template name="get-resultset"/>.getFloat(<xsl:call-template name="get-column"/>)))</xsp:expr>
    </xsl:when>
    <xsl:otherwise>
      <xsp:expr><xsl:call-template name="get-resultset"/>.getFloat(<xsl:call-template name="get-column"/>)</xsp:expr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the current result set</xspdoc:desc>
<xsl:template match="esql:results//esql:get-resultset">
  <xsp:expr><xsl:call-template name="get-resultset"/></xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as an object</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-object">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getObject(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as an array</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-array">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getArray(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a struct</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-struct">
  <xsp:expr>(Struct) <xsl:call-template name="get-resultset"/>.getObject(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as an integer</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-int">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getInt(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a long</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-long">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getLong(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a short</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-short">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getShort(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the value of the given column as a clob</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-ascii">
  <xsp:expr>EsqlHelper.getAscii(<xsl:call-template name="get-resultset"/>, <xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

 <xspdoc:desc>returns the value of the given column interpeted as an xml fragment.
 The fragment is parsed by the default xsp parser and the document element is returned.
 If a root attribute exists, its value is taken to be the name of an element to wrap around the contents of
 the fragment before parsing.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-xml">
  <xsl:variable name="content">
    <xsl:choose>
      <xsl:when test="@root">
        <xsl:text>"&lt;</xsl:text>
        <xsl:value-of select="@root"/>
        <xsl:text>&gt;"+</xsl:text>
        <xsl:call-template name="get-string"/>
        <xsl:text>+"&lt;/</xsl:text>
        <xsl:value-of select="@root"/>
        <xsl:text>&gt;"</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get-string"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$environment = 'Cocoon1'">
      <xsl:choose>
        <xsl:when test="../esql:row-results">
          <xsp:logic>
            xspCurrentNode.appendChild(this.xspParser.parse(new InputSource(new StringReader(<xsl:copy-of select="$content"/>))).getDocumentElement();
          </xsp:logic>
        </xsl:when>
        <xsl:otherwise>
          <xsp:expr>this.xspParser.parse(new InputSource(new StringReader(<xsl:copy-of select="$content"/>))).getDocumentElement()</xsp:expr>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$environment = 'Cocoon2'">
      <xsp:logic>
      {
          org.apache.cocoon.components.parser.Parser newParser = null;

          try {
              newParser = (org.apache.cocoon.components.parser.Parser) this.manager.lookup(org.apache.cocoon.components.parser.Parser.ROLE);

              InputSource __is = new InputSource(
                      new StringReader(
                          String.valueOf(<xsl:copy-of select="$content"/>)
                      )
                  );


              XSPUtil.include(__is, this.contentHandler, newParser);
          } catch (Exception e) {
              getLogger().error("Could not include page", e);
          } finally {
              if (newParser != null) this.manager.release((Component) newParser);
          }
      }
      </xsp:logic>
    </xsl:when>
    <xsl:otherwise>
      <xsp:logic>
        throw new RuntimeException("esql:get-xml is not supported in this environment: "+<xsl:value-of select="$environment"/>);
      </xsp:logic>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>returns the number of columns in the resultset.</xspdoc:desc>
<xsl:template match="esql:results//esql:get-column-count">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData().getColumnCount()</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the metadata of the resultset.</xspdoc:desc>
<xsl:template match="esql:results//esql:get-metadata">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData()</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the position of the current row in the result set</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-row-position|esql:results//esql:get-row-position">
  <xsp:expr>_esql_query.getCurrentRow()</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the name of the given column. the column mus tbe specified by number, not name.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-column-name">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData().getColumnName(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the label of the given column. the column mus tbe specified by number, not name.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-column-label">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData().getColumnLabel(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the name of the type of the given column. the column must be specified by number, not name.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-column-type-name">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData().getColumnTypeName(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the type of the given column as int. the column must be specified by number, not name.</xspdoc:desc>
<xsl:template match="esql:row-results//esql:get-column-type">
  <xsp:expr><xsl:call-template name="get-resultset"/>.getMetaData().getColumnType(<xsl:call-template name="get-column"/>)</xsp:expr>
</xsl:template>

<xspdoc:desc>allows null-column testing. Evaluates to a Java expression, which is true when the referred column contains a null-value for the current resultset row</xspdoc:desc>
<xsl:template match="esql:row-results//esql:is-null">
  <xsp:expr>((<xsl:call-template name="get-resultset"/>.getObject("<xsl:value-of select="@column"/>") == null) || <xsl:call-template name="get-resultset"/>.wasNull())</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the message of the current exception</xspdoc:desc>
<xsl:template match="esql:error-results//esql:get-message">
  <xsp:expr>_esql_exception.getMessage()</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the current exception as a string</xspdoc:desc>
<xsl:template match="esql:error-results//esql:to-string">
  <xsp:expr>_esql_exception.toString()</xsp:expr>
</xsl:template>

<xspdoc:desc>returns the stacktrace of the current exception</xspdoc:desc>
<xsl:template match="esql:error-results//esql:get-stacktrace">
  <xsp:expr>_esql_exception_writer.toString()</xsp:expr>
</xsl:template>

<xsl:template name="get-resultset">
  <xsl:call-template name="get-query"/><xsl:text>.getResultSet()</xsl:text>
</xsl:template>

<xsl:template name="get-query">
  <xsl:choose>
    <xsl:when test="@ancestor">
      <xsl:text>((EsqlQuery)_esql_queries.elementAt(_esql_queries.size()-</xsl:text>
      <xsl:value-of select="@ancestor"/>
      <xsl:text>))</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>_esql_query</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xspdoc:desc>used internally to determine which column is the given column. if a column attribute exists and its value is a number, it is taken to be the column's position. if the value is not a number, it is taken to be the column's name. if a column attribute does not exist, an esql:column element is assumed to exist and to render as a string (after all of the xsp instructions have been evaluated), which is taken to be the column's name.</xspdoc:desc>
<xsl:template name="get-column">
  <xsl:variable name="column">
     <xsl:call-template name="get-parameter">
       <xsl:with-param name="name">column</xsl:with-param>
       <xsl:with-param name="required">true</xsl:with-param>
     </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="starts-with($column,'&quot;')">
      <xsl:variable name="raw-column">
        <xsl:value-of select="substring($column,2,string-length($column)-2)"/>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="not(string(number($raw-column))='NaN')">
          <xsl:value-of select="$raw-column"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$column"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="$column"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get-string-encoded">
  <xsl:param name="column-spec"/>
  <xsl:param name="resultset"/>
  <xsl:variable name="encoding">
    <xsl:choose>
      <xsl:when test="@encoding">"<xsl:value-of select="@encoding"/>"</xsl:when>
      <xsl:when test="esql:encoding">
        <xsl:call-template name="get-nested-string">
          <xsl:with-param name="content" select="esql:encoding"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>default</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$encoding = 'default'">
      <xsl:value-of select="$resultset"/>.getString(<xsl:value-of select="$column-spec"/>)
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$resultset"/>.getBytes(<xsl:value-of select="$column-spec"/>)
      != null ? EsqlHelper.getStringFromByteArray(<xsl:value-of select="$resultset"/>.getBytes
        (<xsl:value-of select="$column-spec"/>), <xsl:value-of select="$encoding"/>)
      : ""
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!--
  Break on error.
  @param message explanation of the error
-->
<xsl:template name="error">
  <xsl:param name="message"/>
  <xsl:message terminate="yes"><xsl:value-of select="$message"/></xsl:message>
</xsl:template>

<xsl:template match="@*|node()" priority="-1">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
