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
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import javax.servlet.*;
import javax.servlet.http.*;

/**
 * Test aggregation of query string and POST parameters.  According to
 * Servlet 2.3 PFD, Section 4.1, all such parameters should be aggregated,
 * and if there are duplicate parameter names from both sources, the
 * parameter value(s) from the query string should appear first in the
 * values returned by request.getParameterValues().
 * <p>
 * This test is the same as Aggregate01, except that it uses the new
 * <code>getParameterMap()</code> method to retrieve parameter values.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/01/27 01:49:14 $
 */

public class Aggregate02 extends HttpServlet {

    public void doPost(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Accumulate any errors that are noticed
        StringBuffer errors = new StringBuffer();
        Map map = request.getParameterMap();
        if (map == null) {
            errors.append("  No parameter map returned.");
            map = new HashMap();
        }
        String values[] = (String[]) map.get("a");
        if (values == null)
            errors.append("  Received no parameter values for 'a'.");
        else if (values.length != 2)
            errors.append("  Received " + values.length +
                          " parameter values for 'a' instead of 2.");
        else {
            if (!"1".equals(values[0]))
                errors.append("  First value for 'a' was '" + values[0] +
                              "' instead of '1'.");
            if (!"2".equals(values[1]))
                errors.append("  Second value for 'a' was '" + values[1] +
                              "' instead of '2'.");
        }
        values = (String[]) map.get("b");
        if (values == null)
            errors.append("  Received no parameter values for 'b'.");
        else if (values.length != 1)
            errors.append("  Received " + values.length +
                          " parameter values for 'b' instead of 1.");
        else {
            if (!"3".equals(values[0]))
                errors.append("  Value for 'b' was '" + values[0] +
                              "' instead of '3'.");
        }
        Iterator names = map.keySet().iterator();
        while (names.hasNext()) {
            String name = (String) names.next();
            if ("a".equals(name))
                continue;
            if ("b".equals(name))
                continue;
            errors.append("  Received parameter '" + name + "'.");
        }

        // Report the results
        if (errors.length() < 1)
            writer.println("Aggregate02 PASSED");
        else
            writer.println("Aggregate02 FAILED -" + errors.toString());
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
