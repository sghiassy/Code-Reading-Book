/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/startup/Bootstrap.java,v 1.26 2001/09/13 14:26:18 glenn Exp $
 * $Revision: 1.26 $
 * $Date: 2001/09/13 14:26:18 $
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
import java.lang.reflect.Method;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import org.apache.catalina.loader.Extension;
import org.apache.catalina.loader.StandardClassLoader;


/**
 * Boostrap loader for Catalina.  This application constructs a class loader
 * for use in loading the Catalina internal classes (by accumulating all of the
 * JAR files found in the "server" directory under "catalina.home"), and
 * starts the regular execution of the container.  The purpose of this
 * roundabout approach is to keep the Catalina internal classes (and any
 * other classes they depend on, such as an XML parser) out of the system
 * class path and therefore not visible to application level classes.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.26 $ $Date: 2001/09/13 14:26:18 $
 */

public final class Bootstrap {


    // ------------------------------------------------------- Static Variables


    /**
     * Debugging detail level for processing the startup.
     */
    private static int debug = 0;


    // ----------------------------------------------------------- Main Program


    /**
     * The main program for the bootstrap.
     *
     * @param args Command line arguments to be processed
     */
    public static void main(String args[]) {

        // Set the debug flag appropriately
        for (int i = 0; i < args.length; i++)  {
            if ("-debug".equals(args[i]))
                debug = 1;
        }

        // Check to see if JNDI is already present in the system classpath
        boolean loadJNDI = true;
        try {
            Class.forName("javax.naming.Context");
            loadJNDI = false;
        } catch (ClassNotFoundException e) {
        }

        // Configure catalina.base from catalina.home if not yet set
        if (System.getProperty("catalina.base") == null)
            System.setProperty("catalina.base", getCatalinaHome());

        // Construct the class loaders we will need
        ClassLoader commonLoader = createCommonLoader(loadJNDI);
        ClassLoader catalinaLoader =
            createCatalinaLoader(commonLoader, loadJNDI);
        ClassLoader sharedLoader =
            createSharedLoader(commonLoader, loadJNDI);

        Thread.currentThread().setContextClassLoader(catalinaLoader);

        // Load our startup class and call its process() method
        try {

            if( System.getSecurityManager() != null ) {
                // Pre load some classes required for SecurityManager
                // so that defineClassInPackage does not throw a
                // security exception.
                String basePackage = "org.apache.catalina.";
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedGetRequestDispatcher");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedGetResource");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedGetResourcePaths");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedLogMessage");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedLogException");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationContext$PrivilegedLogThrowable");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationDispatcher$PrivilegedForward");
                catalinaLoader.loadClass
                    (basePackage +
                     "core.ApplicationDispatcher$PrivilegedInclude");
                catalinaLoader.loadClass
                    (basePackage +
                     "connector.HttpRequestBase$PrivilegedGetSession");
                catalinaLoader.loadClass
                    (basePackage +
                     "loader.WebappClassLoader$PrivilegedFindResource");
                catalinaLoader.loadClass
                    (basePackage + "session.StandardSession");
                catalinaLoader.loadClass
                    (basePackage + "util.CookieTools");
                catalinaLoader.loadClass
                    (basePackage + "util.URL");
                catalinaLoader.loadClass(basePackage + "util.Enumerator");
                catalinaLoader.loadClass("javax.servlet.http.Cookie");
            }

            // Instantiate a startup class instance
            if (debug >= 1)
                log("Loading startup class");
            Class startupClass =
                catalinaLoader.loadClass
                ("org.apache.catalina.startup.Catalina");
            Object startupInstance = startupClass.newInstance();

            // Set the shared extensions class loader
            if (debug >= 1)
                log("Setting startup class properties");
            String methodName = "setParentClassLoader";
            Class paramTypes[] = new Class[1];
            paramTypes[0] = Class.forName("java.lang.ClassLoader");
            Object paramValues[] = new Object[1];
            paramValues[0] = sharedLoader;
            Method method =
                startupInstance.getClass().getMethod(methodName, paramTypes);
            method.invoke(startupInstance, paramValues);

            // Call the process() method
            if (debug >= 1)
                log("Calling startup class process() method");
            methodName = "process";
            paramTypes = new Class[1];
            paramTypes[0] = args.getClass();
            paramValues = new Object[1];
            paramValues[0] = args;
            method =
                startupInstance.getClass().getMethod(methodName, paramTypes);
            method.invoke(startupInstance, paramValues);

        } catch (Exception e) {
            System.out.println("Exception during startup processing");
            e.printStackTrace(System.out);
            System.exit(2);
        }

        System.exit(0);

    }


    /**
     * Construct and return the class loader to be used for loading
     * of the shared system classes.
     *
     * @param loadJNDI Should we load JNDI classes if present?
     */
    private static ClassLoader createCommonLoader(boolean loadJNDI) {

        if (debug >= 1)
            log("Creating COMMON class loader");

        // Construct the "class path" for this class loader
        ArrayList list = new ArrayList();

        // Add the "common/classes" directory if it exists
        File classes = new File(getCatalinaHome(),
                                "common" + File.separator + "classes");
        if (classes.exists() && classes.canRead() &&
            classes.isDirectory()) {
            try {
                URL url = new URL("file", null,
                                  classes.getCanonicalPath() + File.separator);
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   classes.getAbsolutePath());
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Add all JAR files in the "common/lib" directory if it exists
        File directory = new File(getCatalinaHome(), "common/lib");
        if (!directory.exists() || !directory.canRead() ||
            !directory.isDirectory()) {
            System.out.println("Directory " + directory.getAbsolutePath()
                               + " does not exist");
            System.exit(1);
        }
        String filenames[] = directory.list();
        for (int i = 0; i < filenames.length; i++) {
            String filename = filenames[i].toLowerCase();
            if (!filename.endsWith(".jar"))
                continue;
            if ((!loadJNDI) && filename.equals("jndi.jar"))
                continue;
            if (filename.equals("bootstrap.jar"))
                continue;
            File file = new File(directory, filenames[i]);
            try {
                URL url = new URL("file", null, file.getCanonicalPath());
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   filenames[i]);
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Construct the class loader itself
        String array[] = (String[]) list.toArray(new String[list.size()]);
        StandardClassLoader loader = new StandardClassLoader(array);

        return (loader);

    }


    /**
     * Construct and return the class loader to be used for loading
     * Catalina internal classes.
     *
     * @param parent Parent class loader to be assigned
     * @param loadJNDI Should we load JNDI classes if present?
     */
    private static ClassLoader createCatalinaLoader(ClassLoader parent,
                                                    boolean loadJNDI) {

        if (debug >= 1)
            log("Creating CATALINA class loader");

        // Construct the "class path" for this class loader
        ArrayList list = new ArrayList();

        // Add the "server/classes" directory if it exists
        File classes = new File(getCatalinaHome(),
                                "server" + File.separator + "classes");
        if (classes.exists() && classes.canRead() &&
            classes.isDirectory()) {
            try {
                URL url = new URL("file", null,
                                  classes.getCanonicalPath() + File.separator);
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   classes.getAbsolutePath());
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Add all JAR files in the "server/lib" directory if it exists
        File directory = new File(getCatalinaHome(), "server/lib");
        if (!directory.exists() || !directory.canRead() ||
            !directory.isDirectory()) {
            System.out.println("Directory " + directory.getAbsolutePath()
                               + " does not exist");
            System.exit(1);
        }
        String filenames[] = directory.list();
        for (int i = 0; i < filenames.length; i++) {
            String filename = filenames[i].toLowerCase();
            if (!filename.endsWith(".jar"))
                continue;
            if ((!loadJNDI) && filename.equals("jndi.jar"))
                continue;
            File file = new File(directory, filenames[i]);
            try {
                URL url = new URL("file", null, file.getCanonicalPath());
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   filenames[i]);
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Construct the class loader itself
        String array[] = (String[]) list.toArray(new String[list.size()]);
        StandardClassLoader loader = new StandardClassLoader(array, parent);

        return (loader);

    }


    /**
     * Construct and return the class loader to be used for shared
     * extensions by web applications loaded with Catalina.
     *
     * @param parent Parent class loader to be assigned
     * @param loadJNDI Should we load JNDI classes if present?
     */
    private static ClassLoader createSharedLoader(ClassLoader parent,
                                                  boolean loadJNDI) {

        if (debug >= 1)
            log("Creating SHARED class loader");

        // Construct the "class path" for this class loader
        ArrayList list = new ArrayList();

        // Add the "classes" directory if it exists
        File classes = new File(getCatalinaHome(), "classes");
        if (classes.exists() && classes.canRead() &&
            classes.isDirectory()) {
            try {
                URL url = new URL("file", null,
                                  classes.getCanonicalPath() + File.separator);
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   classes.getAbsolutePath());
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Add all JAR files in the "lib" directory if it exists
        File directory = new File(getCatalinaHome(), "lib");
        if (!directory.exists() || !directory.canRead() ||
            !directory.isDirectory()) {
            System.out.println("Directory " + directory.getAbsolutePath()
                               + " does not exist");
            System.exit(1);
        }
        String filenames[] = directory.list();
        for (int i = 0; i < filenames.length; i++) {
            String filename = filenames[i].toLowerCase();
            if (!filename.endsWith(".jar"))
                continue;
            if ((!loadJNDI) && filename.equals("jndi.jar"))
                continue;
            File file = new File(directory, filenames[i]);
            try {
                URL url = new URL("file", null, file.getCanonicalPath());
                if (debug >= 1)
                    log("  Adding " + url.toString());
                list.add(url.toString());
            } catch (IOException e) {
                System.out.println("Cannot create URL for " +
                                   filenames[i]);
                e.printStackTrace(System.out);
                System.exit(1);
            }
        }

        // Construct the class loader itself
        String array[] = (String[]) list.toArray(new String[list.size()]);
        StandardClassLoader loader = new StandardClassLoader(array, parent);

        /*
        System.out.println("AVAILABLE OPTIONAL PACKAGES:");
        Extension available[] = loader.findAvailable();
        for (int i = 0; i < available.length; i++)
            System.out.println(available[i].toString());
        System.out.println("REQUIRED OPTIONAL PACKAGES:");
        Extension required[] = loader.findRequired();
        for (int i = 0; i < required.length; i++)
            System.out.println(required[i].toString());
        System.out.println("===========================");
        */

        return (loader);

    }


    /**
     * Get the value of the catalina.home environment variable.
     */
    private static String getCatalinaHome() {
        return System.getProperty("catalina.home",
                                  System.getProperty("user.dir"));
    }


    /**
     * Log a debugging detail message.
     *
     * @param message The message to be logged
     */
    private static void log(String message) {

        System.out.print("Bootstrap: ");
        System.out.println(message);

    }


}
