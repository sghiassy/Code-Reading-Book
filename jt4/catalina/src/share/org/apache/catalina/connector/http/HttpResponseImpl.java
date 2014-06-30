/*
 * $Header: /home/cvspublic/jakarta-tomcat-4.0/catalina/src/share/org/apache/catalina/connector/http/HttpResponseImpl.java,v 1.11 2001/08/08 19:26:07 pier Exp $
 * $Revision: 1.11 $
 * $Date: 2001/08/08 19:26:07 $
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


package org.apache.catalina.connector.http;


import java.io.IOException;
import java.io.PrintWriter;
import java.io.OutputStream;
import java.util.ArrayList;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletResponse;
import org.apache.catalina.connector.HttpResponseBase;


/**
 * Implementation of <b>HttpResponse</b> specific to the HTTP connector.
 *
 * @author Craig R. McClanahan
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @version $Revision: 1.11 $ $Date: 2001/08/08 19:26:07 $
 */

final class HttpResponseImpl
    extends HttpResponseBase {


    // ----------------------------------------------------- Instance Variables


    /**
     * Descriptive information about this Response implementation.
     */
    protected static final String info =
        "org.apache.catalina.connector.http.HttpResponseImpl/1.0";


    /**
     * True if chunking is allowed.
     */
    protected boolean allowChunking;


    /**
     * Associated HTTP response stream.
     */
    protected HttpResponseStream responseStream;


    // ------------------------------------------------------------- Properties


    /**
     * Return descriptive information about this Response implementation and
     * the corresponding version number, in the format
     * <code>&lt;description&gt;/&lt;version&gt;</code>.
     */
    public String getInfo() {

        return (info);

    }


    /**
     * Set the chunking flag.
     */
    void setAllowChunking(boolean allowChunking) {
        this.allowChunking = allowChunking;
    }


    /**
     * True if chunking is allowed.
     */
    public boolean isChunkingAllowed() {
        return allowChunking;
    }


    // ------------------------------------------------------ Protected Methods

    /**
     * Return the HTTP protocol version implemented by this response
     * object.
     *
     * @return The &quot;HTTP/1.1&quot; string.
     */
    protected String getProtocol() {
        return("HTTP/1.1");
    }


    // --------------------------------------------------------- Public Methods

    /**
     * Release all object references, and initialize instance variables, in
     * preparation for reuse of this object.
     */
    public void recycle() {

        super.recycle();
        responseStream = null;
        allowChunking = false;

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

        setAllowChunking(false);
        addHeader("Connection", "close");
        super.sendError(status, message);

    }


    /**
     * Clear any content written to the buffer.  In addition, all cookies
     * and headers are cleared, and the status is reset.
     *
     * @exception IllegalStateException if this response has already
     *  been committed
     */
    public void reset() {

        // Saving important HTTP/1.1 specific headers
        String connectionValue =
            (String) getHeader("Connection");
        String transferEncodingValue =
            (String) getHeader("Transfer-Encoding");
        super.reset();
        if (connectionValue != null)
            addHeader("Connection", connectionValue);
        if (transferEncodingValue != null)
            addHeader("Transfer-Encoding", transferEncodingValue);

    }


    /**
     * Create and return a ServletOutputStream to write the content
     * associated with this Response.
     *
     * @exception IOException if an input/output error occurs
     */
    public ServletOutputStream createOutputStream() throws IOException {

        responseStream = new HttpResponseStream(this);
        return (responseStream);

    }


    /**
     * Tests is the connection will be closed after the processing of the
     * request.
     */
    public boolean isCloseConnection() {
        String connectionValue = (String) getHeader("Connection");
        return (connectionValue != null
                && connectionValue.equals("close"));
    }


    /**
     * Removes the specified header.
     *
     * @param name Name of the header to remove
     * @param value Value to remove
     */
    public void removeHeader(String name, String value) {

        if (isCommitted())
            return;

        if (included)
            return;     // Ignore any call from an included servlet

        synchronized (headers) {
            ArrayList values = (ArrayList) headers.get(name);
            if ((values != null) && (!values.isEmpty())) {
                values.remove(value);
                if (values.isEmpty())
                    headers.remove(name);
            }
        }

    }


    /**
     * Has stream been created ?
     */
    public boolean isStreamInitialized() {
        return (responseStream != null);
    }


    /**
     * Perform whatever actions are required to flush and close the output
     * stream or writer, in a single operation.
     *
     * @exception IOException if an input/output error occurs
     */
    public void finishResponse() throws IOException {

        if (getStatus() < HttpServletResponse.SC_BAD_REQUEST) {
            if ((!isStreamInitialized()) && (getContentLength() == -1)
                && (getStatus() >= 200)
                && (getStatus() != SC_NOT_MODIFIED)
                && (getStatus() != SC_NO_CONTENT))
                setContentLength(0);
        } else {
            setHeader("Connection", "close");
        }
        super.finishResponse();

    }


    // -------------------------------------------- HttpServletResponse Methods


    /**
     * Set the HTTP status to be returned with this response.
     *
     * @param status The new HTTP status
     */
    public void setStatus(int status) {

        super.setStatus(status);

        if (responseStream != null)
            responseStream.checkChunking(this);

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

        if (responseStream != null)
            responseStream.checkChunking(this);

    }


}
