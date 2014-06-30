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
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Tester request wrapper that logs all calls to the configured logger,
 * before passing them on to the underlying request.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2000/12/22 22:13:07 $
 */

public class TesterServletRequestWrapper extends ServletRequestWrapper {


    // ------------------------------------------------------------ Constructor


    /**
     * Configure a new request wrapper.
     *
     * @param request The request we are wrapping
     */
    public TesterServletRequestWrapper(ServletRequest request) {

        super(request);

    }


    // --------------------------------------------------------- Public Methods


    // For each public method, log the call and pass it to the wrapped response


    public Object getAttribute(String name) {
        StaticLogger.write("TesterServletRequestWrapper.getAttribute()");
        return (getRequest().getAttribute(name));
    }


    public Enumeration getAttributeNames() {
        StaticLogger.write("TesterServletRequestWrapper.getAttributeNames()");
        return (getRequest().getAttributeNames());
    }


    public String getCharacterEncoding() {
        StaticLogger.write("TesterServletRequestWrapper.getCharacterEncoding()");
        return (getRequest().getCharacterEncoding());
    }


    public int getContentLength() {
        StaticLogger.write("TesterServletRequestWrapper.getContentLength()");
        return (getRequest().getContentLength());
    }


    public String getContentType() {
        StaticLogger.write("TesterServletRequestWrapper.getContentType()");
        return (getRequest().getContentType());
    }


    public ServletInputStream getInputStream() throws IOException {
        StaticLogger.write("TesterServletRequestWrapper.getInputStream()");
        return (getRequest().getInputStream());
    }


    public Locale getLocale() {
        StaticLogger.write("TesterServletRequestWrapper.getLocale()");
        return (getRequest().getLocale());
    }


    public Enumeration getLocales() {
        StaticLogger.write("TesterServletRequestWrapper.getLocales()");
        return (getRequest().getLocales());
    }


    public String getParameter(String name) {
        StaticLogger.write("TesterServletRequestWrapper.getParameter()");
        return (getRequest().getParameter(name));
    }


    public Map getParameterMap() {
        StaticLogger.write("TesterServletRequestWrapper.getParameterMap()");
        return (getRequest().getParameterMap());
    }


    public Enumeration getParameterNames() {
        StaticLogger.write("TesterServletRequestWrapper.getParameterNames()");
        return (getRequest().getParameterNames());
    }


    public String[] getParameterValues(String name) {
        StaticLogger.write("TesterServletRequestWrapper.getParameterValues()");
        return (getRequest().getParameterValues(name));
    }


    public String getProtocol() {
        StaticLogger.write("TesterServletRequestWrapper.getProtocol()");
        return (getRequest().getProtocol());
    }


    public BufferedReader getReader() throws IOException {
        StaticLogger.write("TesterServletRequestWrapper.getReader()");
        return (getRequest().getReader());
    }


    public String getRealPath(String path) {
        StaticLogger.write("TesterServletRequestWrapper.getRealPath()");
        return (getRequest().getRealPath(path));
    }


    public String getRemoteAddr() {
        StaticLogger.write("TesterServletRequestWrapper.getRemoteAddr()");
        return (getRequest().getRemoteAddr());
    }


    public String getRemoteHost() {
        StaticLogger.write("TesterServletRequestWrapper.getRemoteHost()");
        return (getRequest().getRemoteHost());
    }


    public RequestDispatcher getRequestDispatcher(String path) {
        StaticLogger.write("TesterServletRequestWrapper.getRequestDispatcher()");
        return (getRequest().getRequestDispatcher(path));
    }


    public String getScheme() {
        StaticLogger.write("TesterServletRequestWrapper.getScheme()");
        return (getRequest().getScheme());
    }


    public String getServerName() {
        StaticLogger.write("TesterServletRequestWrapper.getServerName()");
        return (getRequest().getServerName());
    }


    public int getServerPort() {
        StaticLogger.write("TesterServletRequestWrapper.getServerPort()");
        return (getRequest().getServerPort());
    }


    public boolean isSecure() {
        StaticLogger.write("TesterServletRequestWrapper.isSecure()");
        return (getRequest().isSecure());
    }


    public void removeAttribute(String name) {
        StaticLogger.write("TesterServletRequestWrapper.removeAttribute()");
        getRequest().removeAttribute(name);
    }


    public void setAttribute(String name, Object value) {
        StaticLogger.write("TesterServletRequestWrapper.setAttribute()");
        getRequest().setAttribute(name, value);
    }


    public void setCharacterEncoding(String enc)
        throws UnsupportedEncodingException {
        StaticLogger.write("TesterServletRequestWrapper.setCharacterEncoding()");
        getRequest().setCharacterEncoding(enc);
    }





}
