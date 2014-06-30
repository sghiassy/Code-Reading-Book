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
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Ensure the correct container managed request attributes are set.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/09/18 00:08:00 $
 */

public class Include10a extends HttpServlet {

    private static final String specials[] =
    { "javax.servlet.include.request_uri",
      "javax.servlet.include.context_path",
      "javax.servlet.include.servlet_path",
      "javax.servlet.include.path_info",
      "javax.servlet.include.query_string" };


    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        // Prepare this response
        StringBuffer sb = new StringBuffer();
	PrintWriter writer = response.getWriter();

        // Validate the original request properties
        String value = null;
        value = (String) request.getAttribute("original.request_uri");
        if (!value.equals(request.getRequestURI()))
            sb.append(" getRequestURI() is " + request.getRequestURI() +
                      " but should be " + value + "|");
        value = (String) request.getAttribute("original.context_path");
        if (!value.equals(request.getContextPath()))
            sb.append(" getContextPath() is " + request.getContextPath() +
                      " but should be " + value + "|");
        value = (String) request.getAttribute("original.servlet_path");
        if (!value.equals(request.getServletPath()))
            sb.append(" getServletPath() is " + request.getServletPath() +
                      " but should be " + value + "|");
        value = (String) request.getAttribute("original.path_info");
        if (!value.equals(request.getPathInfo()))
            sb.append(" getPathInfo() is " + request.getPathInfo() +
                      " but should be " + value + "|");
        value = (String) request.getAttribute("original.query_string");
        if (!value.equals(request.getQueryString()))
            sb.append(" getQueryString() is " + request.getQueryString() +
                      " but should be " + value + "|");

        // Validate the container provided request attributes
        value = (String)
            request.getAttribute("javax.servlet.include.request_uri");
        if (!(request.getContextPath() + "/Include10a/include/path").equals(value))
            sb.append(" request_uri is " + value +
                      " but should be " + request.getContextPath() +
                      "/Include10a/include/path|");
        value = (String)
            request.getAttribute("javax.servlet.include.context_path");
        if (!request.getContextPath().equals(value))
            sb.append(" context_path is " + value +
                      " but should be " + request.getContextPath() + "|");
        value = (String)
            request.getAttribute("javax.servlet.include.servlet_path");
        if (!"/Include10a".equals(value))
            sb.append(" servlet_path is " + value +
                      " but should be /Include10a|");
        value = (String)
            request.getAttribute("javax.servlet.include.path_info");
        if (!"/include/path".equals(value))
            sb.append(" path_info is " + value +
                      " but should be /include/path|");
        value = (String)
            request.getAttribute("javax.servlet.include.query_string");
        if (!"name2=value2".equals(value))
            sb.append(" query_string is " + value +
                      " but should be name2=value2|");

        // Generate our success or failure report
        if (sb.length() < 1)
            writer.println("Include10a PASSED");
        else
            writer.println("Include10a FAILED -" + sb.toString());

    }

}
