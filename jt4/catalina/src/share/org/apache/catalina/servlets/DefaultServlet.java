/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/servlets/DefaultServlet.java,v 1.39 2001/09/12 16:19:25 remm Exp $
 * $Revision: 1.39 $
 * $Date: 2001/09/12 16:19:25 $
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


import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Reader;
import java.io.InputStreamReader;
import java.io.Writer;
import java.io.OutputStreamWriter;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Enumeration;
import java.util.Vector;
import java.util.StringTokenizer;
import java.util.Locale;
import java.util.TimeZone;
import java.util.Hashtable;
import java.util.BitSet;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.ServletContext;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.naming.NamingException;
import javax.naming.InitialContext;
import javax.naming.Context;
import javax.naming.NamingEnumeration;
import javax.naming.NameClassPair;
import javax.naming.directory.DirContext;
import javax.naming.directory.Attribute;
import javax.naming.directory.Attributes;
import org.apache.naming.resources.Resource;
import org.apache.naming.resources.ResourceAttributes;
import org.apache.catalina.Globals;
import org.apache.catalina.util.MD5Encoder;
import org.apache.catalina.util.StringManager;
import org.apache.catalina.util.RequestUtil;


/**
 * The default resource-serving servlet for most web applications,
 * used to serve static resources such as HTML pages and images.
 *
 * @author Craig R. McClanahan
 * @author Remy Maucherat
 * @version $Revision: 1.39 $ $Date: 2001/09/12 16:19:25 $
 */

public class DefaultServlet
    extends HttpServlet {


    // ----------------------------------------------------- Instance Variables


    /**
     * The debugging detail level for this servlet.
     */
    protected int debug = 0;


    /**
     * The input buffer size to use when serving resources.
     */
    protected int input = 2048;


    /**
     * Should we generate directory listings when no welcome file is present?
     */
    protected boolean listings = true;


    /**
     * Read only flag. By default, it's set to true.
     */
    protected boolean readOnly = true;


    /**
     * The output buffer size to use when serving resources.
     */
    protected int output = 2048;


    /**
     * The set of welcome files for this web application
     */
    protected String welcomes[] = new String[0];


    /**
     * MD5 message digest provider.
     */
    protected static MessageDigest md5Helper;


    /**
     * The MD5 helper object for this class.
     */
    protected static final MD5Encoder md5Encoder = new MD5Encoder();


    /**
     * The set of SimpleDateFormat formats to use in getDateHeader().
     */
    protected static final SimpleDateFormat formats[] = {
        new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz", Locale.US),
        new SimpleDateFormat("EEEEEE, dd-MMM-yy HH:mm:ss zzz", Locale.US),
        new SimpleDateFormat("EEE MMMM d HH:mm:ss yyyy", Locale.US)
    };


    protected final static TimeZone gmtZone = TimeZone.getTimeZone("GMT");


    /**
     * GMT timezone - all HTTP dates are on GMT
     */
    static {
        formats[0].setTimeZone(gmtZone);
        formats[1].setTimeZone(gmtZone);
        formats[2].setTimeZone(gmtZone);
    }


    /**
     * MIME multipart separation string
     */
    protected static final String mimeSeparation = "CATALINA_MIME_BOUNDARY";


    /**
     * JNDI resources name.
     */
    protected static final String RESOURCES_JNDI_NAME = "java:/comp/Resources";


    /**
     * The string manager for this package.
     */
    protected static StringManager sm =
        StringManager.getManager(Constants.Package);


    /**
     * Array containing the safe characters set.
     */
    protected static BitSet safeCharacters;


    protected static final char[] hexadecimal =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
     'A', 'B', 'C', 'D', 'E', 'F'};


    // ----------------------------------------------------- Static Initializer


    static {
        safeCharacters = new BitSet(256);
        int i;
        for (i = 'a'; i <= 'z'; i++) {
            safeCharacters.set(i);
        }
        for (i = 'A'; i <= 'Z'; i++) {
            safeCharacters.set(i);
        }
        for (i = '0'; i <= '9'; i++) {
            safeCharacters.set(i);
        }
        safeCharacters.set('-');
        safeCharacters.set('_');
        safeCharacters.set('.');
        safeCharacters.set('*');
        safeCharacters.set('/');
    }


    // --------------------------------------------------------- Public Methods


    /**
     * Finalize this servlet.
     */
    public void destroy() {

        ;       // No actions necessary

    }


    /**
     * Initialize this servlet.
     */
    public void init() throws ServletException {

        // Set our properties from the initialization parameters
        String value = null;
        try {
            value = getServletConfig().getInitParameter("debug");
            debug = Integer.parseInt(value);
        } catch (Throwable t) {
            ;
        }
        try {
            value = getServletConfig().getInitParameter("input");
            input = Integer.parseInt(value);
        } catch (Throwable t) {
            ;
        }
        try {
            value = getServletConfig().getInitParameter("listings");
            listings = (new Boolean(value)).booleanValue();
        } catch (Throwable t) {
            ;
        }
        try {
            value = getServletConfig().getInitParameter("readonly");
            if (value != null)
                readOnly = (new Boolean(value)).booleanValue();
        } catch (Throwable t) {
            ;
        }
        try {
            value = getServletConfig().getInitParameter("output");
            output = Integer.parseInt(value);
        } catch (Throwable t) {
            ;
        }

        // Sanity check on the specified buffer sizes
        if (input < 256)
            input = 256;
        if (output < 256)
            output = 256;

        // Initialize the set of welcome files for this application
        welcomes = (String[]) getServletContext().getAttribute
            (Globals.WELCOME_FILES_ATTR);
        if (welcomes == null)
            welcomes = new String[0];

        if (debug > 0) {
            log("DefaultServlet.init:  input buffer size=" + input +
                ", output buffer size=" + output);
            for (int i = 0; i < welcomes.length; i++)
                log("DefaultServlet.init:  welcome file=" +
                    welcomes[i]);
        }

        // Load the MD5 helper used to calculate signatures.
        try {
            md5Helper = MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            throw new IllegalStateException();
        }

    }



    // ------------------------------------------------------ Protected Methods


    /**
     * Get resources. This method will try to retrieve the resources through
     * JNDI first, then in the servlet context if JNDI has failed (it could be
     * disabled). It will return null.
     *
     * @return A JNDI DirContext, or null.
     */
    protected DirContext getResources() {

        DirContext result = null;

        // Try the servlet context
        try {
            result = (DirContext) getServletContext()
                .getAttribute(Globals.RESOURCES_ATTR);
        } catch (ClassCastException e) {
            // Failed : Not the right type
        }

        if (result != null)
            return result;

        // Try JNDI
        try {
            result =
                (DirContext) new InitialContext().lookup(RESOURCES_JNDI_NAME);
        } catch (NamingException e) {
            // Failed
        } catch (ClassCastException e) {
            // Failed : Not the right type
        }

        return result;

    }


    /**
     * Show HTTP header information.
     */
    protected void showRequestInfo(HttpServletRequest req) {

        System.out.println();
        System.out.println("SlideDAV Request Info");
        System.out.println();

        // Show generic info
        System.out.println("Encoding : " + req.getCharacterEncoding());
        System.out.println("Length : " + req.getContentLength());
        System.out.println("Type : " + req.getContentType());

        System.out.println();
        System.out.println("Parameters");

        Enumeration parameters = req.getParameterNames();

        while (parameters.hasMoreElements()) {
            String paramName = (String) parameters.nextElement();
            String[] values = req.getParameterValues(paramName);
            System.out.print(paramName + " : ");
            for (int i = 0; i < values.length; i++) {
                System.out.print(values[i] + ", ");
            }
            System.out.println();
        }

        System.out.println();

        System.out.println("Protocol : " + req.getProtocol());
        System.out.println("Address : " + req.getRemoteAddr());
        System.out.println("Host : " + req.getRemoteHost());
        System.out.println("Scheme : " + req.getScheme());
        System.out.println("Server Name : " + req.getServerName());
        System.out.println("Server Port : " + req.getServerPort());

        System.out.println();
        System.out.println("Attributes");

        Enumeration attributes = req.getAttributeNames();

        while (attributes.hasMoreElements()) {
            String attributeName = (String) attributes.nextElement();
            System.out.print(attributeName + " : ");
            System.out.println(req.getAttribute(attributeName).toString());
        }

        System.out.println();

        // Show HTTP info
        System.out.println();
        System.out.println("HTTP Header Info");
        System.out.println();

        System.out.println("Authentication Type : " + req.getAuthType());
        System.out.println("HTTP Method : " + req.getMethod());
        System.out.println("Path Info : " + req.getPathInfo());
        System.out.println("Path translated : " + req.getPathTranslated());
        System.out.println("Query string : " + req.getQueryString());
        System.out.println("Remote user : " + req.getRemoteUser());
        System.out.println("Requested session id : "
                           + req.getRequestedSessionId());
        System.out.println("Request URI : " + req.getRequestURI());
        System.out.println("Context path : " + req.getContextPath());
        System.out.println("Servlet path : " + req.getServletPath());
        System.out.println("User principal : " + req.getUserPrincipal());


        System.out.println();
        System.out.println("Headers : ");

        Enumeration headers = req.getHeaderNames();

        while (headers.hasMoreElements()) {
            String headerName = (String) headers.nextElement();
            System.out.print(headerName + " : ");
            System.out.println(req.getHeader(headerName));
        }

        System.out.println();
        System.out.println();

    }


    /**
     * Return the relative path associated with this servlet.
     *
     * @param request The servlet request we are processing
     */
    protected String getRelativePath(HttpServletRequest request) {

        // Are we being processed by a RequestDispatcher.include()?
        if (request.getAttribute("javax.servlet.include.request_uri")!=null) {
            String result = (String)
                request.getAttribute("javax.servlet.include.path_info");
            if (result == null)
                result = (String)
                    request.getAttribute("javax.servlet.include.servlet_path");
            if ((result == null) || (result.equals("")))
                result = "/";
            return (result);
        }

        // No, extract the desired path directly from the request
        String result = request.getPathInfo();
        if (result == null) {
            result = request.getServletPath();
        }
        if ((result == null) || (result.equals(""))) {
            result = "/";
        }
        return normalize(result);

    }


    /**
     * Process a GET request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void doGet(HttpServletRequest request,
                         HttpServletResponse response)
        throws IOException, ServletException {

        if (debug > 999)
            showRequestInfo(request);

        // Serve the requested resource, including the data content
        serveResource(request, response, true);

    }


    /**
     * Process a HEAD request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void doHead(HttpServletRequest request,
                          HttpServletResponse response)
        throws IOException, ServletException {

        // Serve the requested resource, without the data content
        serveResource(request, response, false);

    }


    /**
     * Process a POST request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void doPost(HttpServletRequest request,
                          HttpServletResponse response)
        throws IOException, ServletException {
        doGet(request, response);
    }


    /**
     * Process a POST request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void doPut(HttpServletRequest req, HttpServletResponse resp)
        throws ServletException, IOException {

        if (readOnly) {
            resp.sendError(HttpServletResponse.SC_FORBIDDEN);
            return;
        }

        String path = getRelativePath(req);

        if ((path.toUpperCase().startsWith("/WEB-INF")) ||
            (path.toUpperCase().startsWith("/META-INF"))) {
            resp.sendError(HttpServletResponse.SC_FORBIDDEN);
            return;
        }

        // Looking for a Content-Range header
        if (req.getHeader("Content-Range") != null) {
            // No content range header is supported
            resp.sendError(HttpServletResponse.SC_NOT_IMPLEMENTED);
        }

        // Retrieve the resources
        DirContext resources = getResources();

        if (resources == null) {
            resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            return;
        }

        boolean exists = true;
        try {
            resources.lookup(path);
        } catch (NamingException e) {
            exists = false;
        }

        boolean result = true;
        try {
            Resource newResource = new Resource(req.getInputStream());
            // FIXME: Add attributes
            if (exists) {
                resources.rebind(path, newResource);
            } else {
                resources.bind(path, newResource);
            }
        } catch(NamingException e) {
            result = false;
        }

        if (result) {
            if (exists) {
                resp.setStatus(HttpServletResponse.SC_NO_CONTENT);
            } else {
                resp.setStatus(HttpServletResponse.SC_CREATED);
            }
        } else {
            resp.sendError(HttpServletResponse.SC_CONFLICT);
        }

    }


    /**
     * Process a POST request for the specified resource.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void doDelete(HttpServletRequest req, HttpServletResponse resp)
        throws ServletException, IOException {

        if (readOnly) {
            resp.sendError(HttpServletResponse.SC_FORBIDDEN);
            return;
        }

        String path = getRelativePath(req);

        if ((path.toUpperCase().startsWith("/WEB-INF")) ||
            (path.toUpperCase().startsWith("/META-INF"))) {
            resp.sendError(HttpServletResponse.SC_FORBIDDEN);
            return;
        }

        // Retrieve the Catalina context
        // Retrieve the resources
        DirContext resources = getResources();

        if (resources == null) {
            resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            return;
        }

        boolean exists = true;
        try {
            resources.lookup(path);
        } catch (NamingException e) {
            exists = false;
        }

        if (exists) {
            boolean result = true;
            try {
                resources.unbind(path);
            } catch (NamingException e) {
                result = false;
            }
            if (result) {
                resp.setStatus(HttpServletResponse.SC_NO_CONTENT);
            } else {
                resp.sendError(HttpServletResponse.SC_METHOD_NOT_ALLOWED);
            }
        } else {
            resp.sendError(HttpServletResponse.SC_NOT_FOUND);
        }

    }


    /**
     * Check if the conditions specified in the optional If headers are
     * satisfied.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     * @param resourceInfo File object
     * @return boolean true if the resource meets all the specified conditions,
     * and false if any of the conditions is not satisfied, in which case
     * request processing is stopped
     */
    protected boolean checkIfHeaders(HttpServletRequest request,
                                     HttpServletResponse response,
                                     ResourceInfo resourceInfo)
        throws IOException {

        String eTag = getETag(resourceInfo, true);
        long fileLength = resourceInfo.length;
        long lastModified = resourceInfo.date;

        StringTokenizer commaTokenizer;

        String headerValue;

        // Checking If-Match
        headerValue = request.getHeader("If-Match");
        if (headerValue != null) {
            if (headerValue.indexOf('*') == -1) {

                commaTokenizer = new StringTokenizer(headerValue, ",");
                boolean conditionSatisfied = false;

                while (!conditionSatisfied && commaTokenizer.hasMoreTokens()) {
                    String currentToken = commaTokenizer.nextToken();
                    if (currentToken.trim().equals(eTag))
                        conditionSatisfied = true;
                }

                // If none of the given ETags match, 412 Precodition failed is
                // sent back
                if (!conditionSatisfied) {
                    response.sendError
                        (HttpServletResponse.SC_PRECONDITION_FAILED);
                    return false;
                }

            }
        }

        // Checking If-Modified-Since
        headerValue = request.getHeader("If-Modified-Since");
        if (headerValue != null) {

            // If an If-None-Match header has been specified, if modified since
            // is ignored.
            if (request.getHeader("If-None-Match") == null) {

                Date date = null;

                // Parsing the HTTP Date
                for (int i = 0; (date == null) && (i < formats.length); i++) {
                    try {
                        date = formats[i].parse(headerValue);
                    } catch (ParseException e) {
                        ;
                    }
                }

                if ((date != null)
                    && (lastModified <= (date.getTime() + 1000)) ) {
                    // The entity has not been modified since the date
                    // specified by the client. This is not an error case.
                    response.setStatus(HttpServletResponse.SC_NOT_MODIFIED);
                    return false;
                }

            }

        }

        // Checking If-None-Match
        headerValue = request.getHeader("If-None-Match");
        if (headerValue != null) {

            boolean conditionSatisfied = false;

            if (!headerValue.equals("*")) {

                commaTokenizer = new StringTokenizer(headerValue, ",");

                while (!conditionSatisfied && commaTokenizer.hasMoreTokens()) {
                    String currentToken = commaTokenizer.nextToken();
                    if (currentToken.trim().equals(eTag))
                        conditionSatisfied = true;
                }

            } else {
                conditionSatisfied = true;
            }

            if (conditionSatisfied) {

                // For GET and HEAD, we should respond with
                // 304 Not Modified.
                // For every other method, 412 Precondition Failed is sent
                // back.
                if ( ("GET".equals(request.getMethod()))
                     || ("HEAD".equals(request.getMethod())) ) {
                    response.setStatus(HttpServletResponse.SC_NOT_MODIFIED);
                    return false;
                } else {
                    response.sendError
                        (HttpServletResponse.SC_PRECONDITION_FAILED);
                    return false;
                }
            }

        }

        // Checking If-Unmodified-Since
        headerValue = request.getHeader("If-Unmodified-Since");
        if (headerValue != null) {

            Date date = null;

            // Parsing the HTTP Date
            for (int i = 0; (date == null) && (i < formats.length); i++) {
                try {
                    date = formats[i].parse(headerValue);
                } catch (ParseException e) {
                    ;
                }
            }

            if ( (date != null) && (lastModified > date.getTime()) ) {
                // The entity has not been modified since the date
                // specified by the client. This is not an error case.
                response.sendError
                    (HttpServletResponse.SC_PRECONDITION_FAILED);
                return false;
            }

        }

        return true;
    }


    /**
     * Get the ETag value associated with a file.
     *
     * @param resourceInfo File object
     * @param strong True if we want a strong ETag, in which case a checksum
     * of the file has to be calculated
     */
    protected String getETagValue(ResourceInfo resourceInfo, boolean strong) {
        // FIXME : Compute a strong ETag if requested, using an MD5 digest
        // of the file contents
        return resourceInfo.length + "-" + resourceInfo.date;
    }


    /**
     * Get the ETag associated with a file.
     *
     * @param resourceInfo File object
     * @param strong True if we want a strong ETag, in which case a checksum
     * of the file has to be calculated
     */
    protected String getETag(ResourceInfo resourceInfo, boolean strong) {
        if (strong)
            return "\"" + getETagValue(resourceInfo, strong) + "\"";
        else
            return "W/\"" + getETagValue(resourceInfo, strong) + "\"";
    }


    /**
     * Return a context-relative path, beginning with a "/", that represents
     * the canonical version of the specified path after ".." and "." elements
     * are resolved out.  If the specified path attempts to go outside the
     * boundaries of the current context (i.e. too many ".." path elements
     * are present), return <code>null</code> instead.
     *
     * @param path Path to be normalized
     */
    protected String normalize(String path) {

        if (path == null)
            return null;

        // Create a place for the normalized path
        String normalized = path;

        /*
         * Commented out -- already URL-decoded in StandardContextMapper
         * Decoding twice leaves the container vulnerable to %25 --> '%'
         * attacks.
         *
         * if (normalized.indexOf('%') >= 0)
         *     normalized = RequestUtil.URLDecode(normalized, "UTF8");
         */

        if (normalized == null)
            return (null);

        if (normalized.equals("/."))
            return "/";

        // Normalize the slashes and add leading slash if necessary
        if (normalized.indexOf('\\') >= 0)
            normalized = normalized.replace('\\', '/');
        if (!normalized.startsWith("/"))
            normalized = "/" + normalized;

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

        // Return the normalized path that we have completed
        return (normalized);

    }


    /**
     * URL rewriter.
     *
     * @param path Path which has to be rewiten
     */
    protected String rewriteUrl(String path) {

        /**
         * Note: This code portion is very similar to URLEncoder.encode.
         * Unfortunately, there is no way to specify to the URLEncoder which
         * characters should be encoded. Here, ' ' should be encoded as "%20"
         * and '/' shouldn't be encoded.
         */

        int maxBytesPerChar = 10;
        int caseDiff = ('a' - 'A');
        StringBuffer rewrittenPath = new StringBuffer(path.length());
        ByteArrayOutputStream buf = new ByteArrayOutputStream(maxBytesPerChar);
        OutputStreamWriter writer = null;
        try {
            writer = new OutputStreamWriter(buf, "UTF8");
        } catch (Exception e) {
            e.printStackTrace();
            writer = new OutputStreamWriter(buf);
        }

        for (int i = 0; i < path.length(); i++) {
            int c = (int) path.charAt(i);
            if (safeCharacters.get(c)) {
                rewrittenPath.append((char)c);
            } else {
                // convert to external encoding before hex conversion
                try {
                    writer.write(c);
                    writer.flush();
                } catch(IOException e) {
                    buf.reset();
                    continue;
                }
                byte[] ba = buf.toByteArray();
                for (int j = 0; j < ba.length; j++) {
                    // Converting each byte in the buffer
                    byte toEncode = ba[j];
                    rewrittenPath.append('%');
                    int low = (int) (toEncode & 0x0f);
                    int high = (int) ((toEncode & 0xf0) >> 4);
                    rewrittenPath.append(hexadecimal[high]);
                    rewrittenPath.append(hexadecimal[low]);
                }
                buf.reset();
            }
        }

        return rewrittenPath.toString();

    }


    /**
     * Display the size of a file.
     */
    protected void displaySize(StringBuffer buf, int filesize) {

        int leftside = filesize / 1024;
        int rightside = (filesize % 1024) / 103;  // makes 1 digit
        // To avoid 0.0 for non-zero file, we bump to 0.1
        if (leftside == 0 && rightside == 0 && filesize != 0)
            rightside = 1;
        buf.append(leftside).append(".").append(rightside);
        buf.append(" KB");

    }


    /**
     * Serve the specified resource, optionally including the data content.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     * @param content Should the content be included?
     *
     * @exception IOException if an input/output error occurs
     * @exception ServletException if a servlet-specified error occurs
     */
    protected void serveResource(HttpServletRequest request,
                                 HttpServletResponse response,
                                 boolean content)
        throws IOException, ServletException {

        // Identify the requested resource path
        String path = getRelativePath(request);
        if (debug > 0) {
            if (content)
                log("DefaultServlet.serveResource:  Serving resource '" +
                    path + "' headers and data");
            else
                log("DefaultServlet.serveResource:  Serving resource '" +
                    path + "' headers only");
        }

        // Exclude any resource in the /WEB-INF and /META-INF subdirectories
        // (the "toUpperCase()" avoids problems on Windows systems)
        if ((path == null) ||
            path.toUpperCase().startsWith("/WEB-INF") ||
            path.toUpperCase().startsWith("/META-INF")) {
            response.sendError(HttpServletResponse.SC_NOT_FOUND, path);
            return;
        }

        // Retrieve the Catalina context and Resources implementation
        DirContext resources = getResources();
        ResourceInfo resourceInfo = new ResourceInfo(path, resources);

        if (!resourceInfo.exists) {
            response.sendError(HttpServletResponse.SC_NOT_FOUND, path);
            return;
        }

        // If the resource is not a collection, and the resource path
        // ends with "/" or "\", return NOT FOUND
        if (!resourceInfo.collection) {
            if (path.endsWith("/") || (path.endsWith("\\"))) {
                response.sendError(HttpServletResponse.SC_NOT_FOUND, path);
                return;
            }
        }

        // If the resource is a collection (aka a directory), we check
        // the welcome files list.
        if (resourceInfo.collection) {

            if (!request.getRequestURI().endsWith("/")) {
                String redirectPath = request.getRequestURI() + "/";
                redirectPath = appendParameters(request, redirectPath);
                response.sendRedirect(redirectPath);
                return;
            }

            ResourceInfo welcomeFileInfo = checkWelcomeFiles(path, resources);
            if (welcomeFileInfo != null) {
                String redirectPath = welcomeFileInfo.path;
                String contextPath = request.getContextPath();
                if ((contextPath != null) && (!contextPath.equals("/"))) {
                    redirectPath = contextPath + redirectPath;
                }
                redirectPath = appendParameters(request, redirectPath);
                response.sendRedirect(redirectPath);
                return;
            }

        }

        // Checking If headers
        if ( !checkIfHeaders(request, response, resourceInfo) ) {
            return;
        }

        // Find content type.
        String contentType =
            getServletContext().getMimeType(resourceInfo.path);

        if (resourceInfo.collection) {
            // Skip directory listings if we have been configured to
            // suppress them
            if (!listings) {
                response.sendError(HttpServletResponse.SC_NOT_FOUND,
                                   resourceInfo.path);
                return;
            }
            contentType = "text/html;charset=UTF-8";
        }


        // Parse range specifier
        Vector ranges = null;
        if (!resourceInfo.collection) {

            ranges = parseRange(request, response, resourceInfo);

            // ETag header
            response.setHeader("ETag", getETag(resourceInfo, true));

        }

        // Last-Modified header
        if (debug > 0)
            log("DefaultServlet.serveFile:  lastModified='" +
                (new Timestamp(resourceInfo.date)).toString() + "'");
        response.setDateHeader("Last-Modified", resourceInfo.date);

        ServletOutputStream ostream = null;
        PrintWriter writer = null;

        if (content) {

            // Trying to retrieve the servlet output stream

            try {
                ostream = response.getOutputStream();
            } catch (IllegalStateException e) {
                // If it fails, we try to get a Writer instead if we're
                // trying to serve a text file
                if ( (contentType != null)
                     && (contentType.startsWith("text")) ) {
                    writer = response.getWriter();
                } else {
                    throw e;
                }
            }

        }

        if ( (resourceInfo.collection) ||
             ( ((ranges == null) || (ranges.isEmpty()))
               && (request.getHeader("Range") == null) ) ) {

            // Set the appropriate output headers
            if (contentType != null) {
                if (debug > 0)
                    log("DefaultServlet.serveFile:  contentType='" +
                        contentType + "'");
                response.setContentType(contentType);
            }
            long contentLength = resourceInfo.length;
            if ((!resourceInfo.collection) && (contentLength >= 0)) {
                if (debug > 0)
                    log("DefaultServlet.serveFile:  contentLength=" +
                        contentLength);
                response.setContentLength((int) contentLength);
            }

            if (resourceInfo.collection) {

                if (content) {
                    // Serve the directory browser
                    resourceInfo.setStream
                        (render(request.getContextPath(), resourceInfo));
                }

            }

            // Copy the input stream to our output stream (if requested)
            if (content) {
                try {
                    response.setBufferSize(output);
                } catch (IllegalStateException e) {
                    // Silent catch
                }
                if (ostream != null) {
                    copy(resourceInfo, ostream);
                } else {
                    copy(resourceInfo, writer);
                }
            }

        } else {

            if ((ranges == null) || (ranges.isEmpty()))
                return;

            // Partial content response.

            response.setStatus(HttpServletResponse.SC_PARTIAL_CONTENT);

            if (ranges.size() == 1) {

                Range range = (Range) ranges.elementAt(0);
                response.addHeader("Content-Range", "bytes "
                                   + range.start
                                   + "-" + range.end + "/"
                                   + range.length);
                response.setContentLength((int) (range.end - range.start));

                if (contentType != null) {
                    if (debug > 0)
                        log("DefaultServlet.serveFile:  contentType='" +
                            contentType + "'");
                    response.setContentType(contentType);
                }

                if (content) {
                    try {
                        response.setBufferSize(output);
                    } catch (IllegalStateException e) {
                        // Silent catch
                    }
                    if (ostream != null) {
                        copy(resourceInfo, ostream, range);
                    } else {
                        copy(resourceInfo, writer, range);
                    }
                }

            } else {

                response.setContentType("multipart/byteranges; boundary="
                                        + mimeSeparation);

                if (content) {
                    try {
                        response.setBufferSize(output);
                    } catch (IllegalStateException e) {
                        // Silent catch
                    }
                    if (ostream != null) {
                        copy(resourceInfo, ostream, ranges.elements(),
                             contentType);
                    } else {
                        copy(resourceInfo, writer, ranges.elements(),
                             contentType);
                    }
                }

            }

        }

    }


    /**
     * Parse the range header.
     *
     * @param request The servlet request we are processing
     * @param response The servlet response we are creating
     * @return Vector of ranges
     */
    protected Vector parseRange(HttpServletRequest request,
                                HttpServletResponse response,
                                ResourceInfo resourceInfo)
        throws IOException {

        // Checking If-Range
        String headerValue = request.getHeader("If-Range");
        if (headerValue != null) {

            String eTag = getETag(resourceInfo, true);
            long lastModified = resourceInfo.date;

            Date date = null;

            // Parsing the HTTP Date
            for (int i = 0; (date == null) && (i < formats.length); i++) {
                try {
                    date = formats[i].parse(headerValue);
                } catch (ParseException e) {
                    ;
                }
            }

            if (date == null) {

                // If the ETag the client gave does not match the entity
                // etag, then the entire entity is returned.
                if (!eTag.equals(headerValue.trim()))
                    return null;

            } else {

                // If the timestamp of the entity the client got is older than
                // the last modification date of the entity, the entire entity
                // is returned.
                if (lastModified > (date.getTime() + 1000))
                    return null;

            }

        }

        long fileLength = resourceInfo.length;

        if (fileLength == 0)
            return null;

        // Retrieving the range header (if any is specified
        String rangeHeader = request.getHeader("Range");

        if (rangeHeader == null)
            return null;
        // bytes is the only range unit supported (and I don't see the point
        // of adding new ones).
        if (!rangeHeader.startsWith("bytes")) {
            response.sendError
                (HttpServletResponse.SC_REQUESTED_RANGE_NOT_SATISFIABLE);
            return null;
        }

        rangeHeader = rangeHeader.substring(6);

        // Vector which will contain all the ranges which are successfully
        // parsed.
        Vector result = new Vector();
        StringTokenizer commaTokenizer = new StringTokenizer(rangeHeader, ",");

        // Parsing the range list
        while (commaTokenizer.hasMoreTokens()) {
            String rangeDefinition = commaTokenizer.nextToken();

            Range currentRange = new Range();
            currentRange.length = fileLength;

            int dashPos = rangeDefinition.indexOf('-');

            if (dashPos == -1) {
                response.sendError
                    (HttpServletResponse.SC_REQUESTED_RANGE_NOT_SATISFIABLE);
                return null;
            }

            if (dashPos == 0) {

                try {
                    long offset = Long.parseLong(rangeDefinition);
                    currentRange.start = fileLength + offset;
                    currentRange.end = fileLength - 1;
                } catch (NumberFormatException e) {
                    response.sendError
                        (HttpServletResponse
                         .SC_REQUESTED_RANGE_NOT_SATISFIABLE);
                    return null;
                }

            } else {

                try {
                    currentRange.start = Long.parseLong
                        (rangeDefinition.substring(0, dashPos));
                    if (dashPos < rangeDefinition.length() - 1)
                        currentRange.end = Long.parseLong
                            (rangeDefinition.substring
                             (dashPos + 1, rangeDefinition.length()));
                    else
                        currentRange.end = fileLength - 1;
                } catch (NumberFormatException e) {
                    response.sendError
                        (HttpServletResponse
                         .SC_REQUESTED_RANGE_NOT_SATISFIABLE);
                    return null;
                }

            }

            if (!currentRange.validate()) {
                response.sendError
                    (HttpServletResponse.SC_REQUESTED_RANGE_NOT_SATISFIABLE);
                return null;
            }

            result.addElement(currentRange);
        }

        return result;
    }


    /**
     * Append the request parameters to the redirection string before calling
     * sendRedirect.
     */
    protected String appendParameters(HttpServletRequest request,
                                      String redirectPath) {

        StringBuffer result = new StringBuffer(rewriteUrl(redirectPath));

        Enumeration enum = request.getParameterNames();
        if (enum.hasMoreElements())
            result.append("?");

        while (enum.hasMoreElements()) {
            String name = (String) enum.nextElement();
            String[] values = request.getParameterValues(name);
            for (int i = 0; i < values.length; i++) {
                result.append(rewriteUrl(name));
                result.append("=");
                result.append(rewriteUrl(values[i]));
                if (i < (values.length - 1))
                    result.append("&");
            }
            if (enum.hasMoreElements())
                result.append("&");
        }

        return result.toString();

    }


    /**
     * Return an InputStream to an HTML representation of the contents
     * of this directory.
     *
     * @param contextPath Context path to which our internal paths are
     *  relative
     */
    protected InputStream render
        (String contextPath, ResourceInfo resourceInfo) {

        String name = resourceInfo.path;

        // Number of characters to trim from the beginnings of filenames
        int trim = name.length();
        if (!name.endsWith("/"))
            trim += 1;
        if (name.equals("/"))
            trim = 1;

        // Prepare a writer to a buffered area
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        OutputStreamWriter osWriter = null;
        try {
            osWriter = new OutputStreamWriter(stream, "UTF8");
        } catch (Exception e) {
            // Should never happen
            osWriter = new OutputStreamWriter(stream);
        }
        PrintWriter writer = new PrintWriter(osWriter);

        // Render the page header
        writer.print("<html>\r\n");
        writer.print("<head>\r\n");
        writer.print("<title>");
        writer.print(sm.getString("directory.title", name));
        writer.print("</title>\r\n</head>\r\n");
        writer.print("<body bgcolor=\"white\">\r\n");
        writer.print("<table width=\"90%\" cellspacing=\"0\"" +
                     " cellpadding=\"5\" align=\"center\">\r\n");

        // Render the in-page title
        writer.print("<tr><td colspan=\"3\"><font size=\"+2\">\r\n<strong>");
        writer.print(sm.getString("directory.title", name));
        writer.print("</strong>\r\n</font></td></tr>\r\n");

        // Render the link to our parent (if required)
        String parentDirectory = name;
        if (parentDirectory.endsWith("/")) {
            parentDirectory =
                parentDirectory.substring(0, parentDirectory.length() - 1);
        }
        int slash = parentDirectory.lastIndexOf('/');
        if (slash >= 0) {
            String parent = name.substring(0, slash);
            writer.print("<tr><td colspan=\"3\" bgcolor=\"#ffffff\">\r\n");
            writer.print("<a href=\"");
            writer.print(rewriteUrl(contextPath));
            if (parent.equals(""))
                parent = "/";
            writer.print(rewriteUrl(parent));
            if (!parent.endsWith("/"))
                writer.print("/");
            writer.print("\">");
            writer.print(sm.getString("directory.parent", parent));
            writer.print("</a>\r\n");
            writer.print("</td></tr>\r\n");
        }

        // Render the column headings
        writer.print("<tr bgcolor=\"#cccccc\">\r\n");
        writer.print("<td align=\"left\"><font size=\"+1\"><strong>");
        writer.print(sm.getString("directory.filename"));
        writer.print("</strong></font></td>\r\n");
        writer.print("<td align=\"center\"><font size=\"+1\"><strong>");
        writer.print(sm.getString("directory.size"));
        writer.print("</strong></font></td>\r\n");
        writer.print("<td align=\"right\"><font size=\"+1\"><strong>");
        writer.print(sm.getString("directory.lastModified"));
        writer.print("</strong></font></td>\r\n");
        writer.print("</tr>\r\n");

        try {

            // Render the directory entries within this directory
            DirContext directory = resourceInfo.directory;
            NamingEnumeration enum =
                resourceInfo.resources.list(resourceInfo.path);
            boolean shade = false;
            while (enum.hasMoreElements()) {

                NameClassPair ncPair = (NameClassPair) enum.nextElement();
                String resourceName = ncPair.getName();
                ResourceInfo childResourceInfo =
                    new ResourceInfo(resourceName, directory);

                String trimmed = resourceName/*.substring(trim)*/;
                if (trimmed.equalsIgnoreCase("WEB-INF") ||
                    trimmed.equalsIgnoreCase("META-INF"))
                    continue;

                writer.print("<tr");
                if (shade)
                    writer.print(" bgcolor=\"eeeeee\"");
                writer.print(">\r\n");
                shade = !shade;

                writer.print("<td align=\"left\">&nbsp;&nbsp;\r\n");
                writer.print("<a href=\"");
                writer.print(rewriteUrl(contextPath));
                resourceName = rewriteUrl(name + resourceName);
                writer.print(resourceName);
                if (childResourceInfo.collection)
                    writer.print("/");
                writer.print("\"><tt>");
                writer.print(trimmed);

                if (childResourceInfo.collection)
                    writer.print("/");
                writer.print("</tt></a></td>\r\n");

                writer.print("<td align=\"right\"><tt>");
                if (childResourceInfo.collection)
                    writer.print("&nbsp;");
                else
                    writer.print(renderSize(childResourceInfo.length));
                writer.print("</tt></td>\r\n");

                writer.print("<td align=\"right\"><tt>");
                writer.print(renderLastModified(childResourceInfo.date));
                writer.print("</tt></td>\r\n");

                writer.print("</tr>\r\n");
            }

        } catch (NamingException e) {
            // Something went wrong
            e.printStackTrace();
        }

        // Render the page footer
        writer.print("<tr><td colspan=\"3\">&nbsp;</td></tr>\r\n");
        writer.print("<tr><td colspan=\"3\" bgcolor=\"#cccccc\">");
        writer.print("<font size=\"-1\">");
        writer.print(Globals.SERVER_INFO);
        writer.print("</font></td></tr>\r\n");
        writer.print("</table>\r\n");
        writer.print("</body>\r\n");
        writer.print("</html>\r\n");

        // Return an input stream to the underlying bytes
        writer.flush();
        return (new ByteArrayInputStream(stream.toByteArray()));

    }


    /**
     * Render the last modified date and time for the specified timestamp.
     *
     * @param lastModified Last modified date and time, in milliseconds since
     *  the epoch
     */
    protected String renderLastModified(long lastModified) {

        return (formats[0].format(new Date(lastModified)));

    }


    /**
     * Render the specified file size (in bytes).
     *
     * @param size File size (in bytes)
     */
    protected String renderSize(long size) {

        long leftSide = size / 1024;
        long rightSide = (size % 1024) / 103;   // Makes 1 digit
        if ((leftSide == 0) && (rightSide == 0) && (size > 0))
            rightSide = 1;

        return ("" + leftSide + "." + rightSide + " kb");

    }


    // -------------------------------------------------------- Private Methods


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param istream The input stream to read from
     * @param ostream The output stream to write to
     *
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, ServletOutputStream ostream)
        throws IOException {

        IOException exception = null;

        // FIXME : i18n ?
        InputStream resourceInputStream = resourceInfo.getStream();
        InputStream istream = new BufferedInputStream
            (resourceInputStream, input);

        // Copy the input stream to the output stream
        exception = copyRange(istream, ostream);

        // Clean up the input stream
        try {
            istream.close();
        } catch (Throwable t) {
            ;
        }

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param istream The input stream to read from
     * @param writer The writer to write to
     *
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, PrintWriter writer)
        throws IOException {

        IOException exception = null;

        InputStream resourceInputStream = resourceInfo.getStream();
        // FIXME : i18n ?
        Reader reader = new InputStreamReader(resourceInputStream);

        // Copy the input stream to the output stream
        exception = copyRange(reader, writer);

        // Clean up the reader
        try {
            reader.close();
        } catch (Throwable t) {
            ;
        }

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param resourceInfo The ResourceInfo object
     * @param ostream The output stream to write to
     * @param range Range the client wanted to retrieve
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, ServletOutputStream ostream,
                      Range range)
        throws IOException {

        IOException exception = null;

        InputStream resourceInputStream = resourceInfo.getStream();
        InputStream istream =
            new BufferedInputStream(resourceInputStream, input);
        exception = copyRange(istream, ostream, range.start, range.end);

        // Clean up the input stream
        try {
            istream.close();
        } catch (Throwable t) {
            ;
        }

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param resourceInfo The ResourceInfo object
     * @param writer The writer to write to
     * @param range Range the client wanted to retrieve
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, PrintWriter writer,
                      Range range)
        throws IOException {

        IOException exception = null;

        InputStream resourceInputStream = resourceInfo.getStream();
        Reader reader = new InputStreamReader(resourceInputStream);
        exception = copyRange(reader, writer, range.start, range.end);

        // Clean up the input stream
        try {
            reader.close();
        } catch (Throwable t) {
            ;
        }

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param resourceInfo The ResourceInfo object
     * @param ostream The output stream to write to
     * @param ranges Enumeration of the ranges the client wanted to retrieve
     * @param contentType Content type of the resource
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, ServletOutputStream ostream,
                      Enumeration ranges, String contentType)
        throws IOException {

        IOException exception = null;

        while ( (exception == null) && (ranges.hasMoreElements()) ) {

            InputStream resourceInputStream = resourceInfo.getStream();
            InputStream istream =       // FIXME: internationalization???????
                new BufferedInputStream(resourceInputStream, input);

            Range currentRange = (Range) ranges.nextElement();

            // Writing MIME header.
            ostream.println("--" + mimeSeparation);
            if (contentType != null)
                ostream.println("Content-Type: " + contentType);
            ostream.println("Content-Range: bytes " + currentRange.start
                           + "-" + currentRange.end + "/"
                           + currentRange.length);
            ostream.println();

            // Printing content
            exception = copyRange(istream, ostream, currentRange.start,
                                  currentRange.end);

            try {
                istream.close();
            } catch (Throwable t) {
                ;
            }

        }

        ostream.print("--" + mimeSeparation + "--");

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param resourceInfo The ResourceInfo object
     * @param writer The writer to write to
     * @param ranges Enumeration of the ranges the client wanted to retrieve
     * @param contentType Content type of the resource
     * @exception IOException if an input/output error occurs
     */
    private void copy(ResourceInfo resourceInfo, PrintWriter writer,
                      Enumeration ranges, String contentType)
        throws IOException {

        IOException exception = null;

        while ( (exception == null) && (ranges.hasMoreElements()) ) {

            InputStream resourceInputStream = resourceInfo.getStream();
            Reader reader = new InputStreamReader(resourceInputStream);

            Range currentRange = (Range) ranges.nextElement();

            // Writing MIME header.
            writer.println("--" + mimeSeparation);
            if (contentType != null)
                writer.println("Content-Type: " + contentType);
            writer.println("Content-Range: bytes " + currentRange.start
                           + "-" + currentRange.end + "/"
                           + currentRange.length);
            writer.println();

            // Printing content
            exception = copyRange(reader, writer, currentRange.start,
                                  currentRange.end);

            try {
                reader.close();
            } catch (Throwable t) {
                ;
            }

        }

        writer.print("--" + mimeSeparation + "--");

        // Rethrow any exception that has occurred
        if (exception != null)
            throw exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param istream The input stream to read from
     * @param ostream The output stream to write to
     * @return Exception which occured during processing
     */
    private IOException copyRange(InputStream istream,
                                  ServletOutputStream ostream) {

        // Copy the input stream to the output stream
        IOException exception = null;
        byte buffer[] = new byte[input];
        int len = buffer.length;
        while (true) {
            try {
                len = istream.read(buffer);
                if (len == -1)
                    break;
                ostream.write(buffer, 0, len);
            } catch (IOException e) {
                exception = e;
                len = -1;
                break;
            }
        }
        return exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param reader The reader to read from
     * @param writer The writer to write to
     * @return Exception which occured during processing
     */
    private IOException copyRange(Reader reader, PrintWriter writer) {

        // Copy the input stream to the output stream
        IOException exception = null;
        char buffer[] = new char[input];
        int len = buffer.length;
        while (true) {
            try {
                len = reader.read(buffer);
                if (len == -1)
                    break;
                writer.write(buffer, 0, len);
            } catch (IOException e) {
                exception = e;
                len = -1;
                break;
            }
        }
        return exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param istream The input stream to read from
     * @param ostream The output stream to write to
     * @param start Start of the range which will be copied
     * @param end End of the range which will be copied
     * @return Exception which occured during processing
     */
    private IOException copyRange(InputStream istream,
                                  ServletOutputStream ostream,
                                  long start, long end) {

        if (debug > 10)
            System.out.println("Serving bytes:" + start + "-" + end);

        try {
            istream.skip(start);
        } catch (IOException e) {
            return e;
        }

        IOException exception = null;
        long bytesToRead = end - start + 1;

        byte buffer[] = new byte[input];
        int len = buffer.length;
        while ( (bytesToRead > 0) && (len >= buffer.length)) {
            try {
                len = istream.read(buffer);
                if (bytesToRead >= len) {
                    ostream.write(buffer, 0, len);
                    bytesToRead -= len;
                } else {
                    ostream.write(buffer, 0, (int) bytesToRead);
                    bytesToRead = 0;
                }
            } catch (IOException e) {
                exception = e;
                len = -1;
            }
            if (len < buffer.length)
                break;
        }

        return exception;

    }


    /**
     * Copy the contents of the specified input stream to the specified
     * output stream, and ensure that both streams are closed before returning
     * (even in the face of an exception).
     *
     * @param reader The reader to read from
     * @param writer The writer to write to
     * @param start Start of the range which will be copied
     * @param end End of the range which will be copied
     * @return Exception which occured during processing
     */
    private IOException copyRange(Reader reader, PrintWriter writer,
                                  long start, long end) {

        try {
            reader.skip(start);
        } catch (IOException e) {
            return e;
        }

        IOException exception = null;
        long bytesToRead = end - start + 1;

        char buffer[] = new char[input];
        int len = buffer.length;
        while ( (bytesToRead > 0) && (len >= buffer.length)) {
            try {
                len = reader.read(buffer);
                if (bytesToRead >= len) {
                    writer.write(buffer, 0, len);
                    bytesToRead -= len;
                } else {
                    writer.write(buffer, 0, (int) bytesToRead);
                    bytesToRead = 0;
                }
            } catch (IOException e) {
                exception = e;
                len = -1;
            }
            if (len < buffer.length)
                break;
        }

        return exception;

    }


    /**
     * Check to see if a default page exists.
     *
     * @param pathname Pathname of the file to be served
     */
    private ResourceInfo checkWelcomeFiles(String pathname,
                                           DirContext resources) {

        String collectionName = pathname;
        if (!pathname.endsWith("/")) {
            collectionName += "/";
        }

        // Refresh our currently defined set of welcome files
        synchronized (welcomes) {
            welcomes = (String[]) getServletContext().getAttribute
                (Globals.WELCOME_FILES_ATTR);
            if (welcomes == null)
                welcomes = new String[0];
        }

        // Serve a welcome resource or file if one exists
        for (int i = 0; i < welcomes.length; i++) {

            // Does the specified resource exist?
            String resourceName = collectionName + welcomes[i];
            ResourceInfo resourceInfo =
                new ResourceInfo(resourceName, resources);
            if (resourceInfo.exists()) {
                return resourceInfo;
            }

        }

        return null;

    }


    // ------------------------------------------------------ Range Inner Class


    private class Range {

        public long start;
        public long end;
        public long length;

        /**
         * Validate range.
         */
        public boolean validate() {
            return ( (start >= 0) && (end >= 0) && (length > 0)
                     && (start <= end) && (end < length) );
        }

        public void recycle() {
            start = 0;
            end = 0;
            length = 0;
        }

    }


    // ----------------------------------------------  ResourceInfo Inner Class


    protected class ResourceInfo {


        /**
         * Constructor.
         *
         * @param pathname Path name of the file
         */
        public ResourceInfo(String path, DirContext resources) {
            set(path, resources);
        }


        public Object object;
        public DirContext directory;
        public Resource file;
        public Attributes attributes;
        public String path;
        public long creationDate;
        public String httpDate;
        public long date;
        public long length;
        public boolean collection;
        public boolean exists;
        public DirContext resources;
        protected InputStream is;


        public void recycle() {
            object = null;
            directory = null;
            file = null;
            attributes = null;
            path = null;
            creationDate = 0;
            httpDate = null;
            date = 0;
            length = -1;
            collection = true;
            exists = false;
            resources = null;
            is = null;
        }


        public void set(String path, DirContext resources) {

            recycle();

            this.path = path;
            this.resources = resources;
            exists = true;
            try {
                object = resources.lookup(path);
                if (object instanceof Resource) {
                    file = (Resource) object;
                    collection = false;
                } else if (object instanceof DirContext) {
                    directory = (DirContext) object;
                    collection = true;
                } else {
                    // Don't know how to serve another object type
                    exists = false;
                }
            } catch (NamingException e) {
                exists = false;
            }
            if (exists) {
                try {
                    attributes = resources.getAttributes(path);
                    if (attributes instanceof ResourceAttributes) {
                        ResourceAttributes tempAttrs =
                            (ResourceAttributes) attributes;
                        Date tempDate = tempAttrs.getCreationDate();
                        if (tempDate != null)
                            creationDate = tempDate.getTime();
                        tempDate = tempAttrs.getLastModified();
                        if (tempDate != null) {
                            date = tempDate.getTime();
                            httpDate = formats[0].format(tempDate);
                        } else {
                            httpDate = formats[0].format(new Date());
                        }
                        length = tempAttrs.getContentLength();
                    }
                } catch (NamingException e) {
                    // Shouldn't happen, the implementation of the DirContext
                    // is probably broken
                    exists = false;
                }
            }

        }


        /**
         * Test if the associated resource exists.
         */
        public boolean exists() {
            return exists;
        }


        /**
         * String representation.
         */
        public String toString() {
            return path;
        }


        /**
         * Set IS.
         */
        public void setStream(InputStream is) {
            this.is = is;
        }


        /**
         * Get IS from resource.
         */
        public InputStream getStream()
            throws IOException {
            if (is != null)
                return is;
            if (file != null)
                return (file.streamContent());
            else
                return null;
        }


    }


}
