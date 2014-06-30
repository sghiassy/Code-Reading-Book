/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/HttpResponseBase.java,v 1.37 2001/08/24 23:06:08 craigmcc Exp $
 * $Revision: 1.37 $
 * $Date: 2001/08/24 23:06:08 $
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


package org.apache.catalina.connector;


import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.MalformedURLException;
// import java.net.URL;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.TimeZone;
import javax.servlet.ServletResponse;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpUtils;
import org.apache.catalina.HttpResponse;
import org.apache.catalina.Globals;
import org.apache.catalina.Logger;
import org.apache.catalina.util.CookieTools;
import org.apache.catalina.util.RequestUtil;
import org.apache.catalina.util.URL;


/**
 * Convenience base implementation of the <b>HttpResponse</b> interface, which
 * can be used for the <code>Response</code> implementation required by most
 * <code>Connectors</code> that deal with HTTP.  Only the connector-specific
 * methods need to be implemented.
 *
 * @author Craig R. McClanahan
 * @author Remy Maucherat
 * @version $Revision: 1.37 $ $Date: 2001/08/24 23:06:08 $
 */

public class HttpResponseBase
    extends ResponseBase
    implements HttpResponse, HttpServletResponse {


    // ----------------------------------------------------- Instance Variables


    /**
     * The set of Cookies associated with this Response.
     */
    protected ArrayList cookies = new ArrayList();


    /**
     * The date format we will use for creating date headers.
     */
    protected static final SimpleDateFormat format =
        new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz",Locale.US);
    static {
        format.setTimeZone(TimeZone.getTimeZone("GMT"));
    };


    /**
     * The facade associated with this response.
     */
    protected HttpResponseFacade facade = new HttpResponseFacade(this);


    /**
     * The HTTP headers explicitly added via addHeader(), but not including
     * those to be added with setContentLength(), setContentType(), and so on.
     * This collection is keyed by the header name, and the elements are
     * ArrayLists containing the associated values that have been set.
     */
    protected HashMap headers = new HashMap();


    /**
     * Descriptive information about this HttpResponse implementation.
     */
    protected static final String info =
        "org.apache.catalina.connector.HttpResponseBase/1.0";


    /**
     * The error message set by <code>sendError()</code>.
     */
    protected String message = getStatusMessage(HttpServletResponse.SC_OK);


    /**
     * The HTTP status code associated with this Response.
     */
    protected int status = HttpServletResponse.SC_OK;


    /**
     * The time zone with which to construct date headers.
     */
    protected static final TimeZone zone = TimeZone.getTimeZone("GMT");


    // ------------------------------------------------------------- Properties


    /**
     * Return the <code>ServletResponse</code> for which this object
     * is the facade.
     */
    public ServletResponse getResponse() {

        return (facade);

    }


    // --------------------------------------------------------- Public Methods


    /**
     * Perform whatever actions are required to flush and close the output
     * stream or writer, in a single operation.
     *
     * @exception IOException if an input/output error occurs
     */
    public void finishResponse() throws IOException {

        // If an HTTP error >= 400 has been created with no content,
        // attempt to create a simple error message
        if (!isCommitted() &&
            (stream == null) && (writer == null) &&
            (status >= HttpServletResponse.SC_BAD_REQUEST) &&
            (contentType == null) &&
            (contentCount == 0)) {
            try {
                setContentType("text/html");
                PrintWriter writer = getWriter();
                writer.println("<html>");
                writer.println("<head>");
                writer.println("<title>Tomcat Error Report</title>");
                writer.println("<br><br>");
                writer.println("<h1>HTTP Status ");
                writer.print(status);
                writer.print(" - ");
                if (message != null)
                    writer.print(message);
                else
                    writer.print(getStatusMessage(status));
                writer.println("</h1>");
                writer.println("</body>");
                writer.println("</html>");
            } catch (IOException e) {
                throw e;
            } catch (Throwable e) {
                ;       // Just eat it
            }
        }

        // Flush the headers and finish this response
        sendHeaders();
        super.finishResponse();

    }


    /**
     * Return an array of all cookies set for this response, or
     * a zero-length array if no cookies have been set.
     */
    public Cookie[] getCookies() {

        synchronized (cookies) {
            return ((Cookie[]) cookies.toArray(new Cookie[cookies.size()]));
        }

    }


    /**
     * Return the value for the specified header, or <code>null</code> if this
     * header has not been set.  If more than one value was added for this
     * name, only the first is returned; use getHeaderValues() to retrieve all
     * of them.
     *
     * @param name Header name to look up
     */
    public String getHeader(String name) {

        ArrayList values = null;
        synchronized (headers) {
            values = (ArrayList) headers.get(name);
        }
        if (values != null)
            return ((String) values.get(0));
        else
            return (null);

    }


    /**
     * Return an array of all the header names set for this response, or
     * a zero-length array if no headers have been set.
     */
    public String[] getHeaderNames() {

        synchronized (headers) {
            String results[] = new String[headers.size()];
            return ((String[]) headers.keySet().toArray(results));
        }

    }


    /**
     * Return an array of all the header values associated with the
     * specified header name, or an zero-length array if there are no such
     * header values.
     *
     * @param name Header name to look up
     */
    public String[] getHeaderValues(String name) {

        ArrayList values = null;
        synchronized (headers) {
            values = (ArrayList) headers.get(name);
        }
        if (values == null)
            return (new String[0]);
        String results[] = new String[values.size()];
        return ((String[]) values.toArray(results));

    }


    /**
     * Return the error message that was set with <code>sendError()</code>
     * for this Response.
     */
    public String getMessage() {

        return (this.message);

    }


    /**
     * Return the HTTP status code associated with this Response.
     */
    public int getStatus() {

        return (this.status);

    }


    /**
     * Release all object references, and initialize instance variables, in
     * preparation for reuse of this object.
     */
    public void recycle() {

        super.recycle();
        cookies.clear();
        headers.clear();
        message = getStatusMessage(HttpServletResponse.SC_OK);
        status = HttpServletResponse.SC_OK;

    }


    /**
     * Reset this response, and specify the values for the HTTP status code
     * and corresponding message.
     *
     * @exception IllegalStateException if this response has already been
     *  committed
     */
    public void reset(int status, String message) {

        reset();
        setStatus(status, message);

    }


    // ------------------------------------------------------ Protected Methods


    /**
     * Returns a default status message for the specified HTTP status code.
     *
     * @param status The status code for which a message is desired
     */
    protected String getStatusMessage(int status) {

        switch (status) {
        case SC_OK:
            return ("OK");
        case SC_ACCEPTED:
            return ("Accepted");
        case SC_BAD_GATEWAY:
            return ("Bad Gateway");
        case SC_BAD_REQUEST:
            return ("Bad Request");
        case SC_CONFLICT:
            return ("Conflict");
        case SC_CONTINUE:
            return ("Continue");
        case SC_CREATED:
            return ("Created");
        case SC_EXPECTATION_FAILED:
            return ("Expectation Failed");
        case SC_FORBIDDEN:
            return ("Forbidden");
        case SC_GATEWAY_TIMEOUT:
            return ("Gateway Timeout");
        case SC_GONE:
            return ("Gone");
        case SC_HTTP_VERSION_NOT_SUPPORTED:
            return ("HTTP Version Not Supported");
        case SC_INTERNAL_SERVER_ERROR:
            return ("Internal Server Error");
        case SC_LENGTH_REQUIRED:
            return ("Length Required");
        case SC_METHOD_NOT_ALLOWED:
            return ("Method Not Allowed");
        case SC_MOVED_PERMANENTLY:
            return ("Moved Permanently");
        case SC_MOVED_TEMPORARILY:
            return ("Moved Temporarily");
        case SC_MULTIPLE_CHOICES:
            return ("Multiple Choices");
        case SC_NO_CONTENT:
            return ("No Content");
        case SC_NON_AUTHORITATIVE_INFORMATION:
            return ("Non-Authoritative Information");
        case SC_NOT_ACCEPTABLE:
            return ("Not Acceptable");
        case SC_NOT_FOUND:
            return ("Not Found");
        case SC_NOT_IMPLEMENTED:
            return ("Not Implemented");
        case SC_NOT_MODIFIED:
            return ("Not Modified");
        case SC_PARTIAL_CONTENT:
            return ("Partial Content");
        case SC_PAYMENT_REQUIRED:
            return ("Payment Required");
        case SC_PRECONDITION_FAILED:
            return ("Precondition Failed");
        case SC_PROXY_AUTHENTICATION_REQUIRED:
            return ("Proxy Authentication Required");
        case SC_REQUEST_ENTITY_TOO_LARGE:
            return ("Request Entity Too Large");
        case SC_REQUEST_TIMEOUT:
            return ("Request Timeout");
        case SC_REQUEST_URI_TOO_LONG:
            return ("Request URI Too Long");
        case SC_REQUESTED_RANGE_NOT_SATISFIABLE:
            return ("Requested Range Not Satisfiable");
        case SC_RESET_CONTENT:
            return ("Reset Content");
        case SC_SEE_OTHER:
            return ("See Other");
        case SC_SERVICE_UNAVAILABLE:
            return ("Service Unavailable");
        case SC_SWITCHING_PROTOCOLS:
            return ("Switching Protocols");
        case SC_UNAUTHORIZED:
            return ("Unauthorized");
        case SC_UNSUPPORTED_MEDIA_TYPE:
            return ("Unsupported Media Type");
        case SC_USE_PROXY:
            return ("Use Proxy");
        case 207:       // WebDAV
            return ("Multi-Status");
        case 422:       // WebDAV
            return ("Unprocessable Entity");
        case 423:       // WebDAV
            return ("Locked");
        case 507:       // WebDAV
            return ("Insufficient Storage");
        default:
            return ("HTTP Response Status " + status);
        }

    }


    /**
     * Return <code>true</code> if the specified URL should be encoded with
     * a session identifier.  This will be true if all of the following
     * conditions are met:
     * <ul>
     * <li>The request we are responding to asked for a valid session
     * <li>The requested session ID was not received via a cookie
     * <li>The specified URL points back to somewhere within the web
     *     application that is responding to this request
     * </ul>
     *
     * @param location Absolute URL to be validated
     **/
    private boolean isEncodeable(String location) {

        if (location == null)
            return (false);

        // Is this an intra-document reference?
        if (location.startsWith("#"))
            return (false);

        // Are we in a valid session that is not using cookies?
        HttpServletRequest hreq = (HttpServletRequest) request.getRequest();
        HttpSession session = hreq.getSession(false);
        if (session == null)
            return (false);
        if (hreq.isRequestedSessionIdFromCookie())
            return (false);

        // Is this a valid absolute URL?
        URL url = null;
        try {
            url = new URL(location);
        } catch (MalformedURLException e) {
            return (false);
        }

        // Does this URL match down to (and including) the context path?
        if (!hreq.getScheme().equalsIgnoreCase(url.getProtocol()))
            return (false);
        if (!hreq.getServerName().equalsIgnoreCase(url.getHost()))
            return (false);
        int serverPort = hreq.getServerPort();
        if (serverPort == -1) {
            if ("https".equals(hreq.getScheme()))
                serverPort = 443;
            else
                serverPort = 80;
        }
        int urlPort = url.getPort();
        if (urlPort == -1) {
            if ("https".equals(url.getProtocol()))
                urlPort = 443;
            else
                urlPort = 80;
        }
        if (serverPort != urlPort)
            return (false);

        String contextPath = getContext().getPath();
        if ((contextPath != null) && (contextPath.length() > 0)) {
            String file = url.getFile();
            if ((file == null) || !file.startsWith(contextPath))
                return (false);
        }

        // This URL belongs to our web application, so it is encodeable
        return (true);

    }


    private void log(String message) {
        Logger logger = context.getLogger();
        logger.log(message);
    }


    private void log(String message, Throwable throwable) {
        Logger logger = context.getLogger();
        logger.log(message, throwable);
    }

    /**
     * Return the HTTP protocol version implemented by this response
     * object. (This method must be overridden by subclasses of this
     * as to correctly return the highest HTTP version number supported
     * as specified in Section 3.1 of RFC-2616).
     *
     * @return A string in the form of &quot;HTTP/1.0&quot; ...
     */
    protected String getProtocol() {
        return(request.getRequest().getProtocol());
    }

    /**
     * Send the HTTP response headers, if this has not already occurred.
     */
    protected void sendHeaders() throws IOException {

        if (isCommitted())
            return;

        // Check if the request was an HTTP/0.9 request
        if ("HTTP/0.9".equals(request.getRequest().getProtocol())) {
            committed = true;
            return;
        }

        // Prepare a suitable output writer
        OutputStreamWriter osr =
            new OutputStreamWriter(getStream(), getCharacterEncoding());
        final PrintWriter outputWriter = new PrintWriter(osr);

        // Send the "Status:" header
        outputWriter.print(this.getProtocol());
        outputWriter.print(" ");
        outputWriter.print(status);
        if (message != null) {
            outputWriter.print(" ");
            outputWriter.print(message);
        }
        outputWriter.print("\r\n");
        // System.out.println("sendHeaders: " +
        //                    request.getRequest().getProtocol() +
        //                    " " + status + " " + message);

        // Send the content-length and content-type headers (if any)
        if (getContentType() != null) {
            outputWriter.print("Content-Type: " + getContentType() + "\r\n");
            // System.out.println(" Content-Type: " + getContentType());
        }
        if (getContentLength() >= 0) {
            outputWriter.print("Content-Length: " + getContentLength() +
                               "\r\n");
            // System.out.println(" Content-Length: " + getContentLength());
        }

        // Send all specified headers (if any)
        synchronized (headers) {
        Iterator names = headers.keySet().iterator();
        while (names.hasNext()) {
            String name = (String) names.next();
            ArrayList values = (ArrayList) headers.get(name);
            Iterator items = values.iterator();
            while (items.hasNext()) {
                String value = (String) items.next();
                    outputWriter.print(name);
                    outputWriter.print(": ");
                    outputWriter.print(value);
                    outputWriter.print("\r\n");
                    // System.out.println(" " + name + ": " + value);
                }
            }
        }

        // Add the session ID cookie if necessary
        HttpServletRequest hreq = (HttpServletRequest) request.getRequest();
        HttpSession session = hreq.getSession(false);

        if ((session != null) && session.isNew() && (getContext() != null)
            && getContext().getCookies()) {
            Cookie cookie = new Cookie(Globals.SESSION_COOKIE_NAME,
                                       session.getId());
            cookie.setMaxAge(-1);
            String contextPath = null;
            if (context != null)
                contextPath = context.getPath();
            if ((contextPath != null) && (contextPath.length() > 0))
                cookie.setPath(contextPath);
            else
                cookie.setPath("/");
            if (hreq.isSecure())
                cookie.setSecure(true);
            addCookie(cookie);
        }

        // Send all specified cookies (if any)
        synchronized (cookies) {
            Iterator items = cookies.iterator();
            while (items.hasNext()) {
                Cookie cookie = (Cookie) items.next();
                outputWriter.print(CookieTools.getCookieHeaderName(cookie));
                outputWriter.print(": ");
                outputWriter.print(CookieTools.getCookieHeaderValue(cookie));
                outputWriter.print("\r\n");
                //System.out.println(" " +
                //                   CookieTools.getCookieHeaderName(cookie) +
                //                   ": " +
                //                   CookieTools.getCookieHeaderValue(cookie));
            }
        }

        // Send a terminating blank line to mark the end of the headers
        outputWriter.print("\r\n");
        outputWriter.flush();
        // System.out.println("----------");

        // The response is now committed
        committed = true;

    }


    /**
     * Convert (if necessary) and return the absolute URL that represents the
     * resource referenced by this possibly relative URL.  If this URL is
     * already absolute, return it unchanged.
     *
     * @param location URL to be (possibly) converted and then returned
     *
     * @exception IllegalArgumentException if a MalformedURLException is
     *  thrown when converting the relative URL to an absolute one
     */
    private String toAbsolute(String location) {

        if (location == null)
            return (location);

        // Construct a new absolute URL if possible (cribbed from
        // the DefaultErrorPage servlet)
        URL url = null;
        try {
            url = new URL(location);
        } catch (MalformedURLException e1) {
            HttpServletRequest hreq =
                (HttpServletRequest) request.getRequest();
            String requrl = HttpUtils.getRequestURL(hreq).toString();
            try {
                url = new URL(new URL(requrl), location);
            } catch (MalformedURLException e2) {
                throw new IllegalArgumentException(location);
            }
        }
        return (url.toExternalForm());

    }


    /**
     * Return the specified URL with the specified session identifier
     * suitably encoded.
     *
     * @param url URL to be encoded with the session id
     * @param sessionId Session id to be included in the encoded URL
     */
    private String toEncoded(String url, String sessionId) {

        if ((url == null) || (sessionId == null))
            return (url);

        String path = url;
        String query = "";
        int question = url.indexOf('?');
        if (question >= 0) {
            path = url.substring(0, question);
            query = url.substring(question);
        }
        StringBuffer sb = new StringBuffer(path);
        sb.append(";jsessionid=");
        sb.append(sessionId);
        sb.append(query);
        return (sb.toString());

    }


    // ------------------------------------------------ ServletResponse Methods


    /**
     * Flush the buffer and commit this response.  If this is the first output,
     * send the HTTP headers prior to the user data.
     *
     * @exception IOException if an input/output error occurs
     */
    public void flushBuffer() throws IOException {

        if (!isCommitted())
            sendHeaders();

        super.flushBuffer();

    }


    /**
     * Clear any content written to the buffer.  In addition, all cookies
     * and headers are cleared, and the status is reset.
     *
     * @exception IllegalStateException if this response has already
     *  been committed
     */
    public void reset() {

        if (included)
            return;     // Ignore any call from an included servlet

        super.reset();
        cookies.clear();
        headers.clear();
        message = null;
        status = HttpServletResponse.SC_OK;

    }


    /**
     * Set the content length (in bytes) for this Response.
     *
     * @param length The new content length
     */
    public void setContentLength(int length) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        super.setContentLength(length);

    }



    /**
     * Set the content type for this Response.
     *
     * @param type The new content type
     */
    public void setContentType(String type) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        super.setContentType(type);

    }


    /**
     * Set the Locale that is appropriate for this response, including
     * setting the appropriate character encoding.
     *
     * @param locale The new locale
     */
    public void setLocale(Locale locale) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        super.setLocale(locale);
        String language = locale.getLanguage();
        if ((language != null) && (language.length() > 0)) {
            String country = locale.getCountry();
            StringBuffer value = new StringBuffer(language);
            if ((country != null) && (country.length() > 0)) {
                value.append('-');
                value.append(country);
            }
            setHeader("Content-Language", value.toString());
        }

    }


    // -------------------------------------------- HttpServletResponse Methods


    /**
     * Add the specified Cookie to those that will be included with
     * this Response.
     *
     * @param cookie Cookie to be added
     */
    public void addCookie(Cookie cookie) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        synchronized (cookies) {
            cookies.add(cookie);
        }

    }


    /**
     * Add the specified date header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Date value to be set
     */
    public void addDateHeader(String name, long value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        addHeader(name, format.format(new Date(value)));

    }


    /**
     * Add the specified header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Value to be set
     */
    public void addHeader(String name, String value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        synchronized (headers) {
            ArrayList values = (ArrayList) headers.get(name);
            if (values == null) {
                values = new ArrayList();
                headers.put(name, values);
            }
            values.add(value);
        }

    }


    /**
     * Add the specified integer header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Integer value to be set
     */
    public void addIntHeader(String name, int value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        addHeader(name, "" + value);

    }


    /**
     * Has the specified header been set already in this response?
     *
     * @param name Name of the header to check
     */
    public boolean containsHeader(String name) {

        synchronized (headers) {
            return (headers.get(name) != null);
        }

    }


    /**
     * Encode the session identifier associated with this response
     * into the specified redirect URL, if necessary.
     *
     * @param url URL to be encoded
     */
    public String encodeRedirectURL(String url) {

        if (isEncodeable(toAbsolute(url))) {
            HttpServletRequest hreq =
              (HttpServletRequest) request.getRequest();
            return (toEncoded(url, hreq.getSession().getId()));
        } else
            return (url);

    }


    /**
     * Encode the session identifier associated with this response
     * into the specified redirect URL, if necessary.
     *
     * @param url URL to be encoded
     *
     * @deprecated As of Version 2.1 of the Java Servlet API, use
     *  <code>encodeRedirectURL()</code> instead.
     */
    public String encodeRedirectUrl(String url) {

        return (encodeRedirectURL(url));

    }


    /**
     * Encode the session identifier associated with this response
     * into the specified URL, if necessary.
     *
     * @param url URL to be encoded
     */
    public String encodeURL(String url) {
        if (isEncodeable(toAbsolute(url))) {
            HttpServletRequest hreq =
              (HttpServletRequest) request.getRequest();
            return (toEncoded(url, hreq.getSession().getId()));
        } else
            return (url);

    }


    /**
     * Encode the session identifier associated with this response
     * into the specified URL, if necessary.
     *
     * @param url URL to be encoded
     *
     * @deprecated As of Version 2.1 of the Java Servlet API, use
     *  <code>encodeURL()</code> instead.
     */
    public String encodeUrl(String url) {

        return (encodeURL(url));

    }


    /**
     * Send an error response with the specified status and a
     * default message.
     *
     * @param status HTTP status code to send
     *
     * @exception IllegalStateException if this response has
     *  already been committed
     * @exception IOException if an input/output error occurs
     */
    public void sendError(int status) throws IOException {

        sendError(status, getStatusMessage(status));

    }


    /**
     * Send an error response with the specified status and message.
     *
     * @param status HTTP status code to send
     * @param message Corresponding message to send
     *
     * @exception IllegalStateException if this response has
     *  already been committed
     * @exception IOException if an input/output error occurs
     */
    public void sendError(int status, String message) throws IOException {

        if (isCommitted())
            throw new IllegalStateException
                (sm.getString("httpResponseBase.sendError.ise"));

        if (included)
            return;     // Ignore any call from an included servlet

        setError();

        // Record the status code and message.
        this.status = status;
        this.message = message;

        // Clear any data content that has been buffered
        resetBuffer();

        // Cause the response to be committed
        /* Per spec clarification, no default content type is set
        String contentType = getContentType();
        if ((contentType == null) || "text/plain".equals(contentType))
            setContentType("text/html");
        */

        // Temporarily comment out the following flush so that
        // default error reports can still set the content type
        // FIXME - this stuff needs to be refactored
        /*
        try {
            flushBuffer();
        } catch (IOException e) {
            ;
        }
        */

    }


    /**
     * Send a temporary redirect to the specified redirect location URL.
     *
     * @param location Location URL to redirect to
     *
     * @exception IllegalStateException if this response has
     *  already been committed
     * @exception IOException if an input/output error occurs
     */
    public void sendRedirect(String location) throws IOException {

        if (isCommitted())
            throw new IllegalStateException
                (sm.getString("httpResponseBase.sendRedirect.ise"));

        if (included)
            return;     // Ignore any call from an included servlet

        // Clear any data content that has been buffered
        resetBuffer();

        // Generate a temporary redirect to the specified location
        String absolute = toAbsolute(location);
        setStatus(SC_MOVED_TEMPORARILY);
        setHeader("Location", absolute);

    }


    /**
     * Set the specified date header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Date value to be set
     */
    public void setDateHeader(String name, long value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        setHeader(name, format.format(new Date(value)));

    }


    /**
     * Set the specified header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Value to be set
     */
    public void setHeader(String name, String value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        ArrayList values = new ArrayList();
        values.add(value);
        synchronized (headers) {
            headers.put(name, values);
        }

        String match = name.toLowerCase();
        if (match.equals("content-length")) {
            int contentLength = -1;
            try {
                contentLength = Integer.parseInt(value);
            } catch (NumberFormatException e) {
                ;
            }
            if (contentLength >= 0)
                setContentLength(contentLength);
        } else if (match.equals("content-type")) {
            setContentType(value);
        }

    }


    /**
     * Set the specified integer header to the specified value.
     *
     * @param name Name of the header to set
     * @param value Integer value to be set
     */
    public void setIntHeader(String name, int value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        setHeader(name, "" + value);

    }


    /**
     * Set the HTTP status to be returned with this response.
     *
     * @param status The new HTTP status
     */
    public void setStatus(int status) {

        setStatus(status, getStatusMessage(status));

    }


    /**
     * Set the HTTP status and message to be returned with this response.
     *
     * @param status The new HTTP status
     * @param message The associated text message
     *
     * @deprecated As of Version 2.1 of the Java Servlet API, this method
     *  has been deprecated due to the ambiguous meaning of the message
     *  parameter.
     */
    public void setStatus(int status, String message) {

        if (included)
            return;     // Ignore any call from an included servlet

        this.status = status;
        this.message = message;

    }


}

