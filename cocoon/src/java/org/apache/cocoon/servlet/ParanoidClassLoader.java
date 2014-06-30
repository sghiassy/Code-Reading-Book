/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.servlet;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.net.URLStreamHandlerFactory;

/**
 * The <code>ParanoidClassLoader</code> reverses the search order for
 * classes.  It checks this classloader before it checks its parent.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $
 */

public class ParanoidClassLoader extends URLClassLoader {
    /**
     * The parent <code>ClassLoader</code>.
     */
    private final ClassLoader parent;

    /**
     * Default constructor has no parents or initial <code>URL</code>s.
     */
    public ParanoidClassLoader() {
        this(null, null, null);
    }

    /**
     * Alternate constructor to define a parent.
     */
    public ParanoidClassLoader(final ClassLoader parent) {
        this(null, parent, null);
    }

    /**
     * Alternate constructor to define initial <code>URL</code>s.
     */
    public ParanoidClassLoader(final URL[] urls) {
        this(urls, null, null);
    }

    /**
     * Alternate constructor to define a parent and initial
     * <code>URL</code>s.
     */
    public ParanoidClassLoader(final URL[] urls, final ClassLoader parent) {
        this(urls, parent, null);
    }

    /**
     * Alternate constructor to define a parent, initial
     * <code>URL</code>s, and a default
     * <code>URLStreamHandlerFactory</code>.
     */
    public ParanoidClassLoader(final URL[] urls, final ClassLoader parent, final URLStreamHandlerFactory factory) {
        super(urls, null, factory);
        this.parent = parent;
    }

    /**
     * Extends <code>URLClassLoader</code>'s initialization methods so
     * we return a <code>ParanoidClassLoad</code> instead.
     */
    public static final URLClassLoader newInstance(final URL[] urls) {
        return new ParanoidClassLoader(urls);
    }

    /**
     * Extends <code>URLClassLoader</code>'s initialization methods so
     * we return a <code>ParanoidClassLoad</code> instead.
     */
    public static final URLClassLoader newInstance(final URL[] urls, final ClassLoader parent) {
        return new ParanoidClassLoader(urls, parent);
    }

    /**
     * Loads the class from this <code>ClassLoader</class>.  If the
     * class does not exist in this one, we check the parent.  Please
     * note that this is the exact opposite of the
     * <code>ClassLoader</code> spec.  We use it to work around
     * inconsistent class loaders from third party vendors.
     *
     * @param name of class
     * @throws ClassNotFoundException
     */
    public final Class loadClass(final String name) throws ClassNotFoundException {
        Class resource = null;

        try {
            resource = super.loadClass(name);
        } catch (ClassNotFoundException t) {
            if (this.parent != null) {
                this.parent.loadClass(name);
            } else {
                throw t;
            }
        }

        return resource;
    }

    /**
     * Adds a new directory of class files.
     *
     * @param file for jar or directory
     * @throws IOException
     */
    public final void addDirectory(File file) throws IOException {
        try {
            this.addURL(file.getCanonicalFile().toURL());
        } catch (MalformedURLException mue) {
            throw new IOException("Could not add repository");
        }
    }

    /**
     * Gets a resource from this <code>ClassLoader</class>.  If the
     * resource does not exist in this one, we check the parent.
     * Please note that this is the exact opposite of the
     * <code>ClassLoader</code> spec.  We use it to work around
     * inconsistent class loaders from third party vendors.
     *
     * @param name of resource
     */
    public final URL getResource(final String name) {
        URL resource = super.getResource(name);

        if (this.parent != null && resource == null) {
            resource = this.parent.getResource(name);
        }

        return resource;
    }
}
