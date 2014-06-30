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
import org.apache.tester.unpshared.UnpSharedSessionBean;
import org.apache.tester.unshared.UnsharedSessionBean;


/**
 * Part 1 of Session Tests.  Ensures that there is no current session, then
 * creates a new session and sets a session attribute.  Also, ensure that
 * calling setAttribute("name", null) acts like removeAttribute().
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.4 $ $Date: 2001/08/15 00:57:09 $
 */

public class Session01 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Ensure that there is no current session
        boolean ok = true;
        HttpSession session = request.getSession(false);
        if (session != null) {
            writer.println("Session01 FAILED - Requested existing session " +
                           session.getId());
            ok = false;
        }

        // Create a new session
        if (ok) {
            session = request.getSession(true);
            if (session == null) {
                writer.println("Session01 FAILED - No session created");
                ok = false;
            }
        }

        // Ensure that there is no existing attribute
        if (ok) {
            if (session.getAttribute("sessionBean") != null) {
                writer.println("Session01 FAILED - Attribute already exists");
                ok = false;
            }
        }

        // Create and stash a session attribute
        if (ok) {
            SessionBean bean = new SessionBean();
            bean.setStringProperty("Session01");
            session.setAttribute("sessionBean", bean);
        }

        // Ensure that we can retrieve the attribute successfully
        if (ok) {
            Object bean = session.getAttribute("sessionBean");
            if (bean == null) {
                writer.println("Session01 FAILED - Cannot retrieve attribute");
                ok = false;
            } else if (!(bean instanceof SessionBean)) {
                writer.println("Session01 FAILED - Attribute instance of " +
                               bean.getClass().getName());
                ok = false;
            } else {
                String value = ((SessionBean) bean).getStringProperty();
                if (!"Session01".equals(value)) {
                    writer.println("Session01 FAILED - Property = " + value);
                    ok = false;
                }
            }
        }

        // Ensure that setAttribute("name", null) works correctly
        if (ok) {
            session.setAttribute("FOO", "BAR");
            session.setAttribute("FOO", null);
            if (session.getAttribute("FOO") != null) {
                writer.println("Session01 FAILED - setAttribute(name,null)");
                ok = false;
            }
        }

        // Create more beans that will be used to test application restart
        if (ok) {
            SharedSessionBean ssb = new SharedSessionBean();
            ssb.setStringProperty("Session01");
            session.setAttribute("sharedSessionBean", ssb);
            UnpSharedSessionBean ussb = new UnpSharedSessionBean();
            ussb.setStringProperty("Session01");
            session.setAttribute("unpSharedSessionBean", ussb);
            UnsharedSessionBean usb = new UnsharedSessionBean();
            usb.setStringProperty("Session01");
            session.setAttribute("unsharedSessionBean", usb);
        }

        // Report success if everything is still ok
        if (ok)
            writer.println("Session01 PASSED");
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
