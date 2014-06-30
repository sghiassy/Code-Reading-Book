/*
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
 */ 
package org.apache.jasper.compiler;

import java.io.InputStream;
import java.io.FileNotFoundException;
import java.io.File;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Set;
import java.util.jar.*;
import java.net.JarURLConnection;
import java.net.*;

import javax.servlet.ServletContext;
import javax.servlet.jsp.tagext.TagLibraryInfo;
import javax.servlet.jsp.tagext.TagInfo;
import javax.servlet.jsp.tagext.Tag;

import org.apache.jasper.Constants;
import org.apache.jasper.JasperException;
import org.apache.jasper.logging.Logger;
import org.apache.jasper.parser.ParserUtils;
import org.apache.jasper.parser.TreeNode;

/**
 * A container for all tag libraries that are defined "globally"
 * for the web application.
 * 
 * Tag Libraries can be defined globally in one of two ways:
 *   1. Via <taglib> elements in web.xml:
 *      the uri and location of the tag-library are specified in
 *      the <taglib> element.
 *   2. Via packaged jar files that contain .tld files
 *      within the META-INF directory, or some subdirectory
 *      of it. The taglib is 'global' if it has the <uri>
 *      element defined.
 *
 * A mapping between the taglib URI and its associated TaglibraryInfoImpl
 * is maintained in this container.
 * Actually, that's what we'd like to do. However, because of the
 * way the classes TagLibraryInfo and TagInfo have been defined,
 * it is not currently possible to share an instance of TagLibraryInfo
 * across page invocations. A bug has been submitted to the spec lead.
 * In the mean time, all we do is save the 'location' where the
 * TLD associated with a taglib URI can be found.
 *
 * When a JSP page has a taglib directive, the mappings in this container
 * are first searched (see method getLocation()).
 * If a mapping is found, then the location of the TLD is returned.
 * If no mapping is found, then the uri specified
 * in the taglib directive is to be interpreted as the location for
 * the TLD of this tag library.
 *
 * @author Pierre Delisle
 */

public class TldLocationsCache {

    /**
     * The types of URI one may specify for a tag library
     */
    public static final int ABS_URI = 0;
    public static final int ROOT_REL_URI = 1;
    public static final int NOROOT_REL_URI = 2;

    static private final String WEB_XML = "/WEB-INF/web.xml";
    
    /**
     * The mapping of the 'global' tag library URI to the location
     * (resource path) of the TLD associated with that tag library.
     * The location is returned as a String array:
     *    [0] The location
     *    [1] If the location is a jar file, this is the location
     *        of the tld.
     */
    private Hashtable mappings = new Hashtable();

    //*********************************************************************
    // Constructor and Initilizations

    public TldLocationsCache(ServletContext ctxt) {
        try {
            processWebDotXml(ctxt);
            processJars(ctxt);
        } catch (JasperException ex) {
            Constants.message("jsp.error.internal.tldinit",
                              new Object[] { ex.getMessage() },
                              Logger.ERROR);
        }
    }

    private void processWebDotXml(ServletContext ctxt)
        throws JasperException
    {

        // Acquire an input stream to the web application deployment descriptor
        InputStream is = ctxt.getResourceAsStream(WEB_XML);
        if (is == null) {
            Constants.message("jsp.error.internal.filenotfound",
                              new Object[] {WEB_XML},
                              Logger.WARNING);
            return;
        }

        // Parse the web application deployment descriptor
        ClassLoader cl =
            // (ClassLoader) ctxt.getAttribute(Constants.SERVLET_CLASS_LOADER);
            this.getClass().getClassLoader();
        ParserUtils pu = ParserUtils.createParserUtils(cl);
        TreeNode webtld = pu.parseXMLDocument(WEB_XML, is);
        Iterator taglibs = webtld.findChildren("taglib");
        while (taglibs.hasNext()) {

            // Parse the next <taglib> element
            TreeNode taglib = (TreeNode) taglibs.next();
            String tagUri = null;
            String tagLoc = null;
            TreeNode child = taglib.findChild("taglib-uri");
            if (child != null)
                tagUri = child.getBody();
            child = taglib.findChild("taglib-location");
            if (child != null)
                tagLoc = child.getBody();

            // Save this location if appropriate
            if (tagLoc == null)
                continue;
            if (uriType(tagLoc) == NOROOT_REL_URI)
                tagLoc = "/WEB-INF/" + tagLoc;
            String tagLoc2 = null;
            if (tagLoc.endsWith(".jar"))
                tagLoc2 = "META-INF/taglib.tld";
            mappings.put(tagUri, new String[] {tagLoc, tagLoc2});

        }

    }

    /**
     * Process all the jar files contained in this web application
     * WEB-INF/lib directory.
     */
    private void processJars(ServletContext ctxt)
        throws JasperException
    {

        Set libSet = ctxt.getResourcePaths("/WEB-INF/lib");
        Iterator it = libSet.iterator();
        while (it.hasNext()) {
            String resourcePath = (String) it.next();
            if (resourcePath.endsWith(".jar")) 
                tldConfigJar(ctxt, resourcePath);
        }

    }

    /**
     * Process a TLD in the JAR file at the specified resource path 
     * (if there is one).  Will update the URI mappings for all
     * the .tld files found in the META-INF directory tree, if
     * a <uri> element is defined in the TLD.
     *
     * @param resourcePath Context-relative resource path
     */
    private void tldConfigJar(ServletContext ctxt, String resourcePath) 
        throws JasperException
    {
        JarFile jarFile = null;
        InputStream stream = null;
        try {
            URL url = ctxt.getResource(resourcePath);
            if (url == null) return;
            url = new URL("jar:" + url.toString() + "!/");
            JarURLConnection conn =
                (JarURLConnection) url.openConnection();
            jarFile = conn.getJarFile();
            Enumeration entries = jarFile.entries();
            while (entries.hasMoreElements()) {
                JarEntry entry = (JarEntry) entries.nextElement();
                String name = entry.getName();
                if (!name.startsWith("META-INF/")) continue;
                if (!name.endsWith(".tld")) continue;
                //p("tldConfigJar(" + resourcePath +
                //  "): Processing entry '" + name + "'");
                stream = jarFile.getInputStream(entry);
                String uri = parseTldForUri(resourcePath, stream);
                //p("uri in TLD is: " + uri);
                if (uri != null) {
                    mappings.put(uri, 
                                 new String[]{resourcePath, name});
                    //p("added mapping: " + uri +
                    //  " -> " + resourcePath + " " + name);
                }
            }
            // FIXME @@@
            // -- it seems that the JarURLConnection class caches JarFile 
            // objects for particular URLs, and there is no way to get 
            // it to release the cached entry, so
            // there's no way to redeploy from the same JAR file.  Wierd.
        } catch (Exception ex) {
            if (stream != null) {
                try {
                    stream.close();
                } catch (Throwable t) {}
            }
            if (jarFile != null) {
                try {
                    jarFile.close();
                } catch (Throwable t) {}
            }
        }
    }

    private String parseTldForUri(String resourcePath, InputStream in) 
        throws JasperException
    {

        // Parse the tag library descriptor at the specified resource path
        ParserUtils pu = new ParserUtils();
        TreeNode tld = pu.parseXMLDocument(resourcePath, in);
        TreeNode uri = tld.findChild("uri");
        if (uri != null) {
            String body = uri.getBody();
            if (body != null)
                return body;
        }
        return null; // No <uri> element is present

    }

    //*********************************************************************
    // Accessors

    /**
     * Get the 'location' of the TLD associated with 
     * a given taglib 'uri'.
     * 
     * @returns An array of two Strings. The first one is
     * real path to the TLD. If the path to the TLD points
     * to a jar file, then the second string is the
     * name of the entry for the TLD in the jar file.
     * Returns null if the uri is not associated to
     * a tag library 'exposed' in the web application.
     * A tag library is 'exposed' either explicitely in 
     * web.xml or implicitely via the uri tag in the TLD 
     * of a taglib deployed in a jar file (WEB-INF/lib).
     */
    public String[] getLocation(String uri) 
        throws JasperException
    {
        return (String[])mappings.get(uri);
    }

    //*********************************************************************
    // Utility methods

    /** 
     * Returns the type of a URI:
     *     ABS_URI
     *     ROOT_REL_URI
     *     NOROOT_REL_URI
     */
    static public int uriType(String uri) {
        if (uri.indexOf(':') != -1) {
            return ABS_URI;
        } else if (uri.startsWith("/")) {
            return ROOT_REL_URI;
        } else {
            return NOROOT_REL_URI;
        }
    }

    private void p(String s) {
        System.out.println("[TldLocationsCache] " + s);
    }
}
