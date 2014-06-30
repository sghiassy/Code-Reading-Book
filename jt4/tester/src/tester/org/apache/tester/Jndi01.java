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
import javax.naming.Binding;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.servlet.*;
import javax.servlet.http.*;
import org.apache.tester.SessionBean;
import org.apache.tester.shared.SharedSessionBean;
import org.apache.tester.unpshared.UnpSharedSessionBean;
import org.apache.tester.unshared.UnsharedSessionBean;


/**
 * Negative test for ensuring that the naming context provided by the servlet
 * container is immutable.  No attempt to add, modify, or delete any binding
 * should succeed.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.5 $ $Date: 2001/08/16 00:01:34 $
 */

public class Jndi01 extends HttpServlet {

    public void init() throws ServletException {

        // Access the naming context from init()
        Context ctx = null;
        try {
            ctx = new InitialContext();
            ctx.lookup("java:/comp");
            log("initialized successfully in init()");
        } catch (NamingException e) {
            e.printStackTrace();
            log("Cannot create context in init()", e);
            throw new ServletException(e);
        }

        // Access some application beans from init()

        try {
            SessionBean sb = new SessionBean();
            log("OK Accessing SessionBean");
        } catch (Throwable t) {
            log("FAIL Accessing SessionBean", t);
        }

        try {
            SharedSessionBean sb = new SharedSessionBean();
            log("OK Accessing SharedSessionBean");
        } catch (Throwable t) {
            log("FAIL Accessing SharedSessionBean", t);
        }

        try {
            UnpSharedSessionBean sb = new UnpSharedSessionBean();
            log("OK Accessing UnpSharedSessionBean");
        } catch (Throwable t) {
            log("FAIL Accessing UnpSharedSessionBean", t);
        }

        try {
            UnsharedSessionBean sb = new UnsharedSessionBean();
            log("OK Accessing UnsharedSessionBean");
        } catch (Throwable t) {
            log("FAIL Accessing UnsharedSessionBean", t);
        }

    }

    public void destroy() {
        Context ctx = null;
        try {
            ctx = new InitialContext();
            ctx.lookup("java:/comp");
            log("initialized successfully in destroy()");
        } catch (NamingException e) {
            e.printStackTrace();
            log("Cannot create context in destroy()", e);
        }
    }

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        // Prepare to render our output
        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();
        StringBuffer sb = new StringBuffer();
        boolean ok = true;
        Object value = null;

        // Look up the initial context provided by our servlet container
        Context initContext = null;
        try {
            initContext = new InitialContext();
        } catch (NamingException e) {
            log("Create initContext", e);
            sb.append("  Cannot create initContext.");
            ok = false;
        }

        // Look up the environment context provided to our web application
        Context envContext = null;
        try {
            if (ok) {
                value = initContext.lookup("java:comp/env");
                envContext = (Context) value;
                if (envContext == null) {
                    sb.append("  Missing envContext.");
                    ok = false;
                }
            }
        } catch (ClassCastException e) {
            sb.append("  envContext class is ");
            sb.append(value.getClass().getName());
            sb.append(".");
            ok = false;
        } catch (NamingException e) {
            log("Create envContext", e);
            sb.append("  Cannot create envContext.");
            ok = false;
        }

        // Attempt to add a new binding to our environment context
        try {
            if (ok) {
                envContext.bind("newEntry", "New Value");
                sb.append("  Allowed bind().");
                value = envContext.lookup("newEntry");
                if (value != null)
                    sb.append("  Allowed lookup() of added entry.");
            }
        } catch (Throwable e) {
            log("Add binding", e);
        }

        // Attempt to change the value of an existing binding
        try {
            if (ok) {
                envContext.rebind("stringEntry", "Changed Value");
                sb.append("  Allowed rebind().");
                value = envContext.lookup("stringEntry");
                if ((value != null) &&
                    (value instanceof String) &&
                    "Changed Value".equals((String) value))
                    sb.append("  Allowed lookup() of changed entry.");
            }
        } catch (Throwable e) {
            log("Change binding", e);
        }

        // Attempt to delete an existing binding
        try {
            if (ok) {
                envContext.unbind("byteEntry");
                sb.append("  Allowed unbind().");
                value = envContext.lookup("byteEntry");
                if (value == null)
                    sb.append("  Allowed unbind of deleted entry.");
            }
        } catch (Throwable e) {
            log("Delete binding", e);
        }

        // Report our ultimate success or failure
        if (sb.length() < 1)
            writer.println("Jndi01 PASSED");
        else {
            writer.print("Jndi01 FAILED -");
            writer.println(sb);
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
