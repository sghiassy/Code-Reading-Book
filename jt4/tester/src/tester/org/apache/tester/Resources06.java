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
import java.net.MalformedURLException;
import java.net.URL;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;


/**
 * Positive test for <code>getResourcePaths()</code>, which will specify the
 * directory path indicated by the <strong>path</strong> request parameter.
 * For known paths, at least the known set of included resources must be
 * found in order to pass.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.2 $ $Date: 2001/04/25 20:30:42 $
 */

public class Resources06 extends HttpServlet {

    // Resource Lists (incomplete) for known directory paths
    String rootPaths[] =
    { "/ErrorPage06.html", "/ErrorPage06.jsp", "/Forward01.txt",
      "/Include01.txt", "/WEB-INF/", "/Xerces00.jsp", "/Xerces01.xml",
      "/Xerces02.jsp", "/golden/", "/includeme.txt", "/index.shtml",
      "/ssidir/" };

    String goldenPaths[] =
    { "/golden/Golden01.txt", "/golden/SSIConfig01.txt",
      "/golden/SSIConfig03.txt", "/golden/SSIFsize02.txt",
      "/golden/SSIInclude01.txt", "/golden/SSIInclude02.txt",
      "/golden/Session05.txt" };

    String webinfPaths[] =
    { "/WEB-INF/classes/", "/WEB-INF/lib/", "/WEB-INF/web.xml" };


    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        // Prepare our output writer
        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Identify our configuration parameters
        StringBuffer sb = new StringBuffer();
        String path = request.getParameter("path");
        if (path == null)
            path = "/";
        String paths[] = null;
        if ("/".equals(path))
            paths = rootPaths;
        else if ("/golden".equals(path))
            paths = goldenPaths;
        else if ("/WEB-INF".equals(path))
            paths = webinfPaths;
        else {
            sb.append(" Unknown path '");
            sb.append(path);
            sb.append("'/");
        }
        int counts[] = null;
        if (paths != null)
            counts = new int[paths.length];

        // Request the set of resources in the specified path
        StaticLogger.write("Processing path '" + path + "'");
        String first = null;
        Set set = getServletContext().getResourcePaths(path);
        if (set == null) {
            sb.append(" No resources returned/");
            set = new HashSet();
        }

        // Count the occurrences of the resources we know about
        Iterator resources = set.iterator();
        while (resources.hasNext()) {
            String resource = (String) resources.next();
            if (first == null)
                first = resource;
            StaticLogger.write("Found resource '" + resource + "'");
            for (int i = 0; i < paths.length; i++) {
                if (paths[i].equals(resource)) {
                    counts[i]++;
                    break;
                }
            }
        }

        // Report on any missing or duplicated resources
        for (int i = 0; i < paths.length; i++) {
            if (counts[i] < 1) {
                sb.append(" Missing resource '");
                sb.append(paths[i]);
                sb.append("'/");
            } else if (counts[i] > 2) {
                sb.append(" Resource '");
                sb.append(paths[i]);
                sb.append("' occurred ");
                sb.append(counts[i]);
                sb.append(" times/");
            }
        }

        // Verify that the returned set is immutable
        try {
            String newElement = "NEW FOO";
            set.add(newElement);
            if (set.contains(newElement))
              sb.append(" Set allowed add()/");
        } catch (Throwable t) {
            ;
        }
        try {
            if (first != null) {
                set.remove(first);
                if (!set.contains(first))
                  sb.append(" Set allowed remove()/");
            }
        } catch (Throwable t) {
            ;
        }
        try {
            set.clear();
            if (set.size() == 0)
                sb.append(" Set allowed clear()/");
        } catch (Throwable t) {
            ;
        }


        // Report any failures we have encountered
        if (sb.length() > 0) {
            writer.print("Resources06 FAILED -");
            writer.println(sb.toString());
        } else {
            writer.println("Resources06 PASSED");
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
