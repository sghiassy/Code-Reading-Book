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

/**
 * Positive test for handling exceptions thrown by an included servlet.
 * Request parameter <strong>exception</strong> is used to indicate the type
 * of exception that should be thrown, which must be one of
 * <code>IOException</code>, <code>ServletException</code>, or
 * <code>ServletException</code>.  According to the spec, any exceptions of
 * these types should be propogated back to the caller unchanged.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/02/06 17:16:23 $
 */

public class Include02 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        boolean ok = true;
        response.setContentType("text/plain");
	PrintWriter writer = response.getWriter();
        RequestDispatcher rd =
            getServletContext().getRequestDispatcher("/Include02a");
        if (rd == null) {
            writer.println("Include02 FAILED - No RequestDispatcher returned");
	    ok = false;
        }
	String type = request.getParameter("exception");
	if (ok) {
	    if (type == null) {
	        writer.println("Include02 FAILED - No exception type specified");
		ok = false;
	    } else if (!type.equals("IOException") &&
		       !type.equals("ServletException") &&
		       !type.equals("NullPointerException")) {
	        writer.println("Include02 FAILED - Invalid exception type " +
			       type + " requested");
		ok = false;
	    }
	}

	IOException ioException = null;
	ServletException servletException = null;
	Throwable throwable = null;
	try {
            if (ok)
                rd.include(request, response);
	} catch (IOException e) {
	    ioException = e;
	} catch (ServletException e) {
	    servletException = e;
	} catch (Throwable e) {
	    throwable = e;
	}

	if (ok) {
            if (type.equals("IOException")) {
                if (ioException == null) {
		    writer.println("Include02 FAILED - No IOException thrown");
		    ok = false;
		} else {
		    String message = ioException.getMessage();
		    if (!"Include02 IOException".equals(message)) {
		        writer.println("Include02 FAILED - IOException was " +
				       message);
			ok = false;
		    }
		}
	    } else if (type.equals("ServletException")) {
                if (servletException == null) {
		    writer.println("Include02 FAILED - No ServletException thrown");
		    ok = false;
		} else {
		    String message = servletException.getMessage();
		    if (!"Include02 ServletException".equals(message)) {
		        writer.println("Include02 FAILED - ServletException was " +
				       message);
			ok = false;
		    }
		}
	    } else if (type.equals("NullPointerException")) {
                if (throwable == null) {
		    writer.println("Include02 FAILED - No NullPointerException thrown");
		    ok = false;
		} else if (!(throwable instanceof NullPointerException)) {
		    writer.println("Include02 FAILED - Thrown Exception was " +
				   throwable.getClass().getName());
		    ok = false;
		} else {
		    String message = throwable.getMessage();
		    if (!"Include02 NullPointerException".equals(message)) {
		        writer.println("Include02 FAILED - NullPointerException was " +
				       message);
			ok = false;
		    }
		}
	    }
	}

	if (ok)
	    writer.println("Include02 PASSED");
        StaticLogger.reset();

    }

}
