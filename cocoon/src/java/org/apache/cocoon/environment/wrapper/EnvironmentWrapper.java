/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.wrapper;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.AbstractEnvironment;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.log.Logger;

import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;


/**
 * This is a wrapper class for the <code>Environment</code> object.
 * It has the same properties except that the object model
 * contains a <code>RequestWrapper</code> object.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version $Id: EnvironmentWrapper.java,v 1.3 2002/01/22 00:17:13 vgritsenko Exp $
 */
public final class EnvironmentWrapper
extends AbstractEnvironment
implements Environment {

    /** The wrapped environment */
    private Environment environment;

    /** The object model */
    private Map objectModel;

    /** The redirect url */
    private String redirectURL;

    /** The request object */
    private Request request;

    /** The last context */
    private URL lastContext;
    /** The last prefix */
    private String lastPrefix;
    /** The last uri */
    private String lastURI;

    /**
     * Constructs an EnvironmentWrapper object from a Request
     * and Response objects
     */
    public EnvironmentWrapper(Environment env,
                              String      requestURI,
                              String      queryString,
                              Logger      logger)
    throws MalformedURLException {
        super(env.getURI(), env.getView(), env.getRootContext(), env.getAction());
        this.setLogger(logger);
        this.environment = env;

        this.context = env.getContext();
        this.prefix = new StringBuffer(env.getURIPrefix());

        this.setSourceHandler(env.getSourceHandler());
        // create new object model and replace the request object
        this.objectModel = new HashMap(4);
        Map oldObjectModel = env.getObjectModel();
        Iterator keys = oldObjectModel.keySet().iterator();
        String key;
        while (keys.hasNext()) {
            key = (String)keys.next();
            this.objectModel.put(key, oldObjectModel.get(key));
        }
        this.request = new RequestWrapper((Request)oldObjectModel.get(Constants.REQUEST_OBJECT),
                                                 requestURI, queryString, this);
        this.objectModel.put(Constants.REQUEST_OBJECT, this.request);
        this.objectModel.put("Internal-Request", "true");
    }

    /**
     * Redirect the client to a new URL is not allowed
     */
    public void redirect(boolean sessionmode, String newURL)
    throws IOException {
        this.redirectURL = newURL;

        // check if session mode shall be activated
        if (sessionmode) {
            // The session
            Session session = null;
            // get session from request, or create new session
            session = request.getSession(true);
        }
    }

    /**
     * Redirect in the first non-wrapped environment
     */
    public void globalRedirect(boolean sessionmode, String newURL)
    throws IOException {
       if (environment instanceof EnvironmentWrapper) {
         ((EnvironmentWrapper)environment).globalRedirect(sessionmode, newURL);
       }
       else {
         environment.redirect(sessionmode,newURL);
       }
    }

    /**
     * Get the output stream
     */
    public OutputStream getOutputStream()
    throws IOException {
        return this.environment.getOutputStream();
    }

    /**
     * if a redirect should happen this returns the url,
     * otherwise <code>null</code> is returned
     */
    public String getRedirectURL() {
        return this.redirectURL;
    }

    public void reset() {
        this.redirectURL = null;
    }

    /**
     * Set the StatusCode
     */
    public void setStatus(int statusCode) {
        // ignore this
    }

    public void setContentLength(int length) {
        // ignore this
    }

    /**
     * Set the ContentType
     */
    public void setContentType(String contentType) {
        // ignore this
    }

    /**
     * Get the ContentType
     */
    public String getContentType() {
        // ignore this
        return null;
    }

    /**
     * Get the underlying object model
     */
    public Map getObjectModel() {
        return this.objectModel;
    }

    /**
     * Set a new URI for processing. If the prefix is null the
     * new URI is inside the current context.
     * If the prefix is not null the context is changed to the root
     * context and the prefix is set.
     */
    public void setURI(String prefix, String uris) {
        getLogger().debug("Setting uri (prefix=" + prefix + ", uris=" + uris + ")");
        if (prefix != null) {
            setContext(getRootContext());
            setURIPrefix(prefix);
        }
        this.uris = uris;
        this.lastURI = uris;
        this.lastContext = this.context;
        this.lastPrefix = this.prefix.toString();
    }

    public void changeContext(String prefix, String context)
    throws MalformedURLException {
        super.changeContext(prefix, context);
        this.lastContext = this.context;
        this.lastPrefix  = this.prefix.toString();
        this.lastURI     = this.uris;
    }

    /**
     * Change the current context to the last one set by changeContext()
     */
    public void changeToLastContext() {
        this.setContext(this.lastContext);
        this.setURIPrefix(this.lastPrefix);
        this.uris = this.lastURI;
    }
}


