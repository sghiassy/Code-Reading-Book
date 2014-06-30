/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.generation;

import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.regexp.RE;
import org.apache.regexp.RESyntaxException;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;
import java.util.Stack;

/**
 * Generates an XML directory listing.
 * <p>
 * The root node of the generated document will normally be a
 * <code>directory</code> node, and a directory node can contain zero
 * or more <code>file</code> or directory nodes. A file node has no
 * children. Each node will contain the following attributes:
 * <blockquote>
 *   <dl>
 *   <dt> name
 *   <dd> the name of the file or directory
 *   <dt> lastModified
 *   <dd> the time the file was last modified, measured as the number of
 *   milliseconds since the epoch (as in java.io.File.lastModified)
 *   <dt> date (optional)
 *   <dd> the time the file was last modified in human-readable form
 *   </dl>
 * </blockquote>
 * <p>
 * <b>Configuration options:</b>
 * <dl>
 * <dt> <i>depth</i> (optional)
 * <dd> Sets how deep DirectoryGenerator should delve into the
 * directory structure. If set to 1 (the default), only the starting
 * directory's immediate contents will be returned.
 * <dt> <i>dateFormat</i> (optional)
 * <dd> Sets the format for the date attribute of each node, as
 * described in java.text.SimpleDateFormat. If unset, the default
 * format for the current locale will be used.
 * </dl>
 *
 * @author <a href="mailto:fumagalli@exoffice.com">Pierpaolo Fumagalli</a>
 *         (Apache Software Foundation, Exoffice Technologies)
 * @author <a href="mailto:conny@smb-tec.com">Conny Krappatsch</a>
 *         (SMB GmbH) for Virbus AG
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:16 $ */

public class DirectoryGenerator extends ComposerGenerator implements Recyclable {
  private static final String FILE = "file:";
  
    /** The URI of the namespace of this generator. */
    protected static final String URI =
    "http://apache.org/cocoon/directory/2.0";

    /** The namespace prefix for this namespace. */
    protected static final String PREFIX = "dir";

    /* Node and attribute names */
    protected static final String DIR_NODE_NAME         = "directory";
    protected static final String FILE_NODE_NAME        = "file";

    protected static final String FILENAME_ATTR_NAME    = "name";
    protected static final String LASTMOD_ATTR_NAME     = "lastModified";
    protected static final String DATE_ATTR_NAME        = "date";

    /*
     * Variables set per-request
     *
     * FIXME: SimpleDateFormat is not supported by all locales!
     */
    protected int depth;
    protected AttributesImpl attributes = new AttributesImpl();
    protected SimpleDateFormat dateFormatter;

    protected RE rootRE;
    protected RE includeRE;
    protected RE excludeRE;

    protected boolean isRequestedDirectory;


    /**
     * Set the request parameters. Must be called before the generate
     * method.
     *
     * @param   resolver
     *      the SourceResolver object
     * @param   objectModel
     *      a <code>Map</code> containing model object
     * @param   src
     *      the URI for this request (?)
     * @param   par
     *      configuration parameters
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, par);

        String dateFormatString = par.getParameter("dateFormat", null);
        if (dateFormatString != null) {
            this.dateFormatter = new SimpleDateFormat(dateFormatString);
        } else {
            this.dateFormatter = new SimpleDateFormat();
        }

        this.depth = par.getParameterAsInteger("depth", 1);
        getLogger().debug("depth: " + this.depth);

        String rePattern = par.getParameter("root", null);
        try {
            getLogger().debug("root pattern: " + rePattern);
            this.rootRE = (rePattern == null)?null:new RE(rePattern);

            rePattern = par.getParameter("include", null);
            getLogger().debug("include pattern: " + rePattern);
            this.includeRE = (rePattern == null)?null:new RE(rePattern);

            rePattern = par.getParameter("exclude", null);
            getLogger().debug("exclude pattern: " + rePattern);
            this.excludeRE = (rePattern == null)?null:new RE(rePattern);
        } catch (RESyntaxException rese) {
            getLogger().error("Syntax error in regexp pattern '" + rePattern + "'", rese);
            throw new ProcessingException("Syntax error in regexp pattern '"
                + rePattern + "'", rese);
        }

        this.isRequestedDirectory = false;

        /* Create a reusable attributes for creating nodes */
        this.attributes = new AttributesImpl();
    }


    /**
     * Generate XML data.
     *
     * @throws  SAXException
     *      if an error occurs while outputting the document
     * @throws  ProcessingException
     *      if the requsted URI isn't a directory on the local
     *      filesystem
     */
    public void generate()
    throws SAXException, ProcessingException {
        String directory = super.source;
        Source inputSource = null;
        try {
            inputSource = this.resolver.resolve(directory);
            String systemId = inputSource.getSystemId();
            if (!systemId.startsWith(FILE)) {
              throw new ResourceNotFoundException(systemId + " does not denote a directory");
            }
            // This relies on systemId being of the form "file://..."
            File directoryFile = new File(new URL(systemId).getFile());
            if (!directoryFile.isDirectory()) {
                throw new ResourceNotFoundException(directory + " is not a directory.");
            }

            this.contentHandler.startDocument();
            this.contentHandler.startPrefixMapping(PREFIX,URI);

            Stack ancestors = getAncestors(directoryFile);
            addPathWithAncestors(directoryFile, ancestors);

            this.contentHandler.endPrefixMapping(PREFIX);
            this.contentHandler.endDocument();
        } catch (IOException ioe) {
            getLogger().warn("Could not read directory " + directory, ioe);
            throw new ResourceNotFoundException("Could not read directory "
                + directory, ioe);
        } finally {
            if (inputSource != null) inputSource.recycle();
        }
    }

    /**
     * Creates a stack containing the ancestors of File up to specified
     * directory.
     * @param path the File whose ancestors shall be retrieved
     *
     * @return a Stack containing the ancestors.
     */
    protected Stack getAncestors(File path) {
        File parent = path;
        Stack ancestors = new Stack();

        while ((parent != null) && !isRoot(parent)) {
            parent = parent.getParentFile();
            if (parent != null) {
                ancestors.push(parent);
            }
        }

        return ancestors;
    }


    protected void addPathWithAncestors(File path, Stack ancestors)
            throws SAXException {

        if (ancestors.empty()) {
            this.isRequestedDirectory = true;
            addPath(path, depth);
        } else {
            startNode(DIR_NODE_NAME, (File)ancestors.pop());
            addPathWithAncestors(path, ancestors);
            endNode(DIR_NODE_NAME);
        }
    }


    /**
     * Adds a single node to the generated document. If the path is a
     * directory, and depth is greater than zero, then recursive calls
     * are made to add nodes for the directory's children.
     *
     * @param   path
     *      the file/directory to process
     * @param   depth
     *      how deep to scan the directory
     *
     * @throws  SAXException
     *      if an error occurs while constructing nodes
     */
    protected void addPath(File path, int depth)
    throws SAXException {
        if (path.isDirectory()) {
            startNode(DIR_NODE_NAME, path);
            if (depth>0) {
                File contents[] = path.listFiles();
                for (int i=0; i<contents.length; i++) {
                    if (isIncluded(contents[i]) && !isExcluded(contents[i])) {
                        addPath(contents[i], depth-1);
                    }
                }
            }
            endNode(DIR_NODE_NAME);
        } else {
            if (isIncluded(path) && !isExcluded(path)) {
                startNode(FILE_NODE_NAME, path);
                endNode(FILE_NODE_NAME);
            }
        }
    }


    /**
     * Begins a named node, and calls setNodeAttributes to set its
     * attributes.
     *
     * @param   nodeName
     *      the name of the new node
     * @param   path
     *      the file/directory to use when setting attributes
     *
     * @throws  SAXException
     *      if an error occurs while creating the node
     */
    protected void startNode(String nodeName, File path)
    throws SAXException {
        setNodeAttributes(path);
        super.contentHandler.startElement(URI, nodeName, URI+':'+nodeName, attributes);
    }


    /**
     * Sets the attributes for a given path. The default method sets attributes
     * for the name of thefile/directory and for the last modification time
     * of the path.
     *
     * @param path
     *        the file/directory to use when setting attributes
     *
     * @throws SAXException
     *         if an error occurs while setting the attributes
     */
    protected void setNodeAttributes(File path) throws SAXException {
        long lastModified = path.lastModified();
        attributes.clear();
        attributes.addAttribute("", FILENAME_ATTR_NAME,
                    FILENAME_ATTR_NAME, "CDATA",
                    path.getName());
        attributes.addAttribute("", LASTMOD_ATTR_NAME,
                    LASTMOD_ATTR_NAME, "CDATA",
                    Long.toString(path.lastModified()));
        attributes.addAttribute("", DATE_ATTR_NAME,
                    DATE_ATTR_NAME, "CDATA",
                    dateFormatter.format(new Date(lastModified)));

        if (this.isRequestedDirectory) {
            attributes.addAttribute("", "requested", "requested", "CDATA",
                    "true");
            this.isRequestedDirectory = false;
        }
    }


    /**
     * Ends the named node.
     *
     * @param   nodeName
     *      the name of the new node
     * @param   path
     *      the file/directory to use when setting attributes
     *
     * @throws  SAXException
     *      if an error occurs while closing the node
     */
    protected void endNode(String nodeName)
    throws SAXException {
        super.contentHandler.endElement(URI, nodeName, URI+':'+nodeName);
    }


    /**
     * Determines if a given File is the defined root.
     *
     * @param path the File to check
     *
     * @return true if the File is the root or the root pattern is not set,
     *      false otherwise.
     */
    protected boolean isRoot(File path) {

        return (this.rootRE == null)
                ? true
                : this.rootRE.match(path.getName());
    }


    /**
     * Determines if a given File shall be visible.
     *
     * @param path the File to check
     *
     * @return true if the File shall be visible or the include Pattern is
            <code>null</code>, false otherwise.
     */
    protected boolean isIncluded(File path) {

        return (this.includeRE == null)
                ? true
                : this.includeRE.match(path.getName());
    }


    /**
     * Determines if a given File shall be excluded from viewing.
     *
     * @param path the File to check
     *
     * @return false if the given File shall not be excluded or the
     * exclude Pattern is <code>null</code>, true otherwise.
     */
    protected boolean isExcluded(File path) {

        return (this.excludeRE == null)
                ? false
                : this.excludeRE.match(path.getName());
    }
}
