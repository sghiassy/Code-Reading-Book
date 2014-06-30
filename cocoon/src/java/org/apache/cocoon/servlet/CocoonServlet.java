/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.servlet;

import org.apache.avalon.excalibur.logger.DefaultLogKitManager;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.framework.activity.Initializable;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.DefaultConfigurationBuilder;
import org.apache.avalon.framework.context.DefaultContext;
import org.apache.avalon.framework.logger.Loggable;
import org.apache.cocoon.Constants;
import org.apache.cocoon.components.notification.Notifying;
import org.apache.cocoon.components.notification.SimpleNotifyingBean;
import org.apache.cocoon.components.notification.NotifyingBuilder;
import org.apache.cocoon.components.notification.DefaultNotifyingBuilder;
import org.apache.cocoon.components.notification.Notifier;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.ConnectionResetException;
import org.apache.cocoon.Cocoon;
import org.apache.cocoon.components.classloader.RepositoryClassLoader;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.http.HttpContext;
import org.apache.cocoon.environment.http.HttpEnvironment;
import org.apache.cocoon.environment.http.RequestWrapper;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.IOUtils;
import org.apache.cocoon.util.StringUtils;
import org.apache.cocoon.util.log.CocoonLogFormatter;
import org.apache.log.ContextMap;
import org.apache.log.Hierarchy;
import org.apache.log.Logger;
import org.apache.log.Priority;
import org.apache.log.output.ServletOutputLogTarget;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.Arrays;
import java.util.HashMap;
import java.util.StringTokenizer;

/**
 * This is the entry point for Cocoon execution as an HTTP Servlet.
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/22 00:17:13 $
 */
public class CocoonServlet extends HttpServlet {

    protected Logger log;
    protected LogKitManager logKitManager;

    static final float SECOND = 1000;
    static final float MINUTE = 60 * SECOND;
    static final float HOUR   = 60 * MINUTE;

    /** The time the cocoon instance was created */
    protected long creationTime = 0;

    /** The <code>Cocoon</code> instance */
    protected Cocoon cocoon;

    protected Exception exception;

    protected DefaultContext appContext = new DefaultContext();

    /** Allow reloading of cocoon by specifying the cocoon-reload parameter with a request */
    protected boolean allowReload;

    /** Allow adding processing time to the response */
    protected boolean showTime;
    protected boolean hiddenShowTime;

    private static final boolean ALLOW_OVERWRITE = false;
    private static final boolean SILENTLY_RENAME = true;
    private static final boolean SAVE_UPLOADED_FILES_TO_DISK = true;
    private static final int MAX_UPLOAD_SIZE = 10000000; // 10Mb
    private int maxUploadSize = MAX_UPLOAD_SIZE; // 10Mb
    private File uploadDir;
    private File workDir;
    private File cacheDir;

    protected ServletContext servletContext;
    protected RepositoryClassLoader classLoader;

    private String parentComponentManagerClass;

    protected String forceLoadParameter;
    protected String forceSystemProperty;
    private boolean addClassDirs;

    /**
     * This is the path to the servlet context (or the result
     * of calling getRealPath('/') on the ServletContext.
     * Note, that this can be null.
     */
    protected String servletContextPath;

    /**
     * This is the url to the servlet context directory
     */
    protected URL servletContextURL;

    /**
     * Initialize this <code>CocoonServlet</code> instance.  You will
     * notice that I have broken the init into sub methods to make it
     * easier to maintain (BL).  The context is passed to a couple of
     * the subroutines.  This is also because it is better to explicitly
     * pass variables than implicitely.  It is both more maintainable,
     * and more elegant.
     *
     * @param conf The ServletConfig object from the servlet engine.
     *
     * @throws ServletException
     */
    public void init(ServletConfig conf)
    throws ServletException {

        super.init(conf);
        String value;

        this.servletContext = conf.getServletContext();
        this.appContext.put(Constants.CONTEXT_ENVIRONMENT_CONTEXT, new HttpContext(this.servletContext));

        this.servletContextPath = this.servletContext.getRealPath("/");
        // first init the work-directory for the logger.
        // this is required if we are running inside a war file!
        final String workDirParam = conf.getInitParameter("work-directory");
        if ((workDirParam != null) && (!workDirParam.trim().equals(""))) {
            if (this.servletContextPath == null) {
                this.workDir = new File(workDirParam);
            } else {
                this.workDir = IOUtils.createFile( new File(servletContextPath) , workDirParam);
            }
            this.workDir.mkdirs();
        } else {
            this.workDir = (File) this.servletContext.getAttribute("javax.servlet.context.tempdir");
            this.workDir = new File(workDir, "cocoon-files");
            this.workDir.mkdirs();
        }

        this.initLogger();
        String path = this.servletContextPath;
        if (log.isDebugEnabled()) {
            log.debug("getRealPath for /: " + path);
        }
        if (path == null) {
            // Try to figure out the path of the root from that of WEB-INF
            try {
                path = this.servletContext.getResource("/WEB-INF").toString();
            } catch (java.net.MalformedURLException me) {
                throw new ServletException("Unable to get resource 'WEB-INF'.", me);
            }
            if (log.isDebugEnabled()) {
                log.debug("getResource for /WEB-INF: " + path);
            }
            path = path.substring(0,path.length() - "WEB-INF".length());
            if (log.isDebugEnabled()) {
                log.debug("Path for Root: " + path);
            }
        }

        try {
            if (path.indexOf(':') > 1) {
                this.servletContextURL = new URL(path);
            } else {
                this.servletContextURL = (new File(path)).toURL();
            }
        } catch (java.net.MalformedURLException me) {
            throw new ServletException("Unable to determine servlet context URL.", me);
        }
        if (log.isDebugEnabled()) {
            log.debug("URL for Root: " + this.servletContextURL);
        }

        this.forceLoadParameter = conf.getInitParameter("load-class");
        if (conf.getInitParameter("load-class") == null) {
            if (log.isDebugEnabled()) {
                log.debug("load-class was not set - defaulting to false?");
            }
        }

        this.forceSystemProperty = conf.getInitParameter("force-property");

        value = conf.getInitParameter("init-classloader");
        this.addClassDirs = "true".equalsIgnoreCase(value) || "yes".equalsIgnoreCase(value);
        if (value == null) {
            if (log.isDebugEnabled()) {
                log.debug("init-classloader was not set - defaulting to false");
            }
        }

        // add work directory
        if ((workDirParam != null) && (!workDirParam.trim().equals(""))) {
            if (log.isDebugEnabled()) {
                log.debug("using work-directory " + this.workDir);
            }
        } else {
            if (log.isDebugEnabled()) {
                log.debug("work-directory was not set - defaulting to " + this.workDir);
            }
        }
        this.appContext.put(Constants.CONTEXT_WORK_DIR, workDir);

        final String uploadDirParam = conf.getInitParameter("upload-directory");
        if ((uploadDirParam != null) && (!uploadDirParam.trim().equals(""))) {
            if (this.servletContextPath == null) {
                this.uploadDir = new File(uploadDirParam);
            } else {
                this.uploadDir = IOUtils.createFile( new File(servletContextPath) , uploadDirParam);
            }
            this.uploadDir.mkdirs();
            if (log.isDebugEnabled()) {
                log.debug("using upload-directory " + this.uploadDir);
            }
        } else        {
            this.uploadDir = IOUtils.createFile(workDir, "upload-dir" + File.separator);
            if (log.isDebugEnabled()) {
                log.debug("upload-directory was not set - defaulting to " + this.uploadDir);
            }
        }

        this.appContext.put(Constants.CONTEXT_UPLOAD_DIR, this.uploadDir);
        this.uploadDir.mkdirs();

        String maxSizeParam = conf.getInitParameter("upload-max-size");
        if ((maxSizeParam != null) && (!maxSizeParam.trim().equals(""))) {
            this.maxUploadSize = Integer.parseInt(maxSizeParam);
        }

        String cacheDirParam = conf.getInitParameter("cache-directory");
        if ((cacheDirParam != null) && (!cacheDirParam.trim().equals(""))) {
            if (this.servletContextPath == null) {
                this.cacheDir = new File(uploadDirParam);
            } else {
                this.cacheDir = IOUtils.createFile( new File(servletContextPath) , cacheDirParam);
            }
            this.cacheDir.mkdirs();
            if (log.isDebugEnabled()) {
                log.debug("using cache-directory " + this.cacheDir);
            }
        } else        {
            this.cacheDir = IOUtils.createFile(workDir, "cache-dir" + File.separator);
            if (log.isDebugEnabled()) {
                log.debug("cache-directory was not set - defaulting to " + this.cacheDir);
            }
        }

        this.appContext.put(Constants.CONTEXT_CACHE_DIR, this.cacheDir);
        this.cacheDir.mkdirs();

        this.appContext.put(Constants.CONTEXT_CONFIG_URL,
                            this.getConfigFile(conf.getInitParameter("configurations")));
        if (conf.getInitParameter("configurations") == null) {
            if (log.isDebugEnabled()) {
                log.debug("configurations was not set - defaulting to... ?");
            }
        }

        // get allow reload parameter, default is true
        value = conf.getInitParameter("allow-reload");
        this.allowReload = (value == null || value.equalsIgnoreCase("yes") || value.equalsIgnoreCase("true"));
        if (value == null) {
            if (log.isDebugEnabled()) {
                log.debug("allow-reload was not set - defaulting to true");
            }
        }

        value = conf.getInitParameter("show-time");
        this.showTime = "yes".equalsIgnoreCase(value) || "true".equalsIgnoreCase(value)
            || (this.hiddenShowTime = "hide".equals(value));
        if (value == null) {
            if (log.isDebugEnabled()) {
                log.debug("show-time was not set - defaulting to false");
            }
        }

        parentComponentManagerClass = conf.getInitParameter("parent-component-manager");
        if (parentComponentManagerClass == null) {
            if (log.isDebugEnabled()) {
                log.debug("parent-component-manager was not set - defaulting to null.");
            }
        }

        this.createCocoon();
    }

    /**
     * Dispose Cocoon when servlet is destroyed
     */
    public void destroy()
    {
        if (this.cocoon != null)
        {
            if (log.isDebugEnabled()) {
                log.debug("Servlet destroyed - disposing Cocoon");
            }
            this.disposeCocoon();
        }
    }

     /**
      * get the classloader to use for Cocoon instantiation
      */
     protected RepositoryClassLoader buildInitClassLoader() {
         return new RepositoryClassLoader(new URL[] {}, this.getClass().getClassLoader());
     }

    /**
     * This builds the important ClassPath used by this Servlet.  It
     * does so in a Servlet Engine neutral way.  It uses the
     * <code>ServletContext</code>'s <code>getRealPath</code> method
     * to get the Servlet 2.2 identified classes and lib directories.
     * It iterates in alphabetical order through every file in the
     * lib directory and adds it to the classpath.
     *
     * Also, we add the files to the ClassLoader for the Cocoon system.
     * In order to protect ourselves from skitzofrantic classloaders,
     * we need to work with a known one.
     *
     * We need to get this to work properly when Cocoon is in a war.
     *
     * @throws ServletException
     */
     protected String getClassPath()
     throws ServletException {
        StringBuffer buildClassPath = new StringBuffer();

        File root = null;

        if (servletContextPath != null) {
            // Old method.  There *MUST* be a better method than this...

            String classDir = this.servletContext.getRealPath("/WEB-INF/classes");
            String libDir = this.servletContext.getRealPath("/WEB-INF/lib");

            if (libDir != null) {
                root = new File(libDir);
            }

            if (classDir != null) {
                buildClassPath.append(classDir);

                if (this.addClassDirs) {
                    try {
                        classLoader.addDirectory(new File(classDir));
                    } catch (Exception e) {
                        if (log.isDebugEnabled()) {
                            log.debug("Could not add directory" + classDir, e);
                        }
                    }
                }
            }
        } else {
            // New(ish) method for war'd deployments
            URL classDirURL = null;
            URL libDirURL = null;

            try {
                classDirURL = this.servletContext.getResource("/WEB-INF/classes");
            } catch (java.net.MalformedURLException me) {
                if (log.isWarnEnabled()) {
                    this.log.warn("Unable to add WEB-INF/classes to the classpath", me);
                }
            }

            try {
                libDirURL = this.servletContext.getResource("/WEB-INF/lib");
            } catch (java.net.MalformedURLException me) {
                if (log.isWarnEnabled()) {
                    this.log.warn("Unable to add WEB-INF/lib to the classpath", me);
                }
            }

            if (libDirURL != null && libDirURL.toExternalForm().startsWith("file:")) {
                root = new File(libDirURL.toExternalForm().substring(5));
            }

            if (classDirURL != null) {
                buildClassPath.append(classDirURL.toExternalForm());

                if (this.addClassDirs) {
                    try {
                        classLoader.addURL(classDirURL);
                    } catch (Exception e) {
                        if (log.isDebugEnabled()) {
                            log.debug("Could not add directory " + classDirURL, e);
                        }
                    }
                }
            }
        }

        if (root != null && root.isDirectory()) {
            File[] libraries = root.listFiles();
            Arrays.sort(libraries);
            for (int i = 0; i < libraries.length; i++) {
                buildClassPath.append(File.pathSeparatorChar)
                              .append(IOUtils.getFullFilename(libraries[i]));

                if (this.addClassDirs) {
                    try {
                        classLoader.addDirectory(libraries[i]);
                    } catch (Exception e) {
                        if (log.isDebugEnabled()) {
                            log.debug("Could not add file" + IOUtils.getFullFilename(libraries[i]));
                        }
                    }
                }
            }
        }

        buildClassPath.append(File.pathSeparatorChar)
                      .append(System.getProperty("java.class.path"));

        buildClassPath.append(File.pathSeparatorChar)
                      .append(getExtraClassPath());
        return buildClassPath.toString();
     }


    /**
     * Retreives the "extra-classpath" attribute, that needs to be
     * added to the class path.
     *
     * @throws ServletException
     */
     protected String getExtraClassPath()
     throws ServletException {
         String extraClassPath = this.getInitParameter("extra-classpath");
         if ((extraClassPath != null) && !extraClassPath.trim().equals("")) {
             StringBuffer sb = new StringBuffer();
             StringTokenizer st = new StringTokenizer(extraClassPath, System.getProperty("path.separator"), false);
             int i = 0;
             while (st.hasMoreTokens()) {
                 String s = st.nextToken();
                 if (i++ > 0) {
                     sb.append(java.io.File.pathSeparatorChar);
                 }
                 if ((s.charAt(0) == java.io.File.separatorChar) ||
                     (s.charAt(1) == ':')) {
                     if (log.isDebugEnabled()) {
                         log.debug ("extraClassPath is absolute: " + s);
                     }
                     sb.append(s);

                     if (this.addClassDirs) {
                        try {
                            classLoader.addDirectory(s.toString());
                        } catch (Exception e) {
                            if (log.isDebugEnabled()) {
                                log.debug("Could not add " + s.toString());
                            }
                        }
                     }
                 } else {
                     if (s.indexOf("${") != -1) {
                         String path = StringUtils.replaceToken(s);
                         sb.append(path);
                         if (log.isDebugEnabled()) {
                             log.debug ("extraClassPath is not absolute replacing using token: [" + s + "] : " + path);
                         }
                         if (this.addClassDirs) {
                            try {
                                classLoader.addDirectory(path);
                            } catch (Exception e) {
                                if (log.isDebugEnabled()) {
                                    log.debug("Could not add " + path);
                                }
                            }
                         }
                     } else {
                         String path = null;
                         if (this.servletContextPath != null) {
                             path = this.servletContextPath + s;
                             if (log.isDebugEnabled()) {
                                 log.debug ("extraClassPath is not absolute pre-pending context path: " + path);
                             }
                         } else {
                             path = this.workDir.toString() + s;
                             if (log.isDebugEnabled()) {
                                 log.debug ("extraClassPath is not absolute pre-pending work-directory: " + path);
                             }
                         }
                         sb.append(path);
                         if (this.addClassDirs) {
                            try {
                                classLoader.addDirectory(path);
                            } catch (Exception e) {
                                if (log.isDebugEnabled()) {
                                    log.debug("Could not add " + path);
                                }
                            }
                         }
                     }
                 }
             }
             return sb.toString();
         }
         return "";
     }

    /**
     * Set up the log level and path.  The default log level is
     * Priority.ERROR, although it can be overwritten by the parameter
     * "log-level".  The log system goes to both a file and the Servlet
     * container's log system.  Only messages that are Priority.ERROR
     * and above go to the servlet context.  The log messages can
     * be as restrictive (Priority.FATAL_ERROR and above) or as liberal
     * (Priority.DEBUG and above) as you want that get routed to the
     * file.
     *
     * @throws ServletException
     */
    private void initLogger()
    throws ServletException {
        String logLevel = getInitParameter("log-level");
        if (logLevel == null) {
            logLevel = "INFO";
        }

        final String accesslogger = getInitParameter("servlet-logger");

        final Priority logPriority = Priority.getPriorityForName(logLevel.trim());

        final ServletOutputLogTarget servTarget = new ServletOutputLogTarget(this.servletContext);

        final CocoonLogFormatter formatter = new CocoonLogFormatter();
        formatter.setFormat( "%7.7{priority} %{time}   [%8.8{category}] " +
                             "(%{uri}) %{thread}/%{class:short}: %{message}\\n%{throwable}" );

        servTarget.setFormatter(formatter);
        Hierarchy.getDefaultHierarchy().setDefaultLogTarget(servTarget);
        Hierarchy.getDefaultHierarchy().setDefaultPriority(logPriority);
        final Logger logger = Hierarchy.getDefaultHierarchy().getLoggerFor("");
        final DefaultLogKitManager logKitManager = new DefaultLogKitManager(Hierarchy.getDefaultHierarchy());
        logKitManager.setLogger(logger);
        final DefaultContext subcontext = new DefaultContext(this.appContext);
        subcontext.put("servlet-context", this.servletContext);
        if (this.servletContextPath == null) {
            File logSCDir = new File(this.workDir, "log");
            logSCDir.mkdirs();
            if (logger.isWarnEnabled()) {
                logger.warn("Setting servlet-context for LogKit to " + logSCDir);
            }
            subcontext.put("context-root", logSCDir.toString());
        } else {
            subcontext.put("context-root", this.servletContextPath);
        }

        try {
            logKitManager.contextualize(subcontext);
            this.logKitManager = logKitManager;

            //Configure the logkit management
            final String logkitConfig = getInitParameter("logkit-config");
            if (logkitConfig != null) {
                final InputStream is = this.servletContext.getResourceAsStream(logkitConfig);
                final DefaultConfigurationBuilder builder = new DefaultConfigurationBuilder();
                final Configuration conf = builder.build(is);
                logKitManager.configure(conf);
            }
        } catch (Exception e) {
            Hierarchy.getDefaultHierarchy().log("Could not set up Cocoon Logger, will use screen instead", e);
        }

        if (accesslogger != null) {
            this.log = logKitManager.getLogger(accesslogger);
        } else {
            this.log = logKitManager.getLogger("cocoon");
        }

    }

    /**
     * Set the ConfigFile for the Cocoon object.
     *
     * @param configFileName The file location for the cocoon.xconf
     *
     * @throws ServletException
     */
    private URL getConfigFile(final String configFileName)
    throws ServletException {
        final String usedFileName;

        if (configFileName == null) {
            if (log.isWarnEnabled()) {
                log.warn("Servlet initialization argument 'configurations' not specified, attempting to use '/cocoon.xconf'");
            }
            usedFileName = "/cocoon.xconf";
        } else {
            usedFileName = configFileName;
        }

        if (log.isDebugEnabled()) {
            log.debug("Using configuration file: " + usedFileName);
        }

        try {
            return this.servletContext.getResource(usedFileName);
        } catch (Exception mue) {
            if (log.isErrorEnabled()) {
                log.error("Servlet initialization argument 'configurations' not found at " + usedFileName, mue);
            }
            throw new ServletException("Servlet initialization argument 'configurations' not found at " + usedFileName);
        }
    }

    /**
     * Handle the "force-load" parameter.  This overcomes limits in
     * many classpath issues.  One of the more notorious ones is a
     * bug in WebSphere that does not load the URL handler for the
     * "classloader://" protocol.  In order to overcome that bug,
     * set "force-load" to "com.ibm.servlet.classloader.Handler".
     *
     * If you need to force more than one class to load, then
     * separate each entry with whitespace, a comma, or a semi-colon.
     * Cocoon will strip any whitespace from the entry.
     *
     * @throws ServletException
     */
    private void forceLoad() {
        if (this.forceLoadParameter != null) {
            StringTokenizer fqcnTokenizer = new StringTokenizer(forceLoadParameter, " \t\r\n\f;,", false);

            while (fqcnTokenizer.hasMoreTokens()) {
                final String fqcn = fqcnTokenizer.nextToken().trim();

                try {
                    if (log.isDebugEnabled()) {
                        log.debug("Trying to load class: " + fqcn);
                    }
                    ClassUtils.loadClass(fqcn).newInstance();
                } catch (Exception e) {
                    if (log.isWarnEnabled()) {
                        log.warn("Could not force-load class: " + fqcn, e);
                    }
                    // Do not throw an exception, because it is not a fatal error.
                }
            }
        }
    }

    /**
     * Handle the "force-property" parameter.
     *
     * If you need to force more than one property to load, then
     * separate each entry with whitespace, a comma, or a semi-colon.
     * Cocoon will strip any whitespace from the entry.
     *
     * @throws ServletException
     */
    private void forceProperty() {
        if (this.forceSystemProperty != null) {
            StringTokenizer tokenizer = new StringTokenizer(forceSystemProperty, " \t\r\n\f;,", false);

            java.util.Properties systemProps = System.getProperties();
            while (tokenizer.hasMoreTokens()) {
                final String property = tokenizer.nextToken().trim();
                if (property.indexOf('=') == -1) {
                    continue;
                }
                try {

                    String key = property.substring(0,property.indexOf('='));
                    String value = property.substring(property.indexOf('=') + 1);
                    if (value.indexOf("${") != -1) {
                         value = StringUtils.replaceToken(value);
                    }
                    if (log.isDebugEnabled()) {
                        log.debug("setting " + key + "=" + value);
                    }
                    systemProps.setProperty(key,value);
                } catch (Exception e) {
                    if (log.isWarnEnabled()) {
                        log.warn("Could not set property: " + property, e);
                    }
                    // Do not throw an exception, because it is not a fatal error.
                }
            }
            System.setProperties(systemProps);
        }
    }

    /**
     * Process the specified <code>HttpServletRequest</code> producing output
     * on the specified <code>HttpServletResponse</code>.
     */
    public void service(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        /* HACK for reducing class loader problems.                                     */
        /* example: xalan extensions fail if someone adds xalan jars in tomcat3.2.1/lib */
        try {
            Thread.currentThread().setContextClassLoader(classLoader);
        } catch (Exception e){}

        // This is more scalable
        long start = System.currentTimeMillis();
        HttpServletRequest request = RequestWrapper.getServletRequest(req,
                                         CocoonServlet.SAVE_UPLOADED_FILES_TO_DISK,
                                         this.uploadDir,
                                         CocoonServlet.ALLOW_OVERWRITE,
                                         CocoonServlet.SILENTLY_RENAME,
                                         this.maxUploadSize);

        this.cocoon = getCocoon(request.getPathInfo(), request.getParameter(Constants.RELOAD_PARAM));

        // Check if cocoon was initialized
        if (this.cocoon == null) {
            res.setStatus(res.SC_INTERNAL_SERVER_ERROR);

            SimpleNotifyingBean n= new SimpleNotifyingBean(this);
            n.setType("fatal");
            n.setTitle("Internal servlet error");
            n.setSource("Cocoon servlet");
            n.setMessage("Cocoon was not initialized.");
            n.setDescription("Cocoon was not initialized. Cannot process request.");
            n.addExtraDescription("request-uri", request.getRequestURI());
            res.setContentType(Notifier.notify(n, res.getOutputStream()));

            return;
        }

        // We got it... Process the request
        String uri = request.getServletPath();
        if (uri == null) {
            uri = "";
        }
        String pathInfo = request.getPathInfo();
        if (pathInfo != null) {
            uri += pathInfo;
        }

        if (uri.length() == 0) {
            /* empty relative URI
                 -> HTTP-redirect from /cocoon to /cocoon/ to avoid
                    StringIndexOutOfBoundsException when calling
                    "".charAt(0)
               else process URI normally
            */
            String prefix = request.getRequestURI();

            if (prefix == null) {
                prefix = "";
            }

            res.sendRedirect(res.encodeRedirectURL(prefix + "/"));
            return;
        }

        String contentType = null;
        try {
            if (uri.charAt(0) == '/') {
                uri = uri.substring(1);
            }

            Environment env = this.getEnvironment(uri, request, res);

            // Initialize a fresh log context containing the object model : it
            // will be used by the CocoonLogFormatter
            ContextMap ctxMap = org.apache.log.ContextMap.getCurrentContext();
            ctxMap.clear();
            // Add thread name (default content for empty context)
            String threadName = Thread.currentThread().getName();
            ctxMap.set("threadName", threadName);
            // Add the object model
            ctxMap.set("objectModel", env.getObjectModel());
            // Add a unique request id (threadName + currentTime
            ctxMap.set("request-id", threadName + System.currentTimeMillis());

            if (this.cocoon.process(env)) {
                contentType = env.getContentType();
            } else {
                // Should not get here!
                // means SC_NOT_FOUND
                res.sendError(res.SC_NOT_FOUND);

                SimpleNotifyingBean n = new SimpleNotifyingBean(this);
                n.setType("error");
                n.setTitle("Resource not found");
                n.setSource("Cocoon servlet");
                n.setMessage("The requested resource not found.");
                n.setDescription("The requested URI \""
                                 + request.getRequestURI()
                                 + "\" was not found.");
                n.addExtraDescription("request-uri", request.getRequestURI());
                n.addExtraDescription("path-info", uri);
                // send the notification but don't include it in the output stream
                // as the status SC_NOT_FOUND is enough
                res.setContentType(Notifier.notify(n, (OutputStream)null));
            }
        } catch (ResourceNotFoundException rse) {
            if (log.isWarnEnabled()) {
                log.warn("The resource was not found", rse);
            }

            res.sendError(res.SC_NOT_FOUND);

            SimpleNotifyingBean n = new SimpleNotifyingBean(this);
            n.setType("resource-not-found");
            n.setTitle("Resource not found");
            n.setSource("Cocoon servlet");
            n.setMessage("Resource not found");
            n.setDescription("The requested URI \""
                             + request.getRequestURI()
                             + "\" was not found.");
            n.addExtraDescription("request-uri", request.getRequestURI());
            n.addExtraDescription("path-info", uri);
            // send the notification but don't include it in the output stream
            // as the status SC_NOT_FOUND is enough
            res.setContentType(Notifier.notify(n, (OutputStream)null));

        } catch (ConnectionResetException cre) {
            if (log.isWarnEnabled()) {
                log.warn("The connection was reset", cre);
            }

            SimpleNotifyingBean n = new SimpleNotifyingBean(this);
            n.setType("error");
            n.setTitle("Resource not found");
            n.setSource("Cocoon servlet");
            n.setMessage("Resource not found");
            n.setDescription("The requested URI \""
                             + request.getRequestURI()
                             + "\" was not found.");
            n.addExtraDescription("request-uri", request.getRequestURI());
            n.addExtraDescription("path-info", uri);
            // send the notification but don't include it in the output stream
            // as the connection was reset anyway
            res.setContentType(Notifier.notify(n, (OutputStream)null));
        } catch (Exception e) {
            if (log.isErrorEnabled()) {
                log.error("Problem with servlet", e);
            }
            //res.setStatus(res.SC_INTERNAL_SERVER_ERROR);

            HashMap extraDescriptions = new HashMap(2);
            extraDescriptions.put("request-uri", request.getRequestURI());
            extraDescriptions.put("path-info", uri);

            Notifying n=new DefaultNotifyingBuilder().build(
             this, e, "fatal","Internal server error","Cocoon servlet",null,null,extraDescriptions);

            res.setContentType(contentType = Notifier.notify(n, res.getOutputStream()));
        }

        long end = System.currentTimeMillis();
        String timeString = processTime(end - start);
        if (log.isInfoEnabled()) {
            log.info("'" + uri + "' " + timeString);
        }

        if (contentType != null && contentType.equals("text/html")) {
            String showTime = request.getParameter(Constants.SHOWTIME_PARAM);
            boolean show = this.showTime;
            if (showTime != null) {
                show = !showTime.equalsIgnoreCase("no");
            }
            if (show) {
                boolean hide = this.hiddenShowTime;
                if (showTime != null) {
                    hide = showTime.equalsIgnoreCase("hide");
                }
                ServletOutputStream out = res.getOutputStream();
                out.print((hide) ? "<!-- " : "<p>");
                out.print(timeString);
                out.println((hide) ? " -->" : "</p>");
                out.flush();
                out.close();
            }
        }
    }

    /**
     * Create the environment for the request
     */
    protected Environment getEnvironment(String uri,
                                       HttpServletRequest req,
                                       HttpServletResponse res)
    throws Exception {
        HttpEnvironment env;

        env = new HttpEnvironment(uri,
                                  this.servletContextURL,
                                  req,
                                  res,
                                  this.servletContext,
                                  (HttpContext)this.appContext.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT));
        env.setLogger(this.log);
        return env;
    }

    /**
     * Instatiates the parent component manager, as specified in the
     * parent-component-manager init parameter.
     *
     * If none is specified, the method returns <code>null</code>.
     *
     * @return the parent component manager, or <code>null</code>.
     */
    private synchronized ComponentManager getParentComponentManager() {
        ComponentManager parentComponentManager = null;
        if (parentComponentManagerClass != null) {
            try {
                String initParam = null;
                int dividerPos = parentComponentManagerClass.indexOf('/');
                if (dividerPos != -1) {
                    initParam = parentComponentManagerClass.substring (dividerPos + 1);
                    parentComponentManagerClass = parentComponentManagerClass.substring (0, dividerPos);
                }

                Class pcm = ClassUtils.loadClass(parentComponentManagerClass);
                Constructor pcmc = pcm.getConstructor(new Class[]{String.class});
                parentComponentManager = (ComponentManager) pcmc.newInstance(new Object[]{initParam});

                if (parentComponentManager instanceof Loggable) {
                    ((Loggable) parentComponentManager).setLogger(log);
                }
                if (parentComponentManager instanceof Initializable) {
                    ((Initializable) parentComponentManager).initialize();
                }
            } catch (Exception e) {
                if (log.isErrorEnabled()) {
                    log.error("Could not initialize parent component manager.", e);
                }
            }
        }
        return parentComponentManager;
    }



    /**
     * Creates the Cocoon object and handles exception handling.
     */
    private synchronized void createCocoon()
    throws ServletException {
        this.classLoader = this.buildInitClassLoader();

        /* HACK for reducing class loader problems.                                     */
        /* example: xalan extensions fail if someone adds xalan jars in tomcat3.2.1/lib */
        try {
            Thread.currentThread().setContextClassLoader(this.classLoader);
        } catch (Exception e){}

        this.appContext.put(Constants.CONTEXT_CLASS_LOADER, classLoader);
        this.appContext.put(Constants.CONTEXT_CLASSPATH, this.getClassPath());

        this.forceLoad();
        this.forceProperty();

        try {
            URL configFile = (URL) this.appContext.get(Constants.CONTEXT_CONFIG_URL);
            if (log.isInfoEnabled()) {
                log.info("Reloading from: " + configFile.toExternalForm());
            }
            Cocoon c = (Cocoon) ClassUtils.newInstance("org.apache.cocoon.Cocoon");
            final String rootlogger = getInitParameter("cocoon-logger");
            if (rootlogger != null) {
                c.setLogger(this.logKitManager.getLogger(rootlogger));
            } else {
                c.setLogger(log);
            }
            c.contextualize(this.appContext);
            c.compose(getParentComponentManager ());
            c.setLogKitManager(this.logKitManager);
            c.initialize();
            this.creationTime = System.currentTimeMillis();

            this.disposeCocoon();

            this.cocoon = c;
        } catch (Exception e) {
            if (log.isErrorEnabled()) {
                log.error("Exception reloading", e);
            }
            this.exception = e;

            this.disposeCocoon();
        }
    }

    private String processTime(long time) throws IOException {

        StringBuffer out = new StringBuffer("Processed by ")
                           .append(Constants.COMPLETE_NAME)
                           .append(" in ");

        if (time > HOUR) {
            out.append(time / HOUR);
            out.append(" hours.");
        } else if (time > MINUTE) {
            out.append(time / MINUTE);
            out.append(" minutes.");
        } else if (time > SECOND) {
            out.append(time / SECOND);
            out.append(" seconds.");
        } else {
            out.append(time);
            out.append(" milliseconds.");
        }

        return out.toString();
    }

    /**
     * Gets the current cocoon object.  Reload cocoon if configuration
     * changed or we are reloading.
     *
     * @returns Cocoon
     */
    private Cocoon getCocoon(final String pathInfo, final String reloadParam)
    throws ServletException {
        if (this.cocoon != null && this.allowReload) {
            if (this.cocoon.modifiedSince(this.creationTime)) {
                if (log.isInfoEnabled()) {
                    log.info("Configuration changed reload attempt");
                }
                this.initLogger();
                this.createCocoon();
                return this.cocoon;
            } else if ((pathInfo == null) && (reloadParam != null)) {
                if (log.isInfoEnabled()) {
                    log.info("Forced reload attempt");
                }
                this.initLogger();
                this.createCocoon();
                return this.cocoon;
            }
        } else if ((pathInfo == null) && this.allowReload && (reloadParam != null)) {
            if (log.isInfoEnabled()) {
                log.info("Invalid configurations reload");
            }
            this.initLogger();
            this.createCocoon();
            return this.cocoon;
        }

        return this.cocoon;
    }

    /**
     * Destroy Cocoon
     */
    private final void disposeCocoon()
    {
        if (this.cocoon != null) {
            this.cocoon.dispose();
            this.cocoon = null;
        }
    }
}
