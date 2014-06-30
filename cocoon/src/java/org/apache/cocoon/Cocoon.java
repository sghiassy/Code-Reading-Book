/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 2001 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Axis" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

package org.apache.cocoon;

import org.apache.avalon.excalibur.component.DefaultRoleManager;
import org.apache.avalon.excalibur.component.ExcaliburComponentManager;
import org.apache.avalon.excalibur.logger.LogKitManageable;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.SAXConfigurationHandler;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.components.language.generator.CompiledComponent;
import org.apache.cocoon.components.language.generator.ProgramGenerator;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.components.source.SourceHandler;
import org.apache.cocoon.components.source.URLSource;
import org.apache.cocoon.components.store.FilesystemStore;
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.ModifiableSource;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.sitemap.SitemapManager;
import org.apache.cocoon.util.ClassUtils;
import org.xml.sax.InputSource;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Map;

/**
 * The Cocoon Object is the main Kernel for the entire Cocoon system.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a> (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 * @version CVS $Revision: 1.5 $ $Date: 2002/01/22 00:17:11 $
 */
public class Cocoon
        extends AbstractLoggable
        implements ThreadSafe,
                   Component,
                   Initializable,
                   Disposable,
                   Modifiable,
                   Processor,
                   Contextualizable,
                   Composable,
                   LogKitManageable {

    /** The application context */
    private Context context;

    /** The configuration file */
    private ModifiableSource configurationFile;

    /** The configuration tree */
    private Configuration configuration;

    /** The logkit manager */
    private LogKitManager logKitManager;

    /** The classpath (null if not available) */
    private String classpath;

    /** The working directory (null if not available) */
    private File workDir;

    /** The component manager. */
    private ExcaliburComponentManager componentManager;

    /** The parent component manager. */
    private ComponentManager parentComponentManager;

    /** flag for disposed or not */
    private boolean disposed = false;

    /** active request count */
    private volatile int activeRequestCount = 0;

    /** the Processor if it is ThreadSafe */
    private Processor threadSafeProcessor = null;

    /**
     * Creates a new <code>Cocoon</code> instance.
     *
     * @exception ConfigurationException if an error occurs
     */
    public Cocoon() throws ConfigurationException {
        // Set the system properties needed by Xalan2.
        setSystemProperties();
    }

    /**
     * Get the parent component manager. For purposes of
     * avoiding extra method calls, the manager parameter may be null.
     *
     * @param manager the parent component manager. May be <code>null</code>
     */
    public void compose(ComponentManager manager) {
        this.parentComponentManager = manager;
    }

    /**
     * Describe <code>contextualize</code> method here.
     *
     * @param context a <code>Context</code> value
     * @exception ContextException if an error occurs
     */
    public void contextualize(Context context) throws ContextException {
        if (this.context == null) {
            this.context = context;
            this.classpath = (String)context.get(Constants.CONTEXT_CLASSPATH);
            this.workDir = (File)context.get(Constants.CONTEXT_WORK_DIR);
            try {
                this.configurationFile = new URLSource((URL)context.get(Constants.CONTEXT_CONFIG_URL),
                                                       this.componentManager);


            } catch (IOException ioe) {
                getLogger().error("Could not open configuration file.", ioe);
                throw new ContextException("Could not open configuration file.", ioe);
            } catch (Exception e) {
                getLogger().error("contextualize(..) Exception", e);
                throw new ContextException("contextualize(..) Exception", e);
            }
        }
    }

    /**
     * The <code>setLogKitManager</code> method will get a <code>LogKitManager</code>
     * for further use.
     *
     * @param logKitManager a <code>LogKitManager</code> value
     */
    public void setLogKitManager(LogKitManager logKitManager) {
        this.logKitManager = logKitManager;
    }

    /**
     * The <code>initialize</code> method
     *
     * @exception Exception if an error occurs
     */
    public void initialize() throws Exception {
        if (parentComponentManager != null) {
            this.componentManager = new ExcaliburComponentManager(parentComponentManager,(ClassLoader)this.context.get(Constants.CONTEXT_CLASS_LOADER));
        } else {
            this.componentManager = new ExcaliburComponentManager((ClassLoader)this.context.get(Constants.CONTEXT_CLASS_LOADER));
        }
        this.componentManager.setLogger(getLogger());
        this.componentManager.contextualize(this.context);

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("New Cocoon object.");
        }

        // Log the System Properties.
        dumpSystemProperties();

        // Setup the default parser, for parsing configuration.
        // If one need to use a different parser, set the given system property
        String parser = System.getProperty(Constants.PARSER_PROPERTY, Constants.DEFAULT_PARSER);
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Using parser: " + parser);
        }
        ExcaliburComponentManager startupManager = new ExcaliburComponentManager((ClassLoader)this.context.get(Constants.CONTEXT_CLASS_LOADER));
        startupManager.setLogger(getLogger());
        startupManager.contextualize(this.context);
        startupManager.setLogKitManager(this.logKitManager);

        try {
            startupManager.addComponent(Parser.ROLE, ClassUtils.loadClass(parser), new org.apache.avalon.framework.configuration.DefaultConfiguration("", "empty"));
        } catch (Exception e) {
            getLogger().error("Could not load parser, Cocoon object not created.", e);
            throw new ConfigurationException("Could not load parser " + parser, e);
        }

        try {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Creating Repository with this directory: " + this.workDir);
            }
            FilesystemStore repository = new FilesystemStore();
            repository.setLogger(getLogger());
            repository.setDirectory(this.workDir);
            this.componentManager.addComponentInstance(Store.ROLE + "/Filesystem", repository);
        } catch (IOException e) {
            getLogger().error("Could not create repository!", e);
            throw new ConfigurationException("Could not create the repository!", e);
        }

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Classpath = " + classpath);
            getLogger().debug("Work directory = " + workDir.getCanonicalPath());
        }
        startupManager.initialize();

        Configuration conf = this.configure(startupManager);
        startupManager.dispose();
        startupManager = null;

        this.componentManager.initialize();

        // Get the Processor and keep it if it's ThreadSafe
        Processor processor = (Processor)this.componentManager.lookup(Processor.ROLE);
        if (processor instanceof ThreadSafe) {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Processor of class " + processor.getClass().getName() + " is ThreadSafe");
            }
            this.threadSafeProcessor = processor;
        } else {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("Processor of class " + processor.getClass().getName() +
                " is NOT ThreadSafe -- will be looked up at each request");
            }
            this.componentManager.release(processor);
        }
    }

    /** Dump System Properties */
    private void dumpSystemProperties() {
      try {
         Enumeration e = System.getProperties().propertyNames();
         if (getLogger().isDebugEnabled()) {
             getLogger().debug("===== System Properties Start =====");
         }
         for (;e.hasMoreElements();) {
            String key = (String) e.nextElement();
            if (getLogger().isDebugEnabled()) {
                getLogger().debug(key + "=" + System.getProperty(key));
            }
         }
         if (getLogger().isDebugEnabled()) {
             getLogger().debug("===== System Properties End =====");
         }
       } catch ( SecurityException se ) {
         // Ignore Exceptions.
       }
    }

    /**
     * Configure this <code>Cocoon</code> instance.
     *
     * @param startupManager an <code>ExcaliburComponentManager</code> value
     * @return a <code>Configuration</code> value
     * @exception ConfigurationException if an error occurs
     * @exception ContextException if an error occurs
     */
    public Configuration configure(ExcaliburComponentManager startupManager) throws ConfigurationException, ContextException {
        Parser p = null;
        Configuration roleConfig = null;

        try {
            this.configurationFile.refresh();
            p = (Parser)startupManager.lookup(Parser.ROLE);
            SAXConfigurationHandler b = new SAXConfigurationHandler();
            InputStream inputStream = ClassUtils.getResource("org/apache/cocoon/cocoon.roles").openStream();
            InputSource is = new InputSource(inputStream);
            is.setSystemId(this.configurationFile.getSystemId());
            p.setContentHandler(b);
            p.parse(is);
            roleConfig = b.getConfiguration();
        } catch (Exception e) {
            getLogger().error("Could not configure Cocoon environment", e);
            throw new ConfigurationException("Error trying to load configurations", e);
        } finally {
            if (p != null) startupManager.release(p);
        }

        DefaultRoleManager drm = new DefaultRoleManager();
        drm.setLogger(getLogger());
        drm.configure(roleConfig);
        roleConfig = null;

        try {
            p = (Parser)startupManager.lookup(Parser.ROLE);
            SAXConfigurationHandler b = new SAXConfigurationHandler();
            InputSource is = this.configurationFile.getInputSource();
            p.setContentHandler(b);
            p.parse(is);
            this.configuration = b.getConfiguration();
        } catch (Exception e) {
            getLogger().error("Could not configure Cocoon environment", e);
            throw new ConfigurationException("Error trying to load configurations",e);
        } finally {
            if (p != null) startupManager.release(p);
        }

        Configuration conf = this.configuration;

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Root configuration: " + conf.getName());
        }
        if (! "cocoon".equals(conf.getName())) {
            throw new ConfigurationException("Invalid configuration file\n" + conf.toString());
        }
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Configuration version: " + conf.getAttribute("version"));
        }
        if (!Constants.CONF_VERSION.equals(conf.getAttribute("version"))) {
            throw new ConfigurationException("Invalid configuration schema version. Must be '" + Constants.CONF_VERSION + "'.");
        }

        String userRoles = conf.getAttribute("user-roles", "");
        if (!"".equals(userRoles)) {
            try {
                p = (Parser)startupManager.lookup(Parser.ROLE);
                SAXConfigurationHandler b = new SAXConfigurationHandler();
                org.apache.cocoon.environment.Context context =
                    (org.apache.cocoon.environment.Context) this.context.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);
                URL url = context.getResource(userRoles);
                InputSource is = new InputSource(new BufferedInputStream(url.openStream()));
                is.setSystemId(this.configurationFile.getSystemId());
                p.setContentHandler(b);
                p.parse(is);
                roleConfig = b.getConfiguration();
            } catch (Exception e) {
                getLogger().error("Could not configure Cocoon environment", e);
                throw new ConfigurationException("Error trying to load configurations", e);
            } finally {
                if (p != null) startupManager.release(p);
            }

            DefaultRoleManager urm = new DefaultRoleManager(drm);
            urm.setLogger(getLogger());
            urm.configure(roleConfig);
            roleConfig = null;
            drm = urm;
        }

        this.componentManager.setRoleManager(drm);
        this.componentManager.setLogKitManager(this.logKitManager);

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Setting up components...");
        }
        this.componentManager.configure(conf);

        return conf;
    }

    /**
     * Queries the class to estimate its ergodic period termination.
     *
     * @param date a <code>long</code> value
     * @return a <code>boolean</code> value
     */
    public boolean modifiedSince(long date) {
        this.configurationFile.refresh();
        return date < this.configurationFile.getLastModified();
    }

    /**
     * Sets required system properties.
     */
    protected void setSystemProperties() {
        java.util.Properties props = new java.util.Properties();
        // FIXME We shouldn't have to specify the SAXParser...
        // This is needed by Xalan2, it is used by org.xml.sax.helpers.XMLReaderFactory
        // to locate the SAX2 driver.
        props.put("org.xml.sax.driver", "org.apache.xerces.parsers.SAXParser");
        java.util.Properties systemProps = System.getProperties();
        Enumeration propEnum = props.propertyNames();
        while (propEnum.hasMoreElements()) {
            String prop = (String)propEnum.nextElement();
            if (!systemProps.containsKey(prop)) {
                systemProps.put(prop, props.getProperty(prop));
            }
        }
        // FIXME We shouldn't have to specify these. Needed to override jaxp implementation of weblogic.
        if (systemProps.containsKey("javax.xml.parsers.DocumentBuilderFactory") &&
           systemProps.getProperty("javax.xml.parsers.DocumentBuilderFactory").startsWith("weblogic")) {
            systemProps.put("javax.xml.parsers.DocumentBuilderFactory", "org.apache.xerces.jaxp.DocumentBuilderFactoryImpl");
            systemProps.put("javax.xml.parsers.SAXParserFactory","org.apache.xerces.jaxp.SAXParserFactoryImpl");
        }
        System.setProperties(systemProps);
    }

    /**
     * Dispose this instance
     */
    public void dispose() {
        this.componentManager.release(this.threadSafeProcessor);
        this.componentManager.dispose();
        if (this.configurationFile != null) {
            this.configurationFile.recycle();
        }
        this.disposed = true;
    }

    /**
     * Log debug information about the current environment.
     *
     * @param environment an <code>Environment</code> value
     * @param pipeline a <code>StreamPipeline</code> value
     * @param eventPipeline an <code>EventPipeline</code> value
     */
    protected void debug(Environment environment,
                         StreamPipeline pipeline,
                         EventPipeline eventPipeline) {
        String lineSeparator = System.getProperty("line.separator");
        Map objectModel = environment.getObjectModel();
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        Session session = request.getSession(false);
        StringBuffer msg = new StringBuffer();
        msg.append("DEBUGGING INFORMATION:").append(lineSeparator);
        if (pipeline != null && eventPipeline != null) {
            msg.append("INTERNAL ");
        }
        msg.append("REQUEST: ").append(request.getRequestURI()).append(lineSeparator).append(lineSeparator);
        msg.append("CONTEXT PATH: ").append(request.getContextPath()).append(lineSeparator);
        msg.append("SERVLET PATH: ").append(request.getServletPath()).append(lineSeparator);
        msg.append("PATH INFO: ").append(request.getPathInfo()).append(lineSeparator).append(lineSeparator);

        msg.append("REMOTE HOST: ").append(request.getRemoteHost()).append(lineSeparator);
        msg.append("REMOTE ADDRESS: ").append(request.getRemoteAddr()).append(lineSeparator);
        msg.append("REMOTE USER: ").append(request.getRemoteUser()).append(lineSeparator);
        msg.append("REQUEST SESSION ID: ").append(request.getRequestedSessionId()).append(lineSeparator);
        msg.append("REQUEST PREFERRED LOCALE: ").append(request.getLocale().toString()).append(lineSeparator);
        msg.append("SERVER HOST: ").append(request.getServerName()).append(lineSeparator);
        msg.append("SERVER PORT: ").append(request.getServerPort()).append(lineSeparator).append(lineSeparator);

        msg.append("METHOD: ").append(request.getMethod()).append(lineSeparator);
        msg.append("CONTENT LENGTH: ").append(request.getContentLength()).append(lineSeparator);
        msg.append("PROTOCOL: ").append(request.getProtocol()).append(lineSeparator);
        msg.append("SCHEME: ").append(request.getScheme()).append(lineSeparator);
        msg.append("AUTH TYPE: ").append(request.getAuthType()).append(lineSeparator).append(lineSeparator);
        msg.append("CURRENT ACTIVE REQUESTS: ").append(activeRequestCount).append(lineSeparator);

        // log all of the request parameters
        Enumeration e = request.getParameterNames();

        msg.append("REQUEST PARAMETERS:").append(lineSeparator).append(lineSeparator);

        while (e.hasMoreElements()) {
            String p = (String) e.nextElement();

            msg.append("PARAM: '").append(p).append("' ")
               .append("VALUES: '");
            String[] params = request.getParameterValues(p);
            for (int i = 0; i < params.length; i++) {
                msg.append("[" + params[i] + "]");
                if (i != (params.length - 1)) {
                    msg.append(", ");
                }
            }

            msg.append("'").append(lineSeparator);
        }

        // log all of the header parameters
        Enumeration e2 = request.getHeaderNames();

        msg.append("HEADER PARAMETERS:").append(lineSeparator).append(lineSeparator);

        while (e2.hasMoreElements()) {
            String p = (String) e2.nextElement();

            msg.append("PARAM: '").append(p).append("' ")
               .append("VALUES: '");
            Enumeration e3 = request.getHeaders(p);
            while (e3.hasMoreElements()) {
                msg.append("[" + e3.nextElement() + "]");
                if (e3.hasMoreElements()) {
                    msg.append(", ");
                }
            }

            msg.append("'").append(lineSeparator);
        }

        msg.append(lineSeparator).append("SESSION ATTRIBUTES:").append(lineSeparator).append(lineSeparator);

        // log all of the session attributes
        if (session != null) {
             e = session.getAttributeNames();

            while (e.hasMoreElements()) {
                String p = (String) e.nextElement();

                msg.append("PARAM: '").append(p).append("' ")
                   .append("VALUE: '").append(session.getAttribute(p)).append("'").append(lineSeparator);
            }
        }

        if (getLogger().isDebugEnabled()) {
            getLogger().debug(msg.toString());
        }
    }

    /**
     * Process the given <code>Environment</code> to produce the output.
     *
     * @param environment an <code>Environment</code> value
     * @return a <code>boolean</code> value
     * @exception Exception if an error occurs
     */
    public boolean process(Environment environment)
    throws Exception {
        if (disposed) {
            throw new IllegalStateException("You cannot process a Disposed Cocoon engine.");
        }

        try {
            if (this.getLogger().isDebugEnabled()) {
                ++activeRequestCount;
                if (this.getLogger().isDebugEnabled()) {
                    this.debug(environment, null, null);
                }
            }

            if (this.threadSafeProcessor != null) {
                return this.threadSafeProcessor.process(environment);
            } else {
                Processor processor = (Processor)this.componentManager.lookup(Processor.ROLE);
                try {
                    return processor.process(environment);
                }
                finally {
                    this.componentManager.release(processor);
                }
            }
        } finally {
            if (this.getLogger().isDebugEnabled()) {
                --activeRequestCount;
            }
        }
    }

    /**
     * Process the given <code>Environment</code> to assemble
     * a <code>StreamPipeline</code> and an <code>EventPipeline</code>.
     *
     * @param environment an <code>Environment</code> value
     * @param pipeline a <code>StreamPipeline</code> value
     * @param eventPipeline an <code>EventPipeline</code> value
     * @return a <code>boolean</code> value
     * @exception Exception if an error occurs
     */
    public boolean process(Environment environment, StreamPipeline pipeline, EventPipeline eventPipeline)
    throws Exception {
        if (disposed) {
            throw new IllegalStateException("You cannot process a Disposed Cocoon engine.");
        }

        try {
            if (this.getLogger().isDebugEnabled()) {
                ++activeRequestCount;
                if (this.getLogger().isDebugEnabled()) {
                    this.debug(environment, pipeline, eventPipeline);
                }
            }

            if (this.threadSafeProcessor != null) {
                return this.threadSafeProcessor.process(environment, pipeline, eventPipeline);
            } else {
                Processor processor = (Processor)this.componentManager.lookup(Processor.ROLE);
                try {
                    return processor.process(environment);
                }
                finally {
                    this.componentManager.release(processor);
                }
            }

        } finally {
            if (this.getLogger().isDebugEnabled()) {
                --activeRequestCount;
            }
        }
    }

    /**
     * Process the given <code>Environment</code> to generate the sitemap.
     * Delegated to the Processor component if it's a <code>SitemapManager</code>.
     *
     * @param environment an <code>Environment</code> value
     * @exception Exception if an error occurs
     */
    public void generateSitemap(Environment environment)
    throws Exception {
        Component processor = this.componentManager.lookup(Processor.ROLE);
        try {
            if (processor instanceof SitemapManager) {
                ((SitemapManager)processor).generateSitemap(environment);
            }
        } finally {
            this.componentManager.release(processor);
        }
    }

    /**
     * Process the given <code>Environment</code> to generate Java code for specified XSP files.
     *
     * @param fileName a <code>String</code> value
     * @param environment an <code>Environment</code> value
     * @exception Exception if an error occurs
     */
    public void generateXSP(String fileName, Environment environment)
    throws Exception {
        ProgramGenerator programGenerator = null;
        SourceHandler oldSourceHandler = environment.getSourceHandler();
        SourceHandler sourceHandler = null;
        try {
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("XSP generation begin:" + fileName);
            }

            programGenerator = (ProgramGenerator) this.componentManager.lookup(ProgramGenerator.ROLE);
            sourceHandler = (SourceHandler) this.componentManager.lookup(SourceHandler.ROLE);
            environment.setSourceHandler(sourceHandler);
            String markupLanguage = "xsp";
            String programmingLanguage = "java";

            CompiledComponent xsp = programGenerator.load(this.componentManager, fileName, markupLanguage, programmingLanguage, environment);
            if (getLogger().isDebugEnabled()) {
                getLogger().debug("XSP generation complete:" + xsp);
            }
            this.componentManager.release(programGenerator);
        } catch (Exception e) {
            getLogger().error("Main: Error compiling XSP", e);
            throw e;
        } finally {
            environment.setSourceHandler(oldSourceHandler);
            if (programGenerator != null) this.componentManager.release(programGenerator);
            if (sourceHandler != null) this.componentManager.release((Component) sourceHandler);
        }
    }

    /**
     * Accessor for active request count
     */
    public int getActiveRequestCount() {
        return activeRequestCount;
    }
}
