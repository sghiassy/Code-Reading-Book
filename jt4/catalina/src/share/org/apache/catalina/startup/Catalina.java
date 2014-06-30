/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/startup/Catalina.java,v 1.32 2001/09/11 23:34:43 craigmcc Exp $
 * $Revision: 1.32 $
 * $Date: 2001/09/11 23:34:43 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
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
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Constructor;
import java.net.Socket;
import java.security.Security;
import java.util.Stack;
import org.apache.catalina.Container;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleException;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Server;
import org.apache.catalina.Loader;
import org.apache.catalina.util.xml.SaxContext;
import org.apache.catalina.util.xml.XmlAction;
import org.apache.catalina.util.xml.XmlMapper;
import org.xml.sax.AttributeList;


/**
 * Startup/Shutdown shell program for Catalina.  The following command line
 * options are recognized:
 * <ul>
 * <li><b>-config {pathname}</b> - Set the pathname of the configuration file
 *     to be processed.  If a relative path is specified, it will be
 *     interpreted as relative to the directory pathname specified by the
 *     "catalina.base" system property.   [conf/server.xml]
 * <li><b>-help</b> - Display usage information.
 * <li><b>-stop</b> - Stop the currently running instance of Catalina.
 * </u>
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.32 $ $Date: 2001/09/11 23:34:43 $
 */

public class Catalina {


    // ----------------------------------------------------- Instance Variables


    /**
     * Pathname to the server configuration file.
     */
    protected String configFile = "conf/server.xml";


    /**
     * Set the debugging detail level on our XmlMapper.
     */
    protected boolean debug = false;


    /**
     * The shared extensions class loader for this server.
     */
    protected ClassLoader parentClassLoader =
        ClassLoader.getSystemClassLoader();


    /**
     * The server component we are starting or stopping
     */
    protected Server server = null;


    /**
     * Are we starting a new server?
     */
    protected boolean starting = false;


    /**
     * Are we stopping an existing server?
     */
    protected boolean stopping = false;


    /**
     * Are we using naming ?
     */
    protected boolean useNaming = true;


    // ----------------------------------------------------------- Main Program


    /**
     * The application main program.
     *
     * @param args Command line arguments
     */
    public static void main(String args[]) {

        (new Catalina()).process(args);

    }


    /**
     * The instance main program.
     *
     * @param args Command line arguments
     */
    public void process(String args[]) {

        setCatalinaHome();
        setCatalinaBase();
        try {
            if (arguments(args))
                execute();
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Set the shared extensions class loader.
     *
     * @param parentClassLoader The shared extensions class loader.
     */
    public void setParentClassLoader(ClassLoader parentClassLoader) {

        this.parentClassLoader = parentClassLoader;

    }


    /**
     * Set the server instance we are configuring.
     *
     * @param server The new server
     */
    public void setServer(Server server) {

        this.server = server;

    }


    // ------------------------------------------------------ Protected Methods


    /**
     * Process the specified command line arguments, and return
     * <code>true</code> if we should continue processing; otherwise
     * return <code>false</code>.
     *
     * @param args Command line arguments to process
     */
    protected boolean arguments(String args[]) {

        boolean isConfig = false;

        if (args.length < 1) {
            usage();
            return (false);
        }

        for (int i = 0; i < args.length; i++) {
            if (isConfig) {
                configFile = args[i];
                isConfig = false;
            } else if (args[i].equals("-config")) {
                isConfig = true;
            } else if (args[i].equals("-debug")) {
                debug = true;
            } else if (args[i].equals("-nonaming")) {
                useNaming = false;
            } else if (args[i].equals("-help")) {
                usage();
                return (false);
            } else if (args[i].equals("start")) {
                starting = true;
            } else if (args[i].equals("stop")) {
                stopping = true;
            } else {
                usage();
                return (false);
            }
        }

        return (true);

    }


    /**
     * Return a File object representing our configuration file.
     */
    protected File configFile() {

        File file = new File(configFile);
        if (!file.isAbsolute())
            file = new File(System.getProperty("catalina.base"), configFile);
        return (file);

    }


    /**
     * Create and configure the XmlMapper we will be using for startup.
     */
    protected XmlMapper createStartMapper() {

        // Initialize the mapper
        XmlMapper mapper = new XmlMapper();
        if (debug)
            mapper.setDebug(999);
        mapper.setValidating(false);

        // Configure the actions we will be using

        mapper.addRule("Server", mapper.objectCreate
                       ("org.apache.catalina.core.StandardServer", "className"));
        mapper.addRule("Server", mapper.setProperties());
        mapper.addRule("Server", mapper.addChild
                       ("setServer", "org.apache.catalina.Server"));

        mapper.addRule("Server/Listener", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule("Server/Service", mapper.objectCreate
                       ("org.apache.catalina.core.StandardService", "className"));
        mapper.addRule("Server/Service", mapper.setProperties());
        mapper.addRule("Server/Service", mapper.addChild
                       ("addService", "org.apache.catalina.Service"));

        mapper.addRule("Server/Service/Listener", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule("Server/Service/Connector", mapper.objectCreate
                       ("org.apache.catalina.connector.http.HttpConnector",
                        "className"));
        mapper.addRule("Server/Service/Connector", mapper.setProperties());
        mapper.addRule("Server/Service/Connector", mapper.addChild
                       ("addConnector", "org.apache.catalina.Connector"));

        mapper.addRule("Server/Service/Connector/Factory", mapper.objectCreate
                       ("org.apache.catalina.net.DefaultServerSocketFactory",
                        "className"));
        mapper.addRule("Server/Service/Connector/Factory",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Connector/Factory", mapper.addChild
                       ("setFactory",
                        "org.apache.catalina.net.ServerSocketFactory"));

        mapper.addRule("Server/Service/Connector/Listener", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Connector/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Connector/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule("Server/Service/Engine", mapper.objectCreate
                       ("org.apache.catalina.core.StandardEngine",
                        "className"));
        mapper.addRule("Server/Service/Engine", mapper.setProperties());
        mapper.addRule("Server/Service/Engine",
                       new LifecycleListenerAction
                           ("org.apache.catalina.startup.EngineConfig",
                            "configClass"));
        mapper.addRule("Server/Service/Engine",
                       new SetParentClassLoaderAction(parentClassLoader));
        mapper.addRule("Server/Service/Engine", mapper.addChild
                       ("setContainer", "org.apache.catalina.Container"));

        mapper.addRule("Server/Service/Engine/Listener", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        createStartMapperContext("Server/Service/Engine/Context", mapper);
        createStartMapperDefaultContext(
                                        "Server/Service/Engine/DefaultContext",
                                        mapper);

        mapper.addRule("Server/Service/Engine/Host", mapper.objectCreate
                       ("org.apache.catalina.core.StandardHost",
                        "className"));
        mapper.addRule("Server/Service/Engine/Host", mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host",
                       new CopyParentClassLoaderAction());
        mapper.addRule("Server/Service/Engine/Host",
                       new LifecycleListenerAction
                           ("org.apache.catalina.startup.HostConfig",
                            "configClass"));
        mapper.addRule("Server/Service/Engine/Host", mapper.addChild
                       ("addChild", "org.apache.catalina.Container"));

        mapper.addRule("Server/Service/Engine/Host/Alias",
                       mapper.methodSetter("addAlias", 0));

        mapper.addRule("Server/Service/Engine/Host/Cluster",
                       mapper.objectCreate(null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Cluster",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Cluster",
                       mapper.addChild("setCluster",
                                       "org.apache.catalina.Cluster"));

        createStartMapperContext("Server/Service/Engine/Host/Context", mapper);
        createStartMapperDefaultContext
            ("Server/Service/Engine/Host/DefaultContext", mapper);

        mapper.addRule("Server/Service/Engine/Host/Context/Manager/Store",
                       mapper.objectCreate(null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Context/Manager/Store",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Context/Manager/Store",
                       mapper.addChild("setStore", "org.apache.catalina.Store"));

        mapper.addRule("Server/Service/Engine/Host/Listener",
                       mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule("Server/Service/Engine/Host/Logger", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Logger",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Logger", mapper.addChild
                       ("setLogger", "org.apache.catalina.Logger"));

        mapper.addRule("Server/Service/Engine/Host/Realm", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Realm",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Realm", mapper.addChild
                       ("setRealm", "org.apache.catalina.Realm"));

        mapper.addRule("Server/Service/Engine/Host/Resources",
                       mapper.objectCreate
                       ("org.apache.naming.resources.FileDirContext",
                        "className"));
        mapper.addRule("Server/Service/Engine/Host/Resources",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Resources", mapper.addChild
                       ("setResources", "javax.naming.directory.DirContext"));

        mapper.addRule("Server/Service/Engine/Host/Valve", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Host/Valve",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Host/Valve", mapper.addChild
                       ("addValve", "org.apache.catalina.Valve"));

        mapper.addRule("Server/Service/Engine/Listener", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Listener",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule("Server/Service/Engine/Logger", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Logger", mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Logger", mapper.addChild
                       ("setLogger", "org.apache.catalina.Logger"));

        mapper.addRule("Server/Service/Engine/Realm", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Realm", mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Realm", mapper.addChild
                       ("setRealm", "org.apache.catalina.Realm"));

        mapper.addRule("Server/Service/Engine/Resources", mapper.objectCreate
                       ("org.apache.naming.resources.FileDirContext",
                        "className"));
        mapper.addRule("Server/Service/Engine/Resources",
                       mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Resources", mapper.addChild
                       ("setResources", "javax.naming.directory.DirContext"));

        mapper.addRule("Server/Service/Engine/Valve", mapper.objectCreate
                       (null, "className"));
        mapper.addRule("Server/Service/Engine/Valve", mapper.setProperties());
        mapper.addRule("Server/Service/Engine/Valve", mapper.addChild
                       ("addValve", "org.apache.catalina.Valve"));

        return (mapper);

    }


    /**
     * Create the mapper rules for a Context, based on the specified prefix.
     *
     * @param prefix Prefix to rule selectors to be created
     * @param mapper The mapper we are updating
     */
    protected void createStartMapperContext(String prefix, XmlMapper mapper) {

        mapper.addRule(prefix + "", mapper.objectCreate
                       ("org.apache.catalina.core.StandardContext",
                        "className"));
        mapper.addRule(prefix + "", mapper.setProperties());
        mapper.addRule(prefix + "",
                       new CopyParentClassLoaderAction());
        mapper.addRule(prefix + "",
                       new LifecycleListenerAction
                           ("org.apache.catalina.startup.ContextConfig",
                            "configClass"));
        mapper.addRule(prefix + "", mapper.addChild
                       ("addChild", "org.apache.catalina.Container"));

        createContextCommon(prefix, mapper);

    }


    /**
     * Create the mapper rules for a DefaultContext, based on the
     * specified prefix.
     *
     * @param prefix Prefix to rule selectors to be created
     * @param mapper The mapper we are updating
     */
    protected void createStartMapperDefaultContext(String prefix,
        XmlMapper mapper) {

        mapper.addRule(prefix + "", mapper.objectCreate
                       ("org.apache.catalina.core.DefaultContext",
                        "className"));
        mapper.addRule(prefix + "", mapper.setProperties());
        mapper.addRule(prefix + "", mapper.addChild
                       ("addDefaultContext", "org.apache.catalina.core.DefaultContext"));

        createContextCommon(prefix, mapper);

    }


    /**
     * Create the mapper rules for a Context which are common to both
     * a Context and a DefaultContext, based on the specified prefix.
     *
     * @param prefix Prefix to rule selectors to be created
     * @param mapper The mapper we are updating
     */
    protected void createContextCommon(String prefix, XmlMapper mapper) {

        mapper.addRule(prefix + "/Ejb", mapper.objectCreate
                       ("org.apache.catalina.deploy.ContextEjb"));
        mapper.addRule(prefix + "/Ejb",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Ejb", mapper.addChild
                       ("addEjb", "org.apache.catalina.deploy.ContextEjb"));

        mapper.addRule(prefix + "/Environment",
                       mapper.objectCreate
                       ("org.apache.catalina.deploy.ContextEnvironment"));
        mapper.addRule(prefix + "/Environment",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Environment",
                       mapper.addChild
                       ("addEnvironment",
                        "org.apache.catalina.deploy.ContextEnvironment"));

        mapper.addRule(prefix + "/InstanceListener",
                       mapper.methodSetter("addInstanceListener", 0));

        mapper.addRule(prefix + "/Listener",
                       mapper.objectCreate
                       (null, "className"));
        mapper.addRule(prefix + "/Listener",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Listener", mapper.addChild
                       ("addLifecycleListener",
                        "org.apache.catalina.LifecycleListener"));

        mapper.addRule(prefix + "/Loader", new CreateLoaderAction
            ("org.apache.catalina.loader.WebappLoader", "className"));
        mapper.addRule(prefix + "/Loader",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Loader", mapper.addChild
                       ("setLoader", "org.apache.catalina.Loader"));

        mapper.addRule(prefix + "/Logger", mapper.objectCreate
                       (null, "className"));
        mapper.addRule(prefix + "/Logger",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Logger", mapper.addChild
                       ("setLogger", "org.apache.catalina.Logger"));

        mapper.addRule(prefix + "/Manager",
                       mapper.objectCreate
                       ("org.apache.catalina.session.StandardManager",
                        "className"));
        mapper.addRule(prefix + "/Manager",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Manager", mapper.addChild
                       ("setManager", "org.apache.catalina.Manager"));

        mapper.addRule(prefix + "/Parameter", mapper.objectCreate
                       ("org.apache.catalina.deploy.ApplicationParameter"));
        mapper.addRule(prefix + "/Parameter",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Parameter", mapper.addChild
                       ("addApplicationParameter",
                        "org.apache.catalina.deploy.ApplicationParameter"));

        mapper.addRule(prefix + "/Realm", mapper.objectCreate
                       (null, "className"));
        mapper.addRule(prefix + "/Realm",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Realm", mapper.addChild
                       ("setRealm", "org.apache.catalina.Realm"));

        mapper.addRule(prefix + "/Resource",
                       mapper.objectCreate
                       ("org.apache.catalina.deploy.ContextResource"));
        mapper.addRule(prefix + "/Resource",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Resource",
                       mapper.addChild
                       ("addResource",
                        "org.apache.catalina.deploy.ContextResource"));

        mapper.addRule(prefix + "/ResourceParams", mapper.objectCreate
                       ("org.apache.catalina.deploy.ResourceParams"));
        mapper.addRule(prefix + "/ResourceParams",
                       mapper.setProperties());
        mapper.addRule(prefix + "/ResourceParams", mapper.addChild
                       ("addResourceParams",
                        "org.apache.catalina.deploy.ResourceParams"));

        mapper.addRule(prefix + "/ResourceParams/parameter",
                       mapper.methodSetter("addParameter", 2));
        mapper.addRule(prefix + "/ResourceParams/parameter/name",
                       mapper.methodParam(0));
        mapper.addRule(prefix + "/ResourceParams/parameter/value",
                       mapper.methodParam(1));

        mapper.addRule(prefix + "/Resources",
                       mapper.objectCreate
                       ("org.apache.naming.resources.FileDirContext",
                        "className"));
        mapper.addRule(prefix + "/Resources",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Resources", mapper.addChild
                       ("setResources", "javax.naming.directory.DirContext"));

        mapper.addRule(prefix + "/Valve", mapper.objectCreate
                       (null, "className"));
        mapper.addRule(prefix + "/Valve",
                       mapper.setProperties());
        mapper.addRule(prefix + "/Valve", mapper.addChild
                       ("addValve", "org.apache.catalina.Valve"));

        mapper.addRule(prefix + "/WrapperLifecycle",
                       mapper.methodSetter("addWrapperLifecycle", 0));

        mapper.addRule(prefix + "/WrapperListener",
                       mapper.methodSetter("addWrapperListener", 0));

    }


    /**
     * Create and configure the XmlMapper we will be using for shutdown.
     */
    protected XmlMapper createStopMapper() {

        // Initialize the mapper
        XmlMapper mapper = new XmlMapper();
        //        mapper.setDebug(999);

        // Configure the actions we will be using

        mapper.addRule("Server", mapper.objectCreate
                       ("org.apache.catalina.core.StandardServer",
                        "className"));
        mapper.addRule("Server", mapper.setProperties());
        mapper.addRule("Server", mapper.addChild
                       ("setServer", "org.apache.catalina.Server"));

        return (mapper);

    }


    /**
     * Execute the processing that has been configured from the command line.
     */
    protected void execute() throws Exception {

        if (starting)
            start();
        else if (stopping)
            stop();

    }


    /**
     * Set the <code>catalina.base</code> System property to the current
     * working directory if it has not been set.
     */
    protected void setCatalinaBase() {

        if (System.getProperty("catalina.base") != null)
            return;
        System.setProperty("catalina.base",
                           System.getProperty("catalina.home"));

    }


    /**
     * Set the <code>catalina.home</code> System property to the current
     * working directory if it has not been set.
     */
    protected void setCatalinaHome() {

        if (System.getProperty("catalina.home") != null)
            return;
        System.setProperty("catalina.home",
                           System.getProperty("user.dir"));

    }


    /**
     * Start a new server instance.
     */
    protected void start() {

        // Create and execute our mapper
        XmlMapper mapper = createStartMapper();
        File file = configFile();
        try {
            mapper.readXml(file, this);
        } catch (InvocationTargetException e) {
            System.out.println("Catalina.start: InvocationTargetException");
            e.getTargetException().printStackTrace(System.out);
        } catch (Exception e) {
            System.out.println("Catalina.start: " + e);
            e.printStackTrace(System.out);
            System.exit(1);
        }

        // Setting additional variables
        if (!useNaming) {
            System.setProperty("catalina.useNaming", "false");
        } else {
            System.setProperty("catalina.useNaming", "true");
            String value = "org.apache.naming";
            String oldValue =
                System.getProperty(javax.naming.Context.URL_PKG_PREFIXES);
            if (oldValue != null) {
                value = value + ":" + oldValue;
            }
            System.setProperty(javax.naming.Context.URL_PKG_PREFIXES, value);
            System.setProperty(javax.naming.Context.INITIAL_CONTEXT_FACTORY,
                               "org.apache.naming.java.javaURLContextFactory");
        }

        // If a SecurityManager is being used, set properties for
        // checkPackageAccess() and checkPackageDefinition
        if( System.getSecurityManager() != null ) {
            String access = Security.getProperty("package.access");
            if( access != null && access.length() > 0 )
                access += ",";
            else
                access = "sun.,";
            Security.setProperty("package.access",
                access + "org.apache.catalina.,org.apache.jasper.");
            String definition = Security.getProperty("package.definition");
            if( definition != null && definition.length() > 0 )
                definition += ",";
            else
                definition = "sun.,";
            Security.setProperty("package.definition",
                // FIX ME package "javax." was removed to prevent HotSpot
                // fatal internal errors
                definition + "java.,org.apache.catalina.,org.apache.jasper.");
        }

        // Start the new server
        if (server instanceof Lifecycle) {
            try {
                server.initialize();
                ((Lifecycle) server).start();
            } catch (LifecycleException e) {
                System.out.println("Catalina.start: " + e);
                e.printStackTrace(System.out);
                if (e.getThrowable() != null) {
                    System.out.println("----- Root Cause -----");
                    e.getThrowable().printStackTrace(System.out);
                }
            }
        }


        // Wait for the server to be told to shut down
        server.await();

        // Shut down the server
        if (server instanceof Lifecycle) {
            try {
                ((Lifecycle) server).stop();
            } catch (LifecycleException e) {
                System.out.println("Catalina.stop: " + e);
                e.printStackTrace(System.out);
                if (e.getThrowable() != null) {
                    System.out.println("----- Root Cause -----");
                    e.getThrowable().printStackTrace(System.out);
                }
            }
        }

    }


    /**
     * Stop an existing server instance.
     */
    protected void stop() {

      // Create and execute our mapper
      XmlMapper mapper = createStopMapper();
      File file = configFile();
        try {
            mapper.readXml(file, this);
        } catch (Exception e) {
            System.out.println("Catalina.stop: " + e);
            e.printStackTrace(System.out);
            System.exit(1);
        }

      // Stop the existing server
      try {
          Socket socket = new Socket("127.0.0.1", server.getPort());
          OutputStream stream = socket.getOutputStream();
          String shutdown = server.getShutdown();
          for (int i = 0; i < shutdown.length(); i++)
              stream.write(shutdown.charAt(i));
          stream.flush();
          stream.close();
          socket.close();
      } catch (IOException e) {
          System.out.println("Catalina.stop: " + e);
          e.printStackTrace(System.out);
          System.exit(1);
      }


    }


    /**
     * Print usage information for this application.
     */
    protected void usage() {

        System.out.println
            ("usage: java org.apache.catalina.startup.Catalina"
             + " [ -config {pathname} ] [ -debug ]"
             + " [ -nonaming ] { start | stop }");

    }


}


// ------------------------------------------------------------ Private Classes


/**
 * Class that copies the parentClassLoader property from the next-to-top
 * item on the stack (which must be a Container) to the top item on the
 * stack (which must also be a Container).
 */

final class CopyParentClassLoaderAction extends XmlAction {


    /**
     * Construct a new action.
     */
    public CopyParentClassLoaderAction() {
        super();
    }


    /**
     * Copy the required class loader.
     */
    public void start(SaxContext context) throws Exception {
        if (context.getDebug() >= 1)
            context.log("Copying parent class loader");
        Stack stack = context.getObjectStack();
        Container child = (Container) stack.pop();
        Container parent = (Container) stack.peek();
        stack.push(child);
        child.setParentClassLoader(parent.getParentClassLoader());
    }

}


/**
 * Class that creates a new loader instance, with the parent class
 * loader associated with the top object on the stack (which must be a
 * Container), and pushes it on to the stack.
 */

final class CreateLoaderAction extends XmlAction {


    /**
     * Construct a new action.
     */
    public CreateLoaderAction(String loaderClass) {

        this(loaderClass, null);

    }


    /**
     * Construct a new action.
     */
    public CreateLoaderAction(String loaderClass, String attributeName) {

        super();
        this.loaderClass = loaderClass;
        this.attributeName = attributeName;

    }


    /**
     * Classname of the loader.
     */
    protected String loaderClass;


    /**
     * The attribute name of the optional override class (if any).
     */
    protected String attributeName;


    /**
     * Add the requested Loader implemenation.
     */
    public void start(SaxContext context) throws Exception {

        // Look up the required parent class loader
        Stack stack = context.getObjectStack();
        Container container = (Container) stack.peek();
        ClassLoader parentClassLoader = container.getParentClassLoader();

        // Instantiate a new Loader implementation object
        String className = loaderClass;
        if (attributeName != null) {
            int top = context.getTagCount() - 1;
            AttributeList attributes = context.getAttributeList(top);
            if (attributes.getValue(attributeName) != null)
                className = attributes.getValue(attributeName);
        }
        Class clazz = Class.forName(className);
        Class[] paramTypes = { ClassLoader.class };
        Object[] arguments = { parentClassLoader };
        Constructor constructor = clazz.getDeclaredConstructor(paramTypes);
        Loader loader = (Loader) constructor.newInstance(arguments);

        // Push the new loader onto the stack
        stack.push(loader);
        if (context.getDebug() >= 1)
            context.log("new " + loader.getClass().getName());

    }


    /**
     * Pop the loader off the top of the stack.
     */
    public void cleanup(SaxContext context) {
        Stack stack = context.getObjectStack();
        Object o = stack.pop();
        if (context.getDebug() >= 1)
            context.log("pop " + o.getClass().getName());

    }

}


/**
 * Class that adds a LifecycleListener for the top class on the stack.
 */

final class LifecycleListenerAction extends XmlAction {


    /**
     * Construct a new action.
     *
     * @param listenerClass Name of the listener class to create
     */
    public LifecycleListenerAction(String listenerClass) {

        this(listenerClass, null);

    }


    /**
     * Construct a new action.
     *
     * @param listenerClass Name of the listener class to create
     * @param attributeName Name of an attribute optionally overriding
     *  the listener class name
     */
    public LifecycleListenerAction(String listenerClass,
                                   String attributeName) {

        super();
        this.listenerClass = listenerClass;
        this.attributeName = attributeName;

    }


    /**
     * The attribute name of the optional override class (if any).
     */
    private String attributeName = null;


    /**
     * The class name of the listener class to create.
     */
    private String listenerClass = null;


    /**
     * Add the requested lifecycle listener.
     */
    public void start(SaxContext context) throws Exception {

        // Create a new listener object
        String className = listenerClass;
        if (attributeName != null) {
            int top = context.getTagCount() - 1;
            AttributeList attributes = context.getAttributeList(top);
            if (attributes.getValue(attributeName) != null)
                className = attributes.getValue(attributeName);
        }
        if (context.getDebug() >= 1)
            context.log("Add " + className + " listener");
        Class clazz = Class.forName(className);
        LifecycleListener listener =
            (LifecycleListener) clazz.newInstance();

        // Add it to the top object on the stack
        Stack stack = context.getObjectStack();
        Lifecycle top = (Lifecycle) stack.peek();
        top.addLifecycleListener(listener);

    }


}


/**
 * Class that sets the parent class loader for the top class on the stack.
 */

final class SetParentClassLoaderAction extends XmlAction {


    /**
     * Construct a new action.
     *
     * @param parentClassLoader The new parent class loader
     */
    public SetParentClassLoaderAction(ClassLoader parentClassLoader) {

        this.parentClassLoader = parentClassLoader;

    }


    /**
     * The parent class loader to be set.
     */
    private ClassLoader parentClassLoader = null;


    /**
     * Add the requested parent class loader
     */
    public void start(SaxContext context) throws Exception {

        if (context.getDebug() >= 1)
            context.log("Setting parent class loader");

        // Set the parent class loader for this Container
        Stack stack = context.getObjectStack();
        Container top = (Container) stack.peek();
        top.setParentClassLoader(parentClassLoader);

    }


}
