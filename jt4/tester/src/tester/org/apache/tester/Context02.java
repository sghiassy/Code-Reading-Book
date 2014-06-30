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
 * Part 2 of Context Tests.  The context attribute from Context00 should
 * still be here after a restart (because Context00 is a load-on-startup
 * servlet, so the <code>init()</code> method should have been triggered
 * during the restart).  However, the context attribute from Context01
 * should <strong>not</strong> be here, because context attributes should
 * be cleaned up during a restart.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/06/24 00:08:24 $
 */

public class Context02 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        boolean ok = true;
        PrintWriter writer = response.getWriter();
        ServletContext context = getServletContext();

        // Check for the attribute from Context01
        if (ok) {
            Object bean = context.getAttribute("context01");
            if (bean != null) {
                writer.println("Context02 FAILED - context01 value " +
                               bean);
                ok = false;
                context.removeAttribute("context01");
            }
        }

        // Check for the attribute from Context00
        if (ok) {
            Object bean = context.getAttribute("context00");
            if (bean == null) {
                writer.println("Context02 FAILED - context00 missing");
                ok = false;
            } else if (!(bean instanceof ContextBean)) {
                writer.println("Context02 FAILED - context00 class " +
                               bean.getClass().getName());
                ok = false;
            } else {
                String value = ((ContextBean) bean).getStringProperty();
                if (!"Context00".equals(value)) {
                    writer.println("Context02 FAILED - context00 value " +
                                   value);
                    ok = false;
                } else {
                    String lifecycle = ((ContextBean) bean).getLifecycle();
                    if (!"/add".equals(lifecycle)) {
                        writer.println("Context02 FAILED -" +
                                       " context00 lifecycle " +
                                       lifecycle);
                        ok = false;
                    }
                }
            }
        }

        // Check for the attribute from ContextListener01
        if (ok) {
            Object bean = context.getAttribute("contextListener01");
            if (bean == null) {
                writer.println("Context02 FAILED - contextListener01 missing");
                ok = false;
            } else if (!(bean instanceof ContextBean)) {
                writer.println("Context02 FAILED - contextListener01 class " +
                               bean.getClass().getName());
                ok = false;
            } else {
                String value = ((ContextBean) bean).getStringProperty();
                if (!"ContextListener01".equals(value)) {
                    writer.println("Context02 FAILED - contextListener01 " +
                                   "value " + value);
                    ok = false;
                } else {
                    String lifecycle = ((ContextBean) bean).getLifecycle();
                    if (!"/add".equals(lifecycle)) {
                        writer.println("Context02 FAILED -" +
                                       " contextListener01 lifecycle " +
                                       lifecycle);
                        ok = false;
                    }
                }
            }
        }

        // Report success if everything is still ok
        if (ok)
            writer.println("Context02 PASSED");
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
