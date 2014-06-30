/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment;

import org.apache.avalon.excalibur.collections.IteratorEnumeration;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.source.SourceHandler;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

/**
 * Base class for any environment
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/22 00:17:12 $
 */
public abstract class AbstractEnvironment extends AbstractLoggable implements Environment {

    /** The current uri in progress */
    protected String uris;

    /** The current prefix to strip off from the request uri */
    protected StringBuffer prefix = new StringBuffer();

    /** The View requested */
    protected String view = null;

    /** The Action requested */
    protected String action = null;

     /** The Context path */
    protected URL context = null;

    /** The root context path */
    protected URL rootContext = null;

    /** The servlet object model */
    protected HashMap objectModel = null;

    /** The source handler for the current environment */
    protected SourceHandler sourceHandler;

    /** The attributes */
    private Map attributes = new HashMap();

    /**
     * Constructs the abstract enviornment
     */
    public AbstractEnvironment(String uri, String view, String file)
    throws MalformedURLException {
        this(uri, view, new File(file), null);
    }

    /**
     * Constructs the abstract enviornment
     */
    public AbstractEnvironment(String uri, String view, String file, String action)
    throws MalformedURLException {
        this(uri, view, new File(file), action);
    }

    /**
     * Constructs the abstract enviornment
     */
    public AbstractEnvironment(String uri, String view, File file)
    throws MalformedURLException {
        this(uri, view, file, null);
    }

    /**
     * Constructs the abstract enviornment
     */
    public AbstractEnvironment(String uri, String view, File file, String action)
    throws MalformedURLException {
        this(uri, view, file.toURL(), action);
    }

    /**
     * Constructs the abstract enviornment
     */
    public AbstractEnvironment(String uri, String view, URL context, String action)
    throws MalformedURLException {
        this.uris = uri;
        this.view = view;
        this.context = context;
        this.action = action;
        this.objectModel = new HashMap();
        this.rootContext = context;
    }

    /**
     * Get the <code>SourceHandler</code> for the current request
     */
    public SourceHandler getSourceHandler() {
        return this.sourceHandler;
    }

    /**
     * Set the <code>SourceHandler</code> for the current request
     */
    public void setSourceHandler(SourceHandler sourceHandler) {
        this.sourceHandler = sourceHandler;
    }

    // Sitemap methods

    /**
     * Returns the uri in progress. The prefix is stripped off
     */
    public String getURI() {
        return this.uris;
    }

    /**
     * Get the Root Context
     */
    public URL getRootContext() {
        return this.rootContext;
    }

    /**
     * Get the current Context
     */
    public URL getContext() {
        return this.context;
    }

    /**
     * Get the prefix of the URI in progress
     */
    public String getURIPrefix() {
        return this.prefix.toString();
    }

    /**
     * Set the prefix of the URI in progress
     */
    protected void setURIPrefix(String prefix) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Set the URI Prefix (OLD=" + getURIPrefix() + ", NEW=" +  prefix + ")");
        }
        this.prefix = new StringBuffer(prefix);
    }

    /**
     * Set the context.
     */
    protected void setContext(URL context) {
        this.context = context;
    }

    /**
     * Set the context. This is similar to changeContext()
     * except that it is absolute.
     */
    public void setContext(String prefix, String uri) {
        this.setContext(getRootContext());
        this.setURIPrefix(prefix == null ? "" : prefix);
        this.uris = uri;
        if (getLogger().isDebugEnabled()) {
            getLogger().debug("Reset context to " + this.context);
        }
    }

    /**
     * Adds an prefix to the overall stripped off prefix from the request uri
     */
    public void changeContext(String prefix, String newContext)
    throws MalformedURLException {
        if (getLogger().isDebugEnabled())
        {
            getLogger().debug("Changing Cocoon context(" + newContext + ") to prefix(" + prefix + ")");
            getLogger().debug("\tfrom context(" + this.context.toExternalForm() + ") and prefix(" + this.prefix + ")");
            getLogger().debug("\tat URI " + this.uris);
        }
        if (prefix.length() >= 1 && !this.uris.startsWith(prefix)) {
            if (getLogger().isErrorEnabled())
            {
                this.getLogger().error("The current URI ("
                        + this.uris + ") doesn't start with given prefix ("
                        + prefix + ")"
                );
            }

            throw new RuntimeException("The current URI doesn't start with given prefix");
        }
        if (prefix.length() >= 1) {
            this.prefix.append(prefix);
            this.uris = this.uris.substring(prefix.length());

            // check for a slash at the beginning to avoid problems with subsitemaps
            if (this.uris.startsWith("/")) {
                this.uris = this.uris.substring(1);
                this.prefix.append('/');
            }
        }

        //if the resource is zipped into a war file (e.g. Weblogic temp deployment)
        if (this.context.getProtocol().equals("zip")) {
            this.context = new URL(this.context.toString() + newContext);

        // if we got a absolute context or one with a protocol resolve it
        } else {
            if (newContext.charAt(0) == '/') {
                this.context = new URL("file:" + newContext);
            } else if (newContext.indexOf(':') > 1) {
                this.context = new URL(newContext);
            } else {
                this.context = new URL(this.context, newContext);
            }
            File f = new File(this.context.getFile());
            if (f.isFile()) {
                this.context = f.getParentFile().toURL();
            } else {
                this.context = f.toURL();
            }
        }

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("New context is " + this.context.toExternalForm());
        }
    }

    /**
     * Redirect the client to a new URL
     */
    public abstract void redirect(boolean sessionmode, String newURL) throws IOException;
    public void globalRedirect(boolean sessionmode, String newURL) throws IOException {
      redirect(sessionmode,newURL);
    }

    // Request methods

    /**
     * Returns the request view
     */
    public String getView() {
        return this.view;
    }

    /**
     * Returns the request action
     */
    public String getAction() {
        return this.action;
    }

    // Response methods

    /**
     * Set a status code
     */
    public void setStatus(int statusCode) {
    }

    // Object model method

    /**
     * Returns a Map containing environment specific objects
     */
    public Map getObjectModel() {
        return this.objectModel;
    }

    /**
     * Resolve an entity.
     */
    public Source resolve(String systemId)
    throws ProcessingException, SAXException, IOException {
        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("Resolving '"+systemId+"' in context '" + this.context + "'");
        }
        if (systemId == null) throw new SAXException("Invalid System ID");

        Source source;
        if (systemId.length() == 0) {
            source = this.sourceHandler.getSource(this, this.context.toExternalForm());
        } else if (systemId.indexOf(":") > 1) {
            source = this.sourceHandler.getSource(this, systemId);
        } else if (systemId.charAt(0) == '/') {
            source = this.sourceHandler.getSource(this, this.context.getProtocol() +
                                                  ":" + systemId);
        // windows: absolute paths can start with drive letter
        } else if (systemId.length() > 1 && systemId.charAt(1) == ':') {
            source = this.sourceHandler.getSource(this, this.context.getProtocol() +
                                                  ":/" + systemId);
        } else {
            source = this.sourceHandler.getSource(this, this.context, systemId);
        }

        if (getLogger().isDebugEnabled()) {
            this.getLogger().debug("Resolved to '"+source.getSystemId()+"'");
        }
        return source;
    }

    /**
     * Check if the response has been modified since the same
     * "resource" was requested.
     * The caller has to test if it is really the same "resource"
     * which is requested.
     * @result true if the response is modified or if the
     *         environment is not able to test it
     */
    public boolean isResponseModified(long lastModified) {
        return true; // always modified
    }

    /**
     * Mark the response as not modified.
     */
    public void setResponseIsNotModified() {
        // does nothing
    }

    public Object getAttribute(String name) {
        return this.attributes.get(name);
    }

    public void setAttribute(String name, Object value) {
        this.attributes.put(name, value);
    }

    public void removeAttribute(String name) {
        this.attributes.remove(name);
    }

    public Enumeration getAttributeNames() {
        return new IteratorEnumeration(this.attributes.keySet().iterator());
    }

    /**
     * Reset the response if possible. This allows error handlers to have
     * a higher chance to produce clean output if the pipeline that raised
     * the error has already output some data.
     *
     * @return true if the response was successfully reset
    */
    public boolean tryResetResponse() {
        return false;
    }

}
