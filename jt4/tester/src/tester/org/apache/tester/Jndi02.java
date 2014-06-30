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
 * Positive test for looking up environment entries from the naming context
 * provided by the servlet container.  The looked-up values are initialized
 * via <code>&lt;env-entry&gt;</code> elements in the web application
 * deployment descriptor.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/08/16 00:01:34 $
 */

public class Jndi02 extends HttpServlet {

    // Names of the known <env-entry> elements
    String names[] =
    { "booleanEntry", "byteEntry", "doubleEntry", "floatEntry",
      "integerEntry", "longEntry", "stringEntry" };


    // Reference some application classes for the first time in destroy()
    // and log the results
    public void destroy() {

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

        // Validate the booleanEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("booleanEntry");
                Boolean booleanValue = (Boolean) value;
                if (!(booleanValue.booleanValue() == true)) {
                    sb.append("  booleanValue is ");
                    sb.append(booleanValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  booleanValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  booleanValue is missing.");
        } catch (NamingException e) {
            log("Get booleanValue", e);
            sb.append("  Cannot get booleanValue.");
        }

        // Validate the byteEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("byteEntry");
                Byte byteValue = (Byte) value;
                if (!(byteValue.byteValue() == 123)) {
                    sb.append("  byteValue is ");
                    sb.append(byteValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  byteValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  byteValue is missing.");
        } catch (NamingException e) {
            log("Get byteValue", e);
            sb.append("  Cannot get byteValue.");
        }

        // Validate the doubleEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("doubleEntry");
                Double doubleValue = (Double) value;
                if (!(doubleValue.doubleValue() == 123.45)) {
                    sb.append("  doubleValue is ");
                    sb.append(doubleValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  doubleValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  doubleValue is missing.");
        } catch (NamingException e) {
            log("Get doubleValue", e);
            sb.append("  Cannot get doubleValue.");
        }

        // Validate the floatEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("floatEntry");
                Float floatValue = (Float) value;
                float difference = floatValue.floatValue() - ((float) 54.32);
                if ((difference < ((float) -0.01)) ||
                    (difference > ((float)  0.01))) {
                    sb.append("  floatValue is ");
                    sb.append(floatValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  floatValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  floatValue is missing.");
        } catch (NamingException e) {
            log("Get floatValue", e);
            sb.append("  Cannot get floatValue.");
        }

        // Validate the integerEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("integerEntry");
                Integer integerValue = (Integer) value;
                if (!(integerValue.intValue() == 12345)) {
                    sb.append("  integerValue is ");
                    sb.append(integerValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  integerValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  integerValue is missing.");
        } catch (NamingException e) {
            log("Get integerValue", e);
            sb.append("  Cannot get integerValue.");
        }

        // Validate the longEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("longEntry");
                Long longValue = (Long) value;
                if (!(longValue.longValue() == 54321)) {
                    sb.append("  longValue is ");
                    sb.append(longValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  longValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  longValue is missing.");
        } catch (NamingException e) {
            log("Get longValue", e);
            sb.append("  Cannot get longValue.");
        }

        // Validate the stringEntry environment entry
        try {
            if (ok) {
                value = envContext.lookup("stringEntry");
                String stringValue = (String) value;
                if (!"String Value".equals(stringValue)) {
                    sb.append("  stringValue is ");
                    sb.append(stringValue);
                    sb.append(".");
                }
            }
        } catch (ClassCastException e) {
            sb.append("  stringValue class is ");
            sb.append(value.getClass().getName());
            sb.append(".");                      
        } catch (NullPointerException e) {
            sb.append("  stringValue is missing.");
        } catch (NamingException e) {
            log("Get stringValue", e);
            sb.append("  Cannot get stringValue.");
        }

        // Validate that we can enumerate the contents of our environment
        try {
            if (ok) {
                int counts[] = new int[names.length];
                for (int i = 0; i < names.length; i++)
                    counts[i] = 0;
                NamingEnumeration enum =
                    initContext.listBindings("java:comp/env");
                while (enum.hasMore()) {
                    Binding binding = (Binding) enum.next();
                    String name = binding.getName();
                    boolean found = false;
                    for (int i = 0; i < names.length; i++) {
                        if (name.equals(names[i])) {
                            counts[i]++;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        StaticLogger.write("Found binding for '" + name + "'");
                }
                for (int i = 0; i < names.length; i++) {
                    if (counts[i] < 1) {
                        sb.append("  Missing binding for ");
                        sb.append(names[i]);
                        sb.append(".");
                    } else if (counts[i] > 1) {
                        sb.append("  Found ");
                        sb.append(counts[i]);
                        sb.append(" bindings for ");
                        sb.append(names[i]);
                        sb.append(".");
                    }
                }
            }
        } catch (NamingException e) {
            log("Enumerate envContext", e);
            sb.append("  Cannot enumerate envContext");
        }

        // Report our ultimate success or failure
        if (sb.length() < 1)
            writer.println("Jndi02 PASSED");
        else {
            writer.print("Jndi02 FAILED -");
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
