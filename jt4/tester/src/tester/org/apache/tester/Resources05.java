/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *     Copyright (c) 1999, 2000, 2001  The Apache Software Foundation.       *
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
import java.net.MalformedURLException;
import java.net.URL;
import javax.servlet.*;
import javax.servlet.http.*;


/**
 * Positive test for <code>ServletContext.getResource()</code> as well as
 * <code>ClassLoader.getResource()</code>.  The URL returned by
 * these calls is then opened with <code>url.openStream()</code>
 * in order to ensure that the correct data is actually read.
 * Operation is controlled by query parameters:
 * <ul>
 * <li><strong>mode</strong> - Use <code>context</code> for servlet context
 *     test, or <code>class</code> for class loader test.  [context]</li>
 * <li><strong>path</strong> - Resource path to the requested resource,
 *     starting with a slash.  [/WEB-INF/web.xml]</li>
 * <li><strong>stringify</strong> - If set to any arbitrary value, the URL
 *     returned by getResource() will be converted to a String and then back
 *     to a URL before being opened.  [not set]</li>
 * </ul>
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.3 $ $Date: 2001/03/21 20:47:06 $
 */

public class Resources05 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        // Identify our configuration parameters
        String mode = request.getParameter("mode");
        if (mode == null)
            mode = "context";
        String path = request.getParameter("path");
        if (path == null)
            path = "/WEB-INF/web.xml";
        boolean stringify = (request.getParameter("stringify") != null);

        // Prepare for the desired test
        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();
        InputStream is = null;
        InputStreamReader isr = null;
        URL url = null;
        StringBuffer results = new StringBuffer();
        boolean ok = true;

        // Acquire the appropriate URL
        try {
            if ("context".equals(mode))
                url = getServletContext().getResource(path);
            else
                url = this.getClass().getResource(path);
            if (url == null) {
                results.append(" No URL returned");
                ok = false;
            }
        } catch (MalformedURLException e) {
            results.append(" getResource MalformedURLException");
            ok = false;
        }

        // Stringify the URL if requested
        try {
            if (ok) {
                StaticLogger.write("Stringifying the URL");
                String urlString = url.toString();
                url = new URL(urlString);
            }
        } catch (MalformedURLException e) {
            results.append(" stringify MalformedURLException");
        }

        // Open an input stream and input stream reader on this URL
        try {
            if (ok) {
                is = url.openStream();
                isr = new InputStreamReader(is);
            }
        } catch (IOException e) {
            results.append(" Open IOException: " + e);
            ok = false;
        }

        // Copy the contents of this stream to our output
        try {
            if (ok) {
                while (true) {
                    int ch = isr.read();
                    if (ch < 0)
                        break;
                    writer.print((char) ch);
                }
            }
        } catch (IOException e) {
            results.append(" Copy IOException: " + e);
            ok = false;
        }

        // Close the input stream
        try {
            if (ok) {
                isr.close();
            }
        } catch (IOException e) {
            results.append(" Close IOException: " + e);
        }

        // Report any failures we have encountered
        if (!ok) {
            writer.print("Resources05 FAILED -");
            writer.println(results.toString());
        }

        // Add wrapper messages as required
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
