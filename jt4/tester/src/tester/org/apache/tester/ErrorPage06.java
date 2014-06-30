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
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Part 6 of the ErrorPage Tests.  Should be mapped by the container when
 * the ErrorPage05 servlet returns the appropriate exception.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.5 $ $Date: 2001/07/17 04:21:14 $
 */

public class ErrorPage06 extends HttpServlet {


    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.reset();
        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Accumulate all the reasons this request might fail
        StringBuffer sb = new StringBuffer();
        Object value = null;

        value = request.getAttribute("javax.servlet.error.exception");
        StaticLogger.write("exception is '" + value + "'");
        if (value == null) {
            sb.append(" exception is missing/");
        } else if (!(value instanceof java.lang.ArithmeticException)) {
            sb.append(" exception class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        }

        value = request.getAttribute("javax.servlet.error.exception_type");
        StaticLogger.write("exception_type is '" + value + "'");
        if (value != null)
            StaticLogger.write("exception_type class is '" +
                               value.getClass().getName() + "'");
        if (value == null)
            sb.append(" exception_type is missing/");
        else if (!(value instanceof Class)) {
            sb.append(" exception_type class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            Class clazz = (Class) value;
            String name = clazz.getName();
            if (!"java.lang.ArithmeticException".equals(name)) {
                sb.append(" exception_type is ");
                sb.append(name);
                sb.append("/");
            }
        }

        value = request.getAttribute("javax.servlet.error.message");
        StaticLogger.write("message is '" + value + "'");
        if (value == null)
            sb.append(" message is missing/");
        else if (!(value instanceof String)) {
            sb.append(" message class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else if (!"ErrorPage05 Threw ArithmeticException".equals(value) &&
                   !"ErrorPage08 Threw ArithmeticException".equals(value)) {
            sb.append(" message is not correct");
        }

        value = request.getAttribute("javax.servlet.error.request_uri");
        StaticLogger.write("request_uri is '" + value + "'");
        if (value == null)
            sb.append(" request_uri is missing/");
        else if (!(value instanceof String)) {
            sb.append(" request_uri class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            String request_uri = (String) value;
            String test1 = request.getContextPath() + "/ErrorPage05";
            String test2 = request.getContextPath() + "/WrappedErrorPage05";
            String test3 = request.getContextPath() + "/ErrorPage08";
            String test4 = request.getContextPath() + "/WrappedErrorPage08";
            if (!request_uri.equals(test1) && !request_uri.equals(test2) &&
                !request_uri.equals(test3) && !request_uri.equals(test4)) {
                sb.append(" request_uri is ");
                sb.append(request_uri);
                sb.append("/");
            }
        }

        value = request.getAttribute("javax.servlet.error.servlet_name");
        StaticLogger.write("servlet_name is '" + value + "'");
        if (value == null)
            sb.append(" servlet_name is missing/");
        else if (!(value instanceof String)) {
            sb.append(" servlet_name class is ");
            sb.append(value.getClass().getName());
            sb.append("/");
        } else {
            String servlet_name = (String) value;
            if (!"ErrorPage05".equals(servlet_name) &&
                !"ErrorPage08".equals(servlet_name)) {
                sb.append(" servlet_name is ");
                sb.append(servlet_name);
                sb.append("/");
            }
        }

        // Report ultimate success or failure
        if (sb.length() < 1)
            writer.println("ErrorPage06 PASSED - SERVLET");
        else
            writer.println("ErrorPage06 FAILED -" + sb.toString());

        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }


}
