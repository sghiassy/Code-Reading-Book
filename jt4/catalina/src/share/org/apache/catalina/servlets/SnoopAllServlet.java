/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/servlets/SnoopAllServlet.java,v 1.4 2001/07/22 20:25:11 pier Exp $
 * $Revision: 1.4 $
 * $Date: 2001/07/22 20:25:11 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.servlets;


import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Timestamp;
import java.util.Enumeration;
import java.util.Locale;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;


/**
 * Servlet to dump everything that is known in the current request object
 * into an HTML page.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.4 $ $Date: 2001/07/22 20:25:11 $
 */

public final class SnoopAllServlet
    extends HttpServlet {


    /**
     * Process a GET request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    public void doGet(HttpServletRequest request,
                      HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/html");
        PrintWriter writer = response.getWriter();

        // Generate HTML page header
        writer.println("<head>");
        writer.println("<title>SnoopAllServlet Output</title>");
        writer.println("</head>");
        writer.println("<body bgcolor=\"white\">");

        // Document the properties from ServletRequest
        writer.println("<h1>ServletRequest Properties</h1>");
        writer.println("<ul>");
        Enumeration attrs = request.getAttributeNames();
        while (attrs.hasMoreElements()) {
            String attr = (String) attrs.nextElement();
            writer.println("<li><b>attribute</b> " + attr + " = " +
                           request.getAttribute(attr));
        }
        writer.println("<li><b>characterEncoding</b> = " +
                       request.getCharacterEncoding());
        writer.println("<li><b>contentLength</b> = " +
                       request.getContentLength());
        writer.println("<li><b>contentType</b> = " +
                       request.getContentType());
        Enumeration locales = request.getLocales();
        while (locales.hasMoreElements()) {
            Locale locale = (Locale) locales.nextElement();
            writer.println("<li><b>locale</b> = " + locale);
        }
        Enumeration params = request.getParameterNames();
        while (params.hasMoreElements()) {
            String param = (String) params.nextElement();
            String values[] = request.getParameterValues(param);
            for (int i = 0; i < values.length; i++)
                writer.println("<li><b>parameter</b> " + param + " = " +
                               values[i]);
        }
        writer.println("<li><b>protocol</b> = " + request.getProtocol());
        writer.println("<li><b>remoteAddr</b> = " + request.getRemoteAddr());
        writer.println("<li><b>remoteHost</b> = " + request.getRemoteHost());
        writer.println("<li><b>scheme</b> = " + request.getScheme());
        writer.println("<li><b>secure</b> = " + request.isSecure());
        writer.println("<li><b>serverName</b> = " + request.getServerName());
        writer.println("<li><b>serverPort</b> = " + request.getServerPort());
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the properties from HttpServletRequest
        writer.println("<h1>HttpServletRequest Properties</h1>");
        writer.println("<ul>");
        writer.println("<li><b>authType</b> = " + request.getAuthType());
        writer.println("<li><b>contextPath</b> = " +
                       request.getContextPath());
        Cookie cookies[] = request.getCookies();
        if (cookies == null)
            cookies = new Cookie[0];
        for (int i = 0; i < cookies.length; i++)
            writer.println("<li><b>cookie</b> " + cookies[i].getName() +
                           " = " + cookies[i].getValue());
        Enumeration headers = request.getHeaderNames();
        while (headers.hasMoreElements()) {
            String header = (String) headers.nextElement();
            writer.println("<li><b>header</b> " + header + " = " +
                           request.getHeader(header));
        }
        writer.println("<li><b>method</b> = " + request.getMethod());
        writer.println("<li><b>pathInfo</b> = " + request.getPathInfo());
        writer.println("<li><b>pathTranslated</b> = " +
                       request.getPathTranslated());
        writer.println("<li><b>queryString</b> = " +
                       request.getQueryString());
        writer.println("<li><b>remoteUser</b> = " +
                       request.getRemoteUser());
        writer.println("<li><b>requestedSessionId</b> = " +
                       request.getRequestedSessionId());
        writer.println("<li><b>requestedSessionIdFromCookie</b> = " +
                       request.isRequestedSessionIdFromCookie());
        writer.println("<li><b>requestedSessionIdFromURL</b> = " +
                       request.isRequestedSessionIdFromURL());
        writer.println("<li><b>requestedSessionIdValid</b> = " +
                       request.isRequestedSessionIdValid());
        writer.println("<li><b>requestURI</b> = " +
                       request.getRequestURI());
        writer.println("<li><b>servletPath</b> = " +
                       request.getServletPath());
        writer.println("<li><b>userPrincipal</b> = " +
                       request.getUserPrincipal());
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the servlet request attributes
        writer.println("<h1>ServletRequest Attributes</h1>");
        writer.println("<ul>");
        attrs = request.getAttributeNames();
        while (attrs.hasMoreElements()) {
            String attr = (String) attrs.nextElement();
            writer.println("<li><b>" + attr + "</b> = " +
                           request.getAttribute(attr));
        }
        writer.println("</ul>");
        writer.println("<hr>");

        // Process the current session (if there is one)
        HttpSession session = request.getSession(false);
        if (session != null) {

            // Document the session properties
            writer.println("<h1>HttpSession Properties</h1>");
            writer.println("<ul>");
            writer.println("<li><b>id</b> = " +
                           session.getId());
            writer.println("<li><b>creationTime</b> = " +
                           new Timestamp(session.getCreationTime()));
            writer.println("<li><b>lastAccessedTime</b> = " +
                           new Timestamp(session.getLastAccessedTime()));
            writer.println("<li><b>maxInactiveInterval</b> = " +
                           session.getMaxInactiveInterval());
            writer.println("</ul>");
            writer.println("<hr>");

            // Document the session attributes
            writer.println("<h1>HttpSession Attributes</h1>");
            writer.println("<ul>");
            attrs = session.getAttributeNames();
            while (attrs.hasMoreElements()) {
                String attr = (String) attrs.nextElement();
                writer.println("<li><b>" + attr + "</b> = " +
                               session.getAttribute(attr));
            }
            writer.println("</ul>");
            writer.println("<hr>");

        }

        // Document the servlet configuration properties
        writer.println("<h1>ServletConfig Properties</h1>");
        writer.println("<ul>");
        writer.println("<li><b>servletName</b> = " +
                       getServletConfig().getServletName());
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the servlet configuration initialization parameters
        writer.println("<h1>ServletConfig Initialization Parameters</h1>");
        writer.println("<ul>");
        params = getServletConfig().getInitParameterNames();
        while (params.hasMoreElements()) {
            String param = (String) params.nextElement();
            String value = getServletConfig().getInitParameter(param);
            writer.println("<li><b>" + param + "</b> = " + value);
        }
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the servlet context properties
        writer.println("<h1>ServletContext Properties</h1>");
        writer.println("<ul>");
        writer.println("<li><b>majorVersion</b> = " +
                       getServletContext().getMajorVersion());
        writer.println("<li><b>minorVersion</b> = " +
                       getServletContext().getMinorVersion());
        writer.println("<li><b>realPath('/')</b> = " +
                       getServletContext().getRealPath("/"));
        writer.println("<li><b>serverInfo</b> = " +
                       getServletContext().getServerInfo());
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the servlet context initialization parameters
        writer.println("<h1>ServletContext Initialization Parameters</h1>");
        writer.println("<ul>");
        params = getServletContext().getInitParameterNames();
        while (params.hasMoreElements()) {
            String param = (String) params.nextElement();
            String value = getServletContext().getInitParameter(param);
            writer.println("<li><b>" + param + "</b> = " + value);
        }
        writer.println("</ul>");
        writer.println("<hr>");

        // Document the servlet context attributes
        writer.println("<h1>ServletContext Attributes</h1>");
        writer.println("<ul>");
        attrs = getServletContext().getAttributeNames();
        while (attrs.hasMoreElements()) {
            String attr = (String) attrs.nextElement();
            writer.println("<li><b>" + attr + "</b> = " +
                           getServletContext().getAttribute(attr));
        }
        writer.println("</ul>");
        writer.println("<hr>");

        // Generate HTML page footer
        writer.println("</body>");
        writer.println("</html>");

    }


    /**
     * Process a POST request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    public void doPost(HttpServletRequest request,
                       HttpServletResponse response)
        throws IOException, ServletException {

        doGet(request, response);

    }


}
