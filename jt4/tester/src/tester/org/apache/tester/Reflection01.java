/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *      Copyright (c) 1999, 2000, 2001  The Apache Software Foundation.      *
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
import java.lang.reflect.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;


/**
 * Negative test for access to Catalina internals through the objects that
 * are exposed to this servlet by the container.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/05/12 04:58:27 $
 */

public class Reflection01 extends HttpServlet {

    public void service(HttpServletRequest request,
                        HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();
        StringBuffer results = new StringBuffer();

        // Check the ServletConfig object
        try {
            ServletConfig servletConfig = getServletConfig();
            Method method = servletConfig.getClass().getMethod
                ("getParent", new Class[] {});
            Object parent = method.invoke(servletConfig,
                                          new Object[] {});
            results.append(" Can reflect on ServletConfig/");
        } catch (Throwable t) {
            StaticLogger.write("ServletConfig: " + t);
        }

        // Check the ServletContext object
        try {
            ServletContext servletContext = getServletContext();
            Method method = servletContext.getClass().getMethod
                ("getResources", new Class[] {});
            Object resources = method.invoke(servletContext,
                                             new Object[] {});
            results.append(" Can reflect on ServletContext/");
        } catch (Throwable t) {
            StaticLogger.write("ServletContext: " + t);
        }

        // Check the HttpServletRequest object
        try {
            Method method = request.getClass().getMethod
                ("getInfo", new Class[] {});
            Object info = method.invoke(request,
                                        new Object[] {});
            results.append(" Can reflect on HttpServletRequest/");
        } catch (Throwable t) {
            StaticLogger.write("HttpServletRequest: " + t);
        }

        // Check the HttpServletResponse object
        try {
            Method method = request.getClass().getMethod
                ("getInfo", new Class[] {});
            Object info = method.invoke(request,
                                        new Object[] {});
            results.append(" Can reflect on HttpServletResponse/");
        } catch (Throwable t) {
            StaticLogger.write("HttpServletResponse: " + t);
        }

        // Check the HttpSession object
        try {
            HttpSession session = request.getSession(true);
            Method method = session.getClass().getMethod
                ("getInfo", new Class[] {});
            results.append(" Can reflect on HttpSession/");
        } catch (Throwable t) {
            StaticLogger.write("HttpSession: " + t);
        }

        // Check the RequestDispatcher object
        try {
            RequestDispatcher rd =
                getServletContext().getRequestDispatcher("/index.shtml");
            Method method = rd.getClass().getMethod
                ("getInfo", new Class[] {});
            results.append(" Can reflect on RequestDispatcher/");
        } catch (Throwable t) {
            StaticLogger.write("RequestDispatcher: " + t);
        }

        // Report final results
        if (results.length() < 1)
            writer.println("Reflection01 PASSED");
        else {
            writer.print("Reflection01 FAILED -");
            writer.println(results.toString());
        }

        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
