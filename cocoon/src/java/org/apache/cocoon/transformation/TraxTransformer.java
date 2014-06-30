/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.TimeStampCacheValidity;
import org.apache.cocoon.components.browser.Browser;
import org.apache.cocoon.components.xslt.XSLTProcessor;
import org.apache.cocoon.components.deli.Deli;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.Cookie;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.HashUtil;
import org.apache.cocoon.xml.XMLConsumer;
import org.xml.sax.SAXException;

import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.sax.TransformerHandler;
import java.io.IOException;
import java.text.StringCharacterIterator;
import java.util.*;

/**
 * This Transformer is used to transform this incomming SAX stream using
 * a XSLT stylesheet. Use the following sitemap declarations to define, configure
 * and parameterize it:
 *
 * <b>In the map:sitemap/map:components/map:transformers:</b><br>
 * <pre>
 * &lt;map:transformer name="xslt" src="org.apache.cocoon.transformation.TraxTransformer"&gt;<br>
 *   &lt;use-request-parameters&gt;false&lt;/use-request-parameters&gt;
 *   &lt;use-browser-capabilities-db&gt;false&lt;/use-browser-capabilities-db&gt;
 *   &lt;use-session-info&gt;false&lt;/use-session-info&gt;
 *   &lt;xslt-processor-role&gt;org.apache.cocoon.components.xslt.XSLTProcessor&lt;/xslt-processor-role&gt;
 * &lt;/map:transformer&gt;
 * </pre>
 *
 * The &lt;use-request-parameter&gt; configuration forces the transformer to make all
 * request parameters available in the XSLT stylesheet. Note that this might have issues
 * concerning cachability of the generated output of this transformer.<br>
 * This property is false by default.
 * <p>
 * The &lt;use-browser-capabilities-db&gt; configuration forces the transformer to make all
 * properties from the browser capability database available in the XSLT stylesheetas.
 * Note that this might have issues concerning cachability of the generated output of this
 * transformer.<br>
 * This property is false by default.
 * <p>
 * The &lt;use-cookies&gt; configuration forces the transformer to make all
 * cookies from the request available in the XSLT stylesheetas.
 * Note that this might have issues concerning cachability of the generated output of this
 * transformer.<br>
 * This property is false by default.
 * <p>
 * The &lt;use-session-info&gt; configuration forces the transformer to make all
 * of the session information available in the XSLT stylesheetas.<br>
 * These infos are (boolean values are "true" or "false" strings: session-is-new,
 * session-id-from-cookie, session-id-from-url, session-valid, session-id.<br>
 * This property is false by default.
 *
 * <p> The &lt;use-deli&gt; configuration forces the transformer to
 * make all the properties from the CC/PP profile resolved from the
 * request available in the XSLT stylesheets.  CC/PP support is
 * provided via the DELI library. If the request does not provide
 * CC/PP information, then CC/PP information can added via the DELI
 * legacy device database.  This property is false by default.
 *
 * <p>Note that these properties might introduces issues concerning
 * cacheability of the generated output of this transformer.<br>
 *
 *
 * The &lt;xslt-processor&gt; configuration allows to specify the XSLT processor that will be
 * used by its role name. This allows to have several XSLT processors in the configuration
 * (e.g. Xalan and Saxon) and choose one or the other depending on the needs of stylesheet
 * specificities.<br>
 * This property defaults to "org.apache.cocoon.components.xslt.XSLTProcessor" which is the
 * standard role name for an XSLTProcessor.
 * <p>
 * <b>In a map:sitemap/map:pipelines/map:pipeline:</b><br>
 * <pre>
 * &lt;map:transform type="xslt" src="stylesheets/yours.xsl"&gt;<br>
 *   &lt;parameter name="myparam" value="myvalue"/&gt;
 * &lt;/map:transform&gt;
 * </pre>
 * All &lt;parameter&gt; declarations will be made available in the XSLT stylesheet as
 * xsl:variables.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:giacomo@apache.org">Giacomo Pati</a>
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @author <a href="mailto:marbut@hplb.hpl.hp.com">Mark H. Butler</a>
 * @version CVS $Id: TraxTransformer.java,v 1.8 2002/01/23 18:47:43 vgritsenko Exp $
 */
public class TraxTransformer extends AbstractTransformer
implements Transformer, Composable, Recyclable, Configurable, Cacheable, Disposable {

    private static final String FILE = "file:";

    /** The Browser service instance */
    private Browser browser = null;

    /** The DELI service instance */
    private Deli deli = null;

    /** Should we make the request parameters available in the stylesheet? (default is off) */
    private boolean useParameters = false;
    private boolean _useParameters = false;

    /** Should we make the browser capability properties available in the stylesheet? (default is off) */
    private boolean useBrowserCap = false;
    private boolean _useBrowserCap = false;

    /* Should we make the DELI CC/PP profile available to the stylesheet (default is off) */
    private boolean useDeli = false;
    private boolean _useDeli = false;

    /** Should we make the cookies availalbe in the stylesheet? (default is off) */
    private boolean useCookies = false;
    private boolean _useCookies = false;

    /** Should we info about the session availalbe in the stylesheet? (default is off) */
    private boolean useSessionInfo = false;
    private boolean _useSessionInfo = false;

    private ComponentManager manager;

    /** The trax TransformerHandler */
    TransformerHandler transformerHandler;
    /** The Source */
    private Source inputSource;
    /** The parameters */
    private Parameters par;
    /** The object model */
    private Map objectModel;

    /** The XSLTProcessor */
    XSLTProcessor xsltProcessor;

    /**
     * Configure this transformer.
     */
    public void configure(Configuration conf)
    throws ConfigurationException {
        if (conf != null) {
          Configuration child;

          child = conf.getChild("use-request-parameters");
          this.useParameters = child.getValueAsBoolean(false);
          this._useParameters = this.useParameters;

          child = conf.getChild("use-cookies");
          this.useCookies = child.getValueAsBoolean(false);
          this._useCookies = this.useCookies;

          child = conf.getChild("use-browser-capabilities-db");
          this.useBrowserCap = child.getValueAsBoolean(false);
          this._useBrowserCap = this.useBrowserCap;

          child = conf.getChild("use-session-info");
          this.useSessionInfo = child.getValueAsBoolean(false);
          this._useSessionInfo = this.useSessionInfo;

          child = conf.getChild("xslt-processor-role");
          String xsltRole = child.getValue(XSLTProcessor.ROLE);

          child = conf.getChild("use-deli");
          this.useDeli = child.getValueAsBoolean(false);
          this._useDeli = this.useDeli;

          if (this.getLogger().isDebugEnabled()) {
                this.getLogger().debug("Use parameters is " + this.useParameters + " for " + this);
                this.getLogger().debug("Use cookies is " + this.useCookies + " for " + this);
                this.getLogger().debug("Use browser capabilities is " + this.useBrowserCap + " for " + this);
                this.getLogger().debug("Use session info is " + this.useSessionInfo + " for " + this);
                this.getLogger().debug("Use DELI is " + this.useDeli + " for " + this);
                this.getLogger().debug("Use XSLTProcessor of role " + xsltRole);
            }

            try {
                this.xsltProcessor = (XSLTProcessor)this.manager.lookup(xsltRole);
            } catch (ComponentException e) {
                throw new ConfigurationException("Cannot load XSLT processor", e);
            }
        }
    }

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;

        if (this.getLogger().isDebugEnabled()) {
            getLogger().debug("Looking up " + Browser.ROLE);
        }
        this.browser = (Browser) manager.lookup(Browser.ROLE);

        // FIXME (VG): Replace try with hasComponent() check when it works as expected
        // System.out.println(Deli.ROLE + ": " + this.manager.hasComponent(Deli.ROLE));
        // if (this.manager.hasComponent(Deli.ROLE)) {
        try {
            if (this.getLogger().isDebugEnabled()) {
                getLogger().debug("Looking up " + Deli.ROLE);
            }
            this.deli = (Deli) this.manager.lookup(Deli.ROLE);
        // } else {
        } catch (ComponentException e) {
            getLogger().debug("Deli is not available");
        } catch (NoClassDefFoundError e) {
            getLogger().debug("Deli is not available");
        }
    }

    /**
     * Set the <code>SourceResolver</code>, the <code>Map</code> with
     * the object model, the source and sitemap
     * <code>Parameters</code> used to process the request.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
    throws SAXException, ProcessingException, IOException {

        // Check the stylesheet uri
        if (src == null) {
            throw new ProcessingException("Stylesheet URI can't be null");
        }
        this.par = par;
        this.objectModel = objectModel;
        this.inputSource = resolver.resolve(src);
        xsltProcessor.setSourceResolver(resolver);
        _useParameters = par.getParameterAsBoolean("use-request-parameters", this.useParameters);
        _useBrowserCap = par.getParameterAsBoolean("use-browser-capabilities-db", this.useBrowserCap);
        _useDeli = par.getParameterAsBoolean("use-deli", this.useDeli);
        _useCookies = par.getParameterAsBoolean("use-cookies", this.useCookies);
        _useSessionInfo = par.getParameterAsBoolean("use-session-info", this.useSessionInfo);
        if (this.getLogger().isDebugEnabled()) {
            this.getLogger().debug("Using stylesheet: '"+this.inputSource.getSystemId()+"' in " + this + ", last modified: " + this.inputSource.getLastModified());
        }

        /** Get a Transformer Handler */
        this.transformerHandler = this.xsltProcessor.getTransformerHandler(inputSource);
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     *
     * @return The generated key hashes the src
     */
    public long generateKey() {
        if (this.inputSource.getLastModified() != 0) {
            HashMap map = getLogicSheetParameters();
            if (map == null) {
                return HashUtil.hash(this.inputSource.getSystemId());
            }

            StringBuffer sb = new StringBuffer();
            sb.append(this.inputSource.getSystemId());
            Set entries = map.entrySet();
            for(Iterator i=entries.iterator(); i.hasNext();){
                sb.append(';');
                Map.Entry entry = (Map.Entry)i.next();
                sb.append(entry.getKey());
                sb.append('=');
                sb.append(entry.getValue());
            }
            return HashUtil.hash(sb.toString());
        }
        return 0;
    }

    /**
     * Generate the validity object.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        /*
        * VG: Key is generated using parameter/value pairs,
        * so this information does not need to be verified again
        * (if parameter added/removed or value changed, key should
        * change also), only stylesheet's modified time is included.
        */
        if (this.inputSource.getLastModified() != 0) {
            return new TimeStampCacheValidity(this.inputSource.getLastModified());
        }
        return null;
    }

    /**
     * Set the <code>XMLConsumer</code> that will receive XML data.
     */
    public void setConsumer(XMLConsumer consumer) {

        HashMap map = getLogicSheetParameters();
        if (map != null) {
            Iterator iterator = map.keySet().iterator();
            while(iterator.hasNext()) {
                String name = (String)iterator.next();
                transformerHandler.getTransformer().setParameter(name,map.get(name));
            }
        }

        super.setContentHandler(transformerHandler);
        super.setLexicalHandler(transformerHandler);

        if(transformerHandler instanceof Loggable) {
            ((Loggable)transformerHandler).setLogger(getLogger());
        }
        // According to TrAX specs, all TransformerHandlers are LexicalHandlers
        SAXResult result = new SAXResult(consumer);
        result.setLexicalHandler(consumer);
        transformerHandler.setResult(result);
    }

    private HashMap getLogicSheetParameters() {
        HashMap map = null;
        if (par != null) {
            String[] params = par.getNames();
            if (params != null) {
                for(int i = 0; i < params.length; i++) {
                    String name = (String) params[i];
                    if (isValidXSLTParameterName(name)) {
                        String value = par.getParameter(name,null);
                        if (value != null) {
                            if (map == null) {
                                map = new HashMap();
                            }
                            map.put(name,value);
                        }
                    }
                }
            }
        }

        if (this._useParameters) {
            /** The Request object */
            Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);

            Enumeration parameters = request.getParameterNames();
            if ( parameters != null ) {
                while (parameters.hasMoreElements()) {
                    String name = (String) parameters.nextElement();
                    if (isValidXSLTParameterName(name)) {
                        String value = request.getParameter(name);
                        if (map == null) {
                            map = new HashMap();
                        }
                        map.put(name,value);
                    }
                }
            }
        }

        if (this._useSessionInfo) {
            /** The Request object */
            Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
            if (map == null) map = new HashMap(5);

            Session session = request.getSession(false);
            if (session != null) {
                map.put("session-available","true");
                map.put("session-is-new",session.isNew()?"true":"false");
                map.put("session-id-from-cookie",request.isRequestedSessionIdFromCookie()?"true":"false");
                map.put("session-id-from-url",request.isRequestedSessionIdFromURL()?"true":"false");
                map.put("session-valid",request.isRequestedSessionIdValid()?"true":"false");
                map.put("session-id",session.getId());
            } else {
                map.put("session-available","false");
            }
        }

        if (this._useCookies) {
            Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
            Cookie cookies[] = request.getCookies();
            if (cookies != null) {
                for (int i=0; i<cookies.length; i++) {
                    String name = cookies[i].getName();
                    if (isValidXSLTParameterName(name)) {
                        String value = cookies[i].getValue();
                        if (map == null) {
                            map = new HashMap();
                        }
                        map.put(name,value);
                    }
                }
            }
        }

        if (this._useBrowserCap) try {
            Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
            if (map == null) {
                map = new HashMap();
            }

            /* Get the accept header; it's needed to get the browser type. */
            String accept = request.getParameter("accept");
            if (accept == null)
                accept = request.getHeader("accept");

            /* Get the user agent; it's needed to get the browser type. */
            String agent = request.getParameter("user-agent");
            if (agent == null)
                agent = request.getHeader("user-agent");

            /* add the accept param */
            map.put("accept", accept);

            /* add the user agent param */
            map.put("user-agent", java.net.URLEncoder.encode(agent));

            /* add the map param */
            HashMap agmap = browser.getBrowser(agent, accept);
            map.put("browser", agmap);

            /* add the media param */
            String browserMedia = browser.getMedia(agmap);
            map.put("browser-media", browserMedia);

            /* add the uaCapabilities param */
            org.w3c.dom.Document uaCapabilities = browser.getUACapabilities(agmap);
            map.put("ua-capabilities", uaCapabilities);
        } catch (Exception e) {
            getLogger().error("Error setting Browser info", e);
        }

        if (this._useDeli) {
            try {
                Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
                if (map == null) {
                    map = new HashMap();
                }

                if (this.deli != null) {
                    org.w3c.dom.Document deliCapabilities = this.deli.getUACapabilities(request);
                    map.put("deli-capabilities", deliCapabilities);
    
                    String accept = request.getParameter("accept");
                    if (accept == null)
                        accept = request.getHeader("accept");
    
                    /* add the accept param */
                    map.put("accept", accept);
                } else {
                    getLogger().warn("TraxTransformer configured to use DELI in sitemap.xmap but DELI not enabled in cocoon.xconf.");
                }

            } catch (Exception e) {
                getLogger().error("Error setting DELI info", e);
            }
        }

        return map;
    }

    // FIXME (SM): this method may be a hotspot for requests with many
    //             parameters we should try to optimize it further
    static boolean isValidXSLTParameterName(String name) {
        StringCharacterIterator iter = new StringCharacterIterator(name);
        char c = iter.first();
        if (!(Character.isLetter(c) || c == '_')) {
            return false;
        } else {
            c = iter.next();
        }
        while (c != iter.DONE) {
            if (!(Character.isLetterOrDigit(c) ||
                c == '-' ||
                c == '_' ||
                c == '.')) {
                return false;
            } else {
                c = iter.next();
            }
        }

        return true;
    }

    public void dispose()
    {
        if(this.browser != null)
            this.manager.release((Component)this.browser);
        if(this.xsltProcessor != null)
            this.manager.release((Component)this.xsltProcessor);
        if(this.deli != null)
            this.manager.release((Component)this.deli);
    }

    public void recycle()
    {
        //FIXME: Patch for Xalan2J, to stop transform threads if
        //       there is a failure in the pipeline.
        /* VG: Xalan 2.2.0 does not have this class.
               Does it stop now transform threads correctly?
        try {
            Class clazz =
                Thread.currentThread().getContextClassLoader().loadClass("org.apache.xalan.stree.SourceTreeHandler");
            Class  paramTypes[] =
                    new Class[]{ Exception.class };
            Object params[] =
                    new Object[] { new SAXException("Dummy Exception") };
            if(clazz.isInstance(transformerHandler)) {
                Method method = clazz.getMethod("setExceptionThrown",paramTypes);
                method.invoke(transformerHandler,params);
            }
        } catch (Exception e){
            getLogger().debug("Exception in recycle:", e);
        }
        */
        this.transformerHandler = null;
        this.objectModel = null;
        if (this.inputSource != null) {
          this.inputSource.recycle();
          this.inputSource = null;
        }
        this.par = null;
        this._useParameters = this.useParameters;
        this._useCookies = this.useCookies;
        this._useBrowserCap = this.useBrowserCap;
        this._useDeli = this.useDeli;
        this._useSessionInfo = this.useSessionInfo;
        super.recycle();
    }
}
