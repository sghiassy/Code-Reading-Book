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
 * Part 3 of Session Tests.  Ensures that there is an existing session, and
 * that the session bean stashed in Part 1 can be retrieved successfully.
 * Then, it removes that attribute and verifies successful removal.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.4 $ $Date: 2001/08/15 00:57:09 $
 */

public class Session03 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Ensure that there is a current session
        boolean ok = true;
        HttpSession session = request.getSession(false);
        if (session == null) {
            writer.println("Session03 FAILED - No existing session " +
                           request.getRequestedSessionId());
            ok = false;
        }

        // Ensure that we can retrieve the attribute successfully
	SessionBean bean = null;
        if (ok) {
            Object object = session.getAttribute("sessionBean");
            if (object == null) {
                writer.println("Session03 FAILED - Cannot retrieve attribute");
                ok = false;
            } else if (!(object instanceof SessionBean)) {
                writer.println("Session03 FAILED - Attribute instance of " +
                               object.getClass().getName());
                ok = false;
            } else {
                bean = (SessionBean) object;
                String value = bean.getStringProperty();
                if (!"Session01".equals(value)) {
                    writer.println("Session03 FAILED - Property = " + value);
                    ok = false;
                }
            }
        }

        // Remove the attribute and guarantee that this was successful
        if (ok) {
            session.removeAttribute("sessionBean");
            if (session.getAttribute("sessionBean") != null) {
                writer.println("Session03 FAILED - Removal failed");
                ok = false;
            }
        }

	// Validate the bean lifecycle of this bean
	if (ok) {
	    String lifecycle = bean.getLifecycle();
	    if (!"/vb/swp/sda/vu".equals(lifecycle)) {
	        writer.println("Session03 FAILED - Invalid bean lifecycle '" +
			       lifecycle + "'");
		ok = false;
	    }
	}

        // Retrieve and validate the shared session bean
        SharedSessionBean ssb = null;
        if (ok) {
            Object object = session.getAttribute("sharedSessionBean");
            if (object == null) {
                writer.println("Session03 FAILED - Cannot retrieve ssb");
                ok = false;
            } else if (!(object instanceof SharedSessionBean)) {
                writer.println("Session03 FAILED - Shared attribute class "
                               + object.getClass().getName());
                ok = false;
            } else {
                ssb = (SharedSessionBean) object;
                String value = ssb.getStringProperty();
                if (!"Session01".equals(value)) {
                    writer.println("Session03 FAILED - Shared property ="
                                   + value);
                    ok = false;
                } else {
                    session.removeAttribute("sharedSessionBean");
                    String lifecycle = ssb.getLifecycle();
                    if (!"/vb/swp/sda/vu".equals(lifecycle)) {
                        writer.println("Session03 FAILED - Shared lifecycle ="
                                       + lifecycle);
                        ok = false;
                    }
                }
            }
        }

        // Retrieve and validate the unpacked shared session bean
        UnpSharedSessionBean ussb = null;
        if (ok) {
            Object object = session.getAttribute("unpSharedSessionBean");
            if (object == null) {
                writer.println("Session03 FAILED - Cannot retrieve ussb");
                ok = false;
            } else if (!(object instanceof UnpSharedSessionBean)) {
                writer.println("Session03 FAILED - unpShared attribute class "
                               + object.getClass().getName());
                ok = false;
            } else {
                ussb = (UnpSharedSessionBean) object;
                String value = ussb.getStringProperty();
                if (!"Session01".equals(value)) {
                    writer.println("Session03 FAILED - unpShared property ="
                                   + value);
                    ok = false;
                } else {
                    session.removeAttribute("unpSharedSessionBean");
                    String lifecycle = ssb.getLifecycle();
                    if (!"/vb/swp/sda/vu".equals(lifecycle)) {
                        writer.println("Session03 FAILED - unpShared lifecycle ="
                                       + lifecycle);
                        ok = false;
                    }
                }
            }
        }

        // Retrieve and validate the unshared session bean
        UnsharedSessionBean usb = null;
        if (ok) {
            Object object = session.getAttribute("unsharedSessionBean");
            if (object == null) {
                writer.println("Session03 FAILED - Cannot retrieve usb");
                ok = false;
            } else if (!(object instanceof UnsharedSessionBean)) {
                writer.println("Session03 FAILED - Unshared attribute class "
                               + object.getClass().getName());
                ok = false;
            } else {
                usb = (UnsharedSessionBean) object;
                String value = usb.getStringProperty();
                if (!"Session01".equals(value)) {
                    writer.println("Session03 FAILED - Unshared property = "
                                   + value);
                    ok = false;
                } else {
                    session.removeAttribute("unsharedSessionBean");
                    String lifecycle = usb.getLifecycle();
                    if (!"/vb/swp/sda/vu".equals(lifecycle)) {
                        writer.println("Session03 FAILED - Unshared lifecycle"
                                       + " = " + lifecycle);
                        ok = false;
                    }
                }
            }
        }


        // Report success if everything is still ok
        if (ok)
            writer.println("Session03 PASSED");
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
