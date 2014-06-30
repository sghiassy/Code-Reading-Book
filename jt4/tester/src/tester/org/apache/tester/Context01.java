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
import javax.servlet.*;
import javax.servlet.http.*;
import org.apache.tester.shared.SharedSessionBean;
import org.apache.tester.unshared.UnsharedSessionBean;


/**
 * Part 1 of Context Tests.  Exercise various methods for dealing with
 * servlet context attributes.  Leave an attribute named "context01"
 * present, which should be erased after a web application restart.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/06/24 00:00:05 $
 */

public class Context01 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        boolean ok = true;
        PrintWriter writer = response.getWriter();
        ServletContext context = getServletContext();

        // Ensure that there is no existing attribute
        if (ok) {
            if (context.getAttribute("context01") != null) {
                writer.println("Context01 FAILED - Attribute already exists");
                ok = false;
            }
        }

        // Create and stash a context attribute
        if (ok) {
            ContextBean bean = new ContextBean();
            bean.setStringProperty("Context01");
            context.setAttribute("context01", bean);
        }

        // Ensure that we can retrieve the attribute successfully
        if (ok) {
            Object bean = context.getAttribute("context01");
            if (bean == null) {
                writer.println("Context01 FAILED - Cannot retrieve attribute");
                ok = false;
            }
            if (ok) {
                if (!(bean instanceof ContextBean)) {
                    writer.println("Context01 FAILED - Bean instance of " +
                                   bean.getClass().getName());
                    ok = false;
                }
            }
            if (ok) {
                String value = ((ContextBean) bean).getStringProperty();
                if (!"Context01".equals(value)) {
                    writer.println("Context01 FAILED - Value = " + value);
                    ok = false;
                }
            }
            if (ok) {
                String lifecycle = ((ContextBean) bean).getLifecycle();
                if (!"/add".equals(lifecycle)) {
                    writer.println("Context01 FAILED - Bean lifecycle is " +
                                   lifecycle);
                    ok = false;
                }
            }
        }

        // Ensure that we can update this attribute and check its lifecycle
        if (ok) {
            ContextBean bean = (ContextBean) context.getAttribute("context01");
            context.setAttribute("context01", bean);
            String lifecycle = bean.getLifecycle();
            if (!"/add/rep".equals(lifecycle)) {
                writer.println("Context01 FAILED - Bean lifecycle is " +
                               lifecycle);
                ok = false;
            }
        }

        // Ensure that we can remove this attribute and check its lifecycle
        if (ok) {
            ContextBean bean = (ContextBean) context.getAttribute("context01");
            context.removeAttribute("context01");
            String lifecycle = bean.getLifecycle();
            if (!"/add/rep/rem".equals(lifecycle)) {
                writer.println("Context01 FAILED - Bean lifecycle is " +
                               lifecycle);
                ok = false;
            }
        }

        // Add a bean back for the restart application test
        context.setAttribute("context01", new ContextBean());

        // Ensure that setAttribute("name", null) works correctly
        if (ok) {
            context.setAttribute("FOO", "BAR");
            context.setAttribute("FOO", null);
            if (context.getAttribute("FOO") != null) {
                writer.println("Context01 FAILED - setAttribute(name,null)");
                ok = false;
            }
        }

        // Report success if everything is still ok
        if (ok)
            writer.println("Context01 PASSED");
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
