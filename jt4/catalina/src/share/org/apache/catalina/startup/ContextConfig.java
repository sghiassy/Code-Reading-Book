/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/startup/ContextConfig.java,v 1.52 2001/09/04 18:15:30 craigmcc Exp $
 * $Revision: 1.52 $
 * $Date: 2001/09/04 18:15:30 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999-2001 The Apache Software Foundation.  All rights
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
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
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
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.startup;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.JarURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.Stack;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import javax.servlet.ServletContext;
import javax.naming.NamingException;
import javax.naming.NameClassPair;
import javax.naming.NamingEnumeration;
import javax.naming.directory.DirContext;
import org.apache.catalina.Authenticator;
import org.apache.catalina.Container;
import org.apache.catalina.Context;
import org.apache.catalina.Globals;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleEvent;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Logger;
import org.apache.catalina.Pipeline;
import org.apache.catalina.Valve;
import org.apache.catalina.Wrapper;
import org.apache.catalina.core.ContainerBase;
import org.apache.catalina.Engine;
import org.apache.catalina.Host;
import org.apache.catalina.core.ContainerBase;
import org.apache.catalina.core.DefaultContext;
import org.apache.catalina.core.StandardContext;
import org.apache.catalina.deploy.LoginConfig;
import org.apache.catalina.deploy.SecurityConstraint;
import org.apache.catalina.deploy.ApplicationParameter;
import org.apache.catalina.deploy.ContextEjb;
import org.apache.catalina.deploy.ContextEnvironment;
import org.apache.catalina.deploy.ErrorPage;
import org.apache.catalina.deploy.FilterDef;
import org.apache.catalina.deploy.FilterMap;
import org.apache.catalina.deploy.ContextLocalEjb;
import org.apache.catalina.deploy.ContextResource;
import org.apache.catalina.loader.Extension;
import org.apache.catalina.util.StringManager;
import org.apache.catalina.util.xml.SaxContext;
import org.apache.catalina.util.xml.XmlAction;
import org.apache.catalina.util.xml.XmlMapper;
import org.apache.catalina.valves.ValveBase;
import org.xml.sax.SAXParseException;


/**
 * Startup event listener for a <b>Context</b> that configures the properties
 * of that Context, and the associated defined servlets.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.52 $ $Date: 2001/09/04 18:15:30 $
 */

public final class ContextConfig
    implements LifecycleListener {


    // ----------------------------------------------------- Instance Variables


    /**
     * The set of Authenticators that we know how to configure.  The key is
     * the name of the implemented authentication method, and the value is
     * the fully qualified Java class name of the corresponding Valve.
     */
    private static ResourceBundle authenticators = null;


    /**
     * The Context we are associated with.
     */
    private Context context = null;


    /**
     * The debugging detail level for this component.
     */
    private int debug = 0;


    /**
     * Track any fatal errors during startup configuration processing.
     */
    private boolean ok = false;


    /**
     * The string resources for this package.
     */
    private static final StringManager sm =
        StringManager.getManager(Constants.Package);


    // ------------------------------------------------------------- Properties


    /**
     * Return the debugging detail level for this component.
     */
    public int getDebug() {

        return (this.debug);

    }


    /**
     * Set the debugging detail level for this component.
     *
     * @param debug The new debugging detail level
     */
    public void setDebug(int debug) {

        this.debug = debug;

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Process the START event for an associated Context.
     *
     * @param event The lifecycle event that has occurred
     */
    public void lifecycleEvent(LifecycleEvent event) {

        // Identify the context we are associated with
        try {
            context = (Context) event.getLifecycle();
            if (context instanceof StandardContext) {
                int contextDebug = ((StandardContext) context).getDebug();
                if (contextDebug > this.debug)
                    this.debug = contextDebug;
            }
        } catch (ClassCastException e) {
            log(sm.getString("contextConfig.cce", event.getLifecycle()), e);
            return;
        }

        // Process the event that has occurred
        if (event.getType().equals(Lifecycle.START_EVENT))
            start();
        else if (event.getType().equals(Lifecycle.STOP_EVENT))
            stop();

    }


    // -------------------------------------------------------- Private Methods


    /**
     * Process the application configuration file, if it exists.
     *
     * @param mapper The XmlMapper to be used
     */
    private void applicationConfig(XmlMapper mapper) {

        // Open the application web.xml file, if it exists
        InputStream stream = null;
        ServletContext servletContext = context.getServletContext();
        if (servletContext != null)
            stream = servletContext.getResourceAsStream
                (Constants.ApplicationWebXml);
        if (stream == null) {
            log(sm.getString("contextConfig.applicationMissing"));
            return;
        }

        // Process the application web.xml file
        try {
            if (context instanceof StandardContext)
                ((StandardContext) context).setReplaceWelcomeFiles(true);
            mapper.readXml(stream, context);
        } catch (InvocationTargetException e) {
            log(sm.getString("contextConfig.applicationConfig"),
                e.getTargetException());
            ok = false;
        } catch (SAXParseException e) {
            log(sm.getString("contextConfig.applicationParse"), e);
            log(sm.getString("contextConfig.applicationPosition",
                             "" + e.getLineNumber(),
                             "" + e.getColumnNumber()));
            ok = false;
        } catch (Exception e) {
            log(sm.getString("contextConfig.applicationParse"), e);
            ok = false;
        } finally {
            try {
                stream.close();
            } catch (IOException e) {
                log(sm.getString("contextConfig.applicationClose"), e);
            }
        }

    }


    /**
     * Set up an Authenticator automatically if required, and one has not
     * already been configured.
     */
    private synchronized void authenticatorConfig() {

        // Does this Context require an Authenticator?
        SecurityConstraint constraints[] = context.findConstraints();
        if ((constraints == null) || (constraints.length == 0))
            return;
        LoginConfig loginConfig = context.getLoginConfig();
        if (loginConfig == null) {
            loginConfig = new LoginConfig("NONE", null, null, null);
            context.setLoginConfig(loginConfig);
        }

        // Has an authenticator been configured already?
        if (context instanceof Authenticator)
            return;
        if (context instanceof ContainerBase) {
            Pipeline pipeline = ((ContainerBase) context).getPipeline();
            if (pipeline != null) {
                Valve basic = pipeline.getBasic();
                if ((basic != null) && (basic instanceof Authenticator))
                    return;
                Valve valves[] = pipeline.getValves();
                for (int i = 0; i < valves.length; i++) {
                    if (valves[i] instanceof Authenticator)
                        return;
                }
            }
        } else {
            return;     // Cannot install a Valve even if it would be needed
        }

        // Has a Realm been configured for us to authenticate against?
        if (context.getRealm() == null) {
            log(sm.getString("contextConfig.missingRealm"));
            ok = false;
            return;
        }

        // Load our mapping properties if necessary
        if (authenticators == null) {
            try {
                authenticators = ResourceBundle.getBundle
                    ("org.apache.catalina.startup.Authenticators");
            } catch (MissingResourceException e) {
                log(sm.getString("contextConfig.authenticatorResources"), e);
                ok = false;
                return;
            }
        }

        // Identify the class name of the Valve we should configure
        String authenticatorName = null;
        try {
            authenticatorName =
                authenticators.getString(loginConfig.getAuthMethod());
        } catch (MissingResourceException e) {
            authenticatorName = null;
        }
        if (authenticatorName == null) {
            log(sm.getString("contextConfig.authenticatorMissing",
                             loginConfig.getAuthMethod()));
            ok = false;
            return;
        }

        // Instantiate and install an Authenticator of the requested class
        Valve authenticator = null;
        try {
            Class authenticatorClass = Class.forName(authenticatorName);
            authenticator = (Valve) authenticatorClass.newInstance();
            if (context instanceof ContainerBase) {
                Pipeline pipeline = ((ContainerBase) context).getPipeline();
                if (pipeline != null) {
                    pipeline.addValve(authenticator);
                    log(sm.getString("contextConfig.authenticatorConfigured",
                                     loginConfig.getAuthMethod()));
                }
            }
        } catch (Throwable t) {
            log(sm.getString("contextConfig.authenticatorInstantiate",
                             authenticatorName), t);
            ok = false;
        }

    }


    /**
     * Create and deploy a Valve to expose the SSL certificates presented
     * by this client, if any.  If we cannot instantiate such a Valve
     * (because the JSSE classes are not available), silently continue.
     */
    private void certificatesConfig() {

        // Validate that the JSSE classes are present
        try {
            Class clazz = this.getClass().getClassLoader().loadClass
                ("javax.net.ssl.SSLSocket");
            if (clazz == null)
                return;
        } catch (Throwable t) {
            return;
        }

        // Instantiate a new CertificatesValve if possible
        Valve certificates = null;
        try {
            Class clazz =
                Class.forName("org.apache.catalina.valves.CertificatesValve");
            certificates = (Valve) clazz.newInstance();
        } catch (Throwable t) {
            return;     // Probably JSSE classes not present
        }

        // Add this Valve to our Pipeline
        try {
            if (context instanceof ContainerBase) {
                Pipeline pipeline = ((ContainerBase) context).getPipeline();
                if (pipeline != null) {
                    pipeline.addValve(certificates);
                    log(sm.getString
                        ("contextConfig.certificatesConfig.added"));
                }
            }
        } catch (Throwable t) {
            log(sm.getString("contextConfig.certificatesConfig.error"), t);
            ok = false;
        }

    }


    /**
     * Create and return an XmlMapper configured to process a tag library
     * descriptor, looking for additional listener classes to be registered.
     */
    private XmlMapper createTldMapper() {

        XmlMapper mapper = new XmlMapper();
        if (debug > 0)
            mapper.setDebug(3);
        mapper.setValidating(true);
        mapper.registerDTDRes(Constants.TldDtdPublicId_11,
                              Constants.TldDtdResourcePath_11);
        mapper.registerDTDRes(Constants.TldDtdPublicId_12,
                              Constants.TldDtdResourcePath_12);

        mapper.addRule("taglib/listener/listener-class",
                       mapper.methodSetter("addApplicationListener", 0));

        return (mapper);

    }


    /**
     * Create and return an XmlMapper configured to process the web application
     * deployment descriptor (web.xml).
     */
    private XmlMapper createWebMapper() {

        XmlMapper mapper = new XmlMapper();
        if (debug > 0)
            mapper.setDebug(3);
        mapper.setValidating(true);
        mapper.registerDTDRes(Constants.WebDtdPublicId_22,
                              Constants.WebDtdResourcePath_22);
        mapper.registerDTDRes(Constants.WebDtdPublicId_23,
                              Constants.WebDtdResourcePath_23);

        mapper.addRule("web-app",
                       new SetPublicIdAction("setPublicId"));

        mapper.addRule("web-app/context-param",
                       mapper.methodSetter("addParameter", 2));
        mapper.addRule("web-app/context-param/param-name",
                       mapper.methodParam(0));
        mapper.addRule("web-app/context-param/param-value",
                       mapper.methodParam(1));

        mapper.addRule("web-app/display-name",
                       mapper.methodSetter("setDisplayName", 0));

        mapper.addRule("web-app/distributable",
                       new SetDistributableAction());

        mapper.addRule("web-app/ejb-local-ref",
                 mapper.objectCreate("org.apache.catalina.deploy.ContextLocalEjb"));
        mapper.addRule("web-app/ejb-local-ref",
                       mapper.addChild("addLocalEjb",
                                     "org.apache.catalina.deploy.ContextLocalEjb"));
        mapper.addRule("web-app/ejb-local-ref/description",
                       mapper.methodSetter("setDescription", 0));
        mapper.addRule("web-app/ejb-local-ref/ejb-ref-name",
                       mapper.methodSetter("setName", 0));
        mapper.addRule("web-app/ejb-local-ref/ejb-ref-type",
                       mapper.methodSetter("setType", 0));
        mapper.addRule("web-app/ejb-local-ref/local-home",
                       mapper.methodSetter("setHome", 0));
        mapper.addRule("web-app/ejb-local-ref/local",
                       mapper.methodSetter("setLocal", 0));
        mapper.addRule("web-app/ejb-local-ref/ejb-link",
                       mapper.methodSetter("setLink", 0));

        mapper.addRule("web-app/ejb-ref",
                 mapper.objectCreate("org.apache.catalina.deploy.ContextEjb"));
        mapper.addRule("web-app/ejb-ref",
                       mapper.addChild("addEjb",
                                     "org.apache.catalina.deploy.ContextEjb"));
        mapper.addRule("web-app/ejb-ref/description",
                       mapper.methodSetter("setDescription", 0));
        mapper.addRule("web-app/ejb-ref/ejb-ref-name",
                       mapper.methodSetter("setName", 0));
        mapper.addRule("web-app/ejb-ref/ejb-ref-type",
                       mapper.methodSetter("setType", 0));
        mapper.addRule("web-app/ejb-ref/home",
                       mapper.methodSetter("setHome", 0));
        mapper.addRule("web-app/ejb-ref/remote",
                       mapper.methodSetter("setRemote", 0));
        mapper.addRule("web-app/ejb-ref/ejb-link",
                       mapper.methodSetter("setLink", 0));

        mapper.addRule("web-app/env-entry",
         mapper.objectCreate("org.apache.catalina.deploy.ContextEnvironment"));
        mapper.addRule("web-app/env-entry",
                       mapper.addChild("addEnvironment",
                             "org.apache.catalina.deploy.ContextEnvironment"));
        mapper.addRule("web-app/env-entry/env-entry-description",
                       mapper.methodSetter("setDescription", 0));
        mapper.addRule("web-app/env-entry/env-entry-name",
                       mapper.methodSetter("setName", 0));
        mapper.addRule("web-app/env-entry/env-entry-type",
                       mapper.methodSetter("setType", 0));
        mapper.addRule("web-app/env-entry/env-entry-value",
                       mapper.methodSetter("setValue", 0));

        mapper.addRule("web-app/error-page",
                  mapper.objectCreate("org.apache.catalina.deploy.ErrorPage"));
        mapper.addRule("web-app/error-page",
                       mapper.addChild("addErrorPage",
                               "org.apache.catalina.deploy.ErrorPage"));
        mapper.addRule("web-app/error-page/error-code",
                       mapper.methodSetter("setErrorCode", 0));
        mapper.addRule("web-app/error-page/exception-type",
                       mapper.methodSetter("setExceptionType", 0));
        mapper.addRule("web-app/error-page/location",
                       mapper.methodSetter("setLocation", 0));

        mapper.addRule("web-app/filter",
                  mapper.objectCreate("org.apache.catalina.deploy.FilterDef"));
        mapper.addRule("web-app/filter",
                       mapper.addChild("addFilterDef",
                                      "org.apache.catalina.deploy.FilterDef"));
        mapper.addRule("web-app/filter/description",
                       mapper.methodSetter("setDescription", 0));
        mapper.addRule("web-app/filter/display-name",
                       mapper.methodSetter("setDisplayName", 0));
        mapper.addRule("web-app/filter/filter-class",
                       mapper.methodSetter("setFilterClass", 0));
        mapper.addRule("web-app/filter/filter-name",
                       mapper.methodSetter("setFilterName", 0));
        mapper.addRule("web-app/filter/icon/large-icon",
                       mapper.methodSetter("setLargeIcon", 0));
        mapper.addRule("web-app/filter/icon/small-icon",
                       mapper.methodSetter("setSmallIcon", 0));
        mapper.addRule("web-app/filter/init-param",
                       mapper.methodSetter("addInitParameter", 2));
        mapper.addRule("web-app/filter/init-param/param-name",
                       mapper.methodParam(0));
        mapper.addRule("web-app/filter/init-param/param-value",
                       mapper.methodParam(1));

        mapper.addRule("web-app/filter-mapping",
                  mapper.objectCreate("org.apache.catalina.deploy.FilterMap"));
        mapper.addRule("web-app/filter-mapping",
                       mapper.addChild("addFilterMap",
                                      "org.apache.catalina.deploy.FilterMap"));
        mapper.addRule("web-app/filter-mapping/filter-name",
                       mapper.methodSetter("setFilterName", 0));
        mapper.addRule("web-app/filter-mapping/servlet-name",
                       mapper.methodSetter("setServletName", 0));
        mapper.addRule("web-app/filter-mapping/url-pattern",
                       mapper.methodSetter("setURLPattern", 0));

        mapper.addRule("web-app/listener/listener-class",
                       mapper.methodSetter("addApplicationListener", 0));

        mapper.addRule("web-app/login-config",
                mapper.objectCreate("org.apache.catalina.deploy.LoginConfig"));
        mapper.addRule("web-app/login-config",
                       mapper.addChild("setLoginConfig",
                                    "org.apache.catalina.deploy.LoginConfig"));
        mapper.addRule("web-app/login-config/auth-method",
                       mapper.methodSetter("setAuthMethod", 0));
        mapper.addRule("web-app/login-config/realm-name",
                       mapper.methodSetter("setRealmName", 0));
        mapper.addRule("web-app/login-config/form-login-config/form-login-page",
                       mapper.methodSetter("setLoginPage", 0));
        mapper.addRule("web-app/login-config/form-login-config/form-error-page",
                       mapper.methodSetter("setErrorPage", 0));

        mapper.addRule("web-app/mime-mapping",
                       mapper.methodSetter("addMimeMapping", 2));
        mapper.addRule("web-app/mime-mapping/extension",
                       mapper.methodParam(0));
        mapper.addRule("web-app/mime-mapping/mime-type",
                       mapper.methodParam(1));


        mapper.addRule("web-app/resource-env-ref",
                       mapper.methodSetter("addResourceEnvRef", 2));
        mapper.addRule("web-app/resource-env-ref/resource-env-ref-name",
                       mapper.methodParam(0));
        mapper.addRule("web-app/resource-env-ref/resource-env-ref-type",
                       mapper.methodParam(1));

        mapper.addRule("web-app/resource-ref",
            mapper.objectCreate("org.apache.catalina.deploy.ContextResource"));
        mapper.addRule("web-app/resource-ref",
                mapper.addChild("addResource",
                                "org.apache.catalina.deploy.ContextResource"));
        mapper.addRule("web-app/resource-ref/description",
                       mapper.methodSetter("setDescription", 0));
        mapper.addRule("web-app/resource-ref/res-auth",
                       mapper.methodSetter("setAuth", 0));
        mapper.addRule("web-app/resource-ref/res-ref-name",
                       mapper.methodSetter("setName", 0));
        mapper.addRule("web-app/resource-ref/res-sharing-scope",
                       mapper.methodSetter("setScope", 0));
        mapper.addRule("web-app/resource-ref/res-type",
                       mapper.methodSetter("setType", 0));

        mapper.addRule("web-app/security-constraint",
                       mapper.objectCreate("org.apache.catalina.deploy.SecurityConstraint"));
        mapper.addRule("web-app/security-constraint",
                       mapper.addChild("addConstraint",
                                       "org.apache.catalina.deploy.SecurityConstraint"));
        mapper.addRule("web-app/security-constraint/auth-constraint",
                       new SetAuthConstraint());
        mapper.addRule("web-app/security-constraint/auth-constraint/role-name",
                       mapper.methodSetter("addAuthRole", 0));
        mapper.addRule("web-app/security-constraint/display-name",
                       mapper.methodSetter("setDisplayName", 0));
        mapper.addRule("web-app/security-constraint/user-data-constraint/transport-guarantee",
                       mapper.methodSetter("setUserConstraint", 0));
        mapper.addRule("web-app/security-constraint/web-resource-collection",
                       mapper.objectCreate("org.apache.catalina.deploy.SecurityCollection"));
        mapper.addRule("web-app/security-constraint/web-resource-collection",
                       mapper.addChild("addCollection",
                                       "org.apache.catalina.deploy.SecurityCollection"));
        mapper.addRule("web-app/security-constraint/web-resource-collection/http-method",
                       mapper.methodSetter("addMethod", 0));
        mapper.addRule("web-app/security-constraint/web-resource-collection/url-pattern",
                       mapper.methodSetter("addPattern", 0));
        mapper.addRule("web-app/security-constraint/web-resource-collection/web-resource-name",
                       mapper.methodSetter("setName", 0));

        mapper.addRule("web-app/security-role",
                       mapper.methodSetter("addSecurityRole", 1));
        mapper.addRule("web-app/security-role/role-name",
                       mapper.methodParam(0));

        mapper.addRule("web-app/servlet",
                       new WrapperCreate(context));
        mapper.addRule("web-app/servlet",
                       mapper.addChild("addChild",
                                       "org.apache.catalina.Container"));
        mapper.addRule("web-app/servlet/init-param",
                       mapper.methodSetter("addInitParameter", 2));
        mapper.addRule("web-app/servlet/init-param/param-name",
                       mapper.methodParam(0));
        mapper.addRule("web-app/servlet/init-param/param-value",
                       mapper.methodParam(1));
        mapper.addRule("web-app/servlet/jsp-file",
                       mapper.methodSetter("setJspFile", 0));
        mapper.addRule("web-app/servlet/load-on-startup",
                       mapper.methodSetter("setLoadOnStartupString", 0));
        mapper.addRule("web-app/servlet/run-as/role-name",
                       mapper.methodSetter("setRunAs", 0));
        mapper.addRule("web-app/servlet/security-role-ref",
                       mapper.methodSetter("addSecurityReference", 2));
        mapper.addRule("web-app/servlet/security-role-ref/role-link",
                       mapper.methodParam(1));
        mapper.addRule("web-app/servlet/security-role-ref/role-name",
                       mapper.methodParam(0));
        mapper.addRule("web-app/servlet/servlet-class",
                       mapper.methodSetter("setServletClass", 0));
        mapper.addRule("web-app/servlet/servlet-name",
                       mapper.methodSetter("setName", 0));

        mapper.addRule("web-app/servlet-mapping",
                       mapper.methodSetter("addServletMapping", 2));
        mapper.addRule("web-app/servlet-mapping/servlet-name",
                       mapper.methodParam(1));
        mapper.addRule("web-app/servlet-mapping/url-pattern",
                       mapper.methodParam(0));

        mapper.addRule("web-app/session-config",
                       mapper.methodSetter("setSessionTimeout", 1,
                                           new String[]{"int"}));
        mapper.addRule("web-app/session-config/session-timeout",
                       mapper.methodParam(0));

        mapper.addRule("web-app/taglib",
                       mapper.methodSetter("addTaglib", 2));
        mapper.addRule("web-app/taglib/taglib-location",
                       mapper.methodParam(1));
        mapper.addRule("web-app/taglib/taglib-uri",
                       mapper.methodParam(0));

        mapper.addRule("web-app/welcome-file-list/welcome-file",
                       mapper.methodSetter("addWelcomeFile", 0));

        return (mapper);

    }


    /**
     * Process the default configuration file, if it exists.
     *
     * @param mapper The XmlMapper to be used

     */
    private void defaultConfig(XmlMapper mapper) {

        // Open the default web.xml file, if it exists
        File file = new File(Constants.DefaultWebXml);
        if (!file.isAbsolute())
            file = new File(System.getProperty("catalina.base"),
                            Constants.DefaultWebXml);
        FileInputStream stream = null;
        try {
            stream = new FileInputStream(file.getCanonicalPath());
        } catch (FileNotFoundException e) {
            log(sm.getString("contextConfig.defaultMissing"));
            return;
        } catch (IOException e) {
            log(sm.getString("contextConfig.defaultMissing"), e);
            return;
        }

        // Process the default web.xml file
        try {
            if (context instanceof StandardContext)
                ((StandardContext) context).setReplaceWelcomeFiles(true);
            mapper.readXml(stream, context);
        } catch (InvocationTargetException e) {
            log(sm.getString("contextConfig.defaultConfig"),
                e.getTargetException());
            ok = false;
        } catch (SAXParseException e) {
            log(sm.getString("contextConfig.defaultParse"), e);
            log(sm.getString("contextConfig.defaultPosition",
                             "" + e.getLineNumber(),
                             "" + e.getColumnNumber()));
            ok = false;
        } catch (Exception e) {
            log(sm.getString("contextConfig.defaultParse"), e);
            ok = false;
        } finally {
            try {
                stream.close();
            } catch (IOException e) {
                log(sm.getString("contextConfig.defaultClose"), e);
            }
        }

    }


    /**
     * Log a message on the Logger associated with our Context (if any)
     *
     * @param message Message to be logged
     */
    private void log(String message) {

        Logger logger = null;
        if (context != null)
            logger = context.getLogger();
        if (logger != null)
            logger.log("ContextConfig[" + context.getName() + "]: " + message);
        else
            System.out.println("ContextConfig[" + context.getName() + "]: "
                               + message);

    }


    /**
     * Log a message on the Logger associated with our Context (if any)
     *
     * @param message Message to be logged
     * @param throwable Associated exception
     */
    private void log(String message, Throwable throwable) {

        Logger logger = null;
        if (context != null)
            logger = context.getLogger();
        if (logger != null)
            logger.log("ContextConfig[" + context.getName() + "] "
                       + message, throwable);
        else {
            System.out.println("ContextConfig[" + context.getName() + "]: "
                               + message);
            System.out.println("" + throwable);
            throwable.printStackTrace(System.out);
        }

    }


    /**
     * Process a "start" event for this Context.
     */
    private synchronized void start() {

        if (debug > 0)
            log(sm.getString("contextConfig.start"));
        context.setConfigured(false);
        ok = true;

        // Set properties based on DefaultContext
        Container container = context.getParent();
        if( !context.getOverride() ) {
            if( container instanceof Host ) {
                ((Host)container).importDefaultContext(context);
                container = container.getParent();
            }
            if( container instanceof Engine ) {
                ((Engine)container).importDefaultContext(context);
            }
        }

        // Process the default and application web.xml files
        XmlMapper mapper = createWebMapper();
        defaultConfig(mapper);
        applicationConfig(mapper);

        // Scan tag library descriptor files for additional listener classes
        if (ok)
            tldConfig();

        // Configure a certificates exposer valve, if required
        if (ok)
            certificatesConfig();

        // Configure an authenticator if we need one
        if (ok)
            authenticatorConfig();

        // Dump the contents of this pipeline if requested
        if ((debug >= 1) && (context instanceof ContainerBase)) {
            log("Pipline Configuration:");
            Pipeline pipeline = ((ContainerBase) context).getPipeline();
            Valve valves[] = null;
            if (pipeline != null)
                valves = pipeline.getValves();
            if (valves != null) {
                for (int i = 0; i < valves.length; i++) {
                    log("  " + valves[i].getInfo());
                }
            }
            log("======================");
        }

        // Make our application available if no problems were encountered
        if (ok)
            context.setConfigured(true);
        else {
            log(sm.getString("contextConfig.unavailable"));
            context.setConfigured(false);
        }

    }


    /**
     * Process a "stop" event for this Context.
     */
    private synchronized void stop() {

        if (debug > 0)
            log(sm.getString("contextConfig.stop"));

        int i;

        // Removing children
        Container[] children = context.findChildren();
        for (i = 0; i < children.length; i++) {
            context.removeChild(children[i]);
        }

        // FIXME : remove mappers ?

        // Removing application listeners
        String[] applicationListeners = context.findApplicationListeners();
        for (i = 0; i < applicationListeners.length; i++) {
            context.removeApplicationListener(applicationListeners[i]);
        }

        // Removing application parameters
        ApplicationParameter[] applicationParameters =
            context.findApplicationParameters();
        for (i = 0; i < applicationParameters.length; i++) {
            context.removeApplicationParameter
                (applicationParameters[i].getName());
        }

        // Removing security constraints
        SecurityConstraint[] securityConstraints = context.findConstraints();
        for (i = 0; i < securityConstraints.length; i++) {
            context.removeConstraint(securityConstraints[i]);
        }

        // Removing Ejbs
        ContextEjb[] contextEjbs = context.findEjbs();
        for (i = 0; i < contextEjbs.length; i++) {
            context.removeEjb(contextEjbs[i].getName());
        }

        // Removing environments
        ContextEnvironment[] contextEnvironments = context.findEnvironments();
        for (i = 0; i < contextEnvironments.length; i++) {
            context.removeEnvironment(contextEnvironments[i].getName());
        }

        // Removing errors pages
        ErrorPage[] errorPages = context.findErrorPages();
        for (i = 0; i < errorPages.length; i++) {
            context.removeErrorPage(errorPages[i]);
        }

        // Removing filter defs
        FilterDef[] filterDefs = context.findFilterDefs();
        for (i = 0; i < filterDefs.length; i++) {
            context.removeFilterDef(filterDefs[i]);
        }

        // Removing filter maps
        FilterMap[] filterMaps = context.findFilterMaps();
        for (i = 0; i < filterMaps.length; i++) {
            context.removeFilterMap(filterMaps[i]);
        }

        // Removing instance listeners
        String[] instanceListeners = context.findInstanceListeners();
        for (i = 0; i < instanceListeners.length; i++) {
            context.removeInstanceListener(instanceListeners[i]);
        }

        // Removing local ejbs
        ContextLocalEjb[] contextLocalEjbs = context.findLocalEjbs();
        for (i = 0; i < contextLocalEjbs.length; i++) {
            context.removeLocalEjb(contextLocalEjbs[i].getName());
        }

        // Removing Mime mappings
        String[] mimeMappings = context.findMimeMappings();
        for (i = 0; i < mimeMappings.length; i++) {
            context.removeMimeMapping(mimeMappings[i]);
        }

        // Removing parameters
        String[] parameters = context.findParameters();
        for (i = 0; i < parameters.length; i++) {
            context.removeParameter(parameters[i]);
        }

        // Removing resource env refs
        String[] resourceEnvRefs = context.findResourceEnvRefs();
        for (i = 0; i < resourceEnvRefs.length; i++) {
            context.removeResourceEnvRef(resourceEnvRefs[i]);
        }

        // Removing resources
        ContextResource[] contextResources = context.findResources();
        for (i = 0; i < contextResources.length; i++) {
            context.removeResource(contextResources[i].getName());
        }

        // Removing sercurity role
        String[] securityRoles = context.findSecurityRoles();
        for (i = 0; i < securityRoles.length; i++) {
            context.removeSecurityRole(securityRoles[i]);
        }

        // Removing servlet mappings
        String[] servletMappings = context.findServletMappings();
        for (i = 0; i < servletMappings.length; i++) {
            context.removeServletMapping(servletMappings[i]);
        }

        // FIXME : Removing status pages

        // Removing taglibs
        String[] taglibs = context.findTaglibs();
        for (i = 0; i < taglibs.length; i++) {
            context.removeTaglib(taglibs[i]);
        }

        // Removing welcome files
        String[] welcomeFiles = context.findWelcomeFiles();
        for (i = 0; i < welcomeFiles.length; i++) {
            context.removeWelcomeFile(welcomeFiles[i]);
        }

        // Removing wrapper lifecycles
        String[] wrapperLifecycles = context.findWrapperLifecycles();
        for (i = 0; i < wrapperLifecycles.length; i++) {
            context.removeWrapperLifecycle(wrapperLifecycles[i]);
        }

        // Removing wrapper listeners
        String[] wrapperListeners = context.findWrapperListeners();
        for (i = 0; i < wrapperListeners.length; i++) {
            context.removeWrapperListener(wrapperListeners[i]);
        }

        ok = true;

    }


    /**
     * Scan the tag library descriptors of all tag libraries we can find, and
     * register any application descriptor classes that are found there.
     */
    private void tldConfig() {

        // Acquire an XmlMapper to use for parsing
        XmlMapper mapper = createTldMapper();

        // First, scan tag libraries declared in our deployment descriptor
        if (debug >= 1)
            log("Scanning web.xml tag libraries");
        ArrayList resourcePaths = new ArrayList(); // Already processed TLDs
        String taglibs[] = context.findTaglibs();
        for (int i = 0; i < taglibs.length; i++) {

            // Calculate the resource path of the next tag library to check
            String resourcePath = context.findTaglib(taglibs[i]);
            if (!resourcePath.startsWith("/"))
                resourcePath = "/WEB-INF/web.xml/../" + resourcePath;
            if (debug >= 2)
                log("  URI='" + taglibs[i] + "', ResourcePath='" +
                    resourcePath + "'");
            if (resourcePaths.contains(resourcePath)) {
                if (debug >= 2)
                    log("    Already processed");
                continue;
            }
            resourcePaths.add(resourcePath);

            // Process either a JAR file or a TLD at this location
            if (!tldConfigJar(resourcePath, mapper))
                tldConfigTld(resourcePath, mapper);

        }

        // Second, scan tag libraries defined in JAR files
        // FIXME - Yet another dependence on files
        if (debug >= 1)
            log("Scanning library JAR files");
        DirContext resources = context.getResources();
        String libName = "/WEB-INF/lib";
        DirContext libDir = null;
        // Looking up directory /WEB-INF/lib in the context
        try {
            NamingEnumeration enum = resources.list(libName);
            while (enum.hasMoreElements()) {
                NameClassPair ncPair =
                    (NameClassPair) enum.nextElement();
                String filename = libName + "/" + ncPair.getName();
                if (!filename.endsWith(".jar"))
                    continue;
                tldConfigJar(filename, mapper);
            }
        } catch (NamingException e) {
            // Silent catch: it's valid that no /WEB-INF/lib directory
            //exists
        }

    }


    /**
     * Process a TLD (if there is one) in the JAR file (if there is one) at
     * the specified resource path.  Return <code>true</code> if we actually
     * found and processed such a TLD, or <code>false</code> otherwise.
     *
     * @param resourcePath Context-relative resource path
     * @param mapper XmlMapper to use for parsing
     */
    private boolean tldConfigJar(String resourcePath, XmlMapper mapper) {

        JarFile jarFile = null;
        InputStream stream = null;
        try {
            URL url = context.getServletContext().getResource(resourcePath);
            if (url == null)
                return (false);
            url = new URL("jar:" + url.toString() + "!/");
            JarURLConnection conn =
                (JarURLConnection) url.openConnection();
            jarFile = conn.getJarFile();
            boolean found = false;
            Enumeration entries = jarFile.entries();
            while (entries.hasMoreElements()) {
                JarEntry entry = (JarEntry) entries.nextElement();
                String name = entry.getName();
                if (!name.startsWith("META-INF/"))
                    continue;
                if (!name.endsWith(".tld"))
                    continue;
                if (debug >= 2)
                    log("    tldConfigJar(" + resourcePath +
                        "): Processing entry '" + name + "'");
                stream = jarFile.getInputStream(entry);
                mapper.readXml(stream, context);
                stream.close();
                found = true;
            }
            // FIXME jarFile.close();
            return (found);
        } catch (Exception e) {
            if (debug >= 2)
                log("    tldConfigJar(" + resourcePath + "): " + e);
            if (stream != null) {
                try {
                    stream.close();
                } catch (Throwable t) {
                    ;
                }
            }
            if (jarFile != null) {
                try {
                    jarFile.close();
                } catch (Throwable t) {
                    ;
                }
            }
            return (false);
        }

    }


    /**
     * Process a TLD (if there is one) at the specified resource path.
     * Return <code>true</code> if we actually found and processed such
     * a TLD, or <code>false</code> otherwise.
     *
     * @param resourcePath Context-relative resource path
     * @param mapper XmlMapper to use for parsing
     */
    private boolean tldConfigTld(String resourcePath, XmlMapper mapper) {

        InputStream stream = null;
        try {
            stream =
                context.getServletContext().getResourceAsStream(resourcePath);
            if (stream == null)
                return (false);
            mapper.readXml(stream, context);
            stream.close();
            return (true);
        } catch (Exception e) {
            if (debug >= 2)
                log("    tldConfigTld(" + resourcePath + "): " + e);
            if (stream != null) {
                try {
                    stream.close();
                } catch (Throwable t) {
                    ;
                }
            }
            return (false);
        }

    }


}



// ----------------------------------------------------------- Private Classes


/**
 * An XmlAction that calls the <code>setAuthConstraint(true)</code> method of
 * the top item on the stack, which must be of type
 * <code>org.apache.catalina.deploy.SecurityConstraint</code>.
 */

final class SetAuthConstraint extends XmlAction {

    public SetAuthConstraint() {
        super();
    }

    public void start(SaxContext ctx) {
        Stack stack = ctx.getObjectStack();
        SecurityConstraint securityConstraint =
            (SecurityConstraint) stack.peek();
        securityConstraint.setAuthConstraint(true);
        if (ctx.getDebug() > 0)
            ctx.log("Calling SecurityConstraint.setAuthConstraint(true)");
    }

}


/**
 * Class that calls <code>setDistributable(true)</code> for the top object
 * on the stack, which must be a <code>org.apache.catalina.Context</code>.
 */

final class SetDistributableAction extends XmlAction {

    public SetDistributableAction() {
        super();
    }

    public void start(SaxContext ctx) throws Exception {
        Stack stack = ctx.getObjectStack();
        Context context = (Context) stack.peek();
        context.setDistributable(true);
        if (ctx.getDebug() > 0)
            ctx.log(context.getClass().getName() +
                    ".setDistributable( true)");
    }

}


/**
 * Class that calls a property setter for the top object on the stack,
 * passing the public ID of the entity we are currently processing.
 */

final class SetPublicIdAction extends XmlAction {

    public SetPublicIdAction(String method) {
        super();
        this.method = method;
    }


    private String method = null;

    public void start(SaxContext context) throws Exception {

        Stack stack = context.getObjectStack();
        Object top = stack.peek();
        Class paramClasses[] = new Class[1];
        paramClasses[0] = "String".getClass();
        String paramValues[] = new String[1];
        paramValues[0] = context.getPublicId();

        Method m = null;
        try {
            m = top.getClass().getMethod(method, paramClasses);
        } catch (NoSuchMethodException e) {
            context.log("Can't find method " + method + " in " + top +
                        " CLASS " + top.getClass());
            return;
        }

        m.invoke(top, paramValues);
        if (context.getDebug() >= 1)
            context.log("" + top.getClass().getName() + "." + method +
                        "(" + paramValues[0] + ")");

    }

}


/**
 * An XmlAction that calls the factory method on the specified context to
 * create the object that is to be added to the stack.
 */

final class WrapperCreate extends XmlAction {

    private Context context = null;

    public WrapperCreate(Context context) {
        this.context = context;
    }

    public void start(SaxContext ctx) {
        Stack stack = ctx.getObjectStack();
        Wrapper wrapper = context.createWrapper();
        stack.push(wrapper);
        if (ctx.getDebug() > 0)
            ctx.log("new " + wrapper.getClass().getName());
    }

    public void cleanup(SaxContext ctx) {
        Stack stack = ctx.getObjectStack();
        Wrapper wrapper = (Wrapper) stack.pop();
        if (ctx.getDebug() > 0)
            ctx.log("pop " + wrapper.getClass().getName());
    }

}
