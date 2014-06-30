/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *         Copyright (c) 1999, 2000  The Apache Software Foundation.         *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "Tomcat",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

package org.apache.tester;


import java.io.*;
import java.security.Principal;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Tester request wrapper that logs all calls to the configured logger,
 * before passing them on to the underlying request.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2000/12/23 04:06:46 $
 */

public class TesterHttpServletRequestWrapper
    extends HttpServletRequestWrapper {


    // ------------------------------------------------------------ Constructor


    /**
     * Configure a new request wrapper.
     *
     * @param request The request we are wrapping
     */
    public TesterHttpServletRequestWrapper(HttpServletRequest request) {

        super(request);

    }


    // --------------------------------------------------------- Public Methods


    // For each public method, log the call and pass it to the wrapped response


    public Object getAttribute(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getAttribute()");
        return (getRequest().getAttribute(name));
    }


    public Enumeration getAttributeNames() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getAttributeNames()");
        return (getRequest().getAttributeNames());
    }


    public String getAuthType() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getAuthType()");
        return (((HttpServletRequest) getRequest()).getAuthType());
    }


    public String getCharacterEncoding() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getCharacterEncoding()");
        return (getRequest().getCharacterEncoding());
    }


    public int getContentLength() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getContentLength()");
        return (getRequest().getContentLength());
    }


    public String getContentType() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getContentType()");
        return (getRequest().getContentType());
    }


    public String getContextPath() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getContextPath()");
        return (((HttpServletRequest) getRequest()).getContextPath());
    }


    public Cookie[] getCookies() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getCookies()");
        return (((HttpServletRequest) getRequest()).getCookies());
    }


    public long getDateHeader(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getDateHeader()");
        return (((HttpServletRequest) getRequest()).getDateHeader(name));
    }


    public String getHeader(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getHeader()");
        return (((HttpServletRequest) getRequest()).getHeader(name));
    }


    public Enumeration getHeaders(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getHeaders()");
        return (((HttpServletRequest) getRequest()).getHeaders(name));
    }


    public Enumeration getHeaderNames() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getHeaderNames()");
        return (((HttpServletRequest) getRequest()).getHeaderNames());
    }


    public ServletInputStream getInputStream() throws IOException {
        StaticLogger.write("TesterHttpServletRequestWrapper.getInputStream()");
        return (getRequest().getInputStream());
    }


    public int getIntHeader(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getIntHeader()");
        return (((HttpServletRequest) getRequest()).getIntHeader(name));
    }


    public Locale getLocale() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getLocale()");
        return (getRequest().getLocale());
    }


    public Enumeration getLocales() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getLocales()");
        return (getRequest().getLocales());
    }


    public String getMethod() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getMethod()");
        return (((HttpServletRequest) getRequest()).getMethod());
    }


    public String getParameter(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getParameter()");
        return (getRequest().getParameter(name));
    }


    public Map getParameterMap() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getParameterMap()");
        return (getRequest().getParameterMap());
    }


    public Enumeration getParameterNames() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getParameterNames()");
        return (getRequest().getParameterNames());
    }


    public String[] getParameterValues(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getParameterValues()");
        return (getRequest().getParameterValues(name));
    }


    public String getPathInfo() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getPathInfo()");
        return (((HttpServletRequest) getRequest()).getPathInfo());
    }


    public String getPathTranslated() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getPathTranslated()");
        return (((HttpServletRequest) getRequest()).getPathTranslated());
    }


    public String getProtocol() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getProtocol()");
        return (getRequest().getProtocol());
    }


    public String getQueryString() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getQueryString()");
        return (((HttpServletRequest) getRequest()).getQueryString());
    }


    public BufferedReader getReader() throws IOException {
        StaticLogger.write("TesterHttpServletRequestWrapper.getReader()");
        return (getRequest().getReader());
    }


    public String getRealPath(String path) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRealPath()");
        return (getRequest().getRealPath(path));
    }


    public String getRemoteAddr() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRemoteAddr()");
        return (getRequest().getRemoteAddr());
    }


    public String getRemoteHost() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRemoteHost()");
        return (getRequest().getRemoteHost());
    }


    public String getRemoteUser() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRemoteUser()");
        return (((HttpServletRequest) getRequest()).getRemoteUser());
    }


    public RequestDispatcher getRequestDispatcher(String path) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRequestDispatcher()");
        return (getRequest().getRequestDispatcher(path));
    }


    public String getRequestURI() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRequestURI()");
        return (((HttpServletRequest) getRequest()).getRequestURI());
    }


    public StringBuffer getRequestURL() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRequestURL()");
        return (((HttpServletRequest) getRequest()).getRequestURL());
    }


    public String getRequestedSessionId() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getRequestedSessionId()");
        return (((HttpServletRequest) getRequest()).getRequestedSessionId());
    }


    public String getScheme() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getScheme()");
        return (getRequest().getScheme());
    }


    public String getServerName() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getServerName()");
        return (getRequest().getServerName());
    }


    public int getServerPort() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getServerPort()");
        return (getRequest().getServerPort());
    }


    public String getServletPath() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getServletPath()");
        return (((HttpServletRequest) getRequest()).getServletPath());
    }


    public HttpSession getSession() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getSession()");
        return (((HttpServletRequest) getRequest()).getSession());
    }


    public HttpSession getSession(boolean create) {
        StaticLogger.write("TesterHttpServletRequestWrapper.getSession(b)");
        return (((HttpServletRequest) getRequest()).getSession(create));
    }


    public Principal getUserPrincipal() {
        StaticLogger.write("TesterHttpServletRequestWrapper.getUserPrincipal()");
        return (((HttpServletRequest) getRequest()).getUserPrincipal());
    }


    public boolean isRequestedSessionIdFromCookie() {
        StaticLogger.write("TesterHttpServletRequestWrapper.isRequestedSessionIdFromCookie()");
        return (((HttpServletRequest) getRequest()).isRequestedSessionIdFromCookie());
    }


    public boolean isRequestedSessionIdFromUrl() {
        StaticLogger.write("TesterHttpServletRequestWrapper.isRequestedSessionIdFromUrl()");
        return (((HttpServletRequest) getRequest()).isRequestedSessionIdFromUrl());
    }


    public boolean isRequestedSessionIdFromURL() {
        StaticLogger.write("TesterHttpServletRequestWrapper.isRequestedSessionIdFromURL()");
        return (((HttpServletRequest) getRequest()).isRequestedSessionIdFromURL());
    }


    public boolean isRequestedSessionIdValid() {
        StaticLogger.write("TesterHttpServletRequestWrapper.isRequestedSessionIdValid()");
        return (((HttpServletRequest) getRequest()).isRequestedSessionIdValid());
    }


    public boolean isSecure() {
        StaticLogger.write("TesterHttpServletRequestWrapper.isSecure()");
        return (getRequest().isSecure());
    }


    public boolean isUserInRole(String role) {
        StaticLogger.write("TesterHttpServletRequestWrapper.isUserInRole()");
        return (((HttpServletRequest) getRequest()).isUserInRole(role));
    }


    public void removeAttribute(String name) {
        StaticLogger.write("TesterHttpServletRequestWrapper.removeAttribute()");
        getRequest().removeAttribute(name);
    }


    public void setAttribute(String name, Object value) {
        StaticLogger.write("TesterHttpServletRequestWrapper.setAttribute()");
        getRequest().setAttribute(name, value);
    }


    public void setCharacterEncoding(String enc)
        throws UnsupportedEncodingException {
        StaticLogger.write("TesterHttpServletRequestWrapper.setCharacterEncoding()");
        getRequest().setCharacterEncoding(enc);
    }





}
