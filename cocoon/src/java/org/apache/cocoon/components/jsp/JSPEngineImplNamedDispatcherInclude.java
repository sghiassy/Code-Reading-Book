/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.jsp;

import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameterizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.xml.sax.SAXException;

import javax.servlet.*;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import java.io.*;
import java.security.Principal;
import java.util.Enumeration;
import java.util.Locale;

/**
 * Allows JSP to be used as a generator.  Builds upon the JSP servlet
 * functionality - overrides the output method and returns the byte(s).
 * This implementation includes via ServletContext.getNamedDispatcher() the
 * jsp-response. This a generic implementation.
 *
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:bh22351@i-one.at">Bernhard Huber</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/10 15:27:37 $
 */
public class JSPEngineImplNamedDispatcherInclude extends AbstractLoggable
    implements JSPEngine, Parameterizable, ThreadSafe {

    /** The Servlet Include Path */
    public static final String INC_SERVLET_PATH = "javax.servlet.include.servlet_path";

    /** config-parameter name for specifying the jsp servlet-name.
      ie. servlet-name
    */
    public static final String CONFIG_SERVLET_NAME = "servlet-name";
    /** default value of CONFIG_SERVLET_NAME.
      ie. *jsp, this is the WLS JSP servlet default name
    */
    public static final String DEFAULT_SERVLET_NAME = "*.jsp";
    /** the configured name of the jsp servlet
    */
    String servletName = DEFAULT_SERVLET_NAME;

    /**
     * Set the sitemap-provided configuration.
     * @param conf The configuration information
     * @exception ConfigurationException
     */
    public void parameterize(Parameters params)  {
        this.servletName = params.getParameter(CONFIG_SERVLET_NAME, DEFAULT_SERVLET_NAME);
    }

    /**
     * execute the JSP and return the output
     *
     * @param context The Servlet Context
     * @exception IOException
     * @exception ServletException
     * @exception SAXException
     * @exception Exception
     */
    public byte[] executeJSP(String url, HttpServletRequest httpRequest, HttpServletResponse httpResponse, ServletContext context)
        throws IOException, ServletException, SAXException, Exception {

        byte[] bytes = null;

        MyServletRequest request = new MyServletRequest(httpRequest, url);
        MyServletResponse response = new MyServletResponse(httpResponse);

        // start JSPServlet.
        javax.servlet.RequestDispatcher rd = context.getNamedDispatcher( servletName );
        if (rd != null) {
          rd.include( request, response );
          response.flushBuffer();
          bytes = response.toByteArray();
          ByteArrayInputStream input = new ByteArrayInputStream( bytes );
        } else {
          getLogger().error( "Specify a correct " + CONFIG_SERVLET_NAME + " " + servletName );
        }
        return bytes;
    }


    /**
     * Stub implementation of HttpServletRequest
     */
    class MyServletRequest implements HttpServletRequest {
        HttpServletRequest request;
        String jspFile;

        public MyServletRequest(HttpServletRequest request, String jspFile) {
            this.request = request;
            this.jspFile = jspFile;
        }
        public String getAuthType(){ return request.getAuthType(); }
        public Cookie[] getCookies(){ return request.getCookies(); }
        public long getDateHeader(String s){ return request.getDateHeader(s); }
        public String getHeader(String s){ return request.getHeader(s); }
        public Enumeration getHeaders(String s){ return request.getHeaders(s); }
        public Enumeration getHeaderNames(){ return request.getHeaderNames(); }
        public int getIntHeader(String s){ return request.getIntHeader(s); }
        public String getMethod(){ return request.getMethod(); }
        public String getPathInfo(){ return request.getPathInfo(); }
        public String getPathTranslated(){ return request.getPathTranslated(); }
        public String getContextPath(){ return request.getContextPath(); }
        public String getQueryString(){ return request.getQueryString(); }
        public String getRemoteUser(){ return request.getRemoteUser(); }
        public boolean isUserInRole(String s){ return request.isUserInRole(s); }
        public Principal getUserPrincipal(){ return request.getUserPrincipal(); }
        public String getRequestedSessionId(){ return request.getRequestedSessionId(); }
        public String getRequestURI(){ return request.getRequestURI(); }
        public String getServletPath(){ return request.getServletPath(); }
        public HttpSession getSession(boolean flag){ return request.getSession(flag); }
        public HttpSession getSession(){ return request.getSession(); }
        public boolean isRequestedSessionIdValid(){ return request.isRequestedSessionIdValid(); }
        public boolean isRequestedSessionIdFromCookie(){ return request.isRequestedSessionIdFromCookie(); }
        public boolean isRequestedSessionIdFromURL(){ return request.isRequestedSessionIdFromURL(); }
        public boolean isRequestedSessionIdFromUrl(){ return request.isRequestedSessionIdFromUrl(); }
        public Object getAttribute(String s){
            if(s != null && s.equals(INC_SERVLET_PATH))
                return jspFile;
            return request.getAttribute(s);
        }
        public Enumeration getAttributeNames(){ return request.getAttributeNames(); }
        public String getCharacterEncoding(){ return request.getCharacterEncoding(); }
        public int getContentLength(){ return request.getContentLength(); }
        public String getContentType(){ return request.getContentType(); }
        public ServletInputStream getInputStream() throws IOException{ return request.getInputStream(); }
        public String getParameter(String s){ return request.getParameter(s); }
        public Enumeration getParameterNames(){ return request.getParameterNames(); }
        public String[] getParameterValues(String s){ return request.getParameterValues(s); }
        public String getProtocol(){ return request.getProtocol(); }
        public String getScheme(){ return request.getScheme(); }
        public String getServerName(){ return request.getServerName(); }
        public int getServerPort(){ return request.getServerPort(); }
        public BufferedReader getReader()
            throws IOException{ return request.getReader(); }
        public String getRemoteAddr(){ return request.getRemoteAddr(); }
        public String getRemoteHost(){ return request.getRemoteHost(); }
        public void setAttribute(String s, Object obj){ request.setAttribute(s,obj); }
        public void removeAttribute(String s){ request.removeAttribute(s); }
        public Locale getLocale(){ return request.getLocale(); }
        public Enumeration getLocales(){ return request.getLocales(); }
        public boolean isSecure(){ return request.isSecure(); }
        public RequestDispatcher getRequestDispatcher(String s){ return request.getRequestDispatcher(s); }
        public String getRealPath(String s){ return request.getRealPath(s); }
        public java.lang.StringBuffer getRequestURL() { return null; }
        public java.util.Map getParameterMap() { return null; }
        public void setCharacterEncoding(java.lang.String s) { }
    }

    /**
     * Stub implementation of HttpServletResponse
     */
    class MyServletResponse implements HttpServletResponse {
        HttpServletResponse response;
        MyServletOutputStream output;

        public MyServletResponse(HttpServletResponse response){
            this.response = response;
            this.output = new MyServletOutputStream();
        }
        public void flushBuffer() throws IOException { }
        public int getBufferSize() { return 1024; }
        public String getCharacterEncoding() { return this.response.getCharacterEncoding();}
        public Locale getLocale(){ return this.response.getLocale();}
        public PrintWriter getWriter() {
            return this.output.getWriter();
        }
        public boolean isCommitted() { return false; }
        public void reset() {}
        public void setBufferSize(int size) {}
        public void setContentLength(int len) {}
        public void setContentType(java.lang.String type) {}
        public void setLocale(java.util.Locale loc) {}
        public ServletOutputStream getOutputStream() {
            return this.output;
        }
        public void addCookie(Cookie cookie){ response.addCookie(cookie); }
        public boolean containsHeader(String s){ return response.containsHeader(s); }
        public String encodeURL(String s){ return response.encodeURL(s); }
        public String encodeRedirectURL(String s){ return response.encodeRedirectURL(s); }
        public String encodeUrl(String s){ return response.encodeUrl(s); }
        public String encodeRedirectUrl(String s){ return response.encodeRedirectUrl(s); }
        public void sendError(int i, String s)
            throws IOException{response.sendError(i,s); }
        public void sendError(int i)
            throws IOException{response.sendError(i); }
        public void sendRedirect(String s)
            throws IOException{response.sendRedirect(s); }
        public void setDateHeader(String s, long l){response.setDateHeader(s, l); }
        public void addDateHeader(String s, long l){response.addDateHeader(s, l); }
        public void setHeader(String s, String s1){response.setHeader(s, s1); }
        public void addHeader(String s, String s1){response.addHeader(s, s1); }
        public void setIntHeader(String s, int i){response.setIntHeader(s, i); }
        public void addIntHeader(String s, int i){response.addIntHeader(s, i); }
        public void setStatus(int i){response.setStatus(i); }
        public void setStatus(int i, String s){response.setStatus(i, s); }
        public void resetBuffer(){}

        public byte[] toByteArray() {
            return output.toByteArray();
        }
    }

    /**
     * Stub implementation of ServletOutputStream
     */
    class MyServletOutputStream extends ServletOutputStream {
        ByteArrayOutputStream output;
        PrintWriter writer;

        public MyServletOutputStream() {
            this.output = new ByteArrayOutputStream();
            try {
                this.writer = new PrintWriter(new OutputStreamWriter(output, "utf-8"));
            } catch (UnsupportedEncodingException e) {
                // This can't be true: JVM must support UTF-8 encoding.
                this.writer = new PrintWriter(new OutputStreamWriter(output));
            }
        }
        public PrintWriter getWriter() {
            return this.writer;
        }
        public void write(int b) throws java.io.IOException  {
            // This method is not used but have to be implemented
            this.writer.write(b);
        }
        public byte[] toByteArray() {
            this.writer.flush();
            byte[] bytes = output.toByteArray();
            return bytes;
        }
    }
}

