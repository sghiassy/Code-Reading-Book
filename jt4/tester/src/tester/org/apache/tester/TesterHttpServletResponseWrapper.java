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
 * Tester response wrapper that logs all calls to the configured logger,
 * before passing them on to the underlying response.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2000/12/22 22:13:07 $
 */

public class TesterHttpServletResponseWrapper
    extends HttpServletResponseWrapper {


    // ------------------------------------------------------------ Constructor


    /**
     * Configure a new response wrapper.
     *
     * @param response The response we are wrapping
     */
    public TesterHttpServletResponseWrapper(HttpServletResponse response) {

        super(response);

    }


    // --------------------------------------------------------- Public Methods


    // For each public method, log the call and pass it to the wrapped response


    public void addCookie(Cookie cookie) {
        StaticLogger.write("TesterHttpServletResponseWrapper.addCookie()");
        ((HttpServletResponse) getResponse()).addCookie(cookie);
    }


    public void addDateHeader(String name, long value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.addDateHeader()");
        ((HttpServletResponse) getResponse()).addDateHeader(name, value);
    }


    public void addHeader(String name, String value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.addHeader()");
        ((HttpServletResponse) getResponse()).addHeader(name, value);
    }


    public void addIntHeader(String name, int value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.addIntHeader()");
        ((HttpServletResponse) getResponse()).addIntHeader(name, value);
    }


    public boolean containsHeader(String name) {
        StaticLogger.write("TesterHttpServletResponseWrapper.containsHeader()");
        return (((HttpServletResponse) getResponse()).containsHeader(name));
    }


    public String encodeURL(String url) {
        StaticLogger.write("TesterHttpServletResponseWrapper.encodeURL()");
        return (((HttpServletResponse) getResponse()).encodeURL(url));
    }


    public String encodeUrl(String url) {
        StaticLogger.write("TesterHttpServletResponseWrapper.encodeUrl()");
        return (((HttpServletResponse) getResponse()).encodeUrl(url));
    }


    public String encodeRedirectURL(String url) {
        StaticLogger.write("TesterHttpServletResponseWrapper.encodeRedirectURL()");
        return (((HttpServletResponse) getResponse()).encodeRedirectURL(url));
    }


    public String encodeRedirectUrl(String url) {
        StaticLogger.write("TesterHttpServletResponseWrapper.encodeRedirectUrl()");
        return (((HttpServletResponse) getResponse()).encodeRedirectUrl(url));
    }


    public void flushBuffer() throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.flushBuffer()");
        getResponse().flushBuffer();
    }


    public int getBufferSize() {
        StaticLogger.write("TesterHttpServletResponseWrapper.getBufferSize()");
        return (getResponse().getBufferSize());
    }


    public String getCharacterEncoding() {
        StaticLogger.write("TesterHttpServletResponseWrapper.getCharacterEncoding()");
        return (getResponse().getCharacterEncoding());
    }


    public Locale getLocale() {
        StaticLogger.write("TesterHttpServletResponseWrapper.getLocale()");
        return (getResponse().getLocale());
    }


    public ServletOutputStream getOutputStream() throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.getOutputStream()");
        return (getResponse().getOutputStream());
    }


    public PrintWriter getWriter() throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.getWriter()");
        return (getResponse().getWriter());
    }


    public boolean isCommitted() {
        StaticLogger.write("TesterHttpServletResponseWrapper.isCommitted()");
        return (getResponse().isCommitted());
    }


    public void reset() {
        StaticLogger.write("TesterHttpServletResponseWrapper.reset()");
        getResponse().reset();
    }


    public void resetBuffer() {
        StaticLogger.write("TesterHttpServletResponseWrapper.resetBuffer()");
        getResponse().resetBuffer();
    }


    public void sendError(int sc) throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.sendError(i)");
        ((HttpServletResponse) getResponse()).sendError(sc);
    }


    public void sendError(int sc, String msg) throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.sendError(i,s)");
        ((HttpServletResponse) getResponse()).sendError(sc, msg);
    }


    public void sendRedirect(String location) throws IOException {
        StaticLogger.write("TesterHttpServletResponseWrapper.sendRedirect()");
        ((HttpServletResponse) getResponse()).sendRedirect(location);
    }


    public void setBufferSize(int size) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setBufferSize()");
        getResponse().setBufferSize(size);
    }


    public void setContentLength(int len) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setContentLength()");
        getResponse().setContentLength(len);
    }


    public void setContentType(String type) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setContentType()");
        getResponse().setContentType(type);
    }


    public void setDateHeader(String name, long value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setDateHeader()");
        ((HttpServletResponse) getResponse()).setDateHeader(name, value);
    }


    public void setHeader(String name, String value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setHeader()");
        ((HttpServletResponse) getResponse()).setHeader(name, value);
    }


    public void setIntHeader(String name, int value) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setIntHeader()");
        ((HttpServletResponse) getResponse()).setIntHeader(name, value);
    }


    public void setLocale(Locale locale) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setLocale()");
        getResponse().setLocale(locale);
    }


    public void setStatus(int sc) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setStatus(i)");
        ((HttpServletResponse) getResponse()).setStatus(sc);
    }


    public void setStatus(int sc, String msg) {
        StaticLogger.write("TesterHttpServletResponseWrapper.setStatus(i,s)");
        ((HttpServletResponse) getResponse()).setStatus(sc, msg);
    }


}
