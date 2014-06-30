<?xml version="1.0"?>
<!--
 *****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * _________________________________________________________________________ *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************
-->

<!-- Sitemap Core logicsheet for the Java language -->
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:map="http://apache.org/cocoon/sitemap/1.0"
    xmlns:XSLTFactoryLoader="org.apache.cocoon.sitemap.XSLTFactoryLoader"
    version="1.0">

  <xsl:output method="text"/>

  <xsl:variable name="ns">http://apache.org/cocoon/sitemap/1.0</xsl:variable>

  <!--
    this variable holds the factory loader used to get at the code
    matcher/selector factories are producing

    ovidiu: use the class name as the namespace to identify the
    class. This is supposedly portable across XSLT implementations.
  -->
  <xsl:variable
       name="factory-loader"
       select="XSLTFactoryLoader:new()"
       xmlns:XSLTFactoryLoader="org.apache.cocoon.sitemap.XSLTFactoryLoader"/>

  <!-- Define the XSLT processor. Currently only Saxon and Xalan are
  recognized. -->
  <xsl:variable name="xslt-processor">
    <xsl:choose>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'xalan')">xalan</xsl:when>
      <xsl:when test="contains(system-property('xsl:vendor-url'), 'saxon')">saxon</xsl:when>
      <xsl:otherwise>unknown</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:template name="line-number">
    <xsl:choose>
      <xsl:when test="$xslt-processor = 'saxon'"
                xmlns:saxon="http://icl.com/saxon">
// file <xsl:value-of select="saxon:system-id()"/>, line <xsl:value-of select="saxon:line-number()"/>
      </xsl:when>
      <xsl:otherwise>
//line numbers not supported with <xsl:value-of select="$xslt-processor"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- this template wraps the hole content within a single <code> element which
       the xsp core logicsheet uses to build the java source code out of it
  -->
  <xsl:template match="/">
    <!-- break on error when old parameter syntax exists -->
    <xsl:if test="//map:pipelines//parameter | //map:action-set//parameter | //map:resource//parameter">
      <xsl:call-template name="error">
        <xsl:with-param name="message">Sitemap parameters should now be in the sitemap namespace (map:parameter).</xsl:with-param>
      </xsl:call-template>
    </xsl:if>

    <!--
      direct -> generator, transformer, serializer, aggregator, reader
      support -> action, matcher, selector, mount, redirect, call
    -->

    <!-- check pipeline rules -->
    <xsl:for-each select="/map:sitemap/map:pipelines/map:pipeline">
      <xsl:variable name="pipeline" select="."/>

      <!-- break on error when found pipeline with no generate|aggregate|read|mount|redirect-to|call -->
      <xsl:if test="not(.//map:generate[not(ancestor::map:handle-errors)])
          and not(.//map:aggregate[not(ancestor::map:handle-errors)])
          and not(.//map:read[not(ancestor::map:handle-errors)])
          and not(.//map:mount[not(ancestor::map:handle-errors)])
          and not(.//map:redirect-to[not(ancestor::map:handle-errors)])
          and not(.//map:call[not(ancestor::map:handle-errors)])">
        <xsl:call-template name="error">
          <xsl:with-param name="message">Sitemap pipeline must have at least one of: map:generate, map:aggregate, map:read, map:mount, map:redirect-to, map:call.</xsl:with-param>
        </xsl:call-template>
      </xsl:if>

      <!-- break on error when pipeline with generate|aggregate|transform does not end with serializer|call -->
      <xsl:for-each select=".//*[namespace-uri()=$ns
          and (local-name()='generate' or local-name()='aggregate' or local-name()='transform')
          and not(ancestor::map:handle-errors)]">
        <xsl:if test="not(following::*[namespace-uri()=$ns
            and (local-name()='serialize' or local-name()='call')
            and not(ancestor::map:handle-errors)]/ancestor::map:pipeline[. = $pipeline])">
          <xsl:call-template name="error">
            <xsl:with-param name="message">Sitemap pipeline with map:generate|aggregate|transform must have map:serialize.</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:for-each>

      <!-- break on error when pipeline with serialize|read|mount|redirect-to have any sitemap component after that -->
      <xsl:if test=".//*[namespace-uri()=$ns
          and (local-name()='serialize' or local-name()='read' or local-name()='mount' or local-name()='redirect-to')
          ]/following-sibling::*[namespace-uri()=$ns and local-name() != 'handle-errors']">
        <xsl:call-template name="error">
          <xsl:with-param name="message">Sitemap pipeline with map:serialize|read|mount|redirect-to|call can not have any components after them.</xsl:with-param>
        </xsl:call-template>
      </xsl:if>

      <!-- break on error when match|select have no nested components
      <xsl:for-each select=".//*[namespace-uri()=$ns
          and (local-name()='match' or local-name()='when' or local-name='otherwise')]">
        <xsl:if test="not(descendant::*[namespace-uri()=$ns
            and (local-name()='generate' or local-name()='transform' or local-name()='serialize'
                 or local-name()='mount' or local-name()='read' or local-name()='redirect-to'
                 or local-name()='call')])">
          <xsl:call-template name="error">
            <xsl:with-param name="message">match and select must have nested components. Pattern: <xsl:value-of select="@pattern"/></xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:for-each>
       -->

      <!-- Handle-errors -->
      <xsl:for-each select="map:handle-errors">

        <!-- break on error when found handler with generate|mount -->
        <xsl:if test=".//map:generate or .//map:mount">
          <xsl:call-template name="error">
            <xsl:with-param name="message">Error handler can't have: map:generate, map:mount.</xsl:with-param>
          </xsl:call-template>
        </xsl:if>

        <!-- break on error when found handler without serializer|call -->
        <xsl:if test="not(.//map:serialize) and not(.//map:call)">
          <xsl:call-template name="error">
            <xsl:with-param name="message">Error handler must have map:serialize or map:call.</xsl:with-param>
          </xsl:call-template>
        </xsl:if>

        <!-- break on error when handler have any sitemap component after serializer -->
        <xsl:if test=".//map:serialize/following-sibling::*[namespace-uri()=$ns]">
          <xsl:call-template name="error">
            <xsl:with-param name="message">Error handler can not have any components after map:serialize.</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:for-each>
    </xsl:for-each>

    <code xml:space="preserve">
      <xsl:apply-templates/>
    </code>
  </xsl:template>

  <!-- This is the root element we are looking for here. It defines all the java
       code necessary to build up a sitemap engine class
  -->
  <xsl:template match="map:sitemap">
    /*****************************************************************************/
    /* Copyright (C) The Apache Software Foundation. All rights reserved.        */
    /* _________________________________________________________________________ */
    /* This software is published under the terms of the Apache Software License */
    /* version 1.1, a copy of which has been included  with this distribution in */
    /* the LICENSE file.                                                         */
    /*****************************************************************************/
    package <xsl:value-of select="translate(@file-path, '/', '.')"/>;

    import java.io.OutputStream;
    import java.io.IOException;

    import java.util.List;
    import java.util.ArrayList;
    import java.util.Map;
    import java.util.HashMap;
    import java.util.Stack;
    import java.util.StringTokenizer;

    import org.apache.avalon.framework.component.Component;
    import org.apache.avalon.framework.configuration.Configurable;
    import org.apache.avalon.framework.configuration.Configuration;
    import org.apache.avalon.framework.configuration.ConfigurationException;
    import org.apache.avalon.framework.configuration.DefaultConfiguration;
    import org.apache.avalon.framework.parameters.Parameters;

    import org.apache.cocoon.Constants;
    import org.apache.cocoon.ProcessingException;
    import org.apache.cocoon.ResourceNotFoundException;
    import org.apache.cocoon.ConnectionResetException;
    import org.apache.cocoon.acting.Action;
    import org.apache.cocoon.environment.Environment;
    import org.apache.cocoon.environment.Redirector;
    import org.apache.cocoon.matching.Matcher;
    import org.apache.cocoon.matching.PreparableMatcher;
    import org.apache.cocoon.selection.Selector;
    import org.apache.cocoon.sitemap.AbstractSitemap;
    import org.apache.cocoon.components.pipeline.StreamPipeline;
    import org.apache.cocoon.components.pipeline.EventPipeline;
    import org.apache.cocoon.sitemap.Sitemap;
    import org.apache.cocoon.sitemap.NotifyingGenerator;
    import org.apache.cocoon.sitemap.ContentAggregator;
    import org.apache.cocoon.sitemap.Manager;
    import org.apache.cocoon.sitemap.SitemapRedirector;
    import org.apache.cocoon.components.language.markup.xsp.XSPRequestHelper;
    import org.apache.cocoon.components.language.markup.xsp.XSPResponseHelper;
    import org.apache.cocoon.components.notification.NotifyingBuilder;
    import org.apache.cocoon.components.notification.Notifying;
    import org.apache.cocoon.components.notification.SimpleNotifyingBean;

    /**
     * This is the automatically generated class from the sitemap definitions
     *
     * @author &lt;a href="mailto:giacomo@apache.org"&gt;Giacomo Pati&lt;/a&gt;
     * @author &lt;a href="mailto:bloritsch@apache.org"&gt;Berin Loritsch&lt;/a&gt;
     * @author &lt;a href="mailto:barozzi@nicolaken.com"&gt;Nicola Ken Barozzi&lt;/a&gt;
     * @author &lt;a href="mailto:proyal@managingpartners.com"&gt;Peter Royal&lt;/a&gt;
     * @version CVS $Id: sitemap.xsl,v 1.5 2002/01/17 03:51:38 vgritsenko Exp $
     */
    public class <xsl:value-of select="@file-name"/> extends AbstractSitemap {
      static final String LOCATION = "<xsl:value-of select="translate(@file-path, '/', '.')"/>.<xsl:value-of select="@file-name"/>";

      static {
        dateCreated = <xsl:value-of select="@creation-date"/>L;
      }

      /** HashMap relating labels to view names */
      private HashMap view_label_map = new HashMap(<xsl:value-of select="count(/map:sitemap/map:views/map:view[@from-label])"/>);

      <xsl:for-each select="/map:sitemap/map:pipelines//map:match">
        <!-- Generate variables for patterns -->
        <xsl:variable name="matcher-name">
          <xsl:call-template name="generate-name">
            <xsl:with-param name="prefix">matcher_</xsl:with-param>
            <xsl:with-param name="suffix"><xsl:value-of select="generate-id(.)"/></xsl:with-param>
          </xsl:call-template>
        </xsl:variable>
      // Pattern for "<xsl:value-of select="@pattern"/>" (either String or prepared pattern)
      private Object <xsl:value-of select="$matcher-name"/>_expr;
      </xsl:for-each>

      /**
       * Method that handles selectors.
       */
      private boolean isSelected(String hint, String testValue, Parameters params, Map objectModel) throws Exception {
        Selector selector = (Selector)this.selectors.select(hint);
        try {
          return selector.select(testValue, objectModel, params);
        } finally {
          this.selectors.release(selector);
        }
      }

      /**
       * Method that handles matchers.
       */
      private Map matches(String hint, Object preparedPattern, String pattern, Parameters params, Map objectModel) throws Exception {
        Component matcher = (Component)this.matchers.select(hint);
        try {
          if (preparedPattern == null) {
            return ((Matcher)matcher).match(pattern, objectModel, params);
          } else {
            return ((PreparableMatcher)matcher).preparedMatch(preparedPattern, objectModel, params);
          }
        } finally {
          this.matchers.release(matcher);
        }
      }


      /**
       * Pass a &lt;code&gt;Configuration&lt;/code&gt; instance to this
       * &lt;code&gt;Configurable&lt;/code&gt; class.
       */
      public void configure(Configuration conf) throws ConfigurationException {
          this.sitemapManager = new Manager();
          this.sitemapManager.setLogger(getLogger());
          this.sitemapManager.compose(this.manager);
          this.sitemapManager.configure(conf);

        <xsl:for-each select="/map:sitemap/map:views/map:view">
          <!-- generate a HashMap relating labels to view names -->
          <xsl:variable name="view-label-name">
            <xsl:choose>
              <xsl:when test="(@from-label)">
                <xsl:value-of select="@from-label"/>
              </xsl:when>
              <xsl:when test="(@from-position)">
                <xsl:value-of select="@from-position"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:call-template name="error">
                  <xsl:with-param name="message">missing attribute "from-label" or "from-position" in declaration of view <xsl:value-of select="@name"/></xsl:with-param>
                </xsl:call-template>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:variable>
          <xsl:if test="@from-label">
            <xsl:text>view_label_map.put("</xsl:text><xsl:value-of select="$view-label-name"/>
            <xsl:text>", "</xsl:text><xsl:value-of select="@name"/>");
          </xsl:if>
        </xsl:for-each>

        try {
          <!-- configure well known components first -->
          load_component (Sitemap.GENERATOR, "!notifying-generator!", "org.apache.cocoon.sitemap.NotifyingGenerator", new DefaultConfiguration("", LOCATION), null);
          load_component (Sitemap.GENERATOR, "!content-aggregator!", "org.apache.cocoon.sitemap.ContentAggregator", new DefaultConfiguration("", LOCATION), null);
          load_component (Sitemap.TRANSFORMER, "!link-translator!", "org.apache.cocoon.sitemap.LinkTranslator", new DefaultConfiguration("", LOCATION), null);

          Configurer configurer = new Configurer(this, LOCATION);
          configurer.configGenerators();
          configurer.configTransformers();
          configurer.configReaders();
          configurer.configSerializers();
          configurer.configMatchers();
          configurer.configSelectors();
          configurer.configActions();
          configurer = null;

          this.generators.initialize();
          this.transformers.initialize();
          this.serializers.initialize();
          this.readers.initialize();
          this.actions.initialize();
          this.matchers.initialize();
          this.prepareMatchers();
          this.selectors.initialize();

          this.manager.initialize();
          /* catch any exception thrown by a component during configuration */
        } catch (Exception e) {
          getLogger().warn(e.getMessage(), e);
          throw new ConfigurationException ("Error in sitemap configuration : " + e.getMessage(), e);
        }
      }

      <!-- This class handles all component configuration. Because this is done once
          at instantiation time we can get rid of this code afterwards.
      -->
      class Configurer {
        <xsl:value-of select="@file-name"/> sitemap;
        String LOCATION;
        public Configurer (<xsl:value-of select="@file-name"/> sitemap, String location) {
          this.sitemap = sitemap;
          this.LOCATION = location;
        }

        /** Configure generators */
        public void configGenerators() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">generator</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:generators/map:generator"/>
          </xsl:call-template>
        }

        /** Configure transformers */
        public void configTransformers() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">transformer</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:transformers/map:transformer"/>
          </xsl:call-template>
        }

        /** Configure readers */
        public void configReaders() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">reader</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:readers/map:reader"/>
          </xsl:call-template>
        }

        /* Configure serializers */
        public void configSerializers() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">serializer</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:serializers/map:serializer"/>
          </xsl:call-template>
        }

        /** Configure matchers */
        public void configMatchers() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">matcher</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:matchers/map:matcher"/>
          </xsl:call-template>
        }

        /** Configure selectors */
        public void configSelectors() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">selector</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:selectors/map:selector"/>
          </xsl:call-template>
        }

        /** Configure actions */
        public void configActions() throws Exception {
          <xsl:call-template name="config-components">
            <xsl:with-param name="name">action</xsl:with-param>
            <xsl:with-param name="components"
                select="/map:sitemap/map:components/map:actions/map:action"/>
          </xsl:call-template>
        }
      }

      public Object preparePattern(String type, String pattern) throws Exception {
        Component matcher = this.matchers.select(type);
        try {
          if (matcher instanceof PreparableMatcher) {
            // Prepare pattern ('{' have been unescaped)
            return ((PreparableMatcher)matcher).preparePattern(pattern);
          } else {
            // Return null : regular Matcher will be used.
            return null;
          }
        } finally {
          this.matchers.release(matcher);
        }
      }

      /** Prepare patterns of PreparableMatchers. */
      public void prepareMatchers() throws Exception {
      <!-- Generate variables for patterns -->
      <xsl:for-each select="/map:sitemap/map:pipelines//map:match">
        <!-- get the type of matcher used -->
        <xsl:variable name="matcher-type">
          <xsl:call-template name="get-parameter">
            <xsl:with-param name="parname">type</xsl:with-param>
            <xsl:with-param name="default"><xsl:value-of select="/map:sitemap/map:components/map:matchers/@default"/></xsl:with-param>
            <xsl:with-param name="required">true</xsl:with-param>
          </xsl:call-template>
        </xsl:variable>

        <xsl:choose>
          <xsl:when test="XSLTFactoryLoader:hasSubstitutions($factory-loader, @pattern)">
          // Pattern "<xsl:value-of select="@pattern"/>" has substitutions and is not prepared.
          </xsl:when>
          <xsl:otherwise>
            <xsl:variable name="matcher-name">
              <xsl:call-template name="generate-name">
                <xsl:with-param name="prefix">matcher_</xsl:with-param>
                <xsl:with-param name="suffix"><xsl:value-of select="generate-id(.)"/></xsl:with-param>
              </xsl:call-template>
            </xsl:variable>
          // Prepare the pattern for "<xsl:value-of select="@pattern"/>"
          this.<xsl:value-of select="$matcher-name"/>_expr = this.preparePattern("<xsl:value-of select="$matcher-type"/>", "<xsl:value-of select="XSLTFactoryLoader:escapeBraces($factory-loader, @pattern)"/>");
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
      }

      <!-- generate methods for every map:resource element -->
      <xsl:for-each select="/map:sitemap/map:resources/map:resource">
        <xsl:call-template name="line-number"/>
        /**
         * This is the internal resource named "<xsl:value-of select="@name"/>"
         * @param pipeline A &lt;code&gt;StreamPipeline&lt;/code&gt; holding the sitemap component collected so far
         * @param listOfMaps A &lt;code&gt;List&lt;/code&gt; of Maps holding replacement values for src attributes
         * @param environment The &lt;code&gt;Environment&lt;/code&gt; requesting a resource
         * @param cocoon_view The view of the resource requested
         * @return Wether the request has been processed or not
         * @exception Exception If an error occurs during request evaluation and production
         */
        public final boolean resource_<xsl:value-of select="translate(@name, '- ', '__')"/> (StreamPipeline pipeline,
            EventPipeline eventPipeline, List listOfMaps, Environment environment, String cocoon_view, boolean internalRequest)
        throws Exception {
            Map map = null;
            Parameters param = null;
            final boolean debug_enabled = getLogger().isDebugEnabled();
            Map objectModel = environment.getObjectModel();
            SitemapRedirector redirector = new SitemapRedirector(environment);
            <xsl:apply-templates/>
            return internalRequest;
        }
      </xsl:for-each>

      <!-- generate methods for every map:view element -->
      <xsl:for-each select="/map:sitemap/map:views/map:view">
        <xsl:call-template name="line-number"/>
        /**
         * This is the method to produce the "<xsl:value-of select="@name"/>" view of the requested resource
         * @param pipeline A &lt;code&gt;StreamPipeline&lt;/code&gt; holding the sitemap component collected so far
         * @param listOfMaps A &lt;code&gt;List&lt;/code&gt; of Maps holding replacement values for src attributes
         * @param environment The &lt;code&gt;Environment&lt;/code&gt; requesting a resource
         * @return Wether the request has been processed or not
         * @exception Exception If an error occurs during request evaluation and production
         */
        private final boolean view_<xsl:value-of select="translate(@name, '- ', '__')"/> (StreamPipeline pipeline,
            EventPipeline eventPipeline, List listOfMaps, Environment environment, boolean internalRequest)
        throws Exception {
          Map map = null;
          Parameters param = null;
          final boolean debug_enabled = getLogger().isDebugEnabled();
          SitemapRedirector redirector = new SitemapRedirector(environment);
          <xsl:apply-templates/>
          return internalRequest;
        }
      </xsl:for-each>

      <!-- generate contains_view method to check if a view request is satisfied -->
      private final String contains_view(String labels, String cocoon_view) {
        final boolean debug_enabled = getLogger().isDebugEnabled();
        if (debug_enabled) getLogger().debug("contains_view(\"" + labels + "\", \"" + cocoon_view + "\")");
        StringTokenizer st = new StringTokenizer(labels, " ,", false);
        while (st.hasMoreTokens()) {
          String token = st.nextToken();
          String view = (String)view_label_map.get(token);
          if (debug_enabled) getLogger().debug("contains_view: examining token \"" + token + "\" against view \"" + view + "\"");
          if (view != null &amp;&amp; view.equals(cocoon_view)) {
            if (debug_enabled) getLogger().debug("contains_view: view \"" + view + "\" selected");
            return view;
          }
        }
        return null;
      }

      <!-- generate call_view method to satisfy a view request -->
      private final boolean call_view(String view_name,
                                StreamPipeline pipeline,
                                EventPipeline eventPipeline,
                                List listOfMaps,
                                Environment environment,
                                boolean internalRequest)
      throws Exception {
        if (getLogger().isDebugEnabled()) getLogger().debug("call_view(\"" + view_name + "\")");
        <xsl:for-each select="/map:sitemap/map:views/map:view[@from-label]">
          <xsl:choose>
            <xsl:when test="(@from-label)"/>
            <xsl:when test="(@from-position)"/>
            <xsl:otherwise>
              <xsl:call-template name="error">
                <xsl:with-param name="message">missing attribute "from-label" or "from-position" in declaration of view <xsl:value-of select="@name"/></xsl:with-param>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>if ("</xsl:text><xsl:value-of select="@name"/>".equals(view_name)) {
            return view_<xsl:value-of select="translate(@name, '- ', '__')"/>(pipeline, eventPipeline, listOfMaps, environment, internalRequest);
          }
        </xsl:for-each>
        return internalRequest;
      }

      <xsl:for-each select="/map:sitemap/map:action-sets/map:action-set">
        <!-- generate methods for every map:action-set element -->
        <xsl:call-template name="line-number"/>
        /**
         * This is the method to process the "<xsl:value-of select="@name"/>" action-set of the requested resource
         * @param cocoon_action A &lt;code&gt;String&lt;/code&gt; holding the requested action
         * @param listOfMaps A &lt;code&gt;List&lt;/code&gt; of Maps holding replacement values for src attributes
         * @param environment The &lt;code&gt;Environment&lt;/code&gt; requesting a resource
         * @param objectModel The &lt;code&gt;Map&lt;/code&gt; containing the environments model objects
         * @return Wether the request has been processed or not
         * @exception Exception If an error occurs during request evaluation and production
         */
        private final Map action_set_<xsl:value-of select="translate(@name, '- ', '__')"/> (SitemapRedirector redirector, String cocoon_action, List listOfMaps, Environment environment, Map objectModel, String src, Parameters param)
        throws Exception {
          Map map;
          Map allMap = null;
          final boolean debug_enabled = getLogger().isDebugEnabled();
          Parameters nparam = null;
          <xsl:for-each select="map:act">
            map = null;
            <xsl:choose>
              <xsl:when test="@action">
                if ("<xsl:value-of select="@action"/>".equals(cocoon_action)) {
                  <xsl:apply-templates select="." mode="set"/>
                  if (map != null) {
                    if (allMap == null) {
                      allMap = new HashMap();
                    }
                    allMap.putAll (map);
                  }
                }
              </xsl:when>
              <xsl:otherwise>
                <xsl:apply-templates select="." mode="set"/>
                  if (map != null) {
                    if (allMap == null) {
                      allMap = new HashMap();
                    }
                    allMap.putAll (map);
                  }
              </xsl:otherwise>
            </xsl:choose>
          </xsl:for-each>
          return allMap;
        }
      </xsl:for-each>

      /**
       * Process to producing the output to the specified &lt;code&gt;OutputStream&lt;/code&gt;.
       */
      public final boolean process(Environment environment) throws Exception {
        /* the &lt;code&gt;EventPipeline&lt;/code&gt; is used to collect the xml producing sitemap
           components and the &lt;code&gt;StreamPipeline&lt;/code&gt; to produce the requested resource */
        EventPipeline eventPipeline = null;
        StreamPipeline pipeline = null;
        boolean result = false;
        try {
          try {
            eventPipeline = (EventPipeline)this.manager.lookup(EventPipeline.ROLE);
            pipeline = (StreamPipeline)this.manager.lookup(StreamPipeline.ROLE);
            pipeline.setEventPipeline(eventPipeline);
          } catch (Exception e) {
            getLogger().error("Processing of resource failed", e);
            throw e;
          }

          result = process (environment, pipeline, eventPipeline, false);

        } finally {
          if(eventPipeline != null) this.manager.release(eventPipeline);
          if(pipeline != null) this.manager.release(pipeline);
        }
        return result;
      }

      /**
       * Process to producing the output to the specified &lt;code&gt;OutputStream&lt;/code&gt;.
       */
      public final boolean process(Environment environment, StreamPipeline pipeline, EventPipeline eventPipeline)
      throws Exception {
        getLogger().debug("Processing internal sitemap request");
        return process (environment, pipeline, eventPipeline, true);
      }

      /**
       * Process to producing the output to the specified &lt;code&gt;OutputStream&lt;/code&gt;.
       */
      private final boolean process(Environment environment, StreamPipeline pipeline,
                                    EventPipeline eventPipeline, boolean internalRequest)
      throws Exception {
        // set the correct component manager
        pipeline.recompose(this.manager);
        eventPipeline.recompose(this.manager);

        /* the &lt;code&gt;List&lt;/code&gt; objects to hold the replacement values
           delivered from matchers and selectors to replace occurences of
           XPath kind expressions in values of src attribute used with
           generate and transform elements */
        List listOfMaps = (List) new ArrayList();

        Map map;
        Map objectModel = environment.getObjectModel();
        String cocoon_view = environment.getView();
        String cocoon_action = environment.getAction();
        final boolean debug_enabled = getLogger().isDebugEnabled();

        //Adds Constants.NOTIFYING_OBJECT to ObjectModel
        //NKB FIXME add to ObjectModel?
        objectModel.put(Constants.NOTIFYING_OBJECT, new SimpleNotifyingBean(this));
        SitemapRedirector redirector = new SitemapRedirector(environment);
        <!-- process the pipelines -->
        <!-- for each pipeline element generate a try/catch block -->
        <xsl:for-each select="/map:sitemap/map:pipelines/map:pipeline">
          <xsl:call-template name="line-number"/>
          <xsl:variable name="pipeline-position" select="position()"/>
          <xsl:if test="@internal-only = 'yes' or @internal-only='true'">
            if (internalRequest) {
          </xsl:if>
          try {
            <!-- Generate code for all components nested into pipeline -->
            <xsl:apply-templates/>

            <xsl:if test="position() = last()">
              throw new ResourceNotFoundException("No pipeline matched request: " + environment.getURIPrefix()+'/'+environment.getURI());
            </xsl:if>
          } catch (ConnectionResetException cre) {
             // Will be reported by CocoonServlet, rethrowing
            throw cre;
          } catch (ResourceNotFoundException e) {
            <xsl:choose>
              <xsl:when test="(./map:handle-errors[@type='404'])">
                getLogger().warn("404, try to process the error page", e);
                try {
                  return error_process_<xsl:value-of select="$pipeline-position"/>_404 (redirector, environment,
                          environment.getObjectModel(), e, internalRequest);
                } catch (Exception ex) {
                  getLogger().warn("Sitemap Error Process", ex);
                  if (true) return false;
                }
              </xsl:when>
              <xsl:otherwise>
                // Will be reported by CocoonServlet, rethrowing
                if (true) throw e;
              </xsl:otherwise>
            </xsl:choose>
          } catch (Exception e) {
            <xsl:choose>
              <xsl:when test="(./map:handle-errors[not(@type) or (@type='500')])">
                getLogger().warn("Error, try to process the error page", e);
                if (internalRequest) {
                  if (getLogger().isErrorEnabled()) getLogger().error("Sitemap", e);
                  if (true) throw e;
                }
                try {
                  return error_process_<xsl:value-of select="$pipeline-position"/>_500 (redirector, environment,
                          environment.getObjectModel(), e, internalRequest);
                } catch (Exception ex) {
                  getLogger().warn("Sitemap Error Process", ex);
                  if (true) return false;
                }
              </xsl:when>
              <xsl:otherwise>
                if (getLogger().isErrorEnabled()) getLogger().error("Sitemap", e);
                if (true) throw e;
              </xsl:otherwise>
            </xsl:choose>
          }
          <xsl:if test="@internal-only = 'yes' or @internal-only='true'">
            }
          </xsl:if>
        </xsl:for-each>

        return false;
      }

      <xsl:for-each select="/map:sitemap/map:pipelines/map:pipeline">
        <xsl:call-template name="line-number"/>
        <xsl:variable name="pipeline-position" select="position()"/>
        <!-- Generate methods (if required) for all components nested into pipeline -->
        <xsl:apply-templates select="*" mode="method"/>

        <!-- Generate methods for every map:handle-errors elements in all map:pipeline elements -->
        <xsl:for-each select="map:handle-errors">
          <!-- Default error handler's type is 500 -->
          <xsl:variable name="type">
            <xsl:choose>
              <xsl:when test="@type">_<xsl:value-of select="@type"/></xsl:when>
              <xsl:otherwise>_500</xsl:otherwise>
            </xsl:choose>
          </xsl:variable>
          private boolean error_process_<xsl:value-of select="concat($pipeline-position,$type)"/> (SitemapRedirector redirector, Environment environment, Map objectModel, Exception e, boolean internalRequest)
          throws Exception {
            EventPipeline eventPipeline = null;
            StreamPipeline pipeline = null;
            try {
              environment.tryResetResponse();
              eventPipeline = (EventPipeline)this.manager.lookup(EventPipeline.ROLE);
              pipeline = (StreamPipeline)this.manager.lookup(StreamPipeline.ROLE);
              pipeline.setEventPipeline(eventPipeline);
              pipeline.recompose(this.manager);
              eventPipeline.recompose(this.manager);
              List listOfMaps = (List)(new ArrayList());
              Map map;
              Parameters param;
              final boolean debug_enabled = getLogger().isDebugEnabled();

              // Create a Notifying out of this exception and stick it in the object model Constants.NOTIFYING_OBJECTS
              if (debug_enabled) getLogger().debug("Starting to build Notifying");
              NotifyingBuilder notifyingBuilder= (NotifyingBuilder)this.manager.lookup(NotifyingBuilder.ROLE);
              if (debug_enabled) getLogger().debug("Got " + notifyingBuilder);
              Notifying currentNotifying = notifyingBuilder.build(this, e);
              if (debug_enabled) getLogger().debug("Built " + currentNotifying);
              objectModel.put(Constants.NOTIFYING_OBJECT, currentNotifying );
              if (debug_enabled) getLogger().debug("Added to objectModel" + currentNotifying);
              eventPipeline.setGenerator ("!notifying-generator!", e.getMessage(), Parameters.EMPTY_PARAMETERS);

              <xsl:apply-templates/>
            } catch (Exception ex) {
              if (getLogger().isErrorEnabled()) getLogger().error("NotifyingGenerator was not able to notify the exception.", ex);
              throw e;
            } finally {
              if(eventPipeline != null)
                this.manager.release(eventPipeline);
              if(pipeline != null)
                this.manager.release(pipeline);
            }
            return false;
          }
        </xsl:for-each>
      </xsl:for-each>
    }
  </xsl:template> <!-- match="map:sitemap" -->



  <!-- Generate matcher invokation code for top-level matchers -->
  <xsl:template match="map:match">
    // method for handling "<xsl:value-of select="@pattern"/>"
    if (match<xsl:value-of select="generate-id(.)"/>(redirector, environment, pipeline, eventPipeline, internalRequest, listOfMaps))
        return true;
  </xsl:template>



  <!-- Generate method body for top-level matchers -->
  <xsl:template match="map:match" mode="method">
    <!-- get the type of matcher used -->
    <xsl:variable name="matcher-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
        <xsl:with-param name="default"><xsl:value-of select="/map:sitemap/map:components/map:matchers/@default"/></xsl:with-param>
        <xsl:with-param name="required">true</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- remove all invalid chars from the pattern. The result is used to form the name of the generated method
         in case this matcher is produced by a CodeFactory -->
    <xsl:variable name="matcher-name">
      <xsl:call-template name="generate-name">
        <xsl:with-param name="prefix">matcher_</xsl:with-param>
        <xsl:with-param name="suffix"><xsl:value-of select="generate-id(.)"/>_expr</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- Define parameters for this matcher (if present) -->
    <xsl:if test="map:parameter">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:text>// method for handling "</xsl:text><xsl:value-of select="@pattern"/>"
    private final boolean match<xsl:value-of select="generate-id(.)"/>(
                                    SitemapRedirector redirector,
                                    Environment environment, StreamPipeline pipeline,
                                    EventPipeline eventPipeline,
                                    boolean internalRequest, List listOfMaps)
            throws ConnectionResetException, ResourceNotFoundException, Exception {
        Map map;
        Parameters param;
        Map objectModel = environment.getObjectModel();
        String cocoon_view = environment.getView();
        String cocoon_action = environment.getAction();
        final boolean debug_enabled = getLogger().isDebugEnabled();

        if ((map = matches("<xsl:value-of select="$matcher-type"/>", <xsl:value-of select="$matcher-name"/>, <xsl:apply-templates select="@pattern"/>, <xsl:value-of select="$component-param"/>, objectModel)) != null) {
          if (debug_enabled) getLogger().debug("Matched <xsl:value-of select="$matcher-type"/> pattern <xsl:value-of select="XSLTFactoryLoader:escape($factory-loader, @pattern)"/>");
          listOfMaps.add (map);
          this.dumpParameters(listOfMaps);
          <xsl:apply-templates/>
          listOfMaps.remove (listOfMaps.size()-1);
          this.dumpParameters(listOfMaps);
        }
        return false;
    }
  </xsl:template> <!-- match="map:match" mode="method" -->



  <!-- Generate matcher invokation code for nested matchers -->
  <xsl:template match="map:match//map:match">
    <!-- get the type of matcher used -->
    <xsl:variable name="matcher-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
        <xsl:with-param name="default"><xsl:value-of select="/map:sitemap/map:components/map:matchers/@default"/></xsl:with-param>
        <xsl:with-param name="required">true</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- remove all invalid chars from the pattern. The result is used to form the name of the generated method
         in case this matcher is produced by a CodeFactory -->
    <xsl:variable name="matcher-name">
      <xsl:call-template name="generate-name">
        <xsl:with-param name="prefix">matcher_</xsl:with-param>
        <xsl:with-param name="suffix"><xsl:value-of select="generate-id(.)"/>_expr</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- Define parameters for this matcher (if present) -->
    <xsl:if test="map:parameter">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

      <!-- this is the actual code produced -->
      // handling "<xsl:value-of select="@pattern"/>"
      if ((map = matches("<xsl:value-of select="$matcher-type"/>", <xsl:value-of select="$matcher-name"/>, <xsl:apply-templates select="@pattern"/>, <xsl:value-of select="$component-param"/>, objectModel)) != null) {
        if (debug_enabled) getLogger().debug("Matched <xsl:value-of select="$matcher-type"/> pattern <xsl:value-of select="XSLTFactoryLoader:escape($factory-loader, @pattern)"/>");
        listOfMaps.add (map);
        this.dumpParameters(listOfMaps);
        <xsl:apply-templates/>
        listOfMaps.remove (listOfMaps.size()-1);
        this.dumpParameters(listOfMaps);
      }
  </xsl:template> <!-- match="map:match//map:match" -->



  <!-- a select element introduces a multi branch case by calls to a select method on a selector component (or a inlined
       selector method produced by a CodeFactory -->
  <xsl:template match="map:select">

    <!-- get the type of selector used -->
    <xsl:variable name="selector-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
        <xsl:with-param name="default"><xsl:value-of select="/map:sitemap/map:components/map:selectors/@default"/></xsl:with-param>
        <xsl:with-param name="required">true</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- test if we have to define parameters for this action -->
    <xsl:if test="map:parameter">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- loop through all the when cases -->
    <xsl:for-each select="map:when">
      <!-- remove all invalid chars from the test expression. The result is used to form the name of the generated method
           in case this selector is produced by a CodeFactory -->
      <xsl:variable name="selector-name2">
        <xsl:call-template name="generate-name">
          <xsl:with-param name="prefix">selector_</xsl:with-param>
          <xsl:with-param name="suffix">_<xsl:value-of select="generate-id(.)"/></xsl:with-param>
        </xsl:call-template>
      </xsl:variable>

      <!-- this is the actual code produced on the when elements -->
      <xsl:call-template name="line-number"/>
      <xsl:if test="position() > 1">
      else </xsl:if>
      if (isSelected("<xsl:value-of select="$selector-type"/>", <xsl:apply-templates select="@test"/>, <xsl:value-of select="$component-param"/>, objectModel)) {
        if (debug_enabled) getLogger().debug("Select <xsl:value-of select="$selector-type"/> Test <xsl:value-of select="XSLTFactoryLoader:escape($factory-loader, @test)"/>");
        <xsl:apply-templates/>
      }
    </xsl:for-each>

    <!-- this is the actual code produced on the otherwise element -->
    <xsl:for-each select="map:otherwise">
      <xsl:text>else {
      </xsl:text><xsl:call-template name="line-number"/>
        if (debug_enabled) getLogger().debug("Select <xsl:value-of select="$selector-type"/> Otherwise");
        <xsl:apply-templates/>
      }
    </xsl:for-each>
  </xsl:template> <!-- match="map:select" -->



  <!-- processing of an act element having a type attribute -->
  <xsl:template match="map:act[@type]">

    <!-- get the type of action used -->
    <xsl:variable name="action-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- get the source parameter for the Action -->
    <xsl:variable name="action-source">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">src</xsl:with-param>
        <xsl:with-param name="default">null</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- test if we have to define parameters for this action -->
    <xsl:if test="map:parameter">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- gets the action java variable -->
    <xsl:variable name="action-name">
      <xsl:text>action_</xsl:text><xsl:value-of select="generate-id(.)"/>
    </xsl:variable>

    <!-- generate the invocation of the act method of the action component -->
    Action <xsl:value-of select="$action-name"/> = (Action)this.actions.select("<xsl:value-of select="$action-type"/>");
    try {
    <xsl:choose>
      <xsl:when test="*[namespace-uri()=$ns and local-name() != 'parameter']">
        if ((map = <xsl:value-of select="$action-name"/>.act(redirector, environment, objectModel, <xsl:value-of select="$action-source"/>, <xsl:value-of select="$component-param"/>)) != null) {
          if (redirector.hasRedirected()) {
             return true;
          }
          if (debug_enabled) getLogger().debug("Action <xsl:value-of select="$action-type"/>");
          listOfMaps.add (map);
          this.dumpParameters(listOfMaps);
          <xsl:apply-templates select="*[namespace-uri()=$ns and local-name() != 'parameter']"/>
          listOfMaps.remove(listOfMaps.size()-1);
          this.dumpParameters(listOfMaps);
        }
        if (redirector.hasRedirected()) {
          return true;
        }
      </xsl:when>
      <xsl:otherwise>
        map = <xsl:value-of select="$action-name"/>.act(redirector, environment, objectModel, <xsl:value-of select="$action-source"/>, <xsl:value-of select="$component-param"/>);
        if (redirector.hasRedirected()) {
          return true;
        }
      </xsl:otherwise>
    </xsl:choose>
    } finally {
        this.actions.release(<xsl:value-of select="$action-name"/>);
    }
  </xsl:template> <!-- match="map:act[@type]" -->



  <!-- processing of an act element having a type attribute -->
  <xsl:template match="map:act[@type]" mode="set">

    <!-- get the type of action used -->
    <xsl:variable name="action-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- get the source parameter for the Action -->
    <xsl:variable name="action-source">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">src</xsl:with-param>
        <xsl:with-param name="default">null</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- prepare parameters -->
    <xsl:text>
    nparam = new Parameters ();
    </xsl:text>
    <xsl:apply-templates select="map:parameter">
      <xsl:with-param name="param" select="'nparam'"/>
    </xsl:apply-templates>
    <xsl:text>
    nparam.merge(param);
    </xsl:text>
    <xsl:variable name="component-param">nparam</xsl:variable>

    <!-- gets the action java variable -->
    <xsl:variable name="action-name">
      <xsl:text>action_</xsl:text><xsl:value-of select="generate-id(.)"/>
    </xsl:variable>

    <!-- generate the invocation of the act method of the action component -->
    Action <xsl:value-of select="$action-name"/> = (Action)this.actions.select("<xsl:value-of select="$action-type"/>");
    try {
        map = <xsl:value-of select="$action-name"/>.act(redirector, environment, objectModel, <xsl:value-of select="$action-source"/>, <xsl:value-of select="$component-param"/>);
        <xsl:if test="*[namespace-uri()=$ns and local-name() != 'parameter']">
          if (map != null) {
              if (debug_enabled) getLogger().debug("Action <xsl:value-of select="$action-type"/>");
              listOfMaps.add (map);
              this.dumpParameters(listOfMaps);
              <xsl:apply-templates select="*[namespace-uri()=$ns and local-name() != 'parameter']" mode="set"/>
              listOfMaps.remove(listOfMaps.size()-1);
              this.dumpParameters(listOfMaps);
          }
      </xsl:if>
    } finally {
        this.actions.release(<xsl:value-of select="$action-name"/>);
    }
  </xsl:template> <!-- match="map:act[@type]" mode="set" -->



  <!-- processing of an act element having a set attribute -->
  <xsl:template match="map:act[@set]">

    <!-- get the type of action used -->
    <xsl:variable name="action-set">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">set</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- get the source parameter for the Action -->
    <xsl:variable name="action-source">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">src</xsl:with-param>
        <xsl:with-param name="default">null</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- test if we have to define parameters for this action -->
    <xsl:if test="map:parameter">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- gets the string how the action is to be invoked in java code -->
    <xsl:variable name="action-name">
      <xsl:text>action_set_</xsl:text><xsl:value-of select="$action-set"/>
    </xsl:variable>

    <!-- generate the invocation of the act method of the action component -->
    <xsl:choose>
      <xsl:when test="*[namespace-uri()=$ns and local-name() != 'parameter']">
        if ((map = <xsl:value-of select="$action-name"/> (redirector, cocoon_action, listOfMaps, environment, objectModel, <xsl:value-of select="$action-source"/>, <xsl:value-of select="$component-param"/>)) != null) {
          if (redirector.hasRedirected()) {
              return true;
          }
          getLogger().debug("Action set <xsl:value-of select="$action-name"/>");
          listOfMaps.add (map);
          this.dumpParameters(listOfMaps);
          <xsl:apply-templates select="*[namespace-uri()=$ns and local-name() != 'parameter']"/>
          listOfMaps.remove(listOfMaps.size()-1);
          this.dumpParameters(listOfMaps);
        }
        if (redirector.hasRedirected()) {
          return true;
        }
      </xsl:when>
      <xsl:otherwise>
        map = <xsl:value-of select="$action-name"/> (redirector, cocoon_action, listOfMaps, environment, objectModel, <xsl:value-of select="$action-source"/>, <xsl:value-of select="$component-param"/>);
        if (redirector.hasRedirected()) {
          return true;
        }
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template> <!-- match="map:act[@set]" -->



  <!-- generate the code to invoke a generator -->
  <xsl:template match="map:generate">
    <xsl:call-template name="setup-component">
      <xsl:with-param name="default-component" select="/map:sitemap/map:components/map:generators/@default"/>
      <xsl:with-param name="method">eventPipeline.setGenerator</xsl:with-param>
      <xsl:with-param name="prefix">generator</xsl:with-param>
    </xsl:call-template>
  </xsl:template> <!-- match="map:generate" -->



  <!-- generate the code to invoke a transformer -->
  <xsl:template match="map:transform">
    <xsl:call-template name="setup-component">
      <xsl:with-param name="default-component" select="/map:sitemap/map:components/map:transformers/@default"/>
      <xsl:with-param name="method">eventPipeline.addTransformer</xsl:with-param>
      <xsl:with-param name="prefix">transformer</xsl:with-param>
    </xsl:call-template>
  </xsl:template> <!-- match="map:transformer" -->



  <!-- generate the code to invoke a serializer -->
  <xsl:template match="map:serialize">
    <xsl:call-template name="setup-component">
      <xsl:with-param name="default-component" select="/map:sitemap/map:components/map:serializers/@default"/>
      <xsl:with-param name="method">pipeline.setSerializer</xsl:with-param>
      <xsl:with-param name="prefix">serializer</xsl:with-param>
      <xsl:with-param name="mime-type" select="@mime-type"/>
    </xsl:call-template>

    <!-- if there is a status-code attribute tell it to the environment -->
    <xsl:if test="@status-code">
      environment.setStatus(<xsl:value-of select="@status-code"/>);
    </xsl:if>

    <xsl:if test="not(ancestor::map:pipeline[@internal-only='yes' or @internal-only='true'])">
      if (!internalRequest) {
        return pipeline.process(environment);
      }
    </xsl:if>
    <!-- the if(true) prevents "unreachable statement" errors during compile -->
    if (true) return true;
  </xsl:template> <!-- match="map:serialize" -->



  <!-- generate the code to invoke a reader -->
  <xsl:template match="map:read">
    <xsl:call-template name="setup-component">
      <xsl:with-param name="default-component" select="/map:sitemap/map:components/map:readers/@default"/>
      <xsl:with-param name="method">pipeline.setReader</xsl:with-param>
      <xsl:with-param name="prefix">reader</xsl:with-param>
      <xsl:with-param name="mime-type" select="@mime-type"/>
    </xsl:call-template>

    <xsl:if test="not(ancestor::map:pipeline[@internal-only='yes' or @internal-only='true'])">
      if (!internalRequest) {
        return pipeline.process(environment);
      }
    </xsl:if>
    <!-- the if(true) prevents "unreachable statement" errors during compile -->
    if (true) return true;
  </xsl:template> <!-- match="map:read" -->



  <!-- generate the code to invoke a sub sitemap  -->
  <xsl:template match="map:mount">
    <xsl:if test="@src=''">
      <xsl:call-template name="error">
        <xsl:with-param name="message">
          Required attribute src missing in element map:mount
        </xsl:with-param>
      </xsl:call-template>
    </xsl:if>

    <xsl:variable name="check-reload">
      <xsl:choose>
        <xsl:when test="@check-reload='yes'">true</xsl:when>
        <xsl:when test="@check-reload='true'">true</xsl:when>
        <xsl:when test="@check-reload='no'">false</xsl:when>
        <xsl:when test="@check-reload='false'">false</xsl:when>
        <xsl:when test="not(@check-reload)">true</xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="error">
            <xsl:with-param name="message">
              element <xsl:value-of select="name(.)"/> with uri-prefix="<xsl:value-of select="@uri-prefix"/>" has a wrong value in 'check-reload' attribute . Use "yes" or "no" but not "<xsl:value-of select="@check-reload"/>".
            </xsl:with-param>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="reload-method">
      <xsl:choose>
        <xsl:when test="@reload-method='asynchron'">true</xsl:when>
        <xsl:when test="@reload-method='synchron'">false</xsl:when>
        <xsl:when test="not(@reload-method)">true</xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="error">
            <xsl:with-param name="message">
              element <xsl:value-of select="name(.)"/> with uri-prefix="<xsl:value-of select="@uri-prefix"/>" has a wrong value in 'reload-method' attribute . Use "asynchron" or "synchron" but not "<xsl:value-of select="@reload-method"/>".
            </xsl:with-param>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- generate the code to invoke the sitemapManager which handles delegation of control to sub sitemaps -->
    if (internalRequest)
      return sitemapManager.invoke (this.manager, environment, <xsl:apply-templates select="@uri-prefix"/>, <xsl:apply-templates select="@src"/>, <xsl:value-of select="$check-reload"/>, <xsl:value-of select="$reload-method"/>, pipeline, eventPipeline);
    else
      if (true) return sitemapManager.invoke (this.manager, environment, <xsl:apply-templates select="@uri-prefix"/>, <xsl:apply-templates select="@src"/>, <xsl:value-of select="$check-reload"/>, <xsl:value-of select="$reload-method"/>);
  </xsl:template> <!-- match="map:mount" -->



  <!-- generate the code to redirect a request to an internal resource definition -->
  <xsl:template match="map:call">
    <xsl:choose>
      <xsl:when test="map:parameter">
        map = new HashMap(<xsl:value-of select="count(map:parameter)"/>);
        <xsl:for-each select="map:parameter">
          map.put(<xsl:apply-templates select="@name"/>, <xsl:apply-templates select="@value"/>);
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        map = new HashMap(1);
      </xsl:otherwise>
    </xsl:choose>
      listOfMaps.add(map);
      this.dumpParameters(listOfMaps);
      Class[] argTypes = new Class[] {StreamPipeline.class, EventPipeline.class, List.class, Environment.class, String.class, Boolean.TYPE};
      Object[] argValues = new Object[] {pipeline, eventPipeline, listOfMaps, environment, cocoon_view, new Boolean(internalRequest)};
      String methodName = "resource_" + substitute(listOfMaps, "<xsl:value-of select="translate(@resource, '- ', '__')"/>");
      if (true) return invokeMethod(methodName, argTypes, argValues);
  </xsl:template><!-- match="map:call" -->



  <!-- generate the code to redirect a request -->
  <xsl:template match="map:redirect-to">
    <xsl:choose>
      <!-- redirect to a internal resource definition -->
      <xsl:when test="@resource">
        <!-- print out deprecation message. -->
        <xsl:call-template name="warn">
          <xsl:with-param name="message">
            The use of &lt;redirect-to resource="<xsl:value-of select="@resource"/>"/&gt; is deprecated.
            Please use &lt;call resource="<xsl:value-of select="@resource"/>"/&gt; instead.
          </xsl:with-param>
        </xsl:call-template>

        <!-- since the above message shows up in components' log only, have
             this message printed for every request -->
        if (getLogger().isWarnEnabled())
           getLogger().warn("&lt;redirect-to resource=\"<xsl:value-of select="@resource"/>\"/&gt; is deprecated. "
                          + "Please use &lt;call resource=\"<xsl:value-of select="@resource"/>\"/&gt; instead.");

        <xsl:if test="string-length(@target) > 0 and string(@target) != '&quot;&quot;'">
          map = new HashMap(1);
          map.put("target", <xsl:apply-templates select="@target"/>);
          listOfMaps.add(map);
          this.dumpParameters(listOfMaps);
        </xsl:if>
         Class[] argTypes = new Class[] {StreamPipeline.class, EventPipeline.class, List.class, Environment.class, String.class, Boolean.TYPE};
         Object[] argValues = new Object[] {pipeline, eventPipeline, listOfMaps, environment, cocoon_view, new Boolean(internalRequest)};
         String methodName = "resource_" + substitute(listOfMaps, "<xsl:value-of select="translate(@resource, '- ', '__')"/>");
         if (true) return invokeMethod(methodName, argTypes, argValues);
      </xsl:when>

      <!-- redirect to a external resource definition with optional session mode attribute. Let the environment do the redirect -->
      <xsl:when test="@uri">
        <xsl:variable name="sess">
          <xsl:choose>
            <xsl:when test="@session='yes'">true</xsl:when>
            <xsl:when test="@session='true'">true</xsl:when>
            <xsl:when test="@session='no'">false</xsl:when>
            <xsl:when test="@session='false'">false</xsl:when>
            <xsl:when test="not(@session)">false</xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="error">
                <xsl:with-param name="message">Invalid value of session= attribute of redirect-to element</xsl:with-param>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        if (debug_enabled) getLogger().debug("Sitemap: session='<xsl:value-of select="$sess"/>', redirecting to " + <xsl:apply-templates select="@uri"/>);
        environment.redirect (<xsl:value-of select="$sess"/>, <xsl:apply-templates select="@uri"/>);
        if (true) return true;
      </xsl:when>

      <!-- any other combination generates an error message -->
      <xsl:otherwise>
        <xsl:call-template name="error">
          <xsl:with-param name="message">Missing attribute uri= or resource= to element redirect-to</xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template> <!-- match="map:redirect-to" -->



  <!-- generate the code to match a aggregate definition -->
  <xsl:template match="map:aggregate">
    <xsl:call-template name="setup-component">
      <xsl:with-param name="default-component">!content-aggregator!</xsl:with-param>
      <xsl:with-param name="method">eventPipeline.setGenerator</xsl:with-param>
      <xsl:with-param name="prefix">aggregator</xsl:with-param>
    </xsl:call-template>
    <xsl:variable name="ca">contentAggregator_<xsl:value-of select="generate-id(.)"/></xsl:variable>
    ContentAggregator <xsl:value-of select="$ca"/> = (ContentAggregator)eventPipeline.getGenerator();
    <xsl:if test="not (@element)">
      <xsl:call-template name="error">
        <xsl:with-param name="message">
          attribute 'element' missing in aggregate element
        </xsl:with-param>
      </xsl:call-template>
    </xsl:if>

    <xsl:variable name="ns">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">ns</xsl:with-param>
        <xsl:with-param name="default">""</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="prefix">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">prefix</xsl:with-param>
        <xsl:with-param name="default">""</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="$ca"/>.setRootElement(<xsl:apply-templates select="@element"/>,
        <xsl:value-of select="$ns"/>, <xsl:value-of select="$prefix"/>);

    <!-- check if a view was requested which matches one attached to a part element -->
    <xsl:if test="map:part[@label]">
    {
      boolean hasMatchingViewRequest = false;
      <xsl:for-each select="map:part[@label]">
        if (contains_view("<xsl:value-of select="@label"/>", cocoon_view) != null) {
          hasMatchingViewRequest = true;
          <xsl:apply-templates select=".">
            <xsl:with-param name="ca"><xsl:value-of select="$ca"/></xsl:with-param>
          </xsl:apply-templates>
        }
      </xsl:for-each>

      <!-- process all map:parts -->
      if (hasMatchingViewRequest) {
        <xsl:for-each select="map:part[@label]">
          <!-- invoke view, and return here -->
          <xsl:call-template name="view-label">
            <xsl:with-param name="label"><xsl:value-of select="@label"/></xsl:with-param>
          </xsl:call-template>
        </xsl:for-each>
      }
    }
    </xsl:if>
    <xsl:apply-templates select="map:part">
      <xsl:with-param name="ca"><xsl:value-of select="$ca"/></xsl:with-param>
    </xsl:apply-templates>

    <!-- process attached labels to the map:aggregate element -->
    <xsl:if test="@label">
      <xsl:call-template name="view-label">
        <xsl:with-param name="label"><xsl:value-of select="@label"/></xsl:with-param>
      </xsl:call-template>
    </xsl:if>
  </xsl:template> <!-- match="map:aggregate" -->



  <!-- generate the code to match a aggregates part definition -->
  <xsl:template match="map:part">
    <xsl:param name="ca"/>
    <xsl:if test="not(@src)">
      <xsl:call-template name="error">
        <xsl:with-param name="message">src attribute missing in aggregates part element</xsl:with-param>
      </xsl:call-template>
    </xsl:if>
    <xsl:variable name="element">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">element</xsl:with-param>
        <xsl:with-param name="default">""</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="ns">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">ns</xsl:with-param>
        <xsl:with-param name="default">""</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="strip-root">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">strip-root</xsl:with-param>
        <xsl:with-param name="default">"no"</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="prefix">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">prefix</xsl:with-param>
        <xsl:with-param name="default">""</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="$ca"/>.addPart(<xsl:apply-templates select="@src"/>, <xsl:value-of select="$element"/>, <xsl:value-of select="$ns"/>, <xsl:value-of select="$strip-root"/>, <xsl:value-of select="$prefix"/>);
  </xsl:template> <!-- match="map:part" -->



  <!-- collect parameter definitions -->
  <xsl:template match="map:pipeline//map:parameter | map:action-set//map:parameter | map:resource//map:parameter">
    <xsl:param name="param"/>
    <xsl:if test="not($param='')">
      <xsl:value-of select="$param"/><xsl:text>.setParameter (</xsl:text>
      <xsl:apply-templates select="@name"/>
      <xsl:text>, </xsl:text>
      <xsl:apply-templates select="@value"/>);
    </xsl:if>
  </xsl:template>



  <!-- attribute substitution template -->
  <xsl:template match="@*">
    <xsl:choose>
      <xsl:when test="XSLTFactoryLoader:hasSubstitutions($factory-loader, .)">
        <xsl:text>substitute(listOfMaps, "</xsl:text><xsl:value-of select="XSLTFactoryLoader:escape($factory-loader, .)"/><xsl:text>")</xsl:text>
      </xsl:when>
      <xsl:otherwise>"<xsl:value-of select="XSLTFactoryLoader:escape($factory-loader, .)"/>"</xsl:otherwise>
    </xsl:choose>
  </xsl:template>



  <!-- Sitemap Utility templates -->

  <!-- this template generates the code to configure a specific sitemap component -->
  <xsl:template name="config-components">
    <xsl:param name="name"/>
    <xsl:param name="components"/>

    <xsl:variable name="type">
      <xsl:value-of select="translate($name, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
    </xsl:variable>

    <xsl:variable name="ns" select="namespace-uri(.)"/>
    <xsl:for-each select="$components">
      <xsl:call-template name="line-number"/>
      {
        DefaultConfiguration cconf1 = new DefaultConfiguration("<xsl:value-of select="translate(@name, '- ', '__')"/>", LOCATION);
        <xsl:for-each select="attribute::*">
        <xsl:text>cconf1.addAttribute ("</xsl:text><xsl:value-of select="name(.)"/>", "<xsl:value-of select="."/>");
        </xsl:for-each>

        <!-- get nested configuration definitions -->
        <xsl:call-template name="nested-config-components">
          <xsl:with-param name="name" select="$name"/>
          <xsl:with-param name="level" select="2"/>
          <xsl:with-param name="config-name"><xsl:value-of select="concat(local-name(.),'/',@name)"/></xsl:with-param>
          <xsl:with-param name="components" select="*"/>
          <xsl:with-param name="type" select="@name"/>
          <xsl:with-param name="ns" select="$ns"/>
        </xsl:call-template>

        <xsl:choose>
          <xsl:when test="@mime-type">
          sitemap.load_component (Sitemap.<xsl:value-of select="$type"/>, "<xsl:value-of select="@name"/>
          <xsl:text>", "</xsl:text><xsl:value-of select="@src"/>", cconf1, "<xsl:value-of select="@mime-type"/>
          <xsl:text>");</xsl:text>
          </xsl:when>
          <xsl:otherwise>
          sitemap.load_component (Sitemap.<xsl:value-of select="$type"/>, "<xsl:value-of select="@name"/>
          <xsl:text>", "</xsl:text><xsl:value-of select="@src"/>
          <xsl:text>", cconf1, null);</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      }
    </xsl:for-each>
  </xsl:template>



  <!-- this template generates the code to collect configurations for a specific sitemap component -->
  <xsl:template name="nested-config-components">
    <xsl:param name="name"/>
    <xsl:param name="config-name"/>
    <xsl:param name="components"/>
    <xsl:param name="type"/>
    <xsl:param name="ns"/>
    <xsl:param name="subname"/>
    <xsl:param name="level"/>

    <!-- break on error when old map:value syntax exists -->
    <xsl:if test="attribute::map:value">
      <xsl:call-template name="error">
        <xsl:with-param name="message">"map:value" has been deprecated. Use child text nodes for the value.</xsl:with-param>
      </xsl:call-template>
    </xsl:if>

    <!-- process content -->
    <xsl:for-each select="$components">
      <xsl:call-template name="line-number"/>
      {
         DefaultConfiguration cconf<xsl:value-of select="$level"/> = new DefaultConfiguration("<xsl:value-of select="name(.)"/>", LOCATION);
      <xsl:for-each select="attribute::*">
        cconf<xsl:value-of select="$level"/>.addAttribute ("<xsl:value-of select="name(.)"/>", "<xsl:value-of select="."/>");
      </xsl:for-each>
      <xsl:if test="normalize-space(text())">
        cconf<xsl:value-of select="$level"/>.appendValueData("<xsl:value-of select="text()"/>");
      </xsl:if>
        cconf<xsl:value-of select="($level - 1)"/>.addChild(cconf<xsl:value-of select="$level"/>);
     <xsl:variable name="newsubname">
        <xsl:choose>
          <xsl:when test="not($subname)"><xsl:value-of select="position()"/></xsl:when>
          <xsl:otherwise><xsl:value-of select="concat($subname,position())"/></xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:call-template name="nested-config-components">
        <xsl:with-param name="level"><xsl:value-of select="($level + 1)"/></xsl:with-param>
        <xsl:with-param name="name"><xsl:value-of select="$name"/></xsl:with-param>
        <xsl:with-param name="config-name"><xsl:value-of select="local-name(.)"/></xsl:with-param>
        <xsl:with-param name="components" select="*"/>
        <xsl:with-param name="type"><xsl:value-of select="$type"/></xsl:with-param>
        <xsl:with-param name="ns"><xsl:value-of select="namespace-uri(.)"/></xsl:with-param>
        <xsl:with-param name="subname"><xsl:value-of select="$newsubname"/></xsl:with-param>
      </xsl:call-template>
      }
    </xsl:for-each>
  </xsl:template>


  <!-- this template is used to setup a individual sitemap component before putting it into a pipeline -->
  <xsl:template name="setup-component">
    <xsl:param name="default-component"/>
    <xsl:param name="method"/>
    <xsl:param name="prefix"/>
    <xsl:param name="mime-type"/>

    <!-- view/label 'last' check -->
    <xsl:if test="not(ancestor::map:views) and not(ancestor::map:handle-errors)">
      <xsl:if test="$prefix='serializer'">
        <xsl:for-each select="/map:sitemap/map:views/map:view[@from-position='last']">
          if (<xsl:apply-templates select="@name"/>.equals(cocoon_view)) {
            getLogger().debug("View <xsl:value-of select="@name"/>");
            return view_<xsl:value-of select="translate(@name, '- ', '__')"/> (pipeline, eventPipeline, listOfMaps, environment, internalRequest);
          }
        </xsl:for-each>
        // performing link translation
        if (environment.getObjectModel().containsKey(Constants.LINK_OBJECT)) {
            eventPipeline.addTransformer ("!link-translator!", null, Parameters.EMPTY_PARAMETERS);
        }
      </xsl:if>
    </xsl:if>

    <!-- get the type of the component -->
    <xsl:variable name="component-type">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname">type</xsl:with-param>
        <xsl:with-param name="default"><xsl:value-of select="$default-component"/></xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- get the source attribute of the component -->
    <xsl:variable name="component-source">
      <xsl:call-template name="get-parameter-substituted">
        <xsl:with-param name="parname">src</xsl:with-param>
        <xsl:with-param name="default">null</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <!-- test if we have to define parameters for this component -->
    <xsl:if test="count(map:parameter)>0">
      param = new Parameters ();
      <xsl:apply-templates select="map:parameter">
        <xsl:with-param name="param">param</xsl:with-param>
      </xsl:apply-templates>
    </xsl:if>
    <xsl:variable name="component-param">
      <xsl:choose>
        <xsl:when test="map:parameter">param</xsl:when>
        <xsl:otherwise>Parameters.EMPTY_PARAMETERS</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    getLogger().debug("Component <xsl:value-of select="$prefix"/>:<xsl:value-of select="$component-type"/>(<xsl:value-of select="$component-param"/>)");
    <xsl:choose>
      <!-- determine the right invokation according to "has a src attribute" and "has a mime-type attribute" -->
      <xsl:when test="$component-source='null'">
        <xsl:choose>
          <xsl:when test="$mime-type!=''">
            getLogger().debug("Mime-type= <xsl:value-of select="$mime-type"/>");
            <xsl:value-of select="$method"/> ("<xsl:value-of select="$component-type"/>",
              null, <xsl:value-of select="$component-param"/>,"<xsl:value-of select="$mime-type"/>"
            );
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$method"/> ("<xsl:value-of select="$component-type"/>",
              null, <xsl:value-of select="$component-param"/>
            );
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        if (debug_enabled) getLogger().debug("Source= " + <xsl:value-of select="$component-source"/>);
        <xsl:choose>
          <xsl:when test="$mime-type!=''">
            getLogger().debug("Mime-type= <xsl:value-of select="$mime-type"/>");
            <xsl:value-of select="$method"/> ("<xsl:value-of select="$component-type"/>",
              <xsl:value-of select="$component-source"/>,
              <xsl:value-of select="$component-param"/>,"<xsl:value-of select="$mime-type"/>");
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$method"/> ("<xsl:value-of select="$component-type"/>",
              <xsl:value-of select="$component-source"/><xsl:text>,</xsl:text>
              <xsl:value-of select="$component-param"/>);
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>

    <!-- view/label check -->
    <xsl:if test="not(ancestor::map:views) and not(ancestor::map:handle-errors) and not(ancestor-or-self::map:aggregate)">
      <xsl:if test="$prefix='generator'">
        <xsl:for-each select="/map:sitemap/map:views/map:view[@from-position='first']">
          if ("<xsl:value-of select="@name"/>".equals(cocoon_view)) {
            return view_<xsl:value-of select="translate(@name, '- ', '__')"/> (pipeline, eventPipeline, listOfMaps, environment, internalRequest);
          }
        </xsl:for-each>
      </xsl:if>

      <xsl:if test="@label">
        <xsl:call-template name="view-label">
          <xsl:with-param name="label"><xsl:value-of select="@label"/></xsl:with-param>
        </xsl:call-template>
      </xsl:if>

      <xsl:variable name="component-label">
        <xsl:if test="$prefix='generator'">
          <xsl:value-of select="/map:sitemap/map:components/map:generators/map:generator[@name=$component-type]/@label"/>
        </xsl:if>
        <xsl:if test="$prefix='transformer'">
          <xsl:value-of select="/map:sitemap/map:components/map:transformers/map:transformer[@name=$component-type]/@label"/>
        </xsl:if>
      </xsl:variable>
      <xsl:if test="string-length($component-label) &gt; 0">
        <xsl:call-template name="view-label">
          <xsl:with-param name="label"><xsl:value-of select="$component-label"/></xsl:with-param>
        </xsl:call-template>
      </xsl:if>
    </xsl:if>
  </xsl:template>



  <!-- generate the code to match a label definition -->
  <xsl:template name="view-label">
    <xsl:param name="label"/>
    {
      String view_name = null;
      if ((view_name = contains_view("<xsl:value-of select="$label"/>", cocoon_view)) != null) {
        return call_view(view_name, pipeline, eventPipeline, listOfMaps, environment, internalRequest);
      }
    }
  </xsl:template>



  <!-- replace invalid characters with underscores -->
  <xsl:template name="generate-name">
    <xsl:param name="prefix"/>
    <xsl:param name="suffix"/>
    <xsl:value-of select="translate(concat($prefix,$suffix),'- ','__')"/>
  </xsl:template>



  <xsl:template name="get-parameter-substituted">
    <xsl:param name="parname"/>
    <xsl:param name="default"/>
    <xsl:param name="required">false</xsl:param>

    <xsl:variable name="result">
      <xsl:call-template name="get-parameter">
        <xsl:with-param name="parname"  select="$parname"/>
        <xsl:with-param name="default"  select="$default"/>
        <xsl:with-param name="required" select="$required"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$result=$default"><xsl:value-of select="$default"/></xsl:when>
      <xsl:when test="contains($result, '{')">substitute(listOfMaps, "<xsl:value-of select="$result"/>")</xsl:when>
      <xsl:otherwise>"<xsl:value-of select="$result"/>"</xsl:otherwise>
    </xsl:choose>
  </xsl:template>



  <!-- These are the usual utility templates for logicsheets -->
  <xsl:template name="get-parameter">
    <xsl:param name="parname"/>
    <xsl:param name="default"/>
    <xsl:param name="required">false</xsl:param>

    <xsl:choose>
      <xsl:when test="@*[name(.) = $parname]"><xsl:value-of select="@*[name(.) = $parname]"/></xsl:when>
      <xsl:when test="string-length($default) = 0">
        <xsl:choose>
          <xsl:when test="$required = 'true'">
            <xsl:call-template name="error">
              <xsl:with-param name="message">
                [Logicsheet processor] Attribute '<xsl:value-of select="$parname"/>' missing in dynamic tag &lt;<xsl:value-of select="name(.)"/>&gt;
              </xsl:with-param>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise></xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise><xsl:copy-of select="$default"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>



  <xsl:template name="error">
    <xsl:param name="message"/>
    <xsl:message terminate="yes"><xsl:value-of select="$message"/></xsl:message>
  </xsl:template>

  <xsl:template name="warn">
    <xsl:param name="message"/>
    <xsl:message terminate="no"><xsl:value-of select="$message"/></xsl:message>
  </xsl:template>

  <!-- Ignored elements -->
  <xsl:template match="map:logicsheet|map:dependency|map:handle-errors"/>

  <xsl:template match="*" priority="-1">
    <xsl:call-template name="error">
      <xsl:with-param name="message">Sitemap contains invalid tag: <xsl:value-of select="local-name()"/>.</xsl:with-param>
    </xsl:call-template>
  </xsl:template>
</xsl:stylesheet>
