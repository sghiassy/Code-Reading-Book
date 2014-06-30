/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.servlet;

import org.apache.cocoon.util.IOUtils;

import javax.servlet.ServletException;
import java.io.File;

/**
 * This is the entry point for Cocoon execution as an HTTP Servlet.
 * It also creates a buffer by loading the whole servlet inside a ClassLoader.
 * It has been changed to extend <code>CocoonServlet</code> so that it is
 * easier to add and change functionality between the two servlets.
 * The only real differences are the ClassLoader and instantiating Cocoon inside
 * of it.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/22 00:17:13 $
 */

public class ParanoidCocoonServlet extends CocoonServlet {

    /**
     * This builds the important ClassPath used by this Servlet.  It
     * does so in a Servlet Engine neutral way.  It uses the
     * <code>ServletContext</code>'s <code>getRealPath</code> method
     * to get the Servlet 2.2 identified classes and lib directories.
     * It iterates through every file in the lib directory and adds
     * it to the classpath.
     *
     * Also, we add the files to the ClassLoader for the Cocoon system.
     * In order to protect ourselves from skitzofrantic classloaders,
     * we need to work with a known one.
     *
     * @param context  The ServletContext to perform the lookup.
     *
     * @throws ServletException
     */
     protected String getClassPath()
     throws ServletException {

        StringBuffer buildClassPath = new StringBuffer();
        String classDirPath = getInitParameter("class-dir");
        String libDirPath = getInitParameter("lib-dir");
        String classDir;
        File root;

        if ((classDirPath != null) && !classDirPath.trim().equals("")) {
            classDir = classDirPath;
        } else {
            classDir = this.servletContext.getRealPath("/WEB-INF/classes");
        }

        if ((libDirPath != null) && !libDirPath.trim().equals("")) {
            root = new File(libDirPath);
        } else {
            root = new File(this.servletContext.getRealPath("/WEB-INF/lib"));
        }

        try {
            classLoader.addDirectory(new File(classDir));
        } catch (Exception e) {
            log.debug("Could not add directory" + classDir, e);
        }

        buildClassPath.append(classDir);

        if (root.isDirectory()) {
            File[] libraries = root.listFiles();

            for (int i = 0; i < libraries.length; i++) {
                buildClassPath.append(File.pathSeparatorChar)
                              .append(IOUtils.getFullFilename(libraries[i]));

                try {
                    classLoader.addDirectory(libraries[i]);
                } catch (Exception e) {
                    log.debug("Could not add file" + IOUtils.getFullFilename(libraries[i]));
                }
            }
        }

        buildClassPath.append(File.pathSeparatorChar)
                      .append(System.getProperty("java.class.path"));

        buildClassPath.append(File.pathSeparatorChar)
                      .append(getExtraClassPath());

        return buildClassPath.toString();
     }
}

