/*
 * SsiMediator.java
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/util/ssi/SsiMediator.java,v 1.7 2001/07/22 20:25:15 pier Exp $
 * $Revision: 1.7 $
 * $Date: 2001/07/22 20:25:15 $
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

package org.apache.catalina.util.ssi;

import java.io.OutputStream;
import java.util.Hashtable;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.catalina.util.RequestUtil;
import org.apache.catalina.util.DateTool;
import org.apache.catalina.util.ssi.SsiConfig;
import org.apache.catalina.util.ssi.SsiCommand;
import org.apache.catalina.util.ssi.SsiInclude;
import org.apache.catalina.util.ssi.SsiEcho;
import org.apache.catalina.util.ssi.SsiFsize;
import org.apache.catalina.util.ssi.SsiFlastmod;
import org.apache.catalina.util.ssi.SsiExec;

/**
 * @author Bip Thelin
 * @author Amy Roh
 * @version $Revision: 1.7 $, $Date: 2001/07/22 20:25:15 $
 *
 */
public class SsiMediator {

    /**
     * The Servervariables associated with this request
     */
    protected static Hashtable serverVariables =
        new Hashtable(17);

    /**
     * The Commands associated with SSI.
     */
    protected static Hashtable ssiCommands = null;

    /**
     * The HttpServletResponse associated with this request
     */
    protected static HttpServletRequest req = null;

    /**
     * The HttpServletResponse associated with this request.
     */
    protected static HttpServletResponse res = null;

    /**
     * The outputStream to use
     */
    protected static OutputStream out = null;

    /**
     * The ServletContext associated with this request
     */
    protected static ServletContext servletContext = null;

    /**
     * The original ServletContext
     */
    protected static ServletContext origServletContext = null;

    /**
     * The contextPath for this request
     */
    protected static String contextPath = null;

    /**
     * The relative path for this request
     */
    protected static String relpath = "/";

    /**
     * The path for this request
     */
    protected static String path = new String();

    /**
     * The debug level for this component
     */
    protected static int debug = 0;

    /**
     * How this request is being treated
     */
    protected static boolean isVirtualWebappRelative = false;

    /**
     * Initialize our pool of SsiCommands
     */
    static {
        ssiCommands = new Hashtable(6);
        ssiCommands.put("config", new SsiConfig());
        ssiCommands.put("include", new SsiInclude());
        ssiCommands.put("echo", new SsiEcho());
        ssiCommands.put("fsize", new SsiFsize());
        ssiCommands.put("flastmod", new SsiFlastmod());
        ssiCommands.put("exec", new SsiExec());
    }

    public SsiMediator() {}

    /**
     * Initialize and set up out enviroment, called from SsiInvokerServlet
     *
     * @param req The HttpServletRequest to use
     * @param res The HttpServletResponse to use
     * @param out The OutputStream to use
     * @param servletContext The ServletContext to use
     * @param path The current path
     * @param isVirtualWebappRelative How this request is being treated
     */
    public SsiMediator(HttpServletRequest req,
                       HttpServletResponse res,
                       OutputStream out,
                       ServletContext servletContext,
                       int debug,
                       String path,
                       boolean isVirtualWebappRelative) {
        this.debug = debug;
        flush(req, res, out, servletContext, path, isVirtualWebappRelative);
    }

    /**
     * Get the SsiCommand
     *
     * @param cmd The SsiCommand to get
     * @return SsiCommand
     */
    public final SsiCommand getCommand(String cmd) {
        return (SsiCommand)ssiCommands.get(cmd);
    }

    /**
     * Initialize and set up out enviroment, called from SsiInvokerServlet
     *
     * @param req The HttpServletRequest to use
     * @param res The HttpServletResponse to use
     * @param out The OutputStream to use
     * @param servletContext The ServletContext to use
     * @param path The current path
     * @param isVirtualWebappRelative How this request is being treated
     */
    public void flush(HttpServletRequest req,
                      HttpServletResponse res,
                      OutputStream out,
                      ServletContext servletContext,
                      String path,
                      boolean isVirtualWebappRelative) {
        this.req = req;
        this.res = res;
        this.out = out;
        this.servletContext = servletContext;
        this.origServletContext = servletContext;
        this.contextPath = req.getContextPath();
        this.path = path;
        this.relpath = path.substring(0, path.lastIndexOf('/')+1);
        this.isVirtualWebappRelative = isVirtualWebappRelative;
        int c=0;

        serverVariables.put("AUTH_TYPE",
                            nullToString(req.getAuthType()));
        serverVariables.put("CONTENT_LENGTH",
                            nullToString(((c=req.getContentLength())<=0)?
                                         null:
                                         (new Integer(c)).toString()));
        serverVariables.put("CONTENT_TYPE",
                            nullToString(req.getContentType()));
        serverVariables.put("GATEWAY_INTERFACE",
                            "CGI/1.1");
        serverVariables.put("PATH_INFO",
                            nullToString(req.getPathInfo()));
        serverVariables.put("PATH_TRANSLATED ",
                            nullToString(req.getPathTranslated()));
        serverVariables.put("QUERY_STRING",
                            nullToString(req.getQueryString()));
        serverVariables.put("REMOTE_ADDR",
                            nullToString(req.getRemoteAddr()));
        serverVariables.put("REMOTE_HOST",
                            nullToString(req.getRemoteHost()));
        serverVariables.put("REMOTE_IDENT", "");
        serverVariables.put("REMOTE_USER",
                            nullToString(req.getRemoteUser()));
        serverVariables.put("REQUEST_METHOD",
                            nullToString(req.getMethod()));
        serverVariables.put("SCRIPT_NAME",
                            nullToString(req.getServletPath()));
        serverVariables.put("SERVER_NAME",
                            nullToString(req.getServerName()));
        serverVariables.put("SERVER_PORT",
                            (new Integer(req.getServerPort())).toString());
        serverVariables.put("SERVER_PROTOCOL",
                            nullToString(req.getProtocol()));
        serverVariables.put("SERVER_SOFTWARE",
                            nullToString(servletContext.getServerInfo()));
        serverVariables.put("DOCUMENT_NAME",
                            nullToString(path.substring(path.lastIndexOf('/')+1,
                                                        path.length())));
        serverVariables.put("DOCUMENT_URI",
                            nullToString(path));
        serverVariables.put("QUERY_STRING_UNESCAPED",
                            nullToString(""));

        flushDate();
        ((SsiConfig)ssiCommands.get("config")).flush();
    }

    /**
     * Return the current error message
     *
     * @return The current error message
     */
    public byte[] getError() {
        return ((SsiConfig)ssiCommands.get("config")).getError();
    }

    /**
     * Flush the date and make us ready for a new request.
     *
     */
    protected void flushDate() {
        serverVariables.put("DATE_LOCAL",
                            timefmt(new Date()));
        serverVariables.put("DATE_GMT",
                            timefmt(getGMTDate()));
        serverVariables.put("LAST_MODIFIED",
                            ((SsiFlastmod)ssiCommands.get("flastmod")).getDate(path));
    }

    /**
     * Parse a Date according to the current settings.
     *
     * @param date The date to parse
     * @return The parsed date
     */
    protected String timefmt(Date date) {
        String pattern = ((SsiConfig)ssiCommands.get("config")).getTimefmt();
        DateFormat dateFormat = new SimpleDateFormat(pattern, DateTool.LOCALE_US);

        return dateFormat.format(date);
    }

    /**
     * Parse a Date according to the current settings.
     *
     * @param date The date to parse
     * @return The parsed date
     */
    protected String timefmt(String date) {
        DateFormat dateFormat = DateFormat.getDateTimeInstance();
        Date parsedDate = null;

        try {
            parsedDate = dateFormat.parse(date);
        } catch (ParseException e) {
            return new String(getError());
        }

        return timefmt(parsedDate);
    }

    /**
     * Get a server variable
     *
     * @param serverVar The server variable to get
     * @return The parsed result
     */
    protected String getServerVariable(String serverVar) {
        flushDate();

        if(serverVariables.get(serverVar)==null)
            return new String(getError());
        else
            return (String)serverVariables.get(serverVar);
    }

    /**
     * Return a path relative to either the webapp or the root("/")
     *
     * Example of valid paths:
     * "/test/path/test.file"
     * "../test/path/test.file"
     *
     * @param path Path to be normalized
     */
    protected String getVirtualPath(String path) {
        if (path == null)
            return null;

        // Create a place for the normalized path
        String normalized = path;

        if (normalized == null)
            return (null);

        // Normalize the slashes and add leading slash if necessary
        if (normalized.indexOf('\\') >= 0)
            normalized = normalized.replace('\\', '/');
        if (!normalized.startsWith("/"))
            normalized = relpath.concat(normalized);

        // Resolve occurrences of "//" in the normalized path
        while (true) {
            int index = normalized.indexOf("//");
            if (index < 0)
                break;
            normalized = normalized.substring(0, index) +
                normalized.substring(index + 1);
        }

        // Resolve occurrences of "/./" in the normalized path
        while (true) {
            int index = normalized.indexOf("/./");
            if (index < 0)
                break;
            normalized = normalized.substring(0, index) +
                normalized.substring(index + 2);
        }

        // Resolve occurrences of "/../" in the normalized path
        while (true) {
            int index = normalized.indexOf("/../");
            if (index < 0)
                break;
            if (index == 0)
                return (null);  // Trying to go outside our context
            int index2 = normalized.lastIndexOf('/', index - 1);
            normalized = normalized.substring(0, index2) +
                normalized.substring(index + 3);
        }

        if (!isVirtualWebappRelative) {
            // case of virtual="file.txt", "./file.txt", or dir/file.txt
            if ((!path.startsWith("/")) || (path.startsWith("./"))) {
                // handle as file in the current directory with original servletContext
                servletContext = origServletContext;
            }else if (path.indexOf('/', 1)==-1) {
                //root context
                servletContext = servletContext.getContext("/");
            } else if (!contextPath.equals("")) {
                //starts with the context path of this webapp
                if ((normalized !=null) && (normalized.startsWith(contextPath))) {
                    // strip off the context path
                    servletContext = servletContext.getContext(contextPath);
                    normalized = normalized.substring(contextPath.length());
                }
            } else if (normalized != null){
                // find which context is the right one to handle
                String context = normalized.substring(0, path.indexOf('/', 1));
                ServletContext sc = servletContext.getContext(context);
                if (sc!=null) {
                    servletContext = sc;
                    normalized = normalized.substring(context.length());
                }
            }
        }

        return (normalized);
    }

    /**
     * Return a path relative to the file being parsed, if they
     * try to use "../" or have a trailing "/" we return
     * <code>null</code> since that is not allowed within
     * a SSI file directive.
     *
     * Example of valid path:
     * "test/path/test.file"
     *
     * @param path Path to be normalized
     */
    protected String getFilePath(String path) {
        if (path == null)
            return null;

        // Create a place for the normalized path
        String normalized = path;

        servletContext = origServletContext;

        if (normalized == null)
            return (null);

        // Normalize the slashes
        if (normalized.indexOf('\\') >= 0)
            normalized = normalized.replace('\\', '/');

        // Resolve occurrences of "//" in the normalized path
        while (true) {
            int index = normalized.indexOf("//");
            if (index < 0)
                break;
            normalized = normalized.substring(0, index) +
                normalized.substring(index + 1);
        }

        // If it starts with a "/" or contains "../" we return <code>null</code>.
        if (normalized.startsWith("/") || normalized.indexOf("../") >= 0)
            return (null);

        // Return the normalized path that we have completed
        return (relpath.concat(normalized));
    }

    protected String getCGIPath(String path) {

        String cgibinStr = "/cgi-bin/";

        if (path == null)
            return null;

        if (!path.startsWith(cgibinStr)) {
            return null;
        } else {
            //normalized = normalized.substring(1, cgibinStr.length());
            //normalized = cgiPathPrefix + File.separator + normalized;
            path = "http://localhost:8080" + contextPath + path;
        }
        return (path);
    }

    protected String getCommandPath(String path) {

        String commandShellStr = "/bin/sh";

        if (path == null)
            return null;

        if (!path.startsWith("/"))
            path = "/" + path;
        path = commandShellStr + path;
        return (path);
    }

    /**
     * Check if the supplied string is <code>null</code> and
     * if so return an empty string.
     *
     * @param s a value of type 'String'
     * @return a value of type 'String'
     */
    private String nullToString(String s) {
        return s==null?"":s;
    }

    /**
     * Get the current Date GMT formated.
     *
     * @return The current GMT formatted date.
     */
    private String getGMTDate() {
        Date date = new Date();
        DateFormat dateFormat = DateFormat.getDateTimeInstance();

        dateFormat.setTimeZone(DateTool.GMT_ZONE);
        return dateFormat.format(date);
    }
}
