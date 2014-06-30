/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.environment.wrapper;

import org.apache.cocoon.environment.Cookie;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;

import java.security.Principal;
import java.util.*;


/**
 * This is a wrapper class for the <code>Request</code> object.
 * It has the same properties except that the url and the parameters
 * are different.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version $Id: RequestWrapper.java,v 1.3 2002/01/22 00:17:13 vgritsenko Exp $
 */
public final class RequestWrapper implements Request {

    /** The real Request object */
    private Request req;

    /** The query string */
    private String queryString;

    /** The request parameters */
    private RequestParameters parameters ;

    /** The environment */
    private Environment environment;

    /**
     * Constructor
     */
    public RequestWrapper(Request request,
                          String  requestURI,
                          String  queryString,
                          Environment env) {
        this.environment = env;
        this.req = request;
        this.queryString = queryString;
        this.parameters = new RequestParameters(queryString);
        if (this.req.getQueryString() != null) {
            if (this.queryString == null)
                this.queryString = this.req.getQueryString();
            else
                this.queryString += '&' + this.req.getQueryString();
        }
    }

    public Object get(String name) {
        return this.req.get(name);
    }

    public Object getAttribute(String name) {
        return this.req.getAttribute(name);
    }

    public Enumeration getAttributeNames() {
        return this.req.getAttributeNames();
    }

    public String getCharacterEncoding() {
        return this.req.getCharacterEncoding();
    }

    public int getContentLength() {
        return this.req.getContentLength();
    }

    public String getContentType() {
        return this.req.getContentType();
    }

    public String getParameter(String name) {
        String value = this.parameters.getParameter(name);
        if (value == null)
            return this.req.getParameter(name);
        else
            return value;
    }

    public Enumeration getParameterNames() {
        // put all parameter names into a set
        Set parameterNames = new HashSet();
        Enumeration names = this.parameters.getParameterNames();
        while (names.hasMoreElements()) {
            parameterNames.add(names.nextElement());
        }
        names = this.req.getParameterNames();
        while (names.hasMoreElements()) {
            parameterNames.add(names.nextElement());
        }
        return new EnumerationFromIterator(parameterNames.iterator());
    }

    final class EnumerationFromIterator implements Enumeration {
        private Iterator iter;
        EnumerationFromIterator(Iterator iter) {
            this.iter = iter;
        }

        public boolean hasMoreElements() {
            return iter.hasNext();
        }
        public Object nextElement() { return iter.next(); }
    }

    public String[] getParameterValues(String name) {
        String[] values = this.parameters.getParameterValues(name);
        String[] inherited = this.req.getParameterValues(name);
        if (inherited == null) return values;
        if (values == null) return inherited;
        String[] allValues = new String[values.length + inherited.length];
        System.arraycopy(values, 0, allValues, 0, values.length);
        System.arraycopy(inherited, 0, allValues, values.length, inherited.length);
        return allValues;
    }

    public String getProtocol() {
        return this.req.getProtocol();
    }

    public String getScheme() {
        return this.req.getScheme();
    }

    public String getServerName() {
        return this.req.getServerName();
    }

    public int getServerPort() {
        return this.req.getServerPort();
    }

    public String getRemoteAddr() {
        return this.req.getRemoteAddr();
    }

    public String getRemoteHost() {
        return this.req.getRemoteHost();
    }

    public void setAttribute(String name, Object o) {
        this.req.setAttribute(name, o);
    }

    /**
     * Remove one attriube
     */
    public void removeAttribute(String name) {
        this.req.removeAttribute(name);
    }

    public Locale getLocale() {
        return this.req.getLocale();
    }

    public Enumeration getLocales() {
        return this.req.getLocales();
    }

    public boolean isSecure() {
        return this.req.isSecure();
    }

    public Cookie[] getCookies() {
        return this.req.getCookies();
    }

    public Map getCookieMap() {
        return this.req.getCookieMap();
    }

    public long getDateHeader(String name) {
        return this.req.getDateHeader(name);
    }

    public String getHeader(String name) {
        return this.req.getHeader(name);
    }

    public Enumeration getHeaders(String name) {
        return this.req.getHeaders(name);
    }

    public Enumeration getHeaderNames() {
        return this.req.getHeaderNames();
    }

    public String getMethod() {
        return this.req.getMethod();
    }

    public String getPathInfo() {
        return this.req.getPathInfo();
    }

    public String getPathTranslated() {
        return this.req.getPathTranslated();
    }

    public String getContextPath() {
        return this.req.getContextPath();
    }

    public String getQueryString() {
        return this.queryString;
    }

    public String getRemoteUser() {
        return this.req.getRemoteUser();
    }

    public String getRequestedSessionId() {
        return this.req.getRequestedSessionId();
    }

    public String getRequestURI() {
        return this.req.getRequestURI();
    }

    public String getSitemapURI() {
        return this.environment.getURI();
    }

    public String getServletPath() {
        return this.req.getServletPath();
    }

    public Session getSession(boolean create) {
        return this.req.getSession(create);
    }

    public Session getSession() {
        return this.req.getSession();
    }

    public boolean isRequestedSessionIdValid() {
        return this.req.isRequestedSessionIdValid();
    }

    public boolean isRequestedSessionIdFromCookie()  {
        return this.req.isRequestedSessionIdFromCookie();
    }

    public boolean isRequestedSessionIdFromURL() {
        return this.req.isRequestedSessionIdFromURL();
    }

    public boolean isRequestedSessionIdFromUrl() {
        return this.req.isRequestedSessionIdFromURL();
    }

    public Principal getUserPrincipal() {
        return this.req.getUserPrincipal();
    }

    public boolean isUserInRole(String role) {
        return this.req.isUserInRole(role);
    }

    public String getAuthType() {
        return this.req.getAuthType();
    }

}
