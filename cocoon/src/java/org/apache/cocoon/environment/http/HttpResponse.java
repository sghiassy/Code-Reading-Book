/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.environment.http;

import org.apache.cocoon.environment.Cookie;
import org.apache.cocoon.environment.Response;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Locale;

/**
 *
 * Implements the {@link HttpServletResponse} interface to provide HTTP-specific
 * functionality in sending a response.  For example, it has methods
 * to access HTTP headers and cookies.
 */

public class HttpResponse implements Response {

    /** The real HttpServletResponse object */
    private HttpServletResponse res = null;

    /**
     * Creates a HttpServletResponse based on a real HttpServletResponse object
     */
    protected HttpResponse (HttpServletResponse res) {
        super ();
        this.res = res;
    }

    /**
     * Create a new cookie which is not added to the response
     */
    public Cookie createCookie(String name, String value) {
        return new HttpCookie(name, value);
    }

    public void addCookie(Cookie cookie) {
        if (cookie instanceof HttpCookie) {
            this.res.addCookie(((HttpCookie)cookie).getServletCookie());
        } else {
            javax.servlet.http.Cookie newCookie;
            newCookie = new javax.servlet.http.Cookie(cookie.getName(), cookie.getValue());
            newCookie.setComment(cookie.getComment());
            newCookie.setDomain(cookie.getDomain());
            newCookie.setMaxAge(cookie.getMaxAge());
            newCookie.setPath(cookie.getPath());
            newCookie.setSecure(cookie.getSecure());
            newCookie.setVersion(cookie.getVersion());
            this.res.addCookie(newCookie);
        }
    }

    public boolean containsHeader(String name) {
        return this.res.containsHeader(name);
    }

    public String encodeURL(String url) {
        if (url != null && url.indexOf(";jsessionid=") != -1)
            return url;
        return this.res.encodeURL(url);
    }

    public String encodeRedirectURL(String url) {
        if (url != null && url.indexOf(";jsessionid=") != -1)
            return url;
        return this.res.encodeRedirectURL(url);
    }

    public void sendError(int sc, String msg) throws IOException {
        this.res.sendError(sc, msg);
    }

    public void sendError(int sc) throws IOException {
        this.res.sendError(sc);
    }

    public void sendRedirect(String location) throws IOException {
        this.res.sendRedirect(location);
    }

    public void setDateHeader(String name, long date) {
        this.res.setDateHeader(name, date);
    }

    public void addDateHeader(String name, long date) {
        this.res.addDateHeader(name, date);
    }

    public void setHeader(String name, String value) {
        this.res.setHeader(name, value);
    }

    public void addHeader(String name, String value) {
        this.res.addHeader(name, value);
    }

    public void setIntHeader(String name, int value) {
        this.res.setIntHeader(name, value);
    }

    public void addIntHeader(String name, int value) {
        this.res.addIntHeader(name, value);
    }

    public void setStatus(int sc) {
        this.res.setStatus(sc);
    }

    /**
     * @deprecated        As of version 2.1, use encodeURL(String url) instead
     */
    public String encodeUrl(String url) {
        return this.res.encodeUrl(url);
    }

    /**
     * @deprecated        As of version 2.1, use
     *              encodeRedirectURL(String url) instead
     */
    public String encodeRedirectUrl(String url) {
        return this.res.encodeRedirectUrl(url);
    }

    /**
     * @deprecated As of version 2.1, due to ambiguous meaning of the
     * message parameter. To set a status code
     * use <code>setStatus(int)</code>, to send an error with a description
     * use <code>sendError(int, String)</code>.
     */
    public void setStatus(int sc, String sm) {
        this.res.setStatus(sc, sm);
    }

    /* The ServletResponse interface methods */

    public String getCharacterEncoding() {
        return this.res.getCharacterEncoding();
    }

    public ServletOutputStream getOutputStream() throws IOException {
        //throw new IllegalStateException ("you are not a serializer or reader");
        return this.res.getOutputStream();
    }

    public PrintWriter getWriter() throws IOException {
        //throw new IllegalStateException ("you are not a serializer or reader");
        return this.res.getWriter();
    }

    public void setContentLength(int len) {
        this.res.setContentLength(len);
    }

    public void setContentType(String type) {
        this.res.setContentType(type);
    }

    public void setBufferSize(int size) {
        this.res.setBufferSize(size);
    }

    public int getBufferSize() {
        return this.res.getBufferSize();
    }

    public void flushBuffer() throws IOException {
        this.res.flushBuffer();
    }

    public boolean isCommitted() {
        return this.res.isCommitted();
    }

    public void reset() {
        this.res.reset();
    }

    public void setLocale(Locale loc) {
        this.res.setLocale(loc);
    }

    public Locale getLocale() {
        return this.res.getLocale();
    }
}

