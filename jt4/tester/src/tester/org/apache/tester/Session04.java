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
 * Part 4 of Session Tests.  Ensures that there is an existing session, and
 * that the requested session information matches it.  Also, ensure that we
 * can invalidate this session and create a new one (with a different session
 * identifier) while processing this request.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/04/11 04:31:08 $
 */

public class Session04 extends HttpServlet {

    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws IOException, ServletException {

        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();

        // Ensure that there is a current session
        StringBuffer results = new StringBuffer();
        HttpSession oldSession = request.getSession(false);
        if (oldSession == null)
            results.append(" No existing session/");

        // Acquire the session identifier of the old session
        String oldSessionId = null;
        if (oldSession != null) {
            try {
                oldSessionId = oldSession.getId();
            } catch (IllegalStateException e) {
                results.append(" Old session is expired/");
            }
        }

        // Match against the requested session identifier
        String requestedSessionId = null;
        if (oldSessionId != null) {
            requestedSessionId = request.getRequestedSessionId();
            if (requestedSessionId == null) {
                results.append(" No requested session id/");
            } else {
                if (!request.isRequestedSessionIdValid())
                    results.append(" Requested session id is not valid/");
                if (!oldSessionId.equals(requestedSessionId)) {
                    results.append(" Requested session=");
                    results.append(requestedSessionId);
                    results.append(" Old session=");
                    results.append(oldSessionId);
                    results.append("/");
                }
            }
        }

        // Verify that we received the requested session identifier correctly
        if (requestedSessionId != null) {
            if (!request.isRequestedSessionIdFromCookie())
                results.append(" Requested session not from cookie/");
            if (request.isRequestedSessionIdFromURL())
                results.append(" Requested session from URL/");
        }

        // Verify that we can create an attribute in the old session
        if (oldSession != null) {
            SessionBean bean = new SessionBean();
            bean.setStringProperty("Session04");
            oldSession.setAttribute("sessionBean", bean);
        }

        // Verify that we can invalidate the old session
        if (oldSession != null) {
            try {
                oldSession.invalidate();
            } catch (IllegalStateException e) {
                results.append(" Old session is already invalidated/");
            }
        }

        // Verify that we can create a new session
        HttpSession newSession = request.getSession(true);
        if (newSession == null) {
            results.append(" Cannot create new session/");
        } else {
            String newSessionId = null;
            try {
                newSessionId = newSession.getId();
            } catch (IllegalStateException e) {
                results.append(" New session is already invalidated/");
            }
            if ((oldSession != null) && (newSession != null)) {
                if (oldSession == newSession)
                    results.append(" oldSession == newSession/");
                if (oldSession.equals(newSession))
                    results.append(" oldSession equals newSession/");
            }
            if ((oldSessionId != null) && (newSessionId != null) &&
                oldSessionId.equals(newSessionId)) {
                results.append(" New session id = old session id/");
            }
        }

        // Verify that the old session's attribute did not carry forward
        if (newSession != null) {
            SessionBean bean =
                (SessionBean) newSession.getAttribute("sessionBean");
            if (bean != null)
                results.append(" New session has attribute already/");
        }

        // Report success if everything is still ok
        if (results.length() == 0)
            writer.println("Session04 PASSED");
        else {
            writer.print("Session04 FAILED -");
            writer.println(results.toString());
        }
        while (true) {
            String message = StaticLogger.read();
            if (message == null)
                break;
            writer.println(message);
        }
        StaticLogger.reset();

    }

}
