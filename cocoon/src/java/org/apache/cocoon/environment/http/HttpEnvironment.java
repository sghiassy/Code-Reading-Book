/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.http;

import org.apache.cocoon.Constants;
import org.apache.cocoon.environment.AbstractEnvironment;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.Session;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;

public class HttpEnvironment extends AbstractEnvironment implements Redirector {

    public static final String HTTP_REQUEST_OBJECT = "httprequest";
    public static final String HTTP_RESPONSE_OBJECT= "httpresponse";
    public static final String HTTP_SERVLET_CONTEXT= "httpservletcontext";

    /** The HttpRequest */
    private HttpRequest request = null;

    /** The HttpResponse */
    private HttpResponse response = null;

    /** The HttpContext */
    private HttpContext webcontext = null;

    /** The OutputStream */
    private OutputStream outputStream = null;

    /** Cache content type as there is no getContentType() in reponse object */
    private String contentType = null;

    /**
     * Constructs a HttpEnvironment object from a HttpServletRequest
     * and HttpServletResponse objects
     */
    public HttpEnvironment (String uri,
                            URL rootURL,
                            HttpServletRequest req,
                            HttpServletResponse res,
                            ServletContext servletContext,
                            HttpContext context)
    throws MalformedURLException, IOException {
        super(uri, req.getParameter(Constants.VIEW_PARAM), rootURL, req.getParameter(Constants.ACTION_PARAM));

        this.request = new HttpRequest (req, this);
        this.response = new HttpResponse (res);
        this.webcontext = context;
        this.outputStream = response.getOutputStream();
        this.objectModel.put(Constants.REQUEST_OBJECT, this.request);
        this.objectModel.put(Constants.RESPONSE_OBJECT, this.response);
        this.objectModel.put(Constants.CONTEXT_OBJECT, this.webcontext);
        // This is a hack for the Php Generator
        this.objectModel.put(HTTP_REQUEST_OBJECT, req);
        this.objectModel.put(HTTP_RESPONSE_OBJECT, res);
        this.objectModel.put(HTTP_SERVLET_CONTEXT, servletContext);
    }

   /**
    *  Redirect the client to new URL with session mode
    */
    public void redirect(boolean sessionmode, String newURL) throws IOException {
        if (request == null) {
            getLogger().debug("redirect: something's broken, request = null");
            return;
        }
        // check if session mode shall be activated
        if (sessionmode) {
            // The session
            Session session = null;
            getLogger().debug("redirect: entering session mode");
            String s = request.getRequestedSessionId();
            if (s != null) {
                getLogger().debug("Old session ID found in request, id = " + s);
                if ( request.isRequestedSessionIdValid() ) {
                    getLogger().debug("And this old session ID is valid");
                }
            }
            // get session from request, or create new session
            session = request.getSession(true);
            if (session == null) {
                getLogger().debug("redirect session mode: unable to get session object!");
            }
            getLogger().debug ("redirect: session mode completed, id = " + session.getId() );
        }
        // redirect
        String redirect = this.response.encodeRedirectURL(newURL);

        getLogger().debug("Sending redirect to '" + redirect + "'");
        this.response.sendRedirect (redirect);
    }

    /**
     * Set the StatusCode
     */
    public void setStatus(int statusCode) {
        this.response.setStatus(statusCode);
    }

    /**
     * Set the ContentType
     */
    public void setContentType(String contentType) {
        this.response.setContentType(contentType);
        this.contentType = contentType;
    }

    /**
     * Get the ContentType
     */
    public String getContentType() {
        return this.contentType;
    }

    /**
     * Set the length of the generated content
     */
    public void setContentLength(int length) {
        this.response.setContentLength(length);
    }

    /**
     * Get the OutputStream
     */
    public OutputStream getOutputStream() throws IOException {
        return this.outputStream;
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
        long if_modified_since = this.request.getDateHeader("If-Modified-Since");

        this.response.setDateHeader("Last-Modified", lastModified);
        return (if_modified_since < lastModified);
    }

    /**
     * Mark the response as not modified.
     */
    public void setResponseIsNotModified() {
        this.response.setStatus(HttpServletResponse.SC_NOT_MODIFIED);
    }

    /**
     * Reset the response if possible. This allows error handlers to have
     * a higher chance to produce clean output if the pipeline that raised
     * the error has already output some data.
     *
     * @return true if the response was successfully reset
    */
    public boolean tryResetResponse() {
        try {
            if ( !this.response.isCommitted() ) {
                this.response.reset();
                getLogger().debug("Response successfully reset");
                return true;
            }
        } catch(Exception e) {
            // Log the error, but don't transmit it
            getLogger().warn("Problem resetting response", e);
        }
        getLogger().debug("Response wasn't reset");
        return false;
    }

}
