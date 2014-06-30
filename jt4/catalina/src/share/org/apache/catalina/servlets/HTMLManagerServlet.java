/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/servlets/HTMLManagerServlet.java,v 1.3 2001/07/22 20:25:11 pier Exp $
 * $Revision: 1.3 $
 * $Date: 2001/07/22 20:25:11 $
 *
 * ====================================================================
 *
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.servlets;

import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.catalina.Context;
import org.apache.catalina.Globals;

/**
 * Servlet that enables remote management of the web applications installed
 * within the same virtual host as this web application is.  Normally, this
 * functionality will be protected by a security constraint in the web
 * application deployment descriptor.  However, this requirement can be
 * relaxed during testing.
 * <p>
 * The difference between this <code>ManagerServlet</code> and this
 * Servlet is that this Servlet prints out a HTML interface which
 * makes it easier to administrate.
 * <p>
 * However if you use a software that parses the output of
 * <code>ManagerServlet</code you won't be able to upgrade
 * to this Servlet since the output are not in the
 * same format ar from <code>ManagerServlet</code>
 *
 * @author Bip Thelin
 * @version $Revision: 1.3 $, $Date: 2001/07/22 20:25:11 $
 * @see ManagerServlet
 */

public final class HTMLManagerServlet extends ManagerServlet {

    // --------------------------------------------------------- Public Methods

    /**
     * Process a GET request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    public void doGet(HttpServletRequest request,
                      HttpServletResponse response)
        throws IOException, ServletException {

        // Identify the request parameters that we need
        String command = request.getPathInfo();

        String path = request.getParameter("path");
        String war = request.getParameter("war");

        // Prepare our output writer to generate the response message
        response.setContentType("text/html");
        PrintWriter writer = response.getWriter();

        // Process the requested command
        if (command == null) {
            response.sendRedirect(request.getRequestURI()+"/list");
        } else if (command.equals("/install")) {
            install(writer, path, war);
        } else if (command.equals("/list")) {
            list(writer);
        } else if (command.equals("/reload")) {
            reload(writer, path);
        } else if (command.equals("/remove")) {
            remove(writer, path);
        } else if (command.equals("/sessions")) {
            sessions(writer, path);
        } else if (command.equals("/start")) {
            start(writer, path);
        } else if (command.equals("/stop")) {
            stop(writer, path);
        } else {
            writer.println(sm.getString("managerServlet.unknownCommand",
                                        command));
        }

        // Finish up the response
        writer.flush();
        writer.close();
    }

    /**
     * Render a HTML list of the currently
     * active Contexts in our virtual host.
     *
     * @param writer Writer to render to
     */
    public void list(PrintWriter writer) {

        if (debug >= 1)
            log("list: Listing contexts for virtual host '" +
                deployer.getName() + "'");

        writer.print("<html>\n<head>\n<title>");
        writer.print("Listed applications for virtual host ");
        writer.print(deployer.getName()+"</title>\n</head>\n");
        writer.print("<body bgcolor=\"#FFFFFF\">\n");
        writer.print("<form method=\"get\" action=\"install\">");
        writer.print("<table cellspacing=\"0\"");
        writer.print("cellpadding=\"3\" align=\"center\">\n");
        writer.print("<tr><td colspan=\"3\"><font size=\"+2\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Listed applications for virtual host ");
        writer.print(deployer.getName()+"</strong>\n</font></td>\n</tr>\n");
        writer.print("<tr bgcolor=\"#CCCCCC\">\n");
        writer.print("<td align=\"left\"><font size=\"+1\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Path</strong></font>\n</td>");
        writer.print("<td align=\"center\"><font size=\"+1\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Status</strong></font></td>\n");
        writer.print("<td align=\"right\"><font size=\"+1\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Sessions</strong></font></td>\n");
        writer.print("<td align=\"right\"><font size=\"+1\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Reload</strong></font></td>\n");
        writer.print("<td align=\"right\"><font size=\"+1\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print("<strong>Remove</strong></font></td>\n</tr>\n");

        String contextPaths[] = deployer.findDeployedApps();

        for (int i = 0; i < contextPaths.length; i++) {
            Context context = deployer.findDeployedApp(contextPaths[i]);
            String displayPath = contextPaths[i];
            if( displayPath.equals("") )
                displayPath = "/";
            if (context != null ) {
                writer.print("<tr bgcolor=\"#FFFFFF\">\n");
                writer.print("<td align=\"left\"><font size=\"1\"");
                writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
                writer.print(displayPath+"</font>\n</td>");
                writer.print("<td align=\"center\"><font size=\"1\"");
                writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");

                if (context.getAvailable()) {
                    writer.print("running / <a href=\"stop?path="+displayPath);
                    writer.print("\">stop</a>");
                } else {
                    writer.print("<a href=\"start?path="+displayPath);
                    writer.print("\">start</a> / stopped");
                }

                writer.print("</font></td>\n");
                writer.print("<td align=\"right\"><font size=\"1\"");
                writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
                writer.print("<a href=\"sessions?path="+displayPath+"\">");
                writer.print(context.getManager().findSessions().length);
                writer.print("</a></font></td>\n");
                writer.print("<td align=\"right\"><font size=\"1\"");
                writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");

                if (context.getAvailable()) {
                    writer.print("<a href=\"reload?path="+displayPath+"\">");
                    writer.print("Reload &#187;");
                    writer.print("</a></font></td>\n");
                } else {
                    writer.print("-</font></td>");
                }

                writer.print("<td align=\"right\"><font size=\"1\"");
                writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
                writer.print("<a href=\"remove?path="+displayPath+"\">");
                writer.print("Remove &#187;");
                writer.print("</a></font></td>\n</tr>\n");
            }
        }

        writer.print("<tr><td colspan=\"5\">&nbsp;</td></tr>");
        writer.print("<tr><td>");
        writer.print("<font size=\"2\"");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">");
        writer.print("<b>Install a webapp</b></font></td>\n");
        writer.print("<td><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">");
        writer.print("Path:&nbsp;</font>");
        writer.print("<input type=\"text\" name=\"path\" size=\"25\"></td>");
        writer.print("<td><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">");
        writer.print("War-URL:&nbsp;</font>");
        writer.print("<input type=\"text\" name=\"war\" size=\"25\"></td>");
        writer.print("<td colspan=\"2\">");
        writer.print("&nbsp;<input type=\"submit\" value=\"install\"></td></tr>\n");
        writer.print("<tr><td colspan=\"5\" bgcolor=\"#CCCCCC\"><font size=\"-1\" ");
        writer.print(" face=\"Arial, Helvetica, sans-serif\">\n");
        writer.print(Globals.SERVER_INFO+"</font></td>\n</tr>\n");
        writer.print("</table>\n</form>\n</body>\n</html>");
    }

    /**
     * Reload the web application at the specified context path.
     *
     * @param writer Writer to render to
     * @param path Context path of the application to be restarted
     */
    protected void reload(PrintWriter writer, String path) {

        if (debug >= 1)
            log("restart: Reloading web application at '" + path + "'");

        if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
            writer.println(sm.getString("managerServlet.invalidPath", path));
            list(writer);

            return;
        }

        String displayPath = path;

        if( path.equals("/") )
            path = "";

        try {
            Context context = deployer.findDeployedApp(path);

            if (context == null) {
                writer.println(sm.getString("managerServlet.noContext", displayPath));
                list(writer);

                return;
            }

            context.reload();
            list(writer);
        } catch (Throwable t) {
            getServletContext().log("ManagerServlet.reload[" + displayPath + "]", t);
            writer.println(sm.getString("managerServlet.exception",
                                        t.toString()));
        }
    }

    /**
     * Remove the web application at the specified context path.
     *
     * @param writer Writer to render to
     * @param path Context path of the application to be removed
     */
    protected void remove(PrintWriter writer, String path) {

        if (debug >= 1)
            log("remove: Removing web application at '" + path + "'");

        if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
            writer.println(sm.getString("managerServlet.invalidPath", path));
            list(writer);

            return;
        }

        String displayPath = path;

        if( path.equals("/") )
            path = "";

        try {
            Context context = deployer.findDeployedApp(path);

            if (context == null) {
                writer.println(sm.getString("managerServlet.noContext", displayPath));
                list(writer);

                return;
            }

            deployer.remove(path);
            list(writer);
        } catch (Throwable t) {
            getServletContext().log("ManagerServlet.remove[" + displayPath + "]",
                                    t);
            writer.println(sm.getString("managerServlet.exception",
                                        t.toString()));
        }
    }

    /**
     * Start the web application at the specified context path.
     *
     * @param writer Writer to render to
     * @param path Context path of the application to be started
     */
    public void start(PrintWriter writer, String path) {

        if (debug >= 1)
            log("start: Starting web application at '" + path + "'");

        if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
            writer.println(sm.getString("managerServlet.invalidPath", path));
            list(writer);

            return;
        }

        String displayPath = path;

        if( path.equals("/") )
            path = "";

        try {
            Context context = deployer.findDeployedApp(path);

            if (context == null) {
                writer.println(sm.getString("managerServlet.noContext", displayPath));
                list(writer);

                return;
            }

            deployer.start(path);
            list(writer);
        } catch (Throwable t) {
            getServletContext().log("ManagerServlet.start[" + displayPath + "]",
                                    t);
            writer.println(sm.getString("managerServlet.exception",
                                        t.toString()));
        }
    }

    /**
     * Stop the web application at the specified context path.
     *
     * @param writer Writer to render to
     * @param path Context path of the application to be stopped
     */
    protected void stop(PrintWriter writer, String path) {

        if (debug >= 1)
            log("stop: Stopping web application at '" + path + "'");

        if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
            writer.println(sm.getString("managerServlet.invalidPath", path));
            list(writer);

            return;
        }

        String displayPath = path;

        if( path.equals("/") )
            path = "";

        try {
            Context context = deployer.findDeployedApp(path);

            if (context == null) {
                writer.println(sm.getString("managerServlet.noContext", displayPath));
                list(writer);

                return;
            }

            deployer.stop(path);
            list(writer);
        } catch (Throwable t) {
            getServletContext().log("ManagerServlet.stop[" + displayPath + "]",
                                    t);
            writer.println(sm.getString("managerServlet.exception",
                                        t.toString()));
        }
    }
}
